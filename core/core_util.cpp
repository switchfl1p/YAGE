/* switchfl1p 2025-2026 */

#include <core_util.hpp>
#include <vector>
#include <Shader.hpp>
#include <Program.hpp>

core_util::LitProgramData core_util::loadLitProgram(const std::string &vert_shader, const std::string &frag_shader){
    std::vector<GLuint> shaders;

    Shader vertex_shader(vert_shader);
    Shader fragment_shader(frag_shader);
    shaders.push_back(vertex_shader.getShaderUint());
    shaders.push_back(fragment_shader.getShaderUint());

    Program the_program(shaders);
    
    core_util::LitProgramData data;
    data.program_uint = the_program.getProgramUint();

    data.material_diffuse_unif = glGetUniformLocation(data.program_uint, "material_diffuse");
    data.camera_space_light_position_unif = glGetUniformLocation(data.program_uint, "camera_space_light_position");
    data.light_intensity_unif = glGetUniformLocation(data.program_uint, "light_intensity");
    data.ambient_intensity_unif = glGetUniformLocation(data.program_uint, "ambient_intensity");
    data.clip_to_camera_mat_unif = glGetUniformLocation(data.program_uint, "clip_to_camera_mat");
    data.window_size_unif = glGetUniformLocation(data.program_uint, "window_size");
    data.light_attenuation_unif = glGetUniformLocation(data.program_uint, "light_attenuation");

    return data;
}

core_util::UnlitProgramData core_util::loadUnlitProgram(const std::string &vert_shader, const std::string &frag_shader){
    std::vector<GLuint> shaders;

    Shader vertex_shader(vert_shader);
    Shader fragment_shader(frag_shader);
    shaders.push_back(vertex_shader.getShaderUint());
    shaders.push_back(fragment_shader.getShaderUint());

    Program the_program(shaders);

    core_util::UnlitProgramData data;
    data.program_uint = the_program.getProgramUint();

    data.material_diffuse_unif = glGetUniformLocation(data.program_uint, "material_diffuse");

    return data;
}