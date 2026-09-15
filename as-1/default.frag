#version 330 core
//----------------------------------------------------------------------------------------------------------------------
// INPUTS <- Vertex shader
in vec3 color;
in vec3 normal;
in vec3 fragPos;
//----------------------------------------------------------------------------------------------------------------------
// UNIFORMS
uniform vec3 colorInput;
uniform vec3 lightPos;
uniform bool isEmissive;
//----------------------------------------------------------------------------------------------------------------------
// OUTPUTS
out vec4 FragColor;

//======================================================================================================================
void main()
{
	if (isEmissive) {
		FragColor = vec4(colorInput, 1.0); // If mesh is emmisive output white
		return;
	}

	vec3 lightDir    = normalize(lightPos - fragPos);  // Calculate light direction vector
	float lambert    = dot(normal, lightDir);          // Calculate lambert coefficient
	lambert         *= 1.1;							   // A lil brightness boost (still needs gamma correction)
	vec3 lightEnergy = vec3(clamp(lambert, 0.0, 1.0)); // Clamp lambert then convert to a vector
	
	FragColor = vec4(colorInput, 1.0) * vec4(lightEnergy, 1.0); // Calculate final color
}//----------------------------------------------------------------------------------------------------------------------

