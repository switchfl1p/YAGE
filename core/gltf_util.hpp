/* switchfl1p 2025-2026 */

#pragma once
#include <string>

namespace gltf_util{

    class Loader{
        public:
            Loader();
            ~Loader();
    };

    class Model{
        public:
            Model(std::string filename, Loader loader);
            ~Model();

            int vertex_count;
            int index_count;

            const float *positions;
            const float *normals;
            const unsigned short *indices;
    };
}