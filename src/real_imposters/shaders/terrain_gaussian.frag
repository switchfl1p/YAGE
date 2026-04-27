#version 330
#include "light_common.glsl"
#include "terrain_common.glsl"

vec3 calcGaussianSpecular(vec3 light_dir, vec3 surface_normal, vec3 view_dir)
{
    vec3 half_angle = normalize(light_dir + view_dir);
    float angle_normal_half = acos(dot(half_angle, surface_normal));
    float exponent = angle_normal_half / material.shininess_factor;
    exponent = -(exponent * exponent);
    return vec3(exp(exponent));
}

vec4 calcPointLight(PointLight light, vec3 surface_normal, vec3 view_dir, vec4 base_color) {
    vec3 light_dir;
    float attenuation = calcAttenuation(light, light_dir);
    vec4 atten_intensity = attenuation * light.intensity;

    float cos_ang_incidence = clamp(dot(surface_normal, light_dir), 0.0, 1.0);

    float gaussian_term = 0.0;

    if (cos_ang_incidence != 0.0) {
        gaussian_term = calcGaussianSpecular(light_dir, surface_normal, view_dir).r;
    }

    return (base_color * atten_intensity * cos_ang_incidence)
    + (material.specular_color * atten_intensity * gaussian_term);
}

vec4 calcDirLight(DirectionalLight light, vec3 surface_normal, vec3 view_dir, vec4 base_color) {
    // Direction in light_common points *toward* the light (negate if stored as light-to-surface)
    vec3 light_dir = normalize(light.direction.xyz);

    float cos_ang_incidence = clamp(dot(surface_normal, light_dir), 0.0, 1.0);

    float gaussian_term = 0.0;
    if (cos_ang_incidence != 0.0) {
        gaussian_term = calcGaussianSpecular(light_dir, surface_normal, view_dir).r;
    }

    return (base_color * light.intensity * cos_ang_incidence)
    + (material.specular_color * light.intensity * gaussian_term);
}

void main() {
    vec4 base_color = vec4(getColor(), 1.0);
    vec3 surface_normal = normalize(camera_space_normal);
    vec3 view_dir = normalize(-camera_space_position);
    vec4 total_light = base_color * ambient_intensity;

    for (int i = 0; i < point_light_count; i++) {
        total_light += calcPointLight(point_lights[i], surface_normal, view_dir, base_color);
    }

    for (int i = 0; i < dir_light_count; i++) {
        total_light += calcDirLight(dir_lights[i], surface_normal, view_dir, base_color);
    }

    total_light /= max_intensity;
    vec4 gamma_vec = vec4(gamma);
    gamma_vec.w = 1.0;
    output_color = pow(total_light, gamma_vec);
}