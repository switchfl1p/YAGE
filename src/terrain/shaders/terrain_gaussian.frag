//Gaussian

#version 330

uniform vec4 light_intensity;
uniform vec4 ambient_intensity;
uniform vec3 camera_space_light_position;
uniform float light_attenuation;

in vec3 camera_space_position;
in vec3 camera_space_normal;

const vec4 specular_color = vec4(0.25, 0.25, 0.25, 1.0);
uniform float shininess_factor;

out vec4 output_color;

in float terrain_height;
uniform float amplitude;

//color lookup table
vec3 water = vec3(0.05, 0.25, 0.55);
vec3 sand  = vec3(0.85, 0.78, 0.45);
vec3 grass = vec3(0.2,  0.5,  0.15);
vec3 rock  = vec3(0.35, 0.30, 0.25);
vec3 snow  = vec3(0.95, 0.95, 1.0);

float tresholds[5] = float[](-0.31, -0.1, 0, 0.2, 0.4);
vec3 colors[5] = vec3[](water, sand, grass, rock, snow);

//height based colours
vec3 getColor(){
    float height = terrain_height / (amplitude * 0.71);
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

float calcAttenuation(in vec3 camera_space_pos, out vec3 light_dir)
{
    vec3 light_difference = camera_space_light_position - camera_space_pos;
    float light_distance_sqrt = dot(light_difference, light_difference);
    light_dir = light_difference * inversesqrt(light_distance_sqrt);

    return (1 / (1.0 + light_attenuation * sqrt(light_distance_sqrt)));
}

void main()
{
    vec4 color = vec4(getColor(), 1.0);
    vec3 light_dir = vec3(0.0);
    float atten = calcAttenuation(camera_space_position, light_dir);
    vec4 atten_intensity = atten * light_intensity;

    vec3 surface_normal = normalize(camera_space_normal);
    float cos_ang_incidence = dot(surface_normal, light_dir);
    cos_ang_incidence = clamp(cos_ang_incidence, 0, 1);

    vec3 view_dir = normalize(-camera_space_position);

    vec3 half_angle = normalize(light_dir + view_dir);
    float angle_normal_half = acos(dot(half_angle,surface_normal));
    float exponent = angle_normal_half / shininess_factor;
    exponent = -(exponent * exponent);
    float gaussian_term = exp(exponent);

    gaussian_term = cos_ang_incidence != 0.0 ? gaussian_term : 0.0;

    output_color = (color * atten_intensity * cos_ang_incidence) + 
        (specular_color * atten_intensity * gaussian_term) + 
        (color * ambient_intensity);

}