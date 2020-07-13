cmake_minimum_required(VERSION 2.8.12)
project(huffman_encoder)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -fprofile-arcs -ftest-coverage -std=c++11 -g")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# === src dependencies ===
include_directories(subprojects/cxxopts)
# === end src dependencies ===

add_subdirectory(src)

# === test dependencies ===
enable_testing()
add_subdirectory(subprojects/googletest-release-1.8.1)
# === end test dependencies ===

add_subdirectory(test)

# === custom commands ===
add_custom_target(cov COMMAND ${CMAKE_CURRENT_LIST_DIR}/build_scripts/generate_coverage_report -r "make" "make test")

add_custom_target(format COMMAND ${CMAKE_CURRENT_LIST_DIR}/build_scripts/run-clang-format.py -i -r ${CMAKE_CURRENT_LIST_DIR}/src ${CMAKE_CURRENT_LIST_DIR}/test)

add_custom_target(scan-build COMMAND scan-build make)

add_custom_target(cppcheck COMMAND cppcheck --enable=all --std=c++14 --error-exitcode=1 --suppress=missingInclude ${CMAKE_CURRENT_LIST_DIR}/src ${CMAKE_CURRENT_LIST_DIR}/test)

# Make sure build_scripts are always runnable
add_custom_target(chmod COMMAND chmod +x ${CMAKE_CURRENT_LIST_DIR}/build_scripts/*)
add_dependencies(cov chmod)
add_dependencies(format chmod)
