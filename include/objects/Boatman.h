// Boatman.h — Seated villager with oar, placed inside the boat.
#pragma once
#include "Shader.h"
#include "mathutil.h"

namespace Boatman {
    // Draws a boatman (seated person + oar with shoulder→arm hierarchy).
    // oarAngle: rotation of the rowing arm in radians (for future animation).
    void draw(Shader& shader, const math::mat4& model, float oarAngle = 0.0f);
}
