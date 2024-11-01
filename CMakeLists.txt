cmake_minimum_required(VERSION 3.10)
project(WFA)

set(CMAKE_C_STANDARD 11)

find_package(PkgConfig REQUIRED)
find_package(GSL REQUIRED)
find_package(BLAS REQUIRED)
pkg_check_modules(GTK REQUIRED gtk+-3.0)

include_directories(${GTK_INCLUDE_DIRS})
link_directories(${GTK_LIBRARY_DIRS})

add_executable(WFA main.cpp
        source_files/Testing.cpp
        header_files/Testing.h
        source_files/Decoding.cpp
        header_files/Decoding.h
        source_files/Coding.cpp
        header_files/Coding.h
        source_files/Quadrant.cpp
        header_files/Quadrant.h
        header_files/State.cpp
        header_files/State.h)

target_link_libraries(WFA ${GTK_LIBRARIES} GSL::gsl GSL::gslcblas m)