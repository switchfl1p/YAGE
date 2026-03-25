/* switchfl1p 2025-2026 */

#include <gl_util.hpp>
#include <vector>
#include <Shader.hpp>
#include <Program.hpp>
#include <iostream>

namespace gl_util{

    ModelData createBuffers(const gltf_util::Model &model){
        ModelData model_data;

        // VBO
        glGenBuffers(1, &model_data.vbo);
        glBindBuffer(GL_ARRAY_BUFFER, model_data.vbo);
        glBufferData(GL_ARRAY_BUFFER, model.vertex_data.size() * sizeof(gltf_util::Vertex), model.vertex_data.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // IBO
        glGenBuffers(1, &model_data.ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model_data.ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.index_count * sizeof(unsigned short), model.indices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        model_data.index_count = model.index_count;
        return model_data;
    }

    VAOData createVAO(ModelData &model_data){
        VAOData data;
        glGenVertexArrays(1, &data.vao);
        glBindVertexArray(data.vao);

        glBindBuffer(GL_ARRAY_BUFFER, model_data.vbo);

        //positions at attribute location 0
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(gltf_util::Vertex), (void*)offsetof(gltf_util::Vertex, position));

        //colors at attribute location 1
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(gltf_util::Vertex), (void*)offsetof(gltf_util::Vertex, color));

        //normals at attribute location 2
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(gltf_util::Vertex), (void*)offsetof(gltf_util::Vertex, normal));

        //uvs at attribute location 3
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(gltf_util::Vertex), (void*)offsetof(gltf_util::Vertex, uv));

        //indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model_data.ibo);

        glBindVertexArray(0);

        data.index_count = model_data.index_count;
        
        return data;
    }

    void cleanupBuffers(ModelData &model_data){
        glDeleteBuffers(1, &model_data.vbo);
        glDeleteBuffers(1, &model_data.ibo);
    }

    Framebuffer::Framebuffer(int w, int h) : width(w), height(h) {
        // Create framebuffer
        glGenFramebuffers(1, &FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        
        // Create texture to render to
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);
        
        // Create renderbuffer for depth/stencil
        glGenRenderbuffers(1, &RBO);
        glBindRenderbuffer(GL_RENDERBUFFER, RBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
        
        // Check if framebuffer is complete
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Framebuffer::Bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glViewport(0, 0, width, height);
    }

    void Framebuffer::Unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Framebuffer::Resize(int w, int h) {
        width = w;
        height = h;
        
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        
        glBindRenderbuffer(GL_RENDERBUFFER, RBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    }

    Framebuffer::~Framebuffer() {
        glDeleteFramebuffers(1, &FBO);
        glDeleteTextures(1, &textureID);
        glDeleteRenderbuffers(1, &RBO);
    }

    ModelData createTerrainBuffers(const TerrainData& terrain){
        ModelData model_data;

        // VBO
        glGenBuffers(1, &model_data.vbo);
        glBindBuffer(GL_ARRAY_BUFFER, model_data.vbo);
        glBufferData(GL_ARRAY_BUFFER, terrain.vertex_data.size() * sizeof(TerrainData::Vertex), terrain.vertex_data.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // IBO
        glGenBuffers(1, &model_data.ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model_data.ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, terrain.indices.size() * sizeof(unsigned int), terrain.indices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        model_data.index_count = terrain.indices.size();
        return model_data;
    }

    VAOData createTerrainVAO(ModelData& model_data){
        VAOData data;
        glGenVertexArrays(1, &data.vao);
        glBindVertexArray(data.vao);

        glBindBuffer(GL_ARRAY_BUFFER, model_data.vbo);

        //positions at attribute location 0
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainData::Vertex), (void*)offsetof(TerrainData::Vertex, position));

        //no colours, attrib 1 is skipped to comply with shaders

        //normals at attribute location 2
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainData::Vertex), (void*)offsetof(TerrainData::Vertex, normal));

        //uvs at attribute location 3
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(TerrainData::Vertex), (void*)offsetof(TerrainData::Vertex, uv));

        //indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model_data.ibo);

        glBindVertexArray(0);

        data.index_count = model_data.index_count;
        
        return data;
    }
    
} //namespace gl_util