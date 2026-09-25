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

    // Draws a traditional rural Bengali woven cane/bamboo stool (Mora / মোড়া)
    void drawMora(Shader& shader, const math::mat4& model);

    // Draws an authentic traditional rural timber seating bench (Kath-er Bench / কাঠের বেঞ্চ)
    void drawBench(Shader& shader, const math::mat4& model);
}

