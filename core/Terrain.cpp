/* switchfl1p 2025-2026 */

#include <Terrain.hpp>
#include <Perlin.hpp>

TerrainData::TerrainData(int size_x, int size_z, float scale_p, int seed_p, float amplitude_p, float tilescale_p, int octaves_p, float lacunarity_p, float persistance_p) 
    : width(size_x),
      depth(size_z), 
      scale(scale_p),
      seed(seed_p),
      amplitude(amplitude_p),
      tilescale(tilescale_p),
      octaves(octaves_p),
      lacunarity(lacunarity_p),
      persistance(persistance_p)
{
    generateTerrain();
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
    //iterating through the cells, aka the spaces between vertices (length - 1)
    for(int z = 0; z < depth - 1; z++){
        for(int x = 0; x < width - 1; x++){
            unsigned int top_left = z * width + x; //[0][0]
            unsigned int top_right = z * width + x + 1; //[0][1]
            unsigned int bottom_left = (z + 1) * width + x; //[1][0]
            unsigned int bottom_right = (z + 1) * width + x + 1; //[1][1]

            //first triangle
            indices.push_back(top_left);
            indices.push_back(bottom_left);
            indices.push_back(top_right);

            //second triangle
            indices.push_back(top_right);
            indices.push_back(bottom_left);
            indices.push_back(bottom_right);
        }
    }
}

//using the triangle-averaging method
void TerrainData::generateNormals(){
    //each 3 indices in the index array defines a triangle
    //move through the index array with a stride of 3

    normals.resize(vertices.size());

    for(size_t i = 0; i < indices.size(); i += 3){
        //grab the 3 triangle vertices from the index array
        int tri_v0 = indices[i];
        int tri_v1 = indices[i+1];
        int tri_v2 = indices[i+2];

        //build 2 edge vectors
        glm::vec3 edge_vec_1 = vertices[tri_v1] - vertices[tri_v0];
        glm::vec3 edge_vec_2 = vertices[tri_v2] - vertices[tri_v0];

        glm::vec3 normal = glm::cross(edge_vec_1, edge_vec_2);

        //add to normals vector, does area weighting
        normals[tri_v0] += normal;
        normals[tri_v1] += normal;
        normals[tri_v2] += normal;
    }

    //normalize normals after processing all triangles
    for(size_t i = 0; i < normals.size(); i++){
        normals[i] = glm::normalize(normals[i]);
    }
}

void TerrainData::generateTextureCoordinates(){
    uv.resize(vertices.size());

    for(size_t i = 0; i < vertices.size(); i++){
        float u = vertices[i].x / (width - 1) * tilescale;
        float v = vertices[i].z / (depth - 1) * tilescale;

        uv[i] = glm::vec2(u, v);
    }
}

void TerrainData::fillVertexData(){
    vertex_data.resize(vertices.size());

    for(size_t i = 0; i < vertex_data.size(); i++){
        Vertex vert;
        vert.position = vertices[i];
        vert.normal = normals[i];
        vert.uv = uv[i];

        vertex_data[i] = vert;
    }
}

//fBm implementation
void TerrainData::generateTerrain(){
    //clearing allows for dynamic creation in display loop
    vertices.clear();
    indices.clear();
    normals.clear();
    uv.clear();
    vertex_data.clear();
    
    Perlin perlin(seed);

    height_map.assign(width * depth, 0.0f);

    //fBm implementation
    float fBm_freq = 1;
    float fBm_amp = 1;
    float amp_sum = 0;

    for(int i = 0; i < octaves; i++){
        std::vector<float> octave_map = perlin.generateHeightMap(width, depth, scale / fBm_freq);

        for(size_t j = 0; j < octave_map.size(); j++){
            height_map[j] += octave_map[j] * fBm_amp;
        }

        //grab the sum for normalization later
        amp_sum += fBm_amp;
        fBm_freq *= lacunarity;
        fBm_amp *= persistance;
    }

    //normalizing the height map
    for(size_t i = 0; i < height_map.size(); i++){
        height_map[i] = height_map[i]/amp_sum;
    }

    generateVertices();
    generateIndices();
    generateNormals();
    generateTextureCoordinates();

    fillVertexData();
}