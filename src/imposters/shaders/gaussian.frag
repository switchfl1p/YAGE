//Gaussian
#version 330
#include "light_common.glsl"

const vec4 specular_color = vec4(0.25, 0.25, 0.25, 1.0);

vec3 calcGaussianSpecular(vec3 light_dir, vec3 surface_normal, vec3 view_dir)
{
    vec3 half_angle = normalize(light_dir + view_dir);
    float angle_normal_half = acos(dot(half_angle, surface_normal));
    float exponent = angle_normal_half / material.shininess_factor;
    exponent = -(exponent * exponent);
    return vec3(exp(exponent));
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

        float gaussian_term = 0.0;
        if (cos_ang_incidence != 0.0)
            gaussian_term = calcGaussianSpecular(light_dir, surface_normal, view_dir).r;

        total_light += (material.classic_color * atten_intensity * cos_ang_incidence)
                     + (specular_color * atten_intensity * gaussian_term);
    }

    // --- Directional Lights ---
    for (int i = 0; i < dir_light_count; i++)
    {
        // Direction in light_common points *toward* the light (negate if stored as light-to-surface)
        vec3 light_dir = normalize(dir_lights[i].direction.xyz);
        vec4 intensity = dir_lights[i].intensity;

        float cos_ang_incidence = clamp(dot(surface_normal, light_dir), 0.0, 1.0);

        float gaussian_term = 0.0;
        if (cos_ang_incidence != 0.0)
            gaussian_term = calcGaussianSpecular(light_dir, surface_normal, view_dir).r;

        total_light += (material.classic_color * intensity * cos_ang_incidence)
                     + (specular_color * intensity * gaussian_term);
    }

    vec4 result = (total_light + (material.classic_color * ambient_intensity)) / max_intensity;
    vec4 gamma_vec = vec4(gamma);
    gamma_vec.w = 1.0;
    output_color = pow(result, gamma_vec);
}