// Copyright 2021 Narlyyev Parahat
#include <gtest/gtest.h>
#include <vector>
#include <gtest-mpi-listener.hpp>
#include "./narlyyev_gauss.h"

TEST(Parallel_Operations_MPI, Image_10x1) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int rows = 8;
    int cols = 1;
    std::vector<double> Image;
    std::vector<double> control_result;
    if (rank == 0) {
        Image = getRandomImage(rows, cols);
        std::cout << "Image has been generated." << std::endl;
        double seq_start = MPI_Wtime();
        control_result = getSequentialOperations(Image, rows, cols);
        double end = MPI_Wtime();
        std::cout << "Sequential implementation: " << end - seq_start << "s\n";
    }
    double parallel_start = MPI_Wtime();
    std::vector<double> result = getParallelOperations(Image, rows, cols);
    if (rank == 0) {
        double end = MPI_Wtime();
        std::cout << "Parallel implementation: " << end - parallel_start << "s\n";
        ASSERT_EQ(control_result, result);
    }
}

TEST(Parallel_Operations_MPI, Image_1x10) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int rows = 1;
    int cols = 8;
    std::vector<double> Image;
    if (rank == 0) {
        Image = getRandomImage(rows, cols);
    }
    double parallel_start = MPI_Wtime();
    std::vector<double> result = getParallelOperations(Image, rows, cols);
    if (rank == 0) {
        double end = MPI_Wtime();
        std::cout << "Parallel implementation: " << end - parallel_start << "s\n";
        double seq_start = MPI_Wtime();
        std::vector<double> control_result = getSequentialOperations(Image, rows, cols);
        end = MPI_Wtime();
        std::cout << "Sequential implementation: " << end - seq_start << "s\n";
        ASSERT_EQ(control_result, result);
    }
}

TEST(Parallel_Operations_MPI, Image_13x10) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int rows = 5;
    int cols = 5;
    std::vector<double> Image;
    if (rank == 0) {
        Image = getRandomImage(rows, cols);
    }
    double parallel_start = MPI_Wtime();
    std::vector<double> result = getParallelOperations(Image, rows, cols);
    if (rank == 0) {
        double end = MPI_Wtime();
        std::cout << "Parallel implementation: " << end - parallel_start << "s\n";
        double seq_start = MPI_Wtime();
        std::vector<double> control_result = getSequentialOperations(Image, rows, cols);
        end = MPI_Wtime();
        std::cout << "Sequential implementation: " << end - seq_start << "s\n";
        ASSERT_EQ(control_result, result);
    }
}

TEST(Parallel_Operations_MPI, Image_10x10) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int rows = 5;
    int cols = 5;
    std::vector<double> Image;
    if (rank == 0) {
        Image = getRandomImage(rows, cols);
    }
    double parallel_start = MPI_Wtime();
    std::vector<double> result = getParallelOperations(Image, rows, cols);
    if (rank == 0) {
        double end = MPI_Wtime();
        std::cout << "Parallel implementation: " << end - parallel_start << "s\n";
        double seq_start = MPI_Wtime();
        std::vector<double> control_result = getSequentialOperations(Image, rows, cols);
        end = MPI_Wtime();
        std::cout << "Sequential implementation: " << end - seq_start << "s\n";
        ASSERT_EQ(control_result, result);
    }
}

TEST(Parallel_Operations_MPI, Image_12x12) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int rows = 10;
    int cols = 10;
    std::vector<double> Image;
    if (rank == 0) {
        Image = getRandomImage(rows, cols);
    }
    double parallel_start = MPI_Wtime();
    std::vector<double> result = getParallelOperations(Image, rows, cols);
    if (rank == 0) {
        double end = MPI_Wtime();
        std::cout << "Parallel implementation: " << end - parallel_start << "s\n";
        double seq_start = MPI_Wtime();
        std::vector<double> control_result = getSequentialOperations(Image, rows, cols);
        end = MPI_Wtime();
        std::cout << "Sequential implementation: " << end - seq_start << "s\n";
        ASSERT_EQ(control_result, result);
    }
}

TEST(Parallel_Operations_MPI, Empty_Image) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::vector<double> Image;
    int rows = 1;
    int cols = 1;

    ASSERT_ANY_THROW(getParallelOperations(Image, rows, cols));
}

TEST(Parallel_Operations_MPI, Invalid_Param) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::vector<double> Image;
    int rows = 1;
    int cols = 1;
    if (rank == 0) {
        Image = getRandomImage(rows, cols);
    }

    ASSERT_ANY_THROW(getParallelOperations(Image, rows - 1, cols - 1));
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    MPI_Init(&argc, &argv);

    ::testing::AddGlobalTestEnvironment(new GTestMPIListener::MPIEnvironment);
    ::testing::TestEventListeners& listeners =
        ::testing::UnitTest::GetInstance()->listeners();

    listeners.Release(listeners.default_result_printer());
    listeners.Release(listeners.default_xml_generator());

    listeners.Append(new GTestMPIListener::MPIMinimalistPrinter);
    return RUN_ALL_TESTS();
}
