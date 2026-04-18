//PBR
#version 430 core
#include "pbr_common.glsl"

vec3 calcBRDF(vec3 N, vec3 V, vec3 L, vec3 radiance, vec3 F0)
{
    vec3 H = normalize(V + L);

    float NDF = DistributionGGX(N, H, material.roughness);
    float G   = GeometrySmith(N, V, L, material.roughness);
    vec3  F   = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 numerator   = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;

    vec3 kS = F;
    vec3 kD = (vec3(1.0) - kS) * (1.0 - material.metallic);

    float NdotL = max(dot(N, L), 0.0);
    return (kD * material.pbr_color.rgb / PI + specular) * radiance * NdotL;
}

vec3 calcPointLight(PointLight light, vec3 surface_normal, vec3 view_dir, vec3 F0) {
    vec3 light_vec = light.position.xyz - camera_space_position;
    float distance = length(light_vec);
    vec3 L = light_vec / distance;
    float attenuation = 1.0 / (1.0 + light.attenuation * distance * distance);
    vec3 radiance = light.intensity.rgb * attenuation;

    return calcBRDF(surface_normal, view_dir, L, radiance, F0);
}

vec3 calcDirLight(DirectionalLight light, vec3 surface_normal, vec3 view_dir, vec3 F0) {
    vec3 L = normalize(light.direction.xyz);
    vec3 radiance = light.intensity.rgb; //no attenuation

    return calcBRDF(surface_normal, view_dir, L, radiance, F0);
}

void main() {
    vec3 surface_normal = normalize(camera_space_normal);
    vec3 view_dir = normalize(-camera_space_position);
    vec3 F0 = mix(vec3(0.04), material.pbr_color.rgb, material.metallic);
    vec3 total_light = ambient_intensity.rgb * material.pbr_color.rgb;

    for (int i = 0; i < point_light_count; i++) {
        total_light += calcPointLight(point_lights[i], surface_normal, view_dir, F0);
    }

    for (int i = 0; i < dir_light_count; i++) {
        total_light += calcDirLight(dir_lights[i], surface_normal, view_dir, F0);
    }

    //tone mapping
    total_light /= (total_light + vec3(1.0));

    //gamma correction
    vec3 gamma_vec = vec3(gamma);
    total_light = pow(total_light, gamma_vec);

    //dithering
    float dither = (fract(sin(dot(gl_FragCoord.xy, vec2(12.9898, 78.233))) * 43758.5453) - 0.5) / 255.0;
    total_light += dither;

    output_color = vec4(total_light, 1.0);

}