//PBR Terrain
#version 430 core
#include "pbr_common.glsl"
#include "terrain_common.glsl"

vec3 calcBRDF(vec3 N, vec3 V, vec3 L, vec3 radiance, vec3 F0, vec3 terrain_color)
{
    vec3 H = normalize(V + L);

    float NDF = DistributionGGX(N, H, roughness);
    float G   = GeometrySmith(N, V, L, roughness);
    vec3  F   = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;

    vec3 kS = F;
    vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);

    float NdotL = max(dot(N, L), 0.0);
    return (kD * terrain_color / PI + specular) * radiance * NdotL;
}

void main()
{
    vec3 terrain_color = getColor();
    vec3 N = normalize(camera_space_normal);
    vec3 V = normalize(-camera_space_position);

    vec3 F0 = mix(vec3(0.04), terrain_color, metallic);

    vec3 Lo = vec3(0.0);

    // --- Point Lights ---
    for (int i = 0; i < point_light_count; i++)
    {
        vec3 light_vec = point_lights[i].position.xyz - camera_space_position;
        float distance = length(light_vec);
        vec3 L = light_vec / distance;
        float attenuation = 1.0 / (1.0 + point_lights[i].attenuation * distance * distance);
        vec3 radiance = point_lights[i].intensity.rgb * attenuation;

        Lo += calcBRDF(N, V, L, radiance, F0, terrain_color);
    }

    // --- Directional Lights ---
    for (int i = 0; i < dir_light_count; i++)
    {
        vec3 L = normalize(dir_lights[i].direction.xyz);
        vec3 radiance = dir_lights[i].intensity.rgb;

        Lo += calcBRDF(N, V, L, radiance, F0, terrain_color);
    }

    // --- Ambient ---
    vec3 ambient = ambient_intensity.rgb * terrain_color;
    vec3 final_color = ambient + Lo;

    // Tone mapping
    final_color = final_color / (final_color + vec3(1.0));
    // Gamma correction
    final_color = pow(final_color, vec3(1.0/2.2));
    // Dithering
    float dither = (fract(sin(dot(gl_FragCoord.xy, vec2(12.9898, 78.233))) * 43758.5453) - 0.5) / 255.0;
    final_color += dither;

    output_color = vec4(final_color, 1.0);
}