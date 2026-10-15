#ifndef CUSTOM_H
#define CUSTOM_H

#include "Shape.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cmath>
#include <random>

class Custom : public Shape {
public:
    const char* serialType() const override { return "Custom"; }

    Custom(float x, float y, float z, float uniformScale, int colorIndex, int id,
           float scaleX = 1.0f, float scaleY = 1.0f, float scaleZ = 1.0f, bool useUniformScaling = true);
    ~Custom();

    void draw(GLuint shaderProgram) override; // Render the custom shape

private:
    GLuint VAO, VBO, EBO; // OpenGL handles for custom geometry
    void setupCustom();   // Initializes the VAO/VBO/EBO for the custom shape

    void addTriangle(int i1, int i2, int i3);
    void addQuad(int p1, int p2, int p3, int p4);
    void GenerateBismuth(glm::vec3 center, float width, float depth, int step_count, float height_step, float shrink_rate);
};

#endif

