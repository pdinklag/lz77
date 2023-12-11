add_executable(test-gzip test_gzip.cpp)
target_link_libraries(test-gzip PRIVATE lz77)
add_test(gzip ${CMAKE_CURRENT_BINARY_DIR}/test-gzip)

add_executable(test-lpf test_lpf.cpp)
target_link_libraries(test-lpf PRIVATE lz77)
add_test(lpf ${CMAKE_CURRENT_BINARY_DIR}/test-lpf)
