// Stars.cpp — Deterministic pseudo-random star placement.
//             Uses a hash-like function so positions are consistent every frame
//             without needing init() or stored state.

#include "objects/Stars.h"
#include "Primitives.h"
#include <cmath>

using namespace math;

namespace Stars {

// Deterministic pseudo-random float in [0, 1) from a seed
static float hashFloat(int seed, float offset)
{
    float v = sinf((float)seed * 12.9898f + offset * 78.233f) * 43758.5453f;
    return v - floorf(v);
}

void draw(Shader& shader, float time)
{
    shader.setFloat("emissive", 1.0f);

    const int STAR_COUNT = 140;

    for (int i = 0; i < STAR_COUNT; i++) {
        // Spread across a large sky dome
        float theta = hashFloat(i, 1.0f) * 2.0f * PI;
        float phi   = hashFloat(i, 2.0f) * 0.45f + 0.15f;   // 15°–60° above horizon
        float r     = 55.0f + hashFloat(i, 3.0f) * 15.0f;

        float x = r * cosf(phi) * cosf(theta);
        float y = r * sinf(phi) + 8.0f;
        float z = r * cosf(phi) * sinf(theta);

        // Subtle twinkling animation
        float twinkleFreq = 1.0f + hashFloat(i, 4.0f) * 2.5f;
        float twinklePhase = hashFloat(i, 5.0f) * 6.28f;
        float twinkle = 0.70f + 0.30f * sinf(time * twinkleFreq + twinklePhase);

        float bright = (0.75f + hashFloat(i, 6.0f) * 0.25f) * twinkle;
        vec3 col(bright, bright * 0.98f, bright * 0.92f);

        // Size variation
        float sz = 0.06f + hashFloat(i, 7.0f) * 0.08f;

        mat4 m = mat4::identity();
        m = translate(m, vec3(x, y, z));
        m = scale(m, vec3(sz));
        Primitives::drawSphere(shader, m, col);
    }

    shader.setFloat("emissive", 0.0f);
}

} // namespace Stars
