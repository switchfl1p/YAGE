/* switchfl1p 2025-2026 */

#pragma once
#include <string>
#include <tiny_gltf.h>

namespace gltf_util{

    struct Model{
            int vertex_count = 0;
            int index_count = 0;

            std::vector<float> positions;
            std::vector<float> normals;
            std::vector<unsigned short> indices;
    };

    class Loader{
        public:
            Model loadModel(std::string filename);
            
            tinygltf::TinyGLTF loader;
    };
}