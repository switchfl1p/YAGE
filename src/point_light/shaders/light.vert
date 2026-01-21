#version 330

layout(location = 0) in vec3 position;

layout(std140) uniform Matrices
{
	mat4 mvp_mat;
	mat4 norm_view_mat;
};

uniform vec4 diffuse_color;

smooth out vec4 interp_color;

void main()
{
	gl_Position = mvp_mat * vec4(position, 1.0);

    interp_color = diffuse_color;
}