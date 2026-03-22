in vec3 camera_space_position;
in vec3 camera_space_normal;

// PBR uniforms
uniform float metallic;
uniform float roughness;
uniform vec4 base_color;

out vec4 output_color;

const float PI = 3.14159265359;

struct PointLight{
	vec4 position;
	vec4 intensity;
	float attenuation;
};

struct DirectionalLight{
	vec4 direction;
	vec4 intensity;
};

#define MAX_POINT_LIGHTS 3
#define MAX_DIR_LIGHTS 1

layout(std140) uniform Lights{
	PointLight point_lights[MAX_POINT_LIGHTS];
	DirectionalLight dir_lights[MAX_DIR_LIGHTS];
    vec4 ambient_intensity;
	int point_light_count;
	int dir_light_count;
    float max_intensity;
	float gamma;
};


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