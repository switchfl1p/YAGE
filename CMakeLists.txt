cmake_minimum_required(VERSION 3.10)
project(YAGE)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ----- Executable -----
add_executable(yage
main.cpp
core/Shader.cpp)


# ----- GLAD -----
add_library(glad STATIC lib/glad/src/gl.c)
target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/glad/include)

# ----- GLFW -----
add_subdirectory(lib/glfw-3.4)

# ----- GLM (header-only) -----
target_include_directories(yage PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/glm-1.0.2)

# ----- Linking -----
target_link_libraries(yage PRIVATE glad glfw)

add_executable(gladtest gladtest.cpp)
target_include_directories(gladtest PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/glm-1.0.2)
target_link_libraries(gladtest PRIVATE glad glfw)
