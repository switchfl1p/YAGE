#version 330

layout(std140) uniform;

out vec2 mapping;

uniform Matrices {
    mat4 model_mat;
	mat4 view_mat;
	mat4 persp_mat;
}

uniform float sphere_radius;
uniform vec3 camera_sphere_pos;

void main() {
    vec2 offset;
    switch(gl_VertexID) {
    case 0:
        //Bottom-left
        mapping = vec2(-1.0, -1.0);
        offset = vec2(-sphere_radius, -sphere_radius);
        break;
    case 1:
        //Top-left
        mapping = vec2(-1.0, 1.0);
        offset = vec2(-sphere_radius, sphere_radius);
        break;
    case 2:
        //Bottom-right
        mapping = vec2(1.0, -1.0);
        offset = vec2(sphere_radius, -sphere_radius);
        break;
    case 3:
        //Top-right
        mapping = vec2(1.0, 1.0);
        offset = vec2(sphere_radius, sphere_radius);
        break;
    }

    vec4 camera_corner_pos = vec4(camera_sphere_pos, 1.0);
    camera_corner_pos.xy += offset;

    gl_Position = persp_mat * camera_corner_pos;
}