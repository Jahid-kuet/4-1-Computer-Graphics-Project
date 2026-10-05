// Boat.cpp — Traditional Bangladeshi wooden boat (Dingi Nouka / Pal Tola Nouka)
// Faithfully modeled directly after the user's reference folk artwork images:
// 1. BOAT_STYLE_RED_SAIL: Triangular white sail with crimson folk patch & standing Majhi
// 2. BOAT_STYLE_ROUND_CHHOI: Center canopy box with golden-caramel dome & standing Majhi with diagonal Boitha oar
// 3. BOAT_STYLE_WHITE_SAIL: Right-angled pure white triangular sail on tall vertical mast
// COMMON HULL: Symmetrical inverted trapezoid with rich chocolate brown timber planks,
//               bold RED horizontal accent stripe running along both sides,
//               and sharply upturned pointed horn prows (\____/) at both bow and stern!
// STRICT CONSTRAINT: 100% procedural OpenGL 3.3 Core using unit primitives.

#include "objects/Boat.h"
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

inline void drawLineCylinder(Shader& shader, const mat4& base, const vec3& p1, const vec3& p2, float radius, const vec3& color)
{
    vec3 diff = p2 - p1;
    float len = diff.length();
    if (len < 1e-4f) return;

    vec3 dir = diff / len;
    vec3 mid = (p1 + p2) * 0.5f;

    mat4 m = base;
    m = translate(m, mid);

    vec3 up(0.0f, 1.0f, 0.0f);
    float dotP = dot(up, dir);
    if (dotP > 0.9999f) {
        // Aligned with +Y
    } else if (dotP < -0.9999f) {
        m = rotate(m, radians(180.0f), vec3(1.0f, 0.0f, 0.0f));
    } else {
        vec3 axis = cross(up, dir);
        float angle = acosf(dotP);
        m = rotate(m, angle, axis);
    }
    m = scale(m, vec3(radius * 2.0f, len, radius * 2.0f));
    Primitives::drawCylinder(shader, m, color);
}

