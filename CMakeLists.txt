cmake_minimum_required(VERSION 3.5.0 FATAL_ERROR)
project(lz77)

# set C++ build flags
set(CXX_STANDARD c++20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -std=gnu++20 ${GCC_WARNINGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -ggdb")

# dependencies
include(cmake/CPM.cmake)
CPMAddPackage("gh:IlyaGrebnov/libsais#v2.10.4")

# LZ77
add_library(lz77 INTERFACE)
target_include_directories(lz77 INTERFACE ${libsais_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(lz77 INTERFACE libsais)

# provide tests and benchmark if standalone
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    enable_testing()
    add_subdirectory(test)
endif()
