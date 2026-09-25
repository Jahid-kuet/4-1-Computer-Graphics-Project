// Duck.cpp — Authentic Bangladeshi River Duck (Patihash / পাতিহাঁস)
// Sculpted purely from canonical OpenGL unit primitives (spheres, cylinders, cones, cubes)
// featuring buoyant watercraft body hull, graceful arched S-neck with white collar ring,
// broad spatula bill with nail, layered wings with iridescent blue speculum bar,
// upturned drake tail feathers, and wide orange webbed feet.

#include "objects/Duck.h"
#include "Primitives.h"

using namespace math;

namespace Duck {

void draw(Shader& shader, const mat4& model)
{
    // Organic animal shading bypass (clean plumage & bill)
    shader.setInt("uUseTexture", 0);

    // Authentic Bengali River Duck Palette (Mallard / Deshi Patihash Cross)
    vec3 bodyCol      (0.86f, 0.84f, 0.79f); // warm unbleached off-white/cream plumage
    vec3 breastCol    (0.44f, 0.24f, 0.14f); // rich warm chestnut-brown breast
    vec3 headCol      (0.10f, 0.32f, 0.22f); // iridescent deep bottle-green head
    vec3 collarCol    (0.96f, 0.96f, 0.94f); // crisp white neck ring (Kolar mala)
    vec3 billCol      (0.95f, 0.65f, 0.12f); // warm golden-orange spatula bill
    vec3 billNailCol  (0.24f, 0.18f, 0.12f); // dark terminal bill nail
    vec3 wingCovertCol(0.72f, 0.70f, 0.65f); // ash-grey wing coverts
    vec3 speculumCol  (0.12f, 0.28f, 0.58f); // iridescent cobalt-blue speculum wing patch
    vec3 specWhiteCol (0.95f, 0.95f, 0.92f); // white speculum border bar
    vec3 tailCol      (0.32f, 0.28f, 0.24f); // dark slate-brown tail feathers
    vec3 curlCol      (0.12f, 0.12f, 0.12f); // glossy black drake tail curl
    vec3 legCol       (0.94f, 0.52f, 0.10f); // bright orange webbed feet

    // ── 1. Buoyant Watercraft Body (Flat Ventral Hull & Arched Back) ──
    // Main buoyant oval body
    mat4 body = model;
    body = translate(body, vec3(0.0f, 0.240f, -0.010f));
    body = rotate(body, radians(5.0f), vec3(1.0f, 0.0f, 0.0f));
    body = scale(body, vec3(0.240f, 0.170f, 0.360f));
    Primitives::drawSphere(shader, body, bodyCol);

    // Deep rounded chestnut breast bulge
    mat4 breast = model;
    breast = translate(breast, vec3(0.0f, 0.245f, 0.155f));
    breast = scale(breast, vec3(0.210f, 0.160f, 0.150f));
    Primitives::drawSphere(shader, breast, breastCol);

    // Flattened ventral water keel
    mat4 keel = model;
    keel = translate(keel, vec3(0.0f, 0.170f, 0.000f));
    keel = scale(keel, vec3(0.190f, 0.060f, 0.310f));
    Primitives::drawCube(shader, keel, bodyCol * 0.94f);

    // ── 2. Graceful Arched S-Neck & Head ─────────────────────────
    // Lower neck rising upward and forward from breast
    mat4 lowerNeck = model;
    lowerNeck = translate(lowerNeck, vec3(0.0f, 0.320f, 0.175f));
    lowerNeck = rotate(lowerNeck, radians(-18.0f), vec3(1.0f, 0.0f, 0.0f));
    mat4 lowerNeckS = scale(lowerNeck, vec3(0.072f, 0.120f, 0.080f));
    Primitives::drawCylinder(shader, lowerNeckS, headCol);

    // Upper neck curving forward into head
    mat4 upperNeck = model;
    upperNeck = translate(upperNeck, vec3(0.0f, 0.395f, 0.220f));
    upperNeck = rotate(upperNeck, radians(22.0f), vec3(1.0f, 0.0f, 0.0f));
    mat4 upperNeckS = scale(upperNeck, vec3(0.065f, 0.090f, 0.072f));
    Primitives::drawCylinder(shader, upperNeckS, headCol);

    // Crisp white neck ring collar (Kolar mala / গলার মালা)
    mat4 collar = model;
    collar = translate(collar, vec3(0.0f, 0.355f, 0.190f));
    collar = rotate(collar, radians(6.0f), vec3(1.0f, 0.0f, 0.0f));
    mat4 collarS = scale(collar, vec3(0.070f, 0.022f, 0.076f));
    Primitives::drawCylinder(shader, collarS, collarCol);

    // Sleek iridescent green head
    mat4 head = model;
    head = translate(head, vec3(0.0f, 0.445f, 0.265f));
    head = scale(head, vec3(0.082f, 0.088f, 0.115f));
    Primitives::drawSphere(shader, head, headCol);

    // Cheeks & brow flare
    mat4 cheek = model;
    cheek = translate(cheek, vec3(0.0f, 0.438f, 0.250f));
    cheek = scale(cheek, vec3(0.094f, 0.078f, 0.095f));
    Primitives::drawSphere(shader, cheek, headCol * 0.95f);

    // ── 3. Broad Flat Spatula Duck Bill with Nail ────────────────
    // Broad flat upper bill mandible
    mat4 bill = model;
    bill = translate(bill, vec3(0.0f, 0.428f, 0.355f));
    bill = rotate(bill, radians(84.0f), vec3(1.0f, 0.0f, 0.0f));
    bill = scale(bill, vec3(0.055f, 0.110f, 0.022f));
    Primitives::drawCone(shader, bill, billCol);

    // Flat wide spatulate bill base plate
    mat4 billPlate = model;
    billPlate = translate(billPlate, vec3(0.0f, 0.424f, 0.335f));
    billPlate = scale(billPlate, vec3(0.052f, 0.016f, 0.065f));
    Primitives::drawCube(shader, billPlate, billCol);

    // Terminal dark bill nail (beak hook tip)
    mat4 billNail = model;
    billNail = translate(billNail, vec3(0.0f, 0.418f, 0.408f));
    billNail = scale(billNail, vec3(0.015f, 0.008f, 0.014f));
    Primitives::drawSphere(shader, billNail, billNailCol);

    // Lateral nostril depressions
    for (int side = -1; side <= 1; side += 2) {
        float fside = (float)side;
        mat4 nostril = model;
        nostril = translate(nostril, vec3(fside * 0.012f, 0.434f, 0.340f));
        nostril = scale(nostril, vec3(0.004f, 0.004f, 0.012f));
        Primitives::drawCube(shader, nostril, billCol * 0.72f);
    }

    // ── 4. Alert Avian Eyes with Sclera & Dark Pupil ─────────────
    vec3 eyeScleraCol(0.96f, 0.95f, 0.90f);
    vec3 eyePupilCol (0.05f, 0.05f, 0.05f);
    vec3 eyeGlintCol (1.0f, 1.0f, 1.0f);

    for (int side = -1; side <= 1; side += 2) {
        float fside = (float)side;

        // White eye ring
        mat4 eye = model;
        eye = translate(eye, vec3(fside * 0.076f, 0.458f, 0.285f));
        eye = rotate(eye, radians(fside * -24.0f), vec3(0.0f, 1.0f, 0.0f));
        eye = scale(eye, vec3(0.005f, 0.015f, 0.014f));
        Primitives::drawSphere(shader, eye, eyeScleraCol);

        // Gloss black pupil
        mat4 pupil = model;
        pupil = translate(pupil, vec3(fside * 0.079f, 0.458f, 0.286f));
        pupil = rotate(pupil, radians(fside * -24.0f), vec3(0.0f, 1.0f, 0.0f));
        pupil = scale(pupil, vec3(0.0035f, 0.0085f, 0.0080f));
        Primitives::drawSphere(shader, pupil, eyePupilCol);

        // Specular glint
        mat4 glint = model;
        glint = translate(glint, vec3(fside * 0.0805f, 0.461f, 0.288f));
        glint = scale(glint, vec3(0.002f, 0.002f, 0.002f));
        Primitives::drawSphere(shader, glint, eyeGlintCol);
    }

    // ── 5. Layered Folded Wings with Iridescent Speculum Bar ─────
    for (int side = -1; side <= 1; side += 2) {
        float fside = (float)side;

        // Upper wing covert pad
        mat4 wingCovert = model;
        wingCovert = translate(wingCovert, vec3(fside * 0.210f, 0.260f, 0.010f));
        wingCovert = rotate(wingCovert, radians(fside * 14.0f), vec3(0.0f, 0.0f, 1.0f));
        wingCovert = rotate(wingCovert, radians(-8.0f), vec3(1.0f, 0.0f, 0.0f));
        wingCovert = scale(wingCovert, vec3(0.038f, 0.115f, 0.220f));
        Primitives::drawSphere(shader, wingCovert, wingCovertCol);

        // Iridescent blue speculum patch (the signature hallmark of authentic ducks!)
        mat4 speculum = model;
        speculum = translate(speculum, vec3(fside * 0.218f, 0.245f, -0.060f));
        speculum = rotate(speculum, radians(fside * 12.0f), vec3(0.0f, 0.0f, 1.0f));
        speculum = rotate(speculum, radians(-14.0f), vec3(1.0f, 0.0f, 0.0f));
        speculum = scale(speculum, vec3(0.016f, 0.048f, 0.090f));
        Primitives::drawCube(shader, speculum, speculumCol);

        // White border trim bars enclosing the speculum
        mat4 specBar = model;
        specBar = translate(specBar, vec3(fside * 0.220f, 0.266f, -0.058f));
        specBar = rotate(specBar, radians(fside * 12.0f), vec3(0.0f, 0.0f, 1.0f));
        specBar = rotate(specBar, radians(-14.0f), vec3(1.0f, 0.0f, 0.0f));
        specBar = scale(specBar, vec3(0.014f, 0.010f, 0.088f));
        Primitives::drawCube(shader, specBar, specWhiteCol);

        // Primary flight feather quills tapering toward stern
        mat4 primaries = model;
        primaries = translate(primaries, vec3(fside * 0.180f, 0.240f, -0.160f));
        primaries = rotate(primaries, radians(fside * 8.0f), vec3(0.0f, 0.0f, 1.0f));
        primaries = rotate(primaries, radians(-20.0f), vec3(1.0f, 0.0f, 0.0f));
        primaries = scale(primaries, vec3(0.024f, 0.045f, 0.140f));
        Primitives::drawCube(shader, primaries, tailCol);
    }

    // ── 6. Upturned Wedge Tail & Drake Tail Curls ────────────────
    // Main pointed wedge tail angled up at 32°
    mat4 tail = model;
    tail = translate(tail, vec3(0.0f, 0.285f, -0.280f));
    tail = rotate(tail, radians(-34.0f), vec3(1.0f, 0.0f, 0.0f));
    tail = scale(tail, vec3(0.085f, 0.190f, 0.055f));
    Primitives::drawCone(shader, tail, tailCol);

    // Glossy black drake curls above tail
    mat4 curl = model;
    curl = translate(curl, vec3(0.0f, 0.335f, -0.230f));
    curl = scale(curl, vec3(0.024f, 0.040f, 0.038f));
    Primitives::drawSphere(shader, curl, curlCol);

    // ── 7. Scaled Legs & Wide Orange Webbed Feet (Latha Pa) ──────
    for (int side = -1; side <= 1; side += 2) {
        float fside = (float)side;
        float lx = fside * 0.100f;

        // Thigh drumstick feather taper at body base
        mat4 thigh = model;
        thigh = translate(thigh, vec3(lx, 0.160f, -0.010f));
        thigh = scale(thigh, vec3(0.055f, 0.080f, 0.065f));
        Primitives::drawSphere(shader, thigh, bodyCol);

        // Scaled orange shank cylinder
        mat4 leg = model;
        leg = translate(leg, vec3(lx, 0.085f, 0.000f));
        leg = scale(leg, vec3(0.016f, 0.130f, 0.016f));
        Primitives::drawCylinder(shader, leg, legCol);

        // Wide triangular webbed foot paddle resting on ground
        mat4 footWeb = model;
        footWeb = translate(footWeb, vec3(lx, 0.008f, 0.045f));
        footWeb = scale(footWeb, vec3(0.082f, 0.008f, 0.095f));
        Primitives::drawCube(shader, footWeb, legCol);

        // 3 Distinct forward webbed toe ridges
        float toeAngles[3] = { -24.0f, 0.0f, 24.0f };
        for (int t = 0; t < 3; t++) {
            mat4 toe = model;
            toe = translate(toe, vec3(lx, 0.010f, 0.025f));
            toe = rotate(toe, radians(toeAngles[t]), vec3(0.0f, 1.0f, 0.0f));
            toe = translate(toe, vec3(0.0f, 0.0f, 0.035f));
            toe = scale(toe, vec3(0.010f, 0.010f, 0.075f));
            Primitives::drawCube(shader, toe, legCol * 0.92f);
        }

        // Small rear claw toe
        mat4 backToe = model;
        backToe = translate(backToe, vec3(lx, 0.010f, -0.020f));
        backToe = scale(backToe, vec3(0.008f, 0.008f, 0.025f));
        Primitives::drawCube(shader, backToe, legCol * 0.90f);
    }
}

} // namespace Duck
