#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTex;
layout (location = 3) in vec3 aNorm;


// Outputs the color for the Fragment Shader
out vec3 color;
out vec3 normal;
out vec3 fragPos;

// Controls the scale of the vertices
uniform float scale;

// Inputs the matrices needed for 3D viewing with perspective
uniform mat4 model; 
uniform mat4 view;
uniform mat4 proj;

void main()
{
	gl_Position = proj * view * model * vec4(aPos, 1.0); // Outputs the positions/coordinates of all vertices
	color = aColor;     // Assigns the colors from the Vertex Data to "color"
	normal = vec3(model * vec4(aNorm, 0.0));
	fragPos = vec3(model * vec4(aPos, 1.0));
}

