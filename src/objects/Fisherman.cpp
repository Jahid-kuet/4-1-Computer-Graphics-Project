// Fisherman.cpp — Traditional Bangladeshi River Fisherman hunting fish (নদীতে মাছ শিকারী জেলে)
// Pure OpenGL 3.3 Core Profile procedural modeling using unit primitives (cube, triangle, sphere).
// Authentic rural Bengali cultural elements:
// - Sleek wooden fishing dingi boat (জেলে ডিঙি) with folk stripe and horn prow/stern
// - Standing athletic fisherman figure in tucked lungi (মালকোঁচা) and red gamcha headband (মাথায় বাঁধা গামছা)
// - Expanding circular cast net (ঝাঁকি জাল / খেপলা জাল / Khepla Jal) cast forward into open river
// - Magnificent silver river fish leaping in mid-air (লাফানো রূপালী মাছ / পদ্মার রূপালী ইলিশ)
// - Traditional fishing gear: woven bamboo Khalui (খালুই), Polo trap (পলো), Teta spear (টেঁটা), and Hariken lantern

#include "objects/Fisherman.h"
#include "Primitives.h"
#include <cmath>

using namespace math;

namespace {

// Helper: constructs a 4x4 matrix mapping canonical Unit Triangle (base [-0.5, 0.5] along X at Y=0, apex (0, 1, 0))
// to arbitrary 3D triangle with vertices A, B, C:
inline mat4 makeTriangleMatrix(const vec3& A, const vec3& B, const vec3& C)
{
    vec3 c0 = B - A;
    vec3 c3 = (A + B) * 0.5f;
    vec3 c1 = C - c3;
    vec3 c2 = cross(c0, c1);
    float len = c2.length();
    if (len > 1e-6f) c2 = c2 / len;
    else c2 = vec3(0.0f, 0.0f, 1.0f);

    mat4 M;
    M(0, 0) = c0.x; M(1, 0) = c0.y; M(2, 0) = c0.z; M(3, 0) = 0.0f;
    M(0, 1) = c1.x; M(1, 1) = c1.y; M(2, 1) = c1.z; M(3, 1) = 0.0f;
    M(0, 2) = c2.x; M(1, 2) = c2.y; M(2, 2) = c2.z; M(3, 2) = 0.0f;
    M(0, 3) = c3.x; M(1, 3) = c3.y; M(2, 3) = c3.z; M(3, 3) = 1.0f;
    return M;
}

inline void draw3DTriangle(Shader& shader, const mat4& parentModel,
                           const vec3& A, const vec3& B, const vec3& C,
                           const vec3& color)
{
    mat4 M = makeTriangleMatrix(A, B, C);
    Primitives::drawTriangle(shader, parentModel * M, color);
}

inline void draw3DQuad(Shader& shader, const mat4& parentModel,
                       const vec3& A, const vec3& B, const vec3& C, const vec3& D,
                       const vec3& color)
{
    draw3DTriangle(shader, parentModel, A, B, C, color);
    draw3DTriangle(shader, parentModel, A, C, D, color);
}

// ── Line Cylinder Helper ──────────────────────────────────────────
// Draws a cylindrical segment connecting point p1 to point p2 exactly
inline void drawLineCylinder(Shader& shader, const mat4& base, const vec3& p1, const vec3& p2, float radius, const vec3& color)
{
    vec3 diff = p2 - p1;
    float len = diff.length();
    if (len < 1e-4f) return;

    vec3 dir = diff / len;
    vec3 mid = (p1 + p2) * 0.5f;

    mat4 m = base;
    m = translate(m, mid);

    // Unit cylinder height is along Y [-0.5, 0.5]. Orient from +Y to dir.
    vec3 up(0.0f, 1.0f, 0.0f);
    float dotP = dot(up, dir);
    if (dotP > 0.9999f) {
        // Aligned with +Y
    } else if (dotP < -0.9999f) {
        // Exactly opposite (-Y)
        m = rotate(m, radians(180.0f), vec3(1.0f, 0.0f, 0.0f));
    } else {
        vec3 axis = normalize(cross(up, dir));
        float angle = acosf(fmaxf(-1.0f, fminf(1.0f, dotP)));
        m = rotate(m, angle, axis);
    }

    m = scale(m, vec3(radius, len, radius));
    Primitives::drawCylinder(shader, m, color);
}

// ── Fish Modeling Helper ──────────────────────────────────────────
// Crafts an authentic freshwater river fish (e.g. Rupali Ilish / Rui)
// Streamlined spindle body, dorsal spine, operculum gills, fins, expressive eyes, and flexing tail
void renderFishModel(Shader& shader, const mat4& model, float wiggle, float scaleVal)
{
    shader.setInt("uUseTexture", 0);

    const vec3 silverScale (0.88f, 0.93f, 0.98f); // Shimmering silver flank / belly
    const vec3 dorsalBlue  (0.28f, 0.48f, 0.70f); // Iridescent river-blue dorsal spine
    const vec3 finCrimson  (0.92f, 0.38f, 0.22f); // Translucent crimson-orange fins
    const vec3 eyePupil    (0.06f, 0.06f, 0.06f); // Gloss dark pupil
    const vec3 eyeRing     (0.96f, 0.84f, 0.22f); // Golden iris rim

    mat4 root = model;
    root = scale(root, vec3(scaleVal));

    // 1. Spindle Ellipsoid Body (Unit Sphere: radius 1 -> width 0.09m, height 0.16m, length 0.40m)
    mat4 body = root;
    body = scale(body, vec3(0.045f, 0.080f, 0.20f));
    Primitives::drawSphere(shader, body, silverScale);

    // Darker river-blue dorsal spine ridge
    mat4 spine = root;
    spine = translate(spine, vec3(0.0f, 0.042f, -0.01f));
    spine = scale(spine, vec3(0.032f, 0.040f, 0.18f));
    Primitives::drawSphere(shader, spine, dorsalBlue);

    // 2. Head & Snout (tapering forward along +Z)
    mat4 snout = root;
    snout = translate(snout, vec3(0.0f, -0.005f, 0.16f));
    snout = rotate(snout, radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
    snout = scale(snout, vec3(0.038f, 0.075f, 0.055f));
    Primitives::drawCone(shader, snout, silverScale);

    // Mouth cleft
    mat4 mouth = root;
    mouth = translate(mouth, vec3(0.0f, -0.022f, 0.22f));
    mouth = scale(mouth, vec3(0.022f, 0.008f, 0.015f));
    Primitives::drawCube(shader, mouth, dorsalBlue);

    // Operculum (gill slits) & Eyes on both flanks
    for (int side = -1; side <= 1; side += 2) {
        float fside = (float)side;
        mat4 gill = root;
        gill = translate(gill, vec3(fside * 0.042f, 0.0f, 0.10f));
        gill = rotate(gill, radians(fside * -18.0f), vec3(0.0f, 1.0f, 0.0f));
        gill = scale(gill, vec3(0.005f, 0.055f, 0.020f));
        Primitives::drawCube(shader, gill, dorsalBlue);

        // Expressive golden eye with glossy pupil
        mat4 eyeIris = root;
        eyeIris = translate(eyeIris, vec3(fside * 0.036f, 0.018f, 0.14f));
        eyeIris = scale(eyeIris, vec3(0.010f, 0.010f, 0.010f));
        Primitives::drawSphere(shader, eyeIris, eyeRing);

        mat4 pupil = root;
        pupil = translate(pupil, vec3(fside * 0.042f, 0.018f, 0.14f));
        pupil = scale(pupil, vec3(0.006f, 0.006f, 0.006f));
        Primitives::drawSphere(shader, pupil, eyePupil);
    }

    // 3. Dorsal Fin (erect along top spine)
    mat4 dorsal = root;
    dorsal = translate(dorsal, vec3(0.0f, 0.075f, -0.03f));
    dorsal = rotate(dorsal, radians(-22.0f), vec3(1.0f, 0.0f, 0.0f));
    dorsal = scale(dorsal, vec3(0.006f, 0.075f, 0.095f));
    Primitives::drawTriangle(shader, dorsal, finCrimson);

    // 4. Pectoral & Pelvic Side Fins
    for (int side = -1; side <= 1; side += 2) {
        float fside = (float)side;
        // Pectoral fin
        mat4 pec = root;
        pec = translate(pec, vec3(fside * 0.045f, -0.025f, 0.06f));
        pec = rotate(pec, radians(fside * 42.0f), vec3(0.0f, 0.0f, 1.0f));
        pec = rotate(pec, radians(-25.0f), vec3(1.0f, 0.0f, 0.0f));
        pec = scale(pec, vec3(0.005f, 0.042f, 0.060f));
        Primitives::drawTriangle(shader, pec, finCrimson);

        // Pelvic fin
        mat4 pelv = root;
        pelv = translate(pelv, vec3(fside * 0.025f, -0.065f, -0.06f));
        pelv = rotate(pelv, radians(fside * 28.0f), vec3(0.0f, 0.0f, 1.0f));
        pelv = scale(pelv, vec3(0.004f, 0.035f, 0.045f));
        Primitives::drawTriangle(shader, pelv, finCrimson);
    }

    // 5. Articulated Tail Peduncle & Forked Caudal Fin
    mat4 tailJoint = root;
    tailJoint = translate(tailJoint, vec3(0.0f, 0.0f, -0.18f));
    tailJoint = rotate(tailJoint, wiggle, vec3(0.0f, 1.0f, 0.0f)); // dynamic flopping wiggle!

    // Tapered peduncle wrist
    mat4 peduncle = tailJoint;
    peduncle = translate(peduncle, vec3(0.0f, 0.0f, -0.05f));
    peduncle = scale(peduncle, vec3(0.020f, 0.042f, 0.090f));
    Primitives::drawSphere(shader, peduncle, silverScale);

    // Forked Caudal Tail Fin (Upper lobe and lower lobe)
    mat4 upperTail = tailJoint;
    upperTail = translate(upperTail, vec3(0.0f, 0.038f, -0.13f));
    upperTail = rotate(upperTail, radians(32.0f), vec3(1.0f, 0.0f, 0.0f));
    upperTail = scale(upperTail, vec3(0.006f, 0.075f, 0.090f));
    Primitives::drawTriangle(shader, upperTail, finCrimson);

    mat4 lowerTail = tailJoint;
    lowerTail = translate(lowerTail, vec3(0.0f, -0.038f, -0.13f));
    lowerTail = rotate(lowerTail, radians(-32.0f), vec3(1.0f, 0.0f, 0.0f));
    lowerTail = scale(lowerTail, vec3(0.006f, 0.075f, 0.090f));
    Primitives::drawTriangle(shader, lowerTail, finCrimson);
}

} // anonymous namespace

namespace Fisherman {

void drawFish(Shader& shader, const mat4& model, float wiggle, float scaleVal)
{
    renderFishModel(shader, model, wiggle, scaleVal);
}

// ── 1. The Fisherman Figure (মাছ শিকারী জেলে) ─────────────────────
// Muscular, sun-tanned village fisherman in tucked lungi (Malkocha),
// red gamcha headband with fluttering knot, expressive facial features,
// and athletic follow-through casting pose with both arms dynamically positioned.
void drawFishermanFigure(Shader& shader, const mat4& model, float animTime)
{
    shader.setInt("uUseTexture", 0);

    const vec3 skinTone   (0.54f, 0.36f, 0.22f); // Deep sun-tanned muscular skin
    const vec3 lungiBlue  (0.18f, 0.36f, 0.58f); // Traditional indigo/teal working lungi
    const vec3 gamchaRed  (0.85f, 0.20f, 0.14f); // Crimson Bengali gamcha headband
    const vec3 hairDark   (0.12f, 0.10f, 0.08f); // Dark hair
    const vec3 waistBand  (0.14f, 0.12f, 0.10f); // Tucked waist belt cord
    const vec3 eyeWhite   (0.92f, 0.94f, 0.95f); // Sclera

    // Subtle responsive breathing/swaying
    float sway = (animTime > 0.0f) ? (sinf(animTime * 2.0f) * radians(1.5f)) : 0.0f;

    // Master root positioned at boat deck: lean forward into the cast (+Z)
    mat4 fRoot = model;
    fRoot = rotate(fRoot, radians(12.0f) + sway, vec3(1.0f, 0.0f, 0.0f));

    // ── 1. Lower Body: Tucked Lungi (Malkocha / মালকোঁচা) ──────────
    mat4 pelvis = fRoot;
    pelvis = translate(pelvis, vec3(0.0f, 0.52f, 0.0f));
    pelvis = scale(pelvis, vec3(0.24f, 0.16f, 0.20f));
    Primitives::drawCube(shader, pelvis, lungiBlue);

    mat4 waist = fRoot;
    waist = translate(waist, vec3(0.0f, 0.59f, 0.0f));
    waist = scale(waist, vec3(0.248f, 0.035f, 0.205f));
    Primitives::drawCube(shader, waist, waistBand);

    mat4 malkochaKnot = fRoot;
    malkochaKnot = translate(malkochaKnot, vec3(0.0f, 0.44f, 0.0f));
    malkochaKnot = scale(malkochaKnot, vec3(0.12f, 0.18f, 0.14f));
    Primitives::drawCube(shader, malkochaKnot, lungiBlue);

    // Left leg: stepped slightly forward (+Z)
    mat4 legL_Thigh = fRoot;
    legL_Thigh = translate(legL_Thigh, vec3(-0.08f, 0.40f, 0.04f));
    legL_Thigh = rotate(legL_Thigh, radians(-15.0f), vec3(1.0f, 0.0f, 0.0f));
    mat4 legL_Twrap = scale(legL_Thigh, vec3(0.085f, 0.22f, 0.085f));
    Primitives::drawCylinder(shader, legL_Twrap, lungiBlue);

    mat4 legL_Shin = fRoot;
    legL_Shin = translate(legL_Shin, vec3(-0.08f, 0.20f, 0.08f));
    legL_Shin = scale(legL_Shin, vec3(0.065f, 0.26f, 0.065f));
    Primitives::drawCylinder(shader, legL_Shin, skinTone);

    mat4 footL = fRoot;
    footL = translate(footL, vec3(-0.08f, 0.04f, 0.13f));
    footL = scale(footL, vec3(0.065f, 0.045f, 0.13f));
    Primitives::drawCube(shader, footL, skinTone);

    // Right leg: braced slightly rearward for balance
    mat4 legR_Thigh = fRoot;
    legR_Thigh = translate(legR_Thigh, vec3(0.08f, 0.39f, -0.04f));
    legR_Thigh = rotate(legR_Thigh, radians(18.0f), vec3(1.0f, 0.0f, 0.0f));
    mat4 legR_Twrap = scale(legR_Thigh, vec3(0.085f, 0.22f, 0.085f));
    Primitives::drawCylinder(shader, legR_Twrap, lungiBlue);

    mat4 legR_Shin = fRoot;
    legR_Shin = translate(legR_Shin, vec3(0.08f, 0.19f, -0.07f));
    legR_Shin = rotate(legR_Shin, radians(10.0f), vec3(1.0f, 0.0f, 0.0f));
    legR_Shin = scale(legR_Shin, vec3(0.065f, 0.26f, 0.065f));
    Primitives::drawCylinder(shader, legR_Shin, skinTone);

    mat4 footR = fRoot;
    footR = translate(footR, vec3(0.08f, 0.04f, -0.05f));
    footR = scale(footR, vec3(0.065f, 0.045f, 0.13f));
    Primitives::drawCube(shader, footR, skinTone);

    // ── 2. Upper Body: Bare Muscular Sun-Tanned Torso ─────────────
    mat4 lowerTorso = fRoot;
    lowerTorso = translate(lowerTorso, vec3(0.0f, 0.69f, 0.0f));
    lowerTorso = scale(lowerTorso, vec3(0.20f, 0.20f, 0.14f));
    Primitives::drawCylinder(shader, lowerTorso, skinTone);

    mat4 upperTorso = fRoot;
    upperTorso = translate(upperTorso, vec3(0.0f, 0.82f, 0.02f));
    upperTorso = rotate(upperTorso, radians(8.0f), vec3(0.0f, 1.0f, 0.0f));
    upperTorso = scale(upperTorso, vec3(0.22f, 0.18f, 0.15f));
    Primitives::drawCylinder(shader, upperTorso, skinTone);

    mat4 neck = fRoot;
    neck = translate(neck, vec3(0.0f, 0.94f, 0.04f));
    neck = scale(neck, vec3(0.075f, 0.08f, 0.075f));
    Primitives::drawCylinder(shader, neck, skinTone);

    // ── 3. Head, Expressive Face & Tied Red Gamcha Headband ───────
    mat4 head = fRoot;
    head = translate(head, vec3(0.0f, 1.04f, 0.06f));
    head = rotate(head, radians(12.0f), vec3(1.0f, 0.0f, 0.0f));

    mat4 headSphere = scale(head, vec3(0.115f, 0.125f, 0.115f));
    Primitives::drawSphere(shader, headSphere, skinTone);

    mat4 hair = head;
    hair = translate(hair, vec3(0.0f, 0.05f, -0.02f));
    hair = scale(hair, vec3(0.120f, 0.085f, 0.120f));
    Primitives::drawSphere(shader, hair, hairDark);

    mat4 gamchaBand = head;
    gamchaBand = translate(gamchaBand, vec3(0.0f, 0.055f, -0.005f));
    gamchaBand = rotate(gamchaBand, radians(-10.0f), vec3(1.0f, 0.0f, 0.0f));
    gamchaBand = scale(gamchaBand, vec3(0.122f, 0.032f, 0.122f));
    Primitives::drawCylinder(shader, gamchaBand, gamchaRed);

    mat4 knot = head;
    knot = translate(knot, vec3(0.0f, 0.045f, -0.125f));
    knot = scale(knot, vec3(0.040f, 0.040f, 0.035f));
    Primitives::drawSphere(shader, knot, gamchaRed);

    float flutter = (animTime > 0.0f) ? (sinf(animTime * 4.5f) * radians(10.0f)) : 0.0f;
    for (int tail = -1; tail <= 1; tail += 2) {
        float ftail = (float)tail;
        mat4 gTail = head;
        gTail = translate(gTail, vec3(ftail * 0.032f, -0.04f, -0.14f));
        gTail = rotate(gTail, radians(ftail * 18.0f) + flutter, vec3(0.0f, 1.0f, 0.0f));
        gTail = scale(gTail, vec3(0.022f, 0.12f, 0.010f));
        Primitives::drawCube(shader, gTail, gamchaRed);
    }

    // Expressive Facial Features (Eyes, Nose, Bengali Moustache)
    for (int es = -1; es <= 1; es += 2) {
        float fes = (float)es;
        mat4 sclera = head;
        sclera = translate(sclera, vec3(fes * 0.038f, 0.012f, 0.110f));
        sclera = scale(sclera, vec3(0.012f, 0.008f, 0.006f));
        Primitives::drawCube(shader, sclera, eyeWhite);

        mat4 pupil = head;
        pupil = translate(pupil, vec3(fes * 0.038f, 0.012f, 0.114f));
        pupil = scale(pupil, vec3(0.007f, 0.007f, 0.004f));
        Primitives::drawSphere(shader, pupil, hairDark);
    }

    mat4 nose = head;
    nose = translate(nose, vec3(0.0f, -0.008f, 0.124f));
    nose = scale(nose, vec3(0.018f, 0.025f, 0.024f));
    Primitives::drawCube(shader, nose, skinTone);

    mat4 stache = head;
    stache = translate(stache, vec3(0.0f, -0.032f, 0.118f));
    stache = scale(stache, vec3(0.065f, 0.012f, 0.014f));
    Primitives::drawCube(shader, stache, hairDark);

    // ── 4. Dynamic Arm Action: The Circular Net Cast Release ──────
    // Left Arm: gripping the central haul rope
    const vec3 shL(-0.22f, 0.85f, 0.04f);
    const vec3 elbL(-0.20f, 0.68f, 0.28f);
    const vec3 hndL(-0.14f, 0.64f, 0.62f);

    mat4 shCapL = fRoot;
    shCapL = translate(shCapL, shL);
    shCapL = scale(shCapL, vec3(0.055f));
    Primitives::drawSphere(shader, shCapL, skinTone);

    drawLineCylinder(shader, fRoot, shL, elbL, 0.046f, skinTone);

    mat4 elbCapL = fRoot;
    elbCapL = translate(elbCapL, elbL);
    elbCapL = scale(elbCapL, vec3(0.044f));
    Primitives::drawSphere(shader, elbCapL, skinTone);

    drawLineCylinder(shader, fRoot, elbL, hndL, 0.040f, skinTone);

    mat4 hndCapL = fRoot;
    hndCapL = translate(hndCapL, hndL);
    hndCapL = scale(hndCapL, vec3(0.046f, 0.038f, 0.055f));
    Primitives::drawSphere(shader, hndCapL, skinTone);

    // Right Arm: thrown wide, high, and backward in full centrifugal release follow-through!
    const vec3 shR(0.22f, 0.86f, 0.02f);
    const vec3 elbR(0.44f, 0.96f, -0.06f);
    const vec3 hndR(0.60f, 1.08f, 0.08f);

    mat4 shCapR = fRoot;
    shCapR = translate(shCapR, shR);
    shCapR = scale(shCapR, vec3(0.055f));
    Primitives::drawSphere(shader, shCapR, skinTone);

    drawLineCylinder(shader, fRoot, shR, elbR, 0.048f, skinTone);

    mat4 elbCapR = fRoot;
    elbCapR = translate(elbCapR, elbR);
    elbCapR = scale(elbCapR, vec3(0.046f));
    Primitives::drawSphere(shader, elbCapR, skinTone);

    drawLineCylinder(shader, fRoot, elbR, hndR, 0.042f, skinTone);

    mat4 hndCapR = fRoot;
    hndCapR = translate(hndCapR, hndR);
    hndCapR = scale(hndCapR, vec3(0.048f, 0.040f, 0.060f));
    Primitives::drawSphere(shader, hndCapR, skinTone);

    // Open fingers radiating outward in dynamic release
    for (int f = -1; f <= 2; ++f) {
        float ff = (float)f;
        vec3 fTip = hndR + vec3(0.06f, ff * 0.022f, 0.04f);
        drawLineCylinder(shader, fRoot, hndR, fTip, 0.008f, skinTone);
    }
}

// ── 2. The Cast Net (ঝাঁকি জাল / খেপলা জাল / Khepla Jal) ─────────
// Expands into a graceful circular bell canopy hitting the river,
// complete with haul rope, radial surface rib cords, latitudinal mesh rings,
// studded lead sinkers, and water splash foam.
void drawCastNet(Shader& shader, const mat4& model, float animTime)
{
    shader.setInt("uUseTexture", 0);

    const vec3 haulCordCol (0.35f, 0.38f, 0.32f); // Braided nylon haul rope
    const vec3 netMeshCol  (0.76f, 0.88f, 0.93f); // Fine aquatic semi-translucent net mesh
    const vec3 netRibCol   (0.38f, 0.52f, 0.60f); // Radial net cord ribbing & mesh rings
    const vec3 leadSinker  (0.22f, 0.24f, 0.28f); // Dark lead hem weights (Kathir Sisa)
    const vec3 splashFoam  (0.92f, 0.96f, 1.00f); // White frothing water splash

    // Haul cord connecting from the fisherman's left hand to net apex
    // Fisherman hand in boatRoot frame: (-0.14, 0.62, 1.25)
    // Net apex in boatRoot frame: (0.0, 0.65, 2.35)
    drawLineCylinder(shader, model, vec3(-0.14f, 0.62f, 1.25f), vec3(0.0f, 0.65f, 2.35f), 0.012f, haulCordCol);

    // Center apex ring / collar of the net
    mat4 apex = model;
    apex = translate(apex, vec3(0.0f, 0.65f, 2.35f));
    mat4 apexS = scale(apex, vec3(0.065f, 0.040f, 0.065f));
    Primitives::drawCylinder(shader, apexS, haulCordCol);

    // ── The Expanding Bell Mesh Cone ─────────────────────────────
    // Spreads from apex (0, 0.65, 2.35) down to river water (Y = 0.02, Z = 2.40)
    // with a wide 1.85-meter circular diameter!
    mat4 netBell = model;
    netBell = translate(netBell, vec3(0.0f, 0.02f, 2.40f));

    // Outer cone mesh shell (Unit Cone: base at Y=0, apex at Y=1)
    mat4 netCone = netBell;
    netCone = scale(netCone, vec3(0.92f, 0.63f, 0.92f));
    Primitives::drawCone(shader, netCone, netMeshCol);

    // ── 12 Radial Structural Ribs Flush on Cone Surface ───────────
    const int ribCount = 12;
    const vec3 netApexLocal(0.0f, 0.63f, -0.05f);
    for (int r = 0; r < ribCount; ++r) {
        float angle = (float)r * (2.0f * PI / (float)ribCount);
        vec3 rimPt(cosf(angle) * 0.92f, 0.015f, sinf(angle) * 0.92f);
        drawLineCylinder(shader, netBell, netApexLocal, rimPt, 0.005f, netRibCol);
    }

    // ── Concentric Latitudinal Mesh Hoops (Concentric Rings) ───────
    for (float h : { 0.16f, 0.32f, 0.48f }) {
        float r_at_h = 0.92f * (1.0f - h / 0.63f);
        mat4 hoop = netBell;
        hoop = translate(hoop, vec3(0.0f, h, 0.0f));
        hoop = scale(hoop, vec3(r_at_h * 1.005f, 0.008f, r_at_h * 1.005f));
        Primitives::drawCylinder(shader, hoop, netRibCol);
    }

    // ── Weighted Lead Sinker Perimeter Hem (সীসার কাঠি / Sinkers) ──
    mat4 hemRing = netBell;
    hemRing = translate(hemRing, vec3(0.0f, 0.015f, 0.0f));
    hemRing = scale(hemRing, vec3(0.93f, 0.022f, 0.93f));
    Primitives::drawCylinder(shader, hemRing, leadSinker);

    // Studded lead sinker beads around the perimeter
    const int sinkerCount = 16;
    for (int s = 0; s < sinkerCount; ++s) {
        float sAngle = (float)s * (2.0f * PI / (float)sinkerCount);
        mat4 sinkerM = netBell;
        sinkerM = translate(sinkerM, vec3(cosf(sAngle) * 0.93f, 0.015f, sinf(sAngle) * 0.93f));
        sinkerM = scale(sinkerM, vec3(0.025f, 0.035f, 0.025f));
        Primitives::drawSphere(shader, sinkerM, leadSinker);
    }

    // ── Water Splash Foam & Ripples Where Net Hits Water ──────────
    mat4 splashRing = netBell;
    splashRing = translate(splashRing, vec3(0.0f, 0.005f, 0.0f));
    splashRing = scale(splashRing, vec3(1.08f, 0.006f, 1.08f));
    Primitives::drawCylinder(shader, splashRing, splashFoam);

    // Dynamic water droplets spraying into the air
    float sprayPulse = (animTime > 0.0f) ? sinf(animTime * 3.5f) : 0.0f;
    for (int d = 0; d < 8; ++d) {
        float dAngle = (float)d * (2.0f * PI / 8.0f) + 0.3f;
        float dDist = 0.78f + (float)(d % 3) * 0.10f;
        float dH = 0.06f + fabsf(sinf((float)d * 1.8f + animTime * 3.0f)) * 0.08f;

        mat4 drop = netBell;
        drop = translate(drop, vec3(cosf(dAngle) * dDist, dH, sinf(dAngle) * dDist));
        drop = scale(drop, vec3(0.016f, 0.022f, 0.016f));
        Primitives::drawSphere(shader, drop, splashFoam);
    }
}

// ── 3. Traditional Fishing Gear: Bamboo Khalui, Polo & Teta ──────
// The authentic accessories of rural Bengali river fishing.
void drawGear(Shader& shader, const mat4& model)
{
    shader.setInt("uUseTexture", 0);

    const vec3 bambooWeave (0.72f, 0.55f, 0.28f); // Golden woven split bamboo
    const vec3 caneDark    (0.35f, 0.24f, 0.12f); // Dark cane bindings
    const vec3 leafGreen   (0.20f, 0.56f, 0.18f); // Fresh banana leaf liner
    const vec3 tetaShaft   (0.58f, 0.44f, 0.22f); // Seasoned bamboo spear shaft
    const vec3 ironProng   (0.22f, 0.24f, 0.26f); // Forged dark iron barbed tines
    const vec3 lanternBrass(0.82f, 0.68f, 0.25f); // Hariken lantern brass/tin
    const vec3 lanternGlow (1.00f, 0.88f, 0.45f); // Warm flame globe glow

    // ═════════════════════════════════════════════════════════════
    // A. WOVEN BAMBOO FISH BASKET (Khalui / খালুই)
    // ═════════════════════════════════════════════════════════════
    // Traditional narrow-necked pot basket resting on stern thwart (Z = -0.45m)
    mat4 khalui = model;
    khalui = translate(khalui, vec3(-0.16f, 0.12f, -0.45f));

    // Bulbous lower belly
    mat4 k_belly = khalui;
    k_belly = translate(k_belly, vec3(0.0f, 0.08f, 0.0f));
    k_belly = scale(k_belly, vec3(0.13f, 0.11f, 0.13f));
    Primitives::drawSphere(shader, k_belly, bambooWeave);

    // Narrow constricted neck (prevents live fish from jumping out!)
    mat4 k_neck = khalui;
    k_neck = translate(k_neck, vec3(0.0f, 0.17f, 0.0f));
    k_neck = scale(k_neck, vec3(0.075f, 0.06f, 0.075f));
    Primitives::drawCylinder(shader, k_neck, bambooWeave);

    // Flared funnel mouth
    mat4 k_rim = khalui;
    k_rim = translate(k_rim, vec3(0.0f, 0.21f, 0.0f));
    k_rim = scale(k_rim, vec3(0.10f, 0.025f, 0.10f));
    Primitives::drawCylinder(shader, k_rim, caneDark);

    // Fresh green banana leaf lining mouth
    mat4 k_leaf = khalui;
    k_leaf = translate(k_leaf, vec3(0.0f, 0.20f, 0.0f));
    k_leaf = scale(k_leaf, vec3(0.08f, 0.015f, 0.08f));
    Primitives::drawSphere(shader, k_leaf, leafGreen);

    // A fresh silver river fish resting inside the Khalui!
    mat4 caughtFish = khalui;
    caughtFish = translate(caughtFish, vec3(0.02f, 0.21f, 0.0f));
    caughtFish = rotate(caughtFish, radians(42.0f), vec3(0.0f, 0.0f, 1.0f));
    caughtFish = rotate(caughtFish, radians(25.0f), vec3(1.0f, 0.0f, 0.0f));
    renderFishModel(shader, caughtFish, 0.0f, 0.50f);

    // ═════════════════════════════════════════════════════════════
    // B. TRADITIONAL BAMBOO PLUNGE FISH TRAP (Polo / পলো)
    // ═════════════════════════════════════════════════════════════
    // Conical bell-shaped basket resting beside the thwart at Z = -0.20m
    mat4 polo = model;
    polo = translate(polo, vec3(0.14f, 0.12f, -0.20f));

    // Conical bamboo body
    mat4 p_cone = polo;
    p_cone = scale(p_cone, vec3(0.18f, 0.28f, 0.18f));
    Primitives::drawCone(shader, p_cone, bambooWeave);

    // Top open hand collar
    mat4 p_top = polo;
    p_top = translate(p_top, vec3(0.0f, 0.27f, 0.0f));
    p_top = scale(p_top, vec3(0.055f, 0.025f, 0.055f));
    Primitives::drawCylinder(shader, p_top, caneDark);

    // Reinforcing cane hoop rings around polo cone
    for (float hr : { 0.08f, 0.16f, 0.22f }) {
        mat4 p_ring = polo;
        p_ring = translate(p_ring, vec3(0.0f, hr, 0.0f));
        p_ring = scale(p_ring, vec3(0.18f * (1.0f - hr * 0.65f), 0.012f, 0.18f * (1.0f - hr * 0.65f)));
        Primitives::drawCylinder(shader, p_ring, caneDark);
    }

    // ═════════════════════════════════════════════════════════════
    // C. MULTI-PRONGED FISH SPEAR (Teta / টেঁটা / কোঁচ)
    // ═════════════════════════════════════════════════════════════
    // Long bamboo spear resting along port gunwale rail
    // Shaft runs from Z = -0.55m to Z = +0.95m
    drawLineCylinder(shader, model, vec3(-0.31f, 0.29f, -0.55f), vec3(-0.25f, 0.33f, 0.95f), 0.024f, tetaShaft);

    // 5 barbed iron prongs / tines at spearhead (+Z = 0.95m)
    for (int p = -2; p <= 2; ++p) {
        float fp = (float)p;
        mat4 tine = model;
        tine = translate(tine, vec3(-0.25f + fp * 0.012f, 0.33f, 1.02f));
        tine = rotate(tine, radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
        tine = scale(tine, vec3(0.005f, 0.12f, 0.005f));
        Primitives::drawCone(shader, tine, ironProng);
    }

    // Iron collar binding ring
    mat4 t_collar = model;
    t_collar = translate(t_collar, vec3(-0.25f, 0.33f, 0.95f));
    t_collar = scale(t_collar, vec3(0.045f, 0.025f, 0.030f));
    Primitives::drawCube(shader, t_collar, ironProng);

    // ═════════════════════════════════════════════════════════════
    // D. KEROSENE HARIKEN LANTERN FOR NIGHT FISHING
    // ═════════════════════════════════════════════════════════════
    // Mounted on starboard bow rail bracket at Z = 1.15m
    mat4 lantern = model;
    lantern = translate(lantern, vec3(0.205f, 0.35f, 1.15f));

    // Fuel fount base
    mat4 l_base = lantern;
    l_base = scale(l_base, vec3(0.075f, 0.038f, 0.075f));
    Primitives::drawCylinder(shader, l_base, lanternBrass);

    // Glass globe with glowing amber light
    mat4 l_globe = lantern;
    l_globe = translate(l_globe, vec3(0.0f, 0.06f, 0.0f));
    l_globe = scale(l_globe, vec3(0.040f, 0.048f, 0.040f));
    Primitives::drawSphere(shader, l_globe, lanternGlow);

    // Top chimney & cowl
    mat4 l_top = lantern;
    l_top = translate(l_top, vec3(0.0f, 0.11f, 0.0f));
    l_top = scale(l_top, vec3(0.055f, 0.035f, 0.055f));
    Primitives::drawCylinder(shader, l_top, lanternBrass);
}

// ── 4. The Complete Fishing Scene (নদীতে মাছ শিকার) ──────────────
// Assembles:
// - Sleek wooden fishing dingi boat
// - Standing athletic fisherman casting net
// - Wide expanding cast net with lead sinkers and water splash
// - Magnificent silver river fish leaping in mid-air
// - River swimming fish
// - Bamboo Khalui & Polo traps, Teta spear, and Hariken lantern
void draw(Shader& shader, const mat4& model, float animTime)
{
    shader.setInt("uUseTexture", 0);

    const vec3 timberHull  (0.28f, 0.15f, 0.07f); // Seasoned dark timber
    const vec3 redStripe   (0.68f, 0.20f, 0.12f); // Signature terracotta folk stripe
    const vec3 gunwaleTrim (0.16f, 0.08f, 0.04f); // Dark timber rub-rails
    const vec3 deckFloor   (0.36f, 0.20f, 0.10f); // Inner timber floorboards
    const vec3 rippleCol   (0.85f, 0.94f, 0.98f); // Water foam ripples

    // Gentle boat rocking on river current
    float boatBob  = (animTime > 0.0f) ? (sinf(animTime * 1.9f) * 0.015f) : 0.0f;
    float boatRoll = (animTime > 0.0f) ? (cosf(animTime * 1.5f) * radians(2.0f)) : 0.0f;

    mat4 boatRoot = model;
    boatRoot = translate(boatRoot, vec3(0.0f, boatBob, 0.0f));
    boatRoot = rotate(boatRoot, boatRoll, vec3(0.0f, 0.0f, 1.0f));

    // ═════════════════════════════════════════════════════════════
    // 1. SMOOTH LOFTED WATERTIGHT FISHING DINGI HULL (জেলে ডিঙি)
    // ═════════════════════════════════════════════════════════════
    // Mathematically continuous lofted fishing dingi hull:
    // Generates 12 smoothly connected cross-sections along the boat length (Z in [-1.70m, +1.70m]).
    // Ensures ZERO steps, ZERO disjoint boards, ZERO gaps, and 100% watertight inner deck!

    struct HullStation {
        float z;
        vec3 ptKeelL, ptKeelR;
        vec3 ptStripe0L, ptStripe0R;
        vec3 ptStripe1L, ptStripe1R;
        vec3 ptGunwaleL, ptGunwaleR;
        vec3 ptFloorL, ptFloorR;
    };

    const int NUM_SEGS = 12;
    HullStation st[NUM_SEGS + 1];

    for (int i = 0; i <= NUM_SEGS; ++i) {
        float t = -1.0f + 2.0f * ((float)i / (float)NUM_SEGS); // [-1.0, +1.0]
        float z = t * 1.70f;
        float u = fabsf(t);

        // Continuous quadratic rocker and flare profiles
        float yKeel = 0.035f + 0.38f * (u * u);
        float xKeel = 0.24f * (1.0f - u * u) + 0.012f;

        float yGunwale = 0.28f + 0.16f * (u * u);
        float xGunwale = 0.34f * (1.0f - u * u) + 0.020f;

        float yFloor = (yKeel + 0.032f > 0.075f) ? (yKeel + 0.032f) : 0.075f;
        float xFloor = (xKeel - 0.015f > 0.010f) ? (xKeel - 0.015f) : 0.010f;

        float s0 = 0.38f;
        float s1 = 0.58f;
        float yS0 = yKeel + s0 * (yGunwale - yKeel);
        float xS0 = xKeel + s0 * (xGunwale - xKeel);
        float yS1 = yKeel + s1 * (yGunwale - yKeel);
        float xS1 = xKeel + s1 * (xGunwale - xKeel);

        st[i].z = z;
        st[i].ptKeelL    = vec3(-xKeel, yKeel, z);
        st[i].ptKeelR    = vec3( xKeel, yKeel, z);
        st[i].ptStripe0L = vec3(-xS0,   yS0,   z);
        st[i].ptStripe0R = vec3( xS0,   yS0,   z);
        st[i].ptStripe1L = vec3(-xS1,   yS1,   z);
        st[i].ptStripe1R = vec3( xS1,   yS1,   z);
        st[i].ptGunwaleL = vec3(-xGunwale, yGunwale, z);
        st[i].ptGunwaleR = vec3( xGunwale, yGunwale, z);
        st[i].ptFloorL   = vec3(-xFloor, yFloor, z);
        st[i].ptFloorR   = vec3( xFloor, yFloor, z);
    }

    // Lofted Quad Strake Panels between adjacent stations
    for (int i = 0; i < NUM_SEGS; ++i) {
        const HullStation& s0 = st[i];
        const HullStation& s1 = st[i + 1];

        // 1. Keel Bottom Plank (outside underbody)
        draw3DQuad(shader, boatRoot, s0.ptKeelL, s0.ptKeelR, s1.ptKeelR, s1.ptKeelL, timberHull);

        // 2. Starboard Lower Flank (+X)
        draw3DQuad(shader, boatRoot, s0.ptKeelR, s0.ptStripe0R, s1.ptStripe0R, s1.ptKeelR, timberHull);

        // 3. Port Lower Flank (-X)
        draw3DQuad(shader, boatRoot, s0.ptKeelL, s1.ptKeelL, s1.ptStripe0L, s0.ptStripe0L, timberHull);

        // 4. Starboard Crimson Folk Accent Stripe (+X)
        draw3DQuad(shader, boatRoot, s0.ptStripe0R, s0.ptStripe1R, s1.ptStripe1R, s1.ptStripe0R, redStripe);

        // 5. Port Crimson Folk Accent Stripe (-X)
        draw3DQuad(shader, boatRoot, s0.ptStripe0L, s1.ptStripe0L, s1.ptStripe1L, s0.ptStripe1L, redStripe);

        // 6. Starboard Upper Flank (+X)
        draw3DQuad(shader, boatRoot, s0.ptStripe1R, s0.ptGunwaleR, s1.ptGunwaleR, s1.ptStripe1R, timberHull);

        // 7. Port Upper Flank (-X)
        draw3DQuad(shader, boatRoot, s0.ptStripe1L, s1.ptStripe1L, s1.ptGunwaleL, s0.ptGunwaleL, timberHull);

        // 8. Watertight Inner Timber Floor (seen from above)
        draw3DQuad(shader, boatRoot, s0.ptFloorL, s1.ptFloorL, s1.ptFloorR, s0.ptFloorR, deckFloor);

        // 9. Inner Side Strakes (connecting inner floor to gunwales)
        draw3DQuad(shader, boatRoot, s0.ptFloorR, s1.ptFloorR, s1.ptGunwaleR, s0.ptGunwaleR, deckFloor * 0.88f);
        draw3DQuad(shader, boatRoot, s0.ptFloorL, s0.ptGunwaleL, s1.ptGunwaleL, s1.ptFloorL, deckFloor * 0.88f);

        // 10. Gunwale Rub-Rail Capping
        drawLineCylinder(shader, boatRoot, s0.ptGunwaleR, s1.ptGunwaleR, 0.040f, gunwaleTrim);
        drawLineCylinder(shader, boatRoot, s0.ptGunwaleL, s1.ptGunwaleL, 0.040f, gunwaleTrim);
    }

    // Stem Caps at Bow and Stern Horn Apexes (গুলুই / Gului)
    mat4 bowStem = boatRoot;
    bowStem = translate(bowStem, vec3(0.0f, 0.44f, 1.70f));
    bowStem = rotate(bowStem, radians(24.0f), vec3(1.0f, 0.0f, 0.0f));
    bowStem = scale(bowStem, vec3(0.040f, 0.10f, 0.05f));
    Primitives::drawCube(shader, bowStem, gunwaleTrim);

    mat4 sternStem = boatRoot;
    sternStem = translate(sternStem, vec3(0.0f, 0.44f, -1.70f));
    sternStem = rotate(sternStem, radians(-24.0f), vec3(1.0f, 0.0f, 0.0f));
    sternStem = scale(sternStem, vec3(0.040f, 0.10f, 0.05f));
    Primitives::drawCube(shader, sternStem, gunwaleTrim);

    // E. Wooden Cross Thwarts (কাঠের গুলুই ও বসার পাটাতন)
    for (float tz : { -0.80f, -0.35f, 0.10f, 0.95f }) {
        float u = fabsf(tz) / 1.70f;
        float gw = 2.0f * (0.34f * (1.0f - u * u) + 0.020f) - 0.03f;
        float gy = 0.28f + 0.16f * (u * u) - 0.02f;
        mat4 thwart = boatRoot;
        thwart = translate(thwart, vec3(0.0f, gy, tz));
        thwart = scale(thwart, vec3(gw, 0.035f, 0.07f));
        Primitives::drawCube(shader, thwart, deckFloor);
    }

    // ═════════════════════════════════════════════════════════════
    // 2. THE STANDING FISHERMAN CASTING NET (জেলে)
    // ═════════════════════════════════════════════════════════════
    mat4 fisherM = boatRoot;
    fisherM = translate(fisherM, vec3(0.0f, 0.08f, 0.55f));
    drawFishermanFigure(shader, fisherM, animTime);

    // ═════════════════════════════════════════════════════════════
    // 3. THE EXPANDING CAST NET WITH SINKERS & SPLASH
    // ═════════════════════════════════════════════════════════════
    mat4 netM = boatRoot;
    drawCastNet(shader, netM, animTime);

    // ═════════════════════════════════════════════════════════════
    // 4. THE LEAPING FRESHWATER RIVER FISH (লাফানো রূপালী মাছ)
    // ═════════════════════════════════════════════════════════════
    // Spectacular hero fish leaping high out of the river spray right beside the boat and net!
    float fishPhase  = (animTime > 0.0f) ? (animTime * 3.4f) : 0.8f;
    float leapCycle  = fabsf(sinf(fishPhase));
    float fishY      = 0.32f + leapCycle * 0.42f; // Leaps high and beautifully into mid-air!
    float fishZ      = 1.95f + sinf(fishPhase * 0.5f) * 0.12f;
    float tailWiggle = (animTime > 0.0f) ? (sinf(animTime * 18.0f) * radians(30.0f)) : radians(22.0f);

    mat4 heroFish = boatRoot;
    heroFish = translate(heroFish, vec3(0.75f, fishY, fishZ));
    heroFish = rotate(heroFish, radians(38.0f), vec3(1.0f, 0.0f, 0.0f)); // arched head-up breach!
    heroFish = rotate(heroFish, radians(-22.0f), vec3(0.0f, 1.0f, 0.0f));
    heroFish = rotate(heroFish, radians(15.0f), vec3(0.0f, 0.0f, 1.0f));
    renderFishModel(shader, heroFish, tailWiggle, 1.22f);

    // Splash ripple rings on water under the leaping fish
    mat4 fishSplash = boatRoot;
    fishSplash = translate(fishSplash, vec3(0.75f, 0.015f, fishZ));
    fishSplash = scale(fishSplash, vec3(0.38f + leapCycle * 0.16f, 0.006f, 0.38f + leapCycle * 0.16f));
    Primitives::drawCylinder(shader, fishSplash, rippleCol);

    mat4 fishSplashOuter = boatRoot;
    fishSplashOuter = translate(fishSplashOuter, vec3(0.75f, 0.012f, fishZ));
    fishSplashOuter = scale(fishSplashOuter, vec3(0.52f + leapCycle * 0.20f, 0.004f, 0.52f + leapCycle * 0.20f));
    Primitives::drawCylinder(shader, fishSplashOuter, rippleCol * 0.92f);

    // Flying water droplet beads around leaping fish
    for (int d = 0; d < 6; ++d) {
        float fangle = (float)d * (2.0f * PI / 6.0f);
        mat4 fdrop = boatRoot;
        fdrop = translate(fdrop, vec3(0.75f + cosf(fangle) * 0.24f, 0.12f + (float)(d % 3) * 0.09f, fishZ + sinf(fangle) * 0.24f));
        fdrop = scale(fdrop, vec3(0.015f, 0.022f, 0.015f));
        Primitives::drawSphere(shader, fdrop, rippleCol);
    }

    // Second river fish darting on port side away from the falling net
    float fish2Phase  = (animTime > 0.0f) ? (animTime * 2.8f + 1.5f) : 1.2f;
    float leapCycle2  = fabsf(sinf(fish2Phase));
    float tailWiggle2 = (animTime > 0.0f) ? (sinf(animTime * 15.0f + 1.0f) * radians(25.0f)) : radians(15.0f);
    mat4 swimFish = boatRoot;
    swimFish = translate(swimFish, vec3(-0.75f, 0.22f + leapCycle2 * 0.26f, 2.30f + sinf(fish2Phase * 0.4f) * 0.15f));
    swimFish = rotate(swimFish, radians(32.0f), vec3(1.0f, 0.0f, 0.0f));
    swimFish = rotate(swimFish, radians(42.0f), vec3(0.0f, 1.0f, 0.0f));
    renderFishModel(shader, swimFish, tailWiggle2, 0.85f);

    // Splash ripple for second fish
    mat4 fishSplash2 = boatRoot;
    fishSplash2 = translate(fishSplash2, vec3(-0.75f, 0.015f, 2.30f));
    fishSplash2 = scale(fishSplash2, vec3(0.30f + leapCycle2 * 0.12f, 0.005f, 0.30f + leapCycle2 * 0.12f));
    Primitives::drawCylinder(shader, fishSplash2, rippleCol);

    // ═════════════════════════════════════════════════════════════
    // 5. TRADITIONAL FISHING GEAR (Khalui, Polo, Teta & Hariken)
    // ═════════════════════════════════════════════════════════════
    drawGear(shader, boatRoot);
}

} // namespace Fisherman
