add_executable(test-gzip test_gzip.cpp)
target_link_libraries(test-gzip PRIVATE lz77)
add_test(gzip ${CMAKE_CURRENT_BINARY_DIR}/test-gzip)

add_executable(test-kkp2 test_kkp2.cpp)
target_link_libraries(test-kkp2 PRIVATE lz77)
add_test(kkp2 ${CMAKE_CURRENT_BINARY_DIR}/test-kkp2)

add_executable(test-lpf test_lpf.cpp)
target_link_libraries(test-lpf PRIVATE lz77)
add_test(lpf ${CMAKE_CURRENT_BINARY_DIR}/test-lpf)
