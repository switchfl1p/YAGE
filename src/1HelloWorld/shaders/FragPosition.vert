#version 330

layout (location = 0) in vec4 position;
uniform float windowSize;
void main()
{
	gl_Position = position;
}
