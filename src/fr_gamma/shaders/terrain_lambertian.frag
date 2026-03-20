//Lambertian Terrain
#version 330
#include "light_common.glsl"
#include "terrain_common.glsl"

void main()
{
    vec4 color = vec4(getColor(), 1.0);
    vec3 surface_normal = normalize(camera_space_normal);
    vec4 total_light = vec4(0.0);

    // --- Point Lights ---
    for (int i = 0; i < point_light_count; i++)
    {
        vec3 light_dir;
        float attenuation = calcAttenuation(point_lights[i], light_dir);
        vec4 atten_intensity = attenuation * point_lights[i].intensity;

        float cos_ang_incidence = clamp(dot(surface_normal, light_dir), 0.0, 1.0);

        total_light += color * atten_intensity * cos_ang_incidence;
    }

    // --- Directional Lights ---
    for (int i = 0; i < dir_light_count; i++)
    {
        vec3 light_dir = normalize(dir_lights[i].direction.xyz);
        vec4 intensity = dir_lights[i].intensity;

        float cos_ang_incidence = clamp(dot(surface_normal, light_dir), 0.0, 1.0);

        total_light += color * intensity * cos_ang_incidence;
    }

    // --- Ambient ---
    output_color = (total_light + (color * ambient_intensity)) / max_intensity;
}