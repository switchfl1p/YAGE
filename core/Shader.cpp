/* switchfl1p 2025 */
/* For more info on shaders:
https://registry.khronos.org/OpenGL/specs/gl/glspec46.core.pdf#page=109
*/

#include "Shader.hpp"
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

Shader::Shader(const std::string& filename){
    //find file needs to throw
    fs::path shader_path = findFile(filename);
    GLenum type = getShaderType(filename);
    shader_object = glCreateShader(type);
    std::string shader_data = getShaderData(shader_path);

    //string formatting for expected parameters
    const GLchar* data_ptr = shader_data.c_str();
    //nullptr = null-terminated, function figures lenght out 
    glShaderSource(shader_object, 1, &data_ptr, nullptr);
    glCompileShader(shader_object);

    GLint status;
    glGetShaderiv(shader_object, GL_COMPILE_STATUS, &status);

    if(status == GL_FALSE){
        GLint info_log_length;
        glGetShaderiv(shader_object, GL_INFO_LOG_LENGTH, &info_log_length);

        GLchar *str_info_log = new GLchar[info_log_length + 1];
        glGetShaderInfoLog(shader_object, info_log_length, NULL, str_info_log);

        std::string shader_type_str;
		switch(type)
		{
		case GL_VERTEX_SHADER: shader_type_str = "vertex"; break;
		case GL_FRAGMENT_SHADER: shader_type_str = "fragment"; break;
		}
        
        std::cerr << "Compile failure in " << shader_type_str << " shader:\n" << str_info_log << "\n";
    }
}

Shader::~Shader(){
    glDeleteShader(shader_object);
}

GLuint Shader::getShaderUint(){
    return shader_object;
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

    //check ./shaders/
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

std::string Shader::getShaderData(const std::filesystem::path& filepath){
    std::ifstream file(filepath);
    
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open shader file: " + filepath.string());
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    return buffer.str();
}