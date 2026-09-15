#version 330 core

in vec3 color;
out vec4 FragColor;

uniform vec3 colorInput;


void main()
{
	FragColor = vec4(colorInput, 1.0);
}
