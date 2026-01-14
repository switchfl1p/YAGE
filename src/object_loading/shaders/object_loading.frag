#version 330

smooth in vec3 frag_pos;
out vec4 frag_color;

void main()
{
	vec3 color = (frag_pos + 1.0) * 0.5;
	frag_color = vec4(color, 1.0);
}
