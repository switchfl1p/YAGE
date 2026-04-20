/* switchflip 2025 */
/* For more info on shaders:
https://registry.khronos.org/OpenGL/specs/gl/glspec46.core.pdf#page=109
*/

#pragma once
#include <glad/gl.h>
#include <string>
#include <filesystem>
#include <unordered_set>
#include <unordered_map>

class Shader{
    public:
        Shader(const std::string& filename, const std::unordered_map<std::string, std::string>& defines = {});
        ~Shader();
        GLuint getShaderUint();

    private:
        std::filesystem::path findFile(const std::string& filename);
        GLenum getShaderType(const std::string& filename);
        std::string getShaderData(const std::filesystem::path& filepath, const std::unordered_map<std::string, std::string>& defines = {});
        std::string processIncludes(const std::filesystem::path& filepath, std::unordered_set<std::string>& included, const std::unordered_map<std::string, std::string>& defines = {});

        GLuint shader_object;
};
