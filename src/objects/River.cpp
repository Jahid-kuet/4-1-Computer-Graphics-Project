// River.cpp — Authentic meandering Bangladeshi river (Nodi)
// Features a gently curved channel, terraced sandy/muddy riverbanks,
// animated water flow ripples, a traditional wooden/bamboo landing ghat (dock)
// with mooring posts, floating water lilies (Shapla), and tall catkin reeds (Kashbon).

#include "objects/River.h"
#include "Primitives.h"
#include <cmath>

using namespace math;

namespace {

// River centerline curvature function: gives X offset as a function of Z
inline float riverCenterline(float z)
{
    return 1.8f * sinf(z * 0.08f + 0.4f) + 0.6f * cosf(z * 0.04f);
}

// Derivative of centerline to compute bank orientation
inline float riverTangentAngle(float z)
{
    float dx = 1.8f * 0.08f * cosf(z * 0.08f + 0.4f) - 0.6f * 0.04f * sinf(z * 0.04f);
    return atan2f(dx, 1.0f); // angle with Z axis
}

// Draw a single Water Lily (Shapla) pad and blossom
void drawShapla(Shader& shader, const mat4& model, const vec3& pos, float scaleVal = 1.0f)
{
    vec3 padColor   (0.18f, 0.45f, 0.20f);  // lush lily pad green
    vec3 petalWhite (0.95f, 0.92f, 0.94f);  // white/pale pink petal
    vec3 flowerHeart(0.92f, 0.78f, 0.15f);  // golden pollen center

    mat4 m = model;
    m = translate(m, pos);
    m = scale(m, vec3(scaleVal));

    // Lily pad (flattened green cylinder floating on water)
    mat4 pad = m;
    pad = scale(pad, vec3(0.40f, 0.015f, 0.38f));
    Primitives::drawCylinder(shader, pad, padColor);

    // Blossom petals (small angled cones radiating from center)
    for (int p = 0; p < 6; p++) {
        float angle = p * (2.0f * PI / 6.0f);
        mat4 petal = m;
        petal = translate(petal, vec3(0.0f, 0.02f, 0.0f));
        petal = rotate(petal, angle, vec3(0.0f, 1.0f, 0.0f));
        petal = rotate(petal, radians(35.0f), vec3(1.0f, 0.0f, 0.0f));
        petal = scale(petal, vec3(0.05f, 0.12f, 0.03f));
        Primitives::drawCone(shader, petal, petalWhite);
    }

    // Flower center
    mat4 center = m;
    center = translate(center, vec3(0.0f, 0.04f, 0.0f));
    center = scale(center, vec3(0.04f, 0.03f, 0.04f));
    Primitives::drawSphere(shader, center, flowerHeart);
}

// Draw a cluster of riverbank reeds / Kashbon (Catkins)
void drawKashbonCluster(Shader& shader, const mat4& model, const vec3& pos, int count, float seed)
{
    vec3 stalkColor(0.25f, 0.42f, 0.18f); // green-brown reed stalk
    vec3 plumeColor(0.90f, 0.90f, 0.84f); // fluffy white/cream catkin plume

    for (int i = 0; i < count; i++) {
        float fi = (float)i;
        float ox = sinf(seed + fi * 1.7f) * 0.4f;
        float oz = cosf(seed + fi * 2.3f) * 0.4f;
        float h  = 1.2f + sinf(seed * 2.0f + fi) * 0.35f;
        float tiltX = sinf(seed + fi) * 10.0f;
        float tiltZ = cosf(seed + fi) * 10.0f;

        mat4 stalk = model;
        stalk = translate(stalk, pos + vec3(ox, h * 0.5f, oz));
        stalk = rotate(stalk, radians(tiltX), vec3(1.0f, 0.0f, 0.0f));
        stalk = rotate(stalk, radians(tiltZ), vec3(0.0f, 0.0f, 1.0f));

        // Thin stalk
        mat4 sM = scale(stalk, vec3(0.02f, h, 0.02f));
        Primitives::drawCylinder(shader, sM, stalkColor);

        // Soft slender catkin plume at tip
        mat4 pM = stalk;
        pM = translate(pM, vec3(0.0f, h * 0.35f, 0.0f));
        pM = scale(pM, vec3(0.022f, 0.38f, 0.022f));
        Primitives::drawCylinder(shader, pM, plumeColor);

        mat4 pTip = stalk;
        pTip = translate(pTip, vec3(0.0f, h * 0.54f, 0.0f));
        pTip = scale(pTip, vec3(0.022f, 0.035f, 0.022f));
        Primitives::drawSphere(shader, pTip, plumeColor);
    }
}

// Draw a simple rural bamboo mooring stake (Khuti) driven into the riverbank
void drawMooringStake(Shader& shader, const mat4& model, const vec3& rootPos)
{
    vec3 stakeColor(0.38f, 0.28f, 0.14f); // weathered bamboo stake
    vec3 ropeColor (0.62f, 0.54f, 0.38f); // natural coir/jute rope

    mat4 m = model;
    m = translate(m, rootPos);

    // Bamboo mooring stake angled slightly into the bank
    mat4 stake = m;
    stake = translate(stake, vec3(0.0f, 0.38f, 0.0f));
    stake = rotate(stake, radians(-10.0f), vec3(0.0f, 0.0f, 1.0f));
    stake = scale(stake, vec3(0.05f, 0.85f, 0.05f));
    Primitives::drawCylinder(shader, stake, stakeColor);

    // Coiled coir rope ring around stake
    mat4 rope = m;
    rope = translate(rope, vec3(0.0f, 0.50f, 0.0f));
    rope = scale(rope, vec3(0.075f, 0.06f, 0.075f));
    Primitives::drawCylinder(shader, rope, ropeColor);
}

} // anonymous namespace

