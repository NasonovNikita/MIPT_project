cmake_minimum_required(VERSION 3.29)
project(game)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)
add_executable(game src/main.cpp)

add_subdirectory(libs)
target_link_libraries(game PUBLIC raylib)
