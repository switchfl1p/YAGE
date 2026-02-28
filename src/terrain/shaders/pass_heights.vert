#version 330

layout(location = 0) in vec3 position;
layout(location = 2) in vec3 normal;

layout(std140) uniform Matrices
{
	mat4 mv_mat;
	mat4 persp_mat;
};

out vec3 camera_space_normal;
out vec3 camera_space_position;
out float terrain_height;

void main()
{
	vec4 temp_cam_space_pos = mv_mat * vec4(position, 1.0);
	gl_Position = persp_mat * temp_cam_space_pos;

	mat3 normal_mv_mat = mat3(mv_mat);
	camera_space_normal = normal_mv_mat * normal;
	camera_space_position = vec3(temp_cam_space_pos);

    terrain_height = position.y;
}