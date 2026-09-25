// Moon.cpp — Full moon as a large emissive sphere.

#include "objects/Moon.h"
#include "Primitives.h"

using namespace math;

namespace Moon {

void draw(Shader& shader, const mat4& model)
{
    shader.setInt("uUseTexture", 0); // No texture mapping on lunar surface

    // Soft warm ivory moonlight disc
    const vec3 moonCore(0.98f, 0.96f, 0.88f);
    const vec3 mareColor(0.84f, 0.82f, 0.76f); // subtle lunar maria / craters

    // Enable emissive mode (bypasses dark shading)
    shader.setFloat("emissive", 1.0f);

    // Main luminous lunar sphere
    mat4 m = model;
    m = scale(m, vec3(2.0f, 2.0f, 2.0f));
    Primitives::drawSphere(shader, m, moonCore);

    // Subtle lunar maria patches on the face of the moon
    mat4 mare1 = model;
    mare1 = translate(mare1, vec3(0.55f, 0.45f, 1.82f));
    mare1 = scale(mare1, vec3(0.55f, 0.45f, 0.15f));
    Primitives::drawSphere(shader, mare1, mareColor);

    mat4 mare2 = model;
    mare2 = translate(mare2, vec3(-0.45f, -0.35f, 1.84f));
    mare2 = scale(mare2, vec3(0.65f, 0.50f, 0.15f));
    Primitives::drawSphere(shader, mare2, mareColor);

    mat4 mare3 = model;
    mare3 = translate(mare3, vec3(-0.65f, 0.30f, 1.80f));
    mare3 = scale(mare3, vec3(0.40f, 0.35f, 0.15f));
    Primitives::drawSphere(shader, mare3, mareColor);

    shader.setFloat("emissive", 0.0f);
}

vec3 getLightDirection(const vec3& moonPosition)
{
    // Direction FROM the moon toward the scene (normalize position → negate)
    return normalize(moonPosition);
}

} // namespace Moon
