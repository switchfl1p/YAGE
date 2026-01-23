#version 330

in vec4 diffuse_color;
in vec3 camera_space_normal;
in vec3 model_space_position;

out vec4 output_color;

uniform vec3 camera_space_light_position;
uniform vec4 light_intensity;
uniform vec4 ambient_intensity;

uniform float light_attenuation;

uniform mat4 clip_to_camera_mat;
uniform ivec2 window_size;

vec3 calcCameraSpacePosition()
{
    vec4 ndc_pos;
    ndc_pos.xy = ((gl_FragCoord.xy / window_size.xy) * 2.0) - 1.0;
    ndc_pos.z = (2.0 * gl_FragCoord.z - gl_DepthRange.near - gl_DepthRange.far) /
        (gl_DepthRange.far - gl_DepthRange.near);
    ndc_pos.w = 1.0;
    
    vec4 clip_pos = ndc_pos / gl_FragCoord.w;
    
    return vec3(clip_to_camera_mat * clip_pos);
}

vec4 applyLightIntensity(in vec3 camera_space_position, out vec3 light_direction)
{
    vec3 light_difference =  camera_space_light_position - camera_space_position;
    float light_distance_sqr = dot(light_difference, light_difference);
    light_direction = light_difference * inversesqrt(light_distance_sqr);
    
    float dist_factor = light_distance_sqr;
    
    return light_intensity * (1 / ( 1.0 + light_attenuation * light_distance_sqr));
}

void main(){

    vec3 camera_space_position = calcCameraSpacePosition();

    vec3 light_dir = vec3(0.0);
    vec4 atten_intensity = applyLightIntensity(camera_space_position, light_dir);

    float cos_ang_incidence = dot(normalize(camera_space_normal), light_dir);
    cos_ang_incidence = clamp(cos_ang_incidence, 0, 1);

    output_color = (diffuse_color * atten_intensity * cos_ang_incidence) + (diffuse_color * ambient_intensity);
}