#ifndef GL_OBJECTS_H
#define GL_OBJECTS_H

#include<glad.h>

//---------------------------------------------------------------------------------
// VBO

class VBO
{
public:
	GLuint ID; // VBO reference ID

	// Constructor that generates a VBO and links it to vertices
	VBO(GLfloat* vertices, GLsizeiptr size);

	void Bind();   // Binds the VBO
	void Unbind(); // Unbinds the VBO
	void Delete(); // Deletes the VBO
};

//---------------------------------------------------------------------------------
// VAO

class VAO
{
public:
	GLuint ID; // VAO reference ID
	
	// Constructor that generates a VAO ID
	VAO();

	// Links a VBO Attribute such as a position or color to the VAO
	void LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);

	void Bind();   // Binds the VAO
	void Unbind(); // Unbinds the VAO
	void Delete(); // Deletes the VAO
};

//---------------------------------------------------------------------------------
// EBO

class EBO
{
public:
	GLuint ID; // EBO reference ID
	
	// Constructor that generates a Elements Buffer Object and links it to indices
	EBO(GLuint* indices, GLsizeiptr size);

	void Bind();   // Binds the EBO
	void Unbind(); // Unbinds the EBO
	void Delete(); // Deletes the EBO
};
//---------------------------------------------------------------------------------
#endif
