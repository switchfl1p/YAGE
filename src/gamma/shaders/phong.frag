#version 330

uniform vec4 material_diffuse;
uniform vec4 ambient_intensity;
in vec3 camera_space_position;
in vec3 camera_space_normal;
const vec4 specular_color = vec4(0.25, 0.25, 0.25, 1.0);
uniform float shininess_factor;
out vec4 output_color;

struct PointLight {
    vec4 position;
    vec4 intensity;
    float attenuation;
};

struct DirectionalLight {
    vec4 direction;
    vec4 intensity;
};

#define MAX_POINT_LIGHTS 2
#define MAX_DIR_LIGHTS 2

layout(std140) uniform Lights {
    PointLight point_lights[MAX_POINT_LIGHTS];
    DirectionalLight dir_lights[MAX_DIR_LIGHTS];
    int point_light_count;
    int dir_light_count;
};

vec4 calcPointLight(PointLight light, vec3 surface_normal, vec3 view_dir) {
    vec3 light_dir = normalize(light.position.xyz - camera_space_position);
    float distance = length(light.position.xyz - camera_space_position);
    float atten = 1.0 / (1.0 + light.attenuation * distance);
    vec4 atten_intensity = atten * light.intensity;

    // diffuse
    float diff = max(dot(surface_normal, light_dir), 0.0);
    
    // specular (Phong)
    vec3 reflect_dir = reflect(-light_dir, surface_normal);
    float spec = 0.0;
    if(diff > 0.0) {
        spec = pow(max(dot(view_dir, reflect_dir), 0.0), shininess_factor);
    }

    return (material_diffuse * atten_intensity * diff) +
           (specular_color * atten_intensity * spec);
}

vec4 calcDirLight(DirectionalLight light, vec3 surface_normal, vec3 view_dir) {
    vec3 light_dir = normalize(-light.direction.xyz);

    // diffuse
    float diff = max(dot(surface_normal, light_dir), 0.0);

    // specular (Phong)
    vec3 reflect_dir = reflect(-light_dir, surface_normal);
    float spec = 0.0;
    if(diff > 0.0) {
        spec = pow(max(dot(view_dir, reflect_dir), 0.0), shininess_factor);
    }

    return (material_diffuse * light.intensity * diff) +
           (specular_color * light.intensity * spec);
}

void main() {
    vec3 N = normalize(camera_space_normal);
    vec3 V = normalize(-camera_space_position);

    vec4 result = material_diffuse * ambient_intensity;

    for(int i = 0; i < point_light_count; i++)
        result += calcPointLight(point_lights[i], N, V);

    for(int i = 0; i < dir_light_count; i++)
        result += calcDirLight(dir_lights[i], N, V);

    output_color = result;
}