//
// Created by vrin on 1/23/25.
//

#include "matrix_operations.h"
#include <thread>
#include <atomic>
#include <vector>

// Serial implementation
Matrix multiplySerial(const Matrix& A, const Matrix& B) {
    size_t m = A.size();
    size_t n = A[0].size();
    size_t p = B[0].size();

    Matrix C(m, std::vector(p, 0.0));

    for (size_t i = 0; i < m; ++i) {
        for (size_t j = 0; j < p; ++j) {
            for (size_t k = 0; k < n; ++k) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    return C;
}

// Static row-wise implementation
void multiplyRowsStatic(const Matrix& A, const Matrix& B, Matrix& C,
                       size_t start_row, size_t end_row) {
    size_t n = A[0].size();
    size_t p = B[0].size();

    for (size_t i = start_row; i < end_row; ++i) {
        for (size_t j = 0; j < p; ++j) {
            C[i][j] = 0.0;
            for (size_t k = 0; k < n; ++k) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

Matrix multiplyStaticRows(const Matrix& A, const Matrix& B, int numThreads) {
    size_t m = A.size();
    size_t p = B[0].size();

    Matrix C(m, std::vector(p, 0.0));
    std::vector<std::thread> threads;

    size_t rows_per_thread = m / numThreads;
    size_t extra_rows = m % numThreads;
    size_t current_row = 0;

    for (int i = 0; i < numThreads; ++i) {
        size_t thread_rows = rows_per_thread + (i < extra_rows ? 1 : 0);
        size_t end_row = current_row + thread_rows;

        threads.emplace_back(multiplyRowsStatic,
                           std::ref(A), std::ref(B), std::ref(C),
                           current_row, end_row);

        current_row = end_row;
    }

    for (auto& thread : threads) {
        thread.join();
    }

    return C;
}

// Cyclic implementation
void multiplyCyclicRows(const Matrix& A, const Matrix& B, Matrix& C,
                       int thread_id, int num_threads) {
    size_t m = A.size();
    size_t n = A[0].size();
    size_t p = B[0].size();

    for (size_t i = thread_id; i < m; i += num_threads) {
        for (size_t j = 0; j < p; ++j) {
            C[i][j] = 0.0;
            for (size_t k = 0; k < n; ++k) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

Matrix multiplyCyclic(const Matrix& A, const Matrix& B, int numThreads) {
    size_t m = A.size();
    size_t p = B[0].size();

    Matrix C(m, std::vector(p, 0.0));
    std::vector<std::thread> threads;

    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(multiplyCyclicRows,
                           std::ref(A), std::ref(B), std::ref(C),
                           i, numThreads);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    return C;
}

// Dynamic implementation
void multiplyDynamicRows(const Matrix& A, const Matrix& B, Matrix& C,
                        std::atomic<size_t>& next_row) {
    size_t m = A.size();
    size_t n = A[0].size();
    size_t p = B[0].size();

    size_t row;
    while ((row = next_row.fetch_add(1)) < m) {
        for (size_t j = 0; j < p; ++j) {
            C[row][j] = 0.0;
            for (size_t k = 0; k < n; ++k) {
                C[row][j] += A[row][k] * B[k][j];
            }
        }
    }
}

Matrix multiplyDynamic(const Matrix& A, const Matrix& B, int numThreads) {
    size_t m = A.size();
    size_t p = B[0].size();

    Matrix C(m, std::vector(p, 0.0));
    std::vector<std::thread> threads;
    std::atomic<size_t> next_row(0);

    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(multiplyDynamicRows,
                           std::ref(A), std::ref(B), std::ref(C),
                           std::ref(next_row));
    }

    for (auto& thread : threads) {
        thread.join();
    }

    return C;
}