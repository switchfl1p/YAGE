#version 330

uniform vec4 material_diffuse;

out vec4 output_color;

void main()
{
	output_color = material_diffuse;
}



