#version 330 core
//----------------------------------------------------------------------------------------------------------------------
// INPUTS <- CPU
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTex;
layout (location = 3) in vec3 aNorm;
//----------------------------------------------------------------------------------------------------------------------
// UNIFORMS
uniform mat4 model; 
uniform mat4 view;
uniform mat4 proj;
//----------------------------------------------------------------------------------------------------------------------
// OUTPUTS -> fragment shader
out vec3 color;
out vec3 normal;
out vec3 fragPos;
//======================================================================================================================
void main()
{
	gl_Position = proj * view * model * vec4(aPos, 1.0); // Calculate final position
	color = aColor;										 // Pass through color
	normal = vec3(model * vec4(aNorm, 0.0));			 // Calculate normal
	fragPos = vec3(model * vec4(aPos, 1.0));			 // Calculate fragment position
}
//----------------------------------------------------------------------------------------------------------------------

