//Lambertian
#version 330

#include "light_common.glsl"

vec4 calcPointLight(PointLight light, vec3 surface_normal, vec3 view_dir) {
    vec3 light_dir;
    float attenuation = calcAttenuation(light, light_dir);
    vec4 atten_intensity = attenuation * light.intensity;

    float cos_ang_incidence = clamp(dot(surface_normal, light_dir), 0.0, 1.0);

    return material.classic_color * atten_intensity * cos_ang_incidence;
}

vec4 calcDirLight(DirectionalLight light, vec3 surface_normal, vec3 view_dir) {
    vec3 light_dir = normalize(light.direction.xyz);

    float cos_ang_incidence = clamp(dot(surface_normal, light_dir), 0.0, 1.0);

    return material.classic_color * light.intensity * cos_ang_incidence;
}

void main() {
    vec3 surface_normal = normalize(camera_space_normal);
    vec3 view_dir = normalize(-camera_space_position);

    vec4 result = material.classic_color * ambient_intensity;

    for (int i = 0; i < point_light_count; i++) {
        result += calcPointLight(point_lights[i], surface_normal, view_dir);
    }

    for (int i = 0; i < dir_light_count; i++) {
        result += calcDirLight(dir_lights[i], surface_normal, view_dir);
    }

    result = result / max_intensity;
    vec4 gamma_vec = vec4(gamma);
    gamma_vec.w = 1.0;

    output_color = pow(result, gamma_vec);  
}