//Gaussian specular only

#version 330

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

void main()
{
    vec3 light_dir = vec3(0.0);
    float atten = calcAttenuation(camera_space_position, light_dir);
    vec4 atten_intensity = atten * light_intensity;

    vec3 surface_normal = normalize(camera_space_normal);

    vec3 view_dir = normalize(-camera_space_position);

    vec3 half_angle = normalize(light_dir + view_dir);
    float angle_normal_half = acos(dot(half_angle,surface_normal));
    float exponent = angle_normal_half / shininess_factor;
    exponent = -(exponent * exponent);
    float gaussian_term = exp(exponent);

    gaussian_term = dot(surface_normal, light_dir) >= 0.0 ? gaussian_term : 0.0;

    output_color = (specular_color * atten_intensity * gaussian_term) +
        (material_diffuse * ambient_intensity);
}