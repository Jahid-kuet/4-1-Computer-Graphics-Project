// Terrain.h — Dark grey-green ground plane.
#pragma once
#include "Shader.h"
#include "mathutil.h"

namespace Terrain {
    // Draws a large flat terrain plane
    void draw(Shader& shader, const math::mat4& model);
}
