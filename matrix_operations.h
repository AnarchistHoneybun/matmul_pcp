#ifndef MATRIX_OPERATIONS_H
#define MATRIX_OPERATIONS_H

#include <vector>
#include <string>
#include <chrono>

using Matrix = std::vector<std::vector<double>>;

// Matrix generation and file operations
Matrix generateRandomMatrix(size_t rows, size_t cols);
bool saveMatrixToFile(const Matrix& matrix, const std::string& filename);
bool loadMatrixFromFile(Matrix& matrix, const std::string& filename, size_t rows, size_t cols);
void ensureMatrixFiles(size_t size, const std::string& fileA, const std::string& fileB);

// Matrix multiplication implementations
Matrix multiplySerial(const Matrix& A, const Matrix& B);
Matrix multiplyStaticRows(const Matrix& A, const Matrix& B, int numThreads);
Matrix multiplyCyclic(const Matrix& A, const Matrix& B, int numThreads);
Matrix multiplyDynamic(const Matrix& A, const Matrix& B, int numThreads);

// Performance measurement structure
struct PerformanceMetrics {
    int matrixSize;
    int numThreads;
    std::string approach;
    double executionTime;  // in milliseconds
};

// Performance logging
void logPerformance(const std::vector<PerformanceMetrics>& metrics, const std::string& filename);
void validateResults(const Matrix& serial, const Matrix& parallel);

// Helper functions
void printMatrix(const Matrix& matrix, const std::string& name);
bool compareMatrices(const Matrix& A, const Matrix& B, double tolerance = 1e-10);

#endif // MATRIX_OPERATIONS_H