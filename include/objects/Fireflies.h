// Fireflies.h — Bioluminescent Fireflies (Jonaki Poka) with warm sun-yellow nocturnal blink.
#pragma once
#include "Shader.h"
#include "mathutil.h"

namespace Fireflies {
    // Draws scattered bioluminescent fireflies that blink with warm sun-yellow / golden flashes at night.
    void draw(Shader& shader, float time = 0.0f, bool isNight = true);
}
