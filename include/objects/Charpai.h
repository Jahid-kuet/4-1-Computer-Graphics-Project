// Charpai.h — Traditional woven bed (charpai) and handheld fan.
#pragma once
#include "Shader.h"
#include "mathutil.h"

namespace Charpai {
    // Draws the charpai (woven bed with four legs and criss-cross top)
    void draw(Shader& shader, const math::mat4& model);

    // Draws a hand-held rotating fan (hub + blades).
    // fanAngle: current rotation of blades in radians.
    void drawFan(Shader& shader, const math::mat4& model, float fanAngle = 0.0f);

    // Draws a traditional rural Hurricane Kerosene Lantern (Hariken) with glowing flame
    void drawLantern(Shader& shader, const math::mat4& model);
}

