/* switchfl1p 2025 */

#include "Program.hpp"
#include <GL/gl.h>
#include <iostream>

Program::Program(const std::vector<GLuint>& shaders){
    program_object = glCreateProgram();

    for(size_t i = 0; i < shaders.size(); i++){
        glAttachShader(program_object, shaders[i]);
    }

    glLinkProgram(program_object);

    GLint status;
    glGetProgramiv(program_object, GL_LINK_STATUS, &status);

    if(status == GL_FALSE){
        GLint info_log_length;
        glGetProgramiv(program_object, GL_INFO_LOG_LENGTH, &info_log_length);

        GLchar *str_info_log = new GLchar[info_log_length + 1];
        glGetProgramInfoLog(program_object, info_log_length, NULL, str_info_log);

        std::cerr << "Linker failure:\n" << str_info_log;
    }

    for(size_t i = 0; i < shaders.size(); i++){
        glDetachShader(program_object, shaders[i]);
    }
}

Program::~Program(){

}

GLuint Program::getProgramUint(){
    return program_object;
}