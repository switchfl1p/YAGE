#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

layout(std140) uniform Matrices
{
	mat4 mv_mat;
	mat4 persp_mat;
};

out vec3 camera_space_normal;

void main()
{
	gl_Position = persp_mat * mv_mat * vec4(position, 1.0);

	mat3 normal_mv_mat = mat3(mv_mat);
	camera_space_normal = normalize(normal_mv_mat * normal);
}