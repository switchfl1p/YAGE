#version 440

layout(location = 0) in vec3 position;
layout(location = 2) in vec3 normal;

layout(std140) uniform Matrices
{
	mat4 model_mat;
	mat4 view_mat;
	mat4 persp_mat;
};

out vec3 camera_space_normal;
out vec3 camera_space_position;

struct PointLight{
	vec4 position;
	vec4 intensity;
	float attenuation;
};

struct DirectionalLight{
	vec4 direction;
	vec4 intensity;
};

#define MAX_POINT_LIGHTS 2
#define MAX_DIR_LIGHTS 2

layout(std140) uniform Lights{
	PointLight point_lights[MAX_POINT_LIGHTS];
	DirectionalLight dir_lights[MAX_DIR_LIGHTS];
	int point_light_count;
	int dir_light_count;
};

void main()
{
	mat4 mv_mat = view_mat * model_mat;
	vec4 temp_cam_space_pos = mv_mat * vec4(position, 1.0);
	gl_Position = persp_mat * temp_cam_space_pos;

	mat3 normal_mv_mat = mat3(mv_mat);
	camera_space_normal = normal_mv_mat * normal;
	camera_space_position = vec3(temp_cam_space_pos);
}