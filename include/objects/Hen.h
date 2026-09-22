// Hen.h — Village hen / chicken.
#pragma once
#include "Shader.h"
#include "mathutil.h"

namespace Hen {
    // Draws a recognizable hen (body, head, beak, comb, legs, tail)
    void draw(Shader& shader, const math::mat4& model);
}
