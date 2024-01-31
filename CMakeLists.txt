cmake_minimum_required(VERSION 3.5.0 FATAL_ERROR)
project(lz77)

# set C++ build flags
set(CXX_STANDARD c++20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -std=gnu++20 ${GCC_WARNINGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -ggdb")

# initialize git submodules
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmakemodules")
include(InitGitSubmodules)

# create library
set(EXTLIB_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extlib)
set(LIBSAIS_SOURCE_DIR ${EXTLIB_SOURCE_DIR}/libsais)

add_library(lz77
    ${LIBSAIS_SOURCE_DIR}/src/libsais.c
    ${LIBSAIS_SOURCE_DIR}/src/libsais64.c
)
target_include_directories(lz77 PUBLIC ${LIBSAIS_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/include)

# potentially enable OpenMP in libsais
find_package(OpenMP)
if(OpenMP_FOUND)
    target_compile_definitions(lz77 PUBLIC LIBSAIS_OPENMP)
    target_link_libraries(lz77 PUBLIC OpenMP::OpenMP_C)
endif()

# provide tests and benchmark if standalone
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    enable_testing()
    add_subdirectory(test)
endif()
