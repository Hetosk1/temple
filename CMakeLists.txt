cmake_minimum_required(VERSION 3.16)
project(temple CXX) 

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(index src/main.cpp)
target_include_directories(index PRIVATE include)

add_executable(server src/server.cpp)


