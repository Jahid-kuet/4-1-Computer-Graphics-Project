// House.cpp — Authentic Bangladeshi rural house (Mati-r Ghor / Tin-er Ghor)
// Featuring traditional Chouchala (4-sloped hip roof) and Dochala (2-sloped gable roof),
// raised earthen plinth (dawa/viti), bamboo verandah (baranda), wooden door and shutters,
// and terracotta water pitchers (matir kolshi).

#include "objects/House.h"
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

void drawStove(Shader& shader, const mat4& model, bool withFence)
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

    // Glowing live embers inside combustion chamber
    shader.setFloat("emissive", 0.90f);
    mat4 embers = cm;
    embers = translate(embers, vec3(0.0f, 0.055f, 0.16f));
    embers = scale(embers, vec3(0.14f, 0.045f, 0.10f));
    Primitives::drawCube(shader, embers, emberColor);
    shader.setFloat("emissive", 0.0f);

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
}

void draw(Shader& shader, const mat4& model, HouseStyle style, bool withStove)
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
            drawStove(shader, cm, true);
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
            drawStove(shader, cm, true);
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

    // 3. Multi-tiered bulging straw stack body
    // Tier 1: Flared lower base skirt (weathered slightly darker)
    mat4 t1 = m;
    t1 = translate(t1, vec3(0.0f, 0.35f, 0.0f));
    t1 = scale(t1, vec3(0.92f, 0.60f, 0.92f));
    Primitives::drawCylinder(shader, t1, strawDark);

    // Tier 2: Wide bulging belly (classic rounded village silo shape)
    mat4 t2 = m;
    t2 = translate(t2, vec3(0.0f, 0.85f, 0.0f));
    t2 = scale(t2, vec3(1.06f, 0.65f, 1.06f));
    Primitives::drawSphere(shader, t2, strawMid);

    // Tier 3: Tapering chest cone
    mat4 t3 = m;
    t3 = translate(t3, vec3(0.0f, 1.15f, 0.0f));
    t3 = scale(t3, vec3(0.96f, 0.70f, 0.96f));
    Primitives::drawCone(shader, t3, strawMid);

    // Tier 4: Steep conical rain-shedding apex thatch (tip at 1.45 + 1.25 = 2.70)
    mat4 t4 = m;
    t4 = translate(t4, vec3(0.0f, 1.45f, 0.0f));
    t4 = scale(t4, vec3(0.72f, 1.25f, 0.72f));
    Primitives::drawCone(shader, t4, strawTop);

    // 4. Horizontal straw binding ropes (Khorer Badhon / বাঁধন) wrapping the stack
    // Band 1: Lower waist
    mat4 b1 = m;
    b1 = translate(b1, vec3(0.0f, 0.52f, 0.0f));
    b1 = scale(b1, vec3(0.94f, 0.024f, 0.94f));
    Primitives::drawCylinder(shader, b1, ropeCol);

    // Band 2: Mid belly
    mat4 b2 = m;
    b2 = translate(b2, vec3(0.0f, 0.88f, 0.0f));
    b2 = scale(b2, vec3(1.07f, 0.026f, 1.07f));
    Primitives::drawCylinder(shader, b2, ropeCol);

    // Band 3: Upper cone
    mat4 b3 = m;
    b3 = translate(b3, vec3(0.0f, 1.35f, 0.0f));
    b3 = scale(b3, vec3(0.68f, 0.022f, 0.68f));
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
void drawCow(Shader& shader, const mat4& model, bool lyingDown)
{
    shader.setInt("uUseTexture", 0); // smooth natural hide, horns, and muzzle

    vec3 hideCol   (0.64f, 0.44f, 0.26f); // warm fawn-brown coat
    vec3 bellyCol  (0.78f, 0.68f, 0.54f); // lighter cream underbelly & muzzle trim
    vec3 humpCol   (0.58f, 0.38f, 0.22f); // distinctive muscular Zebu shoulder hump (Kud)
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
        for (int l = 0; l < 4; l++) {
            mat4 leg = m;
            leg = translate(leg, vec3(legX[l], 0.40f, legZ[l]));
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

    // 5. Door / Ramp behavior:
    if (isNight) {
        // At night: door is closed and secured with an iron latch bar
        mat4 door = model;
        door = translate(door, vec3(0.0f, floorY + doorH * 0.5f, coopD * 0.485f));
        door = scale(door, vec3(doorW + 0.02f, doorH, 0.03f));
        Primitives::drawCube(shader, door, floorCol);

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

} // namespace House

