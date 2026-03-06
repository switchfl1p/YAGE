// Blinn-Phong Terrain
#version 330

in vec3 camera_space_position;
in vec3 camera_space_normal;

in float terrain_height;
in vec2 world_xz;

out vec4 output_color;

uniform vec4 ambient_intensity;
uniform float shininess_factor;
uniform float amplitude;

const vec4 specular_color = vec4(0.02, 0.02, 0.02, 1.0);

struct PointLight{
    vec4 position;   // camera space
    vec4 intensity;
    float attenuation;
};

struct DirectionalLight{
    vec4 direction;  // camera space
    vec4 intensity;
};

#define MAX_POINT_LIGHTS 2
#define MAX_DIR_LIGHTS 2

layout(std140) uniform Lights{
    PointLight point_lights[MAX_POINT_LIGHTS];
    DirectionalLight dir_lights[MAX_DIR_LIGHTS];
    int point_light_count;
    int dir_light_count;
};

////////////////////////////////////////////////////////////
// Terrain Color
////////////////////////////////////////////////////////////

vec3 water = vec3(0.05, 0.25, 0.55);
vec3 sand  = vec3(0.85, 0.78, 0.45);
vec3 grass = vec3(0.2,  0.5,  0.15);
vec3 rock  = vec3(0.35, 0.30, 0.25);
vec3 snow  = vec3(0.95, 0.95, 1.0);

float thresholds[5] = float[](-0.31, -0.1, 0.0, 0.2, 0.4);
vec3 colors[5] = vec3[](water, sand, grass, rock, snow);

float smoothNoise(vec2 p)
{
    vec2 i = floor(p);
    vec2 f = fract(p);
    f = f * f * (3.0 - 2.0 * f);

    float a = fract(sin(dot(i,              vec2(127.1,311.7))) * 43758.5453);
    float b = fract(sin(dot(i + vec2(1,0),  vec2(127.1,311.7))) * 43758.5453);
    float c = fract(sin(dot(i + vec2(0,1),  vec2(127.1,311.7))) * 43758.5453);
    float d = fract(sin(dot(i + vec2(1,1),  vec2(127.1,311.7))) * 43758.5453);

    return mix(mix(a,b,f.x), mix(c,d,f.x), f.y);
}

vec3 getColor()
{
    float height = terrain_height / (amplitude * 0.71);

    float warp = (smoothNoise(world_xz * 0.7) - 0.5) * 0.15;
    height += warp;

    float blend_range = 0.02;

    vec3 color = colors[0];

    for(int i = 0; i < 4; i++)
    {
        float boundary = thresholds[i + 1];
        if(height < boundary + blend_range)
        {
            float t = smoothstep(boundary - blend_range, boundary + blend_range, height);
            color = mix(colors[i], colors[i+1], t);
            break;
        }
    }

    if(height >= thresholds[4])
        color = colors[4];

    return color;
}

////////////////////////////////////////////////////////////
// Lighting
////////////////////////////////////////////////////////////

void main()
{
    vec3 surface_normal = normalize(camera_space_normal);
    vec3 view_dir = normalize(-camera_space_position);

    vec4 base_color = vec4(getColor(),1.0);

    vec4 diffuse_accum = vec4(0.0);
    vec4 spec_accum = vec4(0.0);

    // POINT LIGHTS
    for(int i = 0; i < point_light_count; i++)
    {
        vec3 light_vec = point_lights[i].position.xyz - camera_space_position;
        float dist2 = dot(light_vec, light_vec);

        vec3 light_dir = light_vec * inversesqrt(dist2);
        float dist = sqrt(dist2);

        float attenuation = 1.0 / (1.0 + point_lights[i].attenuation * dist);

        float NdotL = max(dot(surface_normal, light_dir), 0.0);

        vec3 half_vec = normalize(light_dir + view_dir);
        float spec = pow(max(dot(surface_normal, half_vec),0.0), shininess_factor);
        spec = NdotL > 0.0 ? spec : 0.0;

        diffuse_accum += base_color * point_lights[i].intensity * NdotL * attenuation;
        spec_accum += specular_color * point_lights[i].intensity * spec * attenuation;
    }

    // DIRECTIONAL LIGHTS
    for(int i = 0; i < dir_light_count; i++)
    {
        vec3 light_dir = normalize(-dir_lights[i].direction.xyz);

        float NdotL = max(dot(surface_normal, light_dir), 0.0);

        vec3 half_vec = normalize(light_dir + view_dir);
        float spec = pow(max(dot(surface_normal, half_vec),0.0), shininess_factor);
        spec = NdotL > 0.0 ? spec : 0.0;

        diffuse_accum += base_color * dir_lights[i].intensity * NdotL;
        spec_accum += specular_color * dir_lights[i].intensity * spec;
    }

    vec4 ambient = base_color * ambient_intensity;

    output_color = diffuse_accum + spec_accum + ambient;
}