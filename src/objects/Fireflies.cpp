// Fireflies.cpp — Small emissive firefly spheres with sine-based blink.
//                 Positions are deterministic (no stored state).

#include "objects/Fireflies.h"
#include "Primitives.h"
#include <cmath>

using namespace math;

namespace Fireflies {

static float hashFloat(int seed, float offset)
{
    float v = sinf((float)seed * 12.9898f + offset * 78.233f) * 43758.5453f;
    return v - floorf(v);
}

void draw(Shader& shader, float time)
{
    shader.setFloat("emissive", 1.0f);

    const int FLY_COUNT = 30;

    for (int i = 0; i < FLY_COUNT; i++) {
        // Position: scattered around the village area at low heights
        float x = (hashFloat(i, 1.0f) - 0.5f) * 30.0f;
        float y = 0.5f + hashFloat(i, 2.0f) * 3.0f;
        float z = (hashFloat(i, 3.0f) - 0.5f) * 30.0f;

        // Blink intensity: each firefly has a different phase
        float phase = hashFloat(i, 4.0f) * 2.0f * PI;
        float freq  = 1.5f + hashFloat(i, 5.0f) * 2.0f;
        float blink = 0.5f + 0.5f * sinf(time * freq + phase);

        // Yellowish-green glow, modulated by blink
        vec3 col(0.85f * blink, 0.92f * blink, 0.18f * blink);

        float sz = 0.04f + hashFloat(i, 6.0f) * 0.03f;

        mat4 m = mat4::identity();
        m = translate(m, vec3(x, y, z));
        m = scale(m, vec3(sz));
        Primitives::drawSphere(shader, m, col);
    }

    shader.setFloat("emissive", 0.0f);
}

} // namespace Fireflies
