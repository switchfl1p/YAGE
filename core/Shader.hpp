/* switchflip 2025 */
/* For more info on shaders:
https://registry.khronos.org/OpenGL/specs/gl/glspec46.core.pdf#page=109
*/

#pragma once
#include <glad/gl.h>
#include <string>
#include <filesystem>

class Shader{
public:
    Shader(const std::string& filename);
    ~Shader();
    GLuint getShaderUint();

private:
    std::filesystem::path findFile(const std::string& filename);
    GLenum getShaderType(const std::string& filename);
    std::string getShaderData(const std::filesystem::path& filepath);

    GLuint shader_object;
};
