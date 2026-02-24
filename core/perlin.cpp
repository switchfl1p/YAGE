#include <perlin.hpp>
#include <random>
#include <algorithm>

Perlin::Perlin(int seed) : perm_table(genPermTable(seed)){};

float Perlin::perlin2D(float x, float y){
    //get the floor values
    int floor_x = (int)glm::floor(x);
    int floor_y = (int)glm::floor(y);

    //find which grid cell we're in
    //permutation table size is 256 (2^8), so masking with 255 wraps index
    int cell_x = floor_x & 255;
    int cell_y = floor_y & 255;

    //find local position within the grid cell
    float local_x = x - floor_x;
    float local_y = y - floor_y;

    //get the gradient vector for each of the 4 corners
    glm::vec2 grad_TL = getGradient(cell_x, cell_y + 1);
    glm::vec2 grad_TR = getGradient(cell_x + 1, cell_y + 1);
    glm::vec2 grad_BL = getGradient(cell_x, cell_y);
    glm::vec2 grad_BR = getGradient(cell_x + 1, cell_y);

    //the 4 corner to point vectors
    glm::vec2 vec_TL(local_x, local_y - 1);
    glm::vec2 vec_TR(local_x - 1, local_y - 1);
    glm::vec2 vec_BL(local_x, local_y);
    glm::vec2 vec_BR(local_x - 1, local_y); 

    //calculate the dot products
    float dot_TL = glm::dot(grad_TL, vec_TL);
    float dot_TR = glm::dot(grad_TR, vec_TR);
    float dot_BL = glm::dot(grad_BL, vec_BL);
    float dot_BR = glm::dot(grad_BR, vec_BR);

    //apply the fade curve to smooth out local coordinates
    float u = fade(local_x);
    float v = fade(local_y);

    //interpolate between the 4 dot products
    //first between the bottom and top edges then interpolate those results vertically
    float bottom = glm::mix(dot_BL, dot_BR, u);
    float top = glm::mix(dot_TL, dot_TR, u);
    float result = glm::mix(bottom, top, v);

    return result;
}

std::array<int, 512> Perlin::genPermTable(int seed){
    std::array<int, 512> perm_table;

    for(int i = 0; i <= 255; i++){
        perm_table[i] = i;
    }

    //create random engine using a seed
    std::mt19937 rand_engine(seed);

    //shuffle the first 256 elements using the random engine
    std::shuffle(perm_table.begin(), perm_table.begin() + 256, rand_engine);

    //duplicate the elements into the rest of the array
    for(int i = 256; i < 512; i++){
        perm_table[i] = perm_table[i - 256];
    }

    return perm_table;
}

glm::vec2 Perlin::getGradient(int x, int y){
    int gradient_index = perm_table[perm_table[x] + y];

    glm::vec2 gradient = gradients[gradient_index%gradients.size()];

    return gradient;
}

void Perlin::reseed(int seed){
    perm_table = genPermTable(seed);
}

//improved Perlin noise fade function using Horner method for efficiency
float Perlin::fade(float t){
    return t * t * t * (t * (t * 6 - 15) + 10);
}

//not very cache friendly
std::vector<std::vector<float>> Perlin::generateHeightMap(int size_x, int size_z, float scale){
    std::vector<std::vector<float>> height_map;

    for(int z = 0; z < size_z; z++){
        std::vector<float> temp_vec;
        for(int x = 0; x < size_x; x++){
            temp_vec.push_back(perlin2D(x / scale, z / scale));
        }
        height_map.push_back(temp_vec);
    }

    return height_map;
}