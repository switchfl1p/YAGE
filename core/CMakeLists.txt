set(CORE_SOURCES
	Program.cpp
	Shader.cpp
	renderer.cpp
	CameraController.cpp
	Camera.cpp
	Node.cpp
	tinygltf_impl.cpp
	gltf_util.cpp
	gl_util.cpp
	imgui_util.cpp
	Perlin.cpp
	Terrain.cpp
	Timer.cpp
	LightManager.cpp
	shader_util.cpp
	light_config.cpp
)

set(CORE_HEADERS
	Program.hpp
	Shader.hpp
	CameraController.hpp
	Camera.hpp
	Light.hpp
	Node.hpp
	gltf_util.hpp
	gl_util.hpp
	imgui_util.hpp
	Perlin.hpp
	Terrain.hpp
	Timer.hpp
	Interpolators.hpp
	LightManager.hpp
	shader_util.hpp
	InterpolatorTraits.hpp
	light_config.hpp
)

add_library(core STATIC ${CORE_SOURCES} ${CORE_HEADERS})
target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(core PUBLIC glad glfw glm tinygltf imgui)
