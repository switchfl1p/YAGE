#pragma once
#include <glm/glm.hpp>

class Perlin{
    public:
        Perlin(int seed);
        void reseed(int seed);
        std::vector<std::vector<float>> generateHeightMap(int size_x, int size_z, float scale);
    
    private:
        std::array<int, 512> perm_table;

        std::array<glm::vec2, 8> gradients = {
            glm::vec2( 1,  0),
            glm::vec2(-1,  0),
            glm::vec2( 0,  1),
            glm::vec2( 0, -1),
            glm::normalize(glm::vec2( 1,  1)),
            glm::normalize(glm::vec2(-1,  1)),
            glm::normalize(glm::vec2( 1, -1)),
            glm::normalize(glm::vec2(-1, -1))
        };

        glm::vec2 getGradient(int x, int y);
        std::array<int, 512> genPermTable(int seed);
        float fade(float t);
        float perlin2D(float x, float y);
};
