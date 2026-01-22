#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

//uniform block
layout(std140) uniform Matrices
{
	mat4 mvp_mat;
	mat4 norm_view_mat;
};

uniform mat4 view_mat;
uniform vec4 light_intensity;
uniform vec4 ambient_intensity;
uniform vec4 diffuse_color;

uniform vec3 light_pos;

uniform int is_point_light;

smooth out vec4 interp_color;

void main()
{
	gl_Position = mvp_mat * vec4(position, 1.0);

	//Gouraud shading
	mat3 norm_view_mat_3by3 = mat3(norm_view_mat);
	mat3 norm_matrix = transpose(inverse(norm_view_mat_3by3));
	vec3 norm_cam_space = normalize(norm_matrix * normal); //transform the normals to view space

	vec4 light_pos_camera_space = view_mat * vec4(light_pos, 1.0);
	vec4 camera_position = view_mat * vec4(position,1.0);
	vec3 dir_to_light = normalize(vec3(light_pos_camera_space) - vec3(camera_position));

	float cos_ang_incidence = dot(norm_cam_space, dir_to_light);
	cos_ang_incidence = clamp(cos_ang_incidence, 0, 1);

	interp_color = (diffuse_color * light_intensity * cos_ang_incidence) + (diffuse_color * ambient_intensity);

}


