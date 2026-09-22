// Moon.h — Large emissive sphere representing the full moon.
#pragma once
#include "Shader.h"
#include "mathutil.h"

namespace Moon {
    // Draws a full moon sphere (uses emissive shading).
    void draw(Shader& shader, const math::mat4& model);

    // Returns the direction vector FROM the moon for lighting.
    math::vec3 getLightDirection(const math::vec3& moonPosition);
}
