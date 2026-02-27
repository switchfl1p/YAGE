/* switchfl1p 2025-2026 */
#pragma once
#include <vector>
#include <glm/glm.hpp>

class TerrainData{
    public:
        struct Vertex{
            glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
            glm::vec3 normal = glm::vec3(0.0f, 1.0f, 0.0f);
            glm::vec2 uv = glm::vec2(0.0f, 0.0f);
        };
        
        TerrainData(int size_x, int size_z, float scale, int seed, float amplitude, float tilescale, int octaves, float lacunarity, float persistance);

        std::vector<unsigned int> indices;

        std::vector<Vertex> vertex_data;

        int width;
        int depth;
        float scale;
        int seed;
        float amplitude;
        float tilescale;
        
        int octaves;
        float lacunarity;
        float persistance;

        void generateTerrain();
    
    private:
        std::vector<float> height_map;

        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> uv;

        void generateVertices();
        void generateIndices();
        void generateNormals();
        void generateTextureCoordinates();

        //turns the separate vectors into one interleaved vector
        void fillVertexData();
};