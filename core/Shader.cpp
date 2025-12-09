/* switchfl1p 2025 */
/* For more info on shaders:
https://registry.khronos.org/OpenGL/specs/gl/glspec46.core.pdf#page=109
*/

#include "Shader.hpp"
#include <filesystem>

namespace fs = std::filesystem;

Shader::Shader(const std::string& filename){
    fs::path shader_path = findFile(filename);
    GLenum type = getShaderType(filename);

}

GLenum Shader::getShaderType(const std::string& filename){
    std::string extension;
    GLenum type;
    
    size_t last_dot = filename.find_last_of('.');
    if(last_dot != std::string::npos){
        extension = filename.substr(last_dot + 1);
    }

    if(extension == "vert"){
        type = GL_VERTEX_SHADER;
    }
    else if(extension == "frag"){
        type = GL_FRAGMENT_SHADER;
    }

    return type;
}

std::filesystem::path Shader::findFile(const std::string& filename){
    fs::path cwd = fs::current_path();

    //check /shaders/
    fs::path candidate1 = cwd / "shaders" / filename;
    if(fs::exists(candidate1)){
        return candidate1;
    }

    //check ../shaders/
    fs::path candidate2 = cwd.parent_path() / "shaders" / filename;
    if(fs::exists(candidate2)){
        return candidate2;
    }

    return {};
}

void createShader(GLenum type, const std::string& filename){
    
}