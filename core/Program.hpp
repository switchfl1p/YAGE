#pragma once

#include <glad/gl.h>
#include <vector>

class Program{
    public:
        Program(const std::vector<GLuint>& shaders);
        ~Program();
        GLuint getProgramUint();

    private:
        GLuint program_object;
};