/* switchfl1p 2025-2026 */

#pragma once

#include <glad/gl.h>
#include <gltf_util.hpp>
#include <Terrain.hpp>

namespace gl_util{

    struct ModelData{
        GLuint vbo;
        GLuint ibo;
        int index_count = 0;
    };

    ModelData loadModelData(const gltf_util::Model& model);

    void cleanupBuffers(ModelData& model_data);

    struct VAOData{
        GLuint vao;
        int index_count = 0;
    };

    VAOData loadVAOData(ModelData& model_data);

    class Framebuffer {
        public:
            Framebuffer(int w, int h);
            ~Framebuffer();

            GLuint FBO;
            GLuint textureID;
            GLuint RBO;
            int width, height;
            
            void Bind();
            void Unbind();
            void Resize(int w, int h);
    };

    //good opportunity to look into polymorphism or templates or whatever it is here
    ModelData createTerrainBuffers(const TerrainData& terrain);

    VAOData createTerrainVAO(ModelData& model_data);
}