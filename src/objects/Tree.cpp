// Tree.cpp — Authentic Bangladeshi trees (Narikel, Kola, Bot/Aam, Bansh)
// Redesigned with organic cascading palm fronds, graceful arching banana paddles,
// lush natural foliage clusters, and dense bamboo groves.

#include "objects/Tree.h"
#include "Primitives.h"
#include <cmath>

using namespace math;

namespace Tree {

// ─── 1. Traditional Coconut Palm (Narikel Gach) ──────────────────────
static void drawPalm(Shader& shader, const mat4& model)
{
    vec3 trunkCol (0.42f, 0.28f, 0.14f); // ringed coconut bark
    vec3 ringCol  (0.28f, 0.18f, 0.08f); // dark annular leaf scars
    vec3 frondCol (0.10f, 0.36f, 0.08f); // deep tropical green fronds
    vec3 leafTip  (0.14f, 0.44f, 0.10f); // lush green leaflets
    vec3 nutCol   (0.32f, 0.44f, 0.16f); // green coconut cluster

    // Curved, slender leaning trunk (7 segmented sections)
    const int segments = 8;
    float segH = 0.75f;

    mat4 currT = model;
    for (int i = 0; i < segments; i++) {
        float frac = (float)i / segments;
        float radius = 0.16f - frac * 0.05f;

        // Graceful natural lean
        currT = translate(currT, vec3(0.025f, segH * 0.5f, 0.015f));
        currT = rotate(currT, radians(1.8f), vec3(0.0f, 0.0f, 1.0f));

        mat4 tM = scale(currT, vec3(radius, segH, radius));
        Primitives::drawCylinder(shader, tM, trunkCol);

        // Annular scar ring
        mat4 rM = scale(currT, vec3(radius * 1.15f, 0.035f, radius * 1.15f));
        Primitives::drawCylinder(shader, rM, ringCol);

        currT = translate(currT, vec3(0.0f, segH * 0.5f, 0.0f));
    }

    mat4 crown = currT;

    // Cluster of green coconuts nestled at apex
    for (int n = 0; n < 6; n++) {
        float nAngle = (float)n * (2.0f * PI / 6.0f);
        mat4 nut = crown;
        nut = translate(nut, vec3(cosf(nAngle) * 0.20f, -0.12f, sinf(nAngle) * 0.20f));
        nut = scale(nut, vec3(0.13f, 0.16f, 0.13f));
        Primitives::drawSphere(shader, nut, nutCol);
    }

    // 10 Cascading downward-arching palm fronds
    // Built from 3 sequentially bending segments with central rachis and wide leaflet fans
    const int numFronds = 10;
    for (int f = 0; f < numFronds; f++) {
        float fAngle = (float)f * (360.0f / numFronds) + (f % 2 == 0 ? 0.0f : 18.0f);
        float droopBase = 30.0f + (f % 3) * 6.0f;

        mat4 frond = crown;
        frond = rotate(frond, radians(fAngle), vec3(0.0f, 1.0f, 0.0f));

        // Segment 1: Rises slightly and arches outward (tilt ~ 30-36°)
        mat4 s1 = frond;
        s1 = rotate(s1, radians(droopBase), vec3(1.0f, 0.0f, 0.0f));
        s1 = translate(s1, vec3(0.0f, 0.65f, 0.0f));

        // Central spine (rachis)
        mat4 rib1 = scale(s1, vec3(0.035f, 1.30f, 0.035f));
        Primitives::drawCylinder(shader, rib1, ringCol);

        // Leaflet blades
        mat4 b1 = scale(s1, vec3(0.46f, 1.22f, 0.018f));
        Primitives::drawCube(shader, b1, frondCol);

        // Segment 2: Arches further outward and down (tilt + 32°)
        mat4 s2 = s1;
        s2 = translate(s2, vec3(0.0f, 0.60f, 0.0f));
        s2 = rotate(s2, radians(32.0f), vec3(1.0f, 0.0f, 0.0f));
        s2 = translate(s2, vec3(0.0f, 0.60f, 0.0f));

        mat4 rib2 = scale(s2, vec3(0.028f, 1.20f, 0.028f));
        Primitives::drawCylinder(shader, rib2, ringCol);

        mat4 b2 = scale(s2, vec3(0.36f, 1.15f, 0.016f));
        Primitives::drawCube(shader, b2, frondCol);

        // Segment 3: Cascades steeply downward (tilt + 38°)
        mat4 s3 = s2;
        s3 = translate(s3, vec3(0.0f, 0.55f, 0.0f));
        s3 = rotate(s3, radians(38.0f), vec3(1.0f, 0.0f, 0.0f));
        s3 = translate(s3, vec3(0.0f, 0.45f, 0.0f));

        mat4 b3 = scale(s3, vec3(0.24f, 0.90f, 0.014f));
        Primitives::drawCube(shader, b3, leafTip);
    }
}

// ─── 2. Banana Tree (Kola Gach) ──────────────────────────────────────
static void drawBanana(Shader& shader, const mat4& model)
{
    vec3 trunkCol (0.34f, 0.45f, 0.16f); // succulent green pseudostem
    vec3 leafCol  (0.12f, 0.40f, 0.10f); // deep lush banana leaf
    vec3 leafTip  (0.16f, 0.46f, 0.12f); // lighter leaf edge
    vec3 stemCol  (0.26f, 0.40f, 0.14f); // flower stalk
    vec3 mochaCol (0.42f, 0.08f, 0.12f); // purple-red banana heart (Mocha)
    vec3 fruitCol (0.35f, 0.50f, 0.15f); // green baby bananas

    // Succulent pseudostem trunk
    float trunkH = 2.4f;
    mat4 trunk = model;
    trunk = translate(trunk, vec3(0.0f, trunkH * 0.5f, 0.0f));
    trunk = scale(trunk, vec3(0.18f, trunkH, 0.18f));
    Primitives::drawCylinder(shader, trunk, trunkCol);

    mat4 crown = model;
    crown = translate(crown, vec3(0.0f, trunkH, 0.0f));

    // 7 Broad arching paddle leaves cascading outward
    const int numLeaves = 7;
    for (int i = 0; i < numLeaves; i++) {
        float angle = (float)i * (360.0f / numLeaves) + 15.0f;
        mat4 leaf = crown;
        leaf = rotate(leaf, radians(angle), vec3(0.0f, 1.0f, 0.0f));

        // Part 1: Stalk rising up and out (30° tilt)
        mat4 p1 = leaf;
        p1 = rotate(p1, radians(28.0f), vec3(1.0f, 0.0f, 0.0f));
        p1 = translate(p1, vec3(0.0f, 0.70f, 0.0f));
        mat4 b1 = scale(p1, vec3(0.45f, 1.30f, 0.02f));
        Primitives::drawCube(shader, b1, leafCol);

        // Part 2: Broad paddle arching over (40° tilt)
        mat4 p2 = p1;
        p2 = translate(p2, vec3(0.0f, 0.65f, 0.0f));
        p2 = rotate(p2, radians(42.0f), vec3(1.0f, 0.0f, 0.0f));
        p2 = translate(p2, vec3(0.0f, 0.60f, 0.0f));
        mat4 b2 = scale(p2, vec3(0.52f, 1.20f, 0.02f));
        Primitives::drawCube(shader, b2, leafCol);

        // Part 3: Drooping tip (45° tilt downward)
        mat4 p3 = p2;
        p3 = translate(p3, vec3(0.0f, 0.60f, 0.0f));
        p3 = rotate(p3, radians(48.0f), vec3(1.0f, 0.0f, 0.0f));
        p3 = translate(p3, vec3(0.0f, 0.50f, 0.0f));
        mat4 b3 = scale(p3, vec3(0.36f, 0.95f, 0.018f));
        Primitives::drawCube(shader, b3, leafTip);
    }

    // Hanging fruit stem
    mat4 stem = crown;
    stem = translate(stem, vec3(0.18f, -0.45f, 0.0f));
    stem = scale(stem, vec3(0.04f, 0.90f, 0.04f));
    Primitives::drawCylinder(shader, stem, stemCol);

    // Baby banana bunch (green fingers)
    for (int b = 0; b < 5; b++) {
        float ba = (float)b * (2.0f * PI / 5.0f);
        mat4 banana = crown;
        banana = translate(banana, vec3(0.18f + cosf(ba) * 0.10f, -0.30f, sinf(ba) * 0.10f));
        banana = rotate(banana, radians(25.0f), vec3(0.0f, 0.0f, 1.0f));
        banana = scale(banana, vec3(0.035f, 0.18f, 0.035f));
        Primitives::drawCylinder(shader, banana, fruitCol);
    }

    // Banana flower blossom (Mocha) hanging at the tip
    mat4 mocha = crown;
    mocha = translate(mocha, vec3(0.18f, -0.92f, 0.0f));
    mocha = scale(mocha, vec3(0.13f, 0.28f, 0.13f));
    Primitives::drawSphere(shader, mocha, mochaCol);
}

// ─── 3. Majestic Branching Mango / Banyan Tree (Aam / Bot Gach) ──────
static void drawGeneral(Shader& shader, const mat4& model)
{
    vec3 trunkCol (0.34f, 0.24f, 0.14f); // gnarled bark
    vec3 foliage1 (0.08f, 0.30f, 0.08f); // deep forest green
    vec3 foliage2 (0.12f, 0.38f, 0.10f); // medium lush green
    vec3 foliage3 (0.06f, 0.24f, 0.06f); // dark shadowed foliage

    // Sturdy gnarled trunk
    float trunkH = 2.8f;
    mat4 trunk = model;
    trunk = translate(trunk, vec3(0.0f, trunkH * 0.5f, 0.0f));
    trunk = scale(trunk, vec3(0.40f, trunkH, 0.40f));
    Primitives::drawCylinder(shader, trunk, trunkCol);

    // Buttress root flares at base
    for (int r = 0; r < 4; r++) {
        float ra = (float)r * 90.0f + 25.0f;
        mat4 root = model;
        root = rotate(root, radians(ra), vec3(0.0f, 1.0f, 0.0f));
        root = translate(root, vec3(0.35f, 0.30f, 0.0f));
        root = rotate(root, radians(35.0f), vec3(0.0f, 0.0f, 1.0f));
        root = scale(root, vec3(0.15f, 0.8f, 0.12f));
        Primitives::drawCube(shader, root, trunkCol);
    }

    // 4 Branching limbs radiating outward
    float branchAngles[] = { 15.0f, 105.0f, 195.0f, 285.0f };
    for (int b = 0; b < 4; b++) {
        mat4 branch = model;
        branch = translate(branch, vec3(0.0f, trunkH * 0.75f, 0.0f));
        branch = rotate(branch, radians(branchAngles[b]), vec3(0.0f, 1.0f, 0.0f));
        branch = rotate(branch, radians(38.0f), vec3(1.0f, 0.0f, 0.0f));
        branch = translate(branch, vec3(0.0f, 1.1f, 0.0f));
        mat4 bM = scale(branch, vec3(0.16f, 2.2f, 0.16f));
        Primitives::drawCylinder(shader, bM, trunkCol);
    }

    // Natural tiered canopy (varied overlapping spheres)
    struct FoliageCluster {
        float x, y, z;
        float sx, sy, sz;
        vec3 col;
    };

    FoliageCluster clusters[] = {
        {  0.0f, 4.6f,  0.0f, 2.6f, 2.0f, 2.6f, foliage1 },
        {  0.3f, 5.2f, -0.2f, 1.9f, 1.5f, 1.9f, foliage2 },
        { -1.6f, 4.0f,  0.4f, 1.9f, 1.5f, 1.8f, foliage2 },
        {  1.7f, 4.1f, -0.3f, 2.0f, 1.6f, 1.9f, foliage1 },
        {  0.2f, 3.8f,  1.7f, 1.8f, 1.4f, 1.8f, foliage3 },
        { -0.3f, 3.9f, -1.7f, 1.9f, 1.5f, 1.9f, foliage2 },
        { -1.3f, 3.2f, -1.1f, 1.4f, 1.1f, 1.4f, foliage3 },
        {  1.2f, 3.3f,  1.2f, 1.5f, 1.2f, 1.5f, foliage1 }
    };

    for (const auto& fc : clusters) {
        mat4 m = model;
        m = translate(m, vec3(fc.x, fc.y, fc.z));
        m = scale(m, vec3(fc.sx, fc.sy, fc.sz));
        Primitives::drawSphere(shader, m, fc.col);
    }
}

// ─── 4. Dense Bamboo Grove (Bansher Jhar) ────────────────────────────
static void drawBamboo(Shader& shader, const mat4& model)
{
    vec3 stalkCol(0.28f, 0.44f, 0.16f); // yellowish-green bamboo stalk
    vec3 nodeCol (0.20f, 0.32f, 0.10f); // dark bamboo culm node/joint
    vec3 leafCol (0.14f, 0.46f, 0.12f); // feathery bamboo leaves

    // Clump of 8 tall segmented stalks arching gracefully
    struct BambooStalk {
        float ox, oz;
        float h;
        float leanX, leanZ;
    };

    BambooStalk stalks[] = {
        {  0.00f,  0.00f, 5.2f,  2.0f, -2.0f },
        { -0.35f,  0.25f, 4.8f, -4.0f,  3.0f },
        {  0.40f, -0.20f, 5.5f,  5.0f, -3.0f },
        { -0.25f, -0.40f, 4.4f, -3.0f, -5.0f },
        {  0.30f,  0.40f, 5.0f,  4.0f,  4.0f },
        { -0.50f, -0.15f, 4.2f, -6.0f, -1.0f },
        {  0.50f,  0.15f, 4.6f,  6.0f,  2.0f },
        {  0.10f, -0.50f, 4.9f, -1.0f, -4.0f }
    };

    for (const auto& bs : stalks) {
        mat4 sm = model;
        sm = translate(sm, vec3(bs.ox, 0.0f, bs.oz));
        sm = rotate(sm, radians(bs.leanX), vec3(1.0f, 0.0f, 0.0f));
        sm = rotate(sm, radians(bs.leanZ), vec3(0.0f, 0.0f, 1.0f));

        // Stalk cylinder
        mat4 stalk = sm;
        stalk = translate(stalk, vec3(0.0f, bs.h * 0.5f, 0.0f));
        stalk = scale(stalk, vec3(0.045f, bs.h, 0.045f));
        Primitives::drawCylinder(shader, stalk, stalkCol);

        // Nodes along the culm (rings every 0.7 units)
        int numNodes = (int)(bs.h / 0.7f);
        for (int n = 1; n <= numNodes; n++) {
            mat4 node = sm;
            node = translate(node, vec3(0.0f, (float)n * 0.7f, 0.0f));
            node = scale(node, vec3(0.06f, 0.03f, 0.06f));
            Primitives::drawCylinder(shader, node, nodeCol);
        }

        // Feathery leaves at top
        mat4 crown = sm;
        crown = translate(crown, vec3(0.0f, bs.h, 0.0f));
        for (int l = 0; l < 4; l++) {
            float lAngle = (float)l * 90.0f + 15.0f;
            mat4 leaf = crown;
            leaf = rotate(leaf, radians(lAngle), vec3(0.0f, 1.0f, 0.0f));
            leaf = rotate(leaf, radians(38.0f), vec3(1.0f, 0.0f, 0.0f));
            leaf = scale(leaf, vec3(0.18f, 0.90f, 0.02f));
            Primitives::drawCube(shader, leaf, leafCol);
        }
    }
}

// ─── Public Dispatcher ───────────────────────────────────────────────
void draw(Shader& shader, const mat4& model, TreeType type)
{
    switch (type) {
        case TREE_PALM:    drawPalm   (shader, model); break;
        case TREE_BANANA:  drawBanana (shader, model); break;
        case TREE_BAMBOO:  drawBamboo (shader, model); break;
        case TREE_GENERAL:
        default:           drawGeneral(shader, model); break;
    }
}

} // namespace Tree
