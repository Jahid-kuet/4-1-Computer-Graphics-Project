// River.h — River body with sloped riverbanks.
#pragma once
#include "Shader.h"
#include "mathutil.h"

namespace River {
    // Draws river water plane and sloped banks on both sides.
    // time parameter is exposed for future wave animation (unused for now).
    void draw(Shader& shader, const math::mat4& model, float time = 0.0f);
}