// Helper: draws a right-angled triangle in the (Z, Y) vertical plane:
// - Vertical edge along the mast at Z = mastZ (from posY to posY + height)
// - Horizontal bottom edge at Y = posY extending forward along +Z to mastZ + baseWidth
// - Hypotenuse sloping from (mastZ, posY + height) down to (mastZ + baseWidth, posY)
// Derived from canonical Unit Triangle (base [-0.5, 0.5] along X at Y=0, apex (0, 1, 0)):
// Shear matrix X' = X - 0.5 * Y + 0.5 maps (-0.5, 0) -> (0, 0), (0.5, 0) -> (1, 0), (0, 1) -> (0, 1).
// Then rotation by 90 deg around Y maps X -> +Z, giving a perfect right triangle along the boat length!
void drawRightTriangleSail(Shader& shader, const mat4& model,
                          float mastZ, float posY,
                          float baseWidth, float height,
                          const vec3& color, float xOffset = 0.0f)
{
    mat4 m = model;
    m = translate(m, vec3(xOffset, posY, mastZ));
    m = rotate(m, radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
    m = scale(m, vec3(baseWidth, height, 1.0f));

    mat4 shear = mat4::identity();
    shear(0, 3) = 0.5f;   // translate X by +0.5
    shear(0, 1) = -0.5f;  // shear X -= 0.5 * Y

    mat4 finalM = m * shear;
    Primitives::drawTriangle(shader, finalM, color);
}

// Helper: draws the traditional boatman (Majhi) matching the artwork illustration:
// Conical golden straw sunhat (Mathal), dark hair band, tan face, and clean tapering white kurta robe.
// For Boat 2, also holds the long diagonal Boitha oar dipping into the river.
void drawMajhi(Shader& shader, const mat4& model, bool hasOar, float oarAnim)
{
    shader.setInt("uUseTexture", 0);

    vec3 strawYellow(0.92f, 0.78f, 0.42f); // Golden palm leaf straw (Mathal)
    vec3 hairBand   (0.12f, 0.10f, 0.08f); // Dark hair / headband
    vec3 skinTone   (0.68f, 0.46f, 0.30f); // Warm sun-tanned Bengali skin tone
    vec3 kurtaWhite (0.98f, 0.98f, 0.98f); // Pristine minimalist white kurta robe
    vec3 oarWood    (0.48f, 0.30f, 0.14f); // Seasoned bamboo / timber oar

    // Standing on the stern deck (left side in reference view)
    mat4 m = model;
    m = translate(m, vec3(0.0f, 0.08f, -1.15f));

    // 1. Clean, elegant tapering white kurta robe (standing bell silhouette)
    // Lower body / skirt
    mat4 skirtM = m;
    skirtM = translate(skirtM, vec3(0.0f, 0.30f, 0.0f));
    skirtM = scale(skirtM, vec3(0.24f, 0.58f, 0.22f));
    Primitives::drawCylinder(shader, skirtM, kurtaWhite);

    // Flared base fold at deck
    mat4 baseFold = m;
    baseFold = translate(baseFold, vec3(0.0f, 0.05f, 0.0f));
    baseFold = scale(baseFold, vec3(0.28f, 0.10f, 0.25f));
    Primitives::drawCylinder(shader, baseFold, kurtaWhite);

    // Torso & shoulders
    mat4 torsoM = m;
    torsoM = translate(torsoM, vec3(0.0f, 0.58f, 0.0f));
    torsoM = scale(torsoM, vec3(0.20f, 0.24f, 0.18f));
    Primitives::drawCylinder(shader, torsoM, kurtaWhite);

    // 2. Head and Neck
    mat4 neckM = m;
    neckM = translate(neckM, vec3(0.0f, 0.72f, 0.0f));
    neckM = scale(neckM, vec3(0.08f, 0.08f, 0.08f));
    Primitives::drawCylinder(shader, neckM, skinTone);

    mat4 headM = m;
    headM = translate(headM, vec3(0.0f, 0.81f, 0.0f));
    headM = scale(headM, vec3(0.11f, 0.11f, 0.11f));
    Primitives::drawSphere(shader, headM, skinTone);

    // Dark hair / brim underband
    mat4 hairM = m;
    hairM = translate(hairM, vec3(0.0f, 0.84f, 0.0f));
    hairM = scale(hairM, vec3(0.125f, 0.04f, 0.125f));
    Primitives::drawCylinder(shader, hairM, hairBand);

    // 3. Conical Bamboo Sunhat (Mathal / মাথাল)
    mat4 hatM = m;
    hatM = translate(hatM, vec3(0.0f, 0.88f, 0.0f));
    hatM = scale(hatM, vec3(0.32f, 0.10f, 0.32f));
    Primitives::drawCone(shader, hatM, strawYellow);

    mat4 rimM = m;
    rimM = translate(rimM, vec3(0.0f, 0.88f, 0.0f));
    rimM = scale(rimM, vec3(0.33f, 0.015f, 0.33f));
    Primitives::drawCylinder(shader, rimM, hairBand);

    // 4. Long Diagonal Boitha Oar (for Boat 2)
    if (hasOar) {
        mat4 oarM = m;
        // Diagonal angle reaching from hands downwards into the river to the left (rearward)
        oarM = translate(oarM, vec3(-0.18f, 0.42f, 0.05f));
        oarM = rotate(oarM, radians(-38.0f) + oarAnim * 0.7f, vec3(1.0f, 0.0f, 0.0f));
        oarM = rotate(oarM, radians(-25.0f), vec3(0.0f, 0.0f, 1.0f));

        // Long straight wooden shaft
        mat4 shaft = oarM;
        shaft = translate(shaft, vec3(0.0f, -0.68f, 0.0f));
        shaft = scale(shaft, vec3(0.032f, 1.65f, 0.032f));
        Primitives::drawCylinder(shader, shaft, oarWood);

        // Paddle blade at submerged water end
        mat4 blade = oarM;
        blade = translate(blade, vec3(0.0f, -1.52f, 0.0f));
        blade = scale(blade, vec3(0.16f, 0.44f, 0.022f));
        Primitives::drawCube(shader, blade, oarWood * 0.90f);
    }
}

} // anonymous namespace

