cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 14)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_subdirectory(include)
add_subdirectory(lib)
add_subdirectory(src)
