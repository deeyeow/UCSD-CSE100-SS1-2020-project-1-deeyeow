add_executable (test_HCTree test_HCTree.cpp)
target_link_libraries(test_HCTree PRIVATE gtest_main huffman_encoder)
add_test(test_HCTree test_HCTree)

add_executable(test_HCNode test_HCNode.cpp)
target_link_libraries(test_HCNode PRIVATE gtest_main huffman_encoder)
add_test(test_HCNode test_HCNode)

# add_executable (test_BitOutputStream test_BitOutputStream.cpp)
# target_link_libraries(test_BitOutputStream PRIVATE gtest_main bit_output_stream)
# add_test(test_BitOutputStream test_BitOutputStream)

# add_executable (test_BitInputStream test_BitInputStream.cpp)
# target_link_libraries(test_BitInputStream PRIVATE gtest_main bit_input_stream)
# add_test(test_BitInputStream test_BitInputStream)