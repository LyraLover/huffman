cmake_minimum_required(VERSION 3.17)
project(huffman)

set(CMAKE_CXX_STANDARD 20)

add_executable(huffman main.cpp compress/compress.cpp decompress/decompress.cpp)
