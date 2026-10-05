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
    float slatStep = 0.10f;
    int numSlats = (int)(length / slatStep);
    for (int s = 0; s < numSlats; s++) {
        float sx = ((float)s + 0.5f) * (length / (float)numSlats);
        float sh = fenceH * (0.92f + ((s * 7) % 5) * 0.02f); // slight organic height variation

        mat4 slat = fm;
        slat = translate(slat, vec3(sx, sh * 0.5f, -0.010f));
        slat = scale(slat, vec3(0.062f, sh, 0.016f));
        Primitives::drawCube(shader, slat, (s % 2 == 0) ? slatCol1 : slatCol2);
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

    // A clump (Gochha) consists of outward radiating stalks with gracefully arching golden grain heads
    // Adaptive stalk count: detailed 8 stalks for standalone single-object inspection, 4 stalks for village paddy fields
    const int numStalks = (scaleVal > 1.4f) ? 8 : 4;
    for (int i = 0; i < numStalks; i++) {
        float fi = (float)i;
        float baseAngle = fi * (2.0f * PI / (float)numStalks) + seed * 0.7f;
        float tiltAngle = 14.0f + sinf(seed + fi * 1.3f) * 5.0f;
        float h = (0.78f + sinf(seed * 2.0f + fi) * 0.12f) * scaleVal;

        mat4 stalk = model;
        stalk = translate(stalk, pos);
        stalk = rotate(stalk, baseAngle, vec3(0.0f, 1.0f, 0.0f));
        stalk = rotate(stalk, radians(tiltAngle), vec3(1.0f, 0.0f, 0.0f));

        // Lower green stalk (cylinder from Y=0 to Y=h*0.75)
        mat4 sM = stalk;
        sM = translate(sM, vec3(0.0f, h * 0.38f, 0.0f));
        sM = scale(sM, vec3(0.016f * scaleVal, h * 0.76f, 0.016f * scaleVal));
        Primitives::drawCylinder(shader, sM, stalkCol);

        // Arching rice leaf blade (curving outward)
        mat4 leaf = stalk;
        leaf = translate(leaf, vec3(0.0f, h * 0.40f, 0.02f * scaleVal));
        leaf = rotate(leaf, radians(38.0f), vec3(1.0f, 0.0f, 0.0f));
        leaf = scale(leaf, vec3(0.028f * scaleVal, h * 0.65f, 0.008f));
        Primitives::drawCone(shader, leaf, leafCol);

        // Drooping golden grain panicle (Dhaner Shish) curving gracefully downwards
        // Stage 1: Arching neck (35 degrees)
        mat4 panicle1 = stalk;
        panicle1 = translate(panicle1, vec3(0.0f, h * 0.76f, 0.0f));
        panicle1 = rotate(panicle1, radians(35.0f), vec3(1.0f, 0.0f, 0.0f));
        mat4 p1Stem = panicle1;
        p1Stem = translate(p1Stem, vec3(0.0f, 0.06f * scaleVal, 0.0f));
        p1Stem = scale(p1Stem, vec3(0.012f * scaleVal, 0.12f * scaleVal, 0.012f * scaleVal));
        Primitives::drawCylinder(shader, p1Stem, grainCol);

        // Stage 2: Heavy drooping head with golden ripe grain ear
        mat4 panicle2 = panicle1;
        panicle2 = translate(panicle2, vec3(0.0f, 0.12f * scaleVal, 0.0f));
        panicle2 = rotate(panicle2, radians(44.0f), vec3(1.0f, 0.0f, 0.0f));
        mat4 p2Stem = panicle2;
        p2Stem = translate(p2Stem, vec3(0.0f, 0.11f * scaleVal, 0.0f));
        p2Stem = scale(p2Stem, vec3(0.018f * scaleVal, 0.22f * scaleVal, 0.014f * scaleVal));
        Primitives::drawCylinder(shader, p2Stem, grainCol);

        // Drooping tapered apex tip
        mat4 tip = panicle2;
        tip = translate(tip, vec3(0.0f, 0.23f * scaleVal, 0.0f));
        tip = scale(tip, vec3(0.016f * scaleVal, 0.06f * scaleVal, 0.012f * scaleVal));
        Primitives::drawCone(shader, tip, huskCol);

        // Golden grain clusters along the heavy drooping ear
        mat4 g1 = panicle2;
        g1 = translate(g1, vec3(0.012f * scaleVal, 0.07f * scaleVal, 0.0f));
        g1 = scale(g1, vec3(0.022f * scaleVal, 0.045f * scaleVal, 0.018f * scaleVal));
        Primitives::drawSphere(shader, g1, grainCol);

        mat4 g2 = panicle2;
        g2 = translate(g2, vec3(-0.012f * scaleVal, 0.14f * scaleVal, 0.0f));
        g2 = scale(g2, vec3(0.020f * scaleVal, 0.042f * scaleVal, 0.016f * scaleVal));
        Primitives::drawSphere(shader, g2, huskCol);
    }
}

