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

// ═════════════════════════════════════════════════════════════════════
// RURAL BENGALI WOMAN COOKING FOOD (গ্রাম্য বধূ / রাঁধুনি)
// Seated gracefully on a low wooden stool (Kather Piri) in front of the
// outdoor clay cooking stove, draped in a traditional crimson handloom
// Saree with golden border (Paar) and modest head veil (Ghomta).
// Actively stirring the simmering curry pot with a wooden cooking spatula (Khunti).
// ═════════════════════════════════════════════════════════════════════
static void drawCookingWoman(Shader& shader, const mat4& stoveModel, float animTime)
{
    // Traditional Bengali Rural Color Palette
    vec3 skinTone     (0.58f, 0.40f, 0.26f); // warm natural Bengali skin tone
    vec3 sareeRed     (0.78f, 0.15f, 0.12f); // traditional crimson red handloom saree (Shari)
    vec3 sareePaar    (0.94f, 0.78f, 0.20f); // golden-yellow woven border (Paar)
    vec3 blouseYellow (0.84f, 0.62f, 0.16f); // golden turmeric cotton blouse
    vec3 hairDark     (0.08f, 0.06f, 0.05f); // glossy dark hair bun
    vec3 bindiRed     (0.88f, 0.10f, 0.08f); // vermilion bindi (Laal Tip)
    vec3 sindoorRed   (0.85f, 0.12f, 0.10f); // parting vermilion (Sindoor)
    vec3 glassRed     (0.86f, 0.12f, 0.10f); // red glass bangle (Reshmi Churi)
    vec3 glassGold    (0.96f, 0.82f, 0.22f); // gold bangle
    vec3 altaRed      (0.84f, 0.14f, 0.14f); // red decorative foot dye (Alta)
    vec3 khuntiWood   (0.48f, 0.34f, 0.18f); // wooden spatula handle
    vec3 khuntiSteel  (0.75f, 0.78f, 0.82f); // steel cooking spatula blade (Khunti)

    // Attentive cooking breath and subtle spatula stirring animation
    float breath   = (animTime > 0.0f) ? (sinf(animTime * 2.2f) * 0.003f) : 0.0f;
    float stirAnim = (animTime > 0.0f) ? (sinf(animTime * 3.2f) * radians(6.0f)) : 0.0f;

    // Woman base transform: seated on the Piri at (0.20, 0.0f, 0.44), angled facing the stove pot
    mat4 wm = stoveModel;
    wm = translate(wm, vec3(0.20f, 0.0f, 0.44f));
    wm = rotate(wm, radians(-152.0f), vec3(0.0f, 1.0f, 0.0f));

    // ── 1. Seated Lower Body & Saree Pleats Draped on Piri ──────────
    float seatY = 0.055f;
    mat4 lap = wm;
    lap = translate(lap, vec3(0.0f, seatY + 0.040f, 0.02f));
    lap = scale(lap, vec3(0.20f, 0.085f, 0.16f));
    Primitives::drawSphere(shader, lap, sareeRed);

    // Left folded thigh / knee
    mat4 thighL = wm;
    thighL = translate(thighL, vec3(-0.075f, seatY + 0.030f, 0.10f));
    thighL = rotate(thighL, radians(-12.0f), vec3(0.0f, 1.0f, 0.0f));
    thighL = rotate(thighL, radians(15.0f), vec3(1.0f, 0.0f, 0.0f));
    thighL = scale(thighL, vec3(0.085f, 0.075f, 0.18f));
    Primitives::drawSphere(shader, thighL, sareeRed);

    // Right folded thigh / knee (angled toward the cooking pot)
    mat4 thighR = wm;
    thighR = translate(thighR, vec3(0.075f, seatY + 0.035f, 0.09f));
    thighR = rotate(thighR, radians(10.0f), vec3(0.0f, 1.0f, 0.0f));
    thighR = rotate(thighR, radians(14.0f), vec3(1.0f, 0.0f, 0.0f));
    thighR = scale(thighR, vec3(0.085f, 0.075f, 0.18f));
    Primitives::drawSphere(shader, thighR, sareeRed);

    // Flowing saree cloth mass around legs and ground
    mat4 skirtFold = wm;
    skirtFold = translate(skirtFold, vec3(0.0f, seatY + 0.015f, 0.09f));
    skirtFold = scale(skirtFold, vec3(0.22f, 0.070f, 0.18f));
    Primitives::drawCube(shader, skirtFold, sareeRed);

    // Golden border (Paar) along front hem of saree
    mat4 sareeBorder = wm;
    sareeBorder = translate(sareeBorder, vec3(0.0f, seatY + 0.010f, 0.185f));
    sareeBorder = scale(sareeBorder, vec3(0.20f, 0.014f, 0.012f));
    Primitives::drawCube(shader, sareeBorder, sareePaar);

    // Bare feet peeking out near bottom of saree
    for (int side = -1; side <= 1; side += 2) {
        float fside = (float)side;
        mat4 foot = wm;
        foot = translate(foot, vec3(fside * 0.065f, 0.012f, 0.17f));
        foot = scale(foot, vec3(0.036f, 0.020f, 0.055f));
        Primitives::drawCube(shader, foot, skinTone);

        // Traditional Alta dye rim around foot edge
        mat4 alta = wm;
        alta = translate(alta, vec3(fside * 0.065f, 0.005f, 0.17f));
        alta = scale(alta, vec3(0.038f, 0.008f, 0.058f));
        Primitives::drawCube(shader, alta, altaRed);
    }

    // ── 2. Torso with Blouse & Diagonal Saree Anchol (আঁচল) ─────────
    float torsoH = 0.22f;
    float torsoBaseY = seatY + 0.065f;
    float torsoCenterY = torsoBaseY + torsoH * 0.5f + breath;

    // Torso slightly tilted forward (attentive cooking lean)
    mat4 torso = wm;
    torso = translate(torso, vec3(0.0f, torsoCenterY, 0.03f));
    torso = rotate(torso, radians(10.0f), vec3(1.0f, 0.0f, 0.0f));
    mat4 torsoS = scale(torso, vec3(0.17f, torsoH, 0.11f));
    Primitives::drawCube(shader, torsoS, blouseYellow);

    // Diagonal Saree Anchol drape across chest (from right hip over left shoulder)
    mat4 anchol = torso;
    anchol = translate(anchol, vec3(-0.015f, 0.010f, 0.052f));
    anchol = rotate(anchol, radians(-28.0f), vec3(0.0f, 0.0f, 1.0f));
    mat4 ancholS = scale(anchol, vec3(0.12f, torsoH * 0.90f, 0.022f));
    Primitives::drawCube(shader, ancholS, sareeRed);

    // Golden border trim along the diagonal anchol drape
    mat4 ancholTrim = anchol;
    ancholTrim = translate(ancholTrim, vec3(0.055f, 0.0f, 0.004f));
    mat4 ancholTrimS = scale(ancholTrim, vec3(0.016f, torsoH * 0.92f, 0.024f));
    Primitives::drawCube(shader, ancholTrimS, sareePaar);

    // ── 3. Neck & Head with Expressive Face & Vermilion Tip ──────────
    float neckBaseY = torsoCenterY + torsoH * 0.48f;
    mat4 neck = wm;
    neck = translate(neck, vec3(0.0f, neckBaseY + 0.015f, 0.045f));
    neck = scale(neck, vec3(0.032f, 0.035f, 0.032f));
    Primitives::drawCylinder(shader, neck, skinTone);

    float headR = 0.068f;
    float headCenterY = neckBaseY + 0.032f + headR;
    mat4 headBase = wm;
    headBase = translate(headBase, vec3(0.0f, headCenterY, 0.055f));
    headBase = rotate(headBase, radians(14.0f), vec3(1.0f, 0.0f, 0.0f)); // looking down at pot

    mat4 head = headBase;
    head = scale(head, vec3(headR * 0.92f, headR, headR * 0.94f));
    Primitives::drawSphere(shader, head, skinTone);

    // Glossy hair bun (Khnopa) at the nape/back of head
    mat4 bun = headBase;
    bun = translate(bun, vec3(0.0f, -0.010f, -headR * 0.88f));
    bun = scale(bun, vec3(0.052f, 0.048f, 0.045f));
    Primitives::drawSphere(shader, bun, hairDark);

    // Front hair parting (Shithi)
    mat4 hairFront = headBase;
    hairFront = translate(hairFront, vec3(0.0f, headR * 0.65f, 0.010f));
    hairFront = scale(hairFront, vec3(headR * 0.90f, 0.028f, headR * 0.85f));
    Primitives::drawSphere(shader, hairFront, hairDark);

    // Parting Sindoor (red vermilion in hair parting)
    mat4 sindoor = headBase;
    sindoor = translate(sindoor, vec3(0.0f, headR * 0.82f, headR * 0.40f));
    sindoor = scale(sindoor, vec3(0.008f, 0.010f, 0.035f));
    Primitives::drawCube(shader, sindoor, sindoorRed);

    // Vermilion Bindi (Laal Tip) centered on forehead
    mat4 tipM = headBase;
    tipM = translate(tipM, vec3(0.0f, headR * 0.32f, headR * 0.86f));
    tipM = scale(tipM, vec3(0.013f, 0.013f, 0.006f));
    Primitives::drawSphere(shader, tipM, bindiRed);

    // Delicate nose bridge and tip
    mat4 noseM = headBase;
    noseM = translate(noseM, vec3(0.0f, -0.004f, headR * 0.92f));
    noseM = rotate(noseM, radians(18.0f), vec3(1.0f, 0.0f, 0.0f));
    noseM = scale(noseM, vec3(0.013f, 0.026f, 0.022f));
    Primitives::drawCone(shader, noseM, skinTone * 0.94f);

    // Attentive downward-looking eyes & gentle eyebrows
    vec3 eyeWhite(0.94f, 0.94f, 0.90f);
    vec3 eyePupil(0.12f, 0.09f, 0.07f);
    for (int side = -1; side <= 1; side += 2) {
        float fside = (float)side;
        // Eye white
        mat4 eyeW = headBase;
        eyeW = translate(eyeW, vec3(fside * 0.026f, headR * 0.16f, headR * 0.84f));
        eyeW = scale(eyeW, vec3(0.012f, 0.009f, 0.010f));
        Primitives::drawSphere(shader, eyeW, eyeWhite);

        // Pupil directed down toward pot
        mat4 pupil = headBase;
        pupil = translate(pupil, vec3(fside * 0.026f, headR * 0.13f, headR * 0.88f));
        pupil = scale(pupil, vec3(0.0065f, 0.0065f, 0.0065f));
        Primitives::drawSphere(shader, pupil, eyePupil);

        // Eyebrow
        mat4 brow = headBase;
        brow = translate(brow, vec3(fside * 0.026f, headR * 0.28f, headR * 0.82f));
        brow = rotate(brow, radians(fside * -8.0f), vec3(0.0f, 0.0f, 1.0f));
        brow = scale(brow, vec3(0.018f, 0.004f, 0.008f));
        Primitives::drawCube(shader, brow, hairDark);

        // Gold ear stud (Dul)
        mat4 earStud = headBase;
        earStud = translate(earStud, vec3(fside * (headR * 0.88f), 0.0f, 0.0f));
        earStud = scale(earStud, vec3(0.008f, 0.008f, 0.008f));
        Primitives::drawSphere(shader, earStud, glassGold);
    }

    // ── 4. Modest Draped Bengali Head Veil (Ghomta / ঘোমটা) ───────────
    // Top arch of the veil curving over hair and crown
    mat4 ghomtaTop = headBase;
    ghomtaTop = translate(ghomtaTop, vec3(0.0f, headR * 0.35f, -0.010f));
    ghomtaTop = scale(ghomtaTop, vec3(headR * 1.15f, headR * 0.95f, headR * 1.12f));
    Primitives::drawSphere(shader, ghomtaTop, sareeRed);

    // Golden border (Paar) along front rim of head veil framing the face
    mat4 ghomtaRim = headBase;
    ghomtaRim = translate(ghomtaRim, vec3(0.0f, headR * 0.55f, headR * 0.40f));
    ghomtaRim = rotate(ghomtaRim, radians(38.0f), vec3(1.0f, 0.0f, 0.0f));
    ghomtaRim = scale(ghomtaRim, vec3(headR * 1.05f, 0.015f, 0.018f));
    Primitives::drawCube(shader, ghomtaRim, sareePaar);

    // Left and right veil folds cascading down beside cheeks over shoulders
    for (int side = -1; side <= 1; side += 2) {
        float fside = (float)side;
        mat4 ghomtaSide = headBase;
        ghomtaSide = translate(ghomtaSide, vec3(fside * (headR * 0.95f), -headR * 0.40f, -0.015f));
        ghomtaSide = rotate(ghomtaSide, radians(fside * 10.0f), vec3(0.0f, 0.0f, 1.0f));
        ghomtaSide = scale(ghomtaSide, vec3(0.022f, headR * 0.90f, headR * 0.70f));
        Primitives::drawCube(shader, ghomtaSide, sareeRed);
    }

    // Rear drape of veil falling down the upper back
    mat4 ghomtaBack = headBase;
    ghomtaBack = translate(ghomtaBack, vec3(0.0f, -headR * 0.50f, -headR * 0.85f));
    ghomtaBack = scale(ghomtaBack, vec3(headR * 1.10f, headR * 0.95f, 0.025f));
    Primitives::drawCube(shader, ghomtaBack, sareeRed);

    // ── 5. Right Arm: Stirring Cooking Pot with Khunti (Spatula) ─────
    mat4 shoulderR = wm;
    shoulderR = translate(shoulderR, vec3(0.10f, torsoCenterY + torsoH * 0.35f, 0.04f));

    // Upper arm: short blouse sleeve
    mat4 upperArmR = shoulderR;
    upperArmR = rotate(upperArmR, radians(-42.0f), vec3(1.0f, 0.0f, 0.0f)); // angled forward
    upperArmR = rotate(upperArmR, radians(-14.0f), vec3(0.0f, 0.0f, 1.0f)); // angled slightly inward
    mat4 upperSleeveR = upperArmR;
    upperSleeveR = translate(upperSleeveR, vec3(0.0f, -0.055f, 0.0f));
    upperSleeveR = scale(upperSleeveR, vec3(0.028f, 0.090f, 0.028f));
    Primitives::drawCylinder(shader, upperSleeveR, blouseYellow);

    // Blouse sleeve golden hem
    mat4 sleeveHemR = upperArmR;
    sleeveHemR = translate(sleeveHemR, vec3(0.0f, -0.10f, 0.0f));
    sleeveHemR = scale(sleeveHemR, vec3(0.030f, 0.012f, 0.030f));
    Primitives::drawCylinder(shader, sleeveHemR, sareePaar);

    // Forearm extending toward the cooking pot
    mat4 elbowR = upperArmR;
    elbowR = translate(elbowR, vec3(0.0f, -0.11f, 0.0f));
    elbowR = rotate(elbowR, radians(54.0f + stirAnim), vec3(1.0f, 0.0f, 0.0f)); // bend forward toward pot
    elbowR = rotate(elbowR, radians(-16.0f), vec3(0.0f, 1.0f, 0.0f));          // aim toward stove center
    mat4 foreArmR = elbowR;
    foreArmR = translate(foreArmR, vec3(0.0f, -0.065f, 0.0f));
    foreArmR = scale(foreArmR, vec3(0.022f, 0.120f, 0.022f));
    Primitives::drawCylinder(shader, foreArmR, skinTone);

    // Traditional red and gold glass bangles (Kacher Churi) on wrist
    mat4 wristR = elbowR;
    wristR = translate(wristR, vec3(0.0f, -0.115f, 0.0f));
    for (int b = 0; b < 3; b++) {
        mat4 bangle = wristR;
        bangle = translate(bangle, vec3(0.0f, (float)b * 0.007f, 0.0f));
        bangle = scale(bangle, vec3(0.026f, 0.005f, 0.026f));
        Primitives::drawCylinder(shader, bangle, (b % 2 == 0) ? glassRed : glassGold);
    }

    // Right hand firmly grasping the cooking spatula
    mat4 handR = elbowR;
    handR = translate(handR, vec3(0.0f, -0.135f, 0.005f));
    mat4 handRS = scale(handR, vec3(0.020f, 0.026f, 0.022f));
    Primitives::drawSphere(shader, handRS, skinTone);

    // Traditional Cooking Spatula / Ladle (Khunti / খুন্তি)
    mat4 spatula = handR;
    spatula = rotate(spatula, radians(42.0f), vec3(1.0f, 0.0f, 0.0f));
    spatula = rotate(spatula, radians(10.0f), vec3(0.0f, 1.0f, 0.0f));
    mat4 spHandle = spatula;
    spHandle = translate(spHandle, vec3(0.0f, -0.080f, 0.0f));
    spHandle = scale(spHandle, vec3(0.007f, 0.180f, 0.007f));
    Primitives::drawCylinder(shader, spHandle, khuntiWood);

    // Polished steel spatula blade dipping into pot
    mat4 spBlade = spatula;
    spBlade = translate(spBlade, vec3(0.0f, -0.170f, 0.0f));
    spBlade = scale(spBlade, vec3(0.028f, 0.038f, 0.004f));
    Primitives::drawCube(shader, spBlade, khuntiSteel);

    // ── 6. Left Arm: Tending Stove Firewood / Resting on Knee ───────
    mat4 shoulderL = wm;
    shoulderL = translate(shoulderL, vec3(-0.10f, torsoCenterY + torsoH * 0.35f, 0.04f));

    mat4 upperArmL = shoulderL;
    upperArmL = rotate(upperArmL, radians(-28.0f), vec3(1.0f, 0.0f, 0.0f));
    upperArmL = rotate(upperArmL, radians(18.0f), vec3(0.0f, 0.0f, 1.0f));
    mat4 upperSleeveL = upperArmL;
    upperSleeveL = translate(upperSleeveL, vec3(0.0f, -0.055f, 0.0f));
    upperSleeveL = scale(upperSleeveL, vec3(0.028f, 0.090f, 0.028f));
    Primitives::drawCylinder(shader, upperSleeveL, blouseYellow);

    mat4 sleeveHemL = upperArmL;
    sleeveHemL = translate(sleeveHemL, vec3(0.0f, -0.10f, 0.0f));
    sleeveHemL = scale(sleeveHemL, vec3(0.030f, 0.012f, 0.030f));
    Primitives::drawCylinder(shader, sleeveHemL, sareePaar);

    // Forearm reaching forward-down toward firewood stick
    mat4 elbowL = upperArmL;
    elbowL = translate(elbowL, vec3(0.0f, -0.11f, 0.0f));
    elbowL = rotate(elbowL, radians(38.0f), vec3(1.0f, 0.0f, 0.0f));
    elbowL = rotate(elbowL, radians(15.0f), vec3(0.0f, 1.0f, 0.0f));
    mat4 foreArmL = elbowL;
    foreArmL = translate(foreArmL, vec3(0.0f, -0.060f, 0.0f));
    foreArmL = scale(foreArmL, vec3(0.022f, 0.115f, 0.022f));
    Primitives::drawCylinder(shader, foreArmL, skinTone);

    // Glass bangles on left wrist
    mat4 wristL = elbowL;
    wristL = translate(wristL, vec3(0.0f, -0.110f, 0.0f));
    for (int b = 0; b < 3; b++) {
        mat4 bangle = wristL;
        bangle = translate(bangle, vec3(0.0f, (float)b * 0.007f, 0.0f));
        bangle = scale(bangle, vec3(0.026f, 0.005f, 0.026f));
        Primitives::drawCylinder(shader, bangle, (b % 2 == 0) ? glassRed : glassGold);
    }

    // Left hand gently resting/guiding near firewood
    mat4 handL = elbowL;
    handL = translate(handL, vec3(0.0f, -0.130f, 0.005f));
    handL = scale(handL, vec3(0.020f, 0.026f, 0.022f));
    Primitives::drawSphere(shader, handL, skinTone);
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

    // Terracotta pot lid (Sharani / ঢাকনা) propped tilted to the side so food is stirred
    mat4 potLid = cm;
    potLid = translate(potLid, vec3(-0.065f, 0.415f, 0.045f));
    potLid = rotate(potLid, radians(24.0f), vec3(0.0f, 0.0f, 1.0f));
    mat4 potLidS = scale(potLid, vec3(0.130f, 0.022f, 0.130f));
    Primitives::drawSphere(shader, potLidS, pot2Color);

    // Lid handle knob
    mat4 lidKnob = potLid;
    lidKnob = translate(lidKnob, vec3(0.0f, 0.022f, 0.0f));
    lidKnob = scale(lidKnob, vec3(0.022f, 0.022f, 0.022f));
    Primitives::drawSphere(shader, lidKnob, potColor);

    // Delicious steaming curry / lentil dal inside the cooking pot
    vec3 curryColor(0.88f, 0.64f, 0.14f); // turmeric spiced yellow curry
    mat4 curry = cm;
    curry = translate(curry, vec3(0.015f, 0.380f, 0.065f));
    curry = scale(curry, vec3(0.115f, 0.010f, 0.115f));
    Primitives::drawCylinder(shader, curry, curryColor);

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

    // ── 9. Rural Bengali Woman Sitting in Front of Stove Cooking Food ─
    drawCookingWoman(shader, cm, animTime);
}

