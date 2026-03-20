// Blinn-Phong Terrain
#version 330

#include "light_common.glsl"
#include "terrain_common.glsl"

const vec4 specular_color = vec4(0.02, 0.02, 0.02, 1.0);

void main()
{
    vec3 surface_normal = normalize(camera_space_normal);
    vec3 view_dir = normalize(-camera_space_position);

    vec4 base_color = vec4(getColor(),1.0);

    vec4 diffuse_accum = vec4(0.0);
    vec4 spec_accum = vec4(0.0);

    // POINT LIGHTS
    for(int i = 0; i < point_light_count; i++)
    {
        vec3 light_dir;
        float attenuation = calcAttenuation(point_lights[i], light_dir);
        vec4 atten_intensity = attenuation * point_lights[i].intensity;

        float NdotL = max(dot(surface_normal, light_dir), 0.0);

        vec3 half_vec = normalize(light_dir + view_dir);
        float spec = pow(max(dot(surface_normal, half_vec),0.0), shininess_factor);
        spec = NdotL > 0.0 ? spec : 0.0;

        diffuse_accum += base_color * point_lights[i].intensity * NdotL * attenuation;
        spec_accum += specular_color * point_lights[i].intensity * spec * attenuation;
    }

    // DIRECTIONAL LIGHTS
    for(int i = 0; i < dir_light_count; i++)
    {
        vec3 light_dir = normalize(dir_lights[i].direction.xyz);

        float NdotL = max(dot(surface_normal, light_dir), 0.0);

        vec3 half_vec = normalize(light_dir + view_dir);
        float spec = pow(max(dot(surface_normal, half_vec),0.0), shininess_factor);
        spec = NdotL > 0.0 ? spec : 0.0;

        diffuse_accum += base_color * dir_lights[i].intensity * NdotL;
        spec_accum += specular_color * dir_lights[i].intensity * spec;
    }

    vec4 ambient = base_color * ambient_intensity;

    output_color = diffuse_accum + spec_accum + ambient;
}