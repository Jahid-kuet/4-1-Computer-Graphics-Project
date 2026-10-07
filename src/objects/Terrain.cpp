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

    static const vec3 postCol  (0.42f, 0.32f, 0.16f); // weathered bamboo upright posts
    static const vec3 nodeCol  (0.28f, 0.20f, 0.10f); // bamboo culm nodes
    static const vec3 railCol  (0.56f, 0.44f, 0.24f); // horizontal split bamboo battens (Batar Batti)
    static const vec3 slatCol1 (0.64f, 0.52f, 0.28f); // vertical split bamboo slats (Chatai / Shola)
    static const vec3 slatCol2 (0.58f, 0.46f, 0.22f); // weathered bamboo slat variant
    static const vec3 lashing  (0.22f, 0.16f, 0.08f); // jute coir tie knots (Pat-er Badhon)

    mat4 fm = rotate(translate(model, startPos), radians(angleDeg), vec3(0.0f, 1.0f, 0.0f));

    const float fenceH = 1.15f;
    const float postSpacing = 0.95f;
    const int numPosts = (int)(length / postSpacing) + 1;
    const float postStep = (numPosts > 1) ? (length / (float)(numPosts - 1)) : 0.0f;

    static const float nodeY[2] = { 0.35f, 0.75f };
    static const float railY[3] = { 0.22f, 0.62f, 1.02f };

    // 1. Upright round bamboo posts with node rings & rail lashings
    for (int p = 0; p < numPosts; p++) {
        float x = (float)p * postStep;
        mat4 post = translate(fm, vec3(x, fenceH * 0.5f, 0.0f));
        post = scale(post, vec3(0.048f, fenceH, 0.048f));
        Primitives::drawCylinder(shader, post, postCol);

        // Nodes along post
        for (int n = 0; n < 2; n++) {
            mat4 node = translate(fm, vec3(x, nodeY[n], 0.0f));
            node = scale(node, vec3(0.055f, 0.025f, 0.055f));
            Primitives::drawCylinder(shader, node, nodeCol);
        }

        // Jute coir lashings at post-rail intersections
        for (int r = 0; r < 3; r++) {
            mat4 lash = translate(fm, vec3(x, railY[r], 0.024f));
            lash = scale(lash, vec3(0.065f, 0.045f, 0.065f));
            Primitives::drawCube(shader, lash, lashing);
        }
    }

    // 2. Three horizontal split-bamboo runner battens (bottom, middle, top)
    for (int r = 0; r < 3; r++) {
        mat4 rail = translate(fm, vec3(length * 0.5f, railY[r], 0.024f));
        rail = scale(rail, vec3(length + 0.10f, 0.035f, 0.025f));
        Primitives::drawCube(shader, rail, railCol);
    }

    // 3. Dense vertical split-bamboo slats (Bansh-er Shola) forming authentic Bengali screen
    const float slatStep = 0.125f;
    const int numSlats = (int)(length / slatStep);
    if (numSlats > 0) {
        const float slatStepActual = length / (float)numSlats;
        static const float hFactors[5] = { 0.92f, 0.96f, 1.00f, 0.94f, 0.98f };
        for (int s = 0; s < numSlats; s++) {
            float sx = ((float)s + 0.5f) * slatStepActual;
            float sh = fenceH * hFactors[s % 5];

            mat4 slat = translate(fm, vec3(sx, sh * 0.5f, -0.010f));
            slat = scale(slat, vec3(0.078f, sh, 0.016f));
            Primitives::drawCube(shader, slat, (s & 1) ? slatCol2 : slatCol1);
        }
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
    static const vec3 soilCol (0.20f, 0.28f, 0.16f); // rich fertile damp alluvial silt
    static const vec3 waterCol(0.18f, 0.32f, 0.24f); // shallow flooded irrigation water sheen
    static const vec3 aalCol  (0.48f, 0.36f, 0.22f); // sun-baked clay boundary ridge (Aal)

    // 1. Muddy field floor (elevated above base terrain to eliminate any Z-fighting or bleed-through)
    mat4 bed = translate(model, vec3(center.x, 0.016f, center.z));
    bed = scale(bed, vec3(width, 1.0f, length));
    Primitives::drawPlane(shader, bed, soilCol);

    // 1b. Glistening shallow water layer over fertile mud
    mat4 water = translate(model, vec3(center.x, 0.018f, center.z));
    water = scale(water, vec3(width * 0.98f, 1.0f, length * 0.98f));
    Primitives::drawPlane(shader, water, waterCol);

    // 2. Earthen boundary dikes (Aal) bordering all 4 sides of the plot
    const float halfW = width * 0.5f;
    const float halfL = length * 0.5f;
    const float aalH  = 0.10f;
    const float aalW  = 0.36f;

    // North & South dikes
    for (int side = -1; side <= 1; side += 2) {
        float fside = (float)side;
        mat4 dikeZ = translate(model, vec3(center.x, aalH * 0.5f, center.z + fside * halfL));
        dikeZ = scale(dikeZ, vec3(width + aalW, aalH, aalW));
        Primitives::drawCube(shader, dikeZ, aalCol);
    }
    // East & West dikes
    for (int side = -1; side <= 1; side += 2) {
        float fside = (float)side;
        mat4 dikeX = translate(model, vec3(center.x + fside * halfW, aalH * 0.5f, center.z));
        dikeX = scale(dikeX, vec3(aalW, aalH, length));
        Primitives::drawCube(shader, dikeX, aalCol);
    }

    // 3. Grid of rice plant clumps (Dhan Gachh)
    const int rows = 6;
    const int cols = 7;
    const float stepX = (width - 1.2f) / (cols - 1);
    const float stepZ = (length - 1.2f) / (rows - 1);

    for (int r = 0; r < rows; r++) {
        float z = (center.z - halfL + 0.6f) + (float)r * stepZ;
        for (int c = 0; c < cols; c++) {
            float x = (center.x - halfW + 0.6f) + (float)c * stepX;
            int hashSeed = r * 11 + c * 7;
            float jx = (float)((hashSeed % 9) - 4) * 0.018f;
            float jz = (float)(((r * 7 + c * 13) % 9) - 4) * 0.018f;
            drawRiceClump(shader, model, vec3(x + jx, 0.020f, z + jz), 0.95f, (float)hashSeed);
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
// Standalone traditional river landing ghat (Nodi-r Ghat / নদীর খেয়া ঘাট)
void drawGhat(Shader& shader, const mat4& model, const vec3& pos)
{
    shader.setInt("uUseTexture", 0); // authentic weathered timber and bamboo

    vec3 woodCol     (0.42f, 0.28f, 0.14f); // seasoned dark timber planks
    vec3 woodDark    (0.24f, 0.15f, 0.07f); // solid subfloor backing & deep joists
    vec3 woodRiser   (0.34f, 0.22f, 0.11f); // vertical riser boards connecting steps
    vec3 postCol     (0.32f, 0.20f, 0.09f); // vertical piling posts
    vec3 bambooCol   (0.54f, 0.44f, 0.22f); // bamboo safety handrail & cross-beams
    vec3 ropeCol     (0.68f, 0.58f, 0.36f); // coir / jute mooring ropes

    mat4 m = model;
    m = translate(m, pos);

    // ── 1. Vertical Timber Pilings (Driven into bank & riverbed) ──────
    float posts[4][2] = {
        { -0.90f, -1.20f }, {  0.90f, -1.20f },
        { -0.90f,  1.20f }, {  0.90f,  1.20f }
    };
    for (int i = 0; i < 4; i++) {
        mat4 p = m;
        p = translate(p, vec3(posts[i][0], 0.35f, posts[i][1]));
        p = scale(p, vec3(0.080f, 1.05f, 0.080f));
        Primitives::drawCylinder(shader, p, postCol);

        // Bamboo node rings
        for (int nd = 0; nd < 2; nd++) {
            mat4 node = p;
            node = translate(node, vec3(0.0f, (float)nd * 0.35f - 0.15f, 0.0f));
            node = scale(node, vec3(1.15f, 0.04f, 1.15f));
            Primitives::drawCylinder(shader, node, postCol * 0.85f);
        }

        // Mooring rope coils around front water posts
        if (posts[i][1] > 0.0f) {
            for (int r = 0; r < 2; r++) {
                mat4 rp = m;
                rp = translate(rp, vec3(posts[i][0], 0.42f + (float)r * 0.05f, posts[i][1]));
                rp = scale(rp, vec3(0.115f, 0.04f, 0.115f));
                Primitives::drawCylinder(shader, rp, ropeCol);
            }
        }
    }

    // Mid-span vertical supporting posts at step transition (X = +/-0.90m, Z = 0.00m)
    for (float px : { -0.90f, 0.90f }) {
        mat4 mp = m;
        mp = translate(mp, vec3(px, 0.28f, 0.00f));
        mp = scale(mp, vec3(0.075f, 0.70f, 0.075f));
        Primitives::drawCylinder(shader, mp, postCol);
    }

    // ── 2. Heavy Longitudinal Stringers & Transverse Cross-Beams ──────
    // Three longitudinal timber joists (Left, Center, Right) under platforms
    for (float jx : { -0.85f, 0.00f, 0.85f }) {
        // Upper deck joist
        mat4 uj = m;
        uj = translate(uj, vec3(jx, 0.11f, -0.60f));
        uj = scale(uj, vec3(0.08f, 0.08f, 1.22f));
        Primitives::drawCube(shader, uj, woodDark);

        // Middle step joist
        mat4 mj = m;
        mj = translate(mj, vec3(jx, 0.05f, 0.32f));
        mj = scale(mj, vec3(0.08f, 0.08f, 0.66f));
        Primitives::drawCube(shader, mj, woodDark);

        // Lower step joist
        mat4 lj = m;
        lj = translate(lj, vec3(jx * 0.90f, -0.02f, 0.92f));
        lj = scale(lj, vec3(0.08f, 0.08f, 0.58f));
        Primitives::drawCube(shader, lj, woodDark);
    }

    // Bamboo horizontal cross-support tie beams
    mat4 bBeam1 = m;
    bBeam1 = translate(bBeam1, vec3(0.0f, 0.10f, -0.60f));
    bBeam1 = rotate(bBeam1, radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
    bBeam1 = scale(bBeam1, vec3(0.045f, 2.10f, 0.045f));
    Primitives::drawCylinder(shader, bBeam1, bambooCol);

    mat4 bBeam2 = m;
    bBeam2 = translate(bBeam2, vec3(0.0f, 0.02f, 0.80f));
    bBeam2 = rotate(bBeam2, radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
    bBeam2 = scale(bBeam2, vec3(0.045f, 2.00f, 0.045f));
    Primitives::drawCylinder(shader, bBeam2, bambooCol);

    // ── 3. Solid Undercarriage Subfloor Slabs (Guarantees 100% Zero Gaps) ──
    // Solid timber subfloor backing slabs directly beneath the planks:
    // Upper subfloor (Z: -1.22m -> 0.00m)
    mat4 subUpper = m;
    subUpper = translate(subUpper, vec3(0.0f, 0.145f, -0.61f));
    subUpper = scale(subUpper, vec3(2.18f, 0.025f, 1.22f));
    Primitives::drawCube(shader, subUpper, woodDark);

    // Middle subfloor (Z: 0.00m -> 0.64m)
    mat4 subMid = m;
    subMid = translate(subMid, vec3(0.0f, 0.085f, 0.32f));
    subMid = scale(subMid, vec3(1.98f, 0.025f, 0.64f));
    Primitives::drawCube(shader, subMid, woodDark);

    // Lower subfloor (Z: 0.64m -> 1.20m)
    mat4 subLower = m;
    subLower = translate(subLower, vec3(0.0f, 0.015f, 0.92f));
    subLower = scale(subLower, vec3(1.82f, 0.025f, 0.56f));
    Primitives::drawCube(shader, subLower, woodDark);

    // ── 4. Flush Edge-to-Edge Planks & Vertical Step Risers ──────────
    // A) Upper Bank Platform: 7 continuous flush planks (Z: -1.21m -> 0.00m)
    const int numUpperPlanks = 7;
    const float upperPitch = 1.21f / (float)numUpperPlanks; // ~0.1728m
    for (int pl = 0; pl < numUpperPlanks; pl++) {
        float pz = -1.21f + upperPitch * 0.5f + (float)pl * upperPitch;
        mat4 plank = m;
        plank = translate(plank, vec3(0.0f, 0.18f, pz));
        plank = scale(plank, vec3(2.20f, 0.055f, upperPitch + 0.004f)); // flush edge-to-edge
        vec3 col = (pl % 3 == 0) ? woodCol : ((pl % 3 == 1) ? (woodCol * 0.94f) : (woodCol * 1.04f));
        Primitives::drawCube(shader, plank, col);
    }

    // Vertical Riser Board 1: Seals upper deck step down to middle landing (at Z = 0.00m)
    mat4 riser1 = m;
    riser1 = translate(riser1, vec3(0.0f, 0.145f, 0.00f));
    riser1 = scale(riser1, vec3(2.18f, 0.090f, 0.045f));
    Primitives::drawCube(shader, riser1, woodRiser);

    // B) Middle Landing Step: 4 continuous flush planks (Z: 0.00m -> 0.64m)
    const int numMidPlanks = 4;
    const float midPitch = 0.64f / (float)numMidPlanks; // 0.160m
    for (int pl = 0; pl < numMidPlanks; pl++) {
        float pz = 0.00f + midPitch * 0.5f + (float)pl * midPitch;
        mat4 plank = m;
        plank = translate(plank, vec3(0.0f, 0.12f, pz));
        plank = scale(plank, vec3(2.00f, 0.050f, midPitch + 0.004f)); // flush edge-to-edge
        vec3 col = (pl % 2 == 0) ? woodCol : (woodCol * 0.95f);
        Primitives::drawCube(shader, plank, col);
    }

    // Vertical Riser Board 2: Seals middle step down to lower water step (at Z = 0.64m)
    mat4 riser2 = m;
    riser2 = translate(riser2, vec3(0.0f, 0.080f, 0.64f));
    riser2 = scale(riser2, vec3(1.98f, 0.085f, 0.045f));
    Primitives::drawCube(shader, riser2, woodRiser * 0.90f);

    // C) Lower Water Step: 3 continuous flush planks (Z: 0.64m -> 1.20m)
    const int numLowerPlanks = 3;
    const float lowerPitch = 0.56f / (float)numLowerPlanks; // ~0.1867m
    for (int pl = 0; pl < numLowerPlanks; pl++) {
        float pz = 0.64f + lowerPitch * 0.5f + (float)pl * lowerPitch;
        mat4 plank = m;
        plank = translate(plank, vec3(0.0f, 0.05f, pz));
        plank = scale(plank, vec3(1.85f, 0.045f, lowerPitch + 0.004f)); // flush edge-to-edge
        vec3 col = (pl % 2 == 0) ? (woodCol * 0.85f) : (woodCol * 0.78f);
        Primitives::drawCube(shader, plank, col);
    }

    // ── 5. Bamboo Safety Handrail on Right Side (+X) ─────────────────
    // Handrail vertical baluster posts at Z = -1.20m, 0.00m, +1.20m
    float railPostsZ[3] = { -1.20f, 0.00f, 1.20f };
    float railPostsH[3] = {  0.78f, 0.72f, 0.65f };
    float railPostsY[3] = {  0.48f, 0.42f, 0.35f };
    for (int rp = 0; rp < 3; rp++) {
        mat4 rPost = m;
        rPost = translate(rPost, vec3(0.90f, railPostsY[rp], railPostsZ[rp]));
        rPost = scale(rPost, vec3(0.035f, railPostsH[rp], 0.035f));
        Primitives::drawCylinder(shader, rPost, bambooCol);
    }

    // Continuous descending handrail pole running across the baluster tops
    mat4 rail = m;
    rail = translate(rail, vec3(0.90f, 0.76f, 0.00f));
    rail = rotate(rail, radians(-6.0f), vec3(1.0f, 0.0f, 0.0f)); // descending slope
    rail = rotate(rail, radians(90.0f), vec3(1.0f, 0.0f, 0.0f));  // orient along Z
    rail = scale(rail, vec3(0.038f, 2.50f, 0.038f));
    Primitives::drawCylinder(shader, rail, bambooCol * 1.05f);

    // ── 6. Terracotta Pitcher (Kolshi) & Hanging Lantern ─────────────
    // Terracotta water pitcher resting securely on middle step
    mat4 kolshiM = m;
    kolshiM = translate(kolshiM, vec3(0.55f, 0.145f, 0.32f));
    kolshiM = scale(kolshiM, vec3(0.72f));
    House::drawKolshi(shader, kolshiM);

    // Hanging Hurricane Lantern (Hariken) mounted on rear timber piling post
    mat4 ghatLantern = m;
    ghatLantern = translate(ghatLantern, vec3(-0.90f, 0.68f, -1.20f));
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

    // I. South Agricultural Hamlet Courtyard (Dokkhin-Para Krishi Bari - Relocated to River Side)
    mat4 uthanSouthDeep = model;
    uthanSouthDeep = translate(uthanSouthDeep, vec3(-6.5f, 0.005f, 36.5f));
    uthanSouthDeep = scale(uthanSouthDeep, vec3(9.0f, 1.0f, 8.0f));
    Primitives::drawPlane(shader, uthanSouthDeep, uthanEarth);

    // J. North Riverside Hamlet Courtyard (House 6 North Riverside Cottage)
    mat4 uthanNorthRiver = model;
    uthanNorthRiver = translate(uthanNorthRiver, vec3(-8.5f, 0.005f, -34.0f));
    uthanNorthRiver = scale(uthanNorthRiver, vec3(11.0f, 1.0f, 12.0f));
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

    // Road 11: Southern Agricultural Field Lane (Leading along eastern terrace into southern plots)
    const RoadNode southFieldLane[] = {
        { -7.0f, 19.5f },
        { -7.0f, 25.5f },
        { -6.5f, 34.0f }
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

    // ── 5. Terraced Paddy Fields (Dhan Khet / ধান ক্ষেত - Non-overlapping with clear distance) ─
    // Plot 1: North-West agricultural paddy plot (at South-West corner sector)
    drawPaddyField(shader, model, vec3(-37.5f, 0.0f, 28.0f), 10.0f, 8.0f);

    // Plot 2: South-West agricultural paddy plot (spaced 3.5m south of Plot 1)
    drawPaddyField(shader, model, vec3(-37.5f, 0.0f, 39.5f), 10.0f, 8.0f);

    // Plot 3: South-East agricultural paddy plot (spaced 2.0m east of Plot 2)
    drawPaddyField(shader, model, vec3(-25.5f, 0.0f, 39.5f), 10.0f, 8.0f);

    // Quaternary eastern agricultural paddy field at far south-east corner of plane across river
    drawPaddyField(shader, model, vec3(39.5f, 0.0f, 38.5f), 10.0f, 9.0f);

    // ── 6. Woven Bamboo Fences (Bansh-er Bera / বাঁশের বেড়া) ───────────────────
    // Appropriately placed as homestead boundary fences between neighboring houses:

    // Fence 1: Between House 1 (Elder Bari) & House 1B (Kitchen Cottage) in Moddho Bari
    drawBambooFence(shader, model, vec3(-11.5f, 0.0f, -6.5f), 4.5f, 5.0f);

    // Fence 2: Between House 1B (Moddho Bari) & House 2 (Uttar Bari Farmstead)
    drawBambooFence(shader, model, vec3(-19.0f, 0.0f, -6.8f), 5.4f, 90.0f);

    // Fence 3: Between House 2 (Uttar Bari Farmstead) & House 6 (North Riverside Cottage)
    drawBambooFence(shader, model, vec3(-16.0f, 0.0f, -26.0f), 5.5f, 0.0f);

    // Fence 4: Between House 3 (Dokkhin Bari Main) & House 3B (Field Worker Cottage)
    drawBambooFence(shader, model, vec3(-12.0f, 0.0f, 21.2f), 5.2f, 12.0f);

    // Fence 5: Between House 1 (Moddho Bari) & House 3 (Dokkhin Bari)
    drawBambooFence(shader, model, vec3(-13.5f, 0.0f, 6.0f), 6.0f, 0.0f);

    // Fence 6: Between House 2 (Uttar Bari) & House 4 (Poshchim Bari)
    drawBambooFence(shader, model, vec3(-28.0f, 0.0f, -4.5f), 6.0f, 0.0f);

    // Fence 7: Between House 4 (Poshchim Bari) & House 7 (Tanti Para Weaver Cottage)
    drawBambooFence(shader, model, vec3(-32.5f, 0.0f, 4.2f), 5.5f, 90.0f);

    // Fence 8: Between House 1B (Moddho Bari) & House 5 (Riverside Fisherman Cottage)
    drawBambooFence(shader, model, vec3(-13.0f, 0.0f, -16.8f), 5.0f, 20.0f);

    // Fence 9: Boundary fence (Bera) along the eastern perimeter of the Mosque sanctuary
    drawBambooFence(shader, model, vec3(-24.5f, 0.0f, -25.5f), 16.5f, 90.0f);

    // Fence 9B: Southern boundary fence (Bera) along the Mosque courtyard front
    drawBambooFence(shader, model, vec3(-32.5f, 0.0f, -25.5f), 8.0f, 0.0f);

    // Fence 10: Between House E1 (Central Purbopara) & House E3 (South Purbopara) across the river
    drawBambooFence(shader, model, vec3(25.5f, 0.0f, 7.0f), 7.0f, 0.0f);

    // Fence 11: Between House E1 (Central Purbopara) & House E2 (North Purbopara) across the river
    drawBambooFence(shader, model, vec3(25.5f, 0.0f, -13.0f), 7.0f, 0.0f);

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
