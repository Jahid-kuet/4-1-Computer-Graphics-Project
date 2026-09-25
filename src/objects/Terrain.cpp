// Terrain.cpp — Rich Bangladeshi village environment (Gramin Poribesh)
// Features lush green meadow, beaten-earth village courtyards (Uthan),
// an extensive interconnected network of village roads & footpaths (Poth),
// traditional wooden river landing ghat, terraced paddy fields (Dhan Khet),
// traditional woven bamboo fencing (Bansh-er Bera), and meadow grass tufts.

#include "objects/Terrain.h"
#include "objects/Charpai.h"
#include "objects/House.h"
#include "Primitives.h"
#include <cmath>
#include <vector>

using namespace math;

namespace Terrain {

// Helper: Draw a section of traditional rural woven bamboo fence (Bansh-er Bera / বাঁশের বেড়া)
void drawBambooFence(Shader& shader, const mat4& model, const vec3& startPos, float length, float angleDeg)
{
    shader.setInt("uUseTexture", 0); // smooth natural weathered bamboo culms and coir lashings

    vec3 postCol  (0.42f, 0.32f, 0.16f); // weathered bamboo upright posts
    vec3 nodeCol  (0.28f, 0.20f, 0.10f); // bamboo culm nodes
    vec3 railCol  (0.56f, 0.44f, 0.24f); // horizontal split bamboo battens (Batar Batti)
    vec3 slatCol1 (0.64f, 0.52f, 0.28f); // vertical split bamboo slats (Chatai / Shola)
    vec3 slatCol2 (0.58f, 0.46f, 0.22f); // weathered bamboo slat variant
    vec3 lashing  (0.22f, 0.16f, 0.08f); // jute coir tie knots (Pat-er Badhon)

    mat4 fm = model;
    fm = translate(fm, startPos);
    fm = rotate(fm, radians(angleDeg), vec3(0.0f, 1.0f, 0.0f));

    float fenceH = 1.15f;
    float postSpacing = 0.90f;
    int numPosts = (int)(length / postSpacing) + 1;

    // 1. Upright round bamboo posts with node rings
    for (int p = 0; p < numPosts; p++) {
        float x = (float)p * (length / (float)(numPosts - 1));
        mat4 post = fm;
        post = translate(post, vec3(x, fenceH * 0.5f, 0.0f));
        post = scale(post, vec3(0.048f, fenceH, 0.048f));
        Primitives::drawCylinder(shader, post, postCol);

        // Nodes along post
        for (float ny = 0.30f; ny < fenceH; ny += 0.38f) {
            mat4 node = fm;
            node = translate(node, vec3(x, ny, 0.0f));
            node = scale(node, vec3(0.055f, 0.025f, 0.055f));
            Primitives::drawCylinder(shader, node, nodeCol);
        }
    }

    // 2. Three horizontal split-bamboo runner battens (bottom, middle, top)
    float railY[3] = { 0.22f, 0.62f, 1.02f };
    for (int r = 0; r < 3; r++) {
        mat4 rail = fm;
        rail = translate(rail, vec3(length * 0.5f, railY[r], 0.024f));
        rail = scale(rail, vec3(length + 0.10f, 0.035f, 0.025f));
        Primitives::drawCube(shader, rail, railCol);

        // Jute coir lashings at post-rail intersections
        for (int p = 0; p < numPosts; p++) {
            float px = (float)p * (length / (float)(numPosts - 1));
            mat4 lash = fm;
            lash = translate(lash, vec3(px, railY[r], 0.024f));
            lash = scale(lash, vec3(0.065f, 0.045f, 0.065f));
            Primitives::drawCube(shader, lash, lashing);
        }
    }

    // 3. Dense vertical split-bamboo slats (Bansh-er Shola) forming the authentic Bengali screen
    float slatStep = 0.065f;
    int numSlats = (int)(length / slatStep);
    for (int s = 0; s < numSlats; s++) {
        float sx = ((float)s + 0.5f) * (length / (float)numSlats);
        float sh = fenceH * (0.92f + ((s * 7) % 5) * 0.02f); // slight organic height variation

        mat4 slat = fm;
        slat = translate(slat, vec3(sx, sh * 0.5f, -0.010f));
        slat = scale(slat, vec3(0.036f, sh, 0.014f));
        Primitives::drawCube(shader, slat, (s % 2 == 0) ? slatCol1 : slatCol2);

        // Pointed top tip
        mat4 tip = fm;
        tip = translate(tip, vec3(sx, sh + 0.025f, -0.010f));
        tip = scale(tip, vec3(0.028f, 0.050f, 0.014f));
        Primitives::drawCone(shader, tip, (s % 2 == 0) ? slatCol1 : slatCol2);
    }
}

// Helper: Draw a clump of 3D meadow grass
void drawGrassClump(Shader& shader, const mat4& model, const vec3& pos, float scaleVal)
{
    vec3 grassCol(0.24f, 0.44f, 0.16f);

    for (int b = 0; b < 4; b++) {
        float bAngle = (float)b * 45.0f;
        mat4 blade = model;
        blade = translate(blade, pos);
        blade = rotate(blade, radians(bAngle), vec3(0.0f, 1.0f, 0.0f));
        blade = rotate(blade, radians(18.0f), vec3(1.0f, 0.0f, 0.0f));
        blade = scale(blade, vec3(0.06f * scaleVal, 0.45f * scaleVal, 0.02f * scaleVal));
        Primitives::drawCone(shader, blade, grassCol);
    }
}

// Helper: Draw a clump of rural Bangladeshi Rice Plants (Dhan Gachh / ধান গাছ)
void drawRiceClump(Shader& shader, const mat4& model, const vec3& pos, float scaleVal, float seed)
{
    shader.setInt("uUseTexture", 0); // vibrant agricultural greens and golden ripe paddy

    vec3 stalkCol(0.34f, 0.60f, 0.18f); // lush paddy green stalk
    vec3 leafCol (0.28f, 0.54f, 0.16f); // long arching green blade
    vec3 grainCol(0.94f, 0.82f, 0.26f); // golden ripened grain panicle (Dhaner Shish)
    vec3 huskCol (0.86f, 0.72f, 0.22f); // mature grain husk

    // A clump (Gochha) consists of 10 outward radiating stalks with gracefully arching grain heads
    const int numStalks = 10;
    for (int i = 0; i < numStalks; i++) {
        float fi = (float)i;
        float baseAngle = fi * (2.0f * PI / (float)numStalks) + seed * 0.7f;
        float tiltAngle = 14.0f + sinf(seed + fi * 1.3f) * 6.0f;
        float h = (0.78f + sinf(seed * 2.0f + fi) * 0.12f) * scaleVal;

        mat4 stalk = model;
        stalk = translate(stalk, pos);
        stalk = rotate(stalk, baseAngle, vec3(0.0f, 1.0f, 0.0f));
        stalk = rotate(stalk, radians(tiltAngle), vec3(1.0f, 0.0f, 0.0f));

        // Lower green stalk (cylinder from Y=0 to Y=h*0.75)
        mat4 sM = stalk;
        sM = translate(sM, vec3(0.0f, h * 0.38f, 0.0f));
        sM = scale(sM, vec3(0.014f * scaleVal, h * 0.76f, 0.014f * scaleVal));
        Primitives::drawCylinder(shader, sM, stalkCol);

        // Arching rice leaf blade (curving outward)
        mat4 leaf = stalk;
        leaf = translate(leaf, vec3(0.0f, h * 0.40f, 0.02f * scaleVal));
        leaf = rotate(leaf, radians(38.0f), vec3(1.0f, 0.0f, 0.0f));
        leaf = scale(leaf, vec3(0.024f * scaleVal, h * 0.65f, 0.006f));
        Primitives::drawCone(shader, leaf, leafCol);

        // Second leaf on other side
        mat4 leaf2 = stalk;
        leaf2 = translate(leaf2, vec3(0.0f, h * 0.28f, -0.02f * scaleVal));
        leaf2 = rotate(leaf2, radians(-32.0f), vec3(1.0f, 0.0f, 0.0f));
        leaf2 = scale(leaf2, vec3(0.020f * scaleVal, h * 0.55f, 0.005f));
        Primitives::drawCone(shader, leaf2, leafCol);

        // Drooping golden grain panicle (Dhaner Shish) curving gracefully downwards
        // Stage 1: Arching neck (35 degrees)
        mat4 panicle1 = stalk;
        panicle1 = translate(panicle1, vec3(0.0f, h * 0.76f, 0.0f));
        panicle1 = rotate(panicle1, radians(35.0f), vec3(1.0f, 0.0f, 0.0f));
        mat4 p1Stem = panicle1;
        p1Stem = translate(p1Stem, vec3(0.0f, 0.06f * scaleVal, 0.0f));
        p1Stem = scale(p1Stem, vec3(0.010f * scaleVal, 0.12f * scaleVal, 0.010f * scaleVal));
        Primitives::drawCylinder(shader, p1Stem, grainCol);

        // Stage 2: Heavy drooping head (steep 68 degrees downward)
        mat4 panicle2 = panicle1;
        panicle2 = translate(panicle2, vec3(0.0f, 0.12f * scaleVal, 0.0f));
        panicle2 = rotate(panicle2, radians(42.0f), vec3(1.0f, 0.0f, 0.0f));
        mat4 p2Stem = panicle2;
        p2Stem = translate(p2Stem, vec3(0.0f, 0.10f * scaleVal, 0.0f));
        p2Stem = scale(p2Stem, vec3(0.008f * scaleVal, 0.20f * scaleVal, 0.008f * scaleVal));
        Primitives::drawCylinder(shader, p2Stem, grainCol);

        // 7 Golden grain kernels attached along the drooping panicle
        for (int k = 0; k < 7; k++) {
            float kt = (float)k / 6.0f;
            float ky = (0.02f + kt * 0.18f) * scaleVal;
            float kSide = (k % 2 == 0) ? 1.0f : -1.0f;
            mat4 grain = panicle2;
            grain = translate(grain, vec3(kSide * 0.014f * scaleVal, ky, 0.0f));
            grain = rotate(grain, radians(kSide * 28.0f), vec3(0.0f, 0.0f, 1.0f));
            grain = scale(grain, vec3(0.014f * scaleVal, 0.038f * scaleVal, 0.012f * scaleVal));
            Primitives::drawSphere(shader, grain, (k % 2 == 0) ? grainCol : huskCol);
        }
    }
}

// Helper: Draw a terraced rural Bangladeshi Paddy Field (Dhan Khet / ধান ক্ষেত)
static void drawPaddyField(Shader& shader, const mat4& model, const vec3& center, float width, float length)
{
    vec3 soilCol(0.24f, 0.32f, 0.16f); // damp fertile paddy soil
    vec3 aalCol (0.46f, 0.36f, 0.22f); // raised clay boundary ridge (Aal)

    // 1. Muddy field floor (slightly recessed water/earth bed)
    mat4 bed = model;
    bed = translate(bed, vec3(center.x, 0.006f, center.z));
    bed = scale(bed, vec3(width, 1.0f, length));
    Primitives::drawPlane(shader, bed, soilCol);

    // 2. Earthen boundary dikes (Aal) bordering all 4 sides of the plot
    float halfW = width * 0.5f;
    float halfL = length * 0.5f;
    float aalH  = 0.08f;
    float aalW  = 0.34f;

    // North & South dikes
    for (int side = -1; side <= 1; side += 2) {
        float fside = (float)side;
        mat4 dikeZ = model;
        dikeZ = translate(dikeZ, vec3(center.x, aalH * 0.5f, center.z + fside * halfL));
        dikeZ = scale(dikeZ, vec3(width + aalW, aalH, aalW));
        Primitives::drawCube(shader, dikeZ, aalCol);
    }
    // East & West dikes
    for (int side = -1; side <= 1; side += 2) {
        float fside = (float)side;
        mat4 dikeX = model;
        dikeX = translate(dikeX, vec3(center.x + fside * halfW, aalH * 0.5f, center.z));
        dikeX = scale(dikeX, vec3(aalW, aalH, length));
        Primitives::drawCube(shader, dikeX, aalCol);
    }

    // 3. Grid of rice plant clumps (Dhan Gachh)
    int rows = 6;
    int cols = 7;
    float stepX = (width - 1.2f) / (cols - 1);
    float stepZ = (length - 1.2f) / (rows - 1);

    for (int r = 0; r < rows; r++) {
        float z = (center.z - halfL + 0.6f) + (float)r * stepZ;
        for (int c = 0; c < cols; c++) {
            float x = (center.x - halfW + 0.6f) + (float)c * stepX;
            float seed = (float)(r * 11 + c * 7);
            float jx = sinf(seed) * 0.08f;
            float jz = cosf(seed) * 0.08f;
            drawRiceClump(shader, model, vec3(x + jx, 0.01f, z + jz), 0.95f, seed);
        }
    }
}

// Helper: Draw a continuous road/footpath along a series of waypoint nodes
struct RoadNode { float x, z; };

static void drawRoadStrip(Shader& shader, const mat4& model, const RoadNode* nodes, int count, float width, const vec3& color, float yLevel = 0.008f)
{
    for (int p = 0; p < count - 1; p++) {
        float mx = (nodes[p].x + nodes[p+1].x) * 0.5f;
        float mz = (nodes[p].z + nodes[p+1].z) * 0.5f;
        float dx = nodes[p+1].x - nodes[p].x;
        float dz = nodes[p+1].z - nodes[p].z;
        float len = sqrtf(dx * dx + dz * dz);
        float ang = atan2f(dx, dz);

        mat4 seg = model;
        seg = translate(seg, vec3(mx, yLevel, mz));
        seg = rotate(seg, ang, vec3(0.0f, 1.0f, 0.0f));
        seg = scale(seg, vec3(width, 1.0f, len * 1.08f));
        Primitives::drawPlane(shader, seg, color);
    }
}

// Standalone traditional river landing ghat (Nodi-r Ghat / নদীর খেয়া ঘাট)
void drawGhat(Shader& shader, const mat4& model, const vec3& pos)
{
    shader.setInt("uUseTexture", 0); // authentic weathered timber and bamboo

    vec3 woodCol   (0.42f, 0.28f, 0.14f); // seasoned dark timber planks
    vec3 postCol   (0.34f, 0.22f, 0.10f); // vertical piling posts
    vec3 bambooCol (0.54f, 0.44f, 0.22f); // bamboo safety handrail & cross-beams
    vec3 ropeCol   (0.68f, 0.58f, 0.36f); // coir / jute mooring ropes

    mat4 m = model;
    m = translate(m, pos);

    // 1. Vertical bamboo/timber pilings driven into bank and water
    float posts[4][2] = {
        { -0.9f, -1.2f }, {  0.9f, -1.2f },
        { -0.9f,  1.2f }, {  0.9f,  1.2f }
    };
    for (int i = 0; i < 4; i++) {
        mat4 p = m;
        p = translate(p, vec3(posts[i][0], 0.35f, posts[i][1]));
        p = scale(p, vec3(0.075f, 1.0f, 0.075f));
        Primitives::drawCylinder(shader, p, postCol);

        // Bamboo nodes
        for (int nd = 0; nd < 2; nd++) {
            mat4 node = p;
            node = translate(node, vec3(0.0f, (float)nd * 0.35f - 0.15f, 0.0f));
            node = scale(node, vec3(1.15f, 0.04f, 1.15f));
            Primitives::drawCylinder(shader, node, postCol * 0.85f);
        }

        // Mooring rope loops around front water posts
        if (posts[i][1] > 0.0f) {
            for (int r = 0; r < 2; r++) {
                mat4 rp = m;
                rp = translate(rp, vec3(posts[i][0], 0.42f + (float)r * 0.05f, posts[i][1]));
                rp = scale(rp, vec3(0.11f, 0.04f, 0.11f));
                Primitives::drawCylinder(shader, rp, ropeCol);
            }
        }
    }

    // 2. Bamboo horizontal cross-support beams under platforms
    mat4 bBeam1 = m;
    bBeam1 = translate(bBeam1, vec3(0.0f, 0.12f, -0.6f));
    bBeam1 = rotate(bBeam1, radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
    bBeam1 = scale(bBeam1, vec3(0.045f, 2.1f, 0.045f));
    Primitives::drawCylinder(shader, bBeam1, bambooCol);

    mat4 bBeam2 = m;
    bBeam2 = translate(bBeam2, vec3(0.0f, 0.02f, 0.8f));
    bBeam2 = rotate(bBeam2, radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
    bBeam2 = scale(bBeam2, vec3(0.045f, 2.0f, 0.045f));
    Primitives::drawCylinder(shader, bBeam2, bambooCol);

    // 3. Stepped timber landing platforms descending toward water level
    // Upper deck platform (composed of individual timber planks)
    for (int pl = 0; pl < 6; pl++) {
        float pz = -1.2f + (float)pl * 0.22f;
        mat4 plank = m;
        plank = translate(plank, vec3(0.0f, 0.18f, pz));
        plank = scale(plank, vec3(2.2f, 0.06f, 0.19f));
        Primitives::drawCube(shader, plank, (pl % 2 == 0) ? woodCol : (woodCol * 0.94f));
    }

    // Middle step
    for (int pl = 0; pl < 4; pl++) {
        float pz = 0.15f + (float)pl * 0.22f;
        mat4 plank = m;
        plank = translate(plank, vec3(0.0f, 0.12f, pz));
        plank = scale(plank, vec3(2.0f, 0.055f, 0.19f));
        Primitives::drawCube(shader, plank, (pl % 2 == 0) ? woodCol : (woodCol * 0.94f));
    }

    // Lower water step (submerged right at water surface)
    for (int pl = 0; pl < 3; pl++) {
        float pz = 0.95f + (float)pl * 0.22f;
        mat4 plank = m;
        plank = translate(plank, vec3(0.0f, 0.05f, pz));
        plank = scale(plank, vec3(1.85f, 0.05f, 0.19f));
        Primitives::drawCube(shader, plank, (pl % 2 == 0) ? (woodCol * 0.85f) : (woodCol * 0.80f));
    }

    // 4. Bamboo safety handrail on right side (+X)
    mat4 rail = m;
    rail = translate(rail, vec3(0.9f, 0.65f, 0.0f));
    rail = rotate(rail, radians(-12.0f), vec3(1.0f, 0.0f, 0.0f)); // descending slope with the steps
    rail = rotate(rail, radians(90.0f), vec3(1.0f, 0.0f, 0.0f));  // orient along Z
    rail = scale(rail, vec3(0.035f, 2.5f, 0.035f));
    Primitives::drawCylinder(shader, rail, bambooCol);

    // 5. Traditional terracotta water pitcher (Kolshi) resting on ghat step
    mat4 kolshiM = m;
    kolshiM = translate(kolshiM, vec3(0.55f, 0.15f, 0.35f));
    kolshiM = scale(kolshiM, vec3(0.72f));
    House::drawKolshi(shader, kolshiM);

    // 6. Hanging Hurricane Lantern (Hariken) mounted on rear timber piling post
    mat4 ghatLantern = m;
    ghatLantern = translate(ghatLantern, vec3(-0.9f, 0.68f, -1.2f));
    ghatLantern = scale(ghatLantern, vec3(0.72f));
    Charpai::drawLantern(shader, ghatLantern);
}

void draw(Shader& shader, const mat4& model)
{
    // ── Palette ──────────────────────────────────────────────────
    vec3 grassGreen  (0.30f, 0.48f, 0.20f); // lush Bengal green
    vec3 darkGreen   (0.22f, 0.38f, 0.14f); // deeper vegetation green
    vec3 uthanEarth  (0.60f, 0.48f, 0.32f); // beaten clay courtyard
    vec3 uthanBorder (0.50f, 0.40f, 0.26f); // softer trampled border
    vec3 mainRoadCol (0.56f, 0.44f, 0.28f); // earthen main village road
    vec3 footPathCol (0.58f, 0.47f, 0.30f); // earthen branching footpaths
    vec3 brickSoling (0.64f, 0.42f, 0.28f); // brick-soling path leading to mosque

    // ── 1. Base Meadow Terrain ───────────────────────────────────
    mat4 basePlane = model;
    basePlane = scale(basePlane, vec3(85.0f, 1.0f, 85.0f));
    Primitives::drawPlane(shader, basePlane, grassGreen);

    // ── 2. Swept-Clay Courtyards (Uthan) for Different Homesteads ─
    // A. Main Central Homestead Courtyard (around House 1, Charpai, Child, Stove)
    mat4 uthan1 = model;
    uthan1 = translate(uthan1, vec3(-6.5f, 0.005f, -4.5f));
    uthan1 = scale(uthan1, vec3(17.0f, 1.0f, 18.0f));
    Primitives::drawPlane(shader, uthan1, uthanEarth);

    mat4 uthan1Border = model;
    uthan1Border = translate(uthan1Border, vec3(-6.5f, 0.004f, -4.5f));
    uthan1Border = scale(uthan1Border, vec3(19.0f, 1.0f, 20.0f));
    Primitives::drawPlane(shader, uthan1Border, uthanBorder);

    // Front yard extension (gathering area around charpai & lantern)
    mat4 uthan2 = model;
    uthan2 = translate(uthan2, vec3(-3.0f, 0.006f, 1.5f));
    uthan2 = scale(uthan2, vec3(10.0f, 1.0f, 9.0f));
    Primitives::drawPlane(shader, uthan2, uthanEarth);

    // B. North Homestead Courtyard (around House 2 & Cow Shed)
    mat4 uthanNorth = model;
    uthanNorth = translate(uthanNorth, vec3(-14.0f, 0.005f, -7.5f));
    uthanNorth = scale(uthanNorth, vec3(12.0f, 1.0f, 11.0f));
    Primitives::drawPlane(shader, uthanNorth, uthanEarth);

    // C. Mosque Courtyard & Apron (around Village Mosque at North River Bend)
    mat4 uthanMosque = model;
    uthanMosque = translate(uthanMosque, vec3(-3.8f, 0.005f, -17.5f));
    uthanMosque = scale(uthanMosque, vec3(12.0f, 1.0f, 13.0f));
    Primitives::drawPlane(shader, uthanMosque, uthanEarth);

    // D. South Homestead Courtyard (around House 3 & Straw Stacks)
    mat4 uthanSouth = model;
    uthanSouth = translate(uthanSouth, vec3(-7.5f, 0.005f, 15.0f));
    uthanSouth = scale(uthanSouth, vec3(11.0f, 1.0f, 10.0f));
    Primitives::drawPlane(shader, uthanSouth, uthanEarth);

    // ── 3. Comprehensive Village Road Network (Gramin Rasta-ghat) ─
    // Road 1: Main Village Road (North to South spine traversing the whole village)
    const RoadNode mainRoad[] = {
        { -5.8f, -26.0f },
        { -5.2f, -18.0f },
        { -4.4f, -10.0f },
        { -3.6f,  -2.0f },
        { -3.4f,   4.0f },
        { -4.2f,  12.0f },
        { -5.0f,  20.0f },
        { -5.6f,  26.0f }
    };
    drawRoadStrip(shader, model, mainRoad, 8, 2.10f, mainRoadCol, 0.007f);

    // Road 2: River Ghat Road (Connects Main Road & Courtyard down to the river landing ghat)
    const RoadNode ghatRoad[] = {
        { -3.5f,  0.8f },
        { -1.2f,  0.9f },
        {  1.2f,  1.1f },
        {  3.4f,  1.2f },
        {  5.2f,  1.2f }  // terminates right at the wooden landing ghat
    };
    drawRoadStrip(shader, model, ghatRoad, 5, 1.60f, footPathCol, 0.008f);

    // Road 3: Mosque Access Road (Branches east from main road directly to the mosque entrance portal)
    const RoadNode mosqueRoad[] = {
        { -5.2f, -14.2f },
        { -4.5f, -14.0f },
        { -3.8f, -13.6f }   // connects to mosque front steps
    };
    drawRoadStrip(shader, model, mosqueRoad, 3, 1.65f, brickSoling, 0.008f);

    // Road 4: North Homestead Path (Leading to Dochala House, Cow Shed & Bamboo Grove)
    const RoadNode northPath[] = {
        {  -4.8f, -14.0f },
        {  -8.0f, -12.5f },
        { -11.5f, -10.5f },
        { -14.0f,  -8.0f }
    };
    drawRoadStrip(shader, model, northPath, 4, 1.40f, footPathCol, 0.008f);

    // Road 5: South Homestead & Paddy Field Path (Leading to House 3 and Paddy Dikes)
    const RoadNode southPath[] = {
        {  -4.6f, 14.5f },
        {  -7.5f, 14.8f },
        { -11.0f, 15.2f },
        { -13.5f, 16.0f }
    };
    drawRoadStrip(shader, model, southPath, 4, 1.35f, footPathCol, 0.008f);

    // Road 6: Riverside Fisherman Path (Runs along riverbank to northern cottage)
    const RoadNode riverPath[] = {
        { 5.2f,   1.2f },
        { 4.2f,  -3.5f },
        { 3.2f,  -7.5f },
        { 1.8f, -11.0f }  // leads to riverside cottage
    };
    drawRoadStrip(shader, model, riverPath, 4, 1.30f, footPathCol, 0.008f);

    // ── 4. River Landing Ghat (Wooden / Bamboo Platform & Steps) ─
    drawGhat(shader, model, vec3(5.6f, 0.0f, 1.2f));

    // ── 5. Terraced Paddy Fields (Dhan Khet / ধান ক্ষেত) ─────────
    // Primary large agricultural paddy field with neat rows of rice plants and raised dikes (Aal)
    drawPaddyField(shader, model, vec3(-14.5f, 0.0f, 20.5f), 10.5f, 11.5f);

    // Secondary smaller terraced paddy plot near the southern border
    drawPaddyField(shader, model, vec3(-6.5f, 0.0f, 23.0f), 7.5f, 6.5f);

    // ── 6. Woven Bamboo Fences (Bansh-er Bera) ───────────────────
    // Fence 1: Back boundary behind North homestead & cow shed
    drawBambooFence(shader, model, vec3(-17.5f, 0.0f, -6.5f), 7.0f, 12.0f);

    // Fence 2: Separating North cow shed from main courtyard
    drawBambooFence(shader, model, vec3(-16.0f, 0.0f, -1.5f), 5.5f, 90.0f);

    // Fence 3: Enclosing south side of main courtyard along the path
    drawBambooFence(shader, model, vec3(-11.5f, 0.0f, 4.5f), 6.0f, 0.0f);

    // Fence 4: Garden fence near House 3 and paddy fields
    drawBambooFence(shader, model, vec3(-12.5f, 0.0f, 13.5f), 6.5f, 0.0f);

    // Fence 5: Riverside barrier fence along northern bank
    drawBambooFence(shader, model, vec3(1.2f, 0.0f, -6.5f), 4.5f, 75.0f);

    // Fence 6: Foreground courtyard rustic bamboo fence framing the broader view
    drawBambooFence(shader, model, vec3(-5.8f, 0.0f, 3.8f), 6.2f, 4.0f);
    drawBambooFence(shader, model, vec3( 0.8f, 0.0f, 3.9f), 4.0f, 24.0f);

    // ── 7. Meadow Grass Tufts (Scattered along road borders) ─────
    drawGrassClump(shader, model, vec3(-4.5f, 0.0f,  5.5f), 1.20f);
    drawGrassClump(shader, model, vec3(-2.8f, 0.0f, -1.5f), 1.10f);
    drawGrassClump(shader, model, vec3(-6.2f, 0.0f, 10.5f), 1.30f);
    drawGrassClump(shader, model, vec3( 2.2f, 0.0f,  3.5f), 1.15f);
    drawGrassClump(shader, model, vec3(-1.5f, 0.0f, -8.5f), 1.25f);
    drawGrassClump(shader, model, vec3(-10.5f, 0.0f, -13.0f), 1.20f);
}

} // namespace Terrain
