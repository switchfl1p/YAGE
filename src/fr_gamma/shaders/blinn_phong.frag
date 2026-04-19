#version 330

#include "light_common.glsl"


vec4 calcPointLight(PointLight light, vec3 surface_normal, vec3 view_dir) {
    vec3 light_dir;
    float attenuation = calcAttenuation(light, light_dir);
    vec4 atten_intensity = attenuation * light.intensity;

    float cos_ang_incidence = clamp(dot(surface_normal, light_dir), 0.0, 1.0);

    vec3 half_angle = normalize(light_dir + view_dir);
    float blinn_term = clamp(dot(surface_normal, half_angle), 0.0, 1.0);
    blinn_term = cos_ang_incidence != 0.0 ? blinn_term : 0.0;
    blinn_term = pow(blinn_term, material.shininess_factor);

    return (material.classic_color * atten_intensity * cos_ang_incidence) +
           (material.specular_color * atten_intensity * blinn_term);
}

vec4 calcDirLight(DirectionalLight light, vec3 surface_normal, vec3 view_dir) {
    vec3 light_dir = normalize(light.direction.xyz);

    float cos_ang_incidence = clamp(dot(surface_normal, light_dir), 0.0, 1.0);

    vec3 half_angle = normalize(light_dir + view_dir);
    float blinn_term = clamp(dot(surface_normal, half_angle), 0.0, 1.0);
    blinn_term = cos_ang_incidence != 0.0 ? blinn_term : 0.0;
    blinn_term = pow(blinn_term, material.shininess_factor);

    return (material.classic_color * light.intensity * cos_ang_incidence) +
           (material.specular_color * light.intensity * blinn_term);
}

void main() {
    vec3 surface_normal = normalize(camera_space_normal);
    vec3 view_dir = normalize(-camera_space_position);

    vec4 result = material.classic_color * ambient_intensity;

    for(int i = 0; i < point_light_count; i++)
        result += calcPointLight(point_lights[i], surface_normal, view_dir);

    for(int i = 0; i < dir_light_count; i++)
        result += calcDirLight(dir_lights[i], surface_normal, view_dir);

    result = result / max_intensity;
    vec4 gamma_vec = vec4(gamma);
    gamma_vec.w = 1.0;
    output_color = pow(result, gamma_vec);
}