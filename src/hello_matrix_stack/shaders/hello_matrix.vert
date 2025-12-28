#version 330

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 color;

smooth out vec4 the_color;

uniform mat4 perspective_matrix;
uniform vec2 offset;

void main()
{
	vec4 camera_pos = position + vec4(offset.x, offset.y, 0.0, 0.0);
	gl_Position = perspective_matrix * camera_pos;
	the_color = color;
}
