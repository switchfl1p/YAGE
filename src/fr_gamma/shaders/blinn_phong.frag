#version 330

#include "light_common.glsl"

const vec4 specular_color = vec4(0.25, 0.25, 0.25, 1.0);

vec4 calcPointLight(PointLight light, vec3 surface_normal, vec3 view_dir) {
    vec3 light_difference = light.position.xyz - camera_space_position;
    float light_dist_sq = dot(light_difference, light_difference);
    float light_dist = sqrt(light_dist_sq);
    vec3 light_dir = light_difference / light_dist;

    float atten = 1.0 / (1.0 + light.attenuation * light_dist);
    vec4 atten_intensity = atten * light.intensity;

    float cos_ang_incidence = clamp(dot(surface_normal, light_dir), 0.0, 1.0);

    vec3 half_angle = normalize(light_dir + view_dir);
    float blinn_term = clamp(dot(surface_normal, half_angle), 0.0, 1.0);
    blinn_term = cos_ang_incidence != 0.0 ? blinn_term : 0.0;
    blinn_term = pow(blinn_term, shininess_factor);

    return (material_diffuse * atten_intensity * cos_ang_incidence) +
           (specular_color * atten_intensity * blinn_term);
}

vec4 calcDirLight(DirectionalLight light, vec3 surface_normal, vec3 view_dir) {
    vec3 light_dir = normalize(light.direction.xyz);

    float cos_ang_incidence = clamp(dot(surface_normal, light_dir), 0.0, 1.0);

    vec3 half_angle = normalize(light_dir + view_dir);
    float blinn_term = clamp(dot(surface_normal, half_angle), 0.0, 1.0);
    blinn_term = cos_ang_incidence != 0.0 ? blinn_term : 0.0;
    blinn_term = pow(blinn_term, shininess_factor);

    return (material_diffuse * light.intensity * cos_ang_incidence) +
           (specular_color * light.intensity * blinn_term);
}

void main() {
    vec3 surface_normal = normalize(camera_space_normal);
    vec3 view_dir = normalize(-camera_space_position);

    vec4 result = material_diffuse * ambient_intensity;

    for(int i = 0; i < point_light_count; i++)
        result += calcPointLight(point_lights[i], surface_normal, view_dir);

    for(int i = 0; i < dir_light_count; i++)
        result += calcDirLight(dir_lights[i], surface_normal, view_dir);

    output_color = result;
}