cmake_minimum_required(VERSION 3.0.0)
project(cube)

set(CUBE_SOURCE_FILES
    src/main.cpp)
add_executable(cube ${CUBE_SOURCE_FILES})

find_package(SDL2 REQUIRED)
find_package(spdlog REQUIRED)

target_include_directories(cube PRIVATE
    ${SDL2_INCLUDE_DIRS})

target_link_libraries(cube PRIVATE
    ${SDL2_LIBRARIES}
    spdlog::spdlog)