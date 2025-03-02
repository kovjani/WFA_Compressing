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
        source_files/DeterministicDecoding.cpp
        header_files/DeterministicDecoding.h
        source_files/DeterministicCoding.cpp
        header_files/DeterministicCoding.h
        source_files/NondeterministicDecoding.cpp
        header_files/NondeterministicDecoding.h
        source_files/Quadrant.cpp
        header_files/Quadrant.h
        source_files/Transition.cpp
        header_files/Transition.h
        source_files/NondeterministicCoding.cpp
)

target_link_libraries(WFA ${GTK_LIBRARIES} GSL::gsl GSL::gslcblas m)