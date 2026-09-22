// Duck.h — Village duck.
#pragma once
#include "Shader.h"
#include "mathutil.h"

namespace Duck {
    // Draws a recognizable duck (wider body, flat beak, short legs)
    void draw(Shader& shader, const math::mat4& model);
}
