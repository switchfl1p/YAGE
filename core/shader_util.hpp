/* switchfl1p 2025-2026 */

#pragma once

#include <glad/gl.h>
#include <string>

namespace shader_util{

    struct LitProgramData{
        GLuint program_uint;

        //terrain
        GLuint amplitude_unif;
    };

    struct UnlitProgramData{
        GLuint program_uint;

        GLuint material_diffuse_unif;
    };

    LitProgramData loadLitProgram(const std::string &vert_shader, const std::string &frag_shader);
    UnlitProgramData loadUnlitProgram(const std::string &vert_shader, const std::string &frag_shader);
}