// Stars.h — Scattered emissive star spheres in the night sky.
#pragma once
#include "Shader.h"
#include "mathutil.h"

namespace Stars {
    // Draws many small emissive spheres at deterministic pseudo-random sky positions with twinkling.
    void draw(Shader& shader, float time = 0.0f);
}
