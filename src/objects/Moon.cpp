// Moon.cpp — Full moon as a large emissive sphere.

#include "objects/Moon.h"
#include "Primitives.h"

using namespace math;

namespace Moon {

void draw(Shader& shader, const mat4& model)
{
    vec3 moonColor(0.95f, 0.92f, 0.70f);   // pale yellow-white

    // Enable emissive mode (bypasses Phong calculation)
    shader.setFloat("emissive", 1.0f);

    mat4 m = model;
    m = scale(m, vec3(2.0f, 2.0f, 2.0f));
    Primitives::drawSphere(shader, m, moonColor);

    // Slight glow halo (larger, dimmer sphere around the moon)
    mat4 halo = model;
    halo = scale(halo, vec3(2.8f, 2.8f, 2.8f));
    Primitives::drawSphere(shader, halo, vec3(0.55f, 0.52f, 0.38f));

    shader.setFloat("emissive", 0.0f);
}

vec3 getLightDirection(const vec3& moonPosition)
{
    // Direction FROM the moon toward the scene (normalize position → negate)
    return normalize(moonPosition);
}

} // namespace Moon
