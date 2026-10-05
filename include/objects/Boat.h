// Boat.h — Traditional Bangladeshi wooden boat (Dingi Nouka / Pal Tola Nouka).
#pragma once
#include "Shader.h"
#include "mathutil.h"

namespace Boat {
    // Distinct traditional boat styles matching folk artwork:
    // 1. BOAT_STYLE_RED_SAIL: Triangular white sail with red folk accent patch & standing Majhi
    // 2. BOAT_STYLE_ROUND_CHHOI: Large rounded golden-straw dome Chhoi hood & rowing Majhi with Boitha oar
    // 3. BOAT_STYLE_WHITE_SAIL: Tall mast with pristine white triangular sail & swept prows
    enum BoatStyle {
        BOAT_STYLE_RED_SAIL    = 0,
        BOAT_STYLE_ROUND_CHHOI = 1,
        BOAT_STYLE_WHITE_SAIL  = 2
    };

    // Draws an authentic Bangladeshi Dingi boat in the requested style
    // numPassengers: number of passengers seated on the forward thwart (default -1: 2 for BOAT_STYLE_ROUND_CHHOI, 0 for others)
    void draw(Shader& shader, const math::mat4& model, BoatStyle style = BOAT_STYLE_RED_SAIL, float animTime = 0.0f, float oarAnim = 0.0f, int numPassengers = -1);

    // Dedicated passenger draw function (draws 2 authentic Bengali passengers + market basket & clay pot)
    void drawPassengers(Shader& shader, const math::mat4& model, float animTime = 0.0f);

    // Backwards-compatible overload
    void draw(Shader& shader, const math::mat4& model, bool hasPal, float animTime = 0.0f);

    // Releases GPU buffers for procedural meshes.
    void cleanup();
}
