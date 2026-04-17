//Lambertian
#version 330

#include "light_common.glsl"

vec4 calcPointLight(PointLight light, vec3 surface_normal, vec3 view_dir) {
    vec3 light_dir;
    float attenuation = calcAttenuation(light, light_dir);
    vec4 atten_intensity = attenuation * light.intensity;

}

void main() {
    vec3 surface_normal = normalize(cam_space_normal);
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

void main()
{
    vec3 surface_normal = normalize(camera_space_normal);
    vec3 view_dir = normalize(-camera_space_position);
    vec4 total_light = vec4(0.0);

    // --- Point Lights ---
    for (int i = 0; i < point_light_count; i++)
    {
        vec3 light_dir;
        float attenuation = calcAttenuation(point_lights[i], light_dir);
        vec4 atten_intensity = attenuation * point_lights[i].intensity;

        float cos_ang_incidence = clamp(dot(surface_normal, light_dir), 0.0, 1.0);

        total_light += material.classic_color * atten_intensity * cos_ang_incidence;
    }

    // --- Directional Lights ---
    for (int i = 0; i < dir_light_count; i++)
    {
        vec3 light_dir = normalize(dir_lights[i].direction.xyz);
        vec4 intensity = dir_lights[i].intensity;

        float cos_ang_incidence = clamp(dot(surface_normal, light_dir), 0.0, 1.0);

        total_light += material.classic_color * intensity * cos_ang_incidence;
    }

    // --- Ambient ---
    vec4 result = (total_light + (material.classic_color * ambient_intensity)) / max_intensity;
    vec4 gamma_vec = vec4(gamma);
    gamma_vec.w = 1.0;
    output_color = pow(result, gamma_vec);
}