cmake_minimum_required(VERSION 3.10)
project(YAGE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#Third party libraries
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib)
#Core library
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/core)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)
