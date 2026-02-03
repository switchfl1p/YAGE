/* switchfl1p 2025-2026 */

#pragma once
#include <string>
#include <tiny_gltf.h>
#include <glm/glm.hpp>

namespace gltf_util{

    struct Vertex{
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        glm::vec3 normal = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec2 uv = glm::vec2(0.0f, 0.0f);
    };

    struct Model{
        std::vector<Vertex> vertex_data;
        std::vector<unsigned short> indices;
        int vertex_count = 0;
        int index_count = 0;
    };

    class Loader{
        public:
            Model loadModel(std::string filename);
            
            tinygltf::TinyGLTF loader;
    };
}