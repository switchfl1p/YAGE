#include <Terrain.hpp>
#include <perlin.hpp>

TerrainData::TerrainData(int size_x, int size_z, float scale_p, int seed_p, float amplitude_p) 
    : width(size_x),
      depth(size_z), 
      scale(scale_p),
      seed(seed_p),
      amplitude(amplitude_p)
{
    Perlin perlin(seed);
    height_map = perlin.generateHeightMap(width, depth, scale);
}

void TerrainData::generateVertices(){
    for(int z = 0; z < depth; z++){
        for(int x = 0; x < width ; x++){
            glm::vec3 vertex(x, height_map[z][x] * amplitude, z);
            vertices.push_back(vertex);
        }
    }
}