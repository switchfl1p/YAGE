/* switchfl1p 2025-2026 */
#pragma once
#include <vector>
#include <glm/glm.hpp>

class TerrainData{
    public:
        TerrainData(int size_x, int size_z, float s, int seed, float amplitude);
        std::vector<std::vector<float>> height_map;
        std::vector<glm::vec3> vertices;
    private:
        int width;
        int depth;
        float scale;
        int seed;
        float amplitude;
        void generateVertices();
};