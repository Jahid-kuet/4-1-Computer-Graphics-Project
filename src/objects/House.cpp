// House.cpp — Authentic Bangladeshi rural house (Mati-r Ghor / Tin-er Ghor)
// Featuring traditional Chouchala (4-sloped hip roof) and Dochala (2-sloped gable roof),
// raised earthen plinth (dawa/viti), bamboo verandah (baranda), wooden door and shutters,
// and terracotta water pitchers (matir kolshi).

#include "objects/House.h"
#include "Primitives.h"

using namespace math;

namespace {

// Helper to draw a clay water pitcher (Kolshi)
void drawKolshi(Shader& shader, const mat4& model, const vec3& pos, float scaleVal = 1.0f)
{
    vec3 clayColor(0.68f, 0.35f, 0.18f);
    vec3 rimColor (0.58f, 0.28f, 0.14f);

    mat4 m = model;
    m = translate(m, pos);
    m = scale(m, vec3(scaleVal));

    // Spherical belly
    mat4 belly = m;
    belly = translate(belly, vec3(0.0f, 0.20f, 0.0f));
    belly = scale(belly, vec3(0.22f, 0.20f, 0.22f));
    Primitives::drawSphere(shader, belly, clayColor);

    // Narrow neck
    mat4 neck = m;
    neck = translate(neck, vec3(0.0f, 0.36f, 0.0f));
    neck = scale(neck, vec3(0.09f, 0.14f, 0.09f));
    Primitives::drawCylinder(shader, neck, clayColor);

    // Flared rim
    mat4 rim = m;
    rim = translate(rim, vec3(0.0f, 0.44f, 0.0f));
    rim = scale(rim, vec3(0.14f, 0.04f, 0.14f));
    Primitives::drawCylinder(shader, rim, rimColor);
}

} // anonymous namespace

