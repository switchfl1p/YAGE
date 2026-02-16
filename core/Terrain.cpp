#include <Terrain.hpp>

TerrainData generateTerrain(int width, int depth, float amplitude, float frequency) {
    TerrainData terrain;
    terrain.width = width;
    terrain.depth = depth;
    
    for (int z = 0; z < depth; z++) {
        for (int x = 0; x < width; x++) {
            float nx = x / (float)width;
            float nz = z / (float)depth;
            
            // Simple noise - replace with actual noise function
            //float height = noise(nx * frequency, nz * frequency) * amplitude;
            
            // Optional: Add multiple octaves for detail (fBM)
            // height += noise(nx * frequency * 2, nz * frequency * 2) * amplitude * 0.5;
            
            //terrain.heights.push_back(height);
        }
    }
    
    return terrain;
}


/* core_util::ModelData createTerrainMesh(const TerrainData& terrain) {
    core_util::ModelData mesh;
    
    // Generate vertices
    for (int z = 0; z < terrain.depth; z++) {
        for (int x = 0; x < terrain.width; x++) {
            float height = terrain.heights[z * terrain.width + x];
            
            // Position
            mesh.positions.push_back(x * terrain.scale);
            mesh.positions.push_back(height);
            mesh.positions.push_back(z * terrain.scale);
            
            // Normal (calculated from neighbors)
            glm::vec3 normal = calculateNormal(terrain, x, z);
            mesh.normals.push_back(normal.x);
            mesh.normals.push_back(normal.y);
            mesh.normals.push_back(normal.z);
            
            // UV
            mesh.uvs.push_back(x / (float)terrain.width);
            mesh.uvs.push_back(z / (float)terrain.depth);
        }
    }
    
    // Generate indices (two triangles per quad)
    for (int z = 0; z < terrain.depth - 1; z++) {
        for (int x = 0; x < terrain.width - 1; x++) {
            int topLeft = z * terrain.width + x;
            int topRight = topLeft + 1;
            int bottomLeft = (z + 1) * terrain.width + x;
            int bottomRight = bottomLeft + 1;
            
            // Triangle 1
            mesh.indices.push_back(topLeft);
            mesh.indices.push_back(bottomLeft);
            mesh.indices.push_back(topRight);
            
            // Triangle 2
            mesh.indices.push_back(topRight);
            mesh.indices.push_back(bottomLeft);
            mesh.indices.push_back(bottomRight);
        }
    }
    
    return mesh;
}

glm::vec3 calculateNormal(const TerrainData& terrain, int x, int z) {
    float heightL = getHeight(terrain, x - 1, z);
    float heightR = getHeight(terrain, x + 1, z);
    float heightD = getHeight(terrain, x, z - 1);
    float heightU = getHeight(terrain, x, z + 1);
    
    glm::vec3 normal(heightL - heightR, 2.0f, heightD - heightU);
    return glm::normalize(normal);
}

float getHeight(const TerrainData& terrain, int x, int z) {
    // Clamp to bounds
    x = glm::clamp(x, 0, terrain.width - 1);
    z = glm::clamp(z, 0, terrain.depth - 1);
    return terrain.heights[z * terrain.width + x];
}

// Generate terrain
TerrainData terrain_data = generateTerrain(128, 128, 5.0f, 4.0f);
core_util::ModelData terrain_mesh = createTerrainMesh(terrain_data);
core_util::VAOData terrain_vao = core_util::loadVAOData(terrain_mesh);

// Setup terrain node
Node terrain_node;
terrain_node.material.type = Material::PHONG;
terrain_node.material.phong_color = glm::vec4(0.3, 0.6, 0.3, 1.0); // green grass
nodes["terrain"] = terrain_node; */