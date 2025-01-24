cmake_minimum_required(VERSION 3.30)
project(matmul)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_executable(matmul main.cpp
        matrix_operations.cpp
        matrix_multiply.cpp)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()