#version 330

#include "light_common.glsl"

in vec2 mapping;

uniform float sphere_radius;
uniform vec3 camera_sphere_pos;

uniform Matrices {
    mat4 model_mat;
	mat4 view_mat;
	mat4 persp_mat;
}

vec4 computeLighting(in PointLight light, in vec3 camera_space_pos, in vec3 cam_space_normal) {
    vec3 light_dir;
    vec4 light_intensity;

    if (light.position.w == 0.0) {
        light_dir = vec3(light.position);
        light_intensity = light.intensity;
    } else {
        float atten = calcAttenuation(light, light_dir);
        light_intensity = atten * light.intensity;
    }

    vec3 surface_normal = normalize(camera_space_normal);
    float cos_ang_incidence = dot(surface_normal, light_dir);
    cos_ang_incidence = cos_ang_incidence < 0.0001 ? 0.0 : cos_ang_incidence;

    vec3 view_direction = normalize(-camera_space_pos);

    vec3 half_angle = normalize(light_dir);
    float angle_normal_half = acos(dor(half_angle, surface_normal));
    float exponent = angle_normal_half / material.shininess_factor;
    exponent = -(exponent * exponent);
    float gaussian_term = exp(exponent);

    gaussian_term = cos_ang_incidence != 0.0 ? gaussian_term : 0.0;

    vec4 lighting = material.classic_color * light_intensity * cos_ang_incidence;
    lighting += material.specular_color * light_intensity * gaussian_term;

    return lighting;
}