void draw(Shader& shader, const mat4& model, HouseStyle style, int variant, bool withStove, bool withChimney, float animTime)
{
    shader.setInt("uUseTexture", 0); // authentic sun-dried clay/mud plaster, golden rice thatch, and bamboo

    // ── Traditional Color Palette ───────────────────────────────
    vec3 plinthColor (0.40f, 0.30f, 0.18f);  // dark packed clay earth (Matir Viti)
    vec3 wallColor   (0.65f, 0.56f, 0.44f);  // sun-dried mud / clay plaster
    vec3 cornerPost  (0.32f, 0.22f, 0.12f);  // seasoned timber / dark bamboo
    vec3 roofStraw   (0.66f, 0.52f, 0.22f);  // golden weathered thatch / straw
    vec3 roofRidge   (0.48f, 0.36f, 0.16f);  // thatch ridge cap
    vec3 doorWood    (0.28f, 0.16f, 0.08f);  // dark oiled timber
    vec3 windowFrame (0.22f, 0.14f, 0.08f);  // dark frame
    vec3 shutterColor(0.36f, 0.22f, 0.12f);  // timber shutters
    vec3 postColor   (0.52f, 0.42f, 0.20f);  // bamboo verandah pillars
    vec3 rafterColor (0.38f, 0.26f, 0.12f);  // wooden rafters

    // Visual diversity scale multiplier across the village
    float scaleMult = 1.0f;
    if (variant == 1) {
        scaleMult = 1.08f; // Large Elder / Master Bari
    } else if (variant == 2) {
        scaleMult = 0.90f; // Compact Farmer / Boatman Cottage
    } else {
        scaleMult = 1.00f; // Standard Family Homestead
    }

    mat4 baseModel = scale(model, vec3(scaleMult));

    // Base house dimensions: matching the authentic Chouchala house (01_house_chouchala)
    float houseW = 3.6f;   // width along X
    float houseH = 1.95f;  // wall height
    float houseD = 2.8f;   // depth along Z
    float plinthH = 0.25f; // raised earthen base

    // ── 1. Raised Earthen Plinth (Viti / Dawa) ────────────────────
    mat4 plinth = baseModel;
    plinth = translate(plinth, vec3(0.0f, plinthH * 0.5f, 0.35f));
    plinth = scale(plinth, vec3(houseW + 0.8f, plinthH, houseD + 1.4f));
    Primitives::drawCube(shader, plinth, plinthColor);

    // Front entrance step centered before the verandah
    mat4 step = baseModel;
    step = translate(step, vec3(0.0f, plinthH * 0.25f, (houseD + 1.4f) * 0.5f + 0.35f + 0.15f));
    step = scale(step, vec3(1.2f, plinthH * 0.5f, 0.40f));
    Primitives::drawCube(shader, step, plinthColor);

    // ── 2. Main Walls (Mud / Sun-dried Clay) ──────────────────────
    float wallCenterY = plinthH + houseH * 0.5f;
    mat4 walls = baseModel;
    walls = translate(walls, vec3(0.0f, wallCenterY, 0.0f));
    walls = scale(walls, vec3(houseW, houseH, houseD));
    Primitives::drawCube(shader, walls, wallColor);

    // Timber corner posts at 4 corners
    float hx = houseW * 0.5f;
    float hz = houseD * 0.5f;
    float cornerOffsets[4][2] = { {-hx, -hz}, {hx, -hz}, {-hx, hz}, {hx, hz} };
    for (int i = 0; i < 4; i++) {
        mat4 cp = baseModel;
        cp = translate(cp, vec3(cornerOffsets[i][0], wallCenterY, cornerOffsets[i][1]));
        cp = scale(cp, vec3(0.12f, houseH + 0.05f, 0.12f));
        Primitives::drawCube(shader, cp, cornerPost);
    }

    // Horizontal bamboo tie-beam along top of walls
    mat4 beamFront = baseModel;
    beamFront = translate(beamFront, vec3(0.0f, plinthH + houseH, hz));
    beamFront = scale(beamFront, vec3(houseW + 0.1f, 0.08f, 0.10f));
    Primitives::drawCube(shader, beamFront, cornerPost);

    // ── 3. Roof System: Traditional 4-Sloped Chouchala Roof ───────
    float roofBaseY = plinthH + houseH;
    float roofW = houseW + 1.2f;
    float roofD = houseD + 1.2f;
    float roofH = 1.6f;

    mat4 roof = baseModel;
    roof = translate(roof, vec3(0.0f, roofBaseY, 0.0f));
    roof = scale(roof, vec3(roofW, roofH, roofD));
    Primitives::drawPyramid(shader, roof, roofStraw);


    // Under-eave rafter trim (slight dark underside)
    mat4 eaveTrim = baseModel;
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
        mat4 post = baseModel;
        post = translate(post, vec3(px, plinthH + (houseH * 0.85f) * 0.5f, postZ));
        post = scale(post, vec3(0.07f, houseH * 0.85f, 0.07f));
        Primitives::drawCylinder(shader, post, postColor);

        // Circular post-top peg on the verandah roof
        mat4 cap = baseModel;
        float capY = plinthH + houseH * 0.88f - 0.02f;
        cap = translate(cap, vec3(px, capY, postZ));
        cap = scale(cap, vec3(0.09f, 0.025f, 0.09f));
        Primitives::drawCylinder(shader, cap, postColor * 0.85f);
    }

    // Sloping verandah lean-to roof extending out from main wall
    mat4 vRoof = baseModel;
    vRoof = translate(vRoof, vec3(0.0f, plinthH + houseH * 0.88f, verandahZ));
    vRoof = rotate(vRoof, radians(12.0f), vec3(1.0f, 0.0f, 0.0f)); // gentle forward slope
    vRoof = scale(vRoof, vec3(houseW + 0.8f, 0.07f, verandahDepth + 0.35f));
    Primitives::drawCube(shader, vRoof, roofStraw);

    // Low wooden railing / bench on verandah side
    mat4 vBench = baseModel;
    vBench = translate(vBench, vec3(-hx + 0.4f, plinthH + 0.22f, verandahZ));
    vBench = scale(vBench, vec3(0.7f, 0.06f, verandahDepth * 0.7f));
    Primitives::drawCube(shader, vBench, doorWood);

    // ── 5. Wooden Door & Frame ───────────────────────────────────
    float doorW = 0.70f;
    float doorH = 1.40f;
    float doorY = plinthH + doorH * 0.5f;

    // Door frame
    mat4 dFrame = baseModel;
    dFrame = translate(dFrame, vec3(0.0f, doorY, hz + 0.02f));
    dFrame = scale(dFrame, vec3(doorW + 0.12f, doorH + 0.10f, 0.04f));
    Primitives::drawCube(shader, dFrame, windowFrame);

    // Door panel (slightly recessed)
    mat4 dPanel = baseModel;
    dPanel = translate(dPanel, vec3(0.0f, doorY, hz + 0.03f));
    dPanel = scale(dPanel, vec3(doorW, doorH, 0.03f));
    Primitives::drawCube(shader, dPanel, doorWood);

    // ── 6. Windows with Open Wooden Shutters ─────────────────────
    float winSize = 0.55f;
    float winY = plinthH + houseH * 0.55f;

    // Left front window
    float winLX = -hx * 0.60f;
    mat4 wFrameL = baseModel;
    wFrameL = translate(wFrameL, vec3(winLX, winY, hz + 0.02f));
    wFrameL = scale(wFrameL, vec3(winSize, winSize, 0.04f));
    Primitives::drawCube(shader, wFrameL, windowFrame);

    // Warm interior lantern glow through window
    vec3 windowGlow(0.96f, 0.75f, 0.30f);
    shader.setFloat("emissive", 0.70f);
    mat4 wOpeningL = baseModel;
    wOpeningL = translate(wOpeningL, vec3(winLX, winY, hz + 0.03f));
    wOpeningL = scale(wOpeningL, vec3(winSize * 0.85f, winSize * 0.85f, 0.03f));
    Primitives::drawCube(shader, wOpeningL, windowGlow);
    shader.setFloat("emissive", 0.0f);

    // Wooden shutter swung open
    mat4 wShutterL = baseModel;
    wShutterL = translate(wShutterL, vec3(winLX - winSize * 0.45f, winY, hz + 0.15f));
    wShutterL = rotate(wShutterL, radians(-45.0f), vec3(0.0f, 1.0f, 0.0f));
    wShutterL = scale(wShutterL, vec3(winSize * 0.45f, winSize * 0.85f, 0.025f));
    Primitives::drawCube(shader, wShutterL, shutterColor);

    // Right front window
    float winRX = hx * 0.60f;
    mat4 wFrameR = baseModel;
    wFrameR = translate(wFrameR, vec3(winRX, winY, hz + 0.02f));
    wFrameR = scale(wFrameR, vec3(winSize, winSize, 0.04f));
    Primitives::drawCube(shader, wFrameR, windowFrame);

    shader.setFloat("emissive", 0.70f);
    mat4 wOpeningR = baseModel;
    wOpeningR = translate(wOpeningR, vec3(winRX, winY, hz + 0.03f));
    wOpeningR = scale(wOpeningR, vec3(winSize * 0.85f, winSize * 0.85f, 0.03f));
    Primitives::drawCube(shader, wOpeningR, windowGlow);
    shader.setFloat("emissive", 0.0f);

    // Wooden shutter swung open
    mat4 wShutterR = baseModel;
    wShutterR = translate(wShutterR, vec3(winRX + winSize * 0.45f, winY, hz + 0.15f));
    wShutterR = rotate(wShutterR, radians(45.0f), vec3(0.0f, 1.0f, 0.0f));
    wShutterR = scale(wShutterR, vec3(winSize * 0.45f, winSize * 0.85f, 0.025f));
    Primitives::drawCube(shader, wShutterR, shutterColor);

    // ── 7. Terracotta Water Pitchers (Matir Kolshi) on Verandah ───
    drawKolshi(shader, baseModel, vec3(hx * 0.70f, plinthH, postZ - 0.20f), 0.90f);
    drawKolshi(shader, baseModel, vec3(hx * 0.85f, plinthH, postZ - 0.35f), 0.75f);

    // ── 8. Outdoor Clay Cooking Stove (Matir Chula) beside the house ─
    if (withStove) {
        mat4 cm = baseModel;
        cm = translate(cm, vec3(hx + 1.20f, 0.0f, 0.30f));
        drawStove(shader, cm, true, animTime);
    }
}

