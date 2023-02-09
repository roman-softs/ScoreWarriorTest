cmake_minimum_required(VERSION 3.5)

project(ScoreWarriorTest LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB source_files src/*cpp)

add_library(
    game_lib
    ${source_files}
    )

target_include_directories(
    game_lib
    PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/inc>"
    )

add_executable(ScoreWarriorTest main.cpp)

target_link_libraries(
    ScoreWarriorTest
    PRIVATE
    game_lib
    )
