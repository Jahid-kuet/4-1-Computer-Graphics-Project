// House.cpp — Authentic Bangladeshi rural house (Mati-r Ghor / Tin-er Ghor)
// Featuring traditional Chouchala (4-sloped hip roof) and Dochala (2-sloped gable roof),
// raised earthen plinth (dawa/viti), bamboo verandah (baranda), wooden door and shutters,
// and terracotta water pitchers (matir kolshi).

#include "objects/House.h"
#include "objects/Person.h"
#include "Primitives.h"

using namespace math;

namespace House {

void drawKolshi(Shader& shader, const mat4& model, const vec3& pos, float scaleVal)
{
    shader.setInt("uUseTexture", 0);

    vec3 biraStraw (0.72f, 0.58f, 0.28f); // woven straw ring cushion (Bira)
    vec3 clayColor (0.72f, 0.36f, 0.18f); // rich warm terracotta
    vec3 darkClay  (0.58f, 0.28f, 0.14f); // kiln-fired neck groove & rim
    vec3 bandColor (0.84f, 0.46f, 0.22f); // folk decorative shoulder ring

    mat4 m = model;
    m = translate(m, pos);
    m = scale(m, vec3(scaleVal));

    // 1. Woven Straw Base Cushion (Bira / বিড়ে)
    mat4 bira = m;
    bira = translate(bira, vec3(0.0f, 0.025f, 0.0f));
    bira = scale(bira, vec3(0.18f, 0.045f, 0.18f));
    Primitives::drawCylinder(shader, bira, biraStraw);

    // 2. Spherical lower belly
    mat4 belly = m;
    belly = translate(belly, vec3(0.0f, 0.190f, 0.0f));
    belly = scale(belly, vec3(0.230f, 0.195f, 0.230f));
    Primitives::drawSphere(shader, belly, clayColor);

    // 3. Flared shoulder curve tapering into neck
    mat4 shoulder = m;
    shoulder = translate(shoulder, vec3(0.0f, 0.285f, 0.0f));
    shoulder = scale(shoulder, vec3(0.165f, 0.075f, 0.165f));
    Primitives::drawSphere(shader, shoulder, clayColor * 0.95f);

    // 4. Decorative Folk Ring on Shoulder (Patti)
    mat4 band = m;
    band = translate(band, vec3(0.0f, 0.290f, 0.0f));
    band = scale(band, vec3(0.168f, 0.012f, 0.168f));
    Primitives::drawCylinder(shader, band, bandColor);

    // 5. Slender turned neck (Gola)
    mat4 neck = m;
    neck = translate(neck, vec3(0.0f, 0.355f, 0.0f));
    neck = scale(neck, vec3(0.088f, 0.090f, 0.088f));
    Primitives::drawCylinder(shader, neck, darkClay);

    // 6. Turned Rolled Rim (Mukher Kan)
    mat4 rim = m;
    rim = translate(rim, vec3(0.0f, 0.410f, 0.0f));
    rim = scale(rim, vec3(0.142f, 0.032f, 0.142f));
    Primitives::drawCylinder(shader, rim, darkClay);

    // 7. Inner hollow pouring mouth
    mat4 inner = m;
    inner = translate(inner, vec3(0.0f, 0.422f, 0.0f));
    inner = scale(inner, vec3(0.092f, 0.012f, 0.092f));
    Primitives::drawCylinder(shader, inner, vec3(0.24f, 0.12f, 0.06f));
}

// Helper: Draw a 3-sided traditional woven bamboo windbreak fence (Bera) around the cooking stove
static void drawStoveFence(Shader& shader, const mat4& model)
{
    vec3 postCol  (0.36f, 0.26f, 0.12f); // dark weathered bamboo posts
    vec3 railCol  (0.48f, 0.38f, 0.20f); // split bamboo horizontal tie-rails
    vec3 picketCol(0.44f, 0.34f, 0.18f); // woven vertical bamboo slats

    float xMin = -0.55f, xMax = 0.55f;
    float zBack = -0.50f, zFront = 0.45f;
    float fenceH = 0.64f;
    float postH  = 0.70f;
    float postR  = 0.022f;

    // 1. Four corner/end upright bamboo posts (Back-L, Back-R, Front-L, Front-R)
    float posts[4][2] = {
        { xMin, zBack },
        { xMax, zBack },
        { xMin, zFront },
        { xMax, zFront }
    };

    for (int i = 0; i < 4; i++) {
        mat4 p = model;
        p = translate(p, vec3(posts[i][0], postH * 0.5f, posts[i][1]));
        p = scale(p, vec3(postR, postH, postR));
        Primitives::drawCylinder(shader, p, postCol);
    }

    // 2. Horizontal tie-rails (bottom rail at Y = 0.24, top rail at Y = 0.56) on 3 sides
    float railY[2] = { 0.24f, 0.56f };
    for (int r = 0; r < 2; r++) {
        float y = railY[r];

        // Back wall rail (along X)
        mat4 rBack = model;
        rBack = translate(rBack, vec3(0.0f, y, zBack));
        rBack = scale(rBack, vec3(xMax - xMin, 0.024f, 0.024f));
        Primitives::drawCube(shader, rBack, railCol);

        // Left wall rail (along Z)
        mat4 rLeft = model;
        rLeft = translate(rLeft, vec3(xMin, y, (zBack + zFront) * 0.5f));
        rLeft = scale(rLeft, vec3(0.024f, 0.024f, zFront - zBack));
        Primitives::drawCube(shader, rLeft, railCol);

        // Right wall rail (along Z)
        mat4 rRight = model;
        rRight = translate(rRight, vec3(xMax, y, (zBack + zFront) * 0.5f));
        rRight = scale(rRight, vec3(0.024f, 0.024f, zFront - zBack));
        Primitives::drawCube(shader, rRight, railCol);
    }

    // 3. Vertical bamboo slats/pickets on 3 sides (leaving front open for cook & firewood)
    // Back wall slats
    int backSlats = 7;
    for (int i = 0; i < backSlats; i++) {
        float t = (float)i / (backSlats - 1);
        float x = xMin + 0.06f + t * (xMax - xMin - 0.12f);
        mat4 slat = model;
        slat = translate(slat, vec3(x, fenceH * 0.5f, zBack));
        slat = scale(slat, vec3(0.038f, fenceH, 0.012f));
        Primitives::drawCube(shader, slat, picketCol);
    }

    // Left wall slats
    int sideSlats = 6;
    for (int i = 0; i < sideSlats; i++) {
        float t = (float)i / (sideSlats - 1);
        float z = zBack + 0.06f + t * (zFront - zBack - 0.12f);
        mat4 slat = model;
        slat = translate(slat, vec3(xMin, fenceH * 0.5f, z));
        slat = scale(slat, vec3(0.012f, fenceH, 0.038f));
        Primitives::drawCube(shader, slat, picketCol);
    }

    // Right wall slats
    for (int i = 0; i < sideSlats; i++) {
        float t = (float)i / (sideSlats - 1);
        float z = zBack + 0.06f + t * (zFront - zBack - 0.12f);
        mat4 slat = model;
        slat = translate(slat, vec3(xMax, fenceH * 0.5f, z));
        slat = scale(slat, vec3(0.012f, fenceH, 0.038f));
        Primitives::drawCube(shader, slat, picketCol);
    }
}

// Helper: Draw authentic, roaring rural wood fire (Chular Agun / জলন্ত আগুন)
static void drawStoveFire(Shader& shader, const mat4& cm, float animTime)
{
    shader.setInt("uUseTexture", 0);
    shader.setFloat("emissive", 1.0f); // Fire glows with full radiant brilliance day and night

    // ── Fiery Color Spectrum ─────────────────────────────────────
    vec3 flameWhite  (1.00f, 0.98f, 0.72f); // incandescent white-yellow flame core
    vec3 flameYellow (1.00f, 0.90f, 0.18f); // brilliant bright golden yellow
    vec3 flameAmber  (1.00f, 0.62f, 0.08f); // radiant warm amber
    vec3 flameOrange (1.00f, 0.38f, 0.04f); // fiery roaring orange body
    vec3 flameRed    (0.90f, 0.18f, 0.02f); // crimson/vermilion licking tips
    vec3 coalHot     (1.00f, 0.25f, 0.05f); // glowing red-hot burning charcoal embers
    vec3 coalDeep    (0.68f, 0.12f, 0.02f); // deep simmering coal base

    // Dynamic chaotic organic flicker factors (active when animTime > 0.0f)
    float t = animTime;
    float f1 = (animTime > 0.0f) ? (sinf(t * 18.0f) * 0.12f + cosf(t * 26.5f) * 0.08f) : 0.0f;
    float f2 = (animTime > 0.0f) ? (cosf(t * 21.0f) * 0.13f + sinf(t * 33.0f) * 0.07f) : 0.0f;
    float f3 = (animTime > 0.0f) ? (sinf(t * 15.5f + 1.2f) * 0.11f + cosf(t * 24.0f) * 0.08f) : 0.0f;
    float fH = (animTime > 0.0f) ? (sinf(t * 12.0f) * 0.015f + cosf(t * 19.0f) * 0.010f) : 0.0f;

    // ── A. Deep Glowing Ember Bed inside Combustion Firebox ───────
    float coalCoords[7][3] = {
        { -0.045f, 0.042f, 0.14f },
        {  0.035f, 0.045f, 0.15f },
        {  0.000f, 0.052f, 0.17f },
        { -0.025f, 0.048f, 0.19f },
        {  0.028f, 0.044f, 0.21f },
        { -0.050f, 0.038f, 0.22f },
        {  0.045f, 0.040f, 0.18f }
    };
    for (int c = 0; c < 7; c++) {
        mat4 coal = cm;
        coal = translate(coal, vec3(coalCoords[c][0], coalCoords[c][1], coalCoords[c][2]));
        coal = scale(coal, vec3(0.032f, 0.024f, 0.032f));
        vec3 cCol = (c % 2 == 0) ? coalHot : (c == 2 ? flameYellow : coalDeep);
        Primitives::drawSphere(shader, coal, cCol);
    }

    // Hot central firebed plane
    mat4 bed = cm;
    bed = translate(bed, vec3(0.0f, 0.050f, 0.17f));
    bed = scale(bed, vec3(0.12f, 0.035f, 0.11f));
    Primitives::drawCube(shader, bed, coalHot);

    // ── B. Roaring Flames Licking Out of Front Mouth (Chular Mukh) ──
    // 1. Central Dominant Flame Tongue
    mat4 fMain = cm;
    fMain = translate(fMain, vec3(0.0f, 0.080f + fH, 0.21f));
    fMain = rotate(fMain, radians(14.0f + f1 * 18.0f), vec3(1.0f, 0.0f, 0.0f));
    fMain = rotate(fMain, radians(f2 * 12.0f), vec3(0.0f, 0.0f, 1.0f));
    mat4 fMainOut = scale(fMain, vec3(0.055f * (1.0f + f1 * 0.3f), 0.155f * (1.0f + f2 * 0.25f), 0.042f));
    Primitives::drawCone(shader, fMainOut, flameOrange);

    mat4 fMainMid = scale(fMain, vec3(0.042f * (1.0f + f1 * 0.2f), 0.125f * (1.0f + f3 * 0.20f), 0.032f));
    Primitives::drawCone(shader, fMainMid, flameYellow);

    mat4 fMainCore = scale(fMain, vec3(0.024f, 0.085f, 0.020f));
    Primitives::drawCone(shader, fMainCore, flameWhite);

    // 2. Left Flame Tongue dancing over stick 1
    mat4 fLeft = cm;
    fLeft = translate(fLeft, vec3(-0.042f, 0.075f + fH, 0.23f));
    fLeft = rotate(fLeft, radians(18.0f + f2 * 16.0f), vec3(1.0f, 0.0f, 0.0f));
    fLeft = rotate(fLeft, radians(-15.0f + f1 * 10.0f), vec3(0.0f, 0.0f, 1.0f));
    mat4 fLeftOut = scale(fLeft, vec3(0.038f * (1.0f + f2 * 0.25f), 0.115f * (1.0f + f1 * 0.30f), 0.030f));
    Primitives::drawCone(shader, fLeftOut, flameOrange);
    mat4 fLeftIn = scale(fLeft, vec3(0.022f, 0.080f, 0.018f));
    Primitives::drawCone(shader, fLeftIn, flameYellow);

    // 3. Right Flame Tongue dancing over stick 2
    mat4 fRight = cm;
    fRight = translate(fRight, vec3(0.046f, 0.072f + fH, 0.235f));
    fRight = rotate(fRight, radians(16.0f + f3 * 15.0f), vec3(1.0f, 0.0f, 0.0f));
    fRight = rotate(fRight, radians(16.0f - f2 * 10.0f), vec3(0.0f, 0.0f, 1.0f));
    mat4 fRightOut = scale(fRight, vec3(0.036f * (1.0f + f3 * 0.25f), 0.105f * (1.0f + f2 * 0.30f), 0.028f));
    Primitives::drawCone(shader, fRightOut, flameRed);
    mat4 fRightIn = scale(fRight, vec3(0.020f, 0.070f, 0.016f));
    Primitives::drawCone(shader, fRightIn, flameYellow);

    // 4. Licking flame sleeves directly over burning firewood stick ends
    mat4 stickFlame1 = cm;
    stickFlame1 = translate(stickFlame1, vec3(-0.035f, 0.065f, 0.24f));
    stickFlame1 = scale(stickFlame1, vec3(0.036f, 0.045f, 0.065f));
    Primitives::drawSphere(shader, stickFlame1, flameAmber);

    mat4 stickFlame2 = cm;
    stickFlame2 = translate(stickFlame2, vec3(0.040f, 0.060f, 0.25f));
    stickFlame2 = scale(stickFlame2, vec3(0.034f, 0.042f, 0.060f));
    Primitives::drawSphere(shader, stickFlame2, flameAmber);

    // ── C. Flames Roaring Under and Licking Around Primary Cooking Pot ──
    mat4 underPot = cm;
    underPot = translate(underPot, vec3(0.0f, 0.245f + fH * 0.6f, 0.06f));
    underPot = scale(underPot, vec3(0.130f * (1.0f + f1 * 0.15f), 0.065f * (1.0f + f2 * 0.20f), 0.130f));
    Primitives::drawSphere(shader, underPot, flameYellow);

    mat4 underPotCore = cm;
    underPotCore = translate(underPotCore, vec3(0.0f, 0.240f, 0.06f));
    underPotCore = scale(underPotCore, vec3(0.085f, 0.045f, 0.085f));
    Primitives::drawSphere(shader, underPotCore, flameWhite);

    // 4 flame tongues licking up between the clay prongs around the pot belly
    float potTongues[4][4] = {
        {  50.0f, 0.125f, 0.090f, -22.0f },
        { 125.0f, 0.120f, 0.080f, -20.0f },
        { 210.0f, 0.125f, 0.085f, -22.0f },
        { 290.0f, 0.120f, 0.075f, -18.0f }
    };
    for (int pt = 0; pt < 4; pt++) {
        float pAng = radians(potTongues[pt][0]);
        float pRad = potTongues[pt][1];
        float pH   = potTongues[pt][2];
        float pTilt= potTongues[pt][3];
        float ptFlick = (pt % 2 == 0) ? f1 : f2;

        mat4 ptM = cm;
        ptM = translate(ptM, vec3(cosf(pAng) * pRad, 0.250f + fH * 0.5f, 0.06f + sinf(pAng) * pRad));
        ptM = rotate(ptM, -pAng + radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
        ptM = rotate(ptM, radians(pTilt + ptFlick * 12.0f), vec3(1.0f, 0.0f, 0.0f));

        mat4 ptOut = scale(ptM, vec3(0.024f, pH * (1.0f + ptFlick * 0.28f), 0.016f));
        Primitives::drawCone(shader, ptOut, (pt % 2 == 0) ? flameOrange : flameAmber);

        mat4 ptIn = scale(ptM, vec3(0.014f, pH * 0.65f, 0.010f));
        Primitives::drawCone(shader, ptIn, flameYellow);
    }

    // ── D. Gentle Simmer Fire under Rear Pot ─────────────────────
    mat4 rearFire = cm;
    rearFire = translate(rearFire, vec3(0.0f, 0.218f + fH * 0.4f, -0.22f));
    rearFire = scale(rearFire, vec3(0.080f * (1.0f + f3 * 0.15f), 0.040f, 0.080f));
    Primitives::drawSphere(shader, rearFire, flameAmber);

    mat4 rearFlame = cm;
    rearFlame = translate(rearFlame, vec3(0.025f, 0.225f + fH * 0.5f, -0.21f));
    rearFlame = rotate(rearFlame, radians(15.0f + f2 * 12.0f), vec3(0.0f, 0.0f, 1.0f));
    rearFlame = scale(rearFlame, vec3(0.020f, 0.055f * (1.0f + f1 * 0.20f), 0.015f));
    Primitives::drawCone(shader, rearFlame, flameYellow);

    // ── E. Rising Incandescent Fire Sparks (Agni-kulinga) ─────────
    float sparkCoords[5][3] = {
        { -0.025f, 0.220f, 0.25f },
        {  0.030f, 0.245f, 0.23f },
        {  0.005f, 0.270f, 0.20f },
        { -0.060f, 0.335f, 0.12f },
        {  0.075f, 0.320f, 0.08f }
    };
    for (int s = 0; s < 5; s++) {
        float sDriftY = (animTime > 0.0f) ? (fmodf(animTime * 0.25f + (float)s * 0.2f, 0.08f)) : 0.0f;
        float sJitterX = (animTime > 0.0f) ? (sinf(animTime * 14.0f + (float)s * 2.1f) * 0.006f) : 0.0f;
        mat4 spk = cm;
        spk = translate(spk, vec3(sparkCoords[s][0] + sJitterX, sparkCoords[s][1] + sDriftY, sparkCoords[s][2]));
        spk = scale(spk, vec3(0.007f, 0.007f, 0.007f));
        Primitives::drawSphere(shader, spk, (s % 2 == 0) ? flameWhite : flameYellow);
    }

    shader.setFloat("emissive", 0.0f); // Reset emissive state
}

void drawStove(Shader& shader, const mat4& model, bool withFence, float animTime)
{
    // 3-Sided traditional rural bamboo windbreak fence
    if (withFence) {
        drawStoveFence(shader, model);
    }

    shader.setInt("uUseTexture", 0);

    vec3 mudColor   (0.56f, 0.42f, 0.26f); // sun-baked river clay plaster
    vec3 sootColor  (0.22f, 0.16f, 0.12f); // dark soot/smoke rings around burner holes
    vec3 potColor   (0.72f, 0.38f, 0.20f); // authentic earthenware clay cooking pot (Hari)
    vec3 potSoot    (0.25f, 0.18f, 0.14f); // flame soot on lower pot belly
    vec3 pot2Color  (0.68f, 0.34f, 0.18f); // terracotta simmer pot
    vec3 woodColor  (0.42f, 0.28f, 0.14f); // split firewood sticks
    vec3 charColor  (0.12f, 0.10f, 0.08f); // charred firewood ends
    vec3 emberColor (1.00f, 0.42f, 0.10f); // glowing live embers inside firebox
    vec3 ashColor   (0.62f, 0.60f, 0.58f); // grey wood ash
    vec3 steelColor (0.82f, 0.85f, 0.88f); // bright polished steel blade (Boti)
    vec3 boardWood  (0.38f, 0.26f, 0.14f); // wooden footboard
    vec3 onionRed   (0.72f, 0.18f, 0.22f); // sliced red onion
    vec3 chiliGreen (0.20f, 0.60f, 0.18f); // green chili

    mat4 cm = model;

    // ── 1. Ash Bed & Floor Hearth Pad ────────────────────────────
    mat4 ashPad = cm;
    ashPad = translate(ashPad, vec3(0.0f, 0.015f, 0.08f));
    ashPad = scale(ashPad, vec3(0.55f, 0.030f, 0.68f));
    Primitives::drawCube(shader, ashPad, ashColor);

    // ── 2. Dual-Burner Hand-Sculpted Earthen Stove Body ──────────
    // Front burner mound (primary cooking pot)
    mat4 frontHearth = cm;
    frontHearth = translate(frontHearth, vec3(0.0f, 0.130f, 0.06f));
    frontHearth = scale(frontHearth, vec3(0.38f, 0.230f, 0.36f));
    Primitives::drawCylinder(shader, frontHearth, mudColor);

    // Rear burner mound (simmer pot)
    mat4 rearHearth = cm;
    rearHearth = translate(rearHearth, vec3(0.0f, 0.115f, -0.22f));
    rearHearth = scale(rearHearth, vec3(0.32f, 0.200f, 0.30f));
    Primitives::drawCylinder(shader, rearHearth, mudColor * 0.94f);

    // Connecting clay ridge between the two burners
    mat4 ridgeBridge = cm;
    ridgeBridge = translate(ridgeBridge, vec3(0.0f, 0.110f, -0.08f));
    ridgeBridge = scale(ridgeBridge, vec3(0.28f, 0.190f, 0.20f));
    Primitives::drawCube(shader, ridgeBridge, mudColor);

    // Soot/smoke halos around both burner rims
    mat4 soot1 = cm;
    soot1 = translate(soot1, vec3(0.0f, 0.246f, 0.06f));
    soot1 = scale(soot1, vec3(0.30f, 0.010f, 0.30f));
    Primitives::drawCylinder(shader, soot1, sootColor);

    mat4 soot2 = cm;
    soot2 = translate(soot2, vec3(0.0f, 0.216f, -0.22f));
    soot2 = scale(soot2, vec3(0.24f, 0.010f, 0.24f));
    Primitives::drawCylinder(shader, soot2, sootColor);

    // ── 3. Arched Combustion Fire Mouth (Chular Mukh) ────────────
    // Front dark opening tunnel
    mat4 mouth = cm;
    mouth = translate(mouth, vec3(0.0f, 0.085f, 0.22f));
    mouth = scale(mouth, vec3(0.18f, 0.130f, 0.10f));
    Primitives::drawCube(shader, mouth, charColor);

    // ── 4. Multiple Firewood Sticks with Charred Burning Tips ────
    mat4 stick1 = cm;
    stick1 = translate(stick1, vec3(-0.04f, 0.050f, 0.28f));
    stick1 = rotate(stick1, radians(14.0f), vec3(1.0f, 0.0f, 0.0f));
    stick1 = rotate(stick1, radians(-10.0f), vec3(0.0f, 1.0f, 0.0f));
    mat4 stick1S = scale(stick1, vec3(0.028f, 0.028f, 0.26f));
    Primitives::drawCylinder(shader, stick1S, woodColor);

    // Charred tip of stick 1
    mat4 tip1 = stick1;
    tip1 = translate(tip1, vec3(0.0f, 0.0f, -0.10f));
    tip1 = scale(tip1, vec3(0.030f, 0.030f, 0.06f));
    Primitives::drawCylinder(shader, tip1, charColor);

    mat4 stick2 = cm;
    stick2 = translate(stick2, vec3(0.05f, 0.045f, 0.29f));
    stick2 = rotate(stick2, radians(12.0f), vec3(1.0f, 0.0f, 0.0f));
    stick2 = rotate(stick2, radians(14.0f), vec3(0.0f, 1.0f, 0.0f));
    mat4 stick2S = scale(stick2, vec3(0.026f, 0.026f, 0.28f));
    Primitives::drawCylinder(shader, stick2S, woodColor);

    // ── 5. Clay Prongs & Primary Cooking Pot (Hari with Lid) ──────
    for (int j = 0; j < 3; j++) {
        float ja = (float)j * (2.0f * 3.14159f / 3.0f);
        mat4 prong = cm;
        prong = translate(prong, vec3(cosf(ja) * 0.14f, 0.270f, 0.06f + sinf(ja) * 0.14f));
        prong = scale(prong, vec3(0.035f, 0.065f, 0.035f));
        Primitives::drawCone(shader, prong, mudColor * 0.90f);
    }

    // Flame soot base on cooking pot
    mat4 potSootM = cm;
    potSootM = translate(potSootM, vec3(0.0f, 0.275f, 0.06f));
    potSootM = scale(potSootM, vec3(0.160f, 0.050f, 0.160f));
    Primitives::drawSphere(shader, potSootM, potSoot);

    // Main cooking pot belly (Hari)
    mat4 pot1 = cm;
    pot1 = translate(pot1, vec3(0.0f, 0.335f, 0.06f));
    pot1 = scale(pot1, vec3(0.185f, 0.125f, 0.185f));
    Primitives::drawSphere(shader, pot1, potColor);

    // Flared rim of pot
    mat4 potRim = cm;
    potRim = translate(potRim, vec3(0.0f, 0.395f, 0.06f));
    potRim = scale(potRim, vec3(0.140f, 0.020f, 0.140f));
    Primitives::drawCylinder(shader, potRim, potColor * 0.88f);

    // Terracotta pot lid (Sharani / ঢাকনা)
    mat4 potLid = cm;
    potLid = translate(potLid, vec3(0.0f, 0.415f, 0.06f));
    potLid = scale(potLid, vec3(0.135f, 0.025f, 0.135f));
    Primitives::drawSphere(shader, potLid, pot2Color);

    // Lid handle knob
    mat4 lidKnob = cm;
    lidKnob = translate(lidKnob, vec3(0.0f, 0.435f, 0.06f));
    lidKnob = scale(lidKnob, vec3(0.022f, 0.022f, 0.022f));
    Primitives::drawSphere(shader, lidKnob, potColor);

    // ── 6. Secondary Simmer Pot on Rear Burner ────────────────────
    mat4 pot2 = cm;
    pot2 = translate(pot2, vec3(0.0f, 0.285f, -0.22f));
    pot2 = scale(pot2, vec3(0.145f, 0.110f, 0.145f));
    Primitives::drawSphere(shader, pot2, pot2Color);

    mat4 pot2Rim = cm;
    pot2Rim = translate(pot2Rim, vec3(0.0f, 0.340f, -0.22f));
    pot2Rim = scale(pot2Rim, vec3(0.110f, 0.018f, 0.110f));
    Primitives::drawCylinder(shader, pot2Rim, pot2Color * 0.85f);

    // ── 7. Traditional Bengali Vegetable Cutter (Boti / বঁটি) & Piri ─────
    // Wooden footboard resting on the ground in front-right of stove where cook sits
    mat4 botiBoard = cm;
    botiBoard = translate(botiBoard, vec3(0.22f, 0.015f, 0.28f));
    botiBoard = rotate(botiBoard, radians(-18.0f), vec3(0.0f, 1.0f, 0.0f));
    mat4 bBoardS = scale(botiBoard, vec3(0.13f, 0.028f, 0.30f));
    Primitives::drawCube(shader, bBoardS, boardWood);

    // Upright curved steel cutting blade (Boti-r Daal)
    mat4 botiBlade = botiBoard;
    botiBlade = translate(botiBlade, vec3(0.0f, 0.10f, 0.05f));
    botiBlade = rotate(botiBlade, radians(24.0f), vec3(1.0f, 0.0f, 0.0f));
    mat4 bBladeS = scale(botiBlade, vec3(0.009f, 0.18f, 0.052f));
    Primitives::drawCube(shader, bBladeS, steelColor);

    // Curled tip beak of the blade (Kada)
    mat4 botiTip = botiBoard;
    botiTip = translate(botiTip, vec3(0.0f, 0.19f, 0.085f));
    botiTip = scale(botiTip, vec3(0.012f, 0.028f, 0.028f));
    Primitives::drawSphere(shader, botiTip, steelColor * 0.92f);

    // Traditional low wooden sitting stool (Kather Piri / পিঁড়ি) where the cook sits
    mat4 piri = cm;
    piri = translate(piri, vec3(0.22f, 0.022f, 0.44f));
    piri = scale(piri, vec3(0.20f, 0.040f, 0.13f));
    Primitives::drawCube(shader, piri, boardWood * 1.15f);

    // Small clay bowl (Matir Bati) with sliced onion and green chili beside boti
    mat4 prepBowl = cm;
    prepBowl = translate(prepBowl, vec3(0.36f, 0.025f, 0.22f));
    mat4 pBowlS = scale(prepBowl, vec3(0.065f, 0.032f, 0.065f));
    Primitives::drawCylinder(shader, pBowlS, pot2Color);

    mat4 onion = prepBowl;
    onion = translate(onion, vec3(-0.014f, 0.032f, 0.0f));
    onion = scale(onion, vec3(0.024f, 0.016f, 0.024f));
    Primitives::drawSphere(shader, onion, onionRed);

    mat4 chili = prepBowl;
    chili = translate(chili, vec3(0.018f, 0.032f, 0.006f));
    chili = rotate(chili, radians(35.0f), vec3(0.0f, 1.0f, 0.0f));
    chili = scale(chili, vec3(0.009f, 0.009f, 0.042f));
    Primitives::drawCylinder(shader, chili, chiliGreen);

    // ── 8. Active Roaring Wood Fire & Licking Pot Flames (Chular Agun) ─
    drawStoveFire(shader, cm, animTime);
}

void draw(Shader& shader, const mat4& model, HouseStyle style, bool withStove, float animTime)
{
    shader.setInt("uUseTexture", 0); // authentic sun-dried clay/mud plaster, golden rice thatch, and bamboo

    // ── Traditional Color Palette ───────────────────────────────
    vec3 plinthColor (0.46f, 0.36f, 0.24f);  // dark packed clay earth (Matir Viti)
    vec3 wallColor   (0.72f, 0.64f, 0.50f);  // authentic sun-baked mud / clay plaster
    vec3 cornerPost  (0.42f, 0.30f, 0.16f);  // mature seasoned bamboo/timber post
    vec3 roofStraw   (0.68f, 0.55f, 0.26f);  // golden dried rice straw thatch
    vec3 roofRidge   (0.50f, 0.38f, 0.18f);  // thatch ridge cap / bamboo runner
    vec3 doorWood    (0.32f, 0.20f, 0.10f);  // dark seasoned timber door
    vec3 windowFrame (0.24f, 0.16f, 0.08f);  // timber window frame
    vec3 shutterColor(0.38f, 0.24f, 0.12f);  // open timber window shutters
    vec3 postColor   (0.54f, 0.46f, 0.24f);  // natural bamboo verandah pillars
    vec3 rafterColor (0.40f, 0.28f, 0.14f);  // bamboo/timber rafters

    // Base house dimensions
    float houseW = 3.6f;  // width along X
    float houseH = 2.0f;  // wall height
    float houseD = 2.8f;  // depth along Z
    float plinthH = 0.25f; // raised earthen base
    float roofBaseY = plinthH + houseH;

    if (style == HOUSE_CHOUCHALA) {
        // ── 1. Raised Earthen Plinth (Viti / Dawa) ────────────────────
        mat4 plinth = model;
        plinth = translate(plinth, vec3(0.0f, plinthH * 0.5f, 0.35f));
        plinth = scale(plinth, vec3(houseW + 0.8f, plinthH, houseD + 1.4f));
        Primitives::drawCube(shader, plinth, plinthColor);

        // Front entrance step
        mat4 step = model;
        step = translate(step, vec3(0.0f, plinthH * 0.25f, (houseD + 1.4f) * 0.5f + 0.35f + 0.15f));
        step = scale(step, vec3(1.2f, plinthH * 0.5f, 0.40f));
        Primitives::drawCube(shader, step, plinthColor);

        // ── 2. Main Walls (Mud / Sun-dried Clay) ──────────────────────
        float wallCenterY = plinthH + houseH * 0.5f;
        mat4 walls = model;
        walls = translate(walls, vec3(0.0f, wallCenterY, 0.0f));
        walls = scale(walls, vec3(houseW, houseH, houseD));
        Primitives::drawCube(shader, walls, wallColor);

        // Timber corner posts at 4 corners
        float hx = houseW * 0.5f;
        float hz = houseD * 0.5f;
        float cornerOffsets[4][2] = { {-hx, -hz}, {hx, -hz}, {-hx, hz}, {hx, hz} };
        for (int i = 0; i < 4; i++) {
            mat4 cp = model;
            cp = translate(cp, vec3(cornerOffsets[i][0], wallCenterY, cornerOffsets[i][1]));
            cp = scale(cp, vec3(0.12f, houseH + 0.05f, 0.12f));
            Primitives::drawCube(shader, cp, cornerPost);
        }

        // Horizontal bamboo tie-beam along top of front wall
        mat4 beamFront = model;
        beamFront = translate(beamFront, vec3(0.0f, plinthH + houseH, hz));
        beamFront = scale(beamFront, vec3(houseW + 0.1f, 0.08f, 0.10f));
        Primitives::drawCube(shader, beamFront, cornerPost);

        // ── 3. Traditional 4-sloped pitched hip roof (Chouchala) ─────
        float roofW = houseW + 1.2f;
        float roofD = houseD + 1.2f;
        float roofH = 1.6f;

        mat4 roof = model;
        roof = translate(roof, vec3(0.0f, roofBaseY, 0.0f));
        roof = scale(roof, vec3(roofW, roofH, roofD));
        Primitives::drawPyramid(shader, roof, roofStraw);

        // Under-eave rafter trim (slight dark underside)
        mat4 eaveTrim = model;
        eaveTrim = translate(eaveTrim, vec3(0.0f, roofBaseY - 0.02f, 0.0f));
        eaveTrim = scale(eaveTrim, vec3(roofW * 0.96f, 0.05f, roofD * 0.96f));
        Primitives::drawCube(shader, eaveTrim, rafterColor);

        // ── 4. Front Verandah (Baranda) ──────────────────────────────
        float verandahDepth = 1.0f;
        float verandahZ = hz + verandahDepth * 0.5f;
        float postZ = hz + verandahDepth;

        // 4 slender bamboo pillars supporting verandah roof
        int numPosts = 4;
        for (int i = 0; i < numPosts; i++) {
            float t = (float)i / (numPosts - 1);
            float px = -hx + t * houseW;
            mat4 post = model;
            post = translate(post, vec3(px, plinthH + (houseH * 0.85f) * 0.5f, postZ));
            post = scale(post, vec3(0.07f, houseH * 0.85f, 0.07f));
            Primitives::drawCylinder(shader, post, postColor);
        }

        // Sloping verandah lean-to roof extending out from main wall
        mat4 vRoof = model;
        vRoof = translate(vRoof, vec3(0.0f, plinthH + houseH * 0.88f, verandahZ));
        vRoof = rotate(vRoof, radians(12.0f), vec3(1.0f, 0.0f, 0.0f)); // gentle forward slope
        vRoof = scale(vRoof, vec3(houseW + 0.8f, 0.07f, verandahDepth + 0.35f));
        Primitives::drawCube(shader, vRoof, roofStraw);

        // Low wooden railing / bench on verandah side
        mat4 vBench = model;
        vBench = translate(vBench, vec3(-hx + 0.4f, plinthH + 0.25f, verandahZ));
        vBench = scale(vBench, vec3(0.7f, 0.06f, verandahDepth * 0.7f));
        Primitives::drawCube(shader, vBench, doorWood);

        // ── 5. Wooden Door & Frame ───────────────────────────────────
        float doorW = 0.70f;
        float doorH = 1.40f;
        float doorY = plinthH + doorH * 0.5f;

        mat4 dFrame = model;
        dFrame = translate(dFrame, vec3(0.0f, doorY, hz + 0.02f));
        dFrame = scale(dFrame, vec3(doorW + 0.12f, doorH + 0.10f, 0.04f));
        Primitives::drawCube(shader, dFrame, windowFrame);

        mat4 dPanel = model;
        dPanel = translate(dPanel, vec3(0.0f, doorY, hz + 0.03f));
        dPanel = scale(dPanel, vec3(doorW, doorH, 0.03f));
        Primitives::drawCube(shader, dPanel, doorWood);

        // ── 6. Windows with Open Wooden Shutters ─────────────────────
        float winSize = 0.55f;
        float winY = plinthH + houseH * 0.55f;

        // Left front window
        float winLX = -hx * 0.60f;
        mat4 wFrameL = model;
        wFrameL = translate(wFrameL, vec3(winLX, winY, hz + 0.02f));
        wFrameL = scale(wFrameL, vec3(winSize, winSize, 0.04f));
        Primitives::drawCube(shader, wFrameL, windowFrame);

        vec3 windowGlow(0.92f, 0.70f, 0.28f);
        shader.setFloat("emissive", 0.70f);
        mat4 wOpeningL = model;
        wOpeningL = translate(wOpeningL, vec3(winLX, winY, hz + 0.03f));
        wOpeningL = scale(wOpeningL, vec3(winSize * 0.85f, winSize * 0.85f, 0.03f));
        Primitives::drawCube(shader, wOpeningL, windowGlow);
        shader.setFloat("emissive", 0.0f);

        mat4 wShutterL = model;
        wShutterL = translate(wShutterL, vec3(winLX - winSize * 0.45f, winY, hz + 0.15f));
        wShutterL = rotate(wShutterL, radians(-45.0f), vec3(0.0f, 1.0f, 0.0f));
        wShutterL = scale(wShutterL, vec3(winSize * 0.45f, winSize * 0.85f, 0.025f));
        Primitives::drawCube(shader, wShutterL, shutterColor);

        // Right front window
        float winRX = hx * 0.60f;
        mat4 wFrameR = model;
        wFrameR = translate(wFrameR, vec3(winRX, winY, hz + 0.02f));
        wFrameR = scale(wFrameR, vec3(winSize, winSize, 0.04f));
        Primitives::drawCube(shader, wFrameR, windowFrame);

        shader.setFloat("emissive", 0.70f);
        mat4 wOpeningR = model;
        wOpeningR = translate(wOpeningR, vec3(winRX, winY, hz + 0.03f));
        wOpeningR = scale(wOpeningR, vec3(winSize * 0.85f, winSize * 0.85f, 0.03f));
        Primitives::drawCube(shader, wOpeningR, windowGlow);
        shader.setFloat("emissive", 0.0f);

        mat4 wShutterR = model;
        wShutterR = translate(wShutterR, vec3(winRX + winSize * 0.45f, winY, hz + 0.15f));
        wShutterR = rotate(wShutterR, radians(45.0f), vec3(0.0f, 1.0f, 0.0f));
        wShutterR = scale(wShutterR, vec3(winSize * 0.45f, winSize * 0.85f, 0.025f));
        Primitives::drawCube(shader, wShutterR, shutterColor);

        // ── 7. Terracotta Water Pitchers on Verandah ─────────────────
        drawKolshi(shader, model, vec3(hx * 0.70f, plinthH, postZ - 0.20f), 0.90f);
        drawKolshi(shader, model, vec3(hx * 0.85f, plinthH, postZ - 0.35f), 0.75f);

        // ── 8. Outdoor Clay Cooking Stove ────────────────────────────
        if (withStove) {
            mat4 cm = model;
            cm = translate(cm, vec3(hx + 2.40f, 0.0f, 0.60f));
            drawStove(shader, cm, true, animTime);
        }
    }
    else {
        // =============================================================
        // AUTHENTIC BANGLADESHI DOCHALA HOUSE (দৌচালা ঘর)
        // In vernacular Bengal architecture, the front entrance, door,
        // windows, and bamboo verandah are on the LONG CHALA SIDE (+X, under
        // the sloping eave), while the triangular gable ends form the side walls (+Z / -Z).
        // =============================================================
        float dHouseW = 2.8f; // gable width along X (depth from front to back)
        float dHouseD = 3.6f; // long chala length along Z (frontage width)
        float dHouseH = 2.0f; // wall height

        float dhx = dHouseW * 0.5f; // 1.4f
        float dhz = dHouseD * 0.5f; // 1.8f
        float dWallCenterY = plinthH + dHouseH * 0.5f;

        // ── 1. Raised Earthen Plinth (Viti / Dawa) ────────────────────
        // Extended towards +X to support the front verandah along the long chala side
        mat4 plinth = model;
        plinth = translate(plinth, vec3(0.35f, plinthH * 0.5f, 0.0f));
        plinth = scale(plinth, vec3(dHouseW + 1.4f, plinthH, dHouseD + 0.8f));
        Primitives::drawCube(shader, plinth, plinthColor);

        // Front entrance step along +X leading into the verandah
        mat4 step = model;
        step = translate(step, vec3((dHouseW + 1.4f) * 0.5f + 0.35f + 0.15f, plinthH * 0.25f, 0.0f));
        step = scale(step, vec3(0.40f, plinthH * 0.5f, 1.20f));
        Primitives::drawCube(shader, step, plinthColor);

        // ── 2. Main Mud Walls ────────────────────────────────────────
        mat4 walls = model;
        walls = translate(walls, vec3(0.0f, dWallCenterY, 0.0f));
        walls = scale(walls, vec3(dHouseW, dHouseH, dHouseD));
        Primitives::drawCube(shader, walls, wallColor);

        // Timber corner posts at 4 corners
        float dCornerOffsets[4][2] = { {-dhx, -dhz}, {dhx, -dhz}, {-dhx, dhz}, {dhx, dhz} };
        for (int i = 0; i < 4; i++) {
            mat4 cp = model;
            cp = translate(cp, vec3(dCornerOffsets[i][0], dWallCenterY, dCornerOffsets[i][1]));
            cp = scale(cp, vec3(0.12f, dHouseH + 0.05f, 0.12f));
            Primitives::drawCube(shader, cp, cornerPost);
        }

        // Horizontal bamboo tie-beam along top of front wall (+X)
        mat4 beamFront = model;
        beamFront = translate(beamFront, vec3(dhx, plinthH + dHouseH, 0.0f));
        beamFront = scale(beamFront, vec3(0.10f, 0.08f, dHouseD + 0.1f));
        Primitives::drawCube(shader, beamFront, cornerPost);

        // Horizontal tie-beam along gable wall (+Z)
        mat4 beamGable = model;
        beamGable = translate(beamGable, vec3(0.0f, plinthH + dHouseH, dhz));
        beamGable = scale(beamGable, vec3(dHouseW + 0.1f, 0.08f, 0.10f));
        Primitives::drawCube(shader, beamGable, cornerPost);

        // ── 3. Traditional 2-sloped pitched gable roof (Dochala) ─────
        // Ridge runs along Z; the two chalas slope down to +X (front) and -X (back)
        float roofW = dHouseW + 1.2f; // 4.0m across slopes (0.6m eave overhang on front & back)
        float roofD = dHouseD + 0.8f; // 4.4m along ridge (0.4m gable overhang)
        float roofH = 1.5f;

        mat4 roof = model;
        roof = translate(roof, vec3(0.0f, roofBaseY, 0.0f));
        roof = scale(roof, vec3(roofW, roofH, roofD));
        Primitives::drawPrism(shader, roof, roofStraw);

        // Ridge beam along peak
        mat4 ridge = model;
        ridge = translate(ridge, vec3(0.0f, roofBaseY + roofH, 0.0f));
        ridge = scale(ridge, vec3(0.12f, 0.10f, roofD + 0.1f));
        Primitives::drawCube(shader, ridge, roofRidge);

        // ── 4. Front Verandah (Baranda) on Long Chala Side (+X) ──────
        float verandahDepth = 1.0f;
        float verandahX = dhx + verandahDepth * 0.5f; // 1.90f
        float postX = dhx + verandahDepth;            // 2.40f

        // 4 slender bamboo pillars supporting verandah roof along Z
        int numPosts = 4;
        for (int i = 0; i < numPosts; i++) {
            float t = (float)i / (numPosts - 1);
            float pz = -dhz + t * dHouseD;
            mat4 post = model;
            post = translate(post, vec3(postX, plinthH + (dHouseH * 0.85f) * 0.5f, pz));
            post = scale(post, vec3(0.07f, dHouseH * 0.85f, 0.07f));
            Primitives::drawCylinder(shader, post, postColor);
        }

        // Sloping verandah lean-to roof extending out towards +X (matching main thatch slope)
        mat4 vRoof = model;
        vRoof = translate(vRoof, vec3(verandahX, plinthH + dHouseH * 0.88f, 0.0f));
        vRoof = rotate(vRoof, radians(-12.0f), vec3(0.0f, 0.0f, 1.0f)); // slope forward down to +X
        vRoof = scale(vRoof, vec3(verandahDepth + 0.35f, 0.07f, dHouseD + 0.8f));
        Primitives::drawCube(shader, vRoof, roofStraw);

        // Low wooden railing / bench on verandah side
        mat4 vBench = model;
        vBench = translate(vBench, vec3(verandahX, plinthH + 0.25f, -dhz + 0.4f));
        vBench = scale(vBench, vec3(verandahDepth * 0.7f, 0.06f, 0.7f));
        Primitives::drawCube(shader, vBench, doorWood);

        // ── 5. Wooden Front Door & Frame on Long Chala Wall (+X) ─────
        float doorW = 0.70f;
        float doorH = 1.40f;
        float doorY = plinthH + doorH * 0.5f;

        // Door frame
        mat4 dFrame = model;
        dFrame = translate(dFrame, vec3(dhx + 0.02f, doorY, 0.0f));
        dFrame = scale(dFrame, vec3(0.04f, doorH + 0.10f, doorW + 0.12f));
        Primitives::drawCube(shader, dFrame, windowFrame);

        // Door panel
        mat4 dPanel = model;
        dPanel = translate(dPanel, vec3(dhx + 0.03f, doorY, 0.0f));
        dPanel = scale(dPanel, vec3(0.03f, doorH, doorW));
        Primitives::drawCube(shader, dPanel, doorWood);

        // ── 6. Windows with Open Wooden Shutters on Long Chala Wall (+X) ──
        float winSize = 0.55f;
        float winY = plinthH + dHouseH * 0.55f;
        vec3 windowGlow(0.92f, 0.70f, 0.28f);

        // Left front window (at -Z along the +X wall)
        float winLZ = -dhz * 0.60f;
        mat4 wFrameL = model;
        wFrameL = translate(wFrameL, vec3(dhx + 0.02f, winY, winLZ));
        wFrameL = scale(wFrameL, vec3(0.04f, winSize, winSize));
        Primitives::drawCube(shader, wFrameL, windowFrame);

        shader.setFloat("emissive", 0.70f);
        mat4 wOpeningL = model;
        wOpeningL = translate(wOpeningL, vec3(dhx + 0.03f, winY, winLZ));
        wOpeningL = scale(wOpeningL, vec3(0.03f, winSize * 0.85f, winSize * 0.85f));
        Primitives::drawCube(shader, wOpeningL, windowGlow);
        shader.setFloat("emissive", 0.0f);

        mat4 wShutterL = model;
        wShutterL = translate(wShutterL, vec3(dhx + 0.15f, winY, winLZ - winSize * 0.45f));
        wShutterL = rotate(wShutterL, radians(-45.0f), vec3(0.0f, 1.0f, 0.0f));
        wShutterL = scale(wShutterL, vec3(0.025f, winSize * 0.85f, winSize * 0.45f));
        Primitives::drawCube(shader, wShutterL, shutterColor);

        // Right front window (at +Z along the +X wall)
        float winRZ = dhz * 0.60f;
        mat4 wFrameR = model;
        wFrameR = translate(wFrameR, vec3(dhx + 0.02f, winY, winRZ));
        wFrameR = scale(wFrameR, vec3(0.04f, winSize, winSize));
        Primitives::drawCube(shader, wFrameR, windowFrame);

        shader.setFloat("emissive", 0.70f);
        mat4 wOpeningR = model;
        wOpeningR = translate(wOpeningR, vec3(dhx + 0.03f, winY, winRZ));
        wOpeningR = scale(wOpeningR, vec3(0.03f, winSize * 0.85f, winSize * 0.85f));
        Primitives::drawCube(shader, wOpeningR, windowGlow);
        shader.setFloat("emissive", 0.0f);

        mat4 wShutterR = model;
        wShutterR = translate(wShutterR, vec3(dhx + 0.15f, winY, winRZ + winSize * 0.45f));
        wShutterR = rotate(wShutterR, radians(45.0f), vec3(0.0f, 1.0f, 0.0f));
        wShutterR = scale(wShutterR, vec3(0.025f, winSize * 0.85f, winSize * 0.45f));
        Primitives::drawCube(shader, wShutterR, shutterColor);

        // ── 7. Terracotta Water Pitchers (Matir Kolshi) on Verandah ───
        drawKolshi(shader, model, vec3(postX - 0.20f, plinthH, dhz * 0.70f), 0.90f);
        drawKolshi(shader, model, vec3(postX - 0.35f, plinthH, dhz * 0.85f), 0.75f);

        // ── 8. Side Gable Window on +Z Wall ──────────────────────────
        // Traditional mud house ventilation window beneath the triangular gable
        mat4 wFrameG = model;
        wFrameG = translate(wFrameG, vec3(0.0f, winY, dhz + 0.02f));
        wFrameG = scale(wFrameG, vec3(winSize, winSize, 0.04f));
        Primitives::drawCube(shader, wFrameG, windowFrame);

        shader.setFloat("emissive", 0.70f);
        mat4 wOpeningG = model;
        wOpeningG = translate(wOpeningG, vec3(0.0f, winY, dhz + 0.03f));
        wOpeningG = scale(wOpeningG, vec3(winSize * 0.85f, winSize * 0.85f, 0.03f));
        Primitives::drawCube(shader, wOpeningG, windowGlow);
        shader.setFloat("emissive", 0.0f);

        mat4 wShutterG = model;
        wShutterG = translate(wShutterG, vec3(winSize * 0.45f, winY, dhz + 0.15f));
        wShutterG = rotate(wShutterG, radians(45.0f), vec3(0.0f, 1.0f, 0.0f));
        wShutterG = scale(wShutterG, vec3(winSize * 0.45f, winSize * 0.85f, 0.025f));
        Primitives::drawCube(shader, wShutterG, shutterColor);

        // ── 9. Outdoor Clay Cooking Stove (Optional) ─────────────────
        if (withStove) {
            mat4 cm = model;
            cm = translate(cm, vec3(0.0f, 0.0f, -dhz - 2.0f));
            drawStove(shader, cm, true, animTime);
        }
    }
}

// Traditional Rice Straw Stack (Khorer Paloi / খড়ের পালা)
void drawStrawStack(Shader& shader, const mat4& model, const vec3& pos, float scaleVal)
{
    shader.setInt("uUseTexture", 0); // smooth natural dried rice straw
    vec3 strawDark  (0.66f, 0.52f, 0.20f); // weathered lower base straw
    vec3 strawMid   (0.74f, 0.60f, 0.24f); // sun-dried golden rice straw belly
    vec3 strawTop   (0.80f, 0.66f, 0.28f); // fresh bright straw cap
    vec3 ropeCol    (0.52f, 0.38f, 0.15f); // twisted jute straw tie-bands (Khorer Badhon)
    vec3 poleCol    (0.38f, 0.28f, 0.14f); // central bamboo stabilizer pole (Khuti)
    vec3 nodeCol    (0.26f, 0.18f, 0.10f); // bamboo node rings
    vec3 potCol     (0.72f, 0.36f, 0.18f); // inverted terracotta pot rain-cap (Ulto Kolshi)
    vec3 potRimCol  (0.58f, 0.28f, 0.14f); // clay pot rim trim
    vec3 logCol     (0.34f, 0.22f, 0.12f); // timber/bamboo foundation platform logs

    mat4 m = model;
    m = translate(m, pos);
    m = scale(m, vec3(scaleVal));

    // 1. Raised foundation log cradle (keeps winter straw off damp ground)
    for (int l = 0; l < 4; l++) {
        float la = (float)l * 45.0f;
        mat4 logM = m;
        logM = rotate(logM, radians(la), vec3(0.0f, 1.0f, 0.0f));
        logM = translate(logM, vec3(0.0f, 0.045f, 0.0f));
        logM = scale(logM, vec3(0.10f, 0.08f, 1.70f));
        Primitives::drawCube(shader, logM, logCol);
    }

    // 2. Central bamboo stabilizer pole (Khuti) extending from base up to the top
    mat4 pole = m;
    pole = translate(pole, vec3(0.0f, 1.60f, 0.0f));
    pole = scale(pole, vec3(0.040f, 3.20f, 0.040f));
    Primitives::drawCylinder(shader, pole, poleCol);

    // Bamboo node rings along the upper exposed pole
    for (int nr = 0; nr < 3; nr++) {
        mat4 nRing = m;
        nRing = translate(nRing, vec3(0.0f, 2.72f + (float)nr * 0.18f, 0.0f));
        nRing = scale(nRing, vec3(0.048f, 0.018f, 0.048f));
        Primitives::drawCylinder(shader, nRing, nodeCol);
    }

    // 3. Smooth bulging straw stack body (traditional rural silhouette)
    // Tier 1: Weathered lower base cylinder
    mat4 t1 = m;
    t1 = translate(t1, vec3(0.0f, 0.40f, 0.0f));
    t1 = scale(t1, vec3(0.92f, 0.70f, 0.92f));
    Primitives::drawCylinder(shader, t1, strawDark);

    // Tier 2: Wide rounded bulging belly
    mat4 t2 = m;
    t2 = translate(t2, vec3(0.0f, 0.92f, 0.0f));
    t2 = scale(t2, vec3(1.08f, 0.75f, 1.08f));
    Primitives::drawSphere(shader, t2, strawMid);

    // Tier 3: Single continuous steep conical rain-shedding thatch (Y: 1.05 -> 2.70)
    mat4 t3 = m;
    t3 = translate(t3, vec3(0.0f, 1.05f, 0.0f));
    t3 = scale(t3, vec3(1.02f, 1.65f, 1.02f));
    Primitives::drawCone(shader, t3, strawTop);

    // 4. Horizontal straw binding ropes (Khorer Badhon / বাঁধন) wrapping the stack
    // Band 1: Lower waist
    mat4 b1 = m;
    b1 = translate(b1, vec3(0.0f, 0.55f, 0.0f));
    b1 = scale(b1, vec3(0.94f, 0.024f, 0.94f));
    Primitives::drawCylinder(shader, b1, ropeCol);

    // Band 2: Mid belly
    mat4 b2 = m;
    b2 = translate(b2, vec3(0.0f, 0.92f, 0.0f));
    b2 = scale(b2, vec3(1.09f, 0.024f, 1.09f));
    Primitives::drawCylinder(shader, b2, ropeCol);

    // Band 3: Upper cone
    mat4 b3 = m;
    b3 = translate(b3, vec3(0.0f, 1.60f, 0.0f));
    b3 = scale(b3, vec3(0.69f, 0.022f, 0.69f));
    Primitives::drawCylinder(shader, b3, ropeCol);

    // 5. Inverted Terracotta Clay Pitcher (Ulto Matir Kolshi) capping the pole apex!
    // Iconic rural practice: placing a clay pot upside down over the bamboo pole prevents rainwater ingress
    float potCenterY = 2.78f;
    mat4 potBody = m;
    potBody = translate(potBody, vec3(0.0f, potCenterY, 0.0f));
    potBody = scale(potBody, vec3(0.165f, 0.175f, 0.165f));
    Primitives::drawSphere(shader, potBody, potCol);

    // Inverted neck extending downward
    mat4 potNeck = m;
    potNeck = translate(potNeck, vec3(0.0f, potCenterY - 0.095f, 0.0f));
    potNeck = scale(potNeck, vec3(0.085f, 0.050f, 0.085f));
    Primitives::drawCylinder(shader, potNeck, potCol * 0.92f);

    // Inverted flared rim
    mat4 potRim = m;
    potRim = translate(potRim, vec3(0.0f, potCenterY - 0.120f, 0.0f));
    potRim = scale(potRim, vec3(0.115f, 0.022f, 0.115f));
    Primitives::drawCylinder(shader, potRim, potRimCol);
}

// Traditional Thatched Cow Shed (Gowal Ghor / গোয়াল ঘর)
void drawCowShed(Shader& shader, const mat4& model)
{
    shader.setInt("uUseTexture", 0); // authentic vernacular thatch and timber

    vec3 postCol   (0.42f, 0.30f, 0.16f); // weathered bamboo/timber posts
    vec3 roofStraw (0.68f, 0.55f, 0.26f); // golden dried rice straw thatch
    vec3 roofRidge (0.50f, 0.38f, 0.18f); // bamboo ridge pole
    vec3 troughCol (0.36f, 0.24f, 0.12f); // hollowed timber feeding trough (Chari)
    vec3 hayCol    (0.70f, 0.58f, 0.24f); // sun-dried hay
    vec3 grassCol  (0.28f, 0.48f, 0.18f); // fresh green grass
    vec3 fenceCol  (0.46f, 0.34f, 0.18f); // side bamboo railings
    vec3 strawBed  (0.72f, 0.60f, 0.26f); // straw bedding (Khor-er Bicha)
    vec3 earthFloor(0.44f, 0.34f, 0.22f); // compacted earthen floor
    vec3 ropeCol   (0.68f, 0.56f, 0.34f); // jute halter rope (Pagha)

    float shedW = 3.2f;
    float shedD = 2.4f;
    float postH = 1.70f;
    float halfW = shedW * 0.5f;
    float halfD = shedD * 0.5f;

    // Earthen floor plinth
    mat4 floorM = model;
    floorM = translate(floorM, vec3(0.0f, 0.04f, 0.0f));
    floorM = scale(floorM, vec3(shedW + 0.3f, 0.08f, shedD + 0.3f));
    Primitives::drawCube(shader, floorM, earthFloor);

    // Straw bedding layer across floor
    mat4 bedM = model;
    bedM = translate(bedM, vec3(0.0f, 0.08f, 0.0f));
    bedM = scale(bedM, vec3(shedW * 0.92f, 0.04f, shedD * 0.88f));
    Primitives::drawCube(shader, bedM, strawBed);

    // 4 Corner upright bamboo posts
    float corners[4][2] = {
        {-halfW, -halfD}, {halfW, -halfD},
        {-halfW,  halfD}, {halfW,  halfD}
    };
    for (int i = 0; i < 4; i++) {
        mat4 post = model;
        post = translate(post, vec3(corners[i][0], postH * 0.5f, corners[i][1]));
        post = scale(post, vec3(0.07f, postH, 0.07f));
        Primitives::drawCylinder(shader, post, postCol);
    }

    // 2 Center gable truss posts
    for (int side = -1; side <= 1; side += 2) {
        mat4 gPost = model;
        gPost = translate(gPost, vec3((float)side * halfW, (postH + 0.40f) * 0.5f, 0.0f));
        gPost = scale(gPost, vec3(0.065f, postH + 0.40f, 0.065f));
        Primitives::drawCylinder(shader, gPost, postCol);
    }

    // Bamboo tie beams connecting the posts at the top
    mat4 beamFront = model;
    beamFront = translate(beamFront, vec3(0.0f, postH, halfD));
    beamFront = scale(beamFront, vec3(shedW, 0.07f, 0.07f));
    Primitives::drawCube(shader, beamFront, postCol);

    mat4 beamBack = model;
    beamBack = translate(beamBack, vec3(0.0f, postH, -halfD));
    beamBack = scale(beamBack, vec3(shedW, 0.07f, 0.07f));
    Primitives::drawCube(shader, beamBack, postCol);

    // 2-Sloped thatched Dochala roof (oriented along X so eaves overhang front +Z and back -Z)
    mat4 roof = model;
    roof = translate(roof, vec3(0.0f, postH, 0.0f));
    roof = rotate(roof, radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
    roof = scale(roof, vec3(shedD + 0.9f, 1.15f, shedW + 0.8f));
    Primitives::drawPrism(shader, roof, roofStraw);

    // Bamboo ridge beam running along the peak
    mat4 ridge = model;
    ridge = translate(ridge, vec3(0.0f, postH + 1.16f, 0.0f));
    ridge = scale(ridge, vec3(shedW + 0.95f, 0.08f, 0.08f));
    Primitives::drawCube(shader, ridge, roofRidge);

    // Side bamboo half-railings on 3 sides (back, left, right)
    float railH = 0.55f;
    mat4 rBack = model;
    rBack = translate(rBack, vec3(0.0f, railH * 0.5f, -halfD));
    rBack = scale(rBack, vec3(shedW, railH, 0.04f));
    Primitives::drawCube(shader, rBack, fenceCol);

    mat4 rLeft = model;
    rLeft = translate(rLeft, vec3(-halfW, railH * 0.5f, 0.0f));
    rLeft = scale(rLeft, vec3(0.04f, railH, shedD));
    Primitives::drawCube(shader, rLeft, fenceCol);

    mat4 rRight = model;
    rRight = translate(rRight, vec3(halfW, railH * 0.5f, 0.0f));
    rRight = scale(rRight, vec3(0.04f, railH, shedD));
    Primitives::drawCube(shader, rRight, fenceCol);

    // Wooden cattle feeding trough (Chari / চারি) at the back wall
    mat4 trough = model;
    trough = translate(trough, vec3(0.0f, 0.22f, -halfD + 0.45f));
    trough = scale(trough, vec3(2.4f, 0.30f, 0.45f));
    Primitives::drawCube(shader, trough, troughCol);

    // Fresh grass and hay inside trough
    mat4 grass = model;
    grass = translate(grass, vec3(-0.4f, 0.35f, -halfD + 0.45f));
    grass = scale(grass, vec3(1.1f, 0.10f, 0.36f));
    Primitives::drawCube(shader, grass, grassCol);

    mat4 hay = model;
    hay = translate(hay, vec3(0.5f, 0.35f, -halfD + 0.45f));
    hay = scale(hay, vec3(1.0f, 0.10f, 0.36f));
    Primitives::drawCube(shader, hay, hayCol);

    // Resting Deshi Cow placed in center, facing forward toward the open shed entrance
    mat4 cowM = model;
    cowM = translate(cowM, vec3(0.05f, 0.0f, 0.12f));
    cowM = rotate(cowM, radians(-18.0f), vec3(0.0f, 1.0f, 0.0f)); // slight rakish turn towards viewer
    drawCow(shader, cowM, true);

    // Jute halter tether rope (Pagha) from cow's neck to the front right bamboo post
    mat4 rope = model;
    rope = translate(rope, vec3(0.42f, 0.35f, 0.46f));
    rope = rotate(rope, radians(42.0f), vec3(0.0f, 1.0f, 0.0f));
    rope = rotate(rope, radians(18.0f), vec3(1.0f, 0.0f, 0.0f));
    rope = scale(rope, vec3(0.014f, 0.014f, 0.65f));
    Primitives::drawCylinder(shader, rope, ropeCol);
}

// ─── 6. Procedural Deshi Cow (দেশি গরু) with Shoulder Hump & Horns ───
void drawCow(Shader& shader, const mat4& model, bool lyingDown, const vec3* customHideCol, float walkPhase)
{
    shader.setInt("uUseTexture", 0); // smooth natural hide, horns, and muzzle

    vec3 hideCol   = customHideCol ? *customHideCol : vec3(0.64f, 0.44f, 0.26f); // warm fawn-brown coat (or custom)
    vec3 bellyCol  (0.78f, 0.68f, 0.54f); // lighter cream underbelly & muzzle trim
    vec3 humpCol   = hideCol * 0.90f;     // distinctive muscular Zebu shoulder hump (Kud)
    vec3 hornCol   (0.88f, 0.85f, 0.78f); // smooth ivory horn
    vec3 hornTip   (0.24f, 0.18f, 0.12f); // dark horn tip
    vec3 muzzleCol (0.22f, 0.16f, 0.14f); // dark moist muzzle
    vec3 hoofCol   (0.18f, 0.14f, 0.10f); // dark cloven hooves
    vec3 eyeCol    (0.08f, 0.08f, 0.08f); // gentle dark eyes
    vec3 eyeWhite  (0.92f, 0.90f, 0.86f); // eye sclera highlight
    vec3 halterCol (0.65f, 0.52f, 0.32f); // braided jute neck halter rope (Pagha)

    mat4 m = model;

    // 1. Torso / Barrel Body (centered at origin, extending along Z)
    mat4 body = m;
    body = translate(body, vec3(0.0f, lyingDown ? 0.30f : 0.85f, 0.0f));
    body = scale(body, vec3(0.38f, 0.32f, 0.64f));
    Primitives::drawSphere(shader, body, hideCol);

    // Lighter underbelly patch
    mat4 belly = m;
    belly = translate(belly, vec3(0.0f, lyingDown ? 0.22f : 0.74f, 0.0f));
    belly = scale(belly, vec3(0.34f, 0.22f, 0.54f));
    Primitives::drawSphere(shader, belly, bellyCol);

    // 2. Iconic Zebu Shoulder Hump (Kud / কুঁদ) — placed above front shoulders (+Z)
    mat4 hump = m;
    hump = translate(hump, vec3(0.0f, lyingDown ? 0.50f : 1.22f, 0.15f));
    hump = rotate(hump, radians(12.0f), vec3(1.0f, 0.0f, 0.0f));
    hump = scale(hump, vec3(0.20f, 0.24f, 0.24f));
    Primitives::drawSphere(shader, hump, humpCol);

    // 3. Strong Neck rising upward and forward (+Z)
    mat4 neck = m;
    neck = translate(neck, vec3(0.0f, lyingDown ? 0.38f : 1.02f, 0.36f));
    neck = rotate(neck, radians(lyingDown ? -32.0f : -38.0f), vec3(1.0f, 0.0f, 0.0f));
    mat4 neckS = scale(neck, vec3(0.17f, 0.28f, 0.20f));
    Primitives::drawCylinder(shader, neckS, hideCol);

    // Braided jute rope halter (Pagha) around neck
    mat4 halter = neck;
    halter = translate(halter, vec3(0.0f, 0.02f, 0.0f));
    halter = scale(halter, vec3(0.18f, 0.035f, 0.21f));
    Primitives::drawCylinder(shader, halter, halterCol);

    // Deep Throat Dewlap (Golakomblo / গলকম্বল — pendulous skin folds under throat)
    mat4 dewlap = neck;
    dewlap = translate(dewlap, vec3(0.0f, -0.12f, 0.08f));
    dewlap = scale(dewlap, vec3(0.045f, 0.24f, 0.18f));
    Primitives::drawCube(shader, dewlap, bellyCol);

    // 4. Bovine Head & Muzzle (facing forward +Z)
    mat4 head = neck;
    head = translate(head, vec3(0.0f, 0.18f, 0.08f));
    head = rotate(head, radians(lyingDown ? 42.0f : 45.0f), vec3(1.0f, 0.0f, 0.0f));

    // Cranium
    mat4 cranium = head;
    cranium = scale(cranium, vec3(0.16f, 0.16f, 0.18f));
    Primitives::drawSphere(shader, cranium, hideCol);

    // Forehead brow
    mat4 brow = head;
    brow = translate(brow, vec3(0.0f, 0.08f, 0.06f));
    brow = scale(brow, vec3(0.14f, 0.08f, 0.12f));
    Primitives::drawSphere(shader, brow, hideCol * 0.94f);

    // Tapered Muzzle / Snout extending forward (+Z)
    mat4 muzzle = head;
    muzzle = translate(muzzle, vec3(0.0f, -0.06f, 0.14f));
    mat4 muzS = scale(muzzle, vec3(0.12f, 0.11f, 0.14f));
    Primitives::drawSphere(shader, muzS, muzzleCol);

    // Nostrils
    for (int s = -1; s <= 1; s += 2) {
        mat4 nos = muzzle;
        nos = translate(nos, vec3(s * 0.038f, 0.005f, 0.12f));
        nos = scale(nos, vec3(0.018f, 0.016f, 0.020f));
        Primitives::drawSphere(shader, nos, vec3(0.05f));
    }

    // Large gentle bovine eyes with white highlight
    for (int s = -1; s <= 1; s += 2) {
        mat4 eyeW = head;
        eyeW = translate(eyeW, vec3(s * 0.125f, 0.045f, 0.07f));
        eyeW = scale(eyeW, vec3(0.038f, 0.038f, 0.038f));
        Primitives::drawSphere(shader, eyeW, eyeWhite);

        mat4 eye = head;
        eye = translate(eye, vec3(s * 0.138f, 0.045f, 0.08f));
        eye = scale(eye, vec3(0.026f, 0.026f, 0.026f));
        Primitives::drawSphere(shader, eye, eyeCol);
    }

    // Floppy ears extending sideways and drooping softly downward
    for (int s = -1; s <= 1; s += 2) {
        mat4 ear = head;
        ear = translate(ear, vec3(s * 0.15f, 0.06f, -0.02f));
        ear = rotate(ear, radians(s * 42.0f), vec3(0.0f, 0.0f, 1.0f));
        ear = rotate(ear, radians(22.0f), vec3(1.0f, 0.0f, 0.0f));
        ear = scale(ear, vec3(0.14f, 0.045f, 0.075f));
        Primitives::drawSphere(shader, ear, hideCol);
    }

    // Pair of curved horns arching upward and inward
    for (int s = -1; s <= 1; s += 2) {
        mat4 horn = head;
        horn = translate(horn, vec3(s * 0.095f, 0.13f, -0.02f));
        horn = rotate(horn, radians(s * -22.0f), vec3(0.0f, 0.0f, 1.0f));
        horn = rotate(horn, radians(-26.0f), vec3(1.0f, 0.0f, 0.0f));
        mat4 hornS = scale(horn, vec3(0.032f, 0.18f, 0.032f));
        Primitives::drawCone(shader, hornS, hornCol);

        // Dark horn tip
        mat4 hTip = horn;
        hTip = translate(hTip, vec3(0.0f, 0.16f, 0.0f));
        hTip = scale(hTip, vec3(0.018f, 0.045f, 0.018f));
        Primitives::drawCone(shader, hTip, hornTip);
    }

    // 5. Legs
    if (lyingDown) {
        // Folded front legs under chest (+Z)
        for (int s = -1; s <= 1; s += 2) {
            mat4 fLeg = m;
            fLeg = translate(fLeg, vec3(s * 0.18f, 0.10f, 0.20f));
            fLeg = rotate(fLeg, radians(-80.0f), vec3(1.0f, 0.0f, 0.0f));
            fLeg = scale(fLeg, vec3(0.065f, 0.28f, 0.065f));
            Primitives::drawCylinder(shader, fLeg, hideCol);

            // Cloven hooves tucked neatly under
            mat4 hoof = m;
            hoof = translate(hoof, vec3(s * 0.18f, 0.055f, 0.32f));
            hoof = scale(hoof, vec3(0.065f, 0.045f, 0.075f));
            Primitives::drawCube(shader, hoof, hoofCol);
        }
        // Folded hind haunches tucked along flank (-Z)
        for (int s = -1; s <= 1; s += 2) {
            mat4 hHaunch = m;
            hHaunch = translate(hHaunch, vec3(s * 0.22f, 0.18f, -0.20f));
            hHaunch = scale(hHaunch, vec3(0.18f, 0.20f, 0.28f));
            Primitives::drawSphere(shader, hHaunch, hideCol);

            mat4 hHoof = m;
            hHoof = translate(hHoof, vec3(s * 0.20f, 0.055f, -0.05f));
            hHoof = scale(hHoof, vec3(0.060f, 0.045f, 0.070f));
            Primitives::drawCube(shader, hHoof, hoofCol);
        }
    } else {
        float legX[4] = { -0.26f, 0.26f, -0.28f, 0.28f };
        float legZ[4] = { 0.42f, 0.42f, -0.45f, -0.45f };
        float swingAngles[4] = {
            sinf(walkPhase) * radians(18.0f),
           -sinf(walkPhase) * radians(18.0f),
           -sinf(walkPhase) * radians(16.0f),
            sinf(walkPhase) * radians(16.0f)
        };
        for (int l = 0; l < 4; l++) {
            mat4 leg = m;
            leg = translate(leg, vec3(legX[l], 0.75f, legZ[l]));
            if (walkPhase != 0.0f) {
                leg = rotate(leg, swingAngles[l], vec3(1.0f, 0.0f, 0.0f));
            }
            leg = translate(leg, vec3(0.0f, -0.35f, 0.0f));
            mat4 legCyl = scale(leg, vec3(0.09f, 0.80f, 0.09f));
            Primitives::drawCylinder(shader, legCyl, hideCol);

            mat4 hoof = leg;
            hoof = translate(hoof, vec3(0.0f, -0.38f, 0.0f));
            hoof = scale(hoof, vec3(0.10f, 0.08f, 0.12f));
            Primitives::drawCube(shader, hoof, hoofCol);
        }
    }

    // 6. Tail draped over rear flank (-Z)
    mat4 tail = m;
    tail = translate(tail, vec3(0.0f, lyingDown ? 0.26f : 0.80f, -0.34f));
    tail = rotate(tail, radians(-28.0f), vec3(1.0f, 0.0f, 0.0f));
    mat4 tailStem = scale(tail, vec3(0.022f, 0.38f, 0.022f));
    Primitives::drawCylinder(shader, tailStem, hideCol);

    mat4 tailTuft = tail;
    tailTuft = translate(tailTuft, vec3(0.0f, -0.20f, 0.0f));
    tailTuft = scale(tailTuft, vec3(0.045f, 0.11f, 0.045f));
    Primitives::drawSphere(shader, tailTuft, muzzleCol);
}

// ─── 7. Traditional Bangladeshi Tubewell (Chapa Kol / টিউবওয়েল) ──────
void drawTubewell(Shader& shader, const mat4& model, float pumpAngle, bool isPumping)
{
    vec3 concreteCol(0.66f, 0.66f, 0.64f); // raised concrete platform
    vec3 wetPadCol  (0.50f, 0.52f, 0.50f); // wet concrete spot under spout
    vec3 ironGreen  (0.14f, 0.38f, 0.20f); // classic British-racing rural green cast iron
    vec3 darkIron   (0.18f, 0.20f, 0.18f); // iron bolts, handle, plunger rod
    vec3 drainCol   (0.52f, 0.48f, 0.42f); // drainage channel
    vec3 waterStream(0.65f, 0.85f, 0.95f); // flowing fresh groundwater stream

    mat4 m = model;

    // 1. Raised Concrete Washing Platform (Pacca Tala)
    mat4 pad = m;
    pad = translate(pad, vec3(0.0f, 0.06f, 0.0f));
    pad = scale(pad, vec3(1.60f, 0.12f, 1.60f));
    Primitives::drawCube(shader, pad, concreteCol);

    // Raised safety curb lips
    mat4 rimN = m; rimN = translate(rimN, vec3(0.0f, 0.14f, -0.76f)); rimN = scale(rimN, vec3(1.60f, 0.06f, 0.08f));
    Primitives::drawCube(shader, rimN, concreteCol);
    mat4 rimE = m; rimE = translate(rimE, vec3(0.76f, 0.14f, 0.0f)); rimE = scale(rimE, vec3(0.08f, 0.06f, 1.60f));
    Primitives::drawCube(shader, rimE, concreteCol);
    mat4 rimW = m; rimW = translate(rimW, vec3(-0.76f, 0.14f, 0.0f)); rimW = scale(rimW, vec3(0.08f, 0.06f, 1.60f));
    Primitives::drawCube(shader, rimW, concreteCol);

    // Wet water puddle circle on concrete
    mat4 wetSpot = m;
    wetSpot = translate(wetSpot, vec3(0.0f, 0.122f, 0.28f));
    wetSpot = scale(wetSpot, vec3(0.65f, 0.005f, 0.65f));
    Primitives::drawCylinder(shader, wetSpot, wetPadCol);

    // 2. Concrete Drainage Trough (Pani-r Nala)
    mat4 drain = m;
    drain = translate(drain, vec3(0.0f, 0.05f, 1.35f));
    drain = scale(drain, vec3(0.35f, 0.08f, 1.10f));
    Primitives::drawCube(shader, drain, drainCol);

    // 3. Cast Iron Base Flange
    mat4 baseFlange = m;
    baseFlange = translate(baseFlange, vec3(0.0f, 0.15f, 0.0f));
    baseFlange = scale(baseFlange, vec3(0.26f, 0.06f, 0.26f));
    Primitives::drawCylinder(shader, baseFlange, ironGreen);

    // 4. Main Pump Cylinder Barrel
    mat4 barrel = m;
    barrel = translate(barrel, vec3(0.0f, 0.55f, 0.0f));
    barrel = scale(barrel, vec3(0.12f, 0.75f, 0.12f));
    Primitives::drawCylinder(shader, barrel, ironGreen);

    // Middle reinforcing ring
    mat4 midRing = m;
    midRing = translate(midRing, vec3(0.0f, 0.55f, 0.0f));
    midRing = scale(midRing, vec3(0.145f, 0.05f, 0.145f));
    Primitives::drawCylinder(shader, midRing, darkIron);

    // 5. Water Spout (Mukhi) pointing forward over platform
    mat4 spout = m;
    spout = translate(spout, vec3(0.0f, 0.48f, 0.18f));
    spout = rotate(spout, radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
    mat4 spoutCyl = scale(spout, vec3(0.045f, 0.28f, 0.045f));
    Primitives::drawCylinder(shader, spoutCyl, ironGreen);

    mat4 spoutLip = m;
    spoutLip = translate(spoutLip, vec3(0.0f, 0.44f, 0.31f));
    spoutLip = scale(spoutLip, vec3(0.055f, 0.06f, 0.055f));
    Primitives::drawCylinder(shader, spoutLip, darkIron);

    // 6. Pump Head (Top Chamber)
    mat4 pumpHead = m;
    pumpHead = translate(pumpHead, vec3(0.0f, 0.96f, 0.0f));
    pumpHead = scale(pumpHead, vec3(0.16f, 0.15f, 0.16f));
    Primitives::drawCylinder(shader, pumpHead, ironGreen);

    mat4 topCap = m;
    topCap = translate(topCap, vec3(0.0f, 1.05f, 0.0f));
    topCap = scale(topCap, vec3(0.18f, 0.04f, 0.18f));
    Primitives::drawCylinder(shader, topCap, darkIron);

    // 7. Plunger Rod entering top (reciprocates vertically with pumping)
    mat4 rod = m;
    rod = translate(rod, vec3(0.0f, 1.15f - pumpAngle * 0.08f, 0.0f));
    rod = scale(rod, vec3(0.02f, 0.20f, 0.02f));
    Primitives::drawCylinder(shader, rod, darkIron);

    // 8. Long Curved Cast Iron Pump Handle (Hatol - pivots with pumpAngle)
    mat4 handlePivot = m;
    handlePivot = translate(handlePivot, vec3(0.0f, 0.95f, -0.06f));
    mat4 bracket = scale(handlePivot, vec3(0.06f, 0.08f, 0.10f));
    Primitives::drawCube(shader, bracket, darkIron);

    mat4 handle = handlePivot;
    handle = translate(handle, vec3(0.0f, 0.02f, -0.42f));
    handle = rotate(handle, radians(-20.0f + pumpAngle * 28.0f), vec3(1.0f, 0.0f, 0.0f));
    mat4 handleBar = scale(handle, vec3(0.035f, 0.035f, 0.85f));
    Primitives::drawCylinder(shader, handleBar, ironGreen);

    mat4 handleBall = handle;
    handleBall = translate(handleBall, vec3(0.0f, 0.0f, -0.44f));
    handleBall = scale(handleBall, vec3(0.075f, 0.075f, 0.075f));
    Primitives::drawSphere(shader, handleBall, darkIron);

    // 9. Interactive Flowing Water Stream & Splash
    if (isPumping) {
        mat4 stream = m;
        stream = translate(stream, vec3(0.0f, 0.28f, 0.32f));
        stream = scale(stream, vec3(0.038f, 0.26f, 0.038f));
        Primitives::drawCylinder(shader, stream, waterStream);

        mat4 splash = m;
        splash = translate(splash, vec3(0.0f, 0.40f, 0.32f));
        splash = scale(splash, vec3(0.14f, 0.015f, 0.14f));
        Primitives::drawCylinder(shader, splash, vec3(0.85f, 0.92f, 0.98f));
    }

    // 10. Traditional Clay Water Pitcher (Kolshi) placed under spout
    drawKolshi(shader, m, vec3(0.0f, 0.12f, 0.32f), 0.85f);
}

// Traditional Rural Chicken Coop (Murgir Khopa / মোরগের খোঁপা)
// Elevated on bamboo stilts; closed and latched at night to protect poultry from predators;
// door opened as an entrance ramp during daytime.
void drawChickenCoop(Shader& shader, const mat4& model, bool isNight)
{
    vec3 stiltCol    (0.42f, 0.30f, 0.16f); // dark bamboo posts
    vec3 floorCol    (0.36f, 0.24f, 0.12f); // weathered wood floor
    vec3 wallSlatCol (0.54f, 0.42f, 0.22f); // bamboo split slats
    vec3 roofTinCol  (0.48f, 0.52f, 0.56f); // corrugated metal tin roof
    vec3 latchCol    (0.18f, 0.18f, 0.18f); // iron latch / lock pin
    vec3 interiorCol (0.22f, 0.14f, 0.08f); // dark roosting interior

    const float coopW = 0.95f;
    const float coopD = 0.75f;
    const float floorY = 0.36f;
    const float coopH = 0.55f;

    // 1. Four bamboo stilts (elevating the coop off the ground)
    float stiltCorners[4][2] = {
        { -coopW * 0.45f, -coopD * 0.45f },
        {  coopW * 0.45f, -coopD * 0.45f },
        { -coopW * 0.45f,  coopD * 0.45f },
        {  coopW * 0.45f,  coopD * 0.45f }
    };
    for (int i = 0; i < 4; ++i) {
        mat4 stilt = model;
        stilt = translate(stilt, vec3(stiltCorners[i][0], floorY * 0.5f, stiltCorners[i][1]));
        stilt = scale(stilt, vec3(0.045f, floorY, 0.045f));
        Primitives::drawCylinder(shader, stilt, stiltCol);
    }

    // 2. Heavy timber floor platform
    mat4 floorPlank = model;
    floorPlank = translate(floorPlank, vec3(0.0f, floorY, 0.0f));
    floorPlank = scale(floorPlank, vec3(coopW, 0.05f, coopD));
    Primitives::drawCube(shader, floorPlank, floorCol);

    // 3. Bamboo slatted walls (Left, Right, and Back)
    // Back wall
    mat4 backWall = model;
    backWall = translate(backWall, vec3(0.0f, floorY + coopH * 0.5f, -coopD * 0.48f));
    backWall = scale(backWall, vec3(coopW * 0.96f, coopH, 0.035f));
    Primitives::drawCube(shader, backWall, wallSlatCol);

    // Left wall
    mat4 leftWall = model;
    leftWall = translate(leftWall, vec3(-coopW * 0.48f, floorY + coopH * 0.5f, 0.0f));
    leftWall = scale(leftWall, vec3(0.035f, coopH, coopD * 0.94f));
    Primitives::drawCube(shader, leftWall, wallSlatCol);

    // Right wall
    mat4 rightWall = model;
    rightWall = translate(rightWall, vec3(coopW * 0.48f, floorY + coopH * 0.5f, 0.0f));
    rightWall = scale(rightWall, vec3(0.035f, coopH, coopD * 0.94f));
    Primitives::drawCube(shader, rightWall, wallSlatCol);

    // Slatted front wall with center doorway opening
    float doorW = 0.36f;
    float doorH = 0.42f;
    float sideW = (coopW - doorW) * 0.5f;

    // Left front panel
    mat4 fLeft = model;
    fLeft = translate(fLeft, vec3(-coopW * 0.5f + sideW * 0.5f, floorY + coopH * 0.5f, coopD * 0.48f));
    fLeft = scale(fLeft, vec3(sideW, coopH, 0.035f));
    Primitives::drawCube(shader, fLeft, wallSlatCol);

    // Right front panel
    mat4 fRight = model;
    fRight = translate(fRight, vec3(coopW * 0.5f - sideW * 0.5f, floorY + coopH * 0.5f, coopD * 0.48f));
    fRight = scale(fRight, vec3(sideW, coopH, 0.035f));
    Primitives::drawCube(shader, fRight, wallSlatCol);

    // Lintel above door
    mat4 fLintel = model;
    fLintel = translate(fLintel, vec3(0.0f, floorY + doorH + (coopH - doorH) * 0.5f, coopD * 0.48f));
    fLintel = scale(fLintel, vec3(doorW, coopH - doorH, 0.035f));
    Primitives::drawCube(shader, fLintel, wallSlatCol);

    // 4. Slanted tin / thatched roof overhang
    mat4 roof = model;
    roof = translate(roof, vec3(0.0f, floorY + coopH + 0.04f, 0.0f));
    roof = rotate(roof, radians(-6.0f), vec3(1.0f, 0.0f, 0.0f));
    roof = scale(roof, vec3(coopW + 0.18f, 0.04f, coopD + 0.20f));
    Primitives::drawCube(shader, roof, roofTinCol);

    // Roosting perches inside chicken coop
    mat4 perchLow = model;
    perchLow = translate(perchLow, vec3(0.0f, floorY + 0.06f, -0.16f));
    perchLow = scale(perchLow, vec3(coopW * 0.88f, 0.025f, 0.025f));
    Primitives::drawCube(shader, perchLow, stiltCol);

    mat4 perchHigh = model;
    perchHigh = translate(perchHigh, vec3(0.0f, floorY + 0.18f, 0.06f));
    perchHigh = scale(perchHigh, vec3(coopW * 0.88f, 0.025f, 0.025f));
    Primitives::drawCube(shader, perchHigh, stiltCol);

    // Warm straw bedding on floor
    mat4 strawBed = model;
    strawBed = translate(strawBed, vec3(0.0f, floorY + 0.026f, 0.0f));
    strawBed = scale(strawBed, vec3(coopW * 0.86f, 0.015f, coopD * 0.86f));
    Primitives::drawCube(shader, strawBed, vec3(0.72f, 0.58f, 0.24f));

    // 5. Door / Ramp behavior:
    if (isNight) {
        // At night: door has slatted bamboo grating with wooden security latch bar
        // 4 vertical slats for ventilation so roosting hens are visible inside
        float slatW = (doorW + 0.02f) / 4.0f;
        for (int s = 0; s < 4; ++s) {
            float sx = -doorW * 0.5f + ((float)s + 0.5f) * slatW;
            mat4 slat = model;
            slat = translate(slat, vec3(sx, floorY + doorH * 0.5f, coopD * 0.485f));
            slat = scale(slat, vec3(slatW * 0.72f, doorH, 0.028f));
            Primitives::drawCube(shader, slat, floorCol);
        }

        // Horizontal wooden latch bar
        mat4 latch = model;
        latch = translate(latch, vec3(0.0f, floorY + doorH * 0.52f, coopD * 0.505f));
        latch = scale(latch, vec3(doorW * 0.75f, 0.04f, 0.025f));
        Primitives::drawCube(shader, latch, latchCol);
    } else {
        // By day: door is hinged downward as a walking ramp to the yard
        mat4 ramp = model;
        ramp = translate(ramp, vec3(0.0f, floorY * 0.5f, coopD * 0.48f + 0.22f));
        ramp = rotate(ramp, radians(42.0f), vec3(1.0f, 0.0f, 0.0f));
        ramp = scale(ramp, vec3(doorW * 0.88f, 0.025f, 0.52f));
        Primitives::drawCube(shader, ramp, floorCol);

        // Dark interior roosting shadow inside open door
        mat4 interior = model;
        interior = translate(interior, vec3(0.0f, floorY + doorH * 0.5f, coopD * 0.44f));
        interior = scale(interior, vec3(doorW * 0.90f, doorH * 0.90f, 0.02f));
        Primitives::drawCube(shader, interior, interiorCol);
    }
}

// Traditional Rural Duck House (Hash-er Ghor / হাঁসের ঘর)
// Elevated on bamboo stilts near the riverbank; safe nightly shelter for the flock.
void drawDuckHouse(Shader& shader, const mat4& model, bool isNight)
{
    vec3 stiltCol    (0.40f, 0.28f, 0.15f); // bamboo stilts
    vec3 floorCol    (0.34f, 0.22f, 0.11f); // timber floorboards
    vec3 slatCol     (0.52f, 0.40f, 0.22f); // split bamboo walls
    vec3 roofCol     (0.45f, 0.48f, 0.52f); // corrugated tin roof
    vec3 latchCol    (0.18f, 0.18f, 0.18f); // iron latch
    vec3 strawCol    (0.72f, 0.58f, 0.24f); // dry golden straw bedding
    vec3 interiorCol (0.22f, 0.14f, 0.08f); // dark interior shadow

    const float houseW = 1.25f;
    const float houseD = 0.95f;
    const float floorY = 0.34f;
    const float houseH = 0.55f;

    // 1. Four bamboo stilts (elevating duck house above riverbank mud/water)
    float stiltCorners[4][2] = {
        { -houseW * 0.44f, -houseD * 0.44f },
        {  houseW * 0.44f, -houseD * 0.44f },
        { -houseW * 0.44f,  houseD * 0.44f },
        {  houseW * 0.44f,  houseD * 0.44f }
    };
    for (int i = 0; i < 4; ++i) {
        mat4 stilt = model;
        stilt = translate(stilt, vec3(stiltCorners[i][0], floorY * 0.5f, stiltCorners[i][1]));
        stilt = scale(stilt, vec3(0.048f, floorY, 0.048f));
        Primitives::drawCube(shader, stilt, stiltCol);
    }

    // 2. Heavy timber floor platform
    mat4 floorPlank = model;
    floorPlank = translate(floorPlank, vec3(0.0f, floorY, 0.0f));
    floorPlank = scale(floorPlank, vec3(houseW, 0.045f, houseD));
    Primitives::drawCube(shader, floorPlank, floorCol);

    // Warm golden straw bedding on floor
    mat4 strawBed = model;
    strawBed = translate(strawBed, vec3(0.0f, floorY + 0.025f, 0.0f));
    strawBed = scale(strawBed, vec3(houseW * 0.88f, 0.02f, houseD * 0.88f));
    Primitives::drawCube(shader, strawBed, strawCol);

    // 3. Slatted bamboo walls (Left, Right, Back)
    // Back wall
    mat4 backWall = model;
    backWall = translate(backWall, vec3(0.0f, floorY + houseH * 0.5f, -houseD * 0.48f));
    backWall = scale(backWall, vec3(houseW * 0.96f, houseH, 0.035f));
    Primitives::drawCube(shader, backWall, slatCol);

    // Left wall
    mat4 leftWall = model;
    leftWall = translate(leftWall, vec3(-houseW * 0.48f, floorY + houseH * 0.5f, 0.0f));
    leftWall = scale(leftWall, vec3(0.035f, houseH, houseD * 0.94f));
    Primitives::drawCube(shader, leftWall, slatCol);

    // Right wall
    mat4 rightWall = model;
    rightWall = translate(rightWall, vec3(houseW * 0.48f, floorY + houseH * 0.5f, 0.0f));
    rightWall = scale(rightWall, vec3(0.035f, houseH, houseD * 0.94f));
    Primitives::drawCube(shader, rightWall, slatCol);

    // Front wall with wide duck door opening
    float doorW = 0.52f;
    float doorH = 0.42f;
    float sideW = (houseW - doorW) * 0.5f;

    // Left front panel
    mat4 fLeft = model;
    fLeft = translate(fLeft, vec3(-houseW * 0.5f + sideW * 0.5f, floorY + houseH * 0.5f, houseD * 0.48f));
    fLeft = scale(fLeft, vec3(sideW, houseH, 0.035f));
    Primitives::drawCube(shader, fLeft, slatCol);

    // Right front panel
    mat4 fRight = model;
    fRight = translate(fRight, vec3(houseW * 0.5f - sideW * 0.5f, floorY + houseH * 0.5f, houseD * 0.48f));
    fRight = scale(fRight, vec3(sideW, houseH, 0.035f));
    Primitives::drawCube(shader, fRight, slatCol);

    // Lintel above door
    mat4 fLintel = model;
    fLintel = translate(fLintel, vec3(0.0f, floorY + doorH + (houseH - doorH) * 0.5f, houseD * 0.48f));
    fLintel = scale(fLintel, vec3(doorW, houseH - doorH, 0.035f));
    Primitives::drawCube(shader, fLintel, slatCol);

    // 4. Slanted tin roof overhang
    mat4 roof = model;
    roof = translate(roof, vec3(0.0f, floorY + houseH + 0.04f, 0.0f));
    roof = rotate(roof, radians(-6.0f), vec3(1.0f, 0.0f, 0.0f));
    roof = scale(roof, vec3(houseW + 0.22f, 0.04f, houseD + 0.22f));
    Primitives::drawCube(shader, roof, roofCol);

    // 5. Door & Ramp Behavior
    if (isNight) {
        // At night: door has slatted bamboo grating with wooden latch bar
        // 5 vertical slats for ventilation so ducks are visible nestled inside
        float slatW = (doorW + 0.02f) / 5.0f;
        for (int s = 0; s < 5; ++s) {
            float sx = -doorW * 0.5f + ((float)s + 0.5f) * slatW;
            mat4 slat = model;
            slat = translate(slat, vec3(sx, floorY + doorH * 0.5f, houseD * 0.485f));
            slat = scale(slat, vec3(slatW * 0.70f, doorH, 0.028f));
            Primitives::drawCube(shader, slat, floorCol);
        }

        // Horizontal latch bar
        mat4 latch = model;
        latch = translate(latch, vec3(0.0f, floorY + doorH * 0.52f, houseD * 0.505f));
        latch = scale(latch, vec3(doorW * 0.78f, 0.038f, 0.025f));
        Primitives::drawCube(shader, latch, latchCol);
    } else {
        // By day: ramp lowered down to the grass/riverbank
        mat4 ramp = model;
        ramp = translate(ramp, vec3(0.0f, floorY * 0.5f, houseD * 0.48f + 0.22f));
        ramp = rotate(ramp, radians(38.0f), vec3(1.0f, 0.0f, 0.0f));
        ramp = scale(ramp, vec3(doorW * 0.88f, 0.025f, 0.52f));
        Primitives::drawCube(shader, ramp, floorCol);

        // Dark interior shadow
        mat4 interior = model;
        interior = translate(interior, vec3(0.0f, floorY + doorH * 0.5f, houseD * 0.44f));
        interior = scale(interior, vec3(doorW * 0.90f, doorH * 0.90f, 0.02f));
        Primitives::drawCube(shader, interior, interiorCol);
    }
}

// Traditional Rural Bangladeshi Paddy Granary (Dhaner Gola / ধানের গোলা)
// Elevated cylindrical bamboo-weave storehouse with steep conical thatched roof
void drawGranary(Shader& shader, const mat4& model)
{
    shader.setInt("uUseTexture", 0);
    vec3 stiltCol  (0.38f, 0.26f, 0.14f); // timber / stone pillar supports
    vec3 floorCol  (0.48f, 0.36f, 0.20f); // bamboo floor deck
    vec3 bodyCol   (0.66f, 0.54f, 0.30f); // clay-plastered bamboo weave
    vec3 hoopCol   (0.36f, 0.24f, 0.12f); // bamboo binding rings
    vec3 roofStraw (0.70f, 0.58f, 0.26f); // golden rice straw conical thatch
    vec3 ladderCol (0.45f, 0.32f, 0.18f); // bamboo ladder

    float floorY = 0.55f;
    float bodyR = 0.88f;
    float bodyH = 1.35f;

    // 4 sturdy timber stilt posts elevating granary above moisture & rats
    float postOffsets[4][2] = { {-0.55f, -0.55f}, {0.55f, -0.55f}, {-0.55f, 0.55f}, {0.55f, 0.55f} };
    for (int i = 0; i < 4; ++i) {
        mat4 stilt = model;
        stilt = translate(stilt, vec3(postOffsets[i][0], floorY * 0.5f, postOffsets[i][1]));
        stilt = scale(stilt, vec3(0.09f, floorY, 0.09f));
        Primitives::drawCube(shader, stilt, stiltCol);
    }

    // Circular base platform
    mat4 platform = model;
    platform = translate(platform, vec3(0.0f, floorY, 0.0f));
    platform = scale(platform, vec3(bodyR * 2.15f, 0.07f, bodyR * 2.15f));
    Primitives::drawCylinder(shader, platform, floorCol);

    // Cylindrical woven bamboo granary body (Dhaner Gola)
    mat4 body = model;
    body = translate(body, vec3(0.0f, floorY + bodyH * 0.5f, 0.0f));
    body = scale(body, vec3(bodyR * 2.0f, bodyH, bodyR * 2.0f));
    Primitives::drawCylinder(shader, body, bodyCol);

    // 3 Bamboo strengthening hoop rings around cylinder
    float hoopY[3] = { floorY + 0.25f, floorY + bodyH * 0.5f, floorY + bodyH - 0.15f };
    for (int h = 0; h < 3; ++h) {
        mat4 hoop = model;
        hoop = translate(hoop, vec3(0.0f, hoopY[h], 0.0f));
        hoop = scale(hoop, vec3(bodyR * 2.04f, 0.045f, bodyR * 2.04f));
        Primitives::drawCylinder(shader, hoop, hoopCol);
    }

    // Steep conical thatched roof (Chhoner Mathal)
    float roofH = 1.25f;
    float roofR = bodyR * 1.35f;
    mat4 roof = model;
    roof = translate(roof, vec3(0.0f, floorY + bodyH, 0.0f));
    roof = scale(roof, vec3(roofR * 2.0f, roofH, roofR * 2.0f));
    Primitives::drawCone(shader, roof, roofStraw);

    // Top bamboo apex finial cap
    mat4 cap = model;
    cap = translate(cap, vec3(0.0f, floorY + bodyH + roofH + 0.12f, 0.0f));
    cap = scale(cap, vec3(0.08f, 0.25f, 0.08f));
    Primitives::drawCone(shader, cap, hoopCol);

    // Leaning bamboo ladder leading up to platform
    mat4 ladder = model;
    ladder = translate(ladder, vec3(0.0f, floorY * 0.5f, bodyR + 0.25f));
    ladder = rotate(ladder, radians(-25.0f), vec3(1.0f, 0.0f, 0.0f));
    // Rails
    for (float lx : {-0.18f, 0.18f}) {
        mat4 rail = ladder;
        rail = translate(rail, vec3(lx, 0.0f, 0.0f));
        rail = scale(rail, vec3(0.035f, floorY * 1.3f, 0.035f));
        Primitives::drawCylinder(shader, rail, ladderCol);
    }
    // Rungs
    for (float ry = -floorY * 0.4f; ry <= floorY * 0.45f; ry += 0.22f) {
        mat4 rung = ladder;
        rung = translate(rung, vec3(0.0f, ry, 0.0f));
        rung = rotate(rung, radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
        rung = scale(rung, vec3(0.025f, 0.36f, 0.025f));
        Primitives::drawCylinder(shader, rung, ladderCol);
    }
}

// Traditional Rural Vegetable Trellis (Lau / Kumra Macha / সবজির মাচা)
void drawVegetableTrellis(Shader& shader, const mat4& model)
{
    shader.setInt("uUseTexture", 0);
    vec3 bambooCol (0.50f, 0.38f, 0.20f); // weathered bamboo posts
    vec3 vineCol   (0.24f, 0.50f, 0.15f); // lush green climbing leaves (Lau Shak)
    vec3 leafCol2  (0.20f, 0.42f, 0.12f); // deep green foliage
    vec3 gourdCol  (0.68f, 0.82f, 0.45f); // pale green bottle gourds (Lau)
    vec3 stemCol   (0.35f, 0.55f, 0.20f); // tender gourd stem

    float tw = 2.4f;
    float td = 1.8f;
    float th = 1.35f;

    // 4 upright bamboo corner posts
    float corners[4][2] = { {-tw*0.5f, -td*0.5f}, {tw*0.5f, -td*0.5f}, {-tw*0.5f, td*0.5f}, {tw*0.5f, td*0.5f} };
    for (int i = 0; i < 4; ++i) {
        mat4 post = model;
        post = translate(post, vec3(corners[i][0], th * 0.5f, corners[i][1]));
        post = scale(post, vec3(0.045f, th, 0.045f));
        Primitives::drawCylinder(shader, post, bambooCol);
    }

    // Top horizontal bamboo frame (X rails and Z rails)
    for (float zside : {-td*0.5f, td*0.5f}) {
        mat4 railX = model;
        railX = translate(railX, vec3(0.0f, th, zside));
        railX = scale(railX, vec3(tw + 0.15f, 0.04f, 0.04f));
        Primitives::drawCube(shader, railX, bambooCol);
    }
    for (float xside : {-tw*0.5f, tw*0.5f}) {
        mat4 railZ = model;
        railZ = translate(railZ, vec3(xside, th, 0.0f));
        railZ = scale(railZ, vec3(0.04f, 0.04f, td + 0.15f));
        Primitives::drawCube(shader, railZ, bambooCol);
    }

    // Cross-battens forming the bamboo trellis deck
    for (float bx = -tw * 0.35f; bx <= tw * 0.35f; bx += 0.35f) {
        mat4 bat = model;
        bat = translate(bat, vec3(bx, th + 0.02f, 0.0f));
        bat = scale(bat, vec3(0.03f, 0.025f, td));
        Primitives::drawCube(shader, bat, bambooCol);
    }
    for (float bz = -td * 0.35f; bz <= td * 0.35f; bz += 0.35f) {
        mat4 bat = model;
        bat = translate(bat, vec3(0.0f, th + 0.035f, bz));
        bat = scale(bat, vec3(tw, 0.025f, 0.03f));
        Primitives::drawCube(shader, bat, bambooCol);
    }

    // Lush green leafy vine canopy covering top of trellis
    float foliagePuffs[5][3] = {
        { -0.6f, th + 0.08f, -0.3f },
        {  0.5f, th + 0.09f, -0.4f },
        {  0.0f, th + 0.10f,  0.2f },
        { -0.5f, th + 0.08f,  0.4f },
        {  0.6f, th + 0.07f,  0.3f }
    };
    for (int p = 0; p < 5; ++p) {
        mat4 fol = model;
        fol = translate(fol, vec3(foliagePuffs[p][0], foliagePuffs[p][1], foliagePuffs[p][2]));
        fol = scale(fol, vec3(0.85f, 0.12f, 0.75f));
        Primitives::drawSphere(shader, fol, (p % 2 == 0) ? vineCol : leafCol2);
    }

    // Hanging bottle gourds (Lau) dangling beneath the trellis
    float gourdPositions[3][2] = {
        { -0.45f, -0.15f },
        {  0.35f,  0.25f },
        { -0.10f,  0.35f }
    };
    for (int g = 0; g < 3; ++g) {
        float gx = gourdPositions[g][0];
        float gz = gourdPositions[g][1];
        // Stem
        mat4 stm = model;
        stm = translate(stm, vec3(gx, th - 0.08f, gz));
        stm = scale(stm, vec3(0.015f, 0.16f, 0.015f));
        Primitives::drawCylinder(shader, stm, stemCol);

        // Pear-shaped bottle gourd (top narrow sphere + bottom bulbous sphere)
        mat4 topSphere = model;
        topSphere = translate(topSphere, vec3(gx, th - 0.22f, gz));
        topSphere = scale(topSphere, vec3(0.11f, 0.14f, 0.11f));
        Primitives::drawSphere(shader, topSphere, gourdCol);

        mat4 botSphere = model;
        botSphere = translate(botSphere, vec3(gx, th - 0.36f, gz));
        botSphere = scale(botSphere, vec3(0.16f, 0.20f, 0.16f));
        Primitives::drawSphere(shader, botSphere, gourdCol);
    }
}

// Traditional Rural Fishing Net Drying Rack (Jal Shukabor Macha / মাছের জাল)
void drawNetRack(Shader& shader, const mat4& model)
{
    shader.setInt("uUseTexture", 0);
    vec3 postCol (0.42f, 0.30f, 0.16f); // weathered bamboo
    vec3 netCol  (0.52f, 0.44f, 0.32f); // tan jute fishing net mesh (Jal)

    float rackL = 3.6f;
    float rackH = 1.35f;

    // Two A-frame crossed bamboo trestles at +X and -X ends
    for (float xside : {-rackL * 0.48f, rackL * 0.48f}) {
        for (int leg = -1; leg <= 1; leg += 2) {
            mat4 pole = model;
            pole = translate(pole, vec3(xside, rackH * 0.5f, (float)leg * 0.28f));
            pole = rotate(pole, radians((float)leg * -18.0f), vec3(1.0f, 0.0f, 0.0f));
            pole = scale(pole, vec3(0.045f, rackH * 1.08f, 0.045f));
            Primitives::drawCylinder(shader, pole, postCol);
        }
    }

    // Top horizontal drying spar connecting the trestles
    mat4 spar = model;
    spar = translate(spar, vec3(0.0f, rackH, 0.0f));
    spar = scale(spar, vec3(rackL + 0.3f, 0.045f, 0.045f));
    Primitives::drawCube(shader, spar, postCol);

    // Draped fishing net draped over the spar
    mat4 netFront = model;
    netFront = translate(netFront, vec3(0.0f, rackH * 0.52f, 0.14f));
    netFront = rotate(netFront, radians(12.0f), vec3(1.0f, 0.0f, 0.0f));
    netFront = scale(netFront, vec3(rackL * 0.88f, rackH * 0.85f, 0.02f));
    Primitives::drawCube(shader, netFront, netCol);

    mat4 netBack = model;
    netBack = translate(netBack, vec3(0.0f, rackH * 0.52f, -0.14f));
    netBack = rotate(netBack, radians(-12.0f), vec3(1.0f, 0.0f, 0.0f));
    netBack = scale(netBack, vec3(rackL * 0.88f, rackH * 0.85f, 0.02f));
    Primitives::drawCube(shader, netBack, netCol * 0.92f);
}

// ─── Traditional Rural Bangladeshi Bullock Cart (Gorur Gari / গরুর গাড়ি) ───
void drawBullockCart(Shader& shader, const mat4& model, float wheelRotation, float walkPhase)
{
    shader.setInt("uUseTexture", 0);

    const vec3 woodDark   (0.40f, 0.26f, 0.13f); // weathered sal timber axle & frame
    const vec3 woodLight  (0.56f, 0.42f, 0.24f); // spokes, floor planks, draft tongue
    const vec3 ironCol    (0.22f, 0.20f, 0.18f); // iron wheel tyre & hub banding
    const vec3 chhoiCol   (0.74f, 0.60f, 0.34f); // woven bamboo canopy hood (Chhoi / চাটাই)
    const vec3 hoopCol    (0.48f, 0.36f, 0.18f); // arched bamboo structural hoops
    const vec3 strawCol   (0.86f, 0.74f, 0.30f); // golden rice straw cargo
    const vec3 sackCol    (0.60f, 0.48f, 0.30f); // jute burlap cargo sacks (পাটের বস্তা)
    const vec3 ropeCol    (0.66f, 0.54f, 0.34f); // jute coir hitch ropes
    const vec3 whiteOxCol (0.82f, 0.78f, 0.72f); // light grey/cream coat for right ox

    const float wheelR = 0.72f; // Large wooden cart wheel radius
    const float axleY  = 0.72f; // Axle height matches wheel radius

    // ── 1. The Main Axle & Under-Chassis Beam ────────────────────
    mat4 axle = model;
    axle = translate(axle, vec3(0.0f, axleY, 0.0f));
    axle = scale(axle, vec3(1.88f, 0.085f, 0.085f));
    Primitives::drawCube(shader, axle, woodDark);

    // ── 2. Two Large Wooden Spoked Wheels (কাঠের চাকা) ─────────
    for (float side : { -0.88f, 0.88f }) {
        mat4 wm = model;
        wm = translate(wm, vec3(side, axleY, 0.0f));
        wm = rotate(wm, wheelRotation, vec3(1.0f, 0.0f, 0.0f)); // roll with motion

        // Central Wooden Hub (Nave / নাভি)
        mat4 hub = wm;
        hub = rotate(hub, radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
        hub = scale(hub, vec3(0.12f, 0.20f, 0.12f));
        Primitives::drawCylinder(shader, hub, woodDark);

        // Iron hub reinforcement rings
        for (float hx : { -0.09f, 0.09f }) {
            mat4 hRing = wm;
            hRing = translate(hRing, vec3(hx, 0.0f, 0.0f));
            hRing = rotate(hRing, radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
            hRing = scale(hRing, vec3(0.125f, 0.025f, 0.125f));
            Primitives::drawCylinder(shader, hRing, ironCol);
        }

        // Axle end cap & linchpin (খিল)
        mat4 pin = wm;
        pin = translate(pin, vec3(side > 0 ? 0.12f : -0.12f, 0.0f, 0.0f));
        pin = scale(pin, vec3(0.025f, 0.16f, 0.025f));
        Primitives::drawCylinder(shader, pin, ironCol);

        // 12 Wooden Radial Spokes (১২টি কাঠের অর)
        for (int sp = 0; sp < 12; ++sp) {
            float ang = (float)sp * (2.0f * PI / 12.0f);
            mat4 spoke = wm;
            spoke = rotate(spoke, ang, vec3(1.0f, 0.0f, 0.0f));
            spoke = translate(spoke, vec3(0.0f, wheelR * 0.50f, 0.0f));
            spoke = scale(spoke, vec3(0.038f, wheelR * 0.82f, 0.038f));
            Primitives::drawCylinder(shader, spoke, woodLight);

            // Auspicious folk cloth ribbon marker tied to spoke 0 (লাল শালু কাপড় / ফিতা)
            // Visually indicates exact 180° rotation on Key 'G' (flips between top and bottom!)
            if (sp == 0) {
                mat4 ribbon = wm;
                ribbon = rotate(ribbon, ang, vec3(1.0f, 0.0f, 0.0f));
                ribbon = translate(ribbon, vec3(0.0f, wheelR * 0.58f, 0.0f));
                ribbon = scale(ribbon, vec3(0.048f, 0.080f, 0.048f));
                Primitives::drawCube(shader, ribbon, vec3(0.85f, 0.18f, 0.12f));
            }
        }

        // 16-Segment Outer Wooden Rim & Iron Tyre (লোহার বেড়)
        const int numRimSegs = 16;
        const float segAngle = 2.0f * PI / (float)numRimSegs;
        const float segLen   = 2.0f * wheelR * sinf(segAngle * 0.5f);

        for (int r = 0; r < numRimSegs; ++r) {
            float midAng = ((float)r + 0.5f) * segAngle;
            mat4 rimSeg = wm;
            rimSeg = rotate(rimSeg, midAng, vec3(1.0f, 0.0f, 0.0f));
            rimSeg = translate(rimSeg, vec3(0.0f, wheelR, 0.0f));
            // Wooden felloe segment
            mat4 felloe = rimSeg;
            felloe = scale(felloe, vec3(0.065f, 0.048f, segLen * 1.04f));
            Primitives::drawCube(shader, felloe, woodDark);

            // Iron outer tyre band
            mat4 tyre = rimSeg;
            tyre = translate(tyre, vec3(0.0f, 0.024f, 0.0f));
            tyre = scale(tyre, vec3(0.070f, 0.012f, segLen * 1.05f));
            Primitives::drawCube(shader, tyre, ironCol);
        }
    }

    // ── 3. Cart Bed & Floor Platform (মাচা / পাটাতন) ─────────────
    const float bedW = 1.20f;
    const float bedL = 2.40f;
    const float bedY = axleY + 0.07f; // Y ≈ 0.79m

    // Longitudinal chassis stringers
    for (float sx : { -bedW * 0.48f, bedW * 0.48f }) {
        mat4 stringer = model;
        stringer = translate(stringer, vec3(sx, bedY, -0.20f));
        stringer = scale(stringer, vec3(0.075f, 0.075f, bedL));
        Primitives::drawCube(shader, stringer, woodDark);
    }

    // Cross-bearers (5 cross beams)
    for (int cb = 0; cb < 5; ++cb) {
        float bz = -1.35f + (float)cb * 0.58f;
        mat4 crossB = model;
        crossB = translate(crossB, vec3(0.0f, bedY - 0.035f, bz));
        crossB = scale(crossB, vec3(bedW + 0.08f, 0.065f, 0.075f));
        Primitives::drawCube(shader, crossB, woodDark);
    }

    // Floor deck planks (split bamboo / timber slats)
    mat4 floorDeck = model;
    floorDeck = translate(floorDeck, vec3(0.0f, bedY + 0.025f, -0.20f));
    floorDeck = scale(floorDeck, vec3(bedW, 0.025f, bedL));
    Primitives::drawCube(shader, floorDeck, woodLight);

    // Side retaining upright stakes & guard rails
    for (float sx : { -bedW * 0.50f, bedW * 0.50f }) {
        for (int p = 0; p < 4; ++p) {
            float pz = -1.35f + (float)p * 0.75f;
            mat4 post = model;
            post = translate(post, vec3(sx, bedY + 0.22f, pz));
            post = scale(post, vec3(0.045f, 0.44f, 0.045f));
            Primitives::drawCube(shader, post, woodDark);
        }
        // Side rails
        for (float ry : { 0.18f, 0.38f }) {
            mat4 rail = model;
            rail = translate(rail, vec3(sx, bedY + ry, -0.22f));
            rail = scale(rail, vec3(0.035f, 0.040f, bedL * 0.96f));
            Primitives::drawCube(shader, rail, woodLight);
        }
    }

    // ── 4. Arched Woven Bamboo Canopy / Hood (বাঁশের ছই / Chhoi) ─
    const float hoodZStart = -1.38f;
    const float hoodZEnd   =  0.22f;
    const float hoodLen    = hoodZEnd - hoodZStart;
    const float hoodR      = bedW * 0.52f; // Arch radius ~ 0.62m
    const int   archSegs   = 12;

    // Semicircular arched thatch/bamboo skin
    for (int a = 0; a < archSegs; ++a) {
        float a0 = (float)a * (PI / (float)archSegs);
        float a1 = (float)(a + 1) * (PI / (float)archSegs);
        float aMid = (a0 + a1) * 0.5f;

        float ax = -hoodR * cosf(aMid);
        float ay = bedY + 0.20f + hoodR * sinf(aMid);
        float segW = 2.0f * hoodR * sinf((a1 - a0) * 0.5f);
        float tiltAng = -atan2f(cosf(aMid), sinf(aMid));

        mat4 skinSeg = model;
        skinSeg = translate(skinSeg, vec3(ax, ay, (hoodZStart + hoodZEnd) * 0.5f));
        skinSeg = rotate(skinSeg, tiltAng, vec3(0.0f, 0.0f, 1.0f));
        skinSeg = scale(skinSeg, vec3(segW * 1.05f, 0.020f, hoodLen));
        Primitives::drawCube(shader, skinSeg, (a % 2 == 0) ? chhoiCol : (chhoiCol * 0.94f));
    }

    // 4 Curved Structural Bamboo Hoops
    for (int h = 0; h < 4; ++h) {
        float hz = hoodZStart + (float)h * (hoodLen / 3.0f);
        for (int a = 0; a < archSegs; ++a) {
            float aMid = ((float)a + 0.5f) * (PI / (float)archSegs);
            float ax = -hoodR * 1.01f * cosf(aMid);
            float ay = bedY + 0.20f + hoodR * 1.01f * sinf(aMid);
            float segW = 2.0f * hoodR * sinf(PI / (float)(archSegs * 2));
            float tiltAng = -atan2f(cosf(aMid), sinf(aMid));

            mat4 hoopSeg = model;
            hoopSeg = translate(hoopSeg, vec3(ax, ay, hz));
            hoopSeg = rotate(hoopSeg, tiltAng, vec3(0.0f, 0.0f, 1.0f));
            hoopSeg = scale(hoopSeg, vec3(segW * 1.04f, 0.035f, 0.045f));
            Primitives::drawCube(shader, hoopSeg, hoopCol);
        }
    }

    // ── 5. Cargo Under the Canopy ────────────────────────────────
    // Stacked golden rice straw bales (বিচালি / খড়)
    mat4 straw1 = model;
    straw1 = translate(straw1, vec3(0.0f, bedY + 0.22f, -0.65f));
    straw1 = scale(straw1, vec3(bedW * 0.78f, 0.36f, 0.85f));
    Primitives::drawCube(shader, straw1, strawCol);

    mat4 straw2 = model;
    straw2 = translate(straw2, vec3(0.0f, bedY + 0.44f, -0.62f));
    straw2 = scale(straw2, vec3(bedW * 0.62f, 0.26f, 0.68f));
    Primitives::drawCube(shader, straw2, strawCol * 0.95f);

    // Jute grain sacks (পাটের বস্তা) at the rear
    for (float sx : { -0.22f, 0.22f }) {
        mat4 sack = model;
        sack = translate(sack, vec3(sx, bedY + 0.16f, -1.15f));
        sack = scale(sack, vec3(0.24f, 0.22f, 0.32f));
        Primitives::drawSphere(shader, sack, sackCol);
    }

    // Terracotta water pitcher (Kolshi) lashed to the rear corner
    mat4 kolshiM = model;
    kolshiM = translate(kolshiM, vec3(bedW * 0.45f, bedY + 0.14f, -1.30f));
    kolshiM = scale(kolshiM, vec3(0.70f));
    drawKolshi(shader, kolshiM);

    // ── 6. Long A-Frame Draft Pole & Tongue (ইশাল / ধুরা) ────────
    // Two converging wooden shafts extending forward to yoke
    for (int s = -1; s <= 1; s += 2) {
        float fs = (float)s;
        mat4 pole = model;
        float px0 = fs * 0.45f, py0 = bedY, pz0 = -0.20f;
        float px1 = fs * 0.12f, py1 = 0.96f, pz1 =  2.45f;
        float mx = (px0 + px1) * 0.5f;
        float my = (py0 + py1) * 0.5f;
        float mz = (pz0 + pz1) * 0.5f;
        float dx = px1 - px0;
        float dy = py1 - py0;
        float dz = pz1 - pz0;
        float len = sqrtf(dx * dx + dy * dy + dz * dz);
        float yawAng = atan2f(dx, dz);
        float pitchAng = -atan2f(dy, sqrtf(dx * dx + dz * dz));

        pole = translate(pole, vec3(mx, my, mz));
        pole = rotate(pole, yawAng, vec3(0.0f, 1.0f, 0.0f));
        pole = rotate(pole, pitchAng, vec3(1.0f, 0.0f, 0.0f));
        pole = scale(pole, vec3(0.065f, 0.065f, len));
        Primitives::drawCylinder(shader, pole, woodDark);
    }

    // Central tongue tip extension with iron hitch ring
    mat4 tongueTip = model;
    tongueTip = translate(tongueTip, vec3(0.0f, 0.96f, 2.50f));
    tongueTip = scale(tongueTip, vec3(0.09f, 0.08f, 0.28f));
    Primitives::drawCube(shader, tongueTip, woodDark);

    // ── 7. The Yoke (জোয়াল / Joyal) ──────────────────────────────
    const float yokeY = 0.98f;
    const float yokeZ = 2.45f;

    // Transverse curved wooden beam spanning across both bullocks
    mat4 yoke = model;
    yoke = translate(yoke, vec3(0.0f, yokeY, yokeZ));
    yoke = scale(yoke, vec3(1.72f, 0.075f, 0.085f));
    Primitives::drawCube(shader, yoke, woodDark);

    // Curved neck rests on yoke over each bullock
    for (float yx : { -0.58f, 0.58f }) {
        mat4 neckRest = model;
        neckRest = translate(neckRest, vec3(yx, yokeY - 0.035f, yokeZ));
        neckRest = scale(neckRest, vec3(0.32f, 0.045f, 0.095f));
        Primitives::drawCube(shader, neckRest, woodLight);
    }

    // 4 Vertical wooden yoke pins (জুঁতি / Khuti) hanging down
    for (float kx : { -0.78f, -0.38f, 0.38f, 0.78f }) {
        mat4 pin = model;
        pin = translate(pin, vec3(kx, yokeY - 0.16f, yokeZ));
        pin = scale(pin, vec3(0.035f, 0.32f, 0.035f));
        Primitives::drawCylinder(shader, pin, woodLight);
    }

    // Jute coir tie ropes binding the yoke to the tongue
    mat4 yokeRope = model;
    yokeRope = translate(yokeRope, vec3(0.0f, yokeY, yokeZ));
    yokeRope = scale(yokeRope, vec3(0.18f, 0.10f, 0.16f));
    Primitives::drawCube(shader, yokeRope, ropeCol);

    // ── 8. The Bullock Cart Driver (গাড়িয়াল / Gariyal) ───────────
    PersonParams gariyal;
    gariyal.skinColor   = vec3(0.52f, 0.35f, 0.22f);
    gariyal.shirtColor  = vec3(0.85f, 0.82f, 0.76f); // off-white kurta
    gariyal.pantsColor  = vec3(0.18f, 0.36f, 0.52f); // checkered blue lungi
    gariyal.seated      = true;
    gariyal.hasGamcha   = true;
    gariyal.gamchaColor = vec3(0.82f, 0.18f, 0.12f); // red gamcha around neck
    gariyal.rightArmAngle = radians(28.0f);          // holding driving reins/stick forward

    mat4 gariyalM = model;
    gariyalM = translate(gariyalM, vec3(0.0f, bedY + 0.15f, 0.48f));
    if (walkPhase != 0.0f) {
        float driverSway = sinf(walkPhase * 2.0f) * radians(3.0f);
        gariyalM = rotate(gariyalM, driverSway, vec3(1.0f, 0.0f, 0.0f));
    }
    Person::draw(shader, gariyalM, gariyal);

    // Bamboo driving stick (পাঁচনি / Pachni) held in cartman's right hand
    mat4 stick = model;
    stick = translate(stick, vec3(0.22f, bedY + 0.55f, 0.88f));
    stick = rotate(stick, radians(-25.0f), vec3(1.0f, 0.0f, 0.0f));
    stick = scale(stick, vec3(0.020f, 0.020f, 0.85f));
    Primitives::drawCylinder(shader, stick, woodLight);

    // ── 9. Pair of Harness Deshi Draft Bullocks (এক জোড়া বলদ গরু) ─
    // Left Bullock (ফসল-রঙা বলদ — warm fawn-brown Deshi ox)
    mat4 leftOx = model;
    leftOx = translate(leftOx, vec3(-0.58f, 0.0f, 2.30f));
    drawCow(shader, leftOx, false, nullptr, walkPhase);

    // Right Bullock (সাদা-ধূসর বলদ — light cream-grey Deshi ox)
    mat4 rightOx = model;
    rightOx = translate(rightOx, vec3(0.58f, 0.0f, 2.30f));
    drawCow(shader, rightOx, false, &whiteOxCol, walkPhase + 0.35f);

    // Harness ropes extending from yoke to oxen halters
    for (float oxX : { -0.58f, 0.58f }) {
        mat4 harnessRope = model;
        harnessRope = translate(harnessRope, vec3(oxX, 0.90f, 2.38f));
        harnessRope = rotate(harnessRope, radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
        harnessRope = scale(harnessRope, vec3(0.022f, 0.022f, 0.32f));
        Primitives::drawCylinder(shader, harnessRope, ropeCol);
    }
}

} // namespace House