// Traditional Rice Straw Stack (Khorer Paloi / Khorer Gada / খড়ের পালই)
// Faithfully modeled from authentic rural Bengali photographs:
// 1. Splayed ground straw skirt resting on courtyard earth with loose radial straw fringe.
// 2. Tall packed-bundle cylindrical lower body (H = 1.62m, R = 1.08m -> 1.02m) with vertical packed straw ribs, 40 protruding dry straw tufts, and 3 jute binding ropes.
// ═════════════════════════════════════════════════════════════════════
// 1. RICE STRAW STACK (খড়ের গাদা / Khorer Gada / পালুই)
// ═════════════════════════════════════════════════════════════════════
// Authentic rural Bangladeshi paddy straw stack (খড়ের পালুই / গাদা):
// • Ground timber foundation cradle (কাঠে-বাঁশের মাচা) keeping fodder off damp soil
// • Natural bell-conical bulging silhouette: flared base skirt, rounded belly, and steep thatch cone
// • Multi-tiered overlapping thatch shingle tiers with downward-slanted straw fringes
// • Authentic sun-dried golden-amber rice straw palette with organic fibrous texture
// • Horizontal jute binding cords (খড়ের বাঁধন / দড়ি) compressing the stack waist
// • Tall central upright seasoned bamboo pole (বাঁশের খুঁটি)
// • Iconic inverted terracotta clay pitcher rain-cap (উল্টো মাটির কলসি / হাঁড়ি) on the apex
void drawStrawStack(Shader& shader, const mat4& model, const vec3& pos, float scaleVal)
{
    shader.setInt("uUseTexture", 0);

    // ── Authentic Sun-Cured Golden Rice Straw Palette ─────────────
    const vec3 strawGold    (0.88f, 0.73f, 0.34f); // vibrant sun-cured golden straw
    const vec3 strawWarm    (0.80f, 0.64f, 0.27f); // warm amber thatch body
    const vec3 strawLight   (0.93f, 0.80f, 0.42f); // sunlit golden highlights & fringe tips
    const vec3 strawDeep    (0.65f, 0.49f, 0.19f); // deep warm shadow under thatch tiers
    const vec3 strawBaseCol (0.54f, 0.40f, 0.17f); // weathered, soil-level skirt straw
    const vec3 potTerracotta(0.76f, 0.36f, 0.18f); // iconic inverted terracotta clay pot
    const vec3 potDark      (0.55f, 0.24f, 0.11f); // terracotta pot mouth & rim
    const vec3 ropeJute     (0.42f, 0.30f, 0.16f); // twisted jute binding cords (খড়ের বাঁধন)
    const vec3 bambooCol    (0.70f, 0.64f, 0.40f); // seasoned upright bamboo pole (বাঁশের খুঁটি)
    const vec3 nodeCol      (0.48f, 0.38f, 0.22f); // raised bamboo node rings
    const vec3 logCol       (0.36f, 0.26f, 0.15f); // timber foundation runner logs (মাচা)

    mat4 m = model;
    m = translate(m, pos);
    m = scale(m, vec3(scaleVal));

    // Helper: draw thin cylinder with exact radius R and height H centered at Y
    auto drawCylR = [&](float yCenter, float r, float h, const vec3& col) {
        float s = r / 0.924f;
        mat4 cm = translate(m, vec3(0.0f, yCenter, 0.0f));
        cm = scale(cm, vec3(s, h, s));
        Primitives::drawCylinder(shader, cm, col);
    };

    // ── 1. Ground Timber Foundation Cradle (মাচা / Log Machang) ──────
    // Villagers place timber logs on the soil to elevate winter fodder off damp ground
    mat4 l1 = translate(m, vec3(-0.55f, 0.035f, 0.0f));
    l1 = scale(l1, vec3(0.09f, 0.07f, 2.50f));
    Primitives::drawCube(shader, l1, logCol);

    mat4 l2 = translate(m, vec3(0.55f, 0.035f, 0.0f));
    l2 = scale(l2, vec3(0.09f, 0.07f, 2.50f));
    Primitives::drawCube(shader, l2, logCol);

    mat4 l3 = translate(m, vec3(0.0f, 0.045f, -0.65f));
    l3 = scale(l3, vec3(2.50f, 0.07f, 0.09f));
    Primitives::drawCube(shader, l3, logCol * 0.92f);

    mat4 l4 = translate(m, vec3(0.0f, 0.045f, 0.65f));
    l4 = scale(l4, vec3(2.50f, 0.07f, 0.09f));
    Primitives::drawCube(shader, l4, logCol * 0.92f);

    // ── 2. Natural Volumetric Thatched Hayrick Body ───────────────────
    // A) Central bulging core providing internal volume without hollows
    mat4 coreSphere = translate(m, vec3(0.0f, 1.15f, 0.0f));
    coreSphere = scale(coreSphere, vec3(1.15f, 1.10f, 1.15f));
    Primitives::drawSphere(shader, coreSphere, strawWarm);

    // B) Tier 1: Flared Ground Base Cone (Y = 0.05m -> 2.25m, Base R = 1.28m)
    mat4 cone1 = translate(m, vec3(0.0f, 0.05f, 0.0f));
    cone1 = scale(cone1, vec3(2.56f, 2.20f, 2.56f));
    Primitives::drawCone(shader, cone1, strawBaseCol);

    // C) Tier 2: Mid-Belly Overlapping Thatch Shingle (Y = 0.85m -> 2.60m, Base R = 1.16m)
    mat4 cone2 = translate(m, vec3(0.0f, 0.85f, 0.0f));
    cone2 = scale(cone2, vec3(2.32f, 1.75f, 2.32f));
    Primitives::drawCone(shader, cone2, strawWarm);

    // D) Tier 3: Upper Sloping Thatch Cone (Y = 1.65m -> 2.95m, Base R = 0.93m)
    mat4 cone3 = translate(m, vec3(0.0f, 1.65f, 0.0f));
    cone3 = scale(cone3, vec3(1.86f, 1.30f, 1.86f));
    Primitives::drawCone(shader, cone3, strawGold);

    // E) Tier 4: Peak Conical Thatch Cap (Y = 2.25m -> 3.12m, Base R = 0.63m)
    mat4 cone4 = translate(m, vec3(0.0f, 2.25f, 0.0f));
    cone4 = scale(cone4, vec3(1.26f, 0.87f, 1.26f));
    Primitives::drawCone(shader, cone4, strawLight);

    // ── 3. Downward Thatch Fringe & Organic Straw Wisps ───────────────
    // A) Ground splay: 16 loose straw bundles splaying onto the courtyard earth
    for (int i = 0; i < 16; i++) {
        float angle = (float)i * 22.5f + (float)(i % 3) * 2.5f;
        float wlen  = 0.22f + (float)(i % 5) * 0.024f;
        mat4 w = rotate(m, radians(angle), vec3(0.0f, 1.0f, 0.0f));
        w = translate(w, vec3(1.18f, 0.035f, 0.0f));
        w = rotate(w, radians(15.0f + (float)(i % 4) * 3.0f), vec3(0.0f, 0.0f, 1.0f));
        w = scale(w, vec3(wlen, 0.032f, 0.080f));
        vec3 col = (i % 3 == 0) ? strawBaseCol : ((i % 3 == 1) ? strawWarm : strawGold);
        Primitives::drawCube(shader, w, col);
    }

    // B) Tier 1/2 Downward Thatch Shingle Fringe (Y = 0.85m, R = 1.18m)
    for (int i = 0; i < 16; i++) {
        float angle = (float)i * 22.5f + 5.5f;
        float flen  = 0.20f + (float)(i % 4) * 0.025f;
        mat4 f = rotate(m, radians(angle), vec3(0.0f, 1.0f, 0.0f));
        f = translate(f, vec3(1.17f, 0.84f - flen * 0.40f, 0.0f));
        f = rotate(f, radians(-32.0f + (float)(i % 3) * 3.0f), vec3(0.0f, 0.0f, 1.0f));
        f = scale(f, vec3(0.032f, flen, 0.075f));
        vec3 col = (i % 2 == 0) ? strawDeep : strawWarm;
        Primitives::drawCube(shader, f, col);
    }

    // C) Tier 2/3 Downward Thatch Shingle Fringe (Y = 1.65m, R = 0.94m)
    for (int i = 0; i < 14; i++) {
        float angle = (float)i * (360.0f / 14.0f) + 3.0f;
        float flen  = 0.18f + (float)(i % 3) * 0.022f;
        mat4 f = rotate(m, radians(angle), vec3(0.0f, 1.0f, 0.0f));
        f = translate(f, vec3(0.93f, 1.64f - flen * 0.38f, 0.0f));
        f = rotate(f, radians(-38.0f + (float)(i % 3) * 2.5f), vec3(0.0f, 0.0f, 1.0f));
        f = scale(f, vec3(0.028f, flen, 0.065f));
        vec3 col = (i % 2 == 0) ? strawWarm : strawGold;
        Primitives::drawCube(shader, f, col);
    }

    // D) Tier 3/4 Downward Thatch Shingle Fringe (Y = 2.25m, R = 0.64m)
    for (int i = 0; i < 12; i++) {
        float angle = (float)i * 30.0f + 7.5f;
        float flen  = 0.16f + (float)(i % 3) * 0.020f;
        mat4 f = rotate(m, radians(angle), vec3(0.0f, 1.0f, 0.0f));
        f = translate(f, vec3(0.63f, 2.24f - flen * 0.35f, 0.0f));
        f = rotate(f, radians(-44.0f + (float)(i % 3) * 2.0f), vec3(0.0f, 0.0f, 1.0f));
        f = scale(f, vec3(0.024f, flen, 0.055f));
        vec3 col = (i % 2 == 0) ? strawGold : strawLight;
        Primitives::drawCube(shader, f, col);
    }

    // E) 20 Organic Downward Slanted Straw Strands (following the natural cone slope)
    for (int t = 0; t < 20; t++) {
        float angle = (float)t * 18.0f + 4.0f;
        float yPos  = 0.40f + (float)t * 0.12f;
        float rPos  = 1.25f - (yPos / 3.0f) * 0.72f;
        mat4 st = rotate(m, radians(angle), vec3(0.0f, 1.0f, 0.0f));
        st = translate(st, vec3(rPos, yPos, 0.0f));
        st = rotate(st, radians(-42.0f + (float)(t % 5) * 3.0f), vec3(0.0f, 0.0f, 1.0f));
        st = scale(st, vec3(0.024f, 0.28f, 0.048f));
        vec3 col = (t % 3 == 0) ? strawLight : ((t % 3 == 1) ? strawGold : strawWarm);
        Primitives::drawCube(shader, st, col);
    }

    // ── 4. Horizontal Jute Binding Cords (খড়ের বাঁধন / দড়ি) ────────────
    // Three horizontal twisted jute rope rings compressing the packed stack body
    drawCylR(0.68f, 1.18f, 0.024f, ropeJute);
    drawCylR(1.48f, 1.00f, 0.024f, ropeJute);
    drawCylR(2.12f, 0.72f, 0.022f, ropeJute);

    // ── 5. Central Bamboo Stabilizer Pole (বাঁশের খুঁটি) ───────────────
    // Slender seasoned upright bamboo pole extending through the stack up into the sky
    drawCylR(2.05f, 0.030f, 4.10f, bambooCol);

    // Raised bamboo node rings on the exposed upper section
    drawCylR(3.45f, 0.038f, 0.016f, nodeCol);
    drawCylR(3.70f, 0.037f, 0.016f, nodeCol);
    drawCylR(3.95f, 0.036f, 0.016f, nodeCol);

    // ── 6. Iconic Inverted Terracotta Pitcher (উল্টো মাটির কলসি / হাঁড়ি) ─
    // Placed upside-down over the bamboo pole apex to shed monsoon rain
    // A) Wrapped straw collar / neck sheaf (Thuli) beneath the pot mouth
    mat4 thuli = translate(m, vec3(0.0f, 2.82f, 0.0f));
    thuli = scale(thuli, vec3(0.40f, 0.32f, 0.40f));
    Primitives::drawCone(shader, thuli, strawWarm);

    // Jute cord tying the neck sheaf tightly around the bamboo pole
    drawCylR(2.96f, 0.16f, 0.022f, ropeJute);

    // B) Inverted Terracotta Pot Mouth & Rim
    drawCylR(3.07f, 0.155f, 0.035f, potDark);

    // C) Inverted Pot Neck
    drawCylR(3.13f, 0.115f, 0.060f, potDark);

    // D) Inverted Pot Bulbous Clay Body (rounded belly of the earthen pitcher)
    mat4 potBody = translate(m, vec3(0.0f, 3.25f, 0.0f));
    potBody = scale(potBody, vec3(0.22f, 0.20f, 0.22f));
    Primitives::drawSphere(shader, potBody, potTerracotta);

    // E) Inverted Pot Base (flat round bottom of the clay pot facing the sky)
    drawCylR(3.36f, 0.10f, 0.025f, potTerracotta * 0.90f);
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
void drawCow(Shader& shader, const mat4& model, bool lyingDown, const vec3* customHideCol, float walkPhase, bool hasHarness)
{
    shader.setInt("uUseTexture", 0); // smooth natural hide, horns, and muzzle

    vec3 hideCol   = customHideCol ? *customHideCol : vec3(0.64f, 0.44f, 0.26f); // warm fawn-brown coat (or custom)
    vec3 bellyCol;
    if (customHideCol && customHideCol->x > 0.75f) {
        bellyCol = vec3(hideCol.x * 0.95f, hideCol.y * 0.95f, hideCol.z * 0.94f); // light cream-white underbelly
    } else {
        bellyCol = vec3(0.78f, 0.68f, 0.54f); // lighter cream underbelly
    }
    vec3 humpCol   = hideCol * 0.93f;     // distinctive muscular Zebu shoulder hump (Kud / কুঁজ)
    vec3 hornCol   = (customHideCol && customHideCol->x > 0.75f) ? vec3(0.32f, 0.30f, 0.30f) : vec3(0.84f, 0.82f, 0.78f);
    vec3 hornTip   (0.12f, 0.10f, 0.10f); // dark slate/charcoal horn tip
    vec3 muzzleCol (0.18f, 0.16f, 0.15f); // dark moist muzzle
    vec3 hoofCol   (0.14f, 0.12f, 0.10f); // dark cloven hooves
    vec3 eyeCol    (0.06f, 0.06f, 0.06f); // gentle dark eyes
    vec3 eyeWhite  (0.94f, 0.94f, 0.92f); // eye sclera highlight
    vec3 halterCol (0.12f, 0.11f, 0.10f); // dark leather harness straps (মোরলী ও লাগাম)

    mat4 m = model;

    // 1. Torso / Barrel Body
    if (!lyingDown) {
        // Anatomical standing Deshi draft ox: muscular chest, long ribcage barrel, and rounded rump
        mat4 chest = m;
        chest = translate(chest, vec3(0.0f, 0.88f, 0.32f));
        chest = scale(chest, vec3(0.35f, 0.34f, 0.46f));
        Primitives::drawSphere(shader, chest, hideCol);

        mat4 barrel = m;
        barrel = translate(barrel, vec3(0.0f, 0.86f, -0.05f));
        barrel = scale(barrel, vec3(0.33f, 0.33f, 0.55f));
        Primitives::drawSphere(shader, barrel, hideCol);

        mat4 rump = m;
        rump = translate(rump, vec3(0.0f, 0.88f, -0.42f));
        rump = scale(rump, vec3(0.34f, 0.34f, 0.46f));
        Primitives::drawSphere(shader, rump, hideCol);

        // Smooth top backline spine bridge
        mat4 spine = m;
        spine = translate(spine, vec3(0.0f, 0.96f, -0.05f));
        spine = scale(spine, vec3(0.22f, 0.12f, 0.95f));
        Primitives::drawCube(shader, spine, hideCol);

        // Lighter underbelly patch
        mat4 belly = m;
        belly = translate(belly, vec3(0.0f, 0.76f, -0.05f));
        belly = scale(belly, vec3(0.28f, 0.19f, 0.80f));
        Primitives::drawSphere(shader, belly, bellyCol);
    } else {
        mat4 body = m;
        body = translate(body, vec3(0.0f, 0.30f, 0.0f));
        body = scale(body, vec3(0.38f, 0.32f, 0.64f));
        Primitives::drawSphere(shader, body, hideCol);

        mat4 belly = m;
        belly = translate(belly, vec3(0.0f, 0.22f, 0.0f));
        belly = scale(belly, vec3(0.34f, 0.22f, 0.54f));
        Primitives::drawSphere(shader, belly, bellyCol);
    }

    // 2. Iconic Zebu Shoulder Hump (Kud / কুঁজ) — placed right above shoulders
    mat4 hump = m;
    hump = translate(hump, vec3(0.0f, lyingDown ? 0.50f : 1.20f, lyingDown ? 0.14f : 0.30f));
    hump = rotate(hump, radians(lyingDown ? 10.0f : 14.0f), vec3(1.0f, 0.0f, 0.0f));
    hump = scale(hump, vec3(0.22f, 0.28f, 0.26f));
    Primitives::drawSphere(shader, hump, humpCol);

    // 3. Strong Neck rising upward and forward (+Z)
    mat4 neck = m;
    neck = translate(neck, vec3(0.0f, lyingDown ? 0.38f : 1.00f, lyingDown ? 0.36f : 0.50f));
    neck = rotate(neck, radians(lyingDown ? -32.0f : -34.0f), vec3(1.0f, 0.0f, 0.0f));
    mat4 neckS = scale(neck, vec3(0.18f, 0.30f, 0.22f));
    Primitives::drawCylinder(shader, neckS, hideCol);

    // Neck halter collar (Pagha)
    mat4 halter = neck;
    halter = translate(halter, vec3(0.0f, 0.02f, 0.0f));
    halter = scale(halter, vec3(0.19f, 0.035f, 0.23f));
    Primitives::drawCylinder(shader, halter, hasHarness ? halterCol : vec3(0.65f, 0.52f, 0.32f));

    // Throat Dewlap (Golakomblo / গলকম্বল — natural skin folds under throat and brisket)
    mat4 dewlap = neck;
    dewlap = translate(dewlap, vec3(0.0f, -0.12f, 0.04f));
    dewlap = rotate(dewlap, radians(18.0f), vec3(1.0f, 0.0f, 0.0f));
    dewlap = scale(dewlap, vec3(0.024f, 0.20f, 0.22f));
    Primitives::drawCube(shader, dewlap, bellyCol);

    // Brisket dewlap fold connecting into chest
    mat4 brisket = m;
    brisket = translate(brisket, vec3(0.0f, lyingDown ? 0.24f : 0.78f, lyingDown ? 0.24f : 0.44f));
    brisket = scale(brisket, vec3(0.022f, 0.16f, 0.18f));
    Primitives::drawCube(shader, brisket, bellyCol);

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
        ear = rotate(ear, radians(s * 45.0f), vec3(0.0f, 0.0f, 1.0f));
        ear = rotate(ear, radians(-15.0f), vec3(1.0f, 0.0f, 0.0f));
        ear = scale(ear, vec3(0.14f, 0.045f, 0.075f));
        Primitives::drawSphere(shader, ear, hideCol);
    }

    // Pair of curved horns arching upward and forward (matching reference cartoon oxen!)
    for (int s = -1; s <= 1; s += 2) {
        mat4 horn = head;
        horn = translate(horn, vec3(s * 0.09f, 0.14f, -0.02f));
        horn = rotate(horn, radians(s * -14.0f), vec3(0.0f, 0.0f, 1.0f));
        horn = rotate(horn, radians(18.0f), vec3(1.0f, 0.0f, 0.0f));
        mat4 hornBase = scale(horn, vec3(0.034f, 0.20f, 0.034f));
        Primitives::drawCone(shader, hornBase, hornCol);

        // Curved upper horn with dark tip arching forward and inward
        mat4 hTop = horn;
        hTop = translate(hTop, vec3(0.0f, 0.18f, 0.0f));
        hTop = rotate(hTop, radians(s * 14.0f), vec3(0.0f, 0.0f, 1.0f));
        hTop = rotate(hTop, radians(10.0f), vec3(1.0f, 0.0f, 0.0f));
        mat4 hTopS = scale(hTop, vec3(0.024f, 0.18f, 0.024f));
        Primitives::drawCone(shader, hTopS, hornTip);
    }

    // Authentic Domestic Draft Halter Harness (মোরলী ও লাগাম)
    if (hasHarness) {
        // Noseband wrapping around snout
        mat4 nBand = muzzle;
        nBand = translate(nBand, vec3(0.0f, 0.0f, 0.04f));
        nBand = scale(nBand, vec3(0.130f, 0.120f, 0.028f));
        Primitives::drawCube(shader, nBand, halterCol);

        // Crown / poll strap behind horns across cranium
        mat4 cStrap = head;
        cStrap = translate(cStrap, vec3(0.0f, 0.11f, 0.02f));
        cStrap = scale(cStrap, vec3(0.170f, 0.032f, 0.028f));
        Primitives::drawCube(shader, cStrap, halterCol);

        // Cheek straps connecting noseband to crown strap
        for (int s = -1; s <= 1; s += 2) {
            mat4 chStrap = head;
            chStrap = translate(chStrap, vec3(s * 0.082f, 0.04f, 0.08f));
            chStrap = rotate(chStrap, radians(38.0f), vec3(1.0f, 0.0f, 0.0f));
            chStrap = scale(chStrap, vec3(0.018f, 0.15f, 0.020f));
            Primitives::drawCube(shader, chStrap, halterCol);

            // Brass/iron ring toggle on cheek
            mat4 bitRing = head;
            bitRing = translate(bitRing, vec3(s * 0.088f, -0.02f, 0.12f));
            bitRing = scale(bitRing, vec3(0.024f, 0.024f, 0.024f));
            Primitives::drawSphere(shader, bitRing, vec3(0.30f, 0.28f, 0.24f));
        }

        // Throat latch strap under jaw
        mat4 tLatch = head;
        tLatch = translate(tLatch, vec3(0.0f, -0.07f, 0.03f));
        tLatch = scale(tLatch, vec3(0.14f, 0.020f, 0.024f));
        Primitives::drawCube(shader, tLatch, halterCol);
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
        // Naturally proportioned standing legs with thigh, knee, shank, and cloven hooves
        float legX[4] = { -0.19f, 0.19f, -0.19f, 0.19f };
        float legZ[4] = {  0.32f, 0.32f, -0.42f, -0.42f };
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

            // Upper muscular thigh / shoulder
            mat4 upLeg = leg;
            upLeg = translate(upLeg, vec3(0.0f, -0.16f, 0.0f));
            mat4 upS = scale(upLeg, vec3(0.048f, 0.32f, 0.050f));
            Primitives::drawCylinder(shader, upS, hideCol);

            // Knee / hock joint
            mat4 knee = leg;
            knee = translate(knee, vec3(0.0f, -0.34f, 0.005f));
            mat4 knS = scale(knee, vec3(0.052f, 0.052f, 0.054f));
            Primitives::drawSphere(shader, knS, hideCol);

            // Lower slender shank / cannon bone
            mat4 loLeg = leg;
            loLeg = translate(loLeg, vec3(0.0f, -0.54f, 0.0f));
            mat4 loS = scale(loLeg, vec3(0.038f, 0.34f, 0.040f));
            Primitives::drawCylinder(shader, loS, hideCol);

            // Cloven hoof resting flush on ground
            mat4 hoof = leg;
            hoof = translate(hoof, vec3(0.0f, -0.73f, 0.012f));
            mat4 hS = scale(hoof, vec3(0.060f, 0.055f, 0.076f));
            Primitives::drawCube(shader, hS, hoofCol);
        }
    }

    // 6. Tail draped over rear flank (-Z)
    mat4 tail = m;
    tail = translate(tail, vec3(0.0f, lyingDown ? 0.26f : 0.88f, lyingDown ? -0.34f : -0.62f));
    tail = rotate(tail, radians(-20.0f), vec3(1.0f, 0.0f, 0.0f));
    mat4 tailStem = scale(tail, vec3(0.016f, 0.48f, 0.016f));
    Primitives::drawCylinder(shader, tailStem, hideCol);

    mat4 tailTuft = tail;
    tailTuft = translate(tailTuft, vec3(0.0f, -0.26f, 0.0f));
    tailTuft = scale(tailTuft, vec3(0.038f, 0.14f, 0.038f));
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
    spout = translate(spout, vec3(0.0f, 0.60f, 0.18f));
    spout = rotate(spout, radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
    mat4 spoutCyl = scale(spout, vec3(0.045f, 0.28f, 0.045f));
    Primitives::drawCylinder(shader, spoutCyl, ironGreen);

    mat4 spoutLip = m;
    spoutLip = translate(spoutLip, vec3(0.0f, 0.56f, 0.32f));
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
        shader.setFloat("emissive", 0.45f); // glistening fresh water under moonlight/sunlight

        // Vertical cascading water stream from spout mouth directly into Kolshi
        mat4 stream = m;
        stream = translate(stream, vec3(0.0f, 0.48f, 0.32f));
        stream = scale(stream, vec3(0.040f, 0.16f, 0.040f));
        Primitives::drawCylinder(shader, stream, waterStream);

        // Splashing water froth at the mouth of the Kolshi
        mat4 splash = m;
        splash = translate(splash, vec3(0.0f, 0.41f, 0.32f));
        splash = scale(splash, vec3(0.13f, 0.025f, 0.13f));
        Primitives::drawCylinder(shader, splash, vec3(0.88f, 0.95f, 1.0f));

        shader.setFloat("emissive", 0.0f);
    }

    // 10. Traditional Clay Water Pitcher (Kolshi) placed squarely under spout
    drawKolshi(shader, m, vec3(0.0f, 0.12f, 0.32f), 0.66f);
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
// Elevated traditional rural Bengali paddy granary / storehouse (Dhaner Gola / ধানের গোলা)
// Authentic bulging clay-and-cowdung plastered woven bamboo silo (Dhol) on timber stilts (Macha)
// with an overhanging steep rice straw conical thatched roof (Khorer Mathal), wooden grain hatch, and bamboo ladder.
void drawGranary(Shader& shader, const mat4& model)
{
    shader.setInt("uUseTexture", 0);

    // ── Authentic Rural Materials Palette ────────────────────────
    static const vec3 stiltCol    (0.32f, 0.20f, 0.12f); // seasoned sal-timber / bamboo stilt poles
    static const vec3 footingCol  (0.48f, 0.38f, 0.28f); // burnt clay / stone anti-moisture footing plinths
    static const vec3 beamCol     (0.36f, 0.24f, 0.14f); // timber bearer cross-beams
    static const vec3 floorCol    (0.46f, 0.36f, 0.22f); // aged bamboo slatted platform deck
    static const vec3 bodyClay    (0.66f, 0.54f, 0.34f); // sun-dried clay & cow-dung mud plaster (Matir o gobar lepa)
    static const vec3 bodyShadow  (0.60f, 0.48f, 0.28f); // shaded clay contour
    static const vec3 hoopCol     (0.38f, 0.24f, 0.12f); // dark weathered split-bamboo binding rings (Batar Ber)
    static const vec3 roofStraw   (0.74f, 0.60f, 0.26f); // golden sunlit rice straw thatch cone
    static const vec3 roofEave    (0.60f, 0.46f, 0.20f); // weathered drooping thatch eave skirt fringe
    static const vec3 kalsiCol    (0.62f, 0.30f, 0.16f); // inverted terracotta pot finial cap (Ulto Kalsi)
    static const vec3 hatchFrame  (0.26f, 0.15f, 0.08f); // dark timber grain hatch frame
    static const vec3 hatchDoor   (0.48f, 0.36f, 0.20f); // woven bamboo hatch shutter panel
    static const vec3 latchCol    (0.36f, 0.22f, 0.10f); // bamboo sliding latch bar (Khil)
    static const vec3 ladderCol   (0.50f, 0.38f, 0.20f); // weathered bamboo ladder poles and rungs

    const float floorY = 0.55f; // elevated stilt height above ground
    const float bodyH  = 1.40f; // height of the woven cylindrical basket

    // Helper: draw smooth 16-faceted circular cylinder using 8 rotated unit cubes
    auto drawSmoothCyl = [&](const mat4& parent, float r, float h, const vec3& col) {
        const float chord = r * 0.40f;
        for (int k = 0; k < 8; ++k) {
            mat4 c = rotate(parent, radians((float)k * 22.5f), vec3(0.0f, 1.0f, 0.0f));
            c = scale(c, vec3(r * 2.0f, h, chord));
            Primitives::drawCube(shader, c, col);
        }
    };

    // ── 1. Elevated Timber & Bamboo Stilt Foundation (Macha / মাচা) ───
    // 4 sturdy timber stilt posts elevating granary above ground moisture and rodents
    const float postOffsets[4][2] = { {-0.55f, -0.55f}, {0.55f, -0.55f}, {-0.55f, 0.55f}, {0.55f, 0.55f} };
    for (int i = 0; i < 4; ++i) {
        // Anti-moisture burnt stone/clay footings at ground level
        mat4 foot = translate(model, vec3(postOffsets[i][0], 0.04f, postOffsets[i][1]));
        foot = scale(foot, vec3(0.18f, 0.08f, 0.18f));
        Primitives::drawCube(shader, foot, footingCol);

        // Sturdy upright timber stilt post
        mat4 stilt = translate(model, vec3(postOffsets[i][0], floorY * 0.5f, postOffsets[i][1]));
        stilt = scale(stilt, vec3(0.095f, floorY, 0.095f));
        Primitives::drawCube(shader, stilt, stiltCol);
    }

    // Heavy cross bearer timber beams underneath the floor (Arah & Ruya)
    for (float bz : {-0.55f, 0.55f}) {
        mat4 beamX = translate(model, vec3(0.0f, floorY - 0.045f, bz));
        beamX = scale(beamX, vec3(1.35f, 0.08f, 0.10f));
        Primitives::drawCube(shader, beamX, beamCol);
    }
    for (float bx : {-0.55f, 0.0f, 0.55f}) {
        mat4 beamZ = translate(model, vec3(bx, floorY - 0.015f, 0.0f));
        beamZ = scale(beamZ, vec3(0.08f, 0.06f, 1.35f));
        Primitives::drawCube(shader, beamZ, beamCol * 1.05f);
    }

    // Circular elevated bamboo platform deck (Macha)
    mat4 platform = translate(model, vec3(0.0f, floorY, 0.0f));
    drawSmoothCyl(platform, 0.90f, 0.05f, floorCol);

    // ── 2. Authentic Rounded & Bulging Granary Basket (Dhaner Dhol / গোলা) ─
    // Organic barrel curvature: narrower at base and neck, gracefully bulging in the belly
    // Lower tier (base to lower belly)
    mat4 bodyLow = translate(model, vec3(0.0f, floorY + 0.22f, 0.0f));
    drawSmoothCyl(bodyLow, 0.86f, 0.44f, bodyShadow);

    // Middle bulging belly (where stored paddy exerts maximum outward pressure)
    mat4 bodyMid = translate(model, vec3(0.0f, floorY + 0.70f, 0.0f));
    drawSmoothCyl(bodyMid, 0.92f, 0.60f, bodyClay);

    // Upper tier (upper belly tapering toward roof neck)
    mat4 bodyHigh = translate(model, vec3(0.0f, floorY + 1.15f, 0.0f));
    drawSmoothCyl(bodyHigh, 0.86f, 0.40f, bodyClay * 1.03f);

    // ── 3. Woven Bamboo Reinforcing Hoops (Batar Ber / বাঁশের বাতা) ───────
    // 4 slender binding rings wrapped horizontally to prevent basket bursting
    const float hoopY[4] = { floorY + 0.12f, floorY + 0.52f, floorY + 0.92f, floorY + 1.35f };
    const float hoopR[4] = { 0.88f,          0.94f,          0.94f,          0.88f          };
    for (int h = 0; h < 4; ++h) {
        mat4 hoop = translate(model, vec3(0.0f, hoopY[h], 0.0f));
        drawSmoothCyl(hoop, hoopR[h], 0.040f, hoopCol);
    }

    // ── 4. Traditional Conical Thatched Roof (Khorer Mathal / খড়ের চাল) ──
    // Generously overhanging the body on all sides (protects mud walls from monsoon rain)
    const float roofR = 1.25f; // overhanging eave radius: 0.39m wider than granary body!
    const float roofH = 1.45f; // steep conical pitch
    const float roofBaseY = floorY + bodyH - 0.08f; // starts below top of walls (covers rim completely!)

    // Drooping lower thatch eave skirt fringe (Chaler Chhanch / Karish)
    mat4 eaveSkirt = translate(model, vec3(0.0f, roofBaseY - 0.08f, 0.0f));
    eaveSkirt = scale(eaveSkirt, vec3(1.32f * 2.0f, 0.24f, 1.32f * 2.0f));
    Primitives::drawCone(shader, eaveSkirt, roofEave);

    // Main steep golden rice straw conical roof
    mat4 roof = translate(model, vec3(0.0f, roofBaseY, 0.0f));
    roof = scale(roof, vec3(roofR * 2.0f, roofH, roofR * 2.0f));
    Primitives::drawCone(shader, roof, roofStraw);

    // Inverted terracotta clay pitcher finial cap (Ulto Matir Kalsi / Mathal) at apex
    mat4 kalsi = translate(model, vec3(0.0f, roofBaseY + roofH + 0.02f, 0.0f));
    kalsi = scale(kalsi, vec3(0.18f, 0.15f, 0.18f));
    Primitives::drawSphere(shader, kalsi, kalsiCol);

    // Weathered vertical bamboo finial pin rising through the apex cap
    mat4 apexPin = translate(model, vec3(0.0f, roofBaseY + roofH + 0.10f, 0.0f));
    apexPin = scale(apexPin, vec3(0.035f, 0.28f, 0.035f));
    Primitives::drawCube(shader, apexPin, hoopCol);

    // ── 5. Wooden Grain Access Hatch (Dhaner Khirki / ধানের খিরকি) ───────
    // Traditional small square hatch on the front face where paddy is loaded/unloaded
    const float hatchY = floorY + 0.58f;
    const float hatchZ = 0.93f; // front belly perimeter

    // Dark timber hatch outer frame
    mat4 hFrame = translate(model, vec3(0.0f, hatchY, hatchZ));
    hFrame = scale(hFrame, vec3(0.44f, 0.52f, 0.05f));
    Primitives::drawCube(shader, hFrame, hatchFrame);

    // Woven bamboo hatch door panel (slightly recessed)
    mat4 hDoor = translate(model, vec3(0.0f, hatchY, hatchZ + 0.015f));
    hDoor = scale(hDoor, vec3(0.34f, 0.42f, 0.03f));
    Primitives::drawCube(shader, hDoor, hatchDoor);

    // Horizontal bamboo sliding latch bar (Khil)
    mat4 hLatch = translate(model, vec3(0.0f, hatchY, hatchZ + 0.035f));
    hLatch = scale(hLatch, vec3(0.42f, 0.035f, 0.035f));
    Primitives::drawCube(shader, hLatch, latchCol);

    // ── 6. Rustic Leaning Bamboo Ladder (Bansher Moi / বাঁশের মই) ─────────
    // Slender rustic ladder resting against platform edge for climbing up to hatch
    mat4 ladder = translate(model, vec3(0.28f, floorY * 0.5f, 0.98f));
    ladder = rotate(ladder, radians(-18.0f), vec3(1.0f, 0.0f, 0.0f));
    ladder = rotate(ladder, radians(8.0f),   vec3(0.0f, 1.0f, 0.0f));

    // Two bamboo side poles
    for (float lx : {-0.14f, 0.14f}) {
        mat4 rail = translate(ladder, vec3(lx, 0.0f, 0.0f));
        rail = scale(rail, vec3(0.032f, floorY * 1.25f, 0.032f));
        Primitives::drawCylinder(shader, rail, ladderCol);
    }
    // Bamboo rungs
    for (float ry = -floorY * 0.35f; ry <= floorY * 0.40f; ry += 0.20f) {
        mat4 rung = translate(ladder, vec3(0.0f, ry, 0.0f));
        rung = rotate(rung, radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
        rung = scale(rung, vec3(0.024f, 0.28f, 0.024f));
        Primitives::drawCylinder(shader, rung, ladderCol * 0.92f);
    }

    // ── 7. Terracotta Measuring Pitcher on Platform Edge ───────────────────
    mat4 kolshiM = translate(model, vec3(-0.48f, floorY + 0.12f, 0.65f));
    kolshiM = scale(kolshiM, vec3(0.55f));
    drawKolshi(shader, kolshiM);
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

    const vec3 woodDark    (0.42f, 0.27f, 0.14f); // weathered sal timber axle & frame
    const vec3 woodWarm    (0.58f, 0.38f, 0.18f); // rich warm timber for tongue, spokes, posts
    const vec3 woodDeck    (0.66f, 0.48f, 0.26f); // floor planks
    const vec3 ironCol     (0.20f, 0.19f, 0.18f); // iron rim tyre, linchpins, hub rings
    const vec3 chhoiCol    (0.77f, 0.64f, 0.42f); // woven bamboo mat canopy (চাটাইয়ের ছই)
    const vec3 chhoiAlt    (0.73f, 0.60f, 0.38f); // subtle alternating bamboo weave tone
    const vec3 hoopCol     (0.48f, 0.35f, 0.18f); // bent structural bamboo hoop ribs
    const vec3 battenCol   (0.54f, 0.40f, 0.20f); // longitudinal bamboo battens
    const vec3 tieCol      (0.24f, 0.17f, 0.10f); // dark rattan/coir binding ties
    const vec3 strawCol    (0.86f, 0.74f, 0.32f); // golden rice straw
    const vec3 sackCol     (0.60f, 0.48f, 0.30f); // jute burlap cargo sacks
    const vec3 ropeCol     (0.66f, 0.54f, 0.34f); // jute coir hitch ropes & reins
    const vec3 oxWhite1    (0.93f, 0.93f, 0.92f); // Left Deshi bullock: pure white/light grey
    const vec3 oxWhite2    (0.90f, 0.90f, 0.91f); // Right Deshi bullock: pure white/light grey

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
            Primitives::drawCylinder(shader, spoke, woodWarm);

            // Auspicious folk cloth ribbon marker tied to spoke 0 (লাল শালু কাপড় / ফিতা)
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
    const float bedW = 1.22f;
    const float bedL = 2.45f;
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

    // Floor deck planks extending forward into driver seating area
    mat4 floorDeck = model;
    floorDeck = translate(floorDeck, vec3(0.0f, bedY + 0.025f, -0.20f));
    floorDeck = scale(floorDeck, vec3(bedW, 0.025f, bedL));
    Primitives::drawCube(shader, floorDeck, woodDeck);

    // Front platform extension plank where driver sits
    mat4 driverDeck = model;
    driverDeck = translate(driverDeck, vec3(0.0f, bedY + 0.025f, 0.50f));
    driverDeck = scale(driverDeck, vec3(bedW * 0.85f, 0.028f, 0.65f));
    Primitives::drawCube(shader, driverDeck, woodDeck * 0.96f);

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
            Primitives::drawCube(shader, rail, woodWarm);
        }
    }

    // ── 4. Traditional Arched Woven Bamboo Canopy (বাঁশের ছই / Chhoi) ─
    // Tall vaulted semicircular woven bamboo hood with arched ribs, closed back, and arched front opening
    const float hoodZStart = -1.35f;
    const float hoodZEnd   =  0.05f;                 // front edge of canopy, leaves spacious front deck for driver
    const float hoodLen    = hoodZEnd - hoodZStart; // 1.40m
    const float hoodR      = 0.66f;                 // generous vaulted arch radius
    const float wallH      = 0.52f;                 // tall vertical side walls (1.18m total interior height!)
    const float archCenterY= bedY + wallH;          // base height of semicircle arch (0.79 + 0.52 = 1.31m)
    const int   archSegs   = 48;                    // 48 dense facets (ultra smooth curved mat, zero gaps)

    // A. Side Vertical Woven Bamboo Walls
    for (float side : { -1.0f, 1.0f }) {
        mat4 sideWall = model;
        sideWall = translate(sideWall, vec3(side * (hoodR - 0.01f), bedY + wallH * 0.5f, (hoodZStart + hoodZEnd) * 0.5f));
        sideWall = scale(sideWall, vec3(0.018f, wallH, hoodLen));
        Primitives::drawCube(shader, sideWall, chhoiCol);
    }

    // B. Semicircular Solid Woven Bamboo Arch Shell (smooth golden bamboo mat, exact tangent angles, zero gaps)
    for (int a = 0; a < archSegs; ++a) {
        float a0 = (float)a * (PI / (float)archSegs);
        float a1 = (float)(a + 1) * (PI / (float)archSegs);
        float aMid = (a0 + a1) * 0.5f;

        float ax = -hoodR * cosf(aMid);
        float ay = archCenterY + hoodR * sinf(aMid);
        float segW = 2.0f * hoodR * sinf((a1 - a0) * 0.5f) * 1.05f; // gapless seamless overlap
        float tiltAng = atan2f(cosf(aMid), sinf(aMid));             // mathematically exact tangent angle

        mat4 skinSeg = model;
        skinSeg = translate(skinSeg, vec3(ax, ay, (hoodZStart + hoodZEnd) * 0.5f));
        skinSeg = rotate(skinSeg, tiltAng, vec3(0.0f, 0.0f, 1.0f));
        skinSeg = scale(skinSeg, vec3(segW, 0.012f, hoodLen));
        Primitives::drawCube(shader, skinSeg, chhoiCol);
    }

    // C. Closed Arched Rear Wall (পেছনের ছাতার দেয়াল)
    // Lower rectangular back wall
    mat4 rearLower = model;
    rearLower = translate(rearLower, vec3(0.0f, bedY + wallH * 0.5f, hoodZStart));
    rearLower = scale(rearLower, vec3(hoodR * 2.0f, wallH, 0.025f));
    Primitives::drawCube(shader, rearLower, chhoiCol * 0.94f);

    // Rear upper arched wall fan
    const int rearWallSegs = 24;
    for (int rw = 0; rw < rearWallSegs; ++rw) {
        float rx = -hoodR * 0.97f + (float)rw * (hoodR * 1.94f / (float)(rearWallSegs - 1));
        float maxY = sqrtf(fmaxf(0.0f, hoodR * hoodR - rx * rx));
        if (maxY > 0.03f) {
            mat4 rSlat = model;
            rSlat = translate(rSlat, vec3(rx, archCenterY + maxY * 0.5f, hoodZStart));
            rSlat = scale(rSlat, vec3(hoodR * 1.94f / (float)rearWallSegs * 1.06f, maxY, 0.018f));
            Primitives::drawCube(shader, rSlat, chhoiCol * 0.92f);
        }
    }
    // Rear wall bamboo cross-braces
    mat4 rCrossH = model;
    rCrossH = translate(rCrossH, vec3(0.0f, archCenterY + 0.12f, hoodZStart - 0.015f));
    rCrossH = scale(rCrossH, vec3(hoodR * 1.90f, 0.032f, 0.032f));
    Primitives::drawCube(shader, rCrossH, hoopCol);

    mat4 rCrossV = model;
    rCrossV = translate(rCrossV, vec3(0.0f, archCenterY + hoodR * 0.45f, hoodZStart - 0.015f));
    rCrossV = scale(rCrossV, vec3(0.032f, hoodR * 0.90f, 0.032f));
    Primitives::drawCube(shader, rCrossV, hoopCol);

    // D. 4 Prominent Curved Bamboo Hoops (বাঁশের বাঁক) hugging exterior
    float hoopZPositions[4] = { hoodZEnd, hoodZStart + hoodLen * 0.66f, hoodZStart + hoodLen * 0.33f, hoodZStart };
    for (int h = 0; h < 4; ++h) {
        float hz = hoopZPositions[h];
        bool isRim = (h == 0 || h == 3);
        float hoopThick = isRim ? 0.048f : 0.038f;
        float hoopRadial = isRim ? 0.042f : 0.032f;

        // Side upright bamboo poles
        for (float side : { -1.0f, 1.0f }) {
            mat4 hPole = model;
            hPole = translate(hPole, vec3(side * (hoodR + hoopRadial * 0.5f), bedY + wallH * 0.5f, hz));
            hPole = scale(hPole, vec3(hoopRadial, wallH, hoopThick));
            Primitives::drawCube(shader, hPole, hoopCol);
        }

        // Curved arch segments hugging the outer surface with exact tangent rotation
        for (int a = 0; a < 36; ++a) {
            float aMid = ((float)a + 0.5f) * (PI / 36.0f);
            float ax = -(hoodR + hoopRadial * 0.5f) * cosf(aMid);
            float ay = archCenterY + (hoodR + hoopRadial * 0.5f) * sinf(aMid);
            float segW = 2.0f * (hoodR + hoopRadial * 0.5f) * sinf(PI / 72.0f) * 1.05f;
            float tiltAng = atan2f(cosf(aMid), sinf(aMid));

            mat4 hoopSeg = model;
            hoopSeg = translate(hoopSeg, vec3(ax, ay, hz));
            hoopSeg = rotate(hoopSeg, tiltAng, vec3(0.0f, 0.0f, 1.0f));
            hoopSeg = scale(hoopSeg, vec3(segW, hoopRadial, hoopThick));
            Primitives::drawCube(shader, hoopSeg, hoopCol);
        }
    }

    // E. 5 Longitudinal Bamboo Battens (পড়কা) running along canopy (flush with ends)
    float battenAngles[5] = { 0.0f, PI * 0.25f, PI * 0.50f, PI * 0.75f, PI };
    for (int b = 0; b < 5; ++b) {
        float bAng = battenAngles[b];
        float bx = -(hoodR + 0.024f) * cosf(bAng);
        float by = (bAng == 0.0f || bAng == PI) ? (bedY + wallH) : (archCenterY + (hoodR + 0.024f) * sinf(bAng));

        mat4 batten = model;
        batten = translate(batten, vec3(bx, by, (hoodZStart + hoodZEnd) * 0.5f));
        batten = scale(batten, vec3(0.028f, 0.028f, hoodLen));
        Primitives::drawCube(shader, batten, battenCol);

        // Binding rattan knots at every hoop crossing
        for (int h = 0; h < 4; ++h) {
            mat4 knot = model;
            knot = translate(knot, vec3(bx, by, hoopZPositions[h]));
            knot = scale(knot, vec3(0.038f, 0.038f, 0.048f));
            Primitives::drawCube(shader, knot, tieCol);
        }
    }

    // ── 5. Inside the Canopy (Passenger & Cargo) ────────────────
    // Woven straw/jute floor sitting mat inside
    mat4 floorMat = model;
    floorMat = translate(floorMat, vec3(0.0f, bedY + 0.035f, -0.65f));
    floorMat = scale(floorMat, vec3(hoodR * 1.85f, 0.012f, hoodLen * 0.92f));
    Primitives::drawCube(shader, floorMat, vec3(0.68f, 0.56f, 0.36f));

    // Seated Passenger inside the shaded canopy (clearly framed through arched opening, like reference image 3)
    PersonParams passenger;
    passenger.skinColor   = vec3(0.55f, 0.38f, 0.25f);
    passenger.shirtColor  = vec3(0.24f, 0.44f, 0.82f); // Bengali blue kurta (matches reference image 3)
    passenger.pantsColor  = vec3(0.86f, 0.84f, 0.78f); // cream pajama/dhoti
    passenger.seated      = true;
    passenger.hasGamcha   = true;
    passenger.gamchaColor = vec3(0.94f, 0.40f, 0.10f); // orange/saffron gamcha
    passenger.leftArmAngle= radians(18.0f);
    passenger.rightArmAngle = radians(28.0f);

    mat4 passM = model;
    passM = translate(passM, vec3(0.12f, bedY + 0.02f, -0.32f));
    passM = scale(passM, vec3(1.10f));
    passM = rotate(passM, radians(-20.0f), vec3(0.0f, 1.0f, 0.0f)); // looking out toward front opening
    Person::draw(shader, passM, passenger);

    // Rear cargo: stacked golden rice straw bundles (বিচালি)
    mat4 straw1 = model;
    straw1 = translate(straw1, vec3(-0.15f, bedY + 0.18f, -0.92f));
    straw1 = scale(straw1, vec3(0.55f, 0.26f, 0.55f));
    Primitives::drawCube(shader, straw1, strawCol);

    // Jute grain sacks (পাটের বস্তা) at rear
    mat4 sack1 = model;
    sack1 = translate(sack1, vec3(0.20f, bedY + 0.16f, -0.95f));
    sack1 = scale(sack1, vec3(0.24f, 0.22f, 0.30f));
    Primitives::drawSphere(shader, sack1, sackCol);

    // Terracotta water pitcher (Kolshi) lashed to the rear corner
    mat4 kolshiM = model;
    kolshiM = translate(kolshiM, vec3(bedW * 0.36f, bedY + 0.14f, -1.18f));
    kolshiM = scale(kolshiM, vec3(0.65f));
    drawKolshi(shader, kolshiM);

    // ── 6. Long A-Frame Draft Pole & Tongue (ইশাল / ধুরা) ────────
    const float yokeY = 1.05f;
    const float yokeZ = 2.35f;

    for (int s = -1; s <= 1; s += 2) {
        float fs = (float)s;
        mat4 pole = model;
        float px0 = fs * 0.45f, py0 = bedY, pz0 = -0.20f;
        float px1 = fs * 0.12f, py1 = yokeY, pz1 = yokeZ;
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
        Primitives::drawCube(shader, pole, woodDark);
    }

    // Central tongue tip extension with iron hitch ring
    mat4 tongueTip = model;
    tongueTip = translate(tongueTip, vec3(0.0f, yokeY, yokeZ + 0.08f));
    tongueTip = scale(tongueTip, vec3(0.09f, 0.08f, 0.25f));
    Primitives::drawCube(shader, tongueTip, woodDark);

    // ── 7. The Yoke (জোয়াল / Joyal) ──────────────────────────────
    // Transverse curved wooden beam spanning across both bullocks
    mat4 yoke = model;
    yoke = translate(yoke, vec3(0.0f, yokeY, yokeZ));
    yoke = scale(yoke, vec3(1.72f, 0.075f, 0.085f));
    Primitives::drawCube(shader, yoke, woodDark);

    // Curved neck rests on yoke over each bullock
    for (float yx : { -0.62f, 0.62f }) {
        mat4 neckRest = model;
        neckRest = translate(neckRest, vec3(yx, yokeY - 0.035f, yokeZ));
        neckRest = scale(neckRest, vec3(0.32f, 0.045f, 0.095f));
        Primitives::drawCube(shader, neckRest, woodWarm);
    }

    // 4 Vertical wooden yoke pins (জুঁতি / Khuti) hanging down
    for (float kx : { -0.82f, -0.42f, 0.42f, 0.82f }) {
        mat4 pin = model;
        pin = translate(pin, vec3(kx, yokeY - 0.16f, yokeZ));
        pin = scale(pin, vec3(0.035f, 0.32f, 0.035f));
        Primitives::drawCylinder(shader, pin, woodWarm);
    }

    // Jute coir tie ropes binding the yoke to the tongue
    mat4 yokeRope = model;
    yokeRope = translate(yokeRope, vec3(0.0f, yokeY, yokeZ));
    yokeRope = scale(yokeRope, vec3(0.18f, 0.10f, 0.16f));
    Primitives::drawCube(shader, yokeRope, ropeCol);

    // ── 8. The Bullock Cart Driver (গাড়িয়াল / Gariyal) ───────────
    // Sits outside on the front platform in front of the arched canopy (like reference images)
    PersonParams gariyal;
    gariyal.skinColor   = vec3(0.55f, 0.38f, 0.25f);
    gariyal.shirtColor  = vec3(0.92f, 0.90f, 0.82f); // traditional cream kurta / shirt
    gariyal.pantsColor  = vec3(0.82f, 0.80f, 0.74f); // light dhoti / lungi
    gariyal.seated      = true;
    gariyal.hasGamcha   = true;
    gariyal.gamchaColor = vec3(0.94f, 0.36f, 0.10f); // vibrant orange gamcha draped over shoulder
    gariyal.rightArmAngle = radians(35.0f);          // holding driving stick forward
    gariyal.leftArmAngle  = radians(25.0f);          // holding driving reins forward

    mat4 gariyalM = model;
    gariyalM = translate(gariyalM, vec3(-0.06f, bedY + 0.02f, 0.44f));
    gariyalM = scale(gariyalM, vec3(1.12f));
    if (walkPhase != 0.0f) {
        float driverSway = sinf(walkPhase * 2.0f) * radians(3.0f);
        gariyalM = rotate(gariyalM, driverSway, vec3(1.0f, 0.0f, 0.0f));
    }
    Person::draw(shader, gariyalM, gariyal);

    // Bamboo driving stick (পাঁচনি / Pachni) held in cartman's right hand
    mat4 stick = model;
    stick = translate(stick, vec3(0.18f, bedY + 0.42f, 0.75f));
    stick = rotate(stick, radians(-25.0f), vec3(1.0f, 0.0f, 0.0f));
    stick = scale(stick, vec3(0.020f, 0.020f, 0.85f));
    Primitives::drawCube(shader, stick, woodWarm);

    // Driving reins extending from bullocks' bits to the cart driver's hands
    // Left bullock rein
    {
        mat4 reinL = model;
        float rx0 = -0.62f, ry0 = 0.98f, rz0 = 2.70f;
        float rx1 = -0.06f, ry1 = bedY + 0.38f, rz1 = 0.60f;
        float rmx = (rx0 + rx1) * 0.5f, rmy = (ry0 + ry1) * 0.5f, rmz = (rz0 + rz1) * 0.5f;
        float rdx = rx1 - rx0, rdy = ry1 - ry0, rdz = rz1 - rz0;
        float rlen = sqrtf(rdx * rdx + rdy * rdy + rdz * rdz);
        float ryaw = atan2f(rdx, rdz);
        float rpitch = -atan2f(rdy, sqrtf(rdx * rdx + rdz * rdz));
        reinL = translate(reinL, vec3(rmx, rmy, rmz));
        reinL = rotate(reinL, ryaw, vec3(0.0f, 1.0f, 0.0f));
        reinL = rotate(reinL, rpitch, vec3(1.0f, 0.0f, 0.0f));
        reinL = scale(reinL, vec3(0.012f, 0.012f, rlen));
        Primitives::drawCube(shader, reinL, ropeCol);
    }
    // Right bullock rein
    {
        mat4 reinR = model;
        float rx0 = 0.62f, ry0 = 0.98f, rz0 = 2.70f;
        float rx1 = 0.08f, ry1 = bedY + 0.38f, rz1 = 0.60f;
        float rmx = (rx0 + rx1) * 0.5f, rmy = (ry0 + ry1) * 0.5f, rmz = (rz0 + rz1) * 0.5f;
        float rdx = rx1 - rx0, rdy = ry1 - ry0, rdz = rz1 - rz0;
        float rlen = sqrtf(rdx * rdx + rdy * rdy + rdz * rdz);
        float ryaw = atan2f(rdx, rdz);
        float rpitch = -atan2f(rdy, sqrtf(rdx * rdx + rdz * rdz));
        reinR = translate(reinR, vec3(rmx, rmy, rmz));
        reinR = rotate(reinR, ryaw, vec3(0.0f, 1.0f, 0.0f));
        reinR = rotate(reinR, rpitch, vec3(1.0f, 0.0f, 0.0f));
        reinR = scale(reinR, vec3(0.012f, 0.012f, rlen));
        Primitives::drawCube(shader, reinR, ropeCol);
    }

    // ── 9. Pair of White Deshi Draft Bullocks (এক জোড়া সাদা বলদ গরু) ─
    // Both bullocks are pure white/light grey with humps, curved horns, and harness (like reference images)
    mat4 leftOx = model;
    leftOx = translate(leftOx, vec3(-0.62f, 0.0f, 2.05f));
    drawCow(shader, leftOx, false, &oxWhite1, walkPhase, true);

    mat4 rightOx = model;
    rightOx = translate(rightOx, vec3(0.62f, 0.0f, 2.05f));
    drawCow(shader, rightOx, false, &oxWhite2, walkPhase + 0.35f, true);

    // Harness ropes extending from yoke to oxen halters
    for (float oxX : { -0.62f, 0.62f }) {
        mat4 harnessRope = model;
        harnessRope = translate(harnessRope, vec3(oxX, 0.90f, yokeZ));
        harnessRope = rotate(harnessRope, radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
        harnessRope = scale(harnessRope, vec3(0.022f, 0.022f, 0.32f));
        Primitives::drawCylinder(shader, harnessRope, ropeCol);
    }
}

