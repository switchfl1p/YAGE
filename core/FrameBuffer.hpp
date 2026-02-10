#pragma once
#include <glad/gl.h>

class Framebuffer {
public:
    Framebuffer(int w, int h);
    ~Framebuffer();

    GLuint FBO;
    GLuint textureID;
    GLuint RBO;
    int width, height;
    
    void Bind();
    void Unbind();
    void Resize(int w, int h);
};