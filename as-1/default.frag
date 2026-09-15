#version 330 core

in vec3 color;
in vec3 normal;
in vec3 fragPos;

out vec4 FragColor;

uniform vec3 colorInput;
uniform vec3 lightPos;


void main()
{
	vec3 lightDir    = normalize(lightPos - fragPos);
	float lambert    = dot(normal, lightDir);
	vec3 lightEnergy = vec3(clamp(lambert, 0.0, 1.0));
	
	FragColor = vec4(colorInput, 1.0) * vec4(lightEnergy, 1.0); 
}
