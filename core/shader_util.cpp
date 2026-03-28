/* switchfl1p 2025-2026 */

#include <shader_util.hpp>
#include <Shader.hpp>
#include <Program.hpp>

namespace shader_util{

    LitProgramData loadLitProgram(const std::string &vert_shader, const std::string &frag_shader){
        std::vector<GLuint> shaders;

        Shader vertex_shader(vert_shader);
        Shader fragment_shader(frag_shader);
        shaders.push_back(vertex_shader.getShaderUint());
        shaders.push_back(fragment_shader.getShaderUint());

        Program the_program(shaders);
        
        LitProgramData data;
        data.program_uint = the_program.getProgramUint();

        //terrain gen
        data.amplitude_unif = glGetUniformLocation(data.program_uint, "amplitude");

        return data;
    }

    UnlitProgramData loadUnlitProgram(const std::string &vert_shader, const std::string &frag_shader){
        std::vector<GLuint> shaders;

        Shader vertex_shader(vert_shader);
        Shader fragment_shader(frag_shader);
        shaders.push_back(vertex_shader.getShaderUint());
        shaders.push_back(fragment_shader.getShaderUint());

        Program the_program(shaders);

        UnlitProgramData data;
        data.program_uint = the_program.getProgramUint();

        data.material_diffuse_unif = glGetUniformLocation(data.program_uint, "material_diffuse");

        return data;
    }

} //namespace shader_util
