//PBR
#version 430 core

uniform vec4 material_diffuse;
uniform vec4 light_intensity;
uniform vec4 ambient_intensity;
uniform vec3 camera_space_light_position;
uniform float light_attenuation;

in vec3 camera_space_position;
in vec3 camera_space_normal;

// PBR uniforms
uniform float metallic;
uniform float roughness;

out vec4 output_color;

const float PI = 3.14159265359;

in float terrain_height;
in vec2 world_xz;

uniform float amplitude;

//color lookup table
vec3 water = vec3(0.05, 0.25, 0.55);
vec3 sand  = vec3(0.85, 0.78, 0.45);
vec3 grass = vec3(0.2,  0.5,  0.15);
vec3 rock  = vec3(0.35, 0.30, 0.25);
vec3 snow  = vec3(0.95, 0.95, 1.0);

float tresholds[5] = float[](-0.31, -0.1, 0, 0.2, 0.4);
vec3 colors[5] = vec3[](water, sand, grass, rock, snow);

float smoothNoise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    f = f * f * (3.0 - 2.0 * f); // smoothstep interpolation
    
    float a = fract(sin(dot(i,                vec2(127.1, 311.7))) * 43758.5453);
    float b = fract(sin(dot(i + vec2(1,0),   vec2(127.1, 311.7))) * 43758.5453);
    float c = fract(sin(dot(i + vec2(0,1),   vec2(127.1, 311.7))) * 43758.5453);
    float d = fract(sin(dot(i + vec2(1,1),   vec2(127.1, 311.7))) * 43758.5453);
    
    return mix(mix(a, b, f.x), mix(c, d, f.x), f.y);
}

//height based colours
vec3 getColor(){
    float height = terrain_height / (amplitude * 0.71);

    float warp = (smoothNoise(world_xz * 0.7) - 0.5) * 0.15;
    height += warp;

    float blend_range = 0.02;

    vec3 color = colors[0];
    for (int i = 0; i < tresholds.length() - 1; i++) {
        float boundary = tresholds[i + 1];
        if (height < boundary + blend_range) {
            float t = smoothstep(boundary - blend_range, boundary + blend_range, height);
            color = mix(colors[i], colors[i + 1], t);
            break;
        }
    }

    if (height >= tresholds[tresholds.length() - 1]) {
        color = colors[tresholds.length() - 1];
    }

    return color;
}

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
    vec3 base_color = getColor();
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