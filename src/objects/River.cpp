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

    // ── 3. Swimmers / Bathing Villagers at River Kinara & Landing Ghat Shallows ──
    drawSwimmers(shader, model, time);

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

// ── Swimmers / Bathing Villagers at Wooden Ghat Par (ঘাটের পাড়ে বিভিন্ন বয়সের স্নানরত গ্রামবাসী) ──
void drawSwimmers(Shader& shader, const mat4& model, float animTime)
{
    shader.setInt("uUseTexture", 0);

    // Optimized static palette: avoid per-frame vector allocations
    static const vec3 foamCol    (0.80f, 0.92f, 0.97f); // Translucent water ripple foam
    static const vec3 splashDrop (0.92f, 0.97f, 1.00f); // Frothy water splash droplets
    static const vec3 childSkin  (0.58f, 0.40f, 0.26f); // Warm youthful child skin
    static const vec3 boyHair    (0.07f, 0.05f, 0.04f); // Jet black child cropped hair
    static const vec3 youthSkin  (0.52f, 0.35f, 0.21f); // Energetic teenage skin
    static const vec3 youthHair  (0.06f, 0.05f, 0.04f); // Thick dark wet youth hair
    static const vec3 adultSkin  (0.46f, 0.28f, 0.16f); // Deep sun-baked mature farmer skin
    static const vec3 adultHair  (0.05f, 0.04f, 0.04f); // Jet black mature hair & mustache
    static const vec3 gamchaRed  (0.85f, 0.22f, 0.16f); // Traditional crimson cotton Gamcha
    static const vec3 gamchaTrim (0.95f, 0.95f, 0.92f); // White woven Gamcha fringe
    static const vec3 elderSkin  (0.50f, 0.32f, 0.20f); // Distinguished elder skin tone
    static const vec3 whiteHair  (0.88f, 0.88f, 0.90f); // Silvery-white elder hair cap
    static const vec3 whiteBeard (0.92f, 0.92f, 0.94f); // Flowing white elder beard & mustache

    const bool hasAnim = (animTime > 0.0f);

    // ── 1. Young Child / Boy (ছোট ছেলে - ৬-৮ বছর) ───────────────────
    // Splashing playfully in the shallowest water right by the sandy wooden ghat bank
    {
        float bob = hasAnim ? (sinf(animTime * 3.4f) * 0.012f) : 0.0f;
        mat4 sm = model;
        sm = translate(sm, vec3(-3.65f, 0.032f + bob, -4.80f));
        sm = rotate(sm, radians(20.0f), vec3(0.0f, 1.0f, 0.0f));

        // Water ripple ring
        float ripScale = 1.0f + (hasAnim ? fmodf(animTime * 1.1f, 1.0f) * 0.32f : 0.0f);
        mat4 rip = sm;
        rip = translate(rip, vec3(0.0f, -0.012f, 0.0f));
        rip = scale(rip, vec3(0.24f * ripScale, 0.004f, 0.24f * ripScale));
        Primitives::drawCylinder(shader, rip, foamCol);

        // Child head (smaller scale, warm glowing skin)
        mat4 head = sm;
        head = translate(head, vec3(0.0f, 0.090f, 0.0f));
        head = scale(head, vec3(0.115f, 0.120f, 0.115f));
        Primitives::drawSphere(shader, head, childSkin);

        // Child Hair Cap (prominently sits on crown, rising higher than skull)
        mat4 hairCap = sm;
        hairCap = translate(hairCap, vec3(0.0f, 0.135f, -0.012f));
        hairCap = scale(hairCap, vec3(0.122f, 0.090f, 0.122f));
        Primitives::drawSphere(shader, hairCap, boyHair);

        // Front child bangs (Kopal-er chul)
        mat4 bangs = sm;
        bangs = translate(bangs, vec3(0.0f, 0.125f, 0.070f));
        bangs = scale(bangs, vec3(0.080f, 0.045f, 0.055f));
        Primitives::drawSphere(shader, bangs, boyHair);

        // Side/nape hair volume
        mat4 nape = sm;
        nape = translate(nape, vec3(0.0f, 0.095f, -0.045f));
        nape = scale(nape, vec3(0.124f, 0.065f, 0.090f));
        Primitives::drawSphere(shader, nape, boyHair);

        // Slender child shoulders & chest
        mat4 shoulders = sm;
        shoulders = translate(shoulders, vec3(0.0f, 0.012f, 0.0f));
        shoulders = scale(shoulders, vec3(0.22f, 0.042f, 0.12f));
        Primitives::drawCube(shader, shoulders, childSkin);

        // Playful splashing arms
        float splash1 = hasAnim ? sinf(animTime * 4.6f) * 0.035f : 0.0f;
        mat4 armR = sm;
        armR = translate(armR, vec3(0.10f, 0.04f + splash1, 0.06f));
        armR = scale(armR, vec3(0.038f, 0.038f, 0.085f));
        Primitives::drawSphere(shader, armR, childSkin);

        mat4 armL = sm;
        armL = translate(armL, vec3(-0.10f, 0.02f, 0.04f));
        armL = scale(armL, vec3(0.035f, 0.035f, 0.075f));
        Primitives::drawSphere(shader, armL, childSkin);

        // Animated flying splash droplets
        mat4 drop1 = sm;
        drop1 = translate(drop1, vec3(0.13f, 0.09f + splash1 * 1.5f, 0.12f));
        drop1 = scale(drop1, vec3(0.014f));
        Primitives::drawSphere(shader, drop1, splashDrop);

        mat4 drop2 = sm;
        drop2 = translate(drop2, vec3(0.08f, 0.11f + splash1 * 1.2f, 0.15f));
        drop2 = scale(drop2, vec3(0.011f));
        Primitives::drawSphere(shader, drop2, splashDrop);
    }

    // ── 2. Teenager / Youth (কিশোর যুবক - ১৫-১৮ বছর) ──────────────────
    // Swimming actively in the clear shallows along the wooden ghat bank
    {
        float bob = hasAnim ? (sinf(animTime * 2.5f + 1.2f) * 0.014f) : 0.0f;
        mat4 sm = model;
        sm = translate(sm, vec3(-3.35f, 0.035f + bob, -3.35f));
        sm = rotate(sm, radians(25.0f), vec3(0.0f, 1.0f, 0.0f));

        // Water ripple ring
        float ripScale = 1.0f + (hasAnim ? fmodf(animTime * 0.95f + 0.4f, 1.0f) * 0.28f : 0.0f);
        mat4 rip = sm;
        rip = translate(rip, vec3(0.0f, -0.012f, 0.0f));
        rip = scale(rip, vec3(0.31f * ripScale, 0.005f, 0.31f * ripScale));
        Primitives::drawCylinder(shader, rip, foamCol);

        // Youth head
        mat4 head = sm;
        head = translate(head, vec3(0.0f, 0.110f, 0.0f));
        head = scale(head, vec3(0.145f, 0.155f, 0.145f));
        Primitives::drawSphere(shader, head, youthSkin);

        // Full head of thick dark wet hair (rising well above skull top)
        mat4 hairCap = sm;
        hairCap = translate(hairCap, vec3(0.0f, 0.170f, -0.015f));
        hairCap = scale(hairCap, vec3(0.152f, 0.115f, 0.152f));
        Primitives::drawSphere(shader, hairCap, youthHair);

        // Side-parted front fringe
        mat4 fringe = sm;
        fringe = translate(fringe, vec3(0.02f, 0.160f, 0.085f));
        fringe = scale(fringe, vec3(0.110f, 0.055f, 0.070f));
        Primitives::drawSphere(shader, fringe, youthHair);

        // Sideburns and nape volume
        mat4 sides = sm;
        sides = translate(sides, vec3(0.0f, 0.120f, -0.050f));
        sides = scale(sides, vec3(0.155f, 0.075f, 0.110f));
        Primitives::drawSphere(shader, sides, youthHair);

        // Athletic shoulders & chest
        mat4 shoulders = sm;
        shoulders = translate(shoulders, vec3(0.0f, 0.012f, 0.0f));
        shoulders = scale(shoulders, vec3(0.31f, 0.052f, 0.17f));
        Primitives::drawCube(shader, shoulders, youthSkin);

        // Washing motion: arm raised rubbing wet hair/neck
        float washMove = hasAnim ? sinf(animTime * 3.0f) * 0.02f : 0.0f;
        mat4 arm = sm;
        arm = translate(arm, vec3(0.11f, 0.09f + washMove, 0.02f));
        arm = rotate(arm, radians(-35.0f), vec3(0.0f, 0.0f, 1.0f));
        arm = scale(arm, vec3(0.045f, 0.10f, 0.045f));
        Primitives::drawSphere(shader, arm, youthSkin);
    }

    // ── 3. Adult Villager with Traditional Gamcha (প্রাপ্তবয়স্ক গেরস্থ কৃষক - ৩৫-৪৫ বছর) ─
    // Dipping along the wooden ghat bank with mustache and traditional red Gamcha (লাল গামছা)
    {
        float bob = hasAnim ? (sinf(animTime * 1.9f + 2.5f) * 0.014f) : 0.0f;
        mat4 sm = model;
        sm = translate(sm, vec3(-3.10f, 0.038f + bob, -1.90f));
        sm = rotate(sm, radians(18.0f), vec3(0.0f, 1.0f, 0.0f));

        // Water ripple ring
        float ripScale = 1.0f + (hasAnim ? fmodf(animTime * 0.78f + 0.8f, 1.0f) * 0.25f : 0.0f);
        mat4 rip = sm;
        rip = translate(rip, vec3(0.0f, -0.012f, 0.0f));
        rip = scale(rip, vec3(0.36f * ripScale, 0.005f, 0.36f * ripScale));
        Primitives::drawCylinder(shader, rip, foamCol);

        // Adult head
        mat4 head = sm;
        head = translate(head, vec3(0.0f, 0.120f, 0.0f));
        head = scale(head, vec3(0.165f, 0.175f, 0.165f));
        Primitives::drawSphere(shader, head, adultSkin);

        // Mature adult hair cap (crown sits above skull)
        mat4 hairCap = sm;
        hairCap = translate(hairCap, vec3(0.0f, 0.180f, -0.020f));
        hairCap = scale(hairCap, vec3(0.172f, 0.130f, 0.172f));
        Primitives::drawSphere(shader, hairCap, adultHair);

        // Forehead hairline & crown part
        mat4 hairFront = sm;
        hairFront = translate(hairFront, vec3(-0.02f, 0.175f, 0.080f));
        hairFront = scale(hairFront, vec3(0.120f, 0.060f, 0.075f));
        Primitives::drawSphere(shader, hairFront, adultHair);

        // Back/nape volume
        mat4 hairBack = sm;
        hairBack = translate(hairBack, vec3(0.0f, 0.125f, -0.055f));
        hairBack = scale(hairBack, vec3(0.174f, 0.085f, 0.120f));
        Primitives::drawSphere(shader, hairBack, adultHair);

        // Distinct adult mustache (গোঁফ) on upper lip
        mat4 mustache = sm;
        mustache = translate(mustache, vec3(0.0f, 0.078f, 0.150f));
        mustache = scale(mustache, vec3(0.078f, 0.022f, 0.035f));
        Primitives::drawSphere(shader, mustache, adultHair);

        // Broad muscular farmer shoulders
        mat4 shoulders = sm;
        shoulders = translate(shoulders, vec3(0.0f, 0.012f, 0.0f));
        shoulders = scale(shoulders, vec3(0.38f, 0.062f, 0.20f));
        Primitives::drawCube(shader, shoulders, adultSkin);

        // Traditional Red Gamcha (লাল গামছা) draped over shoulder & chest
        mat4 gamchaBody = sm;
        gamchaBody = translate(gamchaBody, vec3(0.10f, 0.045f, 0.02f));
        gamchaBody = scale(gamchaBody, vec3(0.13f, 0.050f, 0.22f));
        Primitives::drawCube(shader, gamchaBody, gamchaRed);

        // Woven white fringe on Gamcha
        mat4 gamchaFringe = sm;
        gamchaFringe = translate(gamchaFringe, vec3(0.10f, 0.050f, 0.125f));
        gamchaFringe = scale(gamchaFringe, vec3(0.125f, 0.018f, 0.025f));
        Primitives::drawCube(shader, gamchaFringe, gamchaTrim);
    }

    // ── 4. Village Elder with White Hair & Beard (বৃদ্ধ মুরুব্বি - ৬৫-৭৫ বছর) ───────
    // Respectable elder bathing serenely in calm water right by the wooden ghat landing
    {
        float bob = hasAnim ? (sinf(animTime * 1.5f + 3.8f) * 0.010f) : 0.0f;
        mat4 sm = model;
        sm = translate(sm, vec3(-2.85f, 0.036f + bob, -0.45f));
        sm = rotate(sm, radians(22.0f), vec3(0.0f, 1.0f, 0.0f));

        // Water ripple ring
        float ripScale = 1.0f + (hasAnim ? fmodf(animTime * 0.65f + 1.2f, 1.0f) * 0.22f : 0.0f);
        mat4 rip = sm;
        rip = translate(rip, vec3(0.0f, -0.012f, 0.0f));
        rip = scale(rip, vec3(0.34f * ripScale, 0.005f, 0.34f * ripScale));
        Primitives::drawCylinder(shader, rip, foamCol);

        // Elder head
        mat4 head = sm;
        head = translate(head, vec3(0.0f, 0.115f, 0.0f));
        head = scale(head, vec3(0.160f, 0.170f, 0.160f));
        Primitives::drawSphere(shader, head, elderSkin);

        // Silvery-white hair cap (paka chul - rising above skull)
        mat4 hairCap = sm;
        hairCap = translate(hairCap, vec3(0.0f, 0.175f, -0.022f));
        hairCap = scale(hairCap, vec3(0.168f, 0.125f, 0.168f));
        Primitives::drawSphere(shader, hairCap, whiteHair);

        // Silvery-white temple & side/back fringes
        mat4 hairSides = sm;
        hairSides = translate(hairSides, vec3(0.0f, 0.120f, -0.055f));
        hairSides = scale(hairSides, vec3(0.172f, 0.075f, 0.115f));
        Primitives::drawSphere(shader, hairSides, whiteHair);

        // White elder mustache
        mat4 mustache = sm;
        mustache = translate(mustache, vec3(0.0f, 0.078f, 0.145f));
        mustache = scale(mustache, vec3(0.072f, 0.020f, 0.030f));
        Primitives::drawSphere(shader, mustache, whiteHair);

        // Long flowing white elder beard (paka dari)
        mat4 beardUpper = sm;
        beardUpper = translate(beardUpper, vec3(0.0f, 0.052f, 0.115f));
        beardUpper = scale(beardUpper, vec3(0.072f, 0.055f, 0.065f));
        Primitives::drawSphere(shader, beardUpper, whiteBeard);

        mat4 beardTip = sm;
        beardTip = translate(beardTip, vec3(0.0f, 0.018f, 0.095f));
        beardTip = scale(beardTip, vec3(0.052f, 0.040f, 0.050f));
        Primitives::drawSphere(shader, beardTip, whiteBeard);

        // Elder shoulders
        mat4 shoulders = sm;
        shoulders = translate(shoulders, vec3(0.0f, 0.012f, 0.0f));
        shoulders = scale(shoulders, vec3(0.34f, 0.058f, 0.19f));
        Primitives::drawCube(shader, shoulders, elderSkin);

        // Folded hands in holy dip prayer at water surface
        mat4 hands = sm;
        hands = translate(hands, vec3(0.0f, 0.035f, 0.15f));
        hands = scale(hands, vec3(0.060f, 0.030f, 0.050f));
        Primitives::drawSphere(shader, hands, elderSkin);
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
