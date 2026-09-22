// Fireflies.h — Small emissive spheres with sine-based blink.
#pragma once
#include "Shader.h"
#include "mathutil.h"

namespace Fireflies {
    // Draws scattered fireflies. time drives the blink animation.
    void draw(Shader& shader, float time = 0.0f);
}