namespace River {

void draw(Shader& shader, const mat4& model, float time)
{
    // ── River Colors ─────────────────────────────────────────────
    vec3 deepWaterCol (0.08f, 0.26f, 0.42f); // deep Bengal river blue-green
    vec3 shoreWaterCol(0.15f, 0.45f, 0.58f); // shallower edge water
    vec3 bankSandCol  (0.52f, 0.40f, 0.25f); // wet sandy/muddy shoreline
    vec3 bankMudCol   (0.42f, 0.32f, 0.18f); // dark silt / fertile riverbed
    vec3 rippleCol    (0.35f, 0.65f, 0.80f); // shimmering water ripple

    const float riverWidth = 7.5f;
    const float halfWidth  = riverWidth * 0.5f;
    const int   numSegments = 22;
    const float zStart = -28.0f;
    const float zEnd   =  28.0f;
    const float stepZ  = (zEnd - zStart) / numSegments;

    // ── 1. Meandering River Water Body & Banks ───────────────────
    for (int i = 0; i < numSegments; i++) {
        float z0 = zStart + i * stepZ;
        float z1 = z0 + stepZ;
        float zMid = (z0 + z1) * 0.5f;

        float xCenter = riverCenterline(zMid);
        float angle   = riverTangentAngle(zMid);

        mat4 segM = model;
        segM = translate(segM, vec3(xCenter, 0.02f, zMid));
        segM = rotate(segM, angle, vec3(0.0f, 1.0f, 0.0f));

        // Full-width continuous river water surface
        mat4 water = segM;
        water = scale(water, vec3(riverWidth, 1.0f, stepZ * 1.08f));
        Primitives::drawPlane(shader, water, deepWaterCol);

        // Left sloped muddy bank (sloping up toward village land)
        mat4 leftBank = segM;
        leftBank = translate(leftBank, vec3(-halfWidth - 0.7f, 0.05f, 0.0f));
        leftBank = rotate(leftBank, radians(15.0f), vec3(0.0f, 0.0f, 1.0f));
        leftBank = scale(leftBank, vec3(1.6f, 1.0f, stepZ * 1.08f));
        Primitives::drawPlane(shader, leftBank, bankSandCol);

        // Right sloped muddy bank (far shore)
        mat4 rightBank = segM;
        rightBank = translate(rightBank, vec3(halfWidth + 0.7f, 0.05f, 0.0f));
        rightBank = rotate(rightBank, radians(-15.0f), vec3(0.0f, 0.0f, 1.0f));
        rightBank = scale(rightBank, vec3(1.6f, 1.0f, stepZ * 1.08f));
        Primitives::drawPlane(shader, rightBank, bankSandCol);
    }

    // ── 2. River Mooring Stake on Sandy Shore ───────────────────
    // Located on the village bank (left side of river) near Z = 1.2
    float stakeZ = 1.2f;
    float stakeX = riverCenterline(stakeZ) - halfWidth + 0.2f;
    drawMooringStake(shader, model, vec3(stakeX, 0.04f, stakeZ));

    // ── 3. Water Lilies (Shapla) in Calm River Shore Pockets ─────
    float shaplaPositions[][2] = {
        { -3.0f, -4.5f },
        { -2.8f, -3.8f },
        { -3.4f, -5.2f },
        { -3.2f,  5.5f },
        { -3.5f,  6.2f },
        {  3.2f, -8.0f },
        {  3.5f, -7.2f }
    };
    for (int s = 0; s < 7; s++) {
        float sz = shaplaPositions[s][1];
        float sx = riverCenterline(sz) + shaplaPositions[s][0];
        drawShapla(shader, model, vec3(sx, 0.03f, sz), 0.85f);
    }

    // ── 4. Riverbank Reeds / Kashbon (Catkins) along the Shore ───
    float reedLocations[][2] = {
        { -halfWidth - 1.2f, -12.0f },
        { -halfWidth - 1.4f,  -8.0f },
        { -halfWidth - 1.2f,  -2.0f },
        { -halfWidth - 1.3f,   8.0f },
        { -halfWidth - 1.4f,  14.0f },
        {  halfWidth + 1.2f, -10.0f },
        {  halfWidth + 1.3f,   4.0f },
        {  halfWidth + 1.2f,  12.0f }
    };
    for (int r = 0; r < 8; r++) {
        float rz = reedLocations[r][1];
        float rx = riverCenterline(rz) + reedLocations[r][0];
        drawKashbonCluster(shader, model, vec3(rx, 0.0f, rz), 5, (float)r * 3.14f);
    }
}

} // namespace River
