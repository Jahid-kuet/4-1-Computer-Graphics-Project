// Terrain.cpp — Rich Bangladeshi village environment (Gramin Poribesh)
// Features lush green meadow, beaten-earth village courtyard (Uthan),
// winding dirt footpaths (Poth) leading to the river ghat, traditional
// woven bamboo fencing (Bansh-er Bera), rolling background mounds, and grass tufts.

#include "objects/Terrain.h"
#include "Primitives.h"
#include <cmath>

using namespace math;

namespace {

// Helper: Draw a section of traditional rural woven bamboo fence
void drawBambooFence(Shader& shader, const mat4& model, const vec3& startPos, float length, float angleDeg)
{
    vec3 postCol (0.38f, 0.28f, 0.14f); // bamboo upright posts
    vec3 railCol (0.48f, 0.38f, 0.20f); // horizontal bamboo split rails
    vec3 picketCol(0.44f, 0.34f, 0.18f); // diagonal slats

    mat4 fm = model;
    fm = translate(fm, startPos);
    fm = rotate(fm, radians(angleDeg), vec3(0.0f, 1.0f, 0.0f));

    float fenceH = 1.10f;
    int numPosts = (int)(length / 1.0f) + 1;

    // Upright posts
    for (int p = 0; p < numPosts; p++) {
        float x = (float)p * 1.0f;
        mat4 post = fm;
        post = translate(post, vec3(x, fenceH * 0.5f, 0.0f));
        post = scale(post, vec3(0.045f, fenceH, 0.045f));
        Primitives::drawCylinder(shader, post, postCol);
    }

    // Two horizontal bamboo tie-rails
    for (int r = 0; r < 2; r++) {
        float y = (r == 0) ? 0.35f : 0.85f;
        mat4 rail = fm;
        rail = translate(rail, vec3(length * 0.5f, y, 0.0f));
        rail = scale(rail, vec3(length, 0.035f, 0.035f));
        Primitives::drawCube(shader, rail, railCol);
    }

    // Crossed bamboo diagonals between posts
    int numBays = numPosts - 1;
    for (int b = 0; b < numBays; b++) {
        float bx = (float)b * 1.0f + 0.5f;

        // Positive diagonal
        mat4 d1 = fm;
        d1 = translate(d1, vec3(bx, 0.60f, 0.01f));
        d1 = rotate(d1, radians(32.0f), vec3(0.0f, 0.0f, 1.0f));
        d1 = scale(d1, vec3(1.10f, 0.025f, 0.015f));
        Primitives::drawCube(shader, d1, picketCol);

        // Negative diagonal
        mat4 d2 = fm;
        d2 = translate(d2, vec3(bx, 0.60f, -0.01f));
        d2 = rotate(d2, radians(-32.0f), vec3(0.0f, 0.0f, 1.0f));
        d2 = scale(d2, vec3(1.10f, 0.025f, 0.015f));
        Primitives::drawCube(shader, d2, picketCol);
    }
}

// Helper: Draw a clump of 3D meadow grass
void drawGrassClump(Shader& shader, const mat4& model, const vec3& pos, float scaleVal = 1.0f)
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
// Features lush green paddy tillers with drooping golden grain panicles (Dhaner Shish / ধানের শীষ)
void drawRiceClump(Shader& shader, const mat4& model, const vec3& pos, float scaleVal = 1.0f, float seed = 0.0f)
{
    vec3 stalkCol(0.30f, 0.56f, 0.16f); // lush paddy green stalk
    vec3 leafCol (0.26f, 0.50f, 0.14f); // long arching green blade
    vec3 grainCol(0.88f, 0.78f, 0.28f); // golden ripened grain panicle (Dhaner Shish)

    // A clump (Gochha) consists of 5 outward radiating stalks with drooping grain heads
    for (int i = 0; i < 5; i++) {
        float fi = (float)i;
        float baseAngle = fi * (2.0f * PI / 5.0f) + seed * 0.8f;
        float tiltAngle = 14.0f + sinf(seed + fi * 1.5f) * 4.0f;
        float h = (0.65f + sinf(seed * 2.0f + fi) * 0.08f) * scaleVal;

        mat4 stalk = model;
        stalk = translate(stalk, pos);
        stalk = rotate(stalk, baseAngle, vec3(0.0f, 1.0f, 0.0f));
        stalk = rotate(stalk, radians(tiltAngle), vec3(1.0f, 0.0f, 0.0f));

        // Lower green stalk
        mat4 sM = stalk;
        sM = translate(sM, vec3(0.0f, h * 0.40f, 0.0f));
        sM = scale(sM, vec3(0.015f * scaleVal, h * 0.80f, 0.015f * scaleVal));
        Primitives::drawCylinder(shader, sM, stalkCol);

        // Arching rice leaf blade
        mat4 leaf = stalk;
        leaf = translate(leaf, vec3(0.0f, h * 0.45f, 0.05f * scaleVal));
        leaf = rotate(leaf, radians(35.0f), vec3(1.0f, 0.0f, 0.0f));
        leaf = scale(leaf, vec3(0.030f * scaleVal, h * 0.65f, 0.008f));
        Primitives::drawCone(shader, leaf, leafCol);

        // Drooping golden grain panicle (Dhaner Shish) bending gracefully downward
        mat4 shish = stalk;
        shish = translate(shish, vec3(0.0f, h * 0.80f, 0.0f));
        shish = rotate(shish, radians(52.0f), vec3(1.0f, 0.0f, 0.0f)); // droops under weight of grain!
        shish = translate(shish, vec3(0.0f, 0.10f * scaleVal, 0.0f));
        shish = scale(shish, vec3(0.022f * scaleVal, 0.22f * scaleVal, 0.022f * scaleVal));
        Primitives::drawCone(shader, shish, grainCol);

        // Golden grain tip bead
        mat4 grainTip = shish;
        grainTip = translate(grainTip, vec3(0.0f, 0.11f, 0.0f));
        grainTip = scale(grainTip, vec3(1.1f, 0.15f, 1.1f));
        Primitives::drawSphere(shader, grainTip, grainCol);
    }
}

// Helper: Draw a terraced rural Bangladeshi Paddy Field (Dhan Khet / ধান ক্ষেত)
// Enclosed with raised earthen boundary dikes (Aal / আইল) and planted with neat rows of rice clumps
void drawPaddyField(Shader& shader, const mat4& model, const vec3& center, float width, float length)
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
    float aalH  = 0.06f;
    float aalW  = 0.32f;

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
            drawRiceClump(shader, model, vec3(x + jx, 0.01f, z + jz), 0.92f, seed);
        }
    }
}

} // anonymous namespace

