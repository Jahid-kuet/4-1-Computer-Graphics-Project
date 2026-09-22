// Boat.cpp — Traditional Bangladeshi wooden boat (Dingi Nouka)
// Features a slender curved crescent hull with swept-up pointed bow and stern,
// the iconic arched bamboo hood (Chhoi), internal wooden ribs contoured to hull,
// floorboards cleanly recessed inside the hull, and an authentic hanging Hariken lantern.

#include "objects/Boat.h"
#include "objects/Charpai.h"
#include "Primitives.h"

using namespace math;

namespace Boat {

void draw(Shader& shader, const mat4& model)
{
    // ── Traditional Boat Colors ─────────────────────────────────
    vec3 hullWood   (0.35f, 0.22f, 0.10f); // dark tarred/oiled sal wood
    vec3 gunwaleWood(0.48f, 0.32f, 0.15f); // lighter timber trim
    vec3 chhoiBamboo(0.62f, 0.52f, 0.30f); // woven bamboo matting
    vec3 ribWood    (0.30f, 0.18f, 0.08f); // inner framing ribs
    vec3 plankWood  (0.42f, 0.28f, 0.12f); // deck planks

    // ── 1. Main Hull Bottom (Crescent curved hull) ───────────────
    // Flipped hemisphere elongated along Z
    mat4 hull = model;
    hull = rotate(hull, radians(180.0f), vec3(1.0f, 0.0f, 0.0f));
    hull = scale(hull, vec3(0.74f, 0.36f, 2.40f));
    Primitives::drawHemisphere(shader, hull, hullWood);

    // ── 2. Swept-up Pointed Bow (Prow) ──────────────────────────
    // Wide base connects to hull at Z = 1.80m, slim pointed tip sweeps outward forward (+Z) & upward
    mat4 bow = model;
    bow = translate(bow, vec3(0.0f, 0.08f, 1.80f));
    bow = rotate(bow, radians(76.0f), vec3(1.0f, 0.0f, 0.0f));
    bow = scale(bow, vec3(0.36f, 1.15f, 0.15f));
    Primitives::drawCone(shader, bow, hullWood);

    // ── 3. Swept-up Pointed Stern ───────────────────────────────
    // Wide base connects to hull at Z = -1.80m, slim pointed tip sweeps outward backward (-Z) & upward
    mat4 stern = model;
    stern = translate(stern, vec3(0.0f, 0.08f, -1.80f));
    stern = rotate(stern, radians(-76.0f), vec3(1.0f, 0.0f, 0.0f));
    stern = scale(stern, vec3(0.36f, 1.15f, 0.15f));
    Primitives::drawCone(shader, stern, hullWood);

    // ── 4. Interior Wooden Floorboards (Recessed completely inside hull) ─
    // Width 0.42m and length 2.4m cleanly fit inside the tapered crescent hull
    mat4 floor = model;
    floor = translate(floor, vec3(0.0f, 0.04f, 0.0f));
    floor = scale(floor, vec3(0.42f, 0.025f, 2.40f));
    Primitives::drawCube(shader, floor, plankWood);

    // Internal structural cross-ribs (procedurally contoured to fit inside hull at each Z)
    for (int r = -3; r <= 3; r++) {
        float rz = (float)r * 0.40f;
        float factor = sqrtf(fmaxf(0.0f, 1.0f - (rz * rz) / (2.2f * 2.2f)));
        float ribW = 0.50f * factor;

        mat4 rib = model;
        rib = translate(rib, vec3(0.0f, 0.055f, rz));
        rib = scale(rib, vec3(ribW, 0.028f, 0.035f));
        Primitives::drawCube(shader, rib, ribWood);
    }

    // Boatman's seating plank at the stern (strictly contoured to inside hull width)
    mat4 sternSeat = model;
    sternSeat = translate(sternSeat, vec3(0.0f, 0.13f, -1.30f));
    sternSeat = scale(sternSeat, vec3(0.40f, 0.030f, 0.28f));
    Primitives::drawCube(shader, sternSeat, gunwaleWood);

    // ── 5. Iconic Arched Bamboo Hood (Chhoi) ─────────────────────
    // Spans the midsection of the dingi nouka, flush with gunwale width (0.74m)
    mat4 chhoi = model;
    chhoi = translate(chhoi, vec3(0.0f, 0.08f, 0.30f));
    chhoi = scale(chhoi, vec3(0.74f, 0.62f, 1.65f));
    Primitives::drawArch(shader, chhoi, chhoiBamboo);

    // Bamboo arch ribs (hoops framing the canopy ends and center)
    for (int h = -1; h <= 1; h++) {
        float hz = 0.30f + (float)h * 0.78f;
        mat4 hoop = model;
        hoop = translate(hoop, vec3(0.0f, 0.08f, hz));
        hoop = scale(hoop, vec3(0.755f, 0.63f, 0.045f));
        Primitives::drawArch(shader, hoop, ribWood);
    }

    // ── 6. Authentic Traditional Bengali Hariken (Kerosene Lantern) ─
    // Suspended from the front bamboo hoop of the Chhoi at Z = 1.08m
    mat4 boatLantern = model;
    boatLantern = translate(boatLantern, vec3(0.0f, 0.26f, 1.08f));
    boatLantern = scale(boatLantern, vec3(0.82f, 0.82f, 0.82f));
    Charpai::drawLantern(shader, boatLantern);

    // Wire hanger connecting lantern bail to the overhead bamboo arch
    mat4 hanger = model;
    hanger = translate(hanger, vec3(0.0f, 0.54f, 1.08f));
    hanger = scale(hanger, vec3(0.008f, 0.08f, 0.008f));
    Primitives::drawCylinder(shader, hanger, vec3(0.25f, 0.25f, 0.25f));
}

} // namespace Boat
