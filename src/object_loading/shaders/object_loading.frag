#version 330

smooth in vec4 interp_color;

out vec4 output_color;

void main()
{
	output_color = interp_color;
}

