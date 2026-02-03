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
    data.light_attenuation_unif = glGetUniformLocation(data.program_uint, "light_attenuation");
    data.shininess_factor_unif = glGetUniformLocation(data.program_uint, "shininess_factor");

    //pbr
    data.base_color_unif = glGetUniformLocation(data.program_uint, "base_color");
    data.metallic_unif = glGetUniformLocation(data.program_uint, "metallic");
    data.roughness_unif = glGetUniformLocation(data.program_uint, "roughness");

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

core_util::ModelData core_util::loadModelData(const gltf_util::Model &model){
    core_util::ModelData model_data;

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

core_util::VAOData core_util::loadVAOData(ModelData &model_data){
    core_util::VAOData data;
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
    return data;
}

void core_util::cleanupBuffers(ModelData &model_data){
    glDeleteBuffers(1, &model_data.vbo);
    glDeleteBuffers(1, &model_data.ibo);
}