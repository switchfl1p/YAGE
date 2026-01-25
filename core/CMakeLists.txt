set(CORE_SOURCES
	Program.cpp
	Shader.cpp
	renderer.cpp
	CameraController.cpp
	Camera.cpp
	LightController.cpp
	tinygltf_impl.cpp
	gltf_util.cpp
)

set(CORE_HEADERS
	Program.hpp
	Shader.hpp
	CameraController.hpp
	Camera.hpp
	Light.hpp
	Material.hpp
	LightController.hpp
	gltf_util.hpp
)

add_library(core STATIC ${CORE_SOURCES} ${CORE_HEADERS})
target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(core PUBLIC glad glfw glm tinygltf)
