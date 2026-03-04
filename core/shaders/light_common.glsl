uniform vec4 material_diffuse;
uniform vec4 light_intensity;
uniform vec4 ambient_intensity;
uniform vec3 camera_space_light_position;
uniform float light_attenuation;

in vec3 camera_space_position;
in vec3 camera_space_normal;

const vec4 specular_color = vec4(0.25, 0.25, 0.25, 1.0);
uniform float shininess_factor;

out vec4 output_color;

float calcAttenuation(in vec3 camera_space_pos, out vec3 light_dir)
{
    vec3 light_difference = camera_space_light_position - camera_space_pos;
    float light_distance_sqrt = dot(light_difference, light_difference);
    light_dir = light_difference * inversesqrt(light_distance_sqrt);

    return (1 / (1.0 + light_attenuation * sqrt(light_distance_sqrt)));
}