namespace Terrain {

void draw(Shader& shader, const mat4& model)
{
    // ── Palette ──────────────────────────────────────────────────
    vec3 grassGreen  (0.30f, 0.48f, 0.20f); // lush Bengal green
    vec3 darkGreen   (0.22f, 0.38f, 0.14f); // deeper vegetation green
    vec3 uthanEarth  (0.60f, 0.48f, 0.32f); // beaten clay courtyard
    vec3 uthanBorder (0.50f, 0.40f, 0.26f); // softer trampled border
    vec3 pathColor   (0.55f, 0.44f, 0.28f); // earthen footpath
    vec3 moundGreen  (0.20f, 0.35f, 0.15f); // distant backdrop hills

    // ── 1. Base Meadow Terrain ───────────────────────────────────
    mat4 basePlane = model;
    basePlane = scale(basePlane, vec3(75.0f, 1.0f, 75.0f));
    Primitives::drawPlane(shader, basePlane, grassGreen);

    // ── 2. Beaten-Earth Courtyard (Uthan) ────────────────────────
    // Expansive, organic swept-clay courtyard connecting houses, kitchen, and gathering
    mat4 uthan1 = model;
    uthan1 = translate(uthan1, vec3(-6.5f, 0.005f, -4.5f));
    uthan1 = scale(uthan1, vec3(17.0f, 1.0f, 18.0f));
    Primitives::drawPlane(shader, uthan1, uthanEarth);

    // Front yard extension (gathering area around charpai)
    mat4 uthan2 = model;
    uthan2 = translate(uthan2, vec3(-3.0f, 0.006f, 1.5f));
    uthan2 = scale(uthan2, vec3(10.0f, 1.0f, 9.0f));
    Primitives::drawPlane(shader, uthan2, uthanEarth);

    // Soft border apron around the yard
    mat4 uthan3 = model;
    uthan3 = translate(uthan3, vec3(-6.5f, 0.004f, -4.5f));
    uthan3 = scale(uthan3, vec3(19.5f, 1.0f, 20.5f));
    Primitives::drawPlane(shader, uthan3, uthanBorder);

    // ── 3. Winding Village Footpath (Poth) to River Shore ─────────
    // Smooth, gentle curving path from courtyard edge to the boat mooring bank
    struct PathNode { float x, z, w; };
    PathNode path[] = {
        { -1.0f,  0.8f, 1.6f },
        {  0.8f,  0.9f, 1.5f },
        {  2.4f,  1.1f, 1.4f },
        {  3.8f,  1.2f, 1.4f },
        {  5.4f,  1.2f, 1.5f }  // leads to sandy riverbank by the moored boat
    };
    for (int p = 0; p < 4; p++) {
        float mx = (path[p].x + path[p+1].x) * 0.5f;
        float mz = (path[p].z + path[p+1].z) * 0.5f;
        float dx = path[p+1].x - path[p].x;
        float dz = path[p+1].z - path[p].z;
        float len = sqrtf(dx * dx + dz * dz);
        float ang = atan2f(dx, dz);

        mat4 seg = model;
        seg = translate(seg, vec3(mx, 0.008f, mz));
        seg = rotate(seg, ang, vec3(0.0f, 1.0f, 0.0f));
        seg = scale(seg, vec3(path[p].w, 1.0f, len * 1.12f));
        Primitives::drawPlane(shader, seg, pathColor);
    }



    // ── 5. Rural Bangladeshi Rice Tree (Dhan Gachh / ধান গাছ) ─────
    // Exactly 1 representative rice tree clump with golden grain panicles (Dhaner Shish)
    {
        vec3 center(-8.5f, 0.0f, 8.0f);
        vec3 soilCol(0.24f, 0.32f, 0.16f);
        mat4 bed = model;
        bed = translate(bed, vec3(center.x, 0.006f, center.z));
        bed = scale(bed, vec3(3.2f, 1.0f, 3.2f));
        Primitives::drawPlane(shader, bed, soilCol);

        drawRiceClump(shader, model, vec3(center.x, 0.01f, center.z), 1.35f, 1.0f);
    }

    // ── 6. Rural Meadow Grass (1 Single Clump) ────────────────────
    drawGrassClump(shader, model, vec3(-4.5f, 0.0f, 5.5f), 1.30f);
}

} // namespace Terrain
