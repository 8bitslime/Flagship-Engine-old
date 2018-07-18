cmake_minimum_required(VERSION 3.0.2)

if(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE Debug)
endif()

project(Flagship C)

set(ENGINE_SRCS
src/buffer.c
src/cmd.c
src/console.c
src/cvar.c
src/input.c
src/main.c
src/mesh.c
src/shader.c
src/text.c
src/texture.c
src/window.c
src/objloader.c
libs/glad/src/glad.c
)

set(BUILD_SHARED_LIBS OFF)
add_subdirectory(libs/glfw)

add_executable(flagship ${ENGINE_SRCS})
target_include_directories(flagship PUBLIC libs/glfw/include libs/glad/include libs/cglm/include libs/stb/ libs/)
target_link_libraries(flagship glfw)
