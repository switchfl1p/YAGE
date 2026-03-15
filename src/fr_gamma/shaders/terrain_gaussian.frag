//Gaussian Terrain
#version 330
#include "light_common.glsl"
#include "terrain_common.glsl"

const vec4 specular_color = vec4(0.02, 0.02, 0.02, 1.0);

vec3 calcGaussianSpecular(vec3 light_dir, vec3 surface_normal, vec3 view_dir)
{
    vec3 half_angle = normalize(light_dir + view_dir);
    float angle_normal_half = acos(dot(half_angle, surface_normal));
    float exponent = angle_normal_half / shininess_factor;
    exponent = -(exponent * exponent);
    return vec3(exp(exponent));
}

void main()
{
    vec4 color = vec4(getColor(), 1.0);
    vec3 surface_normal = normalize(camera_space_normal);
    vec3 view_dir = normalize(-camera_space_position);
    vec4 total_light = vec4(0.0);

    // --- Point Lights ---
    for (int i = 0; i < point_light_count; i++)
    {
        vec3 light_difference = point_lights[i].position.xyz - camera_space_position;
        float light_dist_sqrt = dot(light_difference, light_difference);
        vec3 light_dir = light_difference * inversesqrt(light_dist_sqrt);
        float atten = 1.0 / (1.0 + point_lights[i].attenuation * sqrt(light_dist_sqrt));
        vec4 atten_intensity = atten * point_lights[i].intensity;

        float cos_ang_incidence = clamp(dot(surface_normal, light_dir), 0.0, 1.0);

        float gaussian_term = 0.0;
        if (cos_ang_incidence != 0.0)
            gaussian_term = calcGaussianSpecular(light_dir, surface_normal, view_dir).r;

        total_light += (color * atten_intensity * cos_ang_incidence)
                     + (specular_color * atten_intensity * gaussian_term);
    }

    // --- Directional Lights ---
    for (int i = 0; i < dir_light_count; i++)
    {
        vec3 light_dir = normalize(dir_lights[i].direction.xyz); // flip to point toward light
        vec4 intensity = dir_lights[i].intensity;

        float cos_ang_incidence = clamp(dot(surface_normal, light_dir), 0.0, 1.0);

        float gaussian_term = 0.0;
        if (cos_ang_incidence != 0.0)
            gaussian_term = calcGaussianSpecular(light_dir, surface_normal, view_dir).r;

        total_light += (color * intensity * cos_ang_incidence)
                     + (specular_color * intensity * gaussian_term);
    }

    // --- Ambient ---
    output_color = total_light + (color * ambient_intensity);
}