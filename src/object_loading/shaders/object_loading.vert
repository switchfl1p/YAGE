#version 330

layout (location = 0) in vec3 position;

uniform mat4 model_matrix;
uniform mat4 camera_matrix;
uniform mat4 perspective_matrix;

smooth out vec3 frag_pos;

void main()
{
	frag_pos = position;
	mat4 mvp_matrix = perspective_matrix * camera_matrix * model_matrix;
	gl_Position = mvp_matrix * vec4(position, 1.0);
}