// ═════════════════════════════════════════════════════════════════════
// 1. TRADITIONAL RURAL ROADSIDE TEA STALL (চা-এর দোকান / Cha-er Dokan)
// ═════════════════════════════════════════════════════════════════════
void drawTeaStall(Shader& shader, const mat4& model, float animTime)
{
    shader.setInt("uUseTexture", 0);

    vec3 woodPlank (0.42f, 0.30f, 0.16f); // weathered timber planks
    vec3 bambooPost(0.52f, 0.44f, 0.22f); // seasoned bamboo posts
    vec3 tinRoof   (0.56f, 0.58f, 0.60f); // corrugated galvanized tin roof
    vec3 benchWood (0.36f, 0.24f, 0.12f); // dark timber seating bench
    vec3 kettleCol (0.82f, 0.84f, 0.86f); // polished aluminum boiling kettle (Ketli)
    vec3 clayPotCol(0.68f, 0.36f, 0.18f); // earthen clay tea cups (Matir Bhar)
    vec3 glassCol  (0.85f, 0.92f, 0.95f); // glass chai tumblers

    // 1. Raised earthen/timber platform plinth
    mat4 plinth = model;
    plinth = translate(plinth, vec3(0.0f, 0.10f, 0.0f));
    plinth = scale(plinth, vec3(3.4f, 0.20f, 2.4f));
    Primitives::drawCube(shader, plinth, woodPlank);

    // 2. Four upright bamboo corner posts
    float postH = 2.1f;
    float hx = 1.55f, hz = 1.05f;
    float posts[4][2] = { {-hx, -hz}, {hx, -hz}, {-hx, hz}, {hx, hz} };
    for (int i = 0; i < 4; i++) {
        mat4 p = model;
        p = translate(p, vec3(posts[i][0], postH * 0.5f, posts[i][1]));
        p = scale(p, vec3(0.065f, postH, 0.065f));
        Primitives::drawCylinder(shader, p, bambooPost);
    }

    // 3. Sloping tin awning roof (sloping down forward towards +Z)
    mat4 roof = model;
    roof = translate(roof, vec3(0.0f, postH + 0.12f, 0.15f));
    roof = rotate(roof, radians(14.0f), vec3(1.0f, 0.0f, 0.0f));
    roof = scale(roof, vec3(3.6f, 0.045f, 2.7f));
    Primitives::drawCube(shader, roof, tinRoof);

    // 4. Wooden Service Counter (facing customer bench at +Z)
    mat4 counter = model;
    counter = translate(counter, vec3(0.0f, 0.72f, 0.55f));
    counter = scale(counter, vec3(2.8f, 0.08f, 0.65f));
    Primitives::drawCube(shader, counter, woodPlank);

    // Counter support vertical posts
    for (float cx : { -1.3f, 0.0f, 1.3f }) {
        mat4 cp = model;
        cp = translate(cp, vec3(cx, 0.36f, 0.55f));
        cp = scale(cp, vec3(0.06f, 0.72f, 0.55f));
        Primitives::drawCube(shader, cp, woodPlank * 0.9f);
    }

    // 5. Customer Seating Bench along front (+Z)
    mat4 bench = model;
    bench = translate(bench, vec3(0.0f, 0.40f, 1.25f));
    bench = scale(bench, vec3(2.8f, 0.06f, 0.35f));
    Primitives::drawCube(shader, bench, benchWood);

    // 6. Large Boiling Aluminum Tea Kettle (Ketli / কেটলি) with animated steam puffs
    mat4 kettle = model;
    kettle = translate(kettle, vec3(-0.65f, 0.88f, 0.55f));
    kettle = scale(kettle, vec3(0.32f, 0.30f, 0.32f));
    Primitives::drawSphere(shader, kettle, kettleCol);

    // Kettle spout
    mat4 spout = model;
    spout = translate(spout, vec3(-0.52f, 0.94f, 0.55f));
    spout = rotate(spout, radians(-40.0f), vec3(0.0f, 0.0f, 1.0f));
    spout = scale(spout, vec3(0.05f, 0.22f, 0.05f));
    Primitives::drawCylinder(shader, spout, kettleCol);

    // 7. Stack of glass chai tumblers & ceramic mugs
    for (int g = 0; g < 4; g++) {
        mat4 glass = model;
        glass = translate(glass, vec3(0.15f + (float)g * 0.12f, 0.80f, 0.55f));
        glass = scale(glass, vec3(0.07f, 0.12f, 0.07f));
        Primitives::drawCylinder(shader, glass, glassCol);
    }

    // 8. Rear shelf holding glass jars and biscuit tins
    mat4 shelf = model;
    shelf = translate(shelf, vec3(0.0f, 1.25f, -0.85f));
    shelf = scale(shelf, vec3(2.8f, 0.04f, 0.35f));
    Primitives::drawCube(shader, shelf, woodPlank);

    // Glass jars on rear shelf
    for (int j = 0; j < 3; j++) {
        mat4 jar = model;
        jar = translate(jar, vec3(-0.70f + (float)j * 0.70f, 1.40f, -0.85f));
        jar = scale(jar, vec3(0.18f, 0.26f, 0.18f));
        Primitives::drawCylinder(shader, jar, glassCol * 0.85f);
    }
}

