/* switchfl1p 2025-2026 */

#pragma once

#include <glad/gl.h>
#include <string>

namespace core_util{

    struct LitProgramData{
    GLuint program_uint;

    GLuint material_diffuse_unif;
    GLuint camera_space_light_position_unif;
    GLuint light_intensity_unif;
    GLuint ambient_intensity_unif;
    GLuint clip_to_camera_mat_unif;
    GLuint light_attenuation_unif;
    GLuint window_size_unif;
};

struct UnlitProgramData{
    GLuint program_uint;

    GLuint material_diffuse_unif;
};

    LitProgramData loadLitProgram(const std::string &vert_shader, const std::string &frag_shader);
    UnlitProgramData loadUnlitProgram(const std::string &vert_shader, const std::string &frag_shader);
}