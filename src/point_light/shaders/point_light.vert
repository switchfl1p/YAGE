#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

//uniform block
layout(std140) uniform Matrices
{
	mat4 mvp_mat;
	mat4 norm_view_mat;
};

uniform vec4 light_dir;
uniform vec4 light_intensity;
uniform vec4 ambient_intensity;
uniform vec4 diffuse_color;

smooth out vec4 interp_color;

void main()
{
	gl_Position = mvp_mat * vec4(position, 1.0);

	//Gouraud shading
	mat3 norm_view_mat_3by3 = mat3(norm_view_mat);
	mat3 norm_matrix = transpose(inverse(norm_view_mat_3by3));
	vec3 norm_cam_space = normalize(norm_matrix * normal); //transform the normals to view space

	vec3 normalized_light_dir = normalize(vec3(light_dir)); 
	float cos_ang_incidence = dot(norm_cam_space, normalized_light_dir); //cos of the angle is the dot product
	cos_ang_incidence = clamp(cos_ang_incidence, 0, 1); //keep values between 0 and 1

	vec4 ambient = ambient_intensity * diffuse_color; //add ambient color

	//Lambertian reflectance model
    vec4 diffuse = light_intensity * diffuse_color * cos_ang_incidence;
	
    interp_color = ambient + diffuse;

}


