cmake_minimum_required(VERSION 3.10)
project(ustcadb)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(src)

add_executable(test main.cpp)
target_link_libraries(test libbmgr)
