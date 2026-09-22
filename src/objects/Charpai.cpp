// Charpai.cpp — Traditional woven bed and hand-fan built from
//               cylinder legs, cuboid frame, criss-cross weave, and fan blades.

#include "objects/Charpai.h"
#include "Primitives.h"

using namespace math;

namespace Charpai {

void draw(Shader& shader, const mat4& model)
{
    vec3 legCol  (0.40f, 0.25f, 0.10f);   // wood
    vec3 frameCol(0.45f, 0.30f, 0.12f);   // wood frame
    vec3 weaveCol(0.58f, 0.52f, 0.32f);   // jute / rope

    // Charpai dimensions
    float bedW = 1.2f;   // width (X)
    float bedL = 2.0f;   // length (Z)
    float bedH = 0.5f;   // height (Y)
    float legR = 0.04f;

    // ── Four legs ───────────────────────────────────────────────
    float hx = bedW * 0.5f - 0.05f;
    float hz = bedL * 0.5f - 0.05f;
    float legPos[4][2] = { {-hx, -hz}, {hx, -hz}, {hx, hz}, {-hx, hz} };

    for (int i = 0; i < 4; i++) {
        mat4 leg = model;
        leg = translate(leg, vec3(legPos[i][0], bedH * 0.5f, legPos[i][1]));
        leg = scale(leg, vec3(legR, bedH, legR));
        Primitives::drawCylinder(shader, leg, legCol);
    }

    // ── Frame rails (4 cuboids around the edges) ────────────────
    // Long sides (Z direction)
    for (int side = -1; side <= 1; side += 2) {
        mat4 rail = model;
        rail = translate(rail, vec3(side * hx, bedH, 0.0f));
        rail = scale(rail, vec3(0.05f, 0.05f, bedL));
        Primitives::drawCube(shader, rail, frameCol);
    }
    // Short sides (X direction)
    for (int side = -1; side <= 1; side += 2) {
        mat4 rail = model;
        rail = translate(rail, vec3(0.0f, bedH, side * hz));
        rail = scale(rail, vec3(bedW, 0.05f, 0.05f));
        Primitives::drawCube(shader, rail, frameCol);
    }

    // ── Criss-cross weave (thin cuboids across the top) ─────────
    int stripes = 8;
    for (int i = 0; i < stripes; i++) {
        float t = (float)i / (stripes - 1);
        float z = -hz + t * bedL * 0.9f + 0.05f;
        mat4 strip = model;
        strip = translate(strip, vec3(0.0f, bedH - 0.02f, z));
        strip = scale(strip, vec3(bedW * 0.9f, 0.015f, 0.03f));
        Primitives::drawCube(shader, strip, weaveCol);
    }
    for (int i = 0; i < 5; i++) {
        float t = (float)i / 4.0f;
        float x = -hx * 0.85f + t * bedW * 0.85f;
        mat4 strip = model;
        strip = translate(strip, vec3(x, bedH - 0.02f, 0.0f));
        strip = scale(strip, vec3(0.03f, 0.015f, bedL * 0.9f));
        Primitives::drawCube(shader, strip, weaveCol);
    }
}

void drawFan(Shader& shader, const mat4& model, float)
{
    // Traditional Bangladeshi Handmade Hand Fan (Haat Pakha / হাতপাখা)
    // Handwoven palm-leaf (Tal pata) with bamboo handle and Nakshi trim
    vec3 handleCol  (0.48f, 0.35f, 0.18f); // bamboo handle
    vec3 palmLeafCol(0.78f, 0.68f, 0.44f); // natural woven palm leaf
    vec3 rimCol     (0.55f, 0.40f, 0.20f); // bent split-bamboo outer rim
    vec3 nakshiRed  (0.78f, 0.22f, 0.16f); // crimson embroidered thread
    vec3 nakshiGreen(0.22f, 0.48f, 0.24f); // forest green woven accent
    vec3 frillCol   (0.88f, 0.35f, 0.18f); // scalloped border lace / frill

    // ── 1. Bamboo Handle & Spine ─────────────────────────────────
    // Grip section extending below the blade
    mat4 handle = model;
    handle = translate(handle, vec3(0.0f, -0.16f, 0.0f));
    handle = scale(handle, vec3(0.012f, 0.32f, 0.012f));
    Primitives::drawCylinder(shader, handle, handleCol);

    // Spine running up through the back of the woven blade
    mat4 spine = model;
    spine = translate(spine, vec3(0.0f, 0.08f, -0.004f));
    spine = scale(spine, vec3(0.010f, 0.24f, 0.008f));
    Primitives::drawCylinder(shader, spine, handleCol);

    // ── 2. Woven Palm-Leaf Blade (Round/Heart-shaped disk) ───────
    mat4 blade = model;
    blade = translate(blade, vec3(0.0f, 0.12f, 0.0f));
    blade = scale(blade, vec3(0.18f, 0.18f, 0.008f));
    Primitives::drawCylinder(shader, blade, palmLeafCol);

    // ── 3. Split-Bamboo Outer Rim Hoop ───────────────────────────
    mat4 rim = model;
    rim = translate(rim, vec3(0.0f, 0.12f, 0.0f));
    rim = scale(rim, vec3(0.186f, 0.186f, 0.012f));
    Primitives::drawCylinder(shader, rim, rimCol);

    // ── 4. Concentric Nakshi Embroidered Weave Patterns ──────────
    // Outer decorative ring (crimson thread)
    mat4 ring1 = model;
    ring1 = translate(ring1, vec3(0.0f, 0.12f, 0.005f));
    ring1 = scale(ring1, vec3(0.14f, 0.14f, 0.004f));
    Primitives::drawCylinder(shader, ring1, nakshiRed);

    // Inner decorative ring (green woven pattern)
    mat4 ring2 = model;
    ring2 = translate(ring2, vec3(0.0f, 0.12f, 0.006f));
    ring2 = scale(ring2, vec3(0.09f, 0.09f, 0.004f));
    Primitives::drawCylinder(shader, ring2, nakshiGreen);

    // Center button / rosette hub
    mat4 center = model;
    center = translate(center, vec3(0.0f, 0.12f, 0.007f));
    center = scale(center, vec3(0.035f, 0.035f, 0.006f));
    Primitives::drawCylinder(shader, center, nakshiRed);

    // ── 5. Scalloped Outer Cloth Frills (ঝালর) ───────────────────
    for (int f = 0; f < 8; f++) {
        float ang = (float)f * (2.0f * PI / 8.0f);
        float fx = cosf(ang) * 0.188f;
        float fy = 0.12f + sinf(ang) * 0.188f;
        mat4 frill = model;
        frill = translate(frill, vec3(fx, fy, 0.0f));
        frill = scale(frill, vec3(0.022f, 0.022f, 0.006f));
        Primitives::drawSphere(shader, frill, frillCol);
    }
}

void drawLantern(Shader& shader, const mat4& model)
{
    // Traditional Bangladeshi Kerosene Hurricane Lantern (Hariken / হ্যারিকেন)
    // Sized to authentic proportions (~16cm base diameter, ~34cm height)
    vec3 tinMetalCol (0.24f, 0.25f, 0.27f); // stamped dark painted tinplate
    vec3 brassBurner (0.72f, 0.54f, 0.18f); // brass wick burner mechanism
    vec3 flameGlow   (1.00f, 0.88f, 0.38f); // luminous warm kerosene flame
    vec3 chimneyGlass(0.90f, 0.94f, 0.98f); // blown glass globe

    // ── 1. Fuel Reservoir Tank (Base) ────────────────────────────
    // Lower tank body
    mat4 tank = model;
    tank = translate(tank, vec3(0.0f, 0.045f, 0.0f));
    tank = scale(tank, vec3(0.12f, 0.09f, 0.12f));
    Primitives::drawCylinder(shader, tank, tinMetalCol);

    // Stepped beveled tank shoulder
    mat4 shoulder = model;
    shoulder = translate(shoulder, vec3(0.0f, 0.095f, 0.0f));
    shoulder = scale(shoulder, vec3(0.095f, 0.025f, 0.095f));
    Primitives::drawCylinder(shader, shoulder, tinMetalCol * 0.9f);

    // Filler cap on side of tank
    mat4 cap = model;
    cap = translate(cap, vec3(0.07f, 0.095f, 0.0f));
    cap = scale(cap, vec3(0.020f, 0.025f, 0.020f));
    Primitives::drawCylinder(shader, cap, brassBurner);

    // ── 2. Brass Burner Collar & Thumb Key ───────────────────────
    mat4 collar = model;
    collar = translate(collar, vec3(0.0f, 0.12f, 0.0f));
    collar = scale(collar, vec3(0.065f, 0.035f, 0.065f));
    Primitives::drawCylinder(shader, collar, brassBurner);

    // Wick adjuster knob extending to the right
    mat4 knob = model;
    knob = translate(knob, vec3(0.075f, 0.12f, 0.0f));
    knob = rotate(knob, radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
    knob = scale(knob, vec3(0.012f, 0.025f, 0.012f));
    Primitives::drawCylinder(shader, knob, brassBurner);

    // ── 3. Glowing Kerosene Flame (Little Light Core) ─────────────
    shader.setFloat("emissive", 1.0f);
    mat4 flame = model;
    flame = translate(flame, vec3(0.0f, 0.185f, 0.0f));
    flame = scale(flame, vec3(0.032f, 0.065f, 0.032f));
    Primitives::drawSphere(shader, flame, flameGlow);

    // Inner bright yellow flame tip
    mat4 tip = model;
    tip = translate(tip, vec3(0.0f, 0.225f, 0.0f));
    tip = scale(tip, vec3(0.018f, 0.040f, 0.018f));
    Primitives::drawCone(shader, tip, vec3(1.0f, 0.96f, 0.60f));
    shader.setFloat("emissive", 0.0f);

    // ── 4. Pear-shaped Glass Chimney Globe ───────────────────────
    // Glass globe bulb
    mat4 globe = model;
    globe = translate(globe, vec3(0.0f, 0.20f, 0.0f));
    globe = scale(globe, vec3(0.082f, 0.11f, 0.082f));
    Primitives::drawSphere(shader, globe, chimneyGlass);

    // Chimney glass neck
    mat4 neck = model;
    neck = translate(neck, vec3(0.0f, 0.26f, 0.0f));
    neck = scale(neck, vec3(0.055f, 0.06f, 0.055f));
    Primitives::drawCylinder(shader, neck, chimneyGlass);

    // ── 5. Twin Tubular Side Air Pipes ───────────────────────────
    for (int side = -1; side <= 1; side += 2) {
        float fside = (float)side;
        mat4 pipe = model;
        pipe = translate(pipe, vec3(fside * 0.105f, 0.20f, 0.0f));
        pipe = scale(pipe, vec3(0.014f, 0.23f, 0.014f));
        Primitives::drawCylinder(shader, pipe, tinMetalCol);

        // Lower bend into tank
        mat4 bend = model;
        bend = translate(bend, vec3(fside * 0.085f, 0.085f, 0.0f));
        bend = rotate(bend, radians(fside * 45.0f), vec3(0.0f, 0.0f, 1.0f));
        bend = scale(bend, vec3(0.014f, 0.06f, 0.014f));
        Primitives::drawCylinder(shader, bend, tinMetalCol);
    }

    // ── 6. Vented Tin Smoke Canopy & Top Ring ────────────────────
    mat4 canopy = model;
    canopy = translate(canopy, vec3(0.0f, 0.30f, 0.0f));
    canopy = scale(canopy, vec3(0.10f, 0.045f, 0.10f));
    Primitives::drawCone(shader, canopy, tinMetalCol);

    mat4 crown = model;
    crown = translate(crown, vec3(0.0f, 0.335f, 0.0f));
    crown = scale(crown, vec3(0.048f, 0.032f, 0.048f));
    Primitives::drawCylinder(shader, crown, tinMetalCol * 0.85f);

    // ── 7. Arched Wire Carrying Bail Handle ───────────────────────
    mat4 bail = model;
    bail = translate(bail, vec3(0.0f, 0.32f, 0.0f));
    bail = scale(bail, vec3(0.20f, 0.18f, 0.012f));
    Primitives::drawArch(shader, bail, tinMetalCol);
}

} // namespace Charpai

