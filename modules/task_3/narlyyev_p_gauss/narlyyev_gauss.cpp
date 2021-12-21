// Copyright 2021 Narlyyev Parahat
#include <mpi.h>
#include <vector>
#include <random>
#include <ctime>
#include <algorithm>
#include <cstring>
#include <stdexcept>
#include "../../../modules/task_3/narlyyev_p_gauss/narlyyev_gauss.h"

std::vector<double> getRandomImage(size_t rows, size_t cols) {
    std::mt19937 gen;
    gen.seed(static_cast<unsigned int>(time(0)));
    std::vector<double> image(cols * rows);
    for (size_t i = 0; i < cols * rows; i++) {
        int color = gen() % 256;
        image[i] = color / 255.0;
    }
    return image;
}

std::vector<double> getSequentialOperations(std::vector<double> image, size_t rows, size_t cols) {
    std::vector<double> result(rows * cols);

    for (size_t y = 0; y < rows; y++)
        for (size_t x = 0; x < cols; x++) {
            double color = 0;
            for (int i = -gaussianRadius; i < gaussianRadius; i++)
                for (int j = -gaussianRadius; j < gaussianRadius; j++) {
                    size_t x_index = x + j, y_index = y + i;
                    if (x_index < cols && y_index < rows) {
                        color += image[y_index * cols + x_index] * gaussianKernel[(i + 1) * gaussianSize + (j + 1)];
                    }
                }
            result[y * cols + x] = std::max(0.0, std::min(color / gaussianNorm, 1.0));
        }
    return result;
}

std::vector<double> getParallelOperations(std::vector<double> global_image, size_t rows, size_t cols) {
    int size, rank;

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int error_code = 0;
    if (rank == 0 && global_image.size() == 0) {
        error_code = 1;
    } else if (rank == 0 && global_image.size() != rows * cols) {
        error_code = 2;
    }
    MPI_Bcast(&error_code, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (error_code == 1) {
        throw std::runtime_error("Image is empty");
    } else if (error_code == 2) {
        throw std::runtime_error("Invalid parameters");
    }

    const size_t delta = (rows / size) * cols;
    const size_t epsilon = (rows % size) * cols;

    int dataSize;
    if (rank == 0) {
        dataSize = delta + epsilon + cols;
    } else if (rank == size - 1) {
        dataSize = delta + cols;
    } else {
        dataSize = delta + 2 * cols;
    }

    std::vector<double> local_part_image(dataSize);

    if (rank == 0) {
        std::memcpy(local_part_image.data(), global_image.data(), (delta + epsilon + cols) * sizeof(double));
        for (int proc = 1; proc < size - 1; proc++) {
            MPI_Send(global_image.data() + epsilon + proc * delta - cols, delta + 2 * cols,
                MPI_DOUBLE, proc, 0, MPI_COMM_WORLD);
        }
        MPI_Send(global_image.data() + epsilon + (static_cast<size_t>(size) - 1) * delta - cols, delta + cols,
                MPI_DOUBLE, size - 1, 0, MPI_COMM_WORLD);
    } else if (rank == size - 1) {
        MPI_Status status;
        MPI_Recv(local_part_image.data(), delta + cols, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
    } else {
        MPI_Status status;
        MPI_Recv(local_part_image.data(), delta + 2 * cols, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
    }

    std::vector<double> local_result = getSequentialOperations(local_part_image,
        rank == 0 ? (delta + epsilon) / cols : delta / cols + 1, cols);
    std::vector<double> result(rows * cols);

    if (rank == 0) {
        std::memcpy(result.data(), local_result.data(), (delta + epsilon) * sizeof(double));
        for (int proc = 1; proc < size; proc++) {
            MPI_Status status;
            MPI_Recv(result.data() + proc *delta + epsilon, delta, MPI_DOUBLE, proc, 0, MPI_COMM_WORLD, &status);
        }
    } else {
        MPI_Send(local_result.data() + cols, delta, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }

    return result;
}
