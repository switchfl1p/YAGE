set(CORE_SOURCES
	Program.cpp
	Shader.cpp
	renderer.cpp
	CameraController.cpp
	Camera.cpp
)

set(CORE_HEADERS
	Program.hpp
	Shader.hpp
	CameraController.hpp
	Camera.hpp
)

add_library(core STATIC ${CORE_SOURCES} ${CORE_HEADERS})
target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(core PUBLIC glad glfw glm)
