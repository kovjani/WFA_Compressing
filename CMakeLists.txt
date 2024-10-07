cmake_minimum_required(VERSION 3.10)
project(WFA)

set(CMAKE_C_STANDARD 11)

find_package(PkgConfig REQUIRED)
find_package(GSL REQUIRED)
find_package(BLAS REQUIRED)
pkg_check_modules(GTK REQUIRED gtk+-3.0)

include_directories(${GTK_INCLUDE_DIRS})
link_directories(${GTK_LIBRARY_DIRS})

add_executable(WFA main.c
        header_files/rgbquadtree.h
        header_files/wfa.h
        source_files/rgbquadtree.c
        source_files/wfa.c
        header_files/quadrant.h
        source_files/grayquadtree.c
        header_files/grayquadtree.h)

target_link_libraries(WFA ${GTK_LIBRARIES} GSL::gsl GSL::gslcblas m)