// =============================================================================
// Shape::getModelMatrix -- the model transform
// =============================================================================
// Split out of Shape.cpp deliberately. That file keeps everything that is
// always given: materials, colour, vertex and normal accessors, texture
// coordinate upload, reset-to-default. This file holds only the composition of
// the model matrix, which is the student's work.
//
// The split is what makes a per-topic solution library possible: the unit the
// linker can swap is a whole translation unit, so the swappable code has to
// live in a file of its own. See SOLUTION-LIBRARIES.md and ASSIGNMENTS.md.
//
// The order is T * Rz * Ry * Rx * S, read right to left: scale the object in
// its own frame, then rotate it, then move it. Any other order gives a
// different result -- scaling after a rotation shears, and translating before
// a rotation swings the object round the origin instead of spinning in place.

#include "Shape.h"

#include <cmath>

glm::mat4 Shape::getModelMatrix() const {
    // TODO:(transforms): compose the model matrix as T * Rx * Ry * Rz * S
    // Return the 4x4 matrix that takes this shape from its own space into
    // the world, accounting for position, rotation about all three axes, and
    // scale.
    //
    // Build the matrices YOURSELF. glm::mat4 and glm::vec3 as types are fine,
    // and so is * to multiply them. glm::translate, glm::rotate and
    // glm::scale are not -- the point is to know what is inside them.
    //
    // Three things to get right:
    //   ORDER. Matrix multiplication does not commute, and the rightmost
    //     factor applies first. Scaling then rotating is not the same as
    //     rotating then scaling; one of them shears your shape.
    //   FRAME. A shape rotates about its OWN origin, not the world's. If
    //     your cube orbits the origin when you rotate it, your order is
    //     wrong.
    //   STORAGE. glm::mat4's constructor takes COLUMNS. The translation
    //     lives in the fourth column. Getting this backwards gives you the
    //     transpose, which looks almost right until something rotates.
    //
    // useUniformScale selects between `scale` and (scaleX, scaleY, scaleZ).
    //
    // Returning the identity leaves every shape at the origin, unrotated and
    // unscaled -- so the Insert menu still works and you can see your
    // geometry, which is the right place to start.

    auto S = glm::mat4(1.0f);  // Scale matrix
    auto Rx = glm::mat4(1.0f); // x-axis rotation matrix
    auto Ry = glm::mat4(1.0f); // y-axis rotation matrix 
    auto Rz = glm::mat4(1.0f); // z-axis rotation matrix
    auto T = glm::mat4(1.0f);  // Translation matrix
   
    if (useUniformScale) {
        // Uniform scaling
        S = glm::mat4(scale); 
        S[3][3] = 1.0f;
    
    } else {
        // Non-uniform scaling 
        S = glm::mat4(scaleX, 0.0f, 0.0f, 0.0f,
                      0.0f, scaleY, 0.0f, 0.0f,
                      0.0f, 0.0f, scaleZ, 0.0f,
                      0.0f, 0.0f, 0.0f, 1.0f);
    }

    float theta; // Rotation angle in radians

    // x-axis rotation
    theta = glm::radians(angleX);
    Rx[1][1] = cos(theta);
    Rx[1][2] = sin(theta);
    Rx[2][1] = -sin(theta);
    Rx[2][2] = cos(theta);

    // y-axis rotation
    theta = glm::radians(angleY);
    Ry[0][0] = cos(theta);
    Ry[0][2] = -sin(theta);
    Ry[2][0] = sin(theta);
    Ry[2][2] = cos(theta);

    // z-axis rotation
    theta = glm::radians(angleZ);
    Rz[0][0] = cos(theta);
    Rz[0][1] = sin(theta);
    Rz[1][0] = -sin(theta);
    Rz[1][1] = cos(theta);

    // Translation
    T[3][0] = x;
    T[3][1] = y;
    T[3][2] = z;

    return T * Rz * Ry * Rx * S; // Apply matrices 
}

