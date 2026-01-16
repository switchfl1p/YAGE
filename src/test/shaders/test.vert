#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

//mvp mat
uniform mat4 mvp_mat;
uniform mat3 norm_view_mat;

uniform vec4 light_dir;
uniform vec4 light_intensity;
uniform vec4 ambient_intensity;

smooth out vec4 interp_color;

void main()
{
	gl_Position = mvp_mat * vec4(position, 1.0);

	vec4 diffuse_color = vec4(0.0, 0.0, 1.0, 1.0);
	vec3 norm_cam_space = normalize(norm_view_mat * normal);
	vec3 normalized_light_dir = normalize(vec3(light_dir));
	float cos_ang_incidence = dot(norm_cam_space, normalized_light_dir);
	cos_ang_incidence = clamp(cos_ang_incidence, 0, 1);

	vec4 ambient = ambient_intensity * diffuse_color;
    vec4 diffuse = light_intensity * diffuse_color * cos_ang_incidence;
    interp_color = ambient + diffuse;
}