namespace Boat {

void cleanup()
{
}

void draw(Shader& shader, const mat4& model, bool hasPal, float animTime)
{
    draw(shader, model, hasPal ? BOAT_STYLE_RED_SAIL : BOAT_STYLE_ROUND_CHHOI, animTime, 0.0f);
}

// ── 2 Authentic Bengali River Passengers (গ্রামের দুইজন যাত্রী) ───────────
// Passenger 1: Seated forward starboard in vibrant teal kurta tunic, maroon lungi, gamcha & Mathal
// Passenger 2: Seated forward port in golden/saffron kurta tunic, emerald lungi, gamcha & white cap
// Between them: Handcrafted split-bamboo market basket (Jhaka) with banana leaf liner & clay pot (Matir Handi)
void drawPassengers(Shader& shader, const mat4& model, float animTime)
{
    shader.setInt("uUseTexture", 0);

    // Warm, culturally rich village color palette
    const vec3 skinTone1   (0.66f, 0.45f, 0.30f); // Warm sun-tanned Bengali skin tone
    const vec3 skinTone2   (0.64f, 0.42f, 0.28f); // Warm Bengali skin tone
    const vec3 kurtaTeal   (0.18f, 0.50f, 0.65f); // Vibrant rural indigo/teal cotton kurta
    const vec3 lungiMaroon (0.58f, 0.16f, 0.12f); // Traditional crimson/maroon checked lungi
    const vec3 gamchaRed   (0.85f, 0.22f, 0.14f); // Crimson Bengali gamcha scarf
    const vec3 kurtaOchre  (0.85f, 0.60f, 0.24f); // Sun-baked golden/saffron kurta tunic
    const vec3 lungiGreen  (0.16f, 0.46f, 0.26f); // Deep forest emerald checked lungi
    const vec3 gamchaWhite (0.94f, 0.92f, 0.88f); // White/cream cotton scarf
    const vec3 strawYellow (0.92f, 0.78f, 0.42f); // Golden palm leaf straw (Mathal)
    const vec3 hairDark    (0.12f, 0.10f, 0.08f); // Dark hair / headband
    const vec3 basketWood  (0.72f, 0.54f, 0.28f); // Split woven bamboo market basket (Jhaka)
    const vec3 clayPotCol  (0.74f, 0.38f, 0.20f); // Terracotta clay pot (Matir Handi)
    const vec3 leafGreen   (0.22f, 0.56f, 0.20f); // Fresh banana leaf wrap

    // Subtle natural swaying animation responsive to boat rocking
    float sway1 = (animTime > 0.0f) ? (sinf(animTime * 1.8f) * radians(2.2f)) : 0.0f;
    float sway2 = (animTime > 0.0f) ? (cosf(animTime * 1.8f) * radians(2.0f)) : 0.0f;

    // ═════════════════════════════════════════════════════════════
    // PASSENGER 1: SEATED FORWARD (STARBOARD SIDE / +X, +Z)
    // ═════════════════════════════════════════════════════════════
    // Sitting on thwart at Z = 0.85m
    mat4 p1 = model;
    p1 = translate(p1, vec3(0.12f, 0.32f, 0.85f));
    p1 = rotate(p1, radians(-10.0f) + sway1, vec3(0.0f, 1.0f, 0.0f));

    // 1. Draped Lungi / Pelvis on seat
    mat4 p1Pelvis = p1;
    p1Pelvis = translate(p1Pelvis, vec3(0.0f, 0.08f, 0.0f));
    p1Pelvis = scale(p1Pelvis, vec3(0.20f, 0.14f, 0.20f));
    Primitives::drawCube(shader, p1Pelvis, lungiMaroon);

    // Seated bent thighs extending forward
    mat4 p1Thighs = p1;
    p1Thighs = translate(p1Thighs, vec3(0.0f, 0.06f, 0.10f));
    p1Thighs = scale(p1Thighs, vec3(0.18f, 0.10f, 0.18f));
    Primitives::drawCube(shader, p1Thighs, lungiMaroon);

    // Draped shins hanging down towards floorboards
    for (int leg = -1; leg <= 1; leg += 2) {
        mat4 p1Shin = p1;
        p1Shin = translate(p1Shin, vec3((float)leg * 0.055f, -0.09f, 0.16f));
        p1Shin = scale(p1Shin, vec3(0.065f, 0.18f, 0.065f));
        Primitives::drawCylinder(shader, p1Shin, lungiMaroon);

        // Bare feet resting on deck
        mat4 p1Foot = p1;
        p1Foot = translate(p1Foot, vec3((float)leg * 0.055f, -0.19f, 0.20f));
        p1Foot = scale(p1Foot, vec3(0.055f, 0.035f, 0.09f));
        Primitives::drawCube(shader, p1Foot, skinTone1);
    }

    // 2. Upright Torso in vibrant indigo/teal kurta
    mat4 p1Torso = p1;
    p1Torso = translate(p1Torso, vec3(0.0f, 0.32f, 0.0f));
    p1Torso = scale(p1Torso, vec3(0.23f, 0.36f, 0.18f));
    Primitives::drawCylinder(shader, p1Torso, kurtaTeal);

    // Kurta collar
    mat4 p1Collar = p1;
    p1Collar = translate(p1Collar, vec3(0.0f, 0.49f, 0.0f));
    p1Collar = scale(p1Collar, vec3(0.09f, 0.03f, 0.09f));
    Primitives::drawCylinder(shader, p1Collar, kurtaTeal);

    // 3. Draped Crimson Gamcha Scarf across right shoulder
    mat4 p1Gamcha = p1;
    p1Gamcha = translate(p1Gamcha, vec3(0.08f, 0.32f, 0.01f));
    p1Gamcha = scale(p1Gamcha, vec3(0.07f, 0.38f, 0.20f));
    Primitives::drawCube(shader, p1Gamcha, gamchaRed);

    // 4. Arms with hands resting comfortably on knees
    for (int arm = -1; arm <= 1; arm += 2) {
        float farm = (float)arm;
        mat4 p1Arm = p1;
        p1Arm = translate(p1Arm, vec3(farm * 0.13f, 0.30f, 0.04f));
        p1Arm = rotate(p1Arm, radians(24.0f), vec3(1.0f, 0.0f, 0.0f));
        p1Arm = scale(p1Arm, vec3(0.052f, 0.20f, 0.052f));
        Primitives::drawCylinder(shader, p1Arm, kurtaTeal);

        mat4 p1Forearm = p1;
        p1Forearm = translate(p1Forearm, vec3(farm * 0.12f, 0.16f, 0.14f));
        p1Forearm = rotate(p1Forearm, radians(62.0f), vec3(1.0f, 0.0f, 0.0f));
        p1Forearm = scale(p1Forearm, vec3(0.045f, 0.18f, 0.045f));
        Primitives::drawCylinder(shader, p1Forearm, skinTone1);

        mat4 p1Hand = p1;
        p1Hand = translate(p1Hand, vec3(farm * 0.10f, 0.12f, 0.22f));
        p1Hand = scale(p1Hand, vec3(0.05f, 0.04f, 0.06f));
        Primitives::drawSphere(shader, p1Hand, skinTone1);
    }

    // 5. Head and Neck
    mat4 p1Neck = p1;
    p1Neck = translate(p1Neck, vec3(0.0f, 0.54f, 0.0f));
    p1Neck = scale(p1Neck, vec3(0.08f, 0.09f, 0.08f));
    Primitives::drawCylinder(shader, p1Neck, skinTone1);

    mat4 p1Head = p1;
    p1Head = translate(p1Head, vec3(0.0f, 0.64f, 0.0f));
    p1Head = scale(p1Head, vec3(0.13f, 0.13f, 0.13f));
    Primitives::drawSphere(shader, p1Head, skinTone1);

    // Dark hair band
    mat4 p1Hair = p1;
    p1Hair = translate(p1Hair, vec3(0.0f, 0.67f, 0.0f));
    p1Hair = scale(p1Hair, vec3(0.14f, 0.05f, 0.14f));
    Primitives::drawCylinder(shader, p1Hair, hairDark);

    // Conical Bamboo Sunhat (Mathal / মাথাল)
    mat4 p1Hat = p1;
    p1Hat = translate(p1Hat, vec3(0.0f, 0.72f, 0.0f));
    p1Hat = scale(p1Hat, vec3(0.28f, 0.10f, 0.28f));
    Primitives::drawCone(shader, p1Hat, strawYellow);

    mat4 p1HatRim = p1;
    p1HatRim = translate(p1HatRim, vec3(0.0f, 0.72f, 0.0f));
    p1HatRim = scale(p1HatRim, vec3(0.285f, 0.015f, 0.285f));
    Primitives::drawCylinder(shader, p1HatRim, hairDark);

    // ═════════════════════════════════════════════════════════════
    // PASSENGER 2: SEATED COMPANION (PORT SIDE / -X, +Z)
    // ═════════════════════════════════════════════════════════════
    // Sitting on thwart at Z = 0.85m, turned slightly inward in conversation
    mat4 p2 = model;
    p2 = translate(p2, vec3(-0.12f, 0.32f, 0.85f));
    p2 = rotate(p2, radians(10.0f) + sway2, vec3(0.0f, 1.0f, 0.0f));

    // 1. Draped Lungi / Pelvis on seat in emerald green
    mat4 p2Pelvis = p2;
    p2Pelvis = translate(p2Pelvis, vec3(0.0f, 0.08f, 0.0f));
    p2Pelvis = scale(p2Pelvis, vec3(0.20f, 0.14f, 0.20f));
    Primitives::drawCube(shader, p2Pelvis, lungiGreen);

    // Seated bent thighs
    mat4 p2Thighs = p2;
    p2Thighs = translate(p2Thighs, vec3(0.0f, 0.06f, 0.10f));
    p2Thighs = scale(p2Thighs, vec3(0.18f, 0.10f, 0.18f));
    Primitives::drawCube(shader, p2Thighs, lungiGreen);

    // Shins & feet
    for (int leg = -1; leg <= 1; leg += 2) {
        mat4 p2Shin = p2;
        p2Shin = translate(p2Shin, vec3((float)leg * 0.055f, -0.09f, 0.16f));
        p2Shin = scale(p2Shin, vec3(0.065f, 0.18f, 0.065f));
        Primitives::drawCylinder(shader, p2Shin, lungiGreen);

        mat4 p2Foot = p2;
        p2Foot = translate(p2Foot, vec3((float)leg * 0.055f, -0.19f, 0.20f));
        p2Foot = scale(p2Foot, vec3(0.055f, 0.035f, 0.09f));
        Primitives::drawCube(shader, p2Foot, skinTone2);
    }

    // 2. Upright Torso in sun-baked golden/saffron kurta
    mat4 p2Torso = p2;
    p2Torso = translate(p2Torso, vec3(0.0f, 0.32f, 0.0f));
    p2Torso = scale(p2Torso, vec3(0.23f, 0.36f, 0.18f));
    Primitives::drawCylinder(shader, p2Torso, kurtaOchre);

    // White cotton scarf / gamcha draped around neck
    mat4 p2Gamcha = p2;
    p2Gamcha = translate(p2Gamcha, vec3(-0.08f, 0.32f, 0.01f));
    p2Gamcha = scale(p2Gamcha, vec3(0.07f, 0.38f, 0.20f));
    Primitives::drawCube(shader, p2Gamcha, gamchaWhite);

    // 3. Arms (left arm resting on gunwale rub-rail, right hand on lap gesturing)
    mat4 p2ArmL = p2;
    p2ArmL = translate(p2ArmL, vec3(-0.13f, 0.28f, 0.02f));
    p2ArmL = rotate(p2ArmL, radians(-25.0f), vec3(0.0f, 0.0f, 1.0f));
    p2ArmL = scale(p2ArmL, vec3(0.05f, 0.22f, 0.05f));
    Primitives::drawCylinder(shader, p2ArmL, kurtaOchre);

    mat4 p2HandL = p2;
    p2HandL = translate(p2HandL, vec3(-0.20f, 0.15f, 0.08f));
    p2HandL = scale(p2HandL, vec3(0.05f, 0.04f, 0.06f));
    Primitives::drawSphere(shader, p2HandL, skinTone2);

    mat4 p2ArmR = p2;
    p2ArmR = translate(p2ArmR, vec3(0.12f, 0.28f, 0.06f));
    p2ArmR = rotate(p2ArmR, radians(38.0f), vec3(1.0f, 0.0f, 0.0f));
    p2ArmR = scale(p2ArmR, vec3(0.05f, 0.20f, 0.05f));
    Primitives::drawCylinder(shader, p2ArmR, kurtaOchre);

    mat4 p2HandR = p2;
    p2HandR = translate(p2HandR, vec3(0.10f, 0.14f, 0.20f));
    p2HandR = scale(p2HandR, vec3(0.05f, 0.04f, 0.06f));
    Primitives::drawSphere(shader, p2HandR, skinTone2);

    // 4. Head and Neck
    mat4 p2Neck = p2;
    p2Neck = translate(p2Neck, vec3(0.0f, 0.54f, 0.0f));
    p2Neck = scale(p2Neck, vec3(0.08f, 0.09f, 0.08f));
    Primitives::drawCylinder(shader, p2Neck, skinTone2);

    mat4 p2Head = p2;
    p2Head = translate(p2Head, vec3(0.0f, 0.64f, 0.0f));
    p2Head = scale(p2Head, vec3(0.13f, 0.13f, 0.13f));
    Primitives::drawSphere(shader, p2Head, skinTone2);

    // Dark hair & traditional white cotton cap (Taqiyah)
    mat4 p2Hair = p2;
    p2Hair = translate(p2Hair, vec3(0.0f, 0.67f, 0.0f));
    p2Hair = scale(p2Hair, vec3(0.138f, 0.05f, 0.138f));
    Primitives::drawCylinder(shader, p2Hair, hairDark);

    mat4 p2Cap = p2;
    p2Cap = translate(p2Cap, vec3(0.0f, 0.71f, 0.0f));
    p2Cap = scale(p2Cap, vec3(0.125f, 0.06f, 0.125f));
    Primitives::drawHemisphere(shader, p2Cap, gamchaWhite);

    // ═════════════════════════════════════════════════════════════
    // 3. TRADITIONAL TRAVEL CARGO: MARKET BASKET & CLAY POT
    // ═════════════════════════════════════════════════════════════
    // Resting on deck forward of the passengers at Z = 1.30m
    mat4 basketM = model;
    basketM = translate(basketM, vec3(-0.02f, 0.23f, 1.30f));
    basketM = scale(basketM, vec3(0.18f, 0.16f, 0.18f));
    Primitives::drawCylinder(shader, basketM, basketWood);

    mat4 basketRim = model;
    basketRim = translate(basketRim, vec3(-0.02f, 0.30f, 1.30f));
    basketRim = scale(basketRim, vec3(0.20f, 0.022f, 0.20f));
    Primitives::drawCylinder(shader, basketRim, basketWood);

    mat4 leafM = model;
    leafM = translate(leafM, vec3(-0.02f, 0.29f, 1.30f));
    leafM = scale(leafM, vec3(0.09f, 0.018f, 0.09f));
    Primitives::drawSphere(shader, leafM, leafGreen);

    // Earthen terracotta water/curd pot (Matir Handi) nestled beside basket
    mat4 potM = model;
    potM = translate(potM, vec3(0.12f, 0.23f, 1.28f));
    potM = scale(potM, vec3(0.080f, 0.080f, 0.080f));
    Primitives::drawSphere(shader, potM, clayPotCol);

    mat4 potRim = model;
    potRim = translate(potRim, vec3(0.12f, 0.295f, 1.28f));
    potRim = scale(potRim, vec3(0.060f, 0.028f, 0.060f));
    Primitives::drawCylinder(shader, potRim, clayPotCol);
}

void draw(Shader& shader, const mat4& model, BoatStyle style, float animTime, float oarAnim, int numPassengers)
{
    bool showPassengers = (numPassengers > 0) || (numPassengers < 0 && style == BOAT_STYLE_ROUND_CHHOI);
    shader.setInt("uUseTexture", 0);

    // ── Precise Color Palette Matching the User's Cropped Artwork ─
    vec3 hullBody    (0.28f, 0.14f, 0.07f); // Deep dark chocolate / umber timber (#4A2612)
    vec3 redStripe   (0.68f, 0.22f, 0.14f); // Signature terracotta / crimson red side stripe (#A53526)
    vec3 gunwaleTrim (0.16f, 0.08f, 0.04f); // Dark black-brown gunwale rub-rail & horn tips
    vec3 innerFloor  (0.36f, 0.18f, 0.09f); // Inner timber floorboards & thwarts
    vec3 mastBrown   (0.28f, 0.14f, 0.07f); // Dark brown bamboo mast
    vec3 sailWhite   (0.95f, 0.93f, 0.87f); // Warm ivory/cream cotton sail canvas (#F4EFE2)
    vec3 sailRed     (0.68f, 0.22f, 0.14f); // Signature crimson red folk triangle patch
    vec3 chhoiBox    (0.26f, 0.13f, 0.06f); // Dark brown timber canopy base plinth
    vec3 chhoiLight  (0.74f, 0.52f, 0.27f); // Warm golden-caramel upper dome (#BD8545)
    vec3 chhoiDark   (0.55f, 0.35f, 0.15f); // Darker caramel middle/lower crescent (#8C5926)

    // ═════════════════════════════════════════════════════════════
    // 1. SMOOTH LOFTED WATERTIGHT HULL, CRIMSON STRIPE & FLOOR
    // ═════════════════════════════════════════════════════════════
    // Mathematically continuous lofted Bengali Dingi Nouka hull:
    // Generates 14 smoothly connected cross-sections along the boat length (Z in [-2.15m, +2.15m]).
    // Ensures ZERO steps, ZERO disjoint boards, ZERO gaps, and 100% watertight inner deck!

    struct HullStation {
        float z;
        vec3 ptKeelL, ptKeelR;
        vec3 ptStripe0L, ptStripe0R;
        vec3 ptStripe1L, ptStripe1R;
        vec3 ptGunwaleL, ptGunwaleR;
        vec3 ptFloorL, ptFloorR;
    };

    const int NUM_SEGS = 14;
    HullStation st[NUM_SEGS + 1];

    for (int i = 0; i <= NUM_SEGS; ++i) {
        float t = -1.0f + 2.0f * ((float)i / (float)NUM_SEGS); // [-1.0, +1.0]
        float z = t * 2.15f;
        float u = fabsf(t);

        // Continuous quadratic rocker and flare profiles
        float yKeel = 0.035f + 0.40f * (u * u);
        float xKeel = 0.28f * (1.0f - u * u) + 0.012f;

        float yGunwale = 0.32f + 0.18f * (u * u);
        float xGunwale = 0.39f * (1.0f - u * u) + 0.024f;

        float yFloor = (yKeel + 0.035f > 0.08f) ? (yKeel + 0.035f) : 0.08f;
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
        draw3DQuad(shader, model, s0.ptKeelL, s0.ptKeelR, s1.ptKeelR, s1.ptKeelL, hullBody);

        // 2. Starboard Lower Flank (+X)
        draw3DQuad(shader, model, s0.ptKeelR, s0.ptStripe0R, s1.ptStripe0R, s1.ptKeelR, hullBody);

        // 3. Port Lower Flank (-X)
        draw3DQuad(shader, model, s0.ptKeelL, s1.ptKeelL, s1.ptStripe0L, s0.ptStripe0L, hullBody);

        // 4. Starboard Crimson Folk Accent Stripe (+X)
        draw3DQuad(shader, model, s0.ptStripe0R, s0.ptStripe1R, s1.ptStripe1R, s1.ptStripe0R, redStripe);

        // 5. Port Crimson Folk Accent Stripe (-X)
        draw3DQuad(shader, model, s0.ptStripe0L, s1.ptStripe0L, s1.ptStripe1L, s0.ptStripe1L, redStripe);

        // 6. Starboard Upper Flank (+X)
        draw3DQuad(shader, model, s0.ptStripe1R, s0.ptGunwaleR, s1.ptGunwaleR, s1.ptStripe1R, hullBody);

        // 7. Port Upper Flank (-X)
        draw3DQuad(shader, model, s0.ptStripe1L, s1.ptStripe1L, s1.ptGunwaleL, s0.ptGunwaleL, hullBody);

        // 8. Watertight Inner Timber Floor (seen from above)
        draw3DQuad(shader, model, s0.ptFloorL, s1.ptFloorL, s1.ptFloorR, s0.ptFloorR, innerFloor);

        // 9. Inner Side Strakes (connecting inner floor to gunwales)
        draw3DQuad(shader, model, s0.ptFloorR, s1.ptFloorR, s1.ptGunwaleR, s0.ptGunwaleR, innerFloor * 0.88f);
        draw3DQuad(shader, model, s0.ptFloorL, s0.ptGunwaleL, s1.ptGunwaleL, s1.ptFloorL, innerFloor * 0.88f);

        // 10. Gunwale Rub-Rail Capping
        drawLineCylinder(shader, model, s0.ptGunwaleR, s1.ptGunwaleR, 0.022f, gunwaleTrim);
        drawLineCylinder(shader, model, s0.ptGunwaleL, s1.ptGunwaleL, 0.022f, gunwaleTrim);
    }

    // Stem Caps at Bow and Stern Horn Apexes (গুলুই / Gului)
    mat4 bowStem = model;
    bowStem = translate(bowStem, vec3(0.0f, 0.50f, 2.15f));
    bowStem = rotate(bowStem, radians(24.0f), vec3(1.0f, 0.0f, 0.0f));
    bowStem = scale(bowStem, vec3(0.045f, 0.12f, 0.06f));
    Primitives::drawCube(shader, bowStem, gunwaleTrim);

    mat4 sternStem = model;
    sternStem = translate(sternStem, vec3(0.0f, 0.50f, -2.15f));
    sternStem = rotate(sternStem, radians(-24.0f), vec3(1.0f, 0.0f, 0.0f));
    sternStem = scale(sternStem, vec3(0.045f, 0.12f, 0.06f));
    Primitives::drawCube(shader, sternStem, gunwaleTrim);

    // Cross Thwarts (কাঠের গুলুই ও বসার পাটাতন)
    for (float tz : { -1.30f, -0.75f, 0.0f, 0.75f, 1.30f }) {
        float u = fabsf(tz) / 2.15f;
        float gw = 2.0f * (0.39f * (1.0f - u * u) + 0.024f) - 0.04f;
        float gy = 0.32f + 0.18f * (u * u) - 0.02f;
        mat4 thwart = model;
        thwart = translate(thwart, vec3(0.0f, gy, tz));
        thwart = scale(thwart, vec3(gw, 0.035f, 0.08f));
        Primitives::drawCube(shader, thwart, innerFloor);
    }

    // ═════════════════════════════════════════════════════════════
    // 2. STYLE-SPECIFIC SUPERSTRUCTURES MATCHING REFERENCE IMAGES
    // ═════════════════════════════════════════════════════════════

    if (style == BOAT_STYLE_ROUND_CHHOI) {
        // =========================================================
        // IMAGE 2: AUTHENTIC ARCHED BAMBOO CHHOI CANOPY & ROWING MAJHI
        // =========================================================
        // Hollow arched bamboo canopy (বাঁশের তৈরি অর্ধবৃত্তাকার ছই)
        // Spans the midsection with woven split-bamboo matting and reinforcing hoops.
        const float chhoiLen = 1.35f;

        // Outer warm golden-caramel woven bamboo canopy arch
        mat4 archM = model;
        archM = translate(archM, vec3(0.0f, 0.35f, 0.0f));
        archM = scale(archM, vec3(0.78f, 0.76f, chhoiLen));
        Primitives::drawArch(shader, archM, chhoiLight);

        // Inner darker shaded bamboo liner
        mat4 innerArch = model;
        innerArch = translate(innerArch, vec3(0.0f, 0.35f, 0.0f));
        innerArch = scale(innerArch, vec3(0.74f, 0.72f, chhoiLen * 0.98f));
        Primitives::drawArch(shader, innerArch, chhoiDark);

        // 4 Arched structural bamboo reinforcing hoops (পাঁজর / কম্বল ধনুক)
        for (float rz : { -0.66f, -0.22f, 0.22f, 0.66f }) {
            mat4 rib = model;
            rib = translate(rib, vec3(0.0f, 0.35f, rz));
            rib = scale(rib, vec3(0.79f, 0.77f, 0.035f));
            Primitives::drawArch(shader, rib, chhoiBox);
        }

        // Longitudinal bamboo runners along crest and flanks
        mat4 crownRunner = model;
        crownRunner = translate(crownRunner, vec3(0.0f, 0.735f, 0.0f));
        crownRunner = scale(crownRunner, vec3(0.035f, 0.025f, chhoiLen + 0.04f));
        Primitives::drawCube(shader, crownRunner, chhoiBox);

        for (int s = -1; s <= 1; s += 2) {
            mat4 flankRunner = model;
            flankRunner = translate(flankRunner, vec3((float)s * 0.37f, 0.55f, 0.0f));
            flankRunner = scale(flankRunner, vec3(0.030f, 0.030f, chhoiLen + 0.04f));
            Primitives::drawCube(shader, flankRunner, chhoiBox);
        }

        // Boatman standing on open stern deck (left) holding diagonal Boitha oar dipping into water
        drawMajhi(shader, model, true, oarAnim);

        // 2 Passengers seated on the forward deck thwart
        if (showPassengers) {
            drawPassengers(shader, model, animTime);
        }
    }
    else if (style == BOAT_STYLE_RED_SAIL) {
        // =========================================================
        // IMAGE 3: RED-SAIL BOAT & STANDING MAJHI
        // =========================================================
        const float mastZ = 0.05f;
        const float mastH = 2.45f;

        // Tall vertical dark brown bamboo mast
        mat4 mast = model;
        mast = translate(mast, vec3(0.0f, 0.10f + mastH * 0.5f, mastZ));
        mast = scale(mast, vec3(0.065f, mastH, 0.065f));
        Primitives::drawCylinder(shader, mast, mastBrown);

        // Mast cap knob
        mat4 cap = model;
        cap = translate(cap, vec3(0.0f, 0.10f + mastH, mastZ));
        cap = scale(cap, vec3(0.085f, 0.06f, 0.085f));
        Primitives::drawSphere(shader, cap, mastBrown);

        // Horizontal bottom boom spar supporting sail foot (rotated along Z!)
        mat4 boom = model;
        boom = translate(boom, vec3(0.0f, 0.45f, mastZ + 0.78f));
        boom = rotate(boom, radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
        boom = scale(boom, vec3(0.045f, 1.55f, 0.045f));
        Primitives::drawCylinder(shader, boom, mastBrown);

        // Right-angled triangular white canvas sail
        drawRightTriangleSail(shader, model, mastZ, 0.45f, 1.50f, 1.95f, sailWhite, 0.0f);

        // SIGNATURE CRIMSON RED RIGHT-TRIANGLE FOLK PATCH (exact match to Image 3!)
        // Sits on both sides of the canvas
        drawRightTriangleSail(shader, model, mastZ, 1.05f, 0.62f, 0.80f, sailRed, 0.005f);
        drawRightTriangleSail(shader, model, mastZ, 1.05f, 0.62f, 0.80f, sailRed, -0.005f);

        // Boatman standing on open stern deck (left)
        drawMajhi(shader, model, false, 0.0f);

        if (showPassengers) {
            drawPassengers(shader, model, animTime);
        }
    }
    else {
        // =========================================================
        // IMAGE 1: PURE WHITE RIGHT-ANGLED SAILBOAT
        // =========================================================
        const float mastZ = 0.05f;
        const float mastH = 2.55f;

        // Tall vertical dark brown bamboo mast
        mat4 mast = model;
        mast = translate(mast, vec3(0.0f, 0.10f + mastH * 0.5f, mastZ));
        mast = scale(mast, vec3(0.065f, mastH, 0.065f));
        Primitives::drawCylinder(shader, mast, mastBrown);

        // Mast cap knob
        mat4 cap = model;
        cap = translate(cap, vec3(0.0f, 0.10f + mastH, mastZ));
        cap = scale(cap, vec3(0.085f, 0.06f, 0.085f));
        Primitives::drawSphere(shader, cap, mastBrown);

        // Horizontal bottom boom spar supporting sail foot (rotated along Z!)
        mat4 boom = model;
        boom = translate(boom, vec3(0.0f, 0.45f, mastZ + 0.80f));
        boom = rotate(boom, radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
        boom = scale(boom, vec3(0.045f, 1.60f, 0.045f));
        Primitives::drawCylinder(shader, boom, mastBrown);

        // Pure white right-angled triangular sail (exact match to Image 1!)
        drawRightTriangleSail(shader, model, mastZ, 0.45f, 1.55f, 2.05f, sailWhite, 0.0f);

        if (showPassengers) {
            drawPassengers(shader, model, animTime);
        }
    }
}

} // namespace Boat
