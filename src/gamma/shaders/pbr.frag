//PBR
#version 430 core
#include "pbr_common.glsl"

void main() {
    vec3 N = normalize(camera_space_normal);
    vec3 V = normalize(-camera_space_position);  // view direction (camera at origin in view space)
    
    // Light calculations
    vec3 L = normalize(camera_space_light_position - camera_space_position);
    vec3 H = normalize(V + L);
    
    float distance = length(camera_space_light_position - camera_space_position);
    float attenuation = 1.0 / (1.0 + light_attenuation * distance * distance);
    vec3 radiance = light_intensity.rgb * attenuation;
    
    // Calculate reflectance at normal incidence
    // For dielectrics, F0 is around 0.04; for metals, use the base color
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, base_color.rgb, metallic);
    
    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
    
    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;
    
    // Energy conservation
    vec3 kS = F;  // specular contribution
    vec3 kD = vec3(1.0) - kS;  // diffuse contribution
    kD *= 1.0 - metallic;  // metals have no diffuse
    
    float NdotL = max(dot(N, L), 0.0);
    vec3 Lo = (kD * base_color.rgb / PI + specular) * radiance * NdotL;
    
    // Ambient
    vec3 ambient = ambient_intensity.rgb * base_color.rgb;
    
    vec3 final_color = ambient + Lo;
    
    // Tone mapping
    final_color = final_color / (final_color + vec3(1.0));
    
    // Gamma correction
    final_color = pow(final_color, vec3(1.0/2.2));

    float dither = (fract(sin(dot(gl_FragCoord.xy, vec2(12.9898, 78.233))) * 43758.5453) - 0.5) / 255.0;
    final_color += dither;
    
    output_color = vec4(final_color, 1.0);
}