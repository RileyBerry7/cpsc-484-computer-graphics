#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H
//--------------------------------------------------------------------------------------------
#include <glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cerrno>
//--------------------------------------------------------------------------------------------
std::string get_file_contents(const char* filename);

//============================================================================================
class Shader
{
public:
	GLuint ID; // Reference ID of the Shader Program
	
	// Constructor that build the Shader Program from 2 different shaders
	Shader(const char* vertexFile, const char* fragmentFile);

	void Activate(); // Activates the Shader Program
	void Delete();   // Deletes the Shader Program
//--------------------------------------------------------------------------------------------
private:
	// Checks if the different Shaders have compiled properly
	void compileErrors(unsigned int shader, const char* type);
};
//--------------------------------------------------------------------------------------------
#endif
