/* switchfl1p 2025-2026 */

#pragma once

#include <glad/gl.h>
#include <string>
#include <gltf_util.hpp>

namespace core_util{

    struct LitProgramData{
        GLuint program_uint;

        GLuint material_diffuse_unif;
        GLuint camera_space_light_position_unif;
        GLuint light_intensity_unif;
        GLuint ambient_intensity_unif;
        GLuint light_attenuation_unif;
        GLuint shininess_factor_unif;
        
        //pbr
        GLuint base_color_unif;
        GLuint metallic_unif;
        GLuint roughness_unif;
    };

    struct UnlitProgramData{
        GLuint program_uint;

        GLuint material_diffuse_unif;
    };

    LitProgramData loadLitProgram(const std::string &vert_shader, const std::string &frag_shader);
    UnlitProgramData loadUnlitProgram(const std::string &vert_shader, const std::string &frag_shader);

    struct ModelData{
        GLuint vbo;
        GLuint ibo;
        int index_count = 0;
    };

    ModelData loadModelData(const gltf_util::Model& model);

    void cleanupBuffers(ModelData& model_data);

    struct VAOData{
        GLuint vao;
    };

    VAOData loadVAOData(ModelData& model_data);
}