#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

//uniform block
layout(std140) uniform Matrices
{
	mat4 mvp_mat;
	mat4 norm_view_mat;
};

uniform mat4 model_to_camera_mat;
uniform mat4 camera_to_persp_mat;
uniform vec4 in_diffuse_color;

out vec4 diffuse_color;
out vec3 vertex_normal;
out vec3 model_space_position;

void main()
{
	gl_Position = camera_to_persp_mat * (model_to_camera_mat * vec4(position, 1.0));
	model_space_position = position;
	vertex_normal = normal;
	diffuse_color = in_diffuse_color;
}