namespace House {

void draw(Shader& shader, const mat4& model, HouseStyle style)
{
    // ── Traditional Color Palette ───────────────────────────────
    vec3 plinthColor (0.42f, 0.32f, 0.20f);  // dark packed clay earth
    vec3 wallColor   (0.64f, 0.52f, 0.36f);  // sun-dried mud / clay plaster
    vec3 cornerPost  (0.32f, 0.22f, 0.12f);  // seasoned timber / dark bamboo
    vec3 roofStraw   (0.48f, 0.38f, 0.18f);  // golden weathered thatch / straw
    vec3 roofRidge   (0.36f, 0.26f, 0.12f);  // thatch ridge cap
    vec3 doorWood    (0.28f, 0.16f, 0.08f);  // dark oiled timber
    vec3 windowFrame (0.22f, 0.14f, 0.08f);  // dark frame
    vec3 shutterColor(0.36f, 0.22f, 0.12f);  // timber shutters
    vec3 postColor   (0.46f, 0.34f, 0.16f);  // bamboo verandah pillars
    vec3 rafterColor (0.38f, 0.26f, 0.12f);  // wooden rafters

    // Base house dimensions
    float houseW = 3.6f;  // width along X
    float houseH = 2.0f;  // wall height
    float houseD = 2.8f;  // depth along Z
    float plinthH = 0.25f; // raised earthen base

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

    // Horizontal bamboo tie-beam along top of walls
    mat4 beamFront = model;
    beamFront = translate(beamFront, vec3(0.0f, plinthH + houseH, hz));
    beamFront = scale(beamFront, vec3(houseW + 0.1f, 0.08f, 0.10f));
    Primitives::drawCube(shader, beamFront, cornerPost);

    // ── 3. Roof System (NO CONE ROOFS!) ──────────────────────────
    float roofBaseY = plinthH + houseH;

    if (style == HOUSE_CHOUCHALA) {
        // Traditional 4-sloped pitched hip roof (Chouchala)
        // Main hip pyramid with overhang
        float roofW = houseW + 1.2f;
        float roofD = houseD + 1.2f;
        float roofH = 1.6f;

        mat4 roof = model;
        roof = translate(roof, vec3(0.0f, roofBaseY, 0.0f));
        roof = scale(roof, vec3(roofW, roofH, roofD));
        Primitives::drawPyramid(shader, roof, roofStraw);

        // Ridge cap along top crest
        mat4 ridge = model;
        ridge = translate(ridge, vec3(0.0f, roofBaseY + roofH, 0.0f));
        ridge = scale(ridge, vec3(roofW * 0.4f, 0.10f, 0.16f));
        Primitives::drawCube(shader, ridge, roofRidge);

        // Under-eave rafter trim (slight dark underside)
        mat4 eaveTrim = model;
        eaveTrim = translate(eaveTrim, vec3(0.0f, roofBaseY - 0.02f, 0.0f));
        eaveTrim = scale(eaveTrim, vec3(roofW * 0.96f, 0.05f, roofD * 0.96f));
        Primitives::drawCube(shader, eaveTrim, rafterColor);
    }
    else {
        // Traditional 2-sloped pitched gable roof (Dochala)
        float roofW = houseW + 1.2f;
        float roofD = houseD + 0.8f;
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
    }

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

    // Door frame
    mat4 dFrame = model;
    dFrame = translate(dFrame, vec3(0.0f, doorY, hz + 0.02f));
    dFrame = scale(dFrame, vec3(doorW + 0.12f, doorH + 0.10f, 0.04f));
    Primitives::drawCube(shader, dFrame, windowFrame);

    // Door panel (slightly recessed)
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

    // Warm interior lantern glow through window
    vec3 windowGlow(0.92f, 0.70f, 0.28f);
    shader.setFloat("emissive", 0.70f);
    mat4 wOpeningL = model;
    wOpeningL = translate(wOpeningL, vec3(winLX, winY, hz + 0.03f));
    wOpeningL = scale(wOpeningL, vec3(winSize * 0.85f, winSize * 0.85f, 0.03f));
    Primitives::drawCube(shader, wOpeningL, windowGlow);
    shader.setFloat("emissive", 0.0f);

    // Wooden shutter swung open
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

    // Wooden shutter swung open
    mat4 wShutterR = model;
    wShutterR = translate(wShutterR, vec3(winRX + winSize * 0.45f, winY, hz + 0.15f));
    wShutterR = rotate(wShutterR, radians(45.0f), vec3(0.0f, 1.0f, 0.0f));
    wShutterR = scale(wShutterR, vec3(winSize * 0.45f, winSize * 0.85f, 0.025f));
    Primitives::drawCube(shader, wShutterR, shutterColor);

    // ── 7. Terracotta Water Pitchers (Matir Kolshi) on Verandah ───
    drawKolshi(shader, model, vec3(hx * 0.70f, plinthH, postZ - 0.20f), 0.90f);
    drawKolshi(shader, model, vec3(hx * 0.85f, plinthH, postZ - 0.35f), 0.75f);

    // ── 8. Outdoor Clay Cooking Stove (Matir Chula) for Kitchen Cottage ─
    if (style == HOUSE_DOCHALA) {
        vec3 mudColor  (0.46f, 0.34f, 0.20f); // baked mud clay
        vec3 potColor  (0.20f, 0.20f, 0.22f); // cast-iron cooking patil
        vec3 woodColor (0.34f, 0.22f, 0.10f); // firewood sticks

        mat4 cm = model;
        cm = translate(cm, vec3(hx + 0.90f, 0.0f, 0.25f));

        // Mud stove body
        mat4 chulaBody = cm;
        chulaBody = translate(chulaBody, vec3(0.0f, 0.14f, 0.0f));
        chulaBody = scale(chulaBody, vec3(0.42f, 0.22f, 0.38f));
        Primitives::drawCylinder(shader, chulaBody, mudColor);

        // Clay prongs supporting pot
        for (int j = 0; j < 3; j++) {
            float ja = (float)j * (2.0f * 3.14159f / 3.0f);
            mat4 prong = cm;
            prong = translate(prong, vec3(cosf(ja) * 0.16f, 0.27f, sinf(ja) * 0.16f));
            prong = scale(prong, vec3(0.04f, 0.07f, 0.04f));
            Primitives::drawCone(shader, prong, mudColor);
        }

        // Cooking pot (Hari / Patil)
        mat4 pot = cm;
        pot = translate(pot, vec3(0.0f, 0.34f, 0.0f));
        pot = scale(pot, vec3(0.18f, 0.13f, 0.18f));
        Primitives::drawSphere(shader, pot, potColor);

        // Firewood sticks
        mat4 stick1 = cm;
        stick1 = translate(stick1, vec3(0.0f, 0.06f, 0.24f));
        stick1 = rotate(stick1, radians(15.0f), vec3(1.0f, 0.0f, 0.0f));
        stick1 = scale(stick1, vec3(0.035f, 0.035f, 0.30f));
        Primitives::drawCylinder(shader, stick1, woodColor);

        mat4 stick2 = cm;
        stick2 = translate(stick2, vec3(0.06f, 0.05f, 0.25f));
        stick2 = rotate(stick2, radians(12.0f), vec3(1.0f, 0.0f, 0.0f));
        stick2 = rotate(stick2, radians(22.0f), vec3(0.0f, 1.0f, 0.0f));
        stick2 = scale(stick2, vec3(0.03f, 0.03f, 0.28f));
        Primitives::drawCylinder(shader, stick2, woodColor);
    }
}

} // namespace House
