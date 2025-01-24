#include "matrix_operations.h"
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>

Matrix multiplySerial(const Matrix& A, const Matrix& B) {
    Matrix C(A.rows, B.cols);

    for (size_t i = 0; i < A.rows; ++i) {
        for (size_t j = 0; j < B.cols; ++j) {
            double sum = 0.0;
            for (size_t k = 0; k < A.cols; ++k) {
                sum += A(i, k) * B(k, j);
            }
            C(i, j) = sum;
        }
    }

    return C;
}

void multiplyRowsStatic(const Matrix& A, const Matrix& B, Matrix& C,
                       size_t start_row, size_t end_row) {
    for (size_t i = start_row; i < end_row; ++i) {
        for (size_t j = 0; j < B.cols; ++j) {
            double sum = 0.0;
            for (size_t k = 0; k < A.cols; ++k) {
                sum += A(i, k) * B(k, j);
            }
            C(i, j) = sum;
        }
    }
}

Matrix multiplyStaticRows(const Matrix& A, const Matrix& B, int numThreads) {
    Matrix C(A.rows, B.cols);
    std::vector<std::thread> threads;

    size_t rows_per_thread = A.rows / numThreads;
    size_t extra_rows = A.rows % numThreads;
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

void multiplyCyclicRows(const Matrix& A, const Matrix& B, Matrix& C,
                       int thread_id, int num_threads) {
    for (size_t i = thread_id; i < A.rows; i += num_threads) {
        for (size_t j = 0; j < B.cols; ++j) {
            double sum = 0.0;
            for (size_t k = 0; k < A.cols; ++k) {
                sum += A(i, k) * B(k, j);
            }
            C(i, j) = sum;
        }
    }
}

Matrix multiplyCyclic(const Matrix& A, const Matrix& B, int numThreads) {
    Matrix C(A.rows, B.cols);
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

void multiplyDynamicRows(const Matrix& A, const Matrix& B, Matrix& C,
                        std::atomic<size_t>& next_row) {
    size_t row;
    while ((row = next_row.fetch_add(1)) < A.rows) {
        for (size_t j = 0; j < B.cols; ++j) {
            double sum = 0.0;
            for (size_t k = 0; k < A.cols; ++k) {
                sum += A(row, k) * B(k, j);
            }
            C(row, j) = sum;
        }
    }
}

Matrix multiplyDynamic(const Matrix& A, const Matrix& B, int numThreads) {
    Matrix C(A.rows, B.cols);
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