//Lambertian

#version 330

uniform vec4 light_intensity;
uniform vec4 ambient_intensity;
uniform vec3 camera_space_light_position;
uniform float light_attenuation;

in vec3 camera_space_position;
in vec3 camera_space_normal;

out vec4 output_color;

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