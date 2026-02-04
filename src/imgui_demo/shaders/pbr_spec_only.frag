//PBR specular only

#version 430 core

uniform vec4 material_diffuse;
uniform vec4 light_intensity;
uniform vec4 ambient_intensity;
uniform vec3 camera_space_light_position;
uniform float light_attenuation;
in vec3 camera_space_position;
in vec3 camera_space_normal;
// PBR uniforms
uniform vec4 base_color;
uniform float metallic;
uniform float roughness;
out vec4 output_color;
const float PI = 3.14159265359;

// Normal Distribution Function (GGX/Trowbridge-Reitz)
float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    return a2 / denom;
}

// Geometry Function (Schlick-GGX)
float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    float denom = NdotV * (1.0 - k) + k;
    return NdotV / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, roughness);
    float ggx2 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

// Fresnel (Schlick approximation)
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main() {
    vec3 N = normalize(camera_space_normal);
    vec3 V = normalize(-camera_space_position);
    
    // Light calculations
    vec3 L = normalize(camera_space_light_position - camera_space_position);
    vec3 H = normalize(V + L);
    
    float distance = length(camera_space_light_position - camera_space_position);
    float attenuation = 1.0 / (1.0 + light_attenuation * distance * distance);
    vec3 radiance = light_intensity.rgb * attenuation;
    
    // Calculate reflectance at normal incidence
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, base_color.rgb, metallic);
    
    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
    
    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;
    
    float NdotL = max(dot(N, L), 0.0);
    
    // SPECULAR ONLY (no diffuse term)
    vec3 Lo = specular * radiance * NdotL;
    
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