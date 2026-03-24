/* switchfl1p 2025-2026 */

#pragma once

#include <glad/gl.h>
#include <string>

namespace shader_util{
    
    struct LitProgramData{
        GLuint program_uint;

        GLuint material_diffuse_unif;
        GLuint ambient_intensity_unif;
        GLuint shininess_factor_unif;
        
        //pbr
        GLuint base_color_unif;
        GLuint metallic_unif;
        GLuint roughness_unif;

        //terrain
        GLuint amplitutde_unif;
    };

    struct UnlitProgramData{
        GLuint program_uint;

        GLuint material_diffuse_unif;
    };

    LitProgramData loadLitProgram(const std::string &vert_shader, const std::string &frag_shader);
    UnlitProgramData loadUnlitProgram(const std::string &vert_shader, const std::string &frag_shader);
}