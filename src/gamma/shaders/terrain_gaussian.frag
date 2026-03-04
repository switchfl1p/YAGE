//Gaussian

#version 330
#include "light_common.glsl"
#include "terrain_common.glsl"

void main()
{
    vec4 color = vec4(getColor(), 1.0);
    vec3 light_dir = vec3(0.0);
    float atten = calcAttenuation(camera_space_position, light_dir);
    vec4 atten_intensity = atten * light_intensity;

    vec3 surface_normal = normalize(camera_space_normal);
    float cos_ang_incidence = dot(surface_normal, light_dir);
    cos_ang_incidence = clamp(cos_ang_incidence, 0, 1);

    vec3 view_dir = normalize(-camera_space_position);

    vec3 half_angle = normalize(light_dir + view_dir);
    float angle_normal_half = acos(dot(half_angle,surface_normal));
    float exponent = angle_normal_half / shininess_factor;
    exponent = -(exponent * exponent);
    float gaussian_term = exp(exponent);

    gaussian_term = cos_ang_incidence != 0.0 ? gaussian_term : 0.0;

    output_color = (color * atten_intensity * cos_ang_incidence) + 
        (specular_color * atten_intensity * gaussian_term) + 
        (color * ambient_intensity);

}