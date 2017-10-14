cmake_minimum_required(VERSION 3.8)
project(Wav)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES main.cpp WavHeader.h WavFile.cpp WavFile.hpp Exceptions/IOException.hpp Exceptions/BadFormatException.hpp Exceptions/HeaderException.hpp Exceptions/UnsupportedFormatException.hpp)
add_executable(Wav ${SOURCE_FILES})