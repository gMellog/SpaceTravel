cmake_minimum_required(VERSION 3.20.0)
project(spaceTravel CXX)

add_subdirectory(sources)
add_executable(spaceTravel main.cpp)

target_link_libraries(spaceTravel spaceTravelLib)
