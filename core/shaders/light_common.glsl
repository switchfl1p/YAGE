uniform vec4 material_diffuse;
uniform float shininess_factor;

in vec3 camera_space_position;
in vec3 camera_space_normal;

out vec4 output_color;

struct PointLight{
	vec4 position;
	vec4 intensity;
	float attenuation;
};

struct DirectionalLight{
	vec4 direction;
	vec4 intensity;
};

#define MAX_POINT_LIGHTS 3
#define MAX_DIR_LIGHTS 2

layout(std140) uniform Lights{
	PointLight point_lights[MAX_POINT_LIGHTS];
	DirectionalLight dir_lights[MAX_DIR_LIGHTS];
	vec4 ambient_intensity;
	int point_light_count;
	int dir_light_count;
	float max_intensity;
	float gamma;
};

float calcAttenuation(in PointLight light, out vec3 light_dir)
{
    vec3 light_difference = light.position.xyz - camera_space_position;
    float light_distance_squared = dot(light_difference, light_difference);
    light_dir = light_difference * inversesqrt(light_distance_squared);

    return (1 / (1.0 + light.attenuation * light_distance_squared));
}