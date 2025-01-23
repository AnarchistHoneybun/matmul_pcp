//
// Created by vrin on 1/23/25.
//

#include "matrix_operations.h"
#include <random>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <filesystem>

Matrix generateRandomMatrix(size_t rows, size_t cols) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(1.0, 100.0);

    Matrix matrix(rows, std::vector<double>(cols));
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            matrix[i][j] = dis(gen);
        }
    }
    return matrix;
}

bool saveMatrixToFile(const Matrix& matrix, const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error: Unable to open file " << filename << " for writing\n";
        return false;
    }

    size_t rows = matrix.size();
    size_t cols = matrix[0].size();

    // Write dimensions
    file.write(reinterpret_cast<const char*>(&rows), sizeof(rows));
    file.write(reinterpret_cast<const char*>(&cols), sizeof(cols));

    // Write data
    for (const auto& row : matrix) {
        file.write(reinterpret_cast<const char*>(row.data()), cols * sizeof(double));
    }

    return true;
}

bool loadMatrixFromFile(Matrix& matrix, const std::string& filename, size_t expected_rows, size_t expected_cols) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        return false;
    }

    size_t rows, cols;
    file.read(reinterpret_cast<char*>(&rows), sizeof(rows));
    file.read(reinterpret_cast<char*>(&cols), sizeof(cols));

    if (rows != expected_rows || cols != expected_cols) {
        std::cerr << "Error: Matrix dimensions in file don't match expected dimensions\n";
        return false;
    }

    matrix.resize(rows, std::vector<double>(cols));
    for (auto& row : matrix) {
        file.read(reinterpret_cast<char*>(row.data()), cols * sizeof(double));
    }

    return true;
}

void ensureMatrixFiles(size_t size, const std::string& fileA, const std::string& fileB) {
    std::cout << "Checking for matrix files of size " << size << "x" << size << "...\n";

    bool generateA = true;
    bool generateB = true;

    // Try to load existing matrices
    Matrix A, B;
    if (std::filesystem::exists(fileA)) {
        if (loadMatrixFromFile(A, fileA, size, size)) {
            std::cout << "Successfully loaded matrix A from " << fileA << "\n";
            generateA = false;
        }
    }

    if (std::filesystem::exists(fileB)) {
        if (loadMatrixFromFile(B, fileB, size, size)) {
            std::cout << "Successfully loaded matrix B from " << fileB << "\n";
            generateB = false;
        }
    }

    // Generate and save matrices if needed
    if (generateA) {
        std::cout << "Generating matrix A...\n";
        A = generateRandomMatrix(size, size);
        if (saveMatrixToFile(A, fileA)) {
            std::cout << "Saved matrix A to " << fileA << "\n";
        }
    }

    if (generateB) {
        std::cout << "Generating matrix B...\n";
        B = generateRandomMatrix(size, size);
        if (saveMatrixToFile(B, fileB)) {
            std::cout << "Saved matrix B to " << fileB << "\n";
        }
    }
}

void logPerformance(const std::vector<PerformanceMetrics>& metrics, const std::string& filename) {
    std::ofstream file(filename, std::ios::app);
    if (!file) {
        std::cerr << "Error: Unable to open performance log file\n";
        return;
    }

    for (const auto& metric : metrics) {
        file << metric.matrixSize << ","
             << metric.numThreads << ","
             << metric.approach << ","
             << std::fixed << std::setprecision(6) << metric.executionTime << "\n";
    }
}

void printMatrix(const Matrix& matrix, const std::string& name) {
    std::cout << "\nMatrix " << name << " (" << matrix.size() << "x" << matrix[0].size() << "):\n";
    if (matrix.size() <= 10) {  // Only print if matrix is small
        for (const auto& row : matrix) {
            for (double val : row) {
                std::cout << std::setw(8) << std::fixed << std::setprecision(2) << val << " ";
            }
            std::cout << "\n";
        }
    } else {
        std::cout << "(Matrix too large to display)\n";
    }
}

bool compareMatrices(const Matrix& A, const Matrix& B, double tolerance) {
    if (A.size() != B.size() || A[0].size() != B[0].size()) {
        return false;
    }

    for (size_t i = 0; i < A.size(); ++i) {
        for (size_t j = 0; j < A[0].size(); ++j) {
            if (std::abs(A[i][j] - B[i][j]) > tolerance) {
                return false;
            }
        }
    }
    return true;
}

void validateResults(const Matrix& serial, const Matrix& parallel) {
    if (compareMatrices(serial, parallel)) {
        std::cout << "Validation passed: Results match serial implementation\n";
    } else {
        std::cout << "Validation failed: Results do not match serial implementation\n";
    }
}