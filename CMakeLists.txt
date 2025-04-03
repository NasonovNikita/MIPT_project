cmake_minimum_required(VERSION 3.29)
project(game)

set(CMAKE_CXX_STANDARD 20)

add_executable(game
        src/game/entities/player.cpp
        src/game/entities/units.cpp
        src/game/gameObjects.cpp
        src/game/stats.cpp
        src/components.cpp
        src/main.cpp)
include_directories(include)

add_subdirectory(libs)
target_link_libraries(game PUBLIC raylib)
