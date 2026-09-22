// Boatman.cpp — Traditional Bangladeshi Majhi (boatman) seated holding an oar.
// Featuring seated pose with red lungi, traditional bamboo conical hat (Mathal),
// and a long steering oar (Boitha) extending into the water.

#include "objects/Boatman.h"
#include "objects/Person.h"
#include "Primitives.h"

using namespace math;

namespace Boatman {

void draw(Shader& shader, const mat4& model, float oarAngle)
{
    // ── 1. Seated Majhi (Boatman) ────────────────────────────────
    PersonParams bp;
    bp.skinColor   = vec3(0.48f, 0.32f, 0.20f);  // warm sun-tanned skin
    bp.shirtColor  = vec3(0.85f, 0.82f, 0.75f);  // off-white cotton vest
    bp.pantsColor  = vec3(0.58f, 0.14f, 0.08f);  // traditional checkered red/maroon lungi
    bp.seated      = true;
    bp.rightArmAngle = radians(-35.0f) + oarAngle; // reaches forward to grasp the oar
    bp.leftArmAngle  = radians(-20.0f);            // resting on knee/gunwale

    Person::draw(shader, model, bp);

    // Seated person coordinate reference:
    // torsoBase = 0.05, torsoH = 0.40, headCenterY = 0.595, head top = 0.715
    const float headCenterY = 0.595f;
    const float headR       = 0.12f;

    // ── 2. Traditional Conical Bamboo Sunhat (Mathal) ────────────
    vec3 hatColor(0.68f, 0.56f, 0.32f); // woven dried bamboo/palm leaves
    vec3 hatRim  (0.45f, 0.35f, 0.18f); // bamboo rim trim

    mat4 hat = model;
    hat = translate(hat, vec3(0.0f, headCenterY + headR * 0.75f, 0.0f));
    hat = rotate(hat, radians(8.0f), vec3(1.0f, 0.0f, 0.0f)); // slight rakish tilt
    hat = scale(hat, vec3(0.32f, 0.14f, 0.32f));
    Primitives::drawCone(shader, hat, hatColor);

    // Hat rim ring
    mat4 hRim = model;
    hRim = translate(hRim, vec3(0.0f, headCenterY + headR * 0.75f, 0.0f));
    hRim = rotate(hRim, radians(8.0f), vec3(1.0f, 0.0f, 0.0f));
    hRim = scale(hRim, vec3(0.33f, 0.02f, 0.33f));
    Primitives::drawCylinder(shader, hRim, hatRim);

    // ── 3. Long Steering Oar (Boitha) ────────────────────────────
    vec3 shaftCol(0.40f, 0.25f, 0.10f); // dark seasoned timber
    vec3 bladeCol(0.48f, 0.32f, 0.14f); // wide carved wooden blade

    mat4 oarPivot = model;
    // Pivot located beside the boatman's hands, resting across the starboard gunwale
    oarPivot = translate(oarPivot, vec3(0.26f, 0.26f, 0.12f));
    oarPivot = rotate(oarPivot, radians(-38.0f) + oarAngle, vec3(1.0f, 0.0f, 0.0f));
    oarPivot = rotate(oarPivot, radians(18.0f), vec3(0.0f, 0.0f, 1.0f));

    // Oar Shaft (long cylinder)
    mat4 shaft = oarPivot;
    shaft = translate(shaft, vec3(0.0f, -0.70f, 0.0f));
    shaft = scale(shaft, vec3(0.025f, 1.60f, 0.025f));
    Primitives::drawCylinder(shader, shaft, shaftCol);

    // Oar Blade (flat paddle entering water)
    mat4 blade = oarPivot;
    blade = translate(blade, vec3(0.0f, -1.60f, 0.0f));
    blade = scale(blade, vec3(0.16f, 0.38f, 0.02f));
    Primitives::drawCube(shader, blade, bladeCol);
}

} // namespace Boatman