// ═════════════════════════════════════════════════════════════════════
// 2. BANYAN TREE GATHERING SPOT (বটতলা / Bot-tola — Authentic Bot Gach)
// ═════════════════════════════════════════════════════════════════════
// Faithfully modeled directly after the user's reference photograph of an authentic
// traditional Bangladeshi Banyan Tree (বটগাছ / Ficus benghalensis):
// 1. Organic fluted multi-stem central trunk formed by intertwined merged columns
// 2. Splayed basal buttress roots crawling radially along the soil
// 3. Spreading heavy arching boughs supporting a massive majestic umbrella canopy
// 4. Dense curtain of iconic hanging aerial prop roots (বটের ঝুড়ি / Jhuri) falling from boughs to ground
// 5. Rich, multi-tiered botanical umbrella foliage with natural leafy contour and tonal depth
void drawBanyanTreeSpot(Shader& shader, const mat4& model)
{
    shader.setInt("uUseTexture", 0);

    // ── Optimized Static Color Palette ────────────────────────────
    static const vec3 barkBase     (0.36f, 0.26f, 0.17f); // weathered woody grey-brown bark
    static const vec3 barkFlute    (0.30f, 0.21f, 0.13f); // fluted intertwined trunk columns
    static const vec3 barkBough    (0.34f, 0.24f, 0.15f); // heavy spreading horizontal limbs
    static const vec3 jhuriPillar  (0.35f, 0.25f, 0.16f); // mature grounded aerial root pillars
    static const vec3 jhuriHanging (0.42f, 0.31f, 0.19f); // hanging aerial root tendrils (বটের ঝুড়ি)
    static const vec3 jhuriTip     (0.48f, 0.37f, 0.23f); // tender descending root tips
    static const vec3 rootCrawl    (0.28f, 0.19f, 0.11f); // surface roots crawling on earth

    // Multi-tiered foliage palette matching the reference photograph
    static const vec3 leafSunlit   (0.24f, 0.52f, 0.17f); // sunlit golden-emerald apex & outer crown
    static const vec3 leafLush     (0.17f, 0.43f, 0.13f); // rich mature banyan foliage
    static const vec3 leafMid      (0.13f, 0.37f, 0.11f); // mid-canopy body
    static const vec3 leafDeep     (0.09f, 0.29f, 0.08f); // shaded understory green
    static const vec3 leafDark     (0.06f, 0.22f, 0.06f); // deep interior foliage shadow

    // ── 1. Fluted Multi-Stem Central Banyan Trunk (গুঁড়ি ও মূল স্তম্ভ) ──────
    // A) Central core structural trunk
    mat4 coreTrunk = translate(model, vec3(0.0f, 1.70f, 0.0f));
    coreTrunk = scale(coreTrunk, vec3(0.92f, 3.40f, 0.92f));
    Primitives::drawCylinder(shader, coreTrunk, barkBase);

    // B) Intertwined fluted vertical bark columns wrapping around core (Merged roots of Ficus)
    struct TrunkFlute { float x, z; float r; float h; float yawDeg; };
    static const TrunkFlute flutes[10] = {
        {  0.58f,  0.22f, 0.26f, 3.45f,  15.0f },
        {  0.42f,  0.52f, 0.22f, 3.35f,  45.0f },
        { -0.05f,  0.64f, 0.25f, 3.50f,  85.0f },
        { -0.48f,  0.44f, 0.24f, 3.40f, 130.0f },
        { -0.65f,  0.08f, 0.26f, 3.55f, 175.0f },
        { -0.52f, -0.42f, 0.23f, 3.40f, 215.0f },
        { -0.12f, -0.62f, 0.25f, 3.50f, 260.0f },
        {  0.38f, -0.54f, 0.24f, 3.35f, 300.0f },
        {  0.64f, -0.18f, 0.25f, 3.45f, 340.0f },
        {  0.15f,  0.25f, 0.22f, 3.60f,  20.0f }
    };
    for (const auto& fl : flutes) {
        mat4 fM = model;
        fM = translate(fM, vec3(fl.x, fl.h * 0.5f, fl.z));
        fM = rotate(fM, radians(fl.yawDeg), vec3(0.0f, 1.0f, 0.0f));
        fM = scale(fM, vec3(fl.r, fl.h, fl.r));
        Primitives::drawCylinder(shader, fM, barkFlute);
    }

    // C) Sprawling Basal Surface Buttress Roots (বটের মাটি আঁকড়ে থাকা মূল)
    struct ButtressRoot { float angDeg; float len; float wid; float dist; };
    static const ButtressRoot buttresses[8] = {
        {  15.0f, 1.85f, 0.26f, 0.75f },
        {  62.0f, 1.65f, 0.24f, 0.70f },
        { 105.0f, 2.05f, 0.28f, 0.80f },
        { 150.0f, 1.70f, 0.24f, 0.72f },
        { 195.0f, 1.95f, 0.27f, 0.78f },
        { 245.0f, 1.60f, 0.22f, 0.68f },
        { 285.0f, 2.10f, 0.28f, 0.82f },
        { 330.0f, 1.75f, 0.25f, 0.74f }
    };
    for (const auto& br : buttresses) {
        mat4 rM = model;
        rM = rotate(rM, radians(br.angDeg), vec3(0.0f, 1.0f, 0.0f));
        rM = translate(rM, vec3(0.0f, 0.12f, br.dist));
        rM = rotate(rM, radians(18.0f), vec3(1.0f, 0.0f, 0.0f)); // splaying downward into soil
        rM = scale(rM, vec3(br.wid, 0.22f, br.len));
        Primitives::drawCube(shader, rM, rootCrawl);
    }

    // ── 2. Spreading Heavy Arching Boughs (সুবিশাল শাখা-প্রশাখা) ─────────
    // 8 Primary radial boughs branching outward horizontally and arching upward
    struct BanyanBough { float yawDeg; float pitchDeg; float len; float thick; float yStart; };
    static const BanyanBough boughs[8] = {
        {   0.0f, 32.0f, 3.2f, 0.24f, 3.10f },
        {  45.0f, 28.0f, 3.5f, 0.25f, 3.25f },
        {  90.0f, 34.0f, 3.1f, 0.23f, 3.15f },
        { 135.0f, 30.0f, 3.6f, 0.26f, 3.30f },
        { 180.0f, 33.0f, 3.3f, 0.24f, 3.15f },
        { 225.0f, 29.0f, 3.5f, 0.25f, 3.25f },
        { 270.0f, 35.0f, 3.0f, 0.23f, 3.10f },
        { 315.0f, 31.0f, 3.4f, 0.25f, 3.20f }
    };
    for (const auto& bg : boughs) {
        // Main limb
        mat4 bM = model;
        bM = translate(bM, vec3(0.0f, bg.yStart, 0.0f));
        bM = rotate(bM, radians(bg.yawDeg), vec3(0.0f, 1.0f, 0.0f));
        bM = rotate(bM, radians(bg.pitchDeg), vec3(1.0f, 0.0f, 0.0f));
        bM = translate(bM, vec3(0.0f, bg.len * 0.5f, 0.0f));
        mat4 bCyl = scale(bM, vec3(bg.thick, bg.len, bg.thick));
        Primitives::drawCylinder(shader, bCyl, barkBough);

        // Secondary spreading horizontal fork extending outward into umbrella canopy
        mat4 subM = bM;
        subM = translate(subM, vec3(0.0f, bg.len * 0.40f, 0.0f));
        subM = rotate(subM, radians(32.0f), vec3(0.0f, 0.0f, 1.0f));
        subM = translate(subM, vec3(0.0f, 0.85f, 0.0f));
        mat4 subCyl = scale(subM, vec3(bg.thick * 0.65f, 1.70f, bg.thick * 0.65f));
        Primitives::drawCylinder(shader, subCyl, barkBough);

        // Secondary ascending fork into crown
        mat4 upM = bM;
        upM = translate(upM, vec3(0.0f, bg.len * 0.48f, 0.0f));
        upM = rotate(upM, radians(-26.0f), vec3(0.0f, 0.0f, 1.0f));
        upM = translate(upM, vec3(0.0f, 0.75f, 0.0f));
        mat4 upCyl = scale(upM, vec3(bg.thick * 0.55f, 1.50f, bg.thick * 0.55f));
        Primitives::drawCylinder(shader, upCyl, barkBough);
    }

    // ── 3. Grounded Pillar Aerial Roots (মাটিতে পৌঁছানো মোটা স্তম্ভমূল) ────
    // Substantial vertical aerial roots that have reached the ground, forming wooden pillars
    struct GroundedPillar { float x, z; float topY; float r; };
    static const GroundedPillar pillars[10] = {
        {  1.85f,  0.60f, 4.40f, 0.080f },
        {  1.30f,  1.75f, 4.55f, 0.085f },
        { -0.30f,  2.15f, 4.50f, 0.078f },
        { -1.75f,  1.45f, 4.45f, 0.082f },
        { -2.10f, -0.20f, 4.40f, 0.085f },
        { -1.55f, -1.65f, 4.55f, 0.080f },
        {  0.10f, -2.10f, 4.50f, 0.082f },
        {  1.60f, -1.50f, 4.45f, 0.085f },
        {  2.45f, -0.45f, 4.25f, 0.075f },
        { -0.85f,  2.55f, 4.30f, 0.075f }
    };
    for (const auto& gp : pillars) {
        mat4 pM = model;
        pM = translate(pM, vec3(gp.x, gp.topY * 0.5f, gp.z));
        pM = scale(pM, vec3(gp.r, gp.topY, gp.r));
        Primitives::drawCylinder(shader, pM, jhuriPillar);

        // Ground anchor flare where aerial pillar touches soil
        mat4 flareM = model;
        flareM = translate(flareM, vec3(gp.x, 0.08f, gp.z));
        flareM = scale(flareM, vec3(gp.r * 1.8f, 0.16f, gp.r * 1.8f));
        Primitives::drawCylinder(shader, flareM, rootCrawl);
    }

    // ── 4. Iconic Hanging Aerial Prop Roots (বটের ঝুড়ি / ঝুলন্ত শিকড়ের পর্দা) ─
    // Slender, curtain-like aerial roots hanging down from horizontal boughs at various heights
    struct HangingJhuri { float x, z; float topY; float len; float r; };
    static const HangingJhuri jhuriTendrils[34] = {
        // Inner ring tendrils
        {  0.95f,  0.40f, 4.10f, 3.80f, 0.035f },
        {  0.70f,  0.85f, 4.20f, 3.90f, 0.032f },
        { -0.20f,  1.10f, 4.15f, 3.85f, 0.034f },
        { -0.85f,  0.75f, 4.25f, 3.95f, 0.032f },
        { -1.15f,  0.15f, 4.10f, 3.80f, 0.035f },
        { -0.80f, -0.80f, 4.20f, 3.90f, 0.033f },
        { -0.15f, -1.15f, 4.15f, 3.85f, 0.034f },
        {  0.80f, -0.85f, 4.25f, 3.95f, 0.032f },
        // Mid-span hanging root curtain (varying descending lengths)
        {  1.55f,  1.15f, 4.35f, 3.60f, 0.028f },
        {  1.15f,  1.85f, 4.40f, 3.80f, 0.026f },
        {  0.45f,  2.25f, 4.30f, 3.40f, 0.025f },
        { -0.75f,  2.05f, 4.45f, 3.75f, 0.027f },
        { -1.45f,  1.80f, 4.35f, 3.50f, 0.026f },
        { -2.05f,  1.05f, 4.40f, 3.85f, 0.028f },
        { -2.35f,  0.35f, 4.30f, 3.30f, 0.024f },
        { -2.15f, -0.75f, 4.45f, 3.70f, 0.027f },
        { -1.65f, -1.45f, 4.35f, 3.55f, 0.026f },
        { -1.05f, -2.05f, 4.40f, 3.80f, 0.028f },
        { -0.35f, -2.35f, 4.30f, 3.40f, 0.025f },
        {  0.55f, -2.15f, 4.45f, 3.75f, 0.027f },
        {  1.35f, -1.75f, 4.35f, 3.60f, 0.026f },
        {  2.05f, -1.05f, 4.40f, 3.85f, 0.028f },
        {  2.25f, -0.25f, 4.30f, 3.45f, 0.025f },
        {  2.15f,  0.55f, 4.45f, 3.70f, 0.027f },
        // Outer peripheral dangling tendrils (gracefully floating in air above ground)
        {  2.75f,  0.85f, 4.50f, 2.65f, 0.022f },
        {  1.95f,  2.35f, 4.60f, 2.80f, 0.022f },
        {  0.85f,  2.85f, 4.55f, 2.50f, 0.020f },
        { -0.65f,  2.90f, 4.60f, 2.75f, 0.022f },
        { -2.15f,  2.25f, 4.55f, 2.60f, 0.021f },
        { -2.85f,  0.85f, 4.50f, 2.80f, 0.022f },
        { -2.80f, -0.95f, 4.60f, 2.70f, 0.021f },
        { -1.95f, -2.40f, 4.55f, 2.85f, 0.022f },
        {  0.75f, -2.85f, 4.50f, 2.60f, 0.020f },
        {  2.45f, -1.95f, 4.60f, 2.75f, 0.022f }
    };
    for (const auto& jh : jhuriTendrils) {
        float yCenter = jh.topY - jh.len * 0.5f;
        mat4 jM = model;
        jM = translate(jM, vec3(jh.x, yCenter, jh.z));
        jM = scale(jM, vec3(jh.r, jh.len, jh.r));
        Primitives::drawCylinder(shader, jM, jhuriHanging);

        // Tender dangling aerial root tip
        mat4 tipM = model;
        tipM = translate(tipM, vec3(jh.x, jh.topY - jh.len, jh.z));
        tipM = scale(tipM, vec3(jh.r * 1.15f, 0.12f, jh.r * 1.15f));
        Primitives::drawCone(shader, tipM, jhuriTip);
    }

    // ── 5. Majestic Broad Umbrella Foliage Canopy (বটের সুবিশাল চন্দ্রাতপ) ─
    // Broad, flat-domed umbrella silhouette matching the reference photo
    struct FoliageDomeLobe { float x, y, z; float rx, ry, rz; vec3 col; };
    static const FoliageDomeLobe banyanCanopy[32] = {
        // Apex crown & upper dome (Central sunlit peak)
        {  0.00f, 7.15f,  0.00f, 2.10f, 0.95f, 2.10f, leafSunlit },
        {  0.85f, 6.85f,  0.65f, 1.85f, 0.90f, 1.85f, leafSunlit },
        { -0.80f, 6.80f, -0.65f, 1.85f, 0.90f, 1.85f, leafSunlit },
        { -0.65f, 6.90f,  0.75f, 1.80f, 0.88f, 1.80f, leafSunlit },
        {  0.75f, 6.75f, -0.75f, 1.80f, 0.88f, 1.80f, leafSunlit },

        // High spreading canopy tier (R ≈ 1.8m - 2.8m)
        {  1.90f, 6.30f,  0.45f, 1.95f, 0.92f, 1.85f, leafLush },
        {  1.45f, 6.20f,  1.55f, 1.85f, 0.88f, 1.80f, leafLush },
        {  0.25f, 6.35f,  2.05f, 1.90f, 0.90f, 1.90f, leafSunlit },
        { -1.45f, 6.25f,  1.60f, 1.85f, 0.88f, 1.80f, leafLush },
        { -2.05f, 6.20f,  0.35f, 1.95f, 0.92f, 1.85f, leafLush },
        { -1.55f, 6.15f, -1.45f, 1.85f, 0.88f, 1.80f, leafMid },
        { -0.20f, 6.30f, -2.05f, 1.90f, 0.90f, 1.90f, leafLush },
        {  1.40f, 6.25f, -1.55f, 1.85f, 0.88f, 1.80f, leafLush },

        // Mid-canopy broad umbrella terrace (R ≈ 2.8m - 4.2m)
        {  2.95f, 5.55f,  0.40f, 2.05f, 0.85f, 1.95f, leafLush },
        {  2.35f, 5.45f,  2.15f, 1.95f, 0.82f, 1.90f, leafMid },
        {  0.40f, 5.60f,  3.05f, 2.00f, 0.85f, 2.00f, leafLush },
        { -2.25f, 5.40f,  2.25f, 1.95f, 0.82f, 1.90f, leafMid },
        { -3.05f, 5.50f,  0.30f, 2.05f, 0.85f, 1.95f, leafLush },
        { -2.35f, 5.35f, -2.15f, 1.95f, 0.82f, 1.90f, leafMid },
        { -0.30f, 5.55f, -3.00f, 2.00f, 0.85f, 2.00f, leafMid },
        {  2.20f, 5.45f, -2.25f, 1.95f, 0.82f, 1.90f, leafLush },

        // Outer perimeter drooping umbrella skirt (R ≈ 3.8m - 5.1m, breaking up silhouette)
        {  3.75f, 4.75f,  0.60f, 1.65f, 0.72f, 1.60f, leafDeep },
        {  3.10f, 4.65f,  2.45f, 1.55f, 0.68f, 1.55f, leafDeep },
        {  1.85f, 4.70f,  3.55f, 1.60f, 0.70f, 1.60f, leafDeep },
        { -0.20f, 4.80f,  3.90f, 1.65f, 0.72f, 1.65f, leafDeep },
        { -2.15f, 4.65f,  3.40f, 1.55f, 0.68f, 1.55f, leafDeep },
        { -3.70f, 4.70f,  0.50f, 1.65f, 0.72f, 1.60f, leafDeep },
        { -3.35f, 4.60f, -2.05f, 1.55f, 0.68f, 1.55f, leafDeep },
        { -1.80f, 4.65f, -3.50f, 1.60f, 0.70f, 1.60f, leafDeep },
        {  0.30f, 4.75f, -3.85f, 1.65f, 0.72f, 1.65f, leafDeep },
        {  2.55f, 4.60f, -3.10f, 1.55f, 0.68f, 1.55f, leafDeep },

        // Interior understory shadow density (nestled between heavy boughs above aerial roots)
        {  0.00f, 4.45f,  0.00f, 1.90f, 0.65f, 1.90f, leafDark }
    };
    for (const auto& cl : banyanCanopy) {
        mat4 cM = model;
        cM = translate(cM, vec3(cl.x, cl.y, cl.z));
        cM = scale(cM, vec3(cl.rx, cl.ry, cl.rz));
        Primitives::drawSphere(shader, cM, cl.col);
    }
}

