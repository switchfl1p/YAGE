#include "Program.hpp"
#include <iostream>

Program::Program(const std::vector<GLuint>& shaders){
    program_object = glCreateProgram();
    
    for(auto& shader:shaders){
        glAttachShader(program_object, shader);
    }

    GLint status;
    glGetProgramiv (program_object, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetProgramiv(program_object, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(program_object, infoLogLength, NULL, strInfoLog);
		fprintf(stderr, "Linker failure: %s\n", strInfoLog);
		delete[] strInfoLog;
	}
}

Program::~Program(){

}

GLuint Program::getProgramUint(){
    return program_object;
}