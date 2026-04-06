#version 330

in vec2 mapping;

out vec4 output_color;

uniform float sphere_radius;
uniform vec3 camera_sphere_pos;

layout(std140) uniform;

uniform 