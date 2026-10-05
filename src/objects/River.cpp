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

} // anonymous namespace

namespace River {

// Draw a single Water Lily (Shapla / শাপলা) pad and multi-tiered blossom
void drawShapla(Shader& shader, const mat4& model, const vec3& pos, float scaleVal)
{
    shader.setInt("uUseTexture", 0); // pristine organic aquatic colors

    vec3 padColor   (0.16f, 0.44f, 0.18f);  // lush floating emerald lily pad
    vec3 padNotchCol(0.12f, 0.32f, 0.14f);  // darker radial cleft / notch
    vec3 petalWhite (0.97f, 0.96f, 0.94f);  // pristine ivory-white blossom petals
    vec3 petalBlush (0.94f, 0.82f, 0.88f);  // soft pinkish blush at outer petal tips
    vec3 flowerHeart(0.98f, 0.82f, 0.12f);  // bright golden pollen bowl (Keshor)
    vec3 stemColor  (0.14f, 0.36f, 0.16f);  // submerged stem

    mat4 m = model;
    m = translate(m, pos);
    m = scale(m, vec3(scaleVal));

    // Submerged floral stem
    mat4 stem = m;
    stem = translate(stem, vec3(0.0f, -0.06f, 0.0f));
    stem = scale(stem, vec3(0.022f, 0.12f, 0.022f));
    Primitives::drawCylinder(shader, stem, stemColor);

    // 1. Floating Lily Pad (circular pad with distinctive radial slit)
    mat4 pad = m;
    pad = translate(pad, vec3(0.0f, 0.005f, 0.0f));
    pad = scale(pad, vec3(0.44f, 0.012f, 0.42f));
    Primitives::drawCylinder(shader, pad, padColor);

    // Radial slit notch (dark cleft cutout illusion)
    mat4 notch = m;
    notch = translate(notch, vec3(0.18f, 0.008f, 0.0f));
    notch = scale(notch, vec3(0.16f, 0.015f, 0.045f));
    Primitives::drawCube(shader, notch, padNotchCol);

    // 2. Blossom Base Receptacle / Calyx
    mat4 calyx = m;
    calyx = translate(calyx, vec3(0.0f, 0.015f, 0.0f));
    calyx = scale(calyx, vec3(0.065f, 0.025f, 0.065f));
    Primitives::drawSphere(shader, calyx, stemColor);

    // 3. Outer Ring of Spreading White Petals (8 petals)
    const int outerCount = 8;
    for (int p = 0; p < outerCount; p++) {
        float angle = (float)p * (2.0f * PI / (float)outerCount);
        mat4 pet = m;
        pet = translate(pet, vec3(0.0f, 0.022f, 0.0f));
        pet = rotate(pet, angle, vec3(0.0f, 1.0f, 0.0f));
        pet = rotate(pet, radians(48.0f), vec3(1.0f, 0.0f, 0.0f)); // spreading outward
        pet = scale(pet, vec3(0.038f, 0.16f, 0.022f));
        Primitives::drawCone(shader, pet, petalBlush);
    }

    // 4. Inner Ring of Upright Cupped White Petals (8 petals)
    const int innerCount = 8;
    for (int p = 0; p < innerCount; p++) {
        float angle = ((float)p + 0.5f) * (2.0f * PI / (float)innerCount);
        mat4 pet = m;
        pet = translate(pet, vec3(0.0f, 0.028f, 0.0f));
        pet = rotate(pet, angle, vec3(0.0f, 1.0f, 0.0f));
        pet = rotate(pet, radians(24.0f), vec3(1.0f, 0.0f, 0.0f)); // cupping inward
        pet = scale(pet, vec3(0.032f, 0.14f, 0.018f));
        Primitives::drawCone(shader, pet, petalWhite);
    }

    // 5. Golden Stamen Bowl (Keshor / কেশর)
    mat4 center = m;
    center = translate(center, vec3(0.0f, 0.045f, 0.0f));
    center = scale(center, vec3(0.048f, 0.035f, 0.048f));
    Primitives::drawSphere(shader, center, flowerHeart);

    for (int k = 0; k < 6; k++) {
        float ka = (float)k * (2.0f * PI / 6.0f);
        mat4 stamen = m;
        stamen = translate(stamen, vec3(cosf(ka) * 0.022f, 0.055f, sinf(ka) * 0.022f));
        stamen = scale(stamen, vec3(0.012f, 0.022f, 0.012f));
        Primitives::drawSphere(shader, stamen, flowerHeart * 1.08f);
    }
}

// Draw a cluster of riverbank reeds / Kashbon (Catkins / কাশফুল)
void drawKashbonCluster(Shader& shader, const mat4& model, const vec3& pos, int count, float seed)
{
    shader.setInt("uUseTexture", 0); // silky white catkin plumes and green reed stems

    vec3 stalkColor (0.34f, 0.50f, 0.22f); // natural green-buff reed stalk
    vec3 stalkDry   (0.48f, 0.54f, 0.28f); // golden reed sheath
    vec3 plumeWhite (0.96f, 0.96f, 0.92f); // silky white catkin plume
    vec3 plumeBase  (0.88f, 0.88f, 0.82f); // softer silvery base

    for (int i = 0; i < count; i++) {
        float fi = (float)i;
        float ox = sinf(seed + fi * 1.7f) * 0.45f;
        float oz = cosf(seed + fi * 2.3f) * 0.45f;
        float h  = 1.3f + sinf(seed * 2.0f + fi) * 0.35f;
        float tiltX = sinf(seed + fi) * 12.0f;
        float tiltZ = cosf(seed + fi) * 12.0f;

        mat4 stalk = model;
        stalk = translate(stalk, pos + vec3(ox, 0.0f, oz));
        stalk = rotate(stalk, radians(tiltX), vec3(1.0f, 0.0f, 0.0f));
        stalk = rotate(stalk, radians(tiltZ), vec3(0.0f, 0.0f, 1.0f));

        // Lower green stalk
        mat4 sM = stalk;
        sM = translate(sM, vec3(0.0f, h * 0.40f, 0.0f));
        sM = scale(sM, vec3(0.018f, h * 0.80f, 0.018f));
        Primitives::drawCylinder(shader, sM, (i % 2 == 0) ? stalkColor : stalkDry);

        // Slender drooping reed blade leaf
        mat4 leaf = stalk;
        leaf = translate(leaf, vec3(0.0f, h * 0.35f, 0.03f));
        leaf = rotate(leaf, radians(32.0f), vec3(1.0f, 0.0f, 0.0f));
        leaf = scale(leaf, vec3(0.018f, h * 0.45f, 0.005f));
        Primitives::drawCone(shader, leaf, stalkColor);

        // Silky feathery catkin plume (main fluffy spindle)
        mat4 pM = stalk;
        pM = translate(pM, vec3(0.0f, h * 0.88f, 0.0f));
        pM = scale(pM, vec3(0.032f, h * 0.35f, 0.032f));
        Primitives::drawCylinder(shader, pM, plumeBase);

        // Soft feathery plume tip
        mat4 pTip = stalk;
        pTip = translate(pTip, vec3(0.0f, h * 1.05f, 0.0f));
        pTip = scale(pTip, vec3(0.038f, 0.08f, 0.038f));
        Primitives::drawSphere(shader, pTip, plumeWhite);

        // Soft radiating wisps
        for (int w = 0; w < 3; w++) {
            float wa = (float)w * (2.0f * PI / 3.0f);
            mat4 wisp = stalk;
            wisp = translate(wisp, vec3(cosf(wa) * 0.018f, h * 0.95f, sinf(wa) * 0.018f));
            wisp = rotate(wisp, radians(18.0f), vec3(cosf(wa), 0.0f, sinf(wa)));
            wisp = scale(wisp, vec3(0.020f, 0.16f, 0.020f));
            Primitives::drawCone(shader, wisp, plumeWhite);
        }
    }
}

// Draw a traditional rural bamboo mooring stake (Khuti / খুঁটি) driven into riverbank
void drawMooringStake(Shader& shader, const mat4& model, const vec3& rootPos)
{
    shader.setInt("uUseTexture", 0); // authentic bamboo culm and coir rope

    vec3 stakeColor(0.42f, 0.30f, 0.16f); // seasoned dark bamboo stake
    vec3 nodeColor (0.30f, 0.20f, 0.10f); // bamboo nodal joint rings
    vec3 ropeColor (0.68f, 0.58f, 0.36f); // natural golden coir/jute rope

    mat4 m = model;
    m = translate(m, rootPos);

    // Bamboo mooring stake angled firmly into riverbank mud
    mat4 stake = m;
    stake = translate(stake, vec3(0.0f, 0.38f, 0.0f));
    stake = rotate(stake, radians(-10.0f), vec3(0.0f, 0.0f, 1.0f));

    mat4 stakeCyl = scale(stake, vec3(0.045f, 0.85f, 0.045f));
    Primitives::drawCylinder(shader, stakeCyl, stakeColor);

    // Bamboo nodes along the stake
    for (int nd = 0; nd < 2; nd++) {
        mat4 node = stake;
        node = translate(node, vec3(0.0f, (float)nd * 0.30f - 0.15f, 0.0f));
        node = scale(node, vec3(0.052f, 0.022f, 0.052f));
        Primitives::drawCylinder(shader, node, nodeColor);
    }

    // Multiple coiled rope turns (Kachhi / Pat-er Dori) wrapped around stake
    for (int t = 0; t < 3; t++) {
        mat4 rope = stake;
        rope = translate(rope, vec3(0.0f, 0.18f + (float)t * 0.045f, 0.0f));
        rope = scale(rope, vec3(0.072f, 0.040f, 0.072f));
        Primitives::drawCylinder(shader, rope, (t % 2 == 0) ? ropeColor : (ropeColor * 0.90f));
    }

    // Trailing mooring painter rope end
    mat4 trail = stake;
    trail = translate(trail, vec3(0.08f, 0.16f, 0.10f));
    trail = rotate(trail, radians(45.0f), vec3(1.0f, 0.0f, 0.0f));
    trail = scale(trail, vec3(0.025f, 0.22f, 0.025f));
    Primitives::drawCylinder(shader, trail, ropeColor);
}

void draw(Shader& shader, const mat4& model, float time)
{
    // ── River Palette Matching Folk Artwork ──────────────────────
    vec3 riverWaterCol(0.30f, 0.65f, 0.88f); // Vibrant tranquil sky-river blue
    vec3 shoreSandCol (0.90f, 0.82f, 0.65f); // Smooth creamy-beige sandy beach strip (Char)
    vec3 bankGrassCol (0.35f, 0.54f, 0.22f); // Lush green meadow transition
    vec3 rippleCol    (0.85f, 0.94f, 0.98f); // Stylized crisp white / cyan wave ripples

    const float riverWidth = 9.8f;
    const float halfWidth  = riverWidth * 0.5f;
    const int   numSegments = 38;
    const float zStart = -45.0f;
    const float zEnd   =  45.0f;
    const float stepZ  = (zEnd - zStart) / numSegments;

    // ── 1. Meandering River Water Body & Smooth Sandy Beach Strips ─
    for (int i = 0; i < numSegments; i++) {
        float z0 = zStart + i * stepZ;
        float z1 = z0 + stepZ;
        float zMid = (z0 + z1) * 0.5f;

        float xCenter = riverCenterline(zMid);
        float angle   = riverTangentAngle(zMid);

        mat4 segM = model;
        segM = translate(segM, vec3(xCenter, 0.02f, zMid));
        segM = rotate(segM, angle, vec3(0.0f, 1.0f, 0.0f));

        // Full-width continuous river water surface with gentle specular shimmer
        shader.setFloat("shininess", 64.0f);
        shader.setFloat("specularStrength", 0.65f);
        mat4 water = segM;
        water = scale(water, vec3(riverWidth, 1.0f, stepZ * 1.08f));
        Primitives::drawPlane(shader, water, riverWaterCol);
        shader.setFloat("shininess", 32.0f);
        shader.setFloat("specularStrength", 0.35f);

        // Near (West) Smooth Sandy Beach Strip (Char / বালুচর)
        mat4 leftBeach = segM;
        leftBeach = translate(leftBeach, vec3(-halfWidth - 0.85f, 0.035f, 0.0f));
        leftBeach = rotate(leftBeach, radians(10.0f), vec3(0.0f, 0.0f, 1.0f));
        leftBeach = scale(leftBeach, vec3(1.85f, 1.0f, stepZ * 1.08f));
        Primitives::drawPlane(shader, leftBeach, shoreSandCol);

        // Far (East) Smooth Sandy Beach Strip (Char / বালুচর)
        mat4 rightBeach = segM;
        rightBeach = translate(rightBeach, vec3(halfWidth + 0.95f, 0.035f, 0.0f));
        rightBeach = rotate(rightBeach, radians(-10.0f), vec3(0.0f, 0.0f, 1.0f));
        rightBeach = scale(rightBeach, vec3(2.10f, 1.0f, stepZ * 1.08f));
        Primitives::drawPlane(shader, rightBeach, shoreSandCol);
    }

    // ── 2. Stylized Horizontal Water Ripple Lines (Matching Artwork) ─
    const int numRipples = 38;
    for (int r = 0; r < numRipples; ++r) {
        float rz = -41.0f + (float)r * 2.20f + sinf((float)r * 4.3f) * 0.4f;
        float rx = riverCenterline(rz) + sinf((float)r * 3.1f + time * 0.4f) * 2.6f;
        float rLen = 0.85f + fabsf(sinf((float)r * 5.2f)) * 1.35f;
        float flowOffset = fmodf(time * 0.35f + (float)r * 0.6f, 1.0f) * 0.3f;

        mat4 ripM = model;
        ripM = translate(ripM, vec3(rx, 0.026f, rz + flowOffset));
        ripM = scale(ripM, vec3(rLen, 1.0f, 0.036f));
        Primitives::drawPlane(shader, ripM, rippleCol);
    }

    // ── 3. Swimmers / Bathing Villagers in River Shallows ────────
    mat4 swimmerGroup = model;
    swimmerGroup = translate(swimmerGroup, vec3(riverCenterline(-1.0f) - 0.5f, 0.0f, -1.0f));
    drawSwimmers(shader, swimmerGroup, time);

    // ── 4. River Mooring Stakes on Sandy Shore ──────────────────
    float stakeZ1 = 1.2f;
    float stakeX1 = riverCenterline(stakeZ1) - halfWidth - 0.1f;
    drawMooringStake(shader, model, vec3(stakeX1, 0.04f, stakeZ1));

    float stakeZ2 = -9.5f;
    float stakeX2 = riverCenterline(stakeZ2) - halfWidth - 0.1f;
    drawMooringStake(shader, model, vec3(stakeX2, 0.04f, stakeZ2));

    // ── 5. Water Lilies (Shapla) Floating in River Shallows ──────
    const float shaplaZ[] = { -32.0f, -22.5f, -12.5f, -7.2f, -3.8f, 4.5f, 9.2f, 15.5f, 24.5f, 34.0f };
    const float shaplaScale[] = { 0.90f, 0.95f, 0.95f, 0.85f, 1.05f, 0.90f, 1.0f, 0.88f, 0.92f, 0.88f };
    for (int s = 0; s < 10; s++) {
        float sz = shaplaZ[s];
        float sx = riverCenterline(sz) - 3.2f + sinf((float)s * 1.7f) * 0.4f;
        drawShapla(shader, model, vec3(sx, 0.028f, sz), shaplaScale[s]);
    }

    // ── 6. Riverbank Catkin Reeds (Kashbon with Fluffy White Plumes) ─
    const float kashbonZ[] = { -36.0f, -26.0f, -16.0f, -10.5f, -5.5f, -2.0f, 6.5f, 12.0f, 18.5f, 27.0f, 36.0f };
    for (int k = 0; k < 11; k++) {
        float rz = kashbonZ[k];
        float rx = riverCenterline(rz) - halfWidth - 1.6f + sinf((float)k * 2.1f) * 0.3f;
        int count = 6 + (k % 3) * 2;
        drawKashbonCluster(shader, model, vec3(rx, 0.0f, rz), count, (float)k * 3.7f);
    }
}

// ── Swimmers / Bathing Villagers in River Shallows ───────────────
void drawSwimmers(Shader& shader, const mat4& model, float animTime)
{
    shader.setInt("uUseTexture", 0);

    vec3 skinCol  (0.54f, 0.36f, 0.22f); // Warm sun-tanned skin
    vec3 hairCol  (0.12f, 0.10f, 0.08f); // Dark hair
    vec3 rippleCol(0.85f, 0.94f, 0.98f); // Water foam ripple ring

    // 4 swimmers arranged in a friendly group in river shallows between boats
    struct SwimmerData {
        float x, z;
        float phase;
    };
    const SwimmerData swimmers[4] = {
        { -0.85f, -1.2f, 0.0f },
        { -0.15f, -0.6f, 1.4f },
        {  0.45f,  0.3f, 2.8f },
        {  1.15f,  0.8f, 4.2f }
    };

    for (int i = 0; i < 4; ++i) {
        float bob = (animTime > 0.0f) ? (sinf(animTime * 2.4f + swimmers[i].phase) * 0.016f) : 0.0f;
        float y = 0.035f + bob;

        mat4 sm = model;
        sm = translate(sm, vec3(swimmers[i].x, y, swimmers[i].z));

        // Circular water ripple ring around swimmer's neck
        float rippleScale = 1.0f + (animTime > 0.0f ? fmodf(animTime * 0.8f + (float)i * 0.5f, 1.0f) * 0.3f : 0.0f);
        mat4 rip = sm;
        rip = translate(rip, vec3(0.0f, -0.012f, 0.0f));
        rip = scale(rip, vec3(0.38f * rippleScale, 0.005f, 0.38f * rippleScale));
        Primitives::drawCylinder(shader, rip, rippleCol);

        // Head sphere
        mat4 head = sm;
        head = translate(head, vec3(0.0f, 0.12f, 0.0f));
        head = scale(head, vec3(0.18f, 0.19f, 0.18f));
        Primitives::drawSphere(shader, head, skinCol);

        // Dark hair cap
        mat4 hair = sm;
        hair = translate(hair, vec3(0.0f, 0.16f, -0.02f));
        hair = scale(hair, vec3(0.185f, 0.12f, 0.185f));
        Primitives::drawSphere(shader, hair, hairCol);

        // Wet shoulder tops just breaking water surface
        mat4 shoulders = sm;
        shoulders = translate(shoulders, vec3(0.0f, 0.01f, 0.0f));
        shoulders = scale(shoulders, vec3(0.36f, 0.06f, 0.20f));
        Primitives::drawCube(shader, shoulders, skinCol);
    }
}

// ── Distant Village Huts & Straw Stacks on Opposite Shore ────────
void drawFarBankVillage(Shader& shader, const mat4& model)
{
    shader.setInt("uUseTexture", 0);

    vec3 wallCol  (0.88f, 0.84f, 0.76f); // Whitewashed / light clay walls
    vec3 roofCol  (0.78f, 0.38f, 0.18f); // Warm terracotta red-brown hip roofs
    vec3 strawCol (0.76f, 0.62f, 0.28f); // Golden rice straw stack
    vec3 poleCol  (0.45f, 0.35f, 0.20f); // Bamboo center pole

    struct FarHut {
        float x, z;
        float angle;
        float scaleVal;
    };
    const FarHut huts[6] = {
        { 6.8f, -22.0f,  12.0f, 0.85f },
        { 7.5f, -16.0f,  -8.0f, 0.90f },
        { 7.8f,  -7.5f,  15.0f, 0.85f },
        { 8.1f,   2.5f,  -5.0f, 0.92f },
        { 7.7f,  12.0f,  10.0f, 0.88f },
        { 7.1f,  20.0f, -15.0f, 0.85f }
    };

    for (int h = 0; h < 6; ++h) {
        float x = huts[h].x;
        float z = huts[h].z;
        float s = huts[h].scaleVal;

        mat4 hm = model;
        hm = translate(hm, vec3(x, 0.06f, z));
        hm = rotate(hm, radians(huts[h].angle), vec3(0.0f, 1.0f, 0.0f));
        hm = scale(hm, vec3(s));

        // Plinth
        mat4 plinth = hm;
        plinth = translate(plinth, vec3(0.0f, 0.06f, 0.0f));
        plinth = scale(plinth, vec3(1.9f, 0.12f, 1.5f));
        Primitives::drawCube(shader, plinth, wallCol * 0.85f);

        // Walls
        mat4 wall = hm;
        wall = translate(wall, vec3(0.0f, 0.45f, 0.0f));
        wall = scale(wall, vec3(1.6f, 0.70f, 1.2f));
        Primitives::drawCube(shader, wall, wallCol);

        // Terracotta Chouchala 4-sloped Hip Roof
        mat4 roof = hm;
        roof = translate(roof, vec3(0.0f, 0.80f, 0.0f));
        roof = scale(roof, vec3(2.1f, 0.75f, 1.6f));
        Primitives::drawPyramid(shader, roof, roofCol);

        // Small door
        mat4 door = hm;
        door = translate(door, vec3(-0.81f, 0.35f, 0.0f));
        door = scale(door, vec3(0.02f, 0.45f, 0.35f));
        Primitives::drawCube(shader, door, vec3(0.28f, 0.16f, 0.08f));
    }

    // Conical Straw Stacks (Khorer Paloi) on far bank
    float strawSpots[2][2] = { { 7.8f, -11.5f }, { 8.2f, 7.5f } };
    for (int sp = 0; sp < 2; ++sp) {
        mat4 sm = model;
        sm = translate(sm, vec3(strawSpots[sp][0], 0.06f, strawSpots[sp][1]));

        // Conical straw body
        mat4 cone = sm;
        cone = scale(cone, vec3(1.2f, 1.4f, 1.2f));
        Primitives::drawCone(shader, cone, strawCol);

        // Central bamboo pole
        mat4 pole = sm;
        pole = translate(pole, vec3(0.0f, 0.85f, 0.0f));
        pole = scale(pole, vec3(0.04f, 1.8f, 0.04f));
        Primitives::drawCylinder(shader, pole, poleCol);
    }
}

// ── 5. Standalone Fishing Net Drying Rack (Jal Shukanor Macha) ────
void drawNetRack(Shader& shader, const mat4& model, const vec3& pos)
{
    vec3 bambooCol (0.42f, 0.30f, 0.14f); // bamboo frame
    vec3 nodeCol   (0.28f, 0.20f, 0.08f); // annular joint rings
    vec3 netMeshCol(0.18f, 0.24f, 0.28f); // dark braided nylon-jute fishing net
    vec3 logiCol   (0.55f, 0.42f, 0.22f); // bamboo boat push pole

    mat4 m = model;
    m = translate(m, pos);

    float rackL = 4.2f;
    float rackH = 2.1f;

    // Two vertical bamboo stilt poles driven into riverbank sand
    for (int side = -1; side <= 1; side += 2) {
        mat4 stilt = m;
        stilt = translate(stilt, vec3(side * (rackL * 0.45f), rackH * 0.5f, 0.0f));
        mat4 stiltCyl = scale(stilt, vec3(0.055f, rackH, 0.055f));
        Primitives::drawCylinder(shader, stiltCyl, bambooCol);

        // Bamboo nodes
        for (int nd = 1; nd <= 3; nd++) {
            mat4 node = stilt;
            node = translate(node, vec3(0.0f, (float)nd * 0.5f - rackH * 0.5f, 0.0f));
            node = scale(node, vec3(0.065f, 0.025f, 0.065f));
            Primitives::drawCylinder(shader, node, nodeCol);
        }
    }

    // Long horizontal bamboo ridge pole
    mat4 ridge = m;
    ridge = translate(ridge, vec3(0.0f, rackH, 0.0f));
    ridge = rotate(ridge, radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
    ridge = scale(ridge, vec3(0.045f, rackL, 0.045f));
    Primitives::drawCylinder(shader, ridge, bambooCol);

    // Draped fishing net mesh (hanging down in soft folds)
    mat4 netMesh = m;
    netMesh = translate(netMesh, vec3(0.0f, rackH * 0.52f, 0.0f));
    netMesh = scale(netMesh, vec3(rackL * 0.88f, rackH * 0.85f, 0.02f));
    Primitives::drawCube(shader, netMesh, netMeshCol);

    // Scalloped bottom fringe / weights
    mat4 fringe = m;
    fringe = translate(fringe, vec3(0.0f, 0.12f, 0.0f));
    fringe = scale(fringe, vec3(rackL * 0.88f, 0.06f, 0.04f));
    Primitives::drawCube(shader, fringe, nodeCol);

    // Bamboo boat push-pole (Logi) propped against the rack
    mat4 logi = m;
    logi = translate(logi, vec3(rackL * 0.38f, rackH * 0.55f, 0.28f));
    logi = rotate(logi, radians(-16.0f), vec3(0.0f, 0.0f, 1.0f));
    logi = rotate(logi, radians(22.0f), vec3(1.0f, 0.0f, 0.0f));
    mat4 logiS = scale(logi, vec3(0.035f, rackH * 1.35f, 0.035f));
    Primitives::drawCylinder(shader, logiS, logiCol);
}

} // namespace River
