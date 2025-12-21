cmake_minimum_required(VERSION 3.10)
project(YAGE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(yage
main.cpp
core/Shader.cpp
core/Program.cpp
core/Renderer.cpp)

target_include_directories(yage PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/core)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib)

target_link_libraries(yage PRIVATE glad glfw)
