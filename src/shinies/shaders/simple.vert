#version 330

layout(location = 0) in vec3 position;

layout(std140) uniform Matrices
{
	mat4 mv_mat;
	mat4 persp_mat;
};

void main()
{
	gl_Position = persp_mat * mv_mat * vec4(position, 1.0);
}