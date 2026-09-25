// Tree.cpp — Authentic Bangladeshi trees (Narikel, Kola, Bot/Aam, Bansh)
// Redesigned with organic cascading palm fronds, graceful arching banana paddles,
// lush natural foliage clusters, and dense bamboo groves.

#include "objects/Tree.h"
#include "Primitives.h"
#include <cmath>

using namespace math;

namespace Tree {

// ─── 1. Traditional Coconut Palm (Narikel Gach / নারকেল গাছ) ─────────
static void drawPalm(Shader& shader, const mat4& model)
{
    shader.setInt("uUseTexture", 0); // vibrant tropical foliage and ringed palm trunk

    vec3 trunkCol (0.46f, 0.34f, 0.20f); // ringed coconut bark
    vec3 ringCol  (0.32f, 0.22f, 0.12f); // dark annular leaf scars
    vec3 frondCol (0.12f, 0.42f, 0.10f); // lush tropical green fronds
    vec3 leafTip  (0.18f, 0.52f, 0.14f); // sunlit green leaflets
    vec3 nutCol   (0.36f, 0.50f, 0.18f); // green coconut cluster (Dab)
    vec3 brownNut (0.45f, 0.35f, 0.16f); // ripe dry coconut

    // Curved, slender leaning trunk (8 segmented sections)
    const int segments = 8;
    float segH = 0.75f;

    mat4 currT = model;
    for (int i = 0; i < segments; i++) {
        float frac = (float)i / segments;
        float radius = 0.17f - frac * 0.05f;

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

    // Cluster of green tender coconuts (Dab / ডাব) nestled at apex
    for (int n = 0; n < 8; n++) {
        float nAngle = (float)n * (2.0f * PI / 8.0f);
        mat4 nut = crown;
        nut = translate(nut, vec3(cosf(nAngle) * 0.22f, -0.10f, sinf(nAngle) * 0.22f));
        nut = scale(nut, vec3(0.14f, 0.18f, 0.14f));
        Primitives::drawSphere(shader, nut, (n % 3 == 0) ? brownNut : nutCol);
    }

    // 12 Cascading downward-arching palm fronds in 2 tiers
    const int numFronds = 12;
    for (int f = 0; f < numFronds; f++) {
        float fAngle = (float)f * (360.0f / (float)numFronds) + (f % 2 == 0 ? 0.0f : 15.0f);
        float droopBase = 28.0f + (f % 3) * 7.0f;

        mat4 frond = crown;
        frond = rotate(frond, radians(fAngle), vec3(0.0f, 1.0f, 0.0f));

        // Segment 1: Rises slightly and arches outward
        mat4 s1 = frond;
        s1 = rotate(s1, radians(droopBase), vec3(1.0f, 0.0f, 0.0f));
        s1 = translate(s1, vec3(0.0f, 0.65f, 0.0f));

        mat4 rib1 = scale(s1, vec3(0.035f, 1.30f, 0.035f));
        Primitives::drawCylinder(shader, rib1, ringCol);

        mat4 b1 = scale(s1, vec3(0.52f, 1.25f, 0.020f));
        Primitives::drawCube(shader, b1, frondCol);

        // Segment 2: Arches further outward and down
        mat4 s2 = s1;
        s2 = translate(s2, vec3(0.0f, 0.60f, 0.0f));
        s2 = rotate(s2, radians(32.0f), vec3(1.0f, 0.0f, 0.0f));
        s2 = translate(s2, vec3(0.0f, 0.60f, 0.0f));

        mat4 rib2 = scale(s2, vec3(0.028f, 1.20f, 0.028f));
        Primitives::drawCylinder(shader, rib2, ringCol);

        mat4 b2 = scale(s2, vec3(0.42f, 1.18f, 0.018f));
        Primitives::drawCube(shader, b2, frondCol);

        // Segment 3: Cascades steeply downward
        mat4 s3 = s2;
        s3 = translate(s3, vec3(0.0f, 0.55f, 0.0f));
        s3 = rotate(s3, radians(38.0f), vec3(1.0f, 0.0f, 0.0f));
        s3 = translate(s3, vec3(0.0f, 0.45f, 0.0f));

        mat4 b3 = scale(s3, vec3(0.28f, 0.95f, 0.015f));
        Primitives::drawCube(shader, b3, leafTip);
    }
}

// ─── 2. Banana Tree (Kola Gach / কলা গাছ) ──────────────────────────
static void drawBanana(Shader& shader, const mat4& model)
{
    shader.setInt("uUseTexture", 0); // translucent succulent greens and purplish banana mocha

    vec3 trunkBase(0.42f, 0.52f, 0.22f); // pale lime succulent pseudostem
    vec3 trunkRing(0.32f, 0.40f, 0.16f); // leaf sheath overlap ring
    vec3 leafCol  (0.16f, 0.48f, 0.14f); // rich lush banana leaf blade
    vec3 leafMid  (0.38f, 0.58f, 0.22f); // yellow-green central leaf midrib
    vec3 leafTip  (0.22f, 0.54f, 0.18f); // fresh leaf tip
    vec3 stalkCol (0.28f, 0.44f, 0.16f); // fruit bunch stalk
    vec3 fruitCol (0.32f, 0.52f, 0.16f); // green baby bananas (Kolar Fona)
    vec3 mochaCol (0.44f, 0.08f, 0.14f); // purplish-crimson pendant flower bud (Kolar Mocha / থোড়)

    // Succulent pseudostem with 4 layered tapering collars
    float trunkH = 2.4f;
    for (int t = 0; t < 4; t++) {
        float ty = (float)t * (trunkH * 0.25f);
        float rad = 0.20f - (float)t * 0.025f;
        mat4 sheath = model;
        sheath = translate(sheath, vec3(0.0f, ty + trunkH * 0.125f, 0.0f));
        mat4 sCyl = scale(sheath, vec3(rad, trunkH * 0.26f, rad));
        Primitives::drawCylinder(shader, sCyl, (t % 2 == 0) ? trunkBase : trunkBase * 0.92f);

        // Collar ring
        mat4 cRing = sheath;
        cRing = translate(cRing, vec3(0.0f, trunkH * 0.125f, 0.0f));
        cRing = scale(cRing, vec3(rad * 1.08f, 0.035f, rad * 1.08f));
        Primitives::drawCylinder(shader, cRing, trunkRing);
    }

    // Small banana sucker shoot (Kola Chara) at base
    mat4 sucker = model;
    sucker = translate(sucker, vec3(0.24f, 0.35f, 0.10f));
    sucker = rotate(sucker, radians(18.0f), vec3(0.0f, 0.0f, -1.0f));
    mat4 suckStem = scale(sucker, vec3(0.06f, 0.70f, 0.06f));
    Primitives::drawCylinder(shader, suckStem, trunkBase);
    mat4 suckLeaf = sucker;
    suckLeaf = translate(suckLeaf, vec3(0.0f, 0.45f, 0.0f));
    suckLeaf = rotate(suckLeaf, radians(32.0f), vec3(1.0f, 0.0f, 0.0f));
    suckLeaf = scale(suckLeaf, vec3(0.18f, 0.55f, 0.015f));
    Primitives::drawCube(shader, suckLeaf, leafCol);

    mat4 crown = model;
    crown = translate(crown, vec3(0.0f, trunkH, 0.0f));

    // 8 Broad arching paddle leaves cascading gracefully outward
    const int numLeaves = 8;
    for (int i = 0; i < numLeaves; i++) {
        float angle = (float)i * (360.0f / (float)numLeaves) + 12.0f;
        mat4 leaf = crown;
        leaf = rotate(leaf, radians(angle), vec3(0.0f, 1.0f, 0.0f));

        // Part 1: Stalk rising up and out (26° tilt)
        mat4 p1 = leaf;
        p1 = rotate(p1, radians(26.0f), vec3(1.0f, 0.0f, 0.0f));
        p1 = translate(p1, vec3(0.0f, 0.65f, 0.0f));

        // Central yellow-green midrib
        mat4 rib1 = scale(p1, vec3(0.035f, 1.30f, 0.035f));
        Primitives::drawCylinder(shader, rib1, leafMid);

        // Broad green leaf blade
        mat4 b1 = scale(p1, vec3(0.55f, 1.25f, 0.020f));
        Primitives::drawCube(shader, b1, leafCol);

        // Part 2: Broad paddle arching over (40° tilt)
        mat4 p2 = p1;
        p2 = translate(p2, vec3(0.0f, 0.60f, 0.0f));
        p2 = rotate(p2, radians(40.0f), vec3(1.0f, 0.0f, 0.0f));
        p2 = translate(p2, vec3(0.0f, 0.58f, 0.0f));

        mat4 rib2 = scale(p2, vec3(0.028f, 1.15f, 0.028f));
        Primitives::drawCylinder(shader, rib2, leafMid);

        mat4 b2 = scale(p2, vec3(0.62f, 1.15f, 0.018f));
        Primitives::drawCube(shader, b2, leafCol);

        // Part 3: Drooping tip (46° tilt downward)
        mat4 p3 = p2;
        p3 = translate(p3, vec3(0.0f, 0.55f, 0.0f));
        p3 = rotate(p3, radians(46.0f), vec3(1.0f, 0.0f, 0.0f));
        p3 = translate(p3, vec3(0.0f, 0.45f, 0.0f));

        mat4 b3 = scale(p3, vec3(0.42f, 0.90f, 0.015f));
        Primitives::drawCube(shader, b3, leafTip);
    }

    // ── Hanging Banana Inflorescence (Kolar Thod & Mocha / থোড় ও মোচা) ──
    // Arching flower stalk emerging from the center of crown and curving downward
    mat4 stalk = crown;
    stalk = translate(stalk, vec3(0.18f, -0.05f, 0.12f));
    stalk = rotate(stalk, radians(135.0f), vec3(1.0f, 0.0f, 0.0f));
    mat4 stalkCyl = scale(stalk, vec3(0.040f, 0.95f, 0.040f));
    Primitives::drawCylinder(shader, stalkCyl, stalkCol);

    // 3 Tiered hands of bananas (Kolar Fona / কলার ফণা)
    for (int tier = 0; tier < 3; tier++) {
        float ty = 0.22f + (float)tier * 0.16f;
        for (int b = 0; b < 6; b++) {
            float ba = (float)b * (2.0f * PI / 6.0f);
            mat4 banana = stalk;
            banana = translate(banana, vec3(cosf(ba) * 0.11f, ty, sinf(ba) * 0.11f));
            banana = rotate(banana, radians(25.0f), vec3(cosf(ba), 0.0f, sinf(ba)));
            banana = scale(banana, vec3(0.032f, 0.16f, 0.032f));
            Primitives::drawCylinder(shader, banana, fruitCol);
        }
    }

    // Large teardrop-shaped pendant blossom heart (Kolar Mocha / কলার মোচা)
    mat4 mocha = stalk;
    mocha = translate(mocha, vec3(0.0f, 0.85f, 0.0f));
    mat4 mochaS = scale(mocha, vec3(0.14f, 0.26f, 0.14f));
    Primitives::drawSphere(shader, mochaS, mochaCol);

    mat4 mochaTip = mocha;
    mochaTip = translate(mochaTip, vec3(0.0f, 0.15f, 0.0f));
    mochaTip = scale(mochaTip, vec3(0.08f, 0.14f, 0.08f));
    Primitives::drawCone(shader, mochaTip, mochaCol * 0.85f);
}

// ─── 3. Majestic Branching Mango / Banyan Tree (Aam / Bot Gach) ──────
static void drawGeneral(Shader& shader, const mat4& model)
{
    shader.setInt("uUseTexture", 0); // vibrant procedural canopy with ripe mangoes

    vec3 trunkCol (0.36f, 0.26f, 0.16f); // gnarled bark
    vec3 foliage1 (0.10f, 0.38f, 0.12f); // deep forest green
    vec3 foliage2 (0.16f, 0.46f, 0.14f); // medium lush green
    vec3 foliage3 (0.08f, 0.30f, 0.08f); // dark shadowed foliage
    vec3 mangoCol (0.95f, 0.72f, 0.16f); // ripe golden mango (Paka Aam)
    vec3 stemCol  (0.38f, 0.28f, 0.14f); // mango stem

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

    // Golden ripe mangoes (Paka Aam / পাকা আম) dangling on outer boughs and under-canopy
    struct MangoPos { float x, y, z; float stemH; vec3 col; };
    MangoPos mangoes[] = {
        // Front-facing clusters (facing camera in 3/4 perspective snapshot)
        {  0.60f, 2.50f,  3.10f, 0.25f, vec3(0.98f, 0.72f, 0.12f) },
        {  0.85f, 2.42f,  2.95f, 0.32f, vec3(0.92f, 0.50f, 0.12f) }, // twin cluster
        { -0.40f, 2.55f,  3.00f, 0.22f, vec3(0.98f, 0.76f, 0.14f) },
        {  1.60f, 2.50f,  2.40f, 0.26f, vec3(0.94f, 0.60f, 0.12f) },
        {  1.85f, 2.40f,  2.15f, 0.34f, vec3(0.90f, 0.46f, 0.10f) }, // twin cluster
        { -1.40f, 2.60f,  2.00f, 0.25f, vec3(0.96f, 0.68f, 0.12f) },
        {  2.30f, 2.65f,  0.80f, 0.28f, vec3(0.96f, 0.70f, 0.12f) },
        {  2.45f, 2.55f,  0.55f, 0.35f, vec3(0.92f, 0.54f, 0.12f) }, // twin cluster
        { -2.10f, 2.70f,  0.50f, 0.26f, vec3(0.95f, 0.64f, 0.14f) },
        {  0.50f, 2.35f,  2.20f, 0.30f, vec3(0.97f, 0.72f, 0.12f) }
    };

    vec3 leafAccent(0.18f, 0.48f, 0.14f);
    for (const auto& mp : mangoes) {
        // Slender stem connecting up to the canopy branch
        mat4 stem = model;
        stem = translate(stem, vec3(mp.x, mp.y + mp.stemH * 0.5f, mp.z));
        stem = scale(stem, vec3(0.016f, mp.stemH, 0.016f));
        Primitives::drawCylinder(shader, stem, stemCol);

        // Little dark green mango leaf attached to stem joint
        mat4 mLeaf = model;
        mLeaf = translate(mLeaf, vec3(mp.x + 0.040f, mp.y + mp.stemH * 0.8f, mp.z));
        mLeaf = rotate(mLeaf, radians(35.0f), vec3(0.0f, 0.0f, 1.0f));
        mLeaf = scale(mLeaf, vec3(0.065f, 0.018f, 0.12f));
        Primitives::drawCube(shader, mLeaf, leafAccent);

        // Distinctive ripe Bengali mango fruit (kidney/ovoid shape with slight asymmetry)
        mat4 aam = model;
        aam = translate(aam, vec3(mp.x, mp.y, mp.z));
        aam = rotate(aam, radians(12.0f), vec3(0.0f, 0.0f, 1.0f));
        aam = scale(aam, vec3(0.14f, 0.21f, 0.13f));
        Primitives::drawSphere(shader, aam, mp.col);
    }
}

// ─── 4. Dense Bamboo Grove (Bansher Jhar / বাঁশঝাড়) ───────────────────
static void drawBamboo(Shader& shader, const mat4& model)
{
    shader.setInt("uUseTexture", 0); // vibrant golden-green bamboo poles and delicate foliage

    vec3 stalkCol(0.36f, 0.52f, 0.20f); // yellowish-green bamboo stalk
    vec3 nodeCol (0.24f, 0.36f, 0.14f); // dark bamboo culm node/joint
    vec3 leafCol (0.20f, 0.52f, 0.16f); // feathery lanceolate bamboo leaves

    // Clump of 10 tall segmented stalks arching gracefully
    struct BambooStalk {
        float ox, oz;
        float h;
        float leanX, leanZ;
    };

    BambooStalk stalks[] = {
        {  0.00f,  0.00f, 5.4f,  2.0f, -2.0f },
        { -0.35f,  0.25f, 4.9f, -4.0f,  3.0f },
        {  0.40f, -0.20f, 5.6f,  5.0f, -3.0f },
        { -0.25f, -0.40f, 4.5f, -3.0f, -5.0f },
        {  0.30f,  0.40f, 5.1f,  4.0f,  4.0f },
        { -0.50f, -0.15f, 4.3f, -6.0f, -1.0f },
        {  0.50f,  0.15f, 4.8f,  6.0f,  2.0f },
        {  0.10f, -0.50f, 5.0f, -1.0f, -4.0f },
        { -0.15f,  0.45f, 4.7f,  3.0f,  5.0f },
        {  0.35f, -0.45f, 5.3f, -2.0f, -3.0f }
    };

    for (const auto& bs : stalks) {
        mat4 sm = model;
        sm = translate(sm, vec3(bs.ox, 0.0f, bs.oz));
        sm = rotate(sm, radians(bs.leanX), vec3(1.0f, 0.0f, 0.0f));
        sm = rotate(sm, radians(bs.leanZ), vec3(0.0f, 0.0f, 1.0f));

        // Stalk cylinder
        mat4 stalk = sm;
        stalk = translate(stalk, vec3(0.0f, bs.h * 0.5f, 0.0f));
        stalk = scale(stalk, vec3(0.050f, bs.h, 0.050f));
        Primitives::drawCylinder(shader, stalk, stalkCol);

        // Nodes along the culm (rings every 0.65 units)
        int numNodes = (int)(bs.h / 0.65f);
        for (int n = 1; n <= numNodes; n++) {
            mat4 node = sm;
            node = translate(node, vec3(0.0f, (float)n * 0.65f, 0.0f));
            node = scale(node, vec3(0.065f, 0.035f, 0.065f));
            Primitives::drawCylinder(shader, node, nodeCol);
        }

        // Feathery leaves at top
        mat4 crown = sm;
        crown = translate(crown, vec3(0.0f, bs.h, 0.0f));
        for (int l = 0; l < 5; l++) {
            float lAngle = (float)l * 72.0f + 15.0f;
            mat4 leaf = crown;
            leaf = rotate(leaf, radians(lAngle), vec3(0.0f, 1.0f, 0.0f));
            leaf = rotate(leaf, radians(38.0f), vec3(1.0f, 0.0f, 0.0f));
            leaf = scale(leaf, vec3(0.22f, 0.95f, 0.02f));
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
