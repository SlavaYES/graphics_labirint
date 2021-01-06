cmake_minimum_required(VERSION 3.17)
project(labirint)

set(CMAKE_CXX_STANDARD 14)

find_package(SFML 2.5 COMPONENTS graphics REQUIRED)

add_executable(labirint main.cpp)

target_link_libraries(labirint sfml-graphics)