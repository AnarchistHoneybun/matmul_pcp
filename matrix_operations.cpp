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

    Matrix matrix(rows, cols);
    for (size_t i = 0; i < rows * cols; ++i) {
        matrix.data[i] = dis(gen);
    }
    return matrix;
}

bool saveMatrixToFile(const Matrix& matrix, const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error: Unable to open file " << filename << " for writing\n";
        return false;
    }

    file.write(reinterpret_cast<const char*>(&matrix.rows), sizeof(matrix.rows));
    file.write(reinterpret_cast<const char*>(&matrix.cols), sizeof(matrix.cols));
    file.write(reinterpret_cast<const char*>(matrix.data.data()), matrix.data.size() * sizeof(double));

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

    matrix.resize(rows, cols);
    file.read(reinterpret_cast<char*>(matrix.data.data()), matrix.data.size() * sizeof(double));

    return true;
}

void ensureMatrixFiles(size_t size, const std::string& fileA, const std::string& fileB) {
    std::cout << "Checking for matrix files of size " << size << "x" << size << "...\n";

    Matrix A, B;
    bool generateA = !std::filesystem::exists(fileA) ||
                    !loadMatrixFromFile(A, fileA, size, size);
    bool generateB = !std::filesystem::exists(fileB) ||
                    !loadMatrixFromFile(B, fileB, size, size);

    if (generateA) {
        std::cout << "Generating matrix A...\n";
        A = generateRandomMatrix(size, size);
        saveMatrixToFile(A, fileA);
    }

    if (generateB) {
        std::cout << "Generating matrix B...\n";
        B = generateRandomMatrix(size, size);
        saveMatrixToFile(B, fileB);
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
    std::cout << "\nMatrix " << name << " (" << matrix.rows << "x" << matrix.cols << "):\n";
    if (matrix.rows <= 10) {
        for (size_t i = 0; i < matrix.rows; ++i) {
            for (size_t j = 0; j < matrix.cols; ++j) {
                std::cout << std::setw(8) << std::fixed << std::setprecision(2) << matrix(i, j) << " ";
            }
            std::cout << "\n";
        }
    } else {
        std::cout << "(Matrix too large to display)\n";
    }
}

bool compareMatrices(const Matrix& A, const Matrix& B, double tolerance) {
    if (A.rows != B.rows || A.cols != B.cols) {
        return false;
    }

    for (size_t i = 0; i < A.data.size(); ++i) {
        if (std::abs(A.data[i] - B.data[i]) > tolerance) {
            return false;
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