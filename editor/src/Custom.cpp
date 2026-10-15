#include "Custom.h"
#include "TexCoords.h"

#include <cmath>

namespace {
// sign(w) * |w|^e. Raising a negative base to a fractional power is NaN, so
// the sign is stripped and reapplied.
float signedPow(float base, float exponent) {
    float magnitude = std::pow(std::fabs(base), exponent);
    return (base < 0.0f) ? -magnitude : magnitude;
}
} // namespace

const int step_count    = 10000;
const float shrink_rate = 0.02f;
const float height_step = 0.01f;

Custom::Custom(float x, float y, float z, float uniformScale, int colorIndex, int id,
               float scaleX, float scaleY, float scaleZ, bool useUniformScaling)
    : Shape(x, y, z, uniformScale, colorIndex, id, scaleX, scaleY, scaleZ, useUniformScaling), VAO(0), VBO(0), EBO(0) {
    shapeType = customShapeName;

    setupCustom();      // Prepare OpenGL buffers

}

Custom::~Custom() {
    // Cleanup OpenGL resources
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Custom::setupCustom() {
    std::vector<float> vertexData;
    std::vector<unsigned int> indexData;

    // Grid resolution, GIVEN and deliberately outside the marked region below:
    // the texture-coordinate code further down builds its grid from these, so
    // they have to survive when the geometry is stripped. If your own shape is
    // not built from a (u, v) grid, ignore them -- the UV code will simply not
    // line up until Assignment 5, which is expected.
    const int uSteps = 64;
    const int vSteps = 48;

    // TODO:(geometry): a shape of your own choosing, at least twelve faces
    // Build the shape: fill `vertices`, `faces`, and `normals` (directly or
    // by calling calculateNormals()). See ASSIGNMENTS.md, A2, for the
    // conventions -- roughly one unit across, centred on the origin,
    // counter-clockwise winding seen from outside, every face index a valid
    // index into `vertices`.
    //
    // This one is yours to design: at least twelve faces, and something you
    // can explain. Name it by editing customShapeName in src/Globals.cpp.
    //
    // What is here is a placeholder square so the editor runs and the Insert
    // menu does something visible. Read src/Torus.cpp first -- it is the
    // worked example of a procedural shape.

   
    //----------------------------------------------------------------------------------------------------
    auto  c = glm::vec3(0.0f, 0.0f, 0.0f); // Center
    float w = 1.5f; // Width
    float d = 1.5f; // Depth

    GenerateBismuth(c + glm::vec3(0.0f, 0.0f, 0.0f), w, d, step_count, height_step, shrink_rate);
    //----------------------------------------------------------------------------------------------------
    
    calculateNormals();
    //CalculateSmoothNormals();

    // The shape's own (u, v) grid parameters, the same way the sphere uses
    // its own. flipU for the same handedness reason as Sphere: a generator that
    // sweeps u anticlockwise seen from above mirrors the image without it.

    const std::vector<glm::vec2> gridUVs = TexCoords::parametricGrid(uSteps, vSteps, true, false);
    texCoords.clear();

    // Build vertex data and index data for OpenGL, using the PER-VERTEX normal
    // so the surface is smooth rather than faceted.
    //
    // SKIPS ANY FACE THAT INDEXES OUTSIDE `vertices` OR `normals`. This shape
    // is the one the student designs, so its topology is whatever they made it
    // -- and an off-by-one in their face indices is a normal thing to hit
    // halfway through. Without this guard that mistake is not a wrong-looking
    // shape, it is an out-of-bounds read: undefined behaviour that usually
    // lands in heap slack and renders garbage, and occasionally crashes on
    // someone else's machine. The handout warns students about exactly this
    // ("an out-of-range index reads memory that is not yours"), so the editor
    // must not do it to them.
    //
    // Skipping rather than clamping is deliberate: a clamped face draws a
    // plausible-looking triangle and hides the bug, while a missing one is
    // visible and geometry_test says so in as many words.
    size_t emitted = 0;
    for (size_t i = 0; i < faces.size(); ++i) {

        bool usable = (faces[i].size() == 3);
        for (size_t j = 0; usable && j < 3; ++j) {
            const int vi = faces[i][j];
            if (vi < 0 ||
                vi >= static_cast<int>(vertices.size()) ||
                vi >= static_cast<int>(normals.size())) {
                usable = false;
            }
        }
        if (!usable) continue;

        // Assign color
        colorIndex = 5;
        glm::vec3 color = (colorIndex == 31)
            ? glm::vec3(customColor[0], customColor[1], customColor[2])
            : glm::vec3(colorPresets[colorIndex].color[0],
                        colorPresets[colorIndex].color[1],
                        colorPresets[colorIndex].color[2]);

        for (int j = 0; j < 3; ++j) {
            int vertexIndex = faces[i][j];
            const glm::vec3& position = vertices[vertexIndex];
            const glm::vec3& normal   = normals[vertexIndex];

            vertexData.insert(vertexData.end(), {position.x, position.y, position.z});
            vertexData.insert(vertexData.end(), {normal.x, normal.y, normal.z});
            vertexData.insert(vertexData.end(), {color.r, color.g, color.b});

            // Expanded per corner: indexData below is sequential, so
            // attribute 3 must match the interleaved buffer's length.
            texCoords.push_back(vertexIndex < static_cast<int>(gridUVs.size())
                                ? gridUVs[static_cast<size_t>(vertexIndex)]
                                : glm::vec2(0.0f));
        }

        // Counts EMITTED faces, not the loop index: a skipped face must not
        // leave a gap in the sequential index buffer.
        indexData.insert(indexData.end(), {static_cast<unsigned int>(emitted * 3),
                                            static_cast<unsigned int>(emitted * 3 + 1),
                                            static_cast<unsigned int>(emitted * 3 + 2)});
        ++emitted;
    }

    // Create and bind VAO, VBO, and EBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexData.size() * sizeof(unsigned int), indexData.data(), GL_STATIC_DRAW);

    // Configure vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0); // Position
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float))); // Normal
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float))); // Color
    glEnableVertexAttribArray(2);

    uploadTexCoords();

    glBindVertexArray(0); // Unbind VAO
    
}

void Custom::draw(GLuint shaderProgram) {

    // Use the shader program
    glUseProgram(shaderProgram);
    
    // Enable lighting for the cube
    GLint lightingLoc = glGetUniformLocation(shaderProgram, "useLighting");
    if (lightingLoc != -1) {
        glUniform1i(lightingLoc, 1); // Enable lighting for the cube
    }

    // Apply transformations and pass to the shader
    applyTransform(shaderProgram);

    // Pass material properties
    GLint colorLoc = glGetUniformLocation(shaderProgram, "material.color");
    if (colorLoc != -1) {
        glUniform3fv(colorLoc, 1, (colorIndex == 31) ? customColor : colorPresets[colorIndex].color);
    }

    // Render the cube
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(faces.size() * 3), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Disable lighting after drawing the cube (for axis rendering)
    if (lightingLoc != -1) {
        glUniform1i(lightingLoc, 0);
    }
}

//-----------------------------------------------------------------------------------------------------
// HELPERS

// Helper to push a clean 3-index triangle face
void Custom::addTriangle(int i1, int i2, int i3) {
    faces.push_back({i1, i2, i3});
}

// Helper to append a 4-corner quad face as two clean triangles
void Custom::addQuad(int p1, int p2, int p3, int p4) {
    addTriangle(p1, p2, p3);
    addTriangle(p3, p4, p1);
}

//-----------------------------------------------------------------------------------------------------
// GENERATE BISMUTH - recursive
void Custom::GenerateBismuth(glm::vec3 center, float width, float depth, int step_count, float height_step, float shrink_rate) {
    // Random generator
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> rand01(0.0f, 1.0f);
    auto randomFloat = [&](float minValue, float maxValue) -> float { return minValue + (maxValue - minValue) * rand01(gen); };
    auto randomSign = [&]() -> float { return (rand01(gen) < 0.5f) ? -1.0f : 1.0f; };

    // Safety
    if (step_count <= 0 || width <= 0.01f || depth <= 0.01f) return;
    step_count = std::min(step_count, 64);

    glm::vec3 c = center;
    float w = width;
    float d = depth;

    // Branch randomness
    const float branchWobble = randomFloat(0.015f, 0.055f);
    const float branchDepthWobble = randomFloat(0.015f, 0.055f);
    const float verticalWobble = randomFloat(0.04f, 0.12f);
    const float aspectBias = randomFloat(0.88f, 1.12f); // Encourages rectangular crystals

    d *= aspectBias;
    glm::vec2 drift(randomFloat(-0.015f, 0.015f), randomFloat(-0.015f, 0.015f));

    for (int step = 0; step < step_count; ++step) {
        if (w <= 0.05f || d <= 0.05f) break;

        // Layer-specific randomness
        const float progress = static_cast<float>(step) / static_cast<float>(std::max(1, step_count - 1));
        const float variation = 1.0f - progress * 0.55f;
        float layerW = w * randomFloat(1.0f - branchWobble * variation, 1.0f + branchWobble * variation);
        float layerD = d * randomFloat(1.0f - branchDepthWobble * variation, 1.0f + branchDepthWobble * variation);
        layerW = std::min(layerW, w * 1.04f); // Prevent accidental expansion.
        layerD = std::min(layerD, d * 1.04f);
        float hW = layerW * 0.5f;
        float hD = layerD * 0.5f;

        // Random horizontal displacement
        drift.x += randomFloat(-0.025f, 0.025f);
        drift.y += randomFloat(-0.025f, 0.025f);
        drift.x = glm::clamp(drift.x, -width * 0.12f, width * 0.12f);
        drift.y = glm::clamp(drift.y, -depth * 0.12f, depth * 0.12f);
        glm::vec3 layerCenter = c + glm::vec3(drift.x, drift.y, 0.0f);

        // Bottom corners
        const float cornerJitter = std::min(layerW, layerD) * randomFloat(0.005f, 0.035f);

        glm::vec3 bo1 = layerCenter + glm::vec3(-hW + randomFloat(-cornerJitter, cornerJitter),
                                                -hD + randomFloat(-cornerJitter, cornerJitter), 0.0f);
        glm::vec3 bo2 = layerCenter + glm::vec3(hW + randomFloat(-cornerJitter, cornerJitter), 
                                                -hD + randomFloat(-cornerJitter, cornerJitter), 0.0f);
        glm::vec3 bo3 = layerCenter + glm::vec3(hW + randomFloat(-cornerJitter, cornerJitter), 
                                                hD + randomFloat(-cornerJitter, cornerJitter), 0.0f);
        glm::vec3 bo4 = layerCenter + glm::vec3(-hW + randomFloat(-cornerJitter, cornerJitter), 
                                                hD + randomFloat(-cornerJitter, cornerJitter), 0.0f);
        // Random terrace height
        float currentHeight = height_step * randomFloat(1.0f - verticalWobble * variation, 1.0f + verticalWobble * variation);
        currentHeight = std::max(currentHeight, height_step * 0.35f);

        // Extrude upward
        glm::vec3 to1 = bo1 + glm::vec3(0.0f, 0.0f, currentHeight);
        glm::vec3 to2 = bo2 + glm::vec3(0.0f, 0.0f, currentHeight);
        glm::vec3 to3 = bo3 + glm::vec3(0.0f, 0.0f, currentHeight);
        glm::vec3 to4 = bo4 + glm::vec3(0.0f, 0.0f, currentHeight);
        c.z += currentHeight;

        // Outer vertical walls
        int vStart = static_cast<int>(vertices.size());
        vertices.insert(vertices.end(), {bo1, bo2, bo3, bo4, to1, to2, to3, to4});
        addQuad(vStart + 0, vStart + 1, vStart + 5, vStart + 4);
        addQuad(vStart + 1, vStart + 2, vStart + 6, vStart + 5);
        addQuad(vStart + 2, vStart + 3, vStart + 7, vStart + 6);
        addQuad(vStart + 3, vStart + 0, vStart + 4, vStart + 7);

        // Calculate the next terrace.
        float baseShrinkW = randomFloat(0.055f, 0.13f);
        float baseShrinkD = randomFloat(0.055f, 0.13f);
        float normalizedShrink = glm::clamp(shrink_rate / std::max(0.001f, std::max(width, depth)), 0.015f, 0.35f);
        float shrinkW = glm::max(shrink_rate * randomFloat(0.70f, 1.30f), width * normalizedShrink * baseShrinkW);
        float shrinkD = glm::max(shrink_rate * randomFloat(0.70f, 1.30f), depth * normalizedShrink * baseShrinkD);
        w -= shrinkW;
        d -= shrinkD;

        // Final cap
        if (w <= 0.05f || d <= 0.05f || step == step_count - 1) {
            glm::vec3 capCenter = c;
            float capW = std::max(0.01f, layerW * randomFloat(0.15f, 0.55f));
            float capD = std::max(0.01f, layerD * randomFloat(0.15f, 0.55f));
            glm::vec3 cap1 = capCenter + glm::vec3(-capW, -capD, 0.0f);
            glm::vec3 cap2 = capCenter + glm::vec3(capW, -capD, 0.0f);
            glm::vec3 cap3 = capCenter + glm::vec3(capW, capD, 0.0f);
            glm::vec3 cap4 = capCenter + glm::vec3(-capW, capD, 0.0f);
            int capStart = static_cast<int>(vertices.size());
            vertices.insert(vertices.end(), {cap1, cap2, cap3, cap4});
            addQuad(capStart + 0, capStart + 3, capStart + 2, capStart + 1);
            break;
        }
        float nHW = w * 0.5f; // Inner terrace dimensions
        float nHD = d * 0.5f;
        float innerOffsetX = randomFloat(-layerW, layerW) * 0.035f;
        float innerOffsetY = randomFloat(-layerD, layerD) * 0.035f;
        glm::vec3 innerCenter = c + glm::vec3(innerOffsetX, innerOffsetY, 0.0f);
        
        // Slightly different shrink on each side.
        float leftScale = randomFloat(0.94f, 1.02f);
        float rightScale = randomFloat(0.94f, 1.02f);
        float frontScale = randomFloat(0.94f, 1.02f);
        float backScale = randomFloat(0.94f, 1.02f);

        glm::vec3 ti1 = innerCenter + glm::vec3(-nHW * leftScale, -nHD * frontScale, 0.0f);
        glm::vec3 ti2 = innerCenter + glm::vec3(nHW * rightScale, -nHD * frontScale, 0.0f);
        glm::vec3 ti3 = innerCenter + glm::vec3(nHW * rightScale, nHD * backScale, 0.0f);
        glm::vec3 ti4 = innerCenter + glm::vec3(-nHW * leftScale, nHD * backScale, 0.0f);

        int lipStart = static_cast<int>(vertices.size()); // Terrace lips
        vertices.insert(vertices.end(), {to1, to2, to3, to4, ti1, ti2, ti3, ti4});

        addQuad(lipStart + 0, lipStart + 1, lipStart + 5, lipStart + 4);
        addQuad(lipStart + 1, lipStart + 2, lipStart + 6, lipStart + 5);
        addQuad(lipStart + 2, lipStart + 3, lipStart + 7, lipStart + 6);
        addQuad(lipStart + 3, lipStart + 0, lipStart + 4, lipStart + 7);

        // Branching
        if ( step >= 2 && w > width * 0.22f ) {
            const int remainingSteps = step_count - step - 1;
            if ( remainingSteps > 3 ) {
                float sizeFactor = glm::clamp(w / std::max(width, 0.001f), 0.0f, 1.0f);
                float branchChance = 0.025f + sizeFactor * 0.11f; // Moderate branching probability.
                branchChance *= randomFloat(0.65f, 1.35f);
                branchChance = glm::clamp(branchChance, 0.015f, 0.16f);
                float childScale = randomFloat(0.32f, 0.52f);
                float childW = w * childScale;
                float childD = d * childScale * randomFloat(0.85f, 1.15f);
                for ( int side = 0; side < 4; ++side ) {
                    if ( rand01(gen) >= branchChance ) continue;
                    glm::vec3 direction(0.0f);

                    switch ( side ) {
                        case 0:
                            direction = glm::vec3(1.0f, 0.0f, 0.0f);
                            break;
                        case 1:
                            direction = glm::vec3(-1.0f, 0.0f, 0.0f);
                            break;
                        case 2:
                            direction = glm::vec3(0.0f, 1.0f, 0.0f);
                            break;
                        case 3:
                            direction = glm::vec3(0.0f, -1.0f, 0.0f);
                            break;
                    }
                    float edgeDistance = std::max(layerW, layerD) * randomFloat(0.72f, 1.05f); // Push branch outwards
                    float verticalOffset = height_step * randomFloat(-0.35f, 0.45f);
                    glm::vec3 sideways(randomFloat(-0.18f, 0.18f), randomFloat(-0.18f, 0.18f), 0.0f);
                    glm::vec3 spawnPos = c + direction * edgeDistance + sideways * std::min(layerW, layerD) 
                                           + glm::vec3(0.0f, 0.0f, verticalOffset);
                    float childHeight = height_step * randomFloat(0.82f, 1.12f); // Randomize branch height
                    float childShrink = shrink_rate * randomFloat(0.95f, 1.35f); // Branches shrink faster than parent

                    // Recursive call
                    Custom::GenerateBismuth(spawnPos, childW, childD, remainingSteps, childHeight, childShrink);
                }
            }
        }
        c.x = innerCenter.x; // Move center to the inner terrace.
        c.y = innerCenter.y;
    }
}
//-----------------------------------------------------------------------------------------------------
