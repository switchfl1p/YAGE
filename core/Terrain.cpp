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
    generateVertices();
    generateIndices();
}

void TerrainData::generateVertices(){
    for(int z = 0; z < depth; z++){
        for(int x = 0; x < width; x++){
            glm::vec3 vertex(x, height_map[z * width + x] * amplitude, z);
            vertices.push_back(vertex);
        }
    }
}

void TerrainData::generateIndices(){
    for(int z = 0; z < depth - 1; z++){
        for(int x = 0; x < width - 1; x++){
            int top_left = z * width + x; //[0][0]
            int top_right = z * width + x + 1; //[0][1]
            int bottom_left = (z + 1) * width + x; //[1][0]
            int bottom_right = (z + 1) * width + x + 1; //[1][1]

            //first triangle
            indices.push_back(top_left);
            indices.push_back(top_right);
            indices.push_back(bottom_left);

            //second triangle
            indices.push_back(top_right);
            indices.push_back(bottom_right);
            indices.push_back(bottom_left);
        }
    }
}

void TerrainData::generateNormals(){
    for(int z = 0; z < depth; z++){
        for(int x = 0; x < width; x++){
            //get the neighbouring vertices
            glm::vec3 left = vertices[z * width + x - 1];
            glm::vec3 right = vertices[z * width + x + 1];
            glm::vec3 top = vertices[(z - 1) * width + x];
            glm::vec3 bot = vertices[(z + 1) * width + x];

            //construct two vectors
            glm::vec3 lr_vec = right - left;
            glm::vec3 tb_vec = bot - top;

            //cross product gives the normal
            glm::vec3 normal = glm::cross(lr_vec, tb_vec);

            normals.push_back(normal);
        }
    }
}