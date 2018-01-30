CMAKE_MINIMUM_REQUIRED(VERSION 2.8.4 FATAL_ERROR)
PROJECT(benchmark)
INCLUDE(ExternalProject)

# Set default ExternalProject root directory
SET_DIRECTORY_PROPERTIES(PROPERTIES EP_PREFIX .)

# Add benchmark
ExternalProject_Add(
googlebenchmark
URL https://github.com/google/benchmark/archive/v1.0.0.tar.gz
TIMEOUT 30
CMAKE_ARGS -DCMAKE_BUILD_TYPE:STRING=Release
# Disable install step
INSTALL_COMMAND ""
# Wrap download, configure and build steps in a script to log output
LOG_DOWNLOAD ON
LOG_CONFIGURE ON
LOG_BUILD ON)

# Specify include dir
ExternalProject_Get_Property(googlebenchmark source_dir)
SET(googlebenchmark_dir ${source_dir})

INCLUDE_DIRECTORIES(
    ${googlebenchmark_dir}/include
    ${fecmagic_dir}/src
    ${ezpwd_rs_dir}/c++
    ${mersinvald_reed_solomon_dir}/include
    ../include)

# Add benchmark executable target
ADD_EXECUTABLE(benchmark
    ConvolutionalEncoderBenchmark.cpp
    ConvolutionalDecoderBenchmark.cpp
    ReedSolomonBenchmark.cpp
    PolarEncoderBenchmark.cpp
    PolarDecoderBenchmark.cpp
    BenchmarkMain.cpp)

# Create dependency of benchmark on googlebenchmark
ADD_DEPENDENCIES(benchmark googlebenchmark fecmagic ezpwd_rs mersinvald_reed_solomon)

# Specify benchmark's link libraries
ExternalProject_Get_Property(googlebenchmark binary_dir)
TARGET_LINK_LIBRARIES(benchmark
    ${binary_dir}/src/${CMAKE_FIND_LIBRARY_PREFIXES}benchmark.a
    pthread)
