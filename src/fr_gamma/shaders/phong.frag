#version 330

#include "light_common.glsl"

vec4 calcPointLight(PointLight light, vec3 surface_normal, vec3 view_dir) {
    vec3 light_dir;
    float attenuation = calcAttenuation(light, light_dir);
    vec4 atten_intensity = attenuation * light.intensity;

    // diffuse
    float diff = max(dot(surface_normal, light_dir), 0.0);

    // specular (Phong)
    vec3 reflect_dir = reflect(-light_dir, surface_normal);
    float spec = 0.0;
    if(diff > 0.0) {
        spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess_factor);
    }

    return (material.classic_color * atten_intensity * diff) +
           (material.specular_color * atten_intensity * spec);
}

vec4 calcDirLight(DirectionalLight light, vec3 surface_normal, vec3 view_dir) {
    vec3 light_dir = normalize(light.direction.xyz);

    // diffuse
    float diff = max(dot(surface_normal, light_dir), 0.0);

    // specular (Phong)
    vec3 reflect_dir = reflect(-light_dir, surface_normal);
    float spec = 0.0;
    if(diff > 0.0) {
        spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess_factor);
    }

    return (material.classic_color * light.intensity * diff) +
           (material.specular_color * light.intensity * spec);
}

void main() {
    vec3 N = normalize(camera_space_normal);
    vec3 V = normalize(-camera_space_position);

    vec4 result = material.classic_color * ambient_intensity;

    for(int i = 0; i < point_light_count; i++)
        result += calcPointLight(point_lights[i], N, V);

    for(int i = 0; i < dir_light_count; i++)
        result += calcDirLight(dir_lights[i], N, V);

    result = result / max_intensity;
    vec4 gamma_vec = vec4(gamma);
    gamma_vec.w = 1.0;
    output_color = pow(result, gamma_vec);
}