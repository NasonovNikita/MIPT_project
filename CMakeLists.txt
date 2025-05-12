cmake_minimum_required(VERSION 3.29)
project(game)

set(CMAKE_CXX_STANDARD 20)

add_definitions (-DPROJECT_ROOT_PATH="${CMAKE_SOURCE_DIR}")

add_executable(game
        src/core/cameraSystem.cpp
        src/core/animation.cpp
        src/game/entities/player.cpp
        src/game/entities/units.cpp
        src/game/gameObjects.cpp
        src/game/stats.cpp
        src/game/worldMap.cpp
        src/UI/buttonSystem.cpp
        src/components.cpp
        src/main.cpp)
include_directories(include)

add_subdirectory(libs)
target_link_libraries(game PUBLIC raylib)
