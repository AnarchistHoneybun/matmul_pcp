cmake_minimum_required(VERSION 3.30)
project(matmul)

set(CMAKE_CXX_STANDARD 20)

add_executable(matmul main.cpp
        matrix_operations.cpp
        matrix_multiply.cpp)
