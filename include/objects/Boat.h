// Boat.h — Traditional Bangladeshi wooden boat (Dingi Nouka / Pal Tola Nouka).
#pragma once
#include "Shader.h"
#include "mathutil.h"

namespace Boat {
    // Draws an authentic traditional Bangladeshi country boat (Dingi / Pal Tola Nouka)
    // featuring a continuous flared sheer hull with authentic sheer curve,
    // sculpted pointed Golui (prow) with auspicious painted eye, swept Pachha (stern),
    // Chhoi bamboo hood with hanging Hariken lantern, internal ribs, thwarts, floorboards,
    // and an iconic billowing sail (Pal) with bamboo mast, yard, and authentic rigging.
    void draw(Shader& shader, const math::mat4& model, bool hasPal = true, float animTime = 0.0f);

    // Releases GPU buffers for procedural hull and sail meshes.
    void cleanup();
}
