#!/usr/bin/env python3
# scene_gen.py
import os
import sys
import argparse
from pathlib import Path
from datetime import datetime

def create_scene(scene_name, executable_name=None):
    if executable_name is None:
        executable_name = scene_name.lower().replace(' ', '_')
    
    # Sanitize the scene name for filenames
    scene_name_safe = scene_name.lower().replace(' ', '_')
    
    base_path = Path("src") / scene_name
    shaders_path = base_path / "shaders"
    
    # Create directories
    base_path.mkdir(parents=True, exist_ok=True)
    shaders_path.mkdir(parents=True, exist_ok=True)
    
    # Create .cpp file
    cpp_template = f'''// {scene_name}.cpp
// Auto-generated on {datetime.now().strftime("%d/%m/%Y %H:%M:%S")}

#include <glad/gl.h>
#include <GLFW/glfw3.h>

void initalizeProgram(){{
    // Initialize shaders and programs here

    // Example shader loading:
    // std::vector<GLuint> shaders;
    // Shader vertex_shader("hello_matrix.vert");
    // Shader fragment_shader("hello_matrix.frag");
    // shaders.push_back(vertex_shader.getShaderUint());
    // shaders.push_back(fragment_shader.getShaderUint());

    //Program the_program(shaders);
    //program_uint = the_program.getProgramUint();
}}

GLuint vertex_buffer_object;
GLuint vao;

void initalizeVertexBuffer(){{
    // Example usage:
    // glGenBuffers(1, &vertex_buffer_object);

	// glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
	// glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);
	// glBindBuffer(GL_ARRAY_BUFFER, 0);
}}

void init() {{
    initalizeProgram();
    initalizeVertexBuffer();

    glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

    glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);
}}

void display(GLFWwindow* window) {{
    // Rendering code here
    // Called every frame
    
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glfwSwapBuffers(window);
    glfwPollEvents();
}}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {{
    glViewport(0, 0, width, height);
}}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}}
'''
    
    (shaders_path / f"{scene_name_safe}.vert").write_text(vertex_shader)
    (shaders_path / f"{scene_name_safe}.frag").write_text(fragment_shader)
    
    # Update CMakeLists.txt
    cmake_path = Path("src") / "CMakeLists.txt"
    if cmake_path.exists():
        with open(cmake_path, 'a') as f:
            f.write(f'\nadd_scene("{scene_name}" "{executable_name}" "{scene_name_safe}.cpp")\n')
        print(f"✓ Added to {cmake_path}")
    
    print(f"✓ Scene '{scene_name}' created successfully!")
    print(f"  - Source: {cpp_file}")
    print(f"  - Shaders: {shaders_path}/")
    print(f"    * {scene_name_safe}.vert (vertex shader)")
    print(f"    * {scene_name_safe}.frag (fragment shader)")
    print(f"  - Executable: {executable_name}")

def main():
    parser = argparse.ArgumentParser(description='Create a new scene for the game engine')
    parser.add_argument('name', help='Name of the scene')
    parser.add_argument('-e', '--executable', help='Executable name (defaults to scene name)')
    
    args = parser.parse_args()
    create_scene(args.name, args.executable)

if __name__ == "__main__":
    main()