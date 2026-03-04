//Phong Lighting

#version 330
#include "light_common.glsl"

void main()
{
    vec3 light_dir = vec3(0.0);
    float atten = calcAttenuation(camera_space_position, light_dir);
    vec4 atten_intensity = atten * light_intensity;

    vec3 surface_normal = normalize(camera_space_normal);
    float cos_ang_incidence = dot(surface_normal, light_dir);
    cos_ang_incidence = clamp(cos_ang_incidence, 0, 1);

    vec3 view_dir = normalize(-camera_space_position);
    vec3 reflect_dir = reflect(-light_dir, surface_normal);
    float phong_term = dot(view_dir, reflect_dir);
    phong_term = clamp(phong_term,0,1);
    phong_term = cos_ang_incidence != 0.0 ? phong_term : 0.0;
    phong_term = pow(phong_term, shininess_factor);

    output_color = (material_diffuse * atten_intensity * cos_ang_incidence) +
        (specular_color * atten_intensity * phong_term) +
        (material_diffuse * ambient_intensity);
}