// Helper: Draw a terraced rural Bangladeshi Paddy Field (Dhan Khet / ধান ক্ষেত)
static void drawPaddyField(Shader& shader, const mat4& model, const vec3& center, float width, float length)
{
    vec3 soilCol (0.20f, 0.28f, 0.16f); // rich fertile damp alluvial silt
    vec3 waterCol(0.18f, 0.32f, 0.24f); // shallow flooded irrigation water sheen
    vec3 aalCol  (0.48f, 0.36f, 0.22f); // sun-baked clay boundary ridge (Aal)

    // 1. Muddy field floor (elevated above base terrain to eliminate any Z-fighting or bleed-through)
    mat4 bed = model;
    bed = translate(bed, vec3(center.x, 0.016f, center.z));
    bed = scale(bed, vec3(width, 1.0f, length));
    Primitives::drawPlane(shader, bed, soilCol);

    // 1b. Glistening shallow water layer over fertile mud
    mat4 water = model;
    water = translate(water, vec3(center.x, 0.018f, center.z));
    water = scale(water, vec3(width * 0.98f, 1.0f, length * 0.98f));
    Primitives::drawPlane(shader, water, waterCol);

    // 2. Earthen boundary dikes (Aal) bordering all 4 sides of the plot
    float halfW = width * 0.5f;
    float halfL = length * 0.5f;
    float aalH  = 0.10f;
    float aalW  = 0.36f;

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
            drawRiceClump(shader, model, vec3(x + jx, 0.020f, z + jz), 0.95f, seed);
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

// Helper: Draw a richly layered authentic Bangladeshi Grameen Rasta (গ্রামীণ কাঁচা মেঠোপথ)
// Features raised earthen embankment, sun-baked clay roadbed, central cart/foot track,
// and sloping shoulder berms.
static void drawGrameenRasta(Shader& shader, const mat4& model, const RoadNode* nodes, int count, float roadWidth = 2.40f)
{
    vec3 centralRoadbed (0.64f, 0.52f, 0.35f); // sun-dried golden-tan sandy loam
    vec3 cartRuts       (0.58f, 0.46f, 0.30f); // compacted cart/pedestrian track
    vec3 sideShoulders  (0.48f, 0.38f, 0.22f); // sloping earthen berms

    // Layer 1: Raised base earthen embankment / sloping side shoulders (width + 0.50m)
    drawRoadStrip(shader, model, nodes, count, roadWidth + 0.50f, sideShoulders, 0.007f);

    // Layer 2: Main crown roadway
    drawRoadStrip(shader, model, nodes, count, roadWidth, centralRoadbed, 0.010f);

    // Layer 3: Central worn pedestrian / cart rut
    drawRoadStrip(shader, model, nodes, count, roadWidth * 0.45f, cartRuts, 0.012f);
}

// Helper: Draw a traditional Bangladeshi roadside / culvert concrete post (সাদা-লাল আরসিসি কালভার্ট পোস্ট)
static void drawCulvertPost(Shader& shader, const mat4& model, const vec3& pos)
{
    vec3 whiteCol(0.92f, 0.90f, 0.86f);
    vec3 redCol  (0.85f, 0.24f, 0.18f);

    // White base pillar (height 0.52m, square 0.13m x 0.13m)
    mat4 base = model;
    base = translate(base, vec3(pos.x, 0.20f, pos.z));
    base = scale(base, vec3(0.13f, 0.40f, 0.13f));
    Primitives::drawCube(shader, base, whiteCol);

    // Red top cap
    mat4 cap = model;
    cap = translate(cap, vec3(pos.x, 0.46f, pos.z));
    cap = scale(cap, vec3(0.135f, 0.12f, 0.135f));
    Primitives::drawCube(shader, cap, redCol);
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
    shader.setInt("uUseTexture", 0); // authentic untextured earth, courtyard clay, and alluvial paddy fields

    // ── Palette ──────────────────────────────────────────────────
    vec3 grassGreen  (0.30f, 0.48f, 0.20f); // lush Bengal green
    vec3 darkGreen   (0.22f, 0.38f, 0.14f); // deeper vegetation green
    vec3 uthanEarth  (0.60f, 0.48f, 0.32f); // beaten clay courtyard
    vec3 uthanBorder (0.50f, 0.40f, 0.26f); // softer trampled border
    vec3 mainRoadCol (0.56f, 0.44f, 0.28f); // earthen main village road
    vec3 footPathCol (0.58f, 0.47f, 0.30f); // earthen branching footpaths
    vec3 brickSoling (0.64f, 0.42f, 0.28f); // brick-soling path leading to mosque

    // ── 1. Base Meadow Terrain (Expansive 95m x 95m rural Bengal landscape) ─
    mat4 basePlane = model;
    basePlane = scale(basePlane, vec3(96.0f, 1.0f, 96.0f));
    Primitives::drawPlane(shader, basePlane, grassGreen);

    // ── 2. Swept-Clay Courtyards (Uthan) Across the Entire Landscape ─
    // A. Main Central Homestead Courtyard (Moddho Bari)
    mat4 uthan1 = model;
    uthan1 = translate(uthan1, vec3(-11.5f, 0.005f, -4.5f));
    uthan1 = scale(uthan1, vec3(18.5f, 1.0f, 16.5f));
    Primitives::drawPlane(shader, uthan1, uthanEarth);

    mat4 uthan1Border = model;
    uthan1Border = translate(uthan1Border, vec3(-11.5f, 0.004f, -4.5f));
    uthan1Border = scale(uthan1Border, vec3(21.0f, 1.0f, 19.0f));
    Primitives::drawPlane(shader, uthan1Border, uthanBorder);

    // Front gathering yard extension (around Charpai, Lantern, and Villagers)
    mat4 uthan2 = model;
    uthan2 = translate(uthan2, vec3(-3.2f, 0.006f, 1.2f));
    uthan2 = scale(uthan2, vec3(9.5f, 1.0f, 8.5f));
    Primitives::drawPlane(shader, uthan2, uthanEarth);

    // B. North Homestead Farmstead Courtyard (Uttar Bari)
    mat4 uthanNorth = model;
    uthanNorth = translate(uthanNorth, vec3(-21.5f, 0.005f, -16.5f));
    uthanNorth = scale(uthanNorth, vec3(18.0f, 1.0f, 18.0f));
    Primitives::drawPlane(shader, uthanNorth, uthanEarth);

    // C. West Homestead Courtyard (Paschim Bari)
    mat4 uthanWest = model;
    uthanWest = translate(uthanWest, vec3(-27.5f, 0.005f, 5.5f));
    uthanWest = scale(uthanWest, vec3(18.0f, 1.0f, 20.0f));
    Primitives::drawPlane(shader, uthanWest, uthanEarth);

    // D. Mosque Courtyard & Apron (North-West Village Sanctuary)
    mat4 uthanMosque = model;
    uthanMosque = translate(uthanMosque, vec3(-33.0f, 0.005f, -34.0f));
    uthanMosque = scale(uthanMosque, vec3(17.0f, 1.0f, 17.0f));
    Primitives::drawPlane(shader, uthanMosque, uthanEarth);

    // E. South Homestead Courtyard (Dokkhin Bari)
    mat4 uthanSouth = model;
    uthanSouth = translate(uthanSouth, vec3(-14.5f, 0.005f, 17.0f));
    uthanSouth = scale(uthanSouth, vec3(18.0f, 1.0f, 15.0f));
    Primitives::drawPlane(shader, uthanSouth, uthanEarth);

    // F. Riverside Fisherman Courtyards (North & South on West Bank terrace)
    mat4 uthanRiverN = model;
    uthanRiverN = translate(uthanRiverN, vec3(-9.0f, 0.005f, -21.5f));
    uthanRiverN = scale(uthanRiverN, vec3(8.5f, 1.0f, 9.0f));
    Primitives::drawPlane(shader, uthanRiverN, uthanEarth);

    mat4 uthanRiverS = model;
    uthanRiverS = translate(uthanRiverS, vec3(-8.5f, 0.005f, 7.0f));
    uthanRiverS = scale(uthanRiverS, vec3(8.5f, 1.0f, 9.0f));
    Primitives::drawPlane(shader, uthanRiverS, uthanEarth);

    // G. Far-West Artisan & Weaver Colony Courtyard (Tanti Para)
    mat4 uthanTanti = model;
    uthanTanti = translate(uthanTanti, vec3(-38.5f, 0.005f, 4.5f));
    uthanTanti = scale(uthanTanti, vec3(16.0f, 1.0f, 24.0f));
    Primitives::drawPlane(shader, uthanTanti, uthanEarth);

    // H. North-West Meadow Farmstead Courtyard (Uttar-Paschim Khemotbari)
    mat4 uthanNW = model;
    uthanNW = translate(uthanNW, vec3(-26.5f, 0.005f, -28.0f));
    uthanNW = scale(uthanNW, vec3(17.0f, 1.0f, 18.0f));
    Primitives::drawPlane(shader, uthanNW, uthanEarth);

    // I. South Agricultural Hamlet Courtyard (Dokkhin-Para Krishi Bari)
    mat4 uthanSouthDeep = model;
    uthanSouthDeep = translate(uthanSouthDeep, vec3(-26.5f, 0.005f, 34.0f));
    uthanSouthDeep = scale(uthanSouthDeep, vec3(24.0f, 1.0f, 12.0f));
    Primitives::drawPlane(shader, uthanSouthDeep, uthanEarth);

    // J. North Riverside Hamlet Courtyard (House 9A & 9B)
    mat4 uthanNorthRiver = model;
    uthanNorthRiver = translate(uthanNorthRiver, vec3(-11.5f, 0.005f, -34.0f));
    uthanNorthRiver = scale(uthanNorthRiver, vec3(12.0f, 1.0f, 14.0f));
    Primitives::drawPlane(shader, uthanNorthRiver, uthanEarth);

    // K. Purbopara Central Courtyard (East Village - Translated Far into Eastern Meadow)
    mat4 uthanEastMid = model;
    uthanEastMid = translate(uthanEastMid, vec3(35.5f, 0.005f, -1.5f));
    uthanEastMid = scale(uthanEastMid, vec3(18.0f, 1.0f, 18.0f));
    Primitives::drawPlane(shader, uthanEastMid, uthanEarth);

    // L. Purbopara North Courtyard (East Village)
    mat4 uthanEastNorth = model;
    uthanEastNorth = translate(uthanEastNorth, vec3(34.5f, 0.005f, -22.5f));
    uthanEastNorth = scale(uthanEastNorth, vec3(16.0f, 1.0f, 20.0f));
    Primitives::drawPlane(shader, uthanEastNorth, uthanEarth);

    // M. Purbopara South Courtyard (East Village)
    mat4 uthanEastSouth = model;
    uthanEastSouth = translate(uthanEastSouth, vec3(31.5f, 0.005f, 19.5f));
    uthanEastSouth = scale(uthanEastSouth, vec3(14.0f, 1.0f, 16.5f));
    Primitives::drawPlane(shader, uthanEastSouth, uthanEarth);

    // ── 3. Comprehensive Village Road Network Across the Entire Plane ─
    // Road 1: Main Village Road (North to South spine spanning the full terrain)
    const RoadNode mainRoad[] = {
        { -5.8f, -42.0f },
        { -5.6f, -32.0f },
        { -5.2f, -18.0f },
        { -4.4f, -10.0f },
        { -3.6f,  -2.0f },
        { -3.4f,   4.0f },
        { -4.2f,  12.0f },
        { -5.0f,  20.0f },
        { -5.4f,  30.0f },
        { -5.6f,  42.0f }
    };
    drawRoadStrip(shader, model, mainRoad, 10, 2.10f, mainRoadCol, 0.007f);

    // Road 2: River Ghat Road (Connects Main Road & Courtyard down to the landing ghat)
    const RoadNode ghatRoad[] = {
        { -3.5f,  0.8f },
        { -1.2f,  0.9f },
        {  1.2f,  1.1f },
        {  3.4f,  1.2f },
        {  5.2f,  1.2f }
    };
    drawRoadStrip(shader, model, ghatRoad, 5, 1.60f, footPathCol, 0.008f);

    // Road 3: Mosque Access Road (Traditional brick-soling path leading to North-West Village Mosque)
    const RoadNode mosqueRoad[] = {
        { -21.5f, -23.0f },
        { -25.5f, -26.0f },
        { -29.0f, -28.5f },
        { -33.0f, -28.5f }
    };
    drawRoadStrip(shader, model, mosqueRoad, 4, 1.65f, brickSoling, 0.008f);

    // Road 4: North Homestead Path
    const RoadNode northPath[] = {
        {  -4.8f, -13.0f },
        { -10.0f, -13.2f },
        { -15.0f, -13.0f },
        { -21.0f, -13.5f }
    };
    drawRoadStrip(shader, model, northPath, 4, 1.40f, footPathCol, 0.008f);

    // Road 5: West Homestead Path
    const RoadNode westPath[] = {
        {  -3.6f,  3.5f },
        {  -9.5f,  3.8f },
        { -16.5f,  4.0f },
        { -23.0f,  4.5f },
        { -28.5f,  5.8f },
        { -34.0f,  7.0f }  // extends to Far-West Artisan Colony!
    };
    drawRoadStrip(shader, model, westPath, 6, 1.40f, footPathCol, 0.008f);

    // Road 6: South Homestead Path
    const RoadNode southPath[] = {
        {  -4.6f, 14.5f },
        {  -7.5f, 16.5f },
        { -11.5f, 17.5f },
        { -15.0f, 17.0f }
    };
    drawRoadStrip(shader, model, southPath, 4, 1.35f, footPathCol, 0.008f);

    // Road 7: Riverside Fisherman Path (West Bank)
    const RoadNode riverPath[] = {
        { 5.2f,   1.2f },
        { 4.2f,  -3.5f },
        { 3.2f,  -7.5f },
        { 1.8f, -11.5f },
        { 1.5f, -16.5f },
        { 2.2f, -24.5f },
        { 1.8f, -32.0f }
    };
    drawRoadStrip(shader, model, riverPath, 7, 1.30f, footPathCol, 0.008f);

    // Road 8: South Riverside Path (West Bank)
    const RoadNode riverSouthPath[] = {
        { 5.2f,  1.2f },
        { 4.5f,  4.8f },
        { 2.8f,  8.0f },
        { 2.2f, 15.5f }
    };
    drawRoadStrip(shader, model, riverSouthPath, 4, 1.25f, footPathCol, 0.008f);

    // Road 9: West-South Connecting Path
    const RoadNode westSouthLink[] = {
        { -21.0f,  6.5f },
        { -19.5f, 10.5f },
        { -16.5f, 14.5f },
        { -13.0f, 16.5f }
    };
    drawRoadStrip(shader, model, westSouthLink, 4, 1.20f, footPathCol, 0.007f);

    // Road 10: North-West Lane (Leading to North-West Farmstead)
    const RoadNode nwLane[] = {
        { -19.5f, -15.5f },
        { -21.0f, -19.5f },
        { -21.5f, -23.5f }
    };
    drawRoadStrip(shader, model, nwLane, 3, 1.25f, footPathCol, 0.007f);

    // Road 11: Southern Agricultural Field Lane (Leading into southern paddy plots)
    const RoadNode southFieldLane[] = {
        { -10.5f, 19.5f },
        { -14.5f, 25.5f },
        { -20.5f, 32.5f }
    };
    drawRoadStrip(shader, model, southFieldLane, 3, 1.30f, footPathCol, 0.007f);

    // Road 12: East Village Spine Road (Purbopara Rasta through the eastern meadow)
    const RoadNode eastSpineRoad[] = {
        { 34.5f, -34.0f },
        { 33.8f, -20.5f },
        { 34.0f,  -2.5f },
        { 34.5f,  12.5f },
        { 32.5f,  24.5f },
        { 34.5f,  36.0f }
    };
    drawRoadStrip(shader, model, eastSpineRoad, 6, 1.50f, footPathCol, 0.008f);

    // ── Road 13: Nearly Straight Grameen Rasta (গ্রামীণ কাঁচা মেঠোপথ / Riverbank Embankment Village Road) ─
    // A classic rural Bangladeshi village road running nearly straight north-to-south along the riverbank embankment.
    const RoadNode grameenRasta[] = {
        { 22.0f, -44.0f },
        { 22.0f, -32.0f },
        { 22.1f, -20.0f },
        { 22.0f,  -8.0f },
        { 22.1f,   6.0f },
        { 22.0f,  20.0f },
        { 22.1f,  32.0f },
        { 22.0f,  44.0f }
    };
    drawGrameenRasta(shader, model, grameenRasta, 8, 2.40f);

    // Connecting Branch 1: Bamboo Bridge Landing to Grameen Rasta
    const RoadNode bridgeLink[] = {
        { 13.5f, -20.5f },
        { 17.5f, -20.5f },
        { 22.0f, -20.5f }
    };
    drawRoadStrip(shader, model, bridgeLink, 3, 1.50f, footPathCol, 0.008f);

    // Connecting Branch 2: Grameen Rasta to Central Purbopara Homestead Courtyard
    const RoadNode midVillageLink[] = {
        { 22.0f, -2.5f },
        { 27.5f, -2.5f },
        { 33.5f, -2.5f }
    };
    drawRoadStrip(shader, model, midVillageLink, 3, 1.45f, footPathCol, 0.008f);

    // Connecting Branch 3: Grameen Rasta to South Purbopara Homestead Courtyard
    const RoadNode southVillageLink[] = {
        { 22.0f, 16.5f },
        { 28.0f, 16.5f },
        { 34.0f, 16.5f }
    };
    drawRoadStrip(shader, model, southVillageLink, 3, 1.40f, footPathCol, 0.008f);

    // Connecting Branch 4: Grameen Rasta to North Purbopara Homestead Courtyard
    const RoadNode northVillageLink[] = {
        { 22.0f, -20.5f },
        { 27.5f, -20.5f },
        { 33.0f, -20.5f }
    };
    drawRoadStrip(shader, model, northVillageLink, 3, 1.40f, footPathCol, 0.008f);

    // Roadside Culvert Concrete Marker Posts (সাদা-লাল আরসিসি পোস্ট) at bridge approach & ditch crossing
    drawCulvertPost(shader, model, vec3(20.6f, 0.0f, -21.6f));
    drawCulvertPost(shader, model, vec3(20.6f, 0.0f, -19.4f));
    drawCulvertPost(shader, model, vec3(23.4f, 0.0f, -21.6f));
    drawCulvertPost(shader, model, vec3(23.4f, 0.0f, -19.4f));

    // ── 4. River Landing Ghat (Wooden / Bamboo Platform & Steps) ─
    drawGhat(shader, model, vec3(5.6f, 0.0f, 1.2f));

    // ── 5. Terraced Paddy Fields (Dhan Khet / ধান ক্ষেত) ─────────
    // Primary large agricultural paddy field
    drawPaddyField(shader, model, vec3(-15.5f, 0.0f, 25.5f), 11.5f, 9.5f);

    // Secondary adjacent terraced paddy plot in the western agricultural expanse
    drawPaddyField(shader, model, vec3(-26.5f, 0.0f, 25.5f), 8.5f, 9.5f);

    // Tertiary deep southern agricultural paddy field
    drawPaddyField(shader, model, vec3(-18.5f, 0.0f, 42.5f), 13.0f, 7.0f);

    // Quaternary eastern agricultural paddy field across the river
    drawPaddyField(shader, model, vec3(39.5f, 0.0f, 24.5f), 10.0f, 9.0f);

    // ── 6. Woven Bamboo Fences (Bansh-er Bera) ───────────────────
    // Fence 1: Back boundary behind North Bari cow shed & house
    drawBambooFence(shader, model, vec3(-28.5f, 0.0f, -14.5f), 7.5f, 12.0f);

    // Fence 2: Separating North farmyard from western meadow (spaced forward to clear House 2 verandah)
    drawBambooFence(shader, model, vec3(-19.2f, 0.0f, -7.5f), 6.0f, 85.0f);

    // Fence 3: Enclosing south side of main courtyard along west path
    drawBambooFence(shader, model, vec3(-14.5f, 0.0f, 5.2f), 8.5f, 0.0f);

    // Fence 4: Garden fence near Dokkhin Bari vegetable trellis
    drawBambooFence(shader, model, vec3(-14.5f, 0.0f, 20.8f), 6.5f, 0.0f);

    // Fence 5: Riverside barrier fence along northern bank
    drawBambooFence(shader, model, vec3(1.2f, 0.0f, -7.5f), 4.5f, 75.0f);

    // Fence 6: Foreground courtyard rustic bamboo fence
    drawBambooFence(shader, model, vec3(-5.8f, 0.0f, 3.8f), 6.2f, 4.0f);
    drawBambooFence(shader, model, vec3( 0.8f, 0.0f, 3.9f), 4.0f, 24.0f);

    // Fence 7: Western homestead garden fence
    drawBambooFence(shader, model, vec3(-29.5f, 0.0f, 4.5f), 7.0f, 90.0f);

    // Fence 8: Southern riverside boundary fence
    drawBambooFence(shader, model, vec3(-6.5f, 0.0f, 10.5f), 5.0f, 15.0f);

    // Fence 9: Far-West Artisan Colony boundary fence
    drawBambooFence(shader, model, vec3(-38.5f, 0.0f, 3.5f), 8.0f, 88.0f);

    // Fence 10: Purbopara Eastern homestead garden fence across the river
    drawBambooFence(shader, model, vec3(36.5f, 0.0f, -5.5f), 6.5f, 85.0f);

    // ── 7. Meadow Grass Tufts Across Full Plane ──────────────────
    drawGrassClump(shader, model, vec3(-4.5f, 0.0f,  5.5f), 1.20f);
    drawGrassClump(shader, model, vec3(-2.8f, 0.0f, -1.5f), 1.10f);
    drawGrassClump(shader, model, vec3(-6.2f, 0.0f, 10.5f), 1.30f);
    drawGrassClump(shader, model, vec3( 2.2f, 0.0f,  3.5f), 1.15f);
    drawGrassClump(shader, model, vec3(-1.5f, 0.0f, -8.5f), 1.25f);
    drawGrassClump(shader, model, vec3(-10.5f, 0.0f, -13.0f), 1.20f);
    drawGrassClump(shader, model, vec3(-32.0f, 0.0f,  8.5f), 1.25f);
    drawGrassClump(shader, model, vec3(-21.0f, 0.0f, -27.5f), 1.15f);
    drawGrassClump(shader, model, vec3( 24.5f, 0.0f,  5.5f), 1.20f);
    drawGrassClump(shader, model, vec3( 21.0f, 0.0f, -15.5f), 1.20f);
}

} // namespace Terrain
