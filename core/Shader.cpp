/* switchfl1p 2025 */

#include "Shader.hpp"

Shader::Shader(const std::string& filename){
    
}

GLenum getShaderType(const std::string& filename){
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