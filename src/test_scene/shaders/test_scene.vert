#version 330

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 color;

smooth out vec4 the_color;

uniform mat4 model_matrix;
uniform mat4 camera_matrix;
uniform mat4 perspective_matrix;

void main()
{
	mat4 mvp_matrix = perspective_matrix * camera_matrix * model_matrix;
	gl_Position = mvp_matrix * position;
	the_color = color;
}
