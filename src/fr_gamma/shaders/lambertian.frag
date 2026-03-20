//Lambertian
#version 330
#include "light_common.glsl"

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

        total_light += material_diffuse * atten_intensity * cos_ang_incidence;
    }

    // --- Directional Lights ---
    for (int i = 0; i < dir_light_count; i++)
    {
        vec3 light_dir = normalize(dir_lights[i].direction.xyz);
        vec4 intensity = dir_lights[i].intensity;

        float cos_ang_incidence = clamp(dot(surface_normal, light_dir), 0.0, 1.0);

        total_light += material_diffuse * intensity * cos_ang_incidence;
    }

    // --- Ambient ---
    output_color = total_light + (material_diffuse * ambient_intensity);
}