// Sun.h — Radiant procedural Sun (Surjo / সূর্য) with glowing core, corona halo, and dynamic solar flare rays.
#pragma once
#include "Shader.h"
#include "mathutil.h"

namespace Sun {
    // Draws the radiant sun with glowing core, photosphere, chromosphere, corona disc, and animated sunbeam flare rays.
    void draw(Shader& shader, const math::mat4& model, float animTime = 0.0f);

    // Returns the direction vector FROM the sun toward the scene origin for directional sunlight.
    math::vec3 getLightDirection(const math::vec3& sunPosition);
}
