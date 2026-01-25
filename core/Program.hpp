/* switchfl1p 2025 */

#pragma once

#include <glad/gl.h>
#include <vector>

class Program{
    public:
        Program(const std::vector<GLuint>& shaders);
        GLuint getProgramUint();

    private:
        GLuint program_object;
};