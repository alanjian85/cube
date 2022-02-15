cmake_minimum_required(VERSION 3.0.0)
project(cube)

set(CUBE_SOURCE_FILES
    src/main.cpp)
add_executable(cube ${CUBE_SOURCE_FILES})

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/)

include(BgfxHelper)

bgfx_add_shaders(cube)

find_package(SDL2 REQUIRED)
find_package(bgfx REQUIRED)

target_include_directories(cube PRIVATE
    ${SDL2_INCLUDE_DIRS})

target_link_libraries(cube PRIVATE
    ${SDL2_LIBRARIES}
    bgfx::bgfx)