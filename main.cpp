#include "matrix_operations.h"
#include <iostream>
#include <chrono>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

// Helper function to measure execution time
template<typename Func>
double measureExecutionTime(Func &&func) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

// Function to run benchmarks for a specific matrix size
void runBenchmark(size_t size, const std::vector<int> &threadCounts) {
    std::cout << "\nRunning benchmark for " << size << "x" << size << " matrices\n";
    std::cout << "================================================\n";

    // Create directory for matrix files if it doesn't exist
    fs::create_directories("matrices");
    fs::create_directories("results");

    // File names for this size
    std::string matrixA_file = "matrices/matrix_A_" + std::to_string(size) + ".bin";
    std::string matrixB_file = "matrices/matrix_B_" + std::to_string(size) + ".bin";

    // Ensure matrices exist
    ensureMatrixFiles(size, matrixA_file, matrixB_file);

    // Load matrices
    Matrix A, B;
    if (!loadMatrixFromFile(A, matrixA_file, size, size) ||
        !loadMatrixFromFile(B, matrixB_file, size, size)) {
        std::cerr << "Failed to load matrices for size " << size << "\n";
        return;
    }

    std::vector<PerformanceMetrics> metrics;

    // Run serial implementation once
    std::cout << "\nRunning serial implementation...\n";
    Matrix serialResult;
    double serialTime = measureExecutionTime([&]() {
        serialResult = multiplySerial(A, B);
    });

    metrics.push_back({static_cast<int>(size), 1, "serial", serialTime});
    std::cout << "Serial execution time: " << serialTime << " ms\n";

    // Run parallel implementations for each thread count
    for (int threadCount: threadCounts) {
        std::cout << "\nTesting with " << threadCount << " threads:\n";

        // Static rows implementation
        double staticTime = measureExecutionTime([&]() {
            Matrix result = multiplyStaticRows(A, B, threadCount);
            validateResults(serialResult, result);
        });
        metrics.push_back({static_cast<int>(size), threadCount, "static_rows", staticTime});
        std::cout << "Static rows time: " << staticTime << " ms\n";

        // Cyclic implementation
        double cyclicTime = measureExecutionTime([&]() {
            Matrix result = multiplyCyclic(A, B, threadCount);
            validateResults(serialResult, result);
        });
        metrics.push_back({static_cast<int>(size), threadCount, "cyclic", cyclicTime});
        std::cout << "Cyclic time: " << cyclicTime << " ms\n";

        // Dynamic implementation
        double dynamicTime = measureExecutionTime([&]() {
            Matrix result = multiplyDynamic(A, B, threadCount);
            validateResults(serialResult, result);
        });
        metrics.push_back({static_cast<int>(size), threadCount, "dynamic", dynamicTime});
        std::cout << "Dynamic time: " << dynamicTime << " ms\n";
    }

    // Log the results
    std::string logFile = "results/performance_" + std::to_string(size) + ".csv";
    if (!fs::exists(logFile)) {
        // Create header if file doesn't exist
        std::ofstream file(logFile);
        file << "matrix_size,num_threads,approach,execution_time\n";
    }
    logPerformance(metrics, logFile);
}

int main() {
    // Matrix sizes to test
    std::vector<size_t> sizes = {1024, 2048, 4096, 8192};

    // Thread counts to test
    const std::vector threadCounts = {2, 4, 8, 16, 32, 64};

    std::cout << "Matrix Multiplication Benchmark\n";
    std::cout << "==============================\n";

    // Run benchmarks for each matrix size
    for (size_t size: sizes) {
        try {
            runBenchmark(size, threadCounts);
        } catch (const std::exception &e) {
            std::cerr << "Error running benchmark for size " << size << ": " << e.what() << "\n";
        }
    }

    return 0;
}
