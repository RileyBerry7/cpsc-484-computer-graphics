// =============================================================================
// Phong local illumination -- the `lighting` topic
// =============================================================================
// See include/Lighting.h for why this is its own translation unit, and for the
// contract with shaders/fragment_shader.glsl. That shader is GIVEN and is the
// reference: these two functions must produce the same numbers it does, or the
// viewport and a ray-traced render of the same scene will disagree.

#include "Lighting.h"

#include <cmath>

LightGeometry lightGeometry(const SceneLightSample& light, const glm::vec3& point) {
    LightGeometry g;

    // TODO: (lighting): light direction, distance and distance falloff
    // Fill in `g` for this light as seen from `point`.
    //
    // Two cases. A DIRECTIONAL light (type == 1) has parallel rays: the
    // direction is the same everywhere and there is no falloff, so distance
    // is effectively infinite and attenuation is 1. Note that the stored
    // direction is the way the light travels, and you want the vector from
    // the surface TOWARDS it.
    //
    // A POINT light has a position: direction and distance both come from
    // subtracting, and attenuation is 1 / (constant + linear*d +
    // quadratic*d*d). Guard the zero-distance case rather than normalising
    // a zero vector.
    //
    // Leaving this as the default -- straight up, no falloff -- lights every
    // surface identically from above, which is a recognisable first result
    // rather than a black screen.

    switch (light.type) 
    {
    // POINT LIGHT (type == 0)
    case 0: {
            auto raw_dir  = light.position - point;
            g.distance    = glm::length(raw_dir);
            if (g.distance > 0.0f) 
            {
                g.direction = raw_dir / g.distance; // Avoids computing length twice

            } else {
                g.direction = glm::vec3(0.0f, 1.0f, 0.0f); // Fallback direction
            }
            
            auto d        = g.distance;
            g.attenuation = 1.0f / (light.constant + light.linear * d + light.quadratic * d * d);
            break;
    }

    // DIRECTIONAL LIGHT (type == 1)
    case 1: {
            g.direction   = -glm::normalize(light.direction);
            g.distance    = std::numeric_limits<float>::infinity();
            g.attenuation = 1.0f;
            break;
    }

    default: 
            break;
    }
    return g;
}

glm::vec3 phongContribution(const SceneLightSample& light,
                            const Material& mat,
                            const glm::vec3& baseColor,
                            const glm::vec3& n,
                            const glm::vec3& viewDir,
                            const glm::vec3& lightDir,
                            float reach) {
    // TODO: (lighting): ambient, diffuse and specular for one light
    // Return ambient + diffuse + specular for this one light.
    //
    // Each term is a light colour times a material coefficient -- mat.ambient,
    // mat.diffuse, mat.specular, mat.shininess. Those are the numbers the
    // Materials work puts on a shape; this is where they mean something.
    //
    // Four things to get right:
    //   - ambient ignores `reach`. It stands in for light that arrived by a
    //     path nobody traced, so a shadow ray towards THIS light says nothing
    //     about it. Scaling it makes every shadow a black hole.
    //   - diffuse is Lambert: max(dot(n, lightDir), 0). Clamp at zero, or a
    //     surface facing away subtracts light from the other lights.
    //   - specular is PHONG, not Blinn-Phong: reflect the light direction
    //     about the normal and dot it with the view direction. The GLSL in
    //     shaders/fragment_shader.glsl does it that way and the two must
    //     agree, or the viewport and a render disagree.
    //   - the specular term does NOT take baseColor. A highlight is the
    //     light seen in the surface, which is why a red ball has a white
    //     highlight.
    //
    // Returning just the ambient term is a usable checkpoint: flat, unlit
    // silhouettes in roughly the right colours.

    // 1. Ambient: ignores `reach` (attenuation/shadow factor)
    glm::vec3 ambient = light.ambient * mat.ambient * baseColor;

    // 2. Diffuse (Lambert): clamped at 0.0f so back-facing surfaces don't subtract light
    float diff = glm::max(glm::dot(n, lightDir), 0.0f);
    glm::vec3 diffuse = light.diffuse * (diff * mat.diffuse) * baseColor;

    // 3. Specular (Phong): reflects light around normal, ignores `baseColor`
    glm::vec3 reflectDir = glm::reflect(-lightDir, n);
    float spec = glm::pow(glm::max(glm::dot(viewDir, reflectDir), 0.0f), mat.shininess);
    glm::vec3 specular = light.specular * (spec * mat.specular);

    // Diffuse and specular are scaled by `reach` (attenuation and shadow visibility)
    return ambient + reach * (diffuse + specular);
}
