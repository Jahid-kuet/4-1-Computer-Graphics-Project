// Boat.h — Traditional Bangladeshi wooden boat (nouka).
#pragma once
#include "Shader.h"
#include "mathutil.h"

namespace Boat {
    // Draws a complete boat hull with interior and bench
    void draw(Shader& shader, const math::mat4& model);
}
