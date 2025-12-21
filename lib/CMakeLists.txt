# ----- GLAD ------
add_library(glad STATIC ${CMAKE_CURRENT_SOURCE_DIR}/glad/src/gl.c)
target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/glad/include)

# ----- GLM (header only) ------
target_include_directories(yage PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/glm)

# ----- GLFW -----
set(GLFW_BUILD_DOCS OFF CACHE BOOL "GLFW lib only")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/GLFW)
