cmake_minimum_required(VERSION 3.10)
project(YAGE)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "GLFW lib only")

# ----- Executable -----
add_executable(yage
main.cpp
core/Shader.cpp
core/Program.cpp)

target_include_directories(yage PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/core)

# ----- GLAD -----
add_library(glad STATIC lib/glad/src/gl.c)
target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/glad/include)

# ----- GLFW -----
add_subdirectory(lib/GLFW)

# ----- GLM (header-only) -----
target_include_directories(yage PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/glm)

# ----- Linking -----
target_link_libraries(yage PRIVATE glad glfw)
