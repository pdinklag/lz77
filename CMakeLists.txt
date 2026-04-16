cmake_minimum_required(VERSION 3.5.0 FATAL_ERROR)
project(lz77)

# dependencies
include(cmake/CPM.cmake)
CPMAddPackage("gh:IlyaGrebnov/libsais#v2.10.4")

# options
option(LIBSAIS_USE_OPENMP "Use OpenMP for parallelization" ON)

if(${LIBSAIS_USE_OPENMP})
    find_package(OpenMP REQUIRED)
    list(APPEND CMAKE_MODULE_PATH "deps/tbb/cmake/")
    find_package(TBB REQUIRED)
endif()

# set C++ build flags
set(CXX_STANDARD c++20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -std=gnu++20 ${GCC_WARNINGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -ggdb")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")

# LZ77
add_library(lz77 INTERFACE)
target_include_directories(lz77 INTERFACE ${libsais_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(lz77 INTERFACE libsais)
if(${LIBSAIS_USE_OPENMP})
    target_compile_definitions(lz77 INTERFACE LIBSAIS_OPENMP)
endif()

# provide tests and benchmark if standalone
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    enable_testing()
    add_subdirectory(test)
endif()
