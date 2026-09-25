// Hen.cpp — Authentic Bangladeshi Village Hen (Deshi Murgi / দেশি মুরগি)
// Sculpted purely from canonical OpenGL unit primitives (spheres, cylinders, cones, cubes)
// featuring feathered hackles, 4-lobed serrated comb, dual wattles, layered wings,
// fanned upright tail feathers, and realistic splayed claws with toes and talons.

#include "objects/Hen.h"
#include "Primitives.h"

using namespace math;

namespace Hen {

void draw(Shader& shader, const mat4& model)
{
    // Organic animal shading bypass (clean vivid feathers)
    shader.setInt("uUseTexture", 0);

    // Authentic Deshi Hen Folk Palette
    vec3 bodyCol      (0.56f, 0.27f, 0.11f); // rich warm rust-brown plumage
    vec3 breastCol    (0.64f, 0.34f, 0.14f); // lighter golden-amber breast
    vec3 hackleCol    (0.74f, 0.46f, 0.16f); // golden-buff neck hackles
    vec3 wingUpperCol (0.48f, 0.22f, 0.08f); // shoulder wing covert
    vec3 wingLowerCol (0.34f, 0.15f, 0.05f); // dark flight feathers
    vec3 combCol      (0.88f, 0.09f, 0.05f); // vivid vascular crimson comb & wattles
    vec3 beakCol      (0.92f, 0.68f, 0.16f); // amber horn beak
    vec3 beakTipCol   (0.78f, 0.52f, 0.12f); // weathered beak tip
    vec3 legCol       (0.88f, 0.66f, 0.18f); // yellow scaled legs
    vec3 clawCol      (0.36f, 0.26f, 0.16f); // dark horn toe claws
    vec3 tailCol      (0.20f, 0.12f, 0.08f); // dark greenish-black tail sickles
    vec3 tailCovertCol(0.42f, 0.18f, 0.07f); // warm brown tail base coverts

    // ── 1. Plump Avian Body (Ventral Belly & Breast) ─────────────
    // Main egg-shaped torso
    mat4 body = model;
    body = translate(body, vec3(0.0f, 0.185f, -0.010f));
    body = rotate(body, radians(8.0f), vec3(1.0f, 0.0f, 0.0f));
    body = scale(body, vec3(0.128f, 0.116f, 0.168f));
    Primitives::drawSphere(shader, body, bodyCol);

    // Full, curved chest / breast bulge
    mat4 breast = model;
    breast = translate(breast, vec3(0.0f, 0.195f, 0.065f));
    breast = scale(breast, vec3(0.114f, 0.105f, 0.105f));
    Primitives::drawSphere(shader, breast, breastCol);

    // ── 2. Neck Hackles & Head ──────────────────────────────────
    // Tapered neck column angled forward
    mat4 neck = model;
    neck = translate(neck, vec3(0.0f, 0.245f, 0.080f));
    neck = rotate(neck, radians(26.0f), vec3(1.0f, 0.0f, 0.0f));
    mat4 neckS = scale(neck, vec3(0.052f, 0.095f, 0.056f));
    Primitives::drawCylinder(shader, neckS, hackleCol);

    // Neck hackle feather collar flair at base
    mat4 hackleRuff = model;
    hackleRuff = translate(hackleRuff, vec3(0.0f, 0.225f, 0.075f));
    hackleRuff = scale(hackleRuff, vec3(0.068f, 0.035f, 0.072f));
    Primitives::drawSphere(shader, hackleRuff, hackleCol * 0.94f);

    // Rounded avian head
    mat4 head = model;
    head = translate(head, vec3(0.0f, 0.295f, 0.120f));
    head = scale(head, vec3(0.056f, 0.062f, 0.064f));
    Primitives::drawSphere(shader, head, bodyCol);

    // ── 3. Iconic Multi-Lobed Serrated Red Comb (Jhut / ঝুঁটি) ────
    // 4 overlapping cascading lobes forming a realistic jagged rooster/hen crown
    float combLobes[4][3] = {
        { 0.100f, 0.335f, 0.016f }, // front brow lobe
        { 0.122f, 0.355f, 0.022f }, // tall main crest lobe
        { 0.144f, 0.362f, 0.020f }, // central peak lobe
        { 0.166f, 0.345f, 0.016f }  // rear tapering lobe
    };
    for (int i = 0; i < 4; i++) {
        mat4 lobe = model;
        lobe = translate(lobe, vec3(0.0f, combLobes[i][1], combLobes[i][0]));
        lobe = scale(lobe, vec3(0.012f, combLobes[i][2], 0.018f));
        Primitives::drawSphere(shader, lobe, combCol);
    }
    // Thin fleshy comb base blade connecting lobes
    mat4 combBlade = model;
    combBlade = translate(combBlade, vec3(0.0f, 0.332f, 0.130f));
    combBlade = scale(combBlade, vec3(0.010f, 0.018f, 0.070f));
    Primitives::drawCube(shader, combBlade, combCol * 0.92f);

    // ── 4. Dual Pendulous Wattles (Lolok / লোলক) under chin ───────
    for (int side = -1; side <= 1; side += 2) {
        float fside = (float)side;
        mat4 wattle = model;
        wattle = translate(wattle, vec3(fside * 0.010f, 0.246f, 0.150f));
        wattle = scale(wattle, vec3(0.008f, 0.022f, 0.012f));
        Primitives::drawSphere(shader, wattle, combCol);
    }

    // ── 5. Anatomical Two-Part Beak with Downward Hook Tip ───────
    // Upper mandible
    mat4 upperBeak = model;
    upperBeak = translate(upperBeak, vec3(0.0f, 0.282f, 0.174f));
    upperBeak = rotate(upperBeak, radians(95.0f), vec3(1.0f, 0.0f, 0.0f));
    upperBeak = scale(upperBeak, vec3(0.016f, 0.042f, 0.015f));
    Primitives::drawCone(shader, upperBeak, beakCol);

    // Curved sharp tip cap
    mat4 beakTip = model;
    beakTip = translate(beakTip, vec3(0.0f, 0.272f, 0.194f));
    beakTip = scale(beakTip, vec3(0.006f, 0.008f, 0.008f));
    Primitives::drawSphere(shader, beakTip, beakTipCol);

    // Lower mandible
    mat4 lowerBeak = model;
    lowerBeak = translate(lowerBeak, vec3(0.0f, 0.268f, 0.168f));
    lowerBeak = rotate(lowerBeak, radians(88.0f), vec3(1.0f, 0.0f, 0.0f));
    lowerBeak = scale(lowerBeak, vec3(0.013f, 0.032f, 0.011f));
    Primitives::drawCone(shader, lowerBeak, beakCol * 0.90f);

    // ── 6. Alert Avian Eyes with Sclera, Dark Iris & Specular Glint
    vec3 eyeScleraCol(0.96f, 0.95f, 0.90f);
    vec3 eyeIrisCol  (0.72f, 0.40f, 0.10f); // golden-orange deshi hen iris ring
    vec3 eyePupilCol (0.05f, 0.05f, 0.05f); // deep black pupil
    vec3 eyeGlintCol (1.0f, 1.0f, 1.0f);

    for (int side = -1; side <= 1; side += 2) {
        float fside = (float)side;

        // Outer white sclera
        mat4 eye = model;
        eye = translate(eye, vec3(fside * 0.050f, 0.304f, 0.144f));
        eye = rotate(eye, radians(fside * -26.0f), vec3(0.0f, 1.0f, 0.0f));
        eye = scale(eye, vec3(0.005f, 0.011f, 0.010f));
        Primitives::drawSphere(shader, eye, eyeScleraCol);

        // Golden-orange iris rim
        mat4 iris = model;
        iris = translate(iris, vec3(fside * 0.052f, 0.304f, 0.145f));
        iris = rotate(iris, radians(fside * -26.0f), vec3(0.0f, 1.0f, 0.0f));
        iris = scale(iris, vec3(0.004f, 0.008f, 0.0075f));
        Primitives::drawSphere(shader, iris, eyeIrisCol);

        // Dark pupil
        mat4 pupil = model;
        pupil = translate(pupil, vec3(fside * 0.0535f, 0.304f, 0.1455f));
        pupil = rotate(pupil, radians(fside * -26.0f), vec3(0.0f, 1.0f, 0.0f));
        pupil = scale(pupil, vec3(0.003f, 0.005f, 0.0048f));
        Primitives::drawSphere(shader, pupil, eyePupilCol);

        // Catchlight glint
        mat4 glint = model;
        glint = translate(glint, vec3(fside * 0.0545f, 0.3065f, 0.147f));
        glint = scale(glint, vec3(0.0016f, 0.0016f, 0.0016f));
        Primitives::drawSphere(shader, glint, eyeGlintCol);
    }

    // ── 7. Layered Folded Wings on Flanks ────────────────────────
    for (int side = -1; side <= 1; side += 2) {
        float fside = (float)side;

        // Shoulder wing covert pad (smooth, rounded upper wing)
        mat4 wingCovert = model;
        wingCovert = translate(wingCovert, vec3(fside * 0.118f, 0.198f, 0.010f));
        wingCovert = rotate(wingCovert, radians(fside * 14.0f), vec3(0.0f, 0.0f, 1.0f));
        wingCovert = rotate(wingCovert, radians(-8.0f), vec3(1.0f, 0.0f, 0.0f));
        wingCovert = scale(wingCovert, vec3(0.024f, 0.070f, 0.115f));
        Primitives::drawSphere(shader, wingCovert, wingUpperCol);

        // Primary flight feather quill block tapering back toward tail
        mat4 flightFeathers = model;
        flightFeathers = translate(flightFeathers, vec3(fside * 0.112f, 0.178f, -0.055f));
        flightFeathers = rotate(flightFeathers, radians(fside * 10.0f), vec3(0.0f, 0.0f, 1.0f));
        flightFeathers = rotate(flightFeathers, radians(-16.0f), vec3(1.0f, 0.0f, 0.0f));
        flightFeathers = scale(flightFeathers, vec3(0.018f, 0.046f, 0.095f));
        Primitives::drawCube(shader, flightFeathers, wingLowerCol);
    }

    // ── 8. Fan-Shaped Upright Layered Tail Feathers (Lej / লেজ) ───
    // Central primary sickle feather pointing upward at 48°
    mat4 tailMain = model;
    tailMain = translate(tailMain, vec3(0.0f, 0.252f, -0.125f));
    tailMain = rotate(tailMain, radians(-48.0f), vec3(1.0f, 0.0f, 0.0f));
    tailMain = scale(tailMain, vec3(0.022f, 0.145f, 0.055f));
    Primitives::drawCone(shader, tailMain, tailCol);

    // Left and right fanned secondary tail feathers angled outward
    for (int side = -1; side <= 1; side += 2) {
        float fside = (float)side;
        mat4 tailFan = model;
        tailFan = translate(tailFan, vec3(fside * 0.026f, 0.238f, -0.115f));
        tailFan = rotate(tailFan, radians(-42.0f), vec3(1.0f, 0.0f, 0.0f));
        tailFan = rotate(tailFan, radians(fside * 22.0f), vec3(0.0f, 0.0f, 1.0f));
        tailFan = scale(tailFan, vec3(0.018f, 0.125f, 0.046f));
        Primitives::drawCone(shader, tailFan, tailCol * 1.15f);
    }

    // Fluffy tail covert cushion supporting the base of the tail
    mat4 tailCushion = model;
    tailCushion = translate(tailCushion, vec3(0.0f, 0.210f, -0.100f));
    tailCushion = scale(tailCushion, vec3(0.065f, 0.055f, 0.065f));
    Primitives::drawSphere(shader, tailCushion, tailCovertCol);

    // ── 9. Anatomical Legs & 4-Toed Splayed Claws ─────────────────
    for (int side = -1; side <= 1; side += 2) {
        float fside = (float)side;
        float lx = fside * 0.048f;

        // Thigh feather breech (fluffy drumstick taper)
        mat4 drumstick = model;
        drumstick = translate(drumstick, vec3(lx, 0.115f, -0.010f));
        drumstick = scale(drumstick, vec3(0.038f, 0.060f, 0.044f));
        Primitives::drawSphere(shader, drumstick, bodyCol);

        // Scaled yellow tarsus shank
        mat4 shank = model;
        shank = translate(shank, vec3(lx, 0.060f, -0.005f));
        shank = scale(shank, vec3(0.010f, 0.090f, 0.010f));
        Primitives::drawCylinder(shader, shank, legCol);

        // Central front toe
        mat4 toeMid = model;
        toeMid = translate(toeMid, vec3(lx, 0.006f, 0.026f));
        toeMid = scale(toeMid, vec3(0.008f, 0.007f, 0.048f));
        Primitives::drawCube(shader, toeMid, legCol);

        // Left front toe (angled 28° outward)
        mat4 toeLeft = model;
        toeLeft = translate(toeLeft, vec3(lx - 0.014f, 0.006f, 0.022f));
        toeLeft = rotate(toeLeft, radians(-28.0f), vec3(0.0f, 1.0f, 0.0f));
        toeLeft = scale(toeLeft, vec3(0.007f, 0.007f, 0.042f));
        Primitives::drawCube(shader, toeLeft, legCol);

        // Right front toe (angled 28° inward)
        mat4 toeRight = model;
        toeRight = translate(toeRight, vec3(lx + 0.014f, 0.006f, 0.022f));
        toeRight = rotate(toeRight, radians(28.0f), vec3(0.0f, 1.0f, 0.0f));
        toeRight = scale(toeRight, vec3(0.007f, 0.007f, 0.042f));
        Primitives::drawCube(shader, toeRight, legCol);

        // Hind spur toe (facing backward)
        mat4 toeBack = model;
        toeBack = translate(toeBack, vec3(lx, 0.006f, -0.020f));
        toeBack = scale(toeBack, vec3(0.007f, 0.007f, 0.024f));
        Primitives::drawCube(shader, toeBack, legCol);

        // Claw talon on mid toe
        mat4 claw = model;
        claw = translate(claw, vec3(lx, 0.005f, 0.052f));
        claw = scale(claw, vec3(0.005f, 0.006f, 0.008f));
        Primitives::drawCone(shader, claw, clawCol);
    }
}

} // namespace Hen
