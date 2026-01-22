#version 330

in vec4 diffuse_color;
in vec3 vertex_normal;
in vec3 model_space_position;

out vec4 output_color;

uniform vec3 model_space_light_position;
uniform vec4 light_intensity;
uniform vec4 ambient_intensity;

void main(){
    vec3 light_dir = normalize(model_space_light_position - model_space_position);

    float cos_ang_incidence = dot(normalize(vertex_normal), light_dir);
    cos_ang_incidence = clamp(cos_ang_incidence, 0, 1);

    output_color = (diffuse_color * light_intensity * cos_ang_incidence) + (diffuse_color * ambient_intensity);
}