// =============================================================================
// Texture coordinate generation and sampling
// =============================================================================
// Student work. See include/TexCoords.h for why this is its own translation
// unit and what stays on the given side.

#include "TexCoords.h"
#include "TextureCache.h"

#include <cmath>
#include <algorithm>

namespace TexCoords {

std::vector<glm::vec2> sphere(unsigned latSegments, unsigned lonSegments) {
    // TODO: (textures): generate one UV per sphere vertex
    // Walk the same loop bounds Sphere::setupSphere uses, so entry i here
    // matches vertex i there. Remember v runs bottom-up for OpenGL.

    std::vector<glm::vec2> uvs;
    uvs.reserve((latSegments + 1) * (lonSegments + 1));

    for (unsigned i = 0; i <= latSegments; i++) {
        for (unsigned j = 0; j <= lonSegments; j++) {
            float u = static_cast<float>(j) / lonSegments;
            float v = 1.0f - static_cast<float>(i) / latSegments;

            uvs.push_back(glm::vec2(u, v));
        }
    }
    return uvs;
}


std::vector<glm::vec2> cube(const std::vector<glm::vec3>& vertices,
                            const std::vector<std::vector<int> >& faces,
                            const std::vector<glm::vec3>& faceNormals) {
    //TODO: (textures): generate one UV per cube triangle corner
    // Three per face, in the order the corners appear in `faces`. Project
    // each vertex onto the two axes the face normal is NOT aligned with.
    // faceNormals is indexed faceNormals[i / 2]: two triangles per face.

    std::vector<glm::vec2> uvs;
    uvs.reserve(faces.size() * 3); // 3 corners per triangle face

    // Face Loop
    for (size_t i = 0; i < faces.size(); ++i) {
        glm::vec3 normal = faceNormals[i];

        // Corner Loop
        for (size_t j = 0; j < faces[i].size(); ++j) {
            glm::vec3 v = vertices[faces[i][j]];
            glm::vec2 uv(0.0f);

            // Project vertex onto the two axes the normal is NOT aligned with
            if (std::abs(normal.x) > 0.5f) {
                // Normal is along X -> project onto Y and Z
                uv = glm::vec2(v.y, v.z);
            } else if (std::abs(normal.y) > 0.5f) {
                // Normal is along Y -> project onto X and Z
                uv = glm::vec2(v.x, v.z);
            } else {
                // Normal is along Z -> project onto X and Y
                uv = glm::vec2(v.x, v.y);
            }

            // Shift from [-0.5, 0.5] to [0.0, 1.0]
            uv += glm::vec2(0.5f);

            uvs.push_back(uv);
        }
    }
    return uvs;
}


// Cylindrical projection, one UV per triangle corner.
//
// For anything shaped roughly like a solid of revolution but WITHOUT a
// generating parameterisation to reuse -- the teapot above all, whose vertex
// data arrives as a bare triangle soup from a table of Bezier patches.
//
// u is the angle round the Y axis, v the height, both normalised by the mesh's
// own bounds.
std::vector<glm::vec2> cylindricalPerCorner(const std::vector<glm::vec3>& vertices,
                                            const std::vector<std::vector<int> >& faces) {
    // TODO: (textures): cylindrical projection with the wrap seam repaired
    // Project every triangle corner onto a cylinder about the Y axis: u is
    // the angle round it (atan2(z, x), remapped from -pi..pi into 0..1), v is
    // the height normalised by the mesh's own Y bounds. Return three UVs per
    // face, in corner order.
    //
    // Then repair the wrap seam, which is the part worth the effort. A
    // triangle straddling the back of the mesh gets corners at u = 0.98 and
    // u = 0.02; interpolated across the face that runs the LONG way round and
    // squeezes the whole image, mirrored, into one triangle -- a bright
    // vertical scar down the back. Detect it (the u range across a triangle
    // exceeds half a turn) and push the low corners past 1.0 so the span is
    // short again; wrapping in the sampler puts them back.
    //
    // That repair is only expressible per corner, which is why this returns
    // per-corner UVs: one point in space needs two different u values
    // depending on which triangle is asking.

    if (vertices.empty() || faces.empty()) return {};

    // Compute Y bounds for vertical normalization
    auto [minIt, maxIt] = std::minmax_element(vertices.begin(), vertices.end(),
        [](const glm::vec3& a, const glm::vec3& b) { return a.y < b.y; });
    float minY = minIt->y;
    float spanY = std::max(maxIt->y - minY, 1e-6f);

    constexpr float PI = 3.14159265358979323846f;
    std::vector<glm::vec2> uvs;
    uvs.reserve(faces.size() * 3);

    for (const auto& face : faces) {
        size_t start = uvs.size();
        float minU = 1.0f, maxU = 0.0f;

        for (int idx : face) {
            const glm::vec3& p = vertices[idx];
            float u = (std::atan2(p.z, p.x) + PI) / (2.0f * PI);
            minU = std::min(minU, u);
            maxU = std::max(maxU, u);
            uvs.emplace_back(u, (p.y - minY) / spanY);
        }

        // Repair wrap seam if triangle spans more than half a turn
        if (maxU - minU > 0.5f) {
            for (size_t k = start; k < uvs.size(); ++k)
                if (uvs[k].x < 0.5f) uvs[k].x += 1.0f;
        }
    }
    return uvs;
}


} // namespace TexCoords

glm::vec3 Texture::sample(float u, float v) const {
    //TODO: (textures): bilinear sample in normalised coordinates, wrapping outside [0, 1]
    // Wrap u and v into [0, 1), find the four surrounding texels, and
    // blend them. Texel CENTRES sit at (i + 0.5) / size, which is why the
    // -0.5 below matters: without it the image is offset by half a texel.

    // Error Checking
    if (width <= 0 || height <= 0) {
        std::cerr << "Invalid texture dimensions, cannot be sampled." << std::endl;
        return glm::vec3(1.0f,1.0f,1.0f);
    }

    // Ensure any uv coordinate is valid. This determines how the texture wraps 
    // when out-of-bounds range = [0, 1]
    
    // Wrap/Repeat uv-space addressing
    u = u - std::floor(u);
    v = v - std::floor(v);

    /***********************************************************************************
     * MATH EQUATIONS:
     * UV-Space    = Texel-Space / axis-length
     * Texel-Space = UV-Space * axis-length 
     * Texel-Space(centered) = (UV-coords * width) - 0.5; 
     ***********************************************************************************/
    
    // Calculate Centered texel coords
    float x = (u * width)  - 0.5f; // Horizontal coord 
    float y = (v * height) - 0.5f; // Vertical coord
    
    // Ensure texel coords so they are always valid: range = [0, axis-length - 1]
    
    // Clamp Texel-space addressing
    //if (x < 0) x = 0;
    //if (y < 0) y = 0;
    //if (x > width - 1) x = width - 1;
    //if (y > width - 1) y = width - 1;

    // Wrap/Repeat Texel-space addressing 
    x = x - width  * std::floor((x + 0.5f) / width);
    y = y - height * std::floor((y + 0.5f) / height);


    int raw_lower_x = static_cast<int>(std::floor(x));
    int raw_lower_y = static_cast<int>(std::floor(y));
    float tx = x - raw_lower_x; // Horizontal lerp weight
    float ty = y - raw_lower_y; // Vertical lerp weight
    int lower_x = (raw_lower_x < 0) ? (width - 1) : raw_lower_x;
    int lower_y = (raw_lower_y < 0) ? (height - 1) : raw_lower_y;
    int upper_x = (lower_x + 1 == width) ? 0 : (lower_x + 1);
    int upper_y = (lower_y + 1 == height) ? 0 : (lower_y + 1);

    // Surrounding texel coords
    //float lower_x = std::floor(x);
    //float upper_x = std::ceil(x);
    //float lower_y = std::floor(y);
    //float upper_y = std::ceil(y);
    
    // FILTERING FUNCTION: calculates color at some texture coordinate

    // Find four surrounding texels
    std::vector<glm::vec2> surrounding_texels;
    surrounding_texels.push_back(glm::vec2(lower_x, lower_y));
	surrounding_texels.push_back(glm::vec2(upper_x,  lower_y));
	surrounding_texels.push_back(glm::vec2(lower_x, upper_y));
	surrounding_texels.push_back(glm::vec2(upper_x,  upper_y));

    // Find each texel's color
    std::vector<glm::vec4> colors;
    for (const auto& texel : surrounding_texels) {

        int texel_index = (texel.y * width + texel.x) * 4;
        colors.push_back(glm::vec4( pixels[texel_index + 0], 
                                    pixels[texel_index + 1], 
                                    pixels[texel_index + 2], 
                                    pixels[texel_index + 3]));
    }

    // Linear interpolate to blend final color
    auto top_lerp = colors[2] + tx * (colors[3] - colors[2]); // Top lerp 
    auto bot_lerp = colors[0] + tx * (colors[1] - colors[0]); // Bottom lerp 
    auto final_color = bot_lerp + ty * (top_lerp - bot_lerp); // Vertical lerp 

    return glm::vec3(final_color);
}
