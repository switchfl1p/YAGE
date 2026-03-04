//Lambertian
#version 330
#include "terrain_common.glsl"

uniform vec4 light_intensity;
uniform vec4 ambient_intensity;
uniform vec3 camera_space_light_position;
uniform float light_attenuation;

in vec3 camera_space_position;
in vec3 camera_space_normal;

out vec4 output_color;

vec4 applyLightIntensity(in vec3 camera_space_position, out vec3 light_direction)
{
    vec3 light_difference =  camera_space_light_position - camera_space_position;
    float light_distance_sqr = dot(light_difference, light_difference);
    light_direction = light_difference * inversesqrt(light_distance_sqr);
    
    float dist_factor = light_distance_sqr;
    
    return light_intensity * (1 / ( 1.0 + light_attenuation * dist_factor));
}

void main(){

    vec4 color = vec4(getColor(), 1.0);
    vec3 light_dir = vec3(0.0);
    vec4 atten_intensity = applyLightIntensity(camera_space_position, light_dir);

    float cos_ang_incidence = dot(normalize(camera_space_normal), light_dir);
    cos_ang_incidence = clamp(cos_ang_incidence, 0, 1);

    output_color = (color * atten_intensity * cos_ang_incidence) + (color * ambient_intensity);
}