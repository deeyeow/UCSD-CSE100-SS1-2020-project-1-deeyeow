# add_subdirectory(bitStream) # TODO: Uncomment this line for final submission
add_subdirectory(encoder)

add_executable (compress compress.cpp FileUtils.hpp)
target_link_libraries(compress PRIVATE huffman_encoder)

add_executable (uncompress uncompress.cpp FileUtils.hpp)
target_link_libraries(uncompress PRIVATE huffman_encoder)
