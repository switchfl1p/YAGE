/* switchflip 2025 */
/* For more info on shaders:
https://registry.khronos.org/OpenGL/specs/gl/glspec46.core.pdf#page=109
*/

#pragma once
#include <glad/gl.h>
#include <string>

class Shader{
public:
    Shader(const std::string& filename);
    ~Shader();

private:
    void createShader(GLenum type, const std::string& filename);
    void findFile();
    GLenum getShaderType(const std::string& filename);
};
