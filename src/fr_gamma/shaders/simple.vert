#version 330

layout(location = 0) in vec3 position;

layout(std140) uniform Matrices
{
	mat4 model_mat;
	mat4 view_mat;
	mat4 persp_mat;
};

void main()
{
	gl_Position = persp_mat * view_mat * model_mat * vec4(position, 1.0);
}