// ═════════════════════════════════════════════════════════════════════
// 3. VILLAGE FRESHWATER POND (পুকুর / Pukur with Ripples & Lilies)
// ═════════════════════════════════════════════════════════════════════
void drawPond(Shader& shader, const mat4& model, float animTime)
{
    shader.setInt("uUseTexture", 0);

    vec3 bankMud   (0.38f, 0.32f, 0.20f); // damp earthen pond embankment
    vec3 waterDeep (0.12f, 0.28f, 0.32f); // tranquil freshwater depth
    vec3 lilyLeaf  (0.24f, 0.52f, 0.18f); // floating circular water lily pad
    vec3 lilyFlower(0.95f, 0.88f, 0.92f); // delicate pink/white lotus blossom

    // 1. Sloping earthen pond rim / embankment
    mat4 embankment = model;
    embankment = translate(embankment, vec3(0.0f, 0.04f, 0.0f));
    embankment = scale(embankment, vec3(12.5f, 0.12f, 9.5f));
    Primitives::drawCube(shader, embankment, bankMud);

    // 2. Reflective, rippling water surface
    float waterRipple = sinf(animTime * 1.6f) * 0.006f;
    mat4 water = model;
    water = translate(water, vec3(0.0f, 0.055f + waterRipple, 0.0f));
    water = scale(water, vec3(11.2f, 0.002f, 8.2f));
    Primitives::drawPlane(shader, water, waterDeep);

    // 3. Water Lily (Shapla / পদ্ম) clusters floating on pond
    float lilyPos[4][2] = {
        { -3.2f,  1.8f },
        {  2.8f, -1.6f },
        { -1.4f, -2.2f },
        {  3.4f,  1.6f }
    };
    for (int l = 0; l < 4; l++) {
        mat4 pad = model;
        pad = translate(pad, vec3(lilyPos[l][0], 0.062f + waterRipple, lilyPos[l][1]));
        pad = scale(pad, vec3(0.65f, 0.005f, 0.65f));
        Primitives::drawSphere(shader, pad, lilyLeaf);

        // Lotus flower blossom
        mat4 flower = model;
        flower = translate(flower, vec3(lilyPos[l][0], 0.085f + waterRipple, lilyPos[l][1]));
        flower = scale(flower, vec3(0.12f, 0.08f, 0.12f));
        Primitives::drawSphere(shader, flower, lilyFlower);
    }
}

// ═════════════════════════════════════════════════════════════════════
// 4. COURTYARD BAMBOO CLOTHESLINE (কাপড় শুকানোর দড়ি / Clothesline)
// ═════════════════════════════════════════════════════════════════════
void drawClothesline(Shader& shader, const mat4& model, float animTime)
{
    shader.setInt("uUseTexture", 0);

    vec3 bambooPost(0.48f, 0.38f, 0.20f); // weathered bamboo upright poles
    vec3 coirRope  (0.36f, 0.26f, 0.14f); // jute coir hanging line
    vec3 gamchaRed (0.84f, 0.22f, 0.14f); // traditional red Bengali Gamcha
    vec3 lungiPlaid(0.18f, 0.38f, 0.52f); // checkered Bengali cotton Lungi

    float lineLength = 3.6f;
    float postH      = 1.75f;

    // 1. Two bamboo poles driven into courtyard ground
    for (float px : { -lineLength * 0.5f, lineLength * 0.5f }) {
        mat4 post = model;
        post = translate(post, vec3(px, postH * 0.5f, 0.0f));
        post = scale(post, vec3(0.045f, postH, 0.045f));
        Primitives::drawCylinder(shader, post, bambooPost);
    }

    // 2. Horizontal coir rope line connecting the posts
    mat4 rope = model;
    rope = translate(rope, vec3(0.0f, postH - 0.10f, 0.0f));
    rope = scale(rope, vec3(lineLength, 0.015f, 0.015f));
    Primitives::drawCube(shader, rope, coirRope);

    // 3. Fluttering Red Gamcha hanging on line
    float windFlutter1 = (animTime > 0.0f) ? (sinf(animTime * 3.4f) * radians(9.0f)) : 0.0f;
    mat4 gamcha = model;
    gamcha = translate(gamcha, vec3(-0.65f, postH - 0.48f, 0.0f));
    gamcha = rotate(gamcha, windFlutter1, vec3(1.0f, 0.0f, 0.0f));
    gamcha = scale(gamcha, vec3(0.75f, 0.72f, 0.014f));
    Primitives::drawCube(shader, gamcha, gamchaRed);

    // 4. Fluttering Blue/Green Checked Lungi drying on line
    float windFlutter2 = (animTime > 0.0f) ? (sinf(animTime * 2.8f + 1.2f) * radians(8.0f)) : 0.0f;
    mat4 lungi = model;
    lungi = translate(lungi, vec3(0.70f, postH - 0.60f, 0.0f));
    lungi = rotate(lungi, windFlutter2, vec3(1.0f, 0.0f, 0.0f));
    lungi = scale(lungi, vec3(1.05f, 0.95f, 0.014f));
    Primitives::drawCube(shader, lungi, lungiPlaid);
}

// ═════════════════════════════════════════════════════════════════════
// 5. BAMBOO MOORING JETTY AT THE GHAT (বাঁশের তৈরি ঘাট / Bamboo Jetty)
// ═════════════════════════════════════════════════════════════════════
void drawBambooJetty(Shader& shader, const mat4& model)
{
    shader.setInt("uUseTexture", 0);

    vec3 bambooPiling(0.42f, 0.32f, 0.18f); // water-soaked bamboo pilings
    vec3 woodPlank   (0.50f, 0.38f, 0.22f); // river jetty timber deck
    vec3 coirLash    (0.24f, 0.18f, 0.10f); // jute coir rope ties

    float jettyL = 3.6f; // extends out into river water
    float jettyW = 1.15f;
    float deckY  = 0.14f;

    // 1. Pairs of bamboo pilings driven into riverbed
    for (float z = 0.4f; z <= jettyL; z += 1.0f) {
        for (float x : { -jettyW * 0.42f, jettyW * 0.42f }) {
            mat4 piling = model;
            piling = translate(piling, vec3(x, 0.35f, z));
            piling = scale(piling, vec3(0.055f, 1.2f, 0.055f));
            Primitives::drawCylinder(shader, piling, bambooPiling);
        }
    }

    // 2. Horizontal tie-bearers
    for (float x : { -jettyW * 0.42f, jettyW * 0.42f }) {
        mat4 bearer = model;
        bearer = translate(bearer, vec3(x, deckY - 0.035f, jettyL * 0.5f));
        bearer = scale(bearer, vec3(0.045f, 0.045f, jettyL));
        Primitives::drawCube(shader, bearer, bambooPiling);
    }

    // 3. Wooden cross-deck planks
    int numPlanks = 16;
    for (int p = 0; p < numPlanks; p++) {
        float pz = 0.15f + (float)p * (jettyL / (float)numPlanks);
        mat4 plank = model;
        plank = translate(plank, vec3(0.0f, deckY, pz));
        plank = scale(plank, vec3(jettyW, 0.035f, 0.18f));
        Primitives::drawCube(shader, plank, (p % 2 == 0) ? woodPlank : (woodPlank * 0.93f));
    }

    // 4. Mooring post at jetty tip for tying boats
    mat4 moorPost = model;
    moorPost = translate(moorPost, vec3(jettyW * 0.35f, 0.42f, jettyL - 0.15f));
    moorPost = scale(moorPost, vec3(0.075f, 0.65f, 0.075f));
    Primitives::drawCylinder(shader, moorPost, bambooPiling * 0.85f);
}

// ═════════════════════════════════════════════════════════════════════
// 6. TRADITIONAL RURAL CORRUGATED TIN WASHROOM (টিনের পায়খানা / বাথরুম)
// ═════════════════════════════════════════════════════════════════════
void drawWashroom(Shader& shader, const mat4& model)
{
    shader.setInt("uUseTexture", 0);

    // ── Authentic Material Palette (Matching Reference CI Sheet Latrine) ──
    const vec3 concreteBase (0.50f, 0.52f, 0.53f); // raised poured concrete plinth
    const vec3 concreteDark (0.38f, 0.40f, 0.41f); // damp concrete skirting & step
    const vec3 timberPost   (0.52f, 0.35f, 0.18f); // seasoned wooden corner framing & purlins
    const vec3 timberTrim   (0.68f, 0.44f, 0.18f); // bright wooden door batten & jambs
    const vec3 tinTealBase  (0.06f, 0.50f, 0.42f); // corrugated CI sheet base (Teal / Sea-Green)
    const vec3 tinTealRib   (0.09f, 0.57f, 0.48f); // 3D corrugated vertical wave ridge highlight
    const vec3 roofTinBase  (0.32f, 0.46f, 0.35f); // weathered olive-green corrugated tin roof
    const vec3 roofTinRib   (0.38f, 0.52f, 0.40f); // corrugated roof wave ridges
    const vec3 metalLatch   (0.22f, 0.20f, 0.18f); // forged iron sliding door bolt (Chhitkini)
    const vec3 pvcVentPipe  (0.24f, 0.25f, 0.26f); // dark charcoal sanitary vent pipe

    // Overall structure dimensions (compact rural footprint)
    const float plinthW = 1.08f;
    const float plinthD = 1.18f;
    const float plinthH = 0.18f;

    const float cabinW  = 0.94f;
    const float cabinD  = 1.02f;
    const float cabinH  = 1.84f; // wall height

    const float halfW   = cabinW * 0.5f; // 0.47f
    const float halfD   = cabinD * 0.5f; // 0.51f

    // ── 1. Raised Concrete Plinth Foundation & Front Step ───────────
    // Main foundation slab
    mat4 plinth = model;
    plinth = translate(plinth, vec3(0.0f, plinthH * 0.5f, 0.0f));
    plinth = scale(plinth, vec3(plinthW, plinthH, plinthD));
    Primitives::drawCube(shader, plinth, concreteBase);

    // Darker perimeter footing band
    mat4 footing = model;
    footing = translate(footing, vec3(0.0f, 0.035f, 0.0f));
    footing = scale(footing, vec3(plinthW + 0.05f, 0.07f, plinthD + 0.05f));
    Primitives::drawCube(shader, footing, concreteDark);

    // Front entrance step centered before the door (+Z)
    mat4 step = model;
    step = translate(step, vec3(0.0f, 0.045f, plinthD * 0.5f + 0.13f));
    step = scale(step, vec3(0.66f, 0.09f, 0.26f));
    Primitives::drawCube(shader, step, concreteDark);

    // ── 2. Timber Structural Frame (4 Corner Posts & Roof Rafters) ──
    const float postSize = 0.055f;
    const float postH    = cabinH + 0.08f;
    const float postY    = plinthH + postH * 0.5f;

    // 4 Corner Wooden Posts
    for (float px : { -halfW + postSize * 0.5f, halfW - postSize * 0.5f }) {
        for (float pz : { -halfD + postSize * 0.5f, halfD - postSize * 0.5f }) {
            mat4 post = model;
            post = translate(post, vec3(px, postY, pz));
            post = scale(post, vec3(postSize, postH, postSize));
            Primitives::drawCube(shader, post, timberPost);
        }
    }

    // Top horizontal wooden plates
    mat4 topPlateL = model;
    topPlateL = translate(topPlateL, vec3(-halfW + postSize * 0.5f, plinthH + cabinH - 0.02f, 0.0f));
    topPlateL = scale(topPlateL, vec3(postSize, 0.05f, cabinD));
    Primitives::drawCube(shader, topPlateL, timberPost);

    mat4 topPlateR = model;
    topPlateR = translate(topPlateR, vec3(halfW - postSize * 0.5f, plinthH + cabinH - 0.02f, 0.0f));
    topPlateR = scale(topPlateR, vec3(postSize, 0.05f, cabinD));
    Primitives::drawCube(shader, topPlateR, timberPost);

    // ── 3. Corrugated Tin Walls (Left, Right, Rear) ───────────────────
    const float wallCenterY = plinthH + cabinH * 0.5f;

    // A. Left Wall (-X)
    mat4 leftWall = model;
    leftWall = translate(leftWall, vec3(-halfW + 0.01f, wallCenterY, 0.0f));
    leftWall = scale(leftWall, vec3(0.018f, cabinH, cabinD - postSize * 2.0f));
    Primitives::drawCube(shader, leftWall, tinTealBase);

    // Left wall vertical corrugation wave ribs (8 flutes)
    for (int r = 0; r < 8; ++r) {
        float rz = (-halfD + postSize + 0.06f) + (float)r * ((cabinD - postSize * 2.0f - 0.12f) / 7.0f);
        mat4 rib = model;
        rib = translate(rib, vec3(-halfW - 0.004f, wallCenterY, rz));
        rib = scale(rib, vec3(0.016f, cabinH, 0.022f));
        Primitives::drawCylinder(shader, rib, tinTealRib);
    }

    // B. Right Wall (+X)
    mat4 rightWall = model;
    rightWall = translate(rightWall, vec3(halfW - 0.01f, wallCenterY, 0.0f));
    rightWall = scale(rightWall, vec3(0.018f, cabinH, cabinD - postSize * 2.0f));
    Primitives::drawCube(shader, rightWall, tinTealBase);

    // Right wall vertical corrugation wave ribs (8 flutes)
    for (int r = 0; r < 8; ++r) {
        float rz = (-halfD + postSize + 0.06f) + (float)r * ((cabinD - postSize * 2.0f - 0.12f) / 7.0f);
        mat4 rib = model;
        rib = translate(rib, vec3(halfW + 0.004f, wallCenterY, rz));
        rib = scale(rib, vec3(0.016f, cabinH, 0.022f));
        Primitives::drawCylinder(shader, rib, tinTealRib);
    }

    // C. Rear Wall (-Z)
    mat4 rearWall = model;
    rearWall = translate(rearWall, vec3(0.0f, wallCenterY, -halfD + 0.01f));
    rearWall = scale(rearWall, vec3(cabinW - postSize * 2.0f, cabinH, 0.018f));
    Primitives::drawCube(shader, rearWall, tinTealBase);

    // Rear wall vertical corrugation wave ribs (7 flutes)
    for (int r = 0; r < 7; ++r) {
        float rx = (-halfW + postSize + 0.06f) + (float)r * ((cabinW - postSize * 2.0f - 0.12f) / 6.0f);
        mat4 rib = model;
        rib = translate(rib, vec3(rx, wallCenterY, -halfD - 0.004f));
        rib = scale(rib, vec3(0.022f, cabinH, 0.016f));
        Primitives::drawCylinder(shader, rib, tinTealRib);
    }

    // ── 4. Front Wall & Corrugated Tin Door (+Z) ─────────────────────
    // Wooden door jambs framing the front opening
    const float doorW = 0.64f;
    const float doorH = cabinH - 0.06f;
    const float doorX = 0.04f; // slightly biased towards center

    // Wooden door jamb (left vertical frame)
    mat4 jambL = model;
    jambL = translate(jambL, vec3(doorX - doorW * 0.5f - 0.02f, plinthH + doorH * 0.5f, halfD - 0.015f));
    jambL = scale(jambL, vec3(0.04f, doorH, 0.04f));
    Primitives::drawCube(shader, jambL, timberPost);

    // Wooden door jamb (right vertical frame)
    mat4 jambR = model;
    jambR = translate(jambR, vec3(doorX + doorW * 0.5f + 0.02f, plinthH + doorH * 0.5f, halfD - 0.015f));
    jambR = scale(jambR, vec3(0.04f, doorH, 0.04f));
    Primitives::drawCube(shader, jambR, timberPost);

    // Wooden lintel over door
    mat4 lintel = model;
    lintel = translate(lintel, vec3(doorX, plinthH + doorH + 0.025f, halfD - 0.015f));
    lintel = scale(lintel, vec3(doorW + 0.08f, 0.05f, 0.04f));
    Primitives::drawCube(shader, lintel, timberPost);

    // Small fixed corrugated tin strip on the far left of front wall
    float fixedW = (cabinW - postSize) * 0.5f - doorW * 0.5f;
    if (fixedW > 0.04f) {
        mat4 fixedTin = model;
        fixedTin = translate(fixedTin, vec3(-halfW + fixedW * 0.5f + 0.02f, wallCenterY, halfD - 0.01f));
        fixedTin = scale(fixedTin, vec3(fixedW, cabinH, 0.018f));
        Primitives::drawCube(shader, fixedTin, tinTealBase);
    }

    // Main Corrugated Tin Door Panel
    mat4 door = model;
    door = translate(door, vec3(doorX, plinthH + doorH * 0.5f, halfD + 0.005f));
    door = scale(door, vec3(doorW, doorH, 0.02f));
    Primitives::drawCube(shader, door, tinTealBase);

    // 6 Vertical corrugation flutes across the front door
    for (int r = 0; r < 6; ++r) {
        float dx = (doorX - doorW * 0.5f + 0.05f) + (float)r * ((doorW - 0.10f) / 5.0f);
        mat4 dRib = model;
        dRib = translate(dRib, vec3(dx, plinthH + doorH * 0.5f, halfD + 0.018f));
        dRib = scale(dRib, vec3(0.022f, doorH, 0.016f));
        Primitives::drawCylinder(shader, dRib, tinTealRib);
    }

    // Prominent golden timber edge batten on door (visible in reference photo!)
    mat4 batten = model;
    batten = translate(batten, vec3(doorX - doorW * 0.5f + 0.025f, plinthH + doorH * 0.5f, halfD + 0.022f));
    batten = scale(batten, vec3(0.045f, doorH, 0.025f));
    Primitives::drawCube(shader, batten, timberTrim);

    // Horizontal metal sliding door latch (ছিটকিনি / Chhitkini)
    float latchY = plinthH + doorH * 0.52f;
    mat4 latchBase = model;
    latchBase = translate(latchBase, vec3(doorX - doorW * 0.5f + 0.055f, latchY, halfD + 0.032f));
    latchBase = scale(latchBase, vec3(0.12f, 0.035f, 0.015f));
    Primitives::drawCube(shader, latchBase, metalLatch);

    mat4 latchBolt = model;
    latchBolt = translate(latchBolt, vec3(doorX - doorW * 0.5f + 0.055f, latchY, halfD + 0.040f));
    latchBolt = scale(latchBolt, vec3(0.14f, 0.014f, 0.014f));
    Primitives::drawCube(shader, latchBolt, metalLatch * 0.8f);

    // ── 5. Slanted Single-Pitch Corrugated Tin Roof (একচালা ঢেউটিনের চাল) ──
    // In reference photo: single pitch sloping gently backwards (~11 degrees)
    const float roofSlopeDeg = -10.5f; // slope downwards towards rear (-Z)
    const float roofW        = cabinW + 0.28f; // overhanging eaves on sides
    const float roofD        = cabinD + 0.36f; // generous overhanging eaves front & back
    const float roofThick    = 0.024f;
    const float roofPivotY   = plinthH + cabinH + 0.09f;

    // Wooden purlin rafters supporting the corrugated roof
    for (float rx : { -roofW * 0.32f, 0.0f, roofW * 0.32f }) {
        mat4 rafter = model;
        rafter = translate(rafter, vec3(rx, roofPivotY - 0.035f, 0.0f));
        rafter = rotate(rafter, radians(roofSlopeDeg), vec3(1.0f, 0.0f, 0.0f));
        rafter = scale(rafter, vec3(0.045f, 0.045f, roofD * 0.94f));
        Primitives::drawCube(shader, rafter, timberPost);
    }

    // Main Corrugated Tin Roof Sheet
    mat4 roofSheet = model;
    roofSheet = translate(roofSheet, vec3(0.0f, roofPivotY, 0.0f));
    roofSheet = rotate(roofSheet, radians(roofSlopeDeg), vec3(1.0f, 0.0f, 0.0f));
    roofSheet = scale(roofSheet, vec3(roofW, roofThick, roofD));
    Primitives::drawCube(shader, roofSheet, roofTinBase);

    // 9 Corrugated roof ribs along the slope
    for (int r = 0; r < 9; ++r) {
        float rx = (-roofW * 0.5f + 0.06f) + (float)r * ((roofW - 0.12f) / 8.0f);
        mat4 rRib = model;
        rRib = translate(rRib, vec3(rx, roofPivotY + 0.014f, 0.0f));
        rRib = rotate(rRib, radians(roofSlopeDeg), vec3(1.0f, 0.0f, 0.0f));
        rRib = scale(rRib, vec3(0.024f, 0.016f, roofD));
        Primitives::drawCylinder(shader, rRib, roofTinRib);
    }

    // ── 6. Sanitary PVC Ventilation Pipe (ভেন্ট পাইপ at rear corner) ──
    const float pipeH = cabinH + 0.42f; // extends well above roof
    mat4 vent = model;
    vent = translate(vent, vec3(-halfW + 0.09f, plinthH + pipeH * 0.5f, -halfD - 0.03f));
    vent = scale(vent, vec3(0.065f, pipeH, 0.065f));
    Primitives::drawCylinder(shader, vent, pvcVentPipe);

    // Vent pipe T-cap / cowl on top
    mat4 cowl = model;
    cowl = translate(cowl, vec3(-halfW + 0.09f, plinthH + pipeH + 0.02f, -halfD - 0.03f));
    cowl = scale(cowl, vec3(0.12f, 0.05f, 0.07f));
    Primitives::drawCube(shader, cowl, pvcVentPipe * 0.85f);
}

// ═════════════════════════════════════════════════════════════════════
// 15. TRADITIONAL RURAL AGRICULTURAL FIELD PREPARATION & OX PLOWING (হালচাষ ও জমি তৈরি)
// ═════════════════════════════════════════════════════════════════════
void drawPlowingScene(Shader& shader, const mat4& model, float animTime,
                      float customX, float customZ, float customHeading, float customWalkPhase,
                      bool isNight)
{
    shader.setInt("uUseTexture", 0);

    const vec3 soilBedCol   (0.24f, 0.17f, 0.10f); // rich damp dark alluvial soil
    const vec3 furrowDark   (0.16f, 0.11f, 0.07f); // deep furrow shadows
    const vec3 furrowRidge  (0.35f, 0.25f, 0.15f); // upturned ploughed soil ridges
    const vec3 clodHighlight(0.42f, 0.31f, 0.18f); // sun-dried soil clods
    const vec3 aalBorderCol (0.48f, 0.36f, 0.22f); // raised clay boundary ridge

    // Oxen & plow materials (matching reference photo)
    const vec3 oxTan   (0.76f, 0.60f, 0.40f);      // Left ox: warm fawn / tan coat
    const vec3 oxBrown (0.38f, 0.20f, 0.11f);      // Right ox: rich dark chestnut brown coat
    const vec3 blueHorn(0.20f, 0.55f, 0.92f);      // Painted blue horns (authentic rural custom)
    const vec3 yokeWood(0.46f, 0.32f, 0.18f);      // Heavy hardwood yoke (Joyal)
    const vec3 plowWood(0.42f, 0.28f, 0.15f);      // Seasoned jackfruit wood plow body (Langol)
    const vec3 steelFhal(0.28f, 0.28f, 0.30f);     // Dark iron plowshare point (Fhal)
    const vec3 reinRope(0.82f, 0.78f, 0.68f);      // Jute coir reins / guide ropes

    const float centerX = -24.5f;
    const float centerZ =  27.0f;
    const float fieldW  =  12.0f;
    const float fieldL  =   9.0f;

    // ── 1. The Prepared Tilled Agricultural Field Bed (চাষ দেওয়া খাস জমি) ──
    mat4 fieldBase = model;
    fieldBase = translate(fieldBase, vec3(centerX, 0.012f, centerZ));
    fieldBase = scale(fieldBase, vec3(fieldW, 1.0f, fieldL));
    Primitives::drawPlane(shader, fieldBase, soilBedCol);

    // Parallel ploughed soil furrows across the field
    const int numFurrows = 18;
    const float furrowStep = (fieldW - 0.8f) / (float)(numFurrows - 1);
    for (int f = 0; f < numFurrows; ++f) {
        float fx = (centerX - fieldW * 0.5f + 0.4f) + (float)f * furrowStep;

        // Dark furrow trough
        mat4 trough = model;
        trough = translate(trough, vec3(fx, 0.015f, centerZ));
        trough = scale(trough, vec3(0.10f, 0.012f, fieldL * 0.94f));
        Primitives::drawCube(shader, trough, furrowDark);

        // Raised ploughed earth ridge
        mat4 ridge = model;
        ridge = translate(ridge, vec3(fx + furrowStep * 0.45f, 0.035f, centerZ));
        ridge = scale(ridge, vec3(furrowStep * 0.62f, 0.055f, fieldL * 0.94f));
        Primitives::drawCube(shader, ridge, furrowRidge);

        // Soil clods (মাটির ঢেলা) along the tilled ridges
        for (int c = 0; c < 6; ++c) {
            float cz = (centerZ - fieldL * 0.42f) + (float)c * (fieldL * 0.84f / 5.0f);
            float clodJitterX = (float)((f * 7 + c * 13) % 7 - 3) * 0.022f;
            float clodJitterZ = (float)((f * 11 + c * 5) % 7 - 3) * 0.032f;

            mat4 clod = model;
            clod = translate(clod, vec3(fx + furrowStep * 0.45f + clodJitterX, 0.065f, cz + clodJitterZ));
            clod = scale(clod, vec3(0.085f, 0.055f, 0.085f));
            Primitives::drawSphere(shader, clod, clodHighlight);
        }
    }

    // 4 Earthen Boundary Ridges (Aal / মাটির আইল)
    const float aalH = 0.09f, aalW = 0.32f;
    for (int side = -1; side <= 1; side += 2) {
        mat4 dikeZ = model;
        dikeZ = translate(dikeZ, vec3(centerX, aalH * 0.5f, centerZ + (float)side * (fieldL * 0.5f)));
        dikeZ = scale(dikeZ, vec3(fieldW + aalW, aalH, aalW));
        Primitives::drawCube(shader, dikeZ, aalBorderCol);

        mat4 dikeX = model;
        dikeX = translate(dikeX, vec3(centerX + (float)side * (fieldW * 0.5f), aalH * 0.5f, centerZ));
        dikeX = scale(dikeX, vec3(aalW, aalH, fieldL));
        Primitives::drawCube(shader, dikeX, aalBorderCol);
    }

    // At nighttime, the draft oxen and farmer are resting at home/cowshed (only present during daytime/inspection)
    if (isNight) {
        return;
    }

    // ── 2. Plowing Motion Trajectory (Keyboard Driven or Animated Trajectory) ──
    float teamPosX;
    float teamPosZ;
    float headingYaw;
    float walkPhase;

    if (customX > -900.0f) {
        teamPosX   = customX;
        teamPosZ   = customZ;
        headingYaw = customHeading;
        walkPhase  = customWalkPhase;
    } else {
        teamPosX = centerX;
        const float movePeriod = 16.0f; // 16s per furrow circuit
        float cycleT = fmodf(animTime, movePeriod) / movePeriod;
        if (cycleT < 0.0f) cycleT += 1.0f;

        if (cycleT < 0.45f) {
            float s = cycleT / 0.45f;
            teamPosZ = (centerZ - 2.5f) + s * 5.0f;
            headingYaw = 0.0f;
        } else if (cycleT < 0.50f) {
            float s = (cycleT - 0.45f) / 0.05f;
            teamPosZ = centerZ + 2.5f;
            headingYaw = s * 180.0f;
        } else if (cycleT < 0.95f) {
            float s = (cycleT - 0.50f) / 0.45f;
            teamPosZ = (centerZ + 2.5f) - s * 5.0f;
            headingYaw = 180.0f;
        } else {
            float s = (cycleT - 0.95f) / 0.05f;
            teamPosZ = centerZ - 2.5f;
            headingYaw = 180.0f + s * 180.0f;
        }
        walkPhase = animTime * 5.2f;
    }

    // Team root matrix (oxen lead, farmer follows behind)
    mat4 teamM = model;
    teamM = translate(teamM, vec3(teamPosX, 0.0f, teamPosZ));
    teamM = rotate(teamM, radians(headingYaw), vec3(0.0f, 1.0f, 0.0f));

    // ── 3. Pair of Draft Oxen (এক জোড়া বলদ গরু) ──
    mat4 leftOxM = teamM;
    leftOxM = translate(leftOxM, vec3(-0.65f, 0.0f, 1.35f));
    drawCow(shader, leftOxM, false, &oxTan, walkPhase, true);

    mat4 rightOxM = teamM;
    rightOxM = translate(rightOxM, vec3(0.65f, 0.0f, 1.35f));
    drawCow(shader, rightOxM, false, &oxBrown, walkPhase + 0.45f, true);

    // Blue Painted Horns (নীল রঙের শিং - matching reference image)
    for (float hornSide : { -1.0f, 1.0f }) {
        mat4 bHornL = leftOxM;
        bHornL = translate(bHornL, vec3(hornSide * 0.18f, 1.52f, 0.72f));
        bHornL = rotate(bHornL, radians(hornSide * 28.0f), vec3(0.0f, 0.0f, 1.0f));
        bHornL = rotate(bHornL, radians(22.0f), vec3(1.0f, 0.0f, 0.0f));
        bHornL = scale(bHornL, vec3(0.048f, 0.22f, 0.048f));
        Primitives::drawCone(shader, bHornL, blueHorn);

        mat4 bHornR = rightOxM;
        bHornR = translate(bHornR, vec3(hornSide * 0.18f, 1.52f, 0.72f));
        bHornR = rotate(bHornR, radians(hornSide * 28.0f), vec3(0.0f, 0.0f, 1.0f));
        bHornR = rotate(bHornR, radians(22.0f), vec3(1.0f, 0.0f, 0.0f));
        bHornR = scale(bHornR, vec3(0.048f, 0.22f, 0.048f));
        Primitives::drawCone(shader, bHornR, blueHorn);
    }

    // ── 4. Wooden Yoke & Plow Assembly (জোয়াল, ঈশ ও কাঠের লাঙ্গল) ──
    // Heavy wooden yoke beam (Joyal / জোয়াল) across both oxen necks
    mat4 yoke = teamM;
    yoke = translate(yoke, vec3(0.0f, 1.06f, 1.38f));
    yoke = rotate(yoke, radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
    yoke = scale(yoke, vec3(0.075f, 0.075f, 1.95f));
    Primitives::drawCylinder(shader, yoke, yokeWood);

    // Yoke retaining pins (Shail / শাইল) locking each ox
    for (float oxX : { -0.82f, -0.48f, 0.48f, 0.82f }) {
        mat4 pin = teamM;
        pin = translate(pin, vec3(oxX, 0.98f, 1.38f));
        pin = scale(pin, vec3(0.035f, 0.26f, 0.035f));
        Primitives::drawCylinder(shader, pin, yokeWood * 0.85f);
    }

    // Long central wooden draft beam (Eesh / ঈশ) from yoke center to plow base
    mat4 eesh = teamM;
    eesh = translate(eesh, vec3(0.0f, 0.58f, 0.35f));
    eesh = rotate(eesh, radians(22.0f), vec3(1.0f, 0.0f, 0.0f));
    eesh = scale(eesh, vec3(0.055f, 0.055f, 2.25f));
    Primitives::drawCylinder(shader, eesh, plowWood);

    // Curved wooden plow body (Langol / লাঙ্গল) at the base slicing into earth
    mat4 plowBody = teamM;
    plowBody = translate(plowBody, vec3(0.0f, 0.22f, -0.75f));
    plowBody = rotate(plowBody, radians(-32.0f), vec3(1.0f, 0.0f, 0.0f));
    plowBody = scale(plowBody, vec3(0.08f, 0.48f, 0.12f));
    Primitives::drawCube(shader, plowBody, plowWood);

    // Steel plowshare point (Fhal / ফাল)
    mat4 fhal = teamM;
    fhal = translate(fhal, vec3(0.0f, 0.04f, -0.68f));
    fhal = rotate(fhal, radians(45.0f), vec3(1.0f, 0.0f, 0.0f));
    fhal = scale(fhal, vec3(0.095f, 0.22f, 0.035f));
    Primitives::drawCone(shader, fhal, steelFhal);

    // Upright wooden steering handle (Muthia / মুঠিয়া)
    mat4 muthia = teamM;
    muthia = translate(muthia, vec3(0.0f, 0.62f, -0.82f));
    muthia = rotate(muthia, radians(-18.0f), vec3(1.0f, 0.0f, 0.0f));
    muthia = scale(muthia, vec3(0.045f, 0.85f, 0.045f));
    Primitives::drawCylinder(shader, muthia, plowWood);

    // Handle crossbar grip
    mat4 handleGrip = teamM;
    handleGrip = translate(handleGrip, vec3(0.0f, 1.02f, -0.92f));
    handleGrip = rotate(handleGrip, radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
    handleGrip = scale(handleGrip, vec3(0.035f, 0.035f, 0.34f));
    Primitives::drawCylinder(shader, handleGrip, plowWood);

    // ── 5. The Farmer Steering Behind the Plow (হালচাষী কৃষক) ──
    float fStride = sinf(walkPhase) * radians(26.0f);
    float fBob    = fabsf(sinf(walkPhase)) * 0.022f;

    PersonParams farmer;
    farmer.skinColor   = vec3(0.48f, 0.32f, 0.18f); // Sun-tanned Bengali farmer
    farmer.shirtColor  = vec3(0.95f, 0.95f, 0.93f); // White cotton shirt (like in Image 2)
    farmer.pantsColor  = vec3(0.55f, 0.15f, 0.10f); // Hitched-up maroon lungi / kacha
    farmer.hasGamcha   = true;
    farmer.gamchaColor = vec3(0.84f, 0.18f, 0.12f); // Red gamcha around waist
    farmer.leftLegAngle  = fStride;
    farmer.rightLegAngle = -fStride;
    farmer.leftArmAngle  = radians(-68.0f); // Forward gripping plow handle
    farmer.rightArmAngle = radians(-68.0f);

    mat4 farmerM = teamM;
    farmerM = translate(farmerM, vec3(0.0f, fBob, -1.28f));
    farmerM = rotate(farmerM, radians(12.0f), vec3(1.0f, 0.0f, 0.0f)); // Leaning forward with effort
    farmerM = scale(farmerM, vec3(0.98f));
    Person::draw(shader, farmerM, farmer);

    // Checkered Headwrap / Turban (Pagri / গামছার পাগড়ি like in reference Image 2)
    mat4 pagri = farmerM;
    pagri = translate(pagri, vec3(0.0f, 1.08f, 0.02f));
    pagri = scale(pagri, vec3(0.24f, 0.12f, 0.24f));
    Primitives::drawCylinder(shader, pagri, vec3(0.92f, 0.90f, 0.86f));

    mat4 pagriBand = farmerM;
    pagriBand = translate(pagriBand, vec3(0.0f, 1.10f, 0.02f));
    pagriBand = scale(pagriBand, vec3(0.25f, 0.05f, 0.25f));
    Primitives::drawCylinder(shader, pagriBand, vec3(0.80f, 0.20f, 0.15f));

    // ── 6. Two Guide Reins from Oxen to Farmer Hands (লাগাম / রশি) ──
    for (float side : { -1.0f, 1.0f }) {
        float x0 = side * 0.65f, y0 = 1.05f, z0 = 1.35f;
        float x1 = side * 0.12f, y1 = 0.95f, z1 = -1.15f;
        float mx = (x0 + x1) * 0.5f, my = (y0 + y1) * 0.5f, mz = (z0 + z1) * 0.5f;
        float dx = x1 - x0, dy = y1 - y0, dz = z1 - z0;
        float len = sqrtf(dx * dx + dy * dy + dz * dz);
        float yaw = atan2f(dx, dz);
        float pitch = -atan2f(dy, sqrtf(dx * dx + dz * dz));

        mat4 rein = teamM;
        rein = translate(rein, vec3(mx, my, mz));
        rein = rotate(rein, yaw, vec3(0.0f, 1.0f, 0.0f));
        rein = rotate(rein, pitch, vec3(1.0f, 0.0f, 0.0f));
        rein = scale(rein, vec3(0.012f, 0.012f, len));
        Primitives::drawCube(shader, rein, reinRope);
    }
}

} // namespace House


