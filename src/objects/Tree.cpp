// Tree.cpp — Authentic Bangladeshi trees (Narikel, Kola, Bot/Aam, Bansh)
// All foliage and leaves redesigned for maximum natural realism using STRICTLY
// unit cubes (Primitives::drawCube) and unit cubes made of 2 triangles (Primitives::drawTriangle / Primitives::drawPlane).

#include "objects/Tree.h"
#include "Primitives.h"
#include <cmath>

using namespace math;

namespace {

// Helper: draw a realistic tapering lanceolate leaflet/blade using strictly unit cube and unit triangle
inline void drawLeaflet(Shader& shader, const mat4& base, float length, float width, float droopAngle, const vec3& color)
{
    mat4 m = base;
    if (fabsf(droopAngle) > 1e-4f) {
        m = rotate(m, radians(droopAngle), vec3(1.0f, 0.0f, 0.0f));
    }
    // Main leaf blade body: thin rectangular unit cube
    mat4 b = m;
    b = translate(b, vec3(0.0f, length * 0.40f, 0.0f));
    b = scale(b, vec3(width, length * 0.80f, 0.006f));
    Primitives::drawCube(shader, b, color);

    // Pointed leaf tip: unit triangle
    mat4 tip = m;
    tip = translate(tip, vec3(0.0f, length * 0.80f, 0.0f));
    tip = scale(tip, vec3(width, length * 0.20f, 1.0f));
    Primitives::drawTriangle(shader, tip, color * 1.06f);
}

} // anonymous namespace

namespace Tree {

// ─── 1. Traditional Coconut Palm (Narikel Gach / নারকেল গাছ) ─────────
// Modeled with curved segmented trunk, cluster of tender coconuts (Dab),
// and cascading pinnate fronds with realistic rachis ribs and individual tapering leaflets.
// All leaves constructed purely from unit cubes and unit triangles.
static void drawPalm(Shader& shader, const mat4& model)
{
    shader.setInt("uUseTexture", 0); // vibrant tropical foliage and ringed palm trunk

    vec3 trunkCol (0.46f, 0.34f, 0.20f); // ringed coconut bark
    vec3 ringCol  (0.32f, 0.22f, 0.12f); // dark annular leaf scars
    vec3 frondCol (0.11f, 0.38f, 0.09f); // deep lush tropical green fronds
    vec3 midGreen (0.16f, 0.48f, 0.12f); // vibrant middle leaflet green
    vec3 leafTip  (0.22f, 0.56f, 0.15f); // sunlit golden-green leaflet tips
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

    // 14 Cascading downward-arching palm fronds in 3 concentric tiers
    // Modeled with individual pairs of tapering leaflets along the rachis
    const int numFronds = 14;
    for (int f = 0; f < numFronds; f++) {
        float fAngle = (float)f * (360.0f / (float)numFronds) + (f % 2 == 0 ? 0.0f : 12.0f);
        float droopBase = (f < 4) ? 18.0f : ((f < 9) ? 32.0f : 44.0f);
        float tierScale = (f < 4) ? 0.88f : ((f < 9) ? 1.00f : 0.95f);

        mat4 frond = crown;
        frond = rotate(frond, radians(fAngle), vec3(0.0f, 1.0f, 0.0f));

        // Segment 1: Rises outward from crown
        mat4 s1 = frond;
        s1 = rotate(s1, radians(droopBase), vec3(1.0f, 0.0f, 0.0f));
        s1 = translate(s1, vec3(0.0f, 0.55f * tierScale, 0.0f));

        // Central rachis rib (unit cube)
        mat4 rib1 = scale(s1, vec3(0.028f, 1.10f * tierScale, 0.028f));
        Primitives::drawCube(shader, rib1, ringCol);

        // Pinnate leaflets along segment 1 (4 pairs)
        for (int p = 0; p < 4; p++) {
            float py = ((float)p - 1.5f) * 0.24f * tierScale;
            float pLen = (0.32f + (float)p * 0.06f) * tierScale;
            float pWidth = 0.038f;

            // Left leaflet
            mat4 lft = s1;
            lft = translate(lft, vec3(-0.015f, py, 0.0f));
            lft = rotate(lft, radians(52.0f), vec3(0.0f, 0.0f, 1.0f));
            drawLeaflet(shader, lft, pLen, pWidth, 18.0f, frondCol);

            // Right leaflet
            mat4 rgt = s1;
            rgt = translate(rgt, vec3(0.015f, py, 0.0f));
            rgt = rotate(rgt, radians(-52.0f), vec3(0.0f, 0.0f, 1.0f));
            drawLeaflet(shader, rgt, pLen, pWidth, 18.0f, frondCol);
        }

        // Segment 2: Arches outward and begins descent
        mat4 s2 = s1;
        s2 = translate(s2, vec3(0.0f, 0.55f * tierScale, 0.0f));
        s2 = rotate(s2, radians(30.0f), vec3(1.0f, 0.0f, 0.0f));
        s2 = translate(s2, vec3(0.0f, 0.55f * tierScale, 0.0f));

        mat4 rib2 = scale(s2, vec3(0.024f, 1.10f * tierScale, 0.024f));
        Primitives::drawCube(shader, rib2, ringCol);

        // Pinnate leaflets along segment 2 (5 pairs, longest mature leaflets)
        for (int p = 0; p < 5; p++) {
            float py = ((float)p - 2.0f) * 0.20f * tierScale;
            float pLen = (0.52f + sinf((float)p * 0.7f) * 0.08f) * tierScale;
            float pWidth = 0.042f;

            mat4 lft = s2;
            lft = translate(lft, vec3(-0.012f, py, 0.0f));
            lft = rotate(lft, radians(58.0f), vec3(0.0f, 0.0f, 1.0f));
            drawLeaflet(shader, lft, pLen, pWidth, 22.0f, (p % 2 == 0) ? frondCol : midGreen);

            mat4 rgt = s2;
            rgt = translate(rgt, vec3(0.012f, py, 0.0f));
            rgt = rotate(rgt, radians(-58.0f), vec3(0.0f, 0.0f, 1.0f));
            drawLeaflet(shader, rgt, pLen, pWidth, 22.0f, (p % 2 == 0) ? frondCol : midGreen);
        }

        // Segment 3: Cascades downward to pointed frond tip
        mat4 s3 = s2;
        s3 = translate(s3, vec3(0.0f, 0.55f * tierScale, 0.0f));
        s3 = rotate(s3, radians(36.0f), vec3(1.0f, 0.0f, 0.0f));
        s3 = translate(s3, vec3(0.0f, 0.45f * tierScale, 0.0f));

        mat4 rib3 = scale(s3, vec3(0.020f, 0.90f * tierScale, 0.020f));
        Primitives::drawCube(shader, rib3, ringCol);

        // Pinnate leaflets along segment 3 (4 pairs tapering toward tip)
        for (int p = 0; p < 4; p++) {
            float py = ((float)p - 1.5f) * 0.20f * tierScale;
            float pLen = (0.42f - (float)p * 0.08f) * tierScale;
            float pWidth = 0.034f;

            mat4 lft = s3;
            lft = translate(lft, vec3(-0.010f, py, 0.0f));
            lft = rotate(lft, radians(48.0f), vec3(0.0f, 0.0f, 1.0f));
            drawLeaflet(shader, lft, pLen, pWidth, 26.0f, leafTip);

            mat4 rgt = s3;
            rgt = translate(rgt, vec3(0.010f, py, 0.0f));
            rgt = rotate(rgt, radians(-48.0f), vec3(0.0f, 0.0f, 1.0f));
            drawLeaflet(shader, rgt, pLen, pWidth, 26.0f, leafTip);
        }

        // Terminal apex leaflet
        mat4 term = s3;
        term = translate(term, vec3(0.0f, 0.45f * tierScale, 0.0f));
        drawLeaflet(shader, term, 0.28f * tierScale, 0.035f, 10.0f, leafTip * 1.08f);
    }
}

// ─── 2. Banana Tree (Kola Gach / কলা গাছ) ──────────────────────────
// Modeled with layered succulent pseudostem, arching flower stalk with Kolar Mocha,
// unfurling central rolled leaf shoot, and 8 broad arching paddle leaves with
// yellow-green midribs, V-gutter dihedral angles, wind-cleft sub-panels, and pointed dual tips.
// All leaves constructed purely from unit cubes and unit triangles.
static void drawBanana(Shader& shader, const mat4& model)
{
    shader.setInt("uUseTexture", 0); // translucent succulent greens and purplish banana mocha

    vec3 trunkBase(0.42f, 0.52f, 0.22f); // pale lime succulent pseudostem
    vec3 trunkRing(0.32f, 0.40f, 0.16f); // leaf sheath overlap ring
    vec3 leafCol  (0.15f, 0.48f, 0.13f); // rich lush emerald banana leaf blade
    vec3 leafMid  (0.38f, 0.58f, 0.20f); // yellow-green central leaf midrib
    vec3 leafTip  (0.22f, 0.56f, 0.18f); // fresh bright leaf tip
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
    drawLeaflet(shader, suckLeaf, 0.55f, 0.16f, 32.0f, leafCol);

    mat4 crown = model;
    crown = translate(crown, vec3(0.0f, trunkH, 0.0f));

    // 1. Unfurling young central rolled leaf spike (Kola-r Nobin Majh-Pata)
    mat4 spire = crown;
    spire = translate(spire, vec3(0.0f, 0.70f, 0.0f));
    mat4 sBody = scale(spire, vec3(0.038f, 1.40f, 0.038f));
    Primitives::drawCube(shader, sBody, vec3(0.36f, 0.60f, 0.18f));
    mat4 sTip = spire;
    sTip = translate(sTip, vec3(0.0f, 0.70f, 0.0f));
    sTip = scale(sTip, vec3(0.045f, 0.25f, 1.0f));
    Primitives::drawTriangle(shader, sTip, vec3(0.42f, 0.66f, 0.20f));

    // 2. 8 Broad arching paddle leaves (Kola Pata) cascading gracefully outward
    const int numLeaves = 8;
    for (int i = 0; i < numLeaves; i++) {
        float angle = (float)i * (360.0f / (float)numLeaves) + 12.0f;
        mat4 leaf = crown;
        leaf = rotate(leaf, radians(angle), vec3(0.0f, 1.0f, 0.0f));

        // Part 1: Stalk rising up and out (26° tilt)
        mat4 p1 = leaf;
        p1 = rotate(p1, radians(26.0f), vec3(1.0f, 0.0f, 0.0f));
        p1 = translate(p1, vec3(0.0f, 0.65f, 0.0f));

        // Central yellow-green midrib (unit cube)
        mat4 rib1 = scale(p1, vec3(0.036f, 1.30f, 0.036f));
        Primitives::drawCube(shader, rib1, leafMid);

        // Broad green leaf blade: left and right halves angled in a shallow V-gutter (12 deg)
        mat4 lBlade1 = p1;
        lBlade1 = translate(lBlade1, vec3(-0.16f, 0.0f, 0.0f));
        lBlade1 = rotate(lBlade1, radians(12.0f), vec3(0.0f, 1.0f, 0.0f));
        lBlade1 = scale(lBlade1, vec3(0.30f, 1.25f, 0.008f));
        Primitives::drawCube(shader, lBlade1, leafCol);

        mat4 rBlade1 = p1;
        rBlade1 = translate(rBlade1, vec3(0.16f, 0.0f, 0.0f));
        rBlade1 = rotate(rBlade1, radians(-12.0f), vec3(0.0f, 1.0f, 0.0f));
        rBlade1 = scale(rBlade1, vec3(0.30f, 1.25f, 0.008f));
        Primitives::drawCube(shader, rBlade1, leafCol);

        // Part 2: Broad paddle arching over (40° tilt)
        mat4 p2 = p1;
        p2 = translate(p2, vec3(0.0f, 0.60f, 0.0f));
        p2 = rotate(p2, radians(40.0f), vec3(1.0f, 0.0f, 0.0f));
        p2 = translate(p2, vec3(0.0f, 0.58f, 0.0f));

        mat4 rib2 = scale(p2, vec3(0.028f, 1.15f, 0.028f));
        Primitives::drawCube(shader, rib2, leafMid);

        // Sub-panel 1 (inner paddle)
        mat4 lBlade2A = p2;
        lBlade2A = translate(lBlade2A, vec3(-0.19f, -0.25f, 0.0f));
        lBlade2A = rotate(lBlade2A, radians(14.0f), vec3(0.0f, 1.0f, 0.0f));
        lBlade2A = scale(lBlade2A, vec3(0.36f, 0.55f, 0.007f));
        Primitives::drawCube(shader, lBlade2A, leafCol);

        mat4 rBlade2A = p2;
        rBlade2A = translate(rBlade2A, vec3(0.19f, -0.25f, 0.0f));
        rBlade2A = rotate(rBlade2A, radians(-14.0f), vec3(0.0f, 1.0f, 0.0f));
        rBlade2A = scale(rBlade2A, vec3(0.36f, 0.55f, 0.007f));
        Primitives::drawCube(shader, rBlade2A, leafCol);

        // Sub-panel 2 (outer paddle with natural slight wind flutter)
        mat4 lBlade2B = p2;
        lBlade2B = translate(lBlade2B, vec3(-0.18f, 0.28f, 0.0f));
        lBlade2B = rotate(lBlade2B, radians(16.0f), vec3(0.0f, 1.0f, 0.0f));
        lBlade2B = scale(lBlade2B, vec3(0.34f, 0.52f, 0.007f));
        Primitives::drawCube(shader, lBlade2B, leafCol * 1.04f);

        mat4 rBlade2B = p2;
        rBlade2B = translate(rBlade2B, vec3(0.18f, 0.28f, 0.0f));
        rBlade2B = rotate(rBlade2B, radians(-16.0f), vec3(0.0f, 1.0f, 0.0f));
        rBlade2B = scale(rBlade2B, vec3(0.34f, 0.52f, 0.007f));
        Primitives::drawCube(shader, rBlade2B, leafCol * 1.04f);

        // Part 3: Drooping tip (46° tilt downward)
        mat4 p3 = p2;
        p3 = translate(p3, vec3(0.0f, 0.55f, 0.0f));
        p3 = rotate(p3, radians(46.0f), vec3(1.0f, 0.0f, 0.0f));
        p3 = translate(p3, vec3(0.0f, 0.45f, 0.0f));

        mat4 rib3 = scale(p3, vec3(0.022f, 0.90f, 0.022f));
        Primitives::drawCube(shader, rib3, leafMid);

        // Tapering blade halves
        mat4 lBlade3 = p3;
        lBlade3 = translate(lBlade3, vec3(-0.12f, -0.08f, 0.0f));
        lBlade3 = rotate(lBlade3, radians(10.0f), vec3(0.0f, 1.0f, 0.0f));
        lBlade3 = scale(lBlade3, vec3(0.22f, 0.70f, 0.006f));
        Primitives::drawCube(shader, lBlade3, leafTip);

        mat4 rBlade3 = p3;
        rBlade3 = translate(rBlade3, vec3(0.12f, -0.08f, 0.0f));
        rBlade3 = rotate(rBlade3, radians(-10.0f), vec3(0.0f, 1.0f, 0.0f));
        rBlade3 = scale(rBlade3, vec3(0.22f, 0.70f, 0.006f));
        Primitives::drawCube(shader, rBlade3, leafTip);

        // Pointed apex tip (dual unit triangles)
        mat4 lTip = p3;
        lTip = translate(lTip, vec3(-0.06f, 0.32f, 0.0f));
        lTip = scale(lTip, vec3(0.12f, 0.24f, 1.0f));
        Primitives::drawTriangle(shader, lTip, leafTip * 1.06f);

        mat4 rTip = p3;
        rTip = translate(rTip, vec3(0.06f, 0.32f, 0.0f));
        rTip = scale(rTip, vec3(0.12f, 0.24f, 1.0f));
        Primitives::drawTriangle(shader, rTip, leafTip * 1.06f);
    }

    // ── Hanging Banana Inflorescence (Kolar Thod & Mocha / থোড় ও মোচা) ──
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
// Modeled with sturdy gnarled trunk, buttress root flares, radiating limbs,
// and natural tiered volumetric foliage canopy constructed from organic volumetric
// spheres (Primitives::drawSphere) arranged in billowing clouds with subtle color gradients,
// accented with delicate perimeter lanceolate leaves (drawLeaflet) and golden ripe
// mango fruits (Paka Aam) dangling beneath the canopy.
static void drawGeneral(Shader& shader, const mat4& model)
{
    shader.setInt("uUseTexture", 0); // vibrant procedural canopy with ripe mangoes

    vec3 trunkCol (0.36f, 0.26f, 0.16f); // gnarled bark
    vec3 barkDark (0.28f, 0.20f, 0.12f); // dark bark creases

    // Foliage canopy gradient palette
    vec3 foliageSunlit(0.24f, 0.54f, 0.16f); // sunlit golden-green apex highlights
    vec3 foliageTop   (0.20f, 0.48f, 0.14f); // upper canopy lush mango green
    vec3 foliageMid   (0.15f, 0.42f, 0.12f); // mid canopy deep mango green
    vec3 foliageDark  (0.09f, 0.32f, 0.09f); // shaded under-canopy green
    vec3 foliageCore  (0.06f, 0.24f, 0.06f); // deep interior core shadow green
    vec3 youngFlush   (0.38f, 0.34f, 0.14f); // tender coppery-bronze new shoots (Aam-er Pollob)
    vec3 youngFlush2  (0.44f, 0.32f, 0.12f); // fresh reddish-bronze spring growth
    vec3 stemCol      (0.38f, 0.28f, 0.14f); // mango fruit stem

    // Sturdy gnarled trunk
    float trunkH = 2.8f;
    mat4 trunk = model;
    trunk = translate(trunk, vec3(0.0f, trunkH * 0.5f, 0.0f));
    trunk = scale(trunk, vec3(0.42f, trunkH, 0.42f));
    Primitives::drawCylinder(shader, trunk, trunkCol);

    // Buttress root flares at base
    for (int r = 0; r < 4; r++) {
        float ra = (float)r * 90.0f + 25.0f;
        mat4 root = model;
        root = rotate(root, radians(ra), vec3(0.0f, 1.0f, 0.0f));
        root = translate(root, vec3(0.35f, 0.30f, 0.0f));
        root = rotate(root, radians(35.0f), vec3(0.0f, 0.0f, 1.0f));
        root = scale(root, vec3(0.16f, 0.85f, 0.14f));
        Primitives::drawCube(shader, root, barkDark);
    }

    // 4 Primary spreading limbs radiating outward
    float branchAngles[] = { 15.0f, 105.0f, 195.0f, 285.0f };
    for (int b = 0; b < 4; b++) {
        mat4 branch = model;
        branch = translate(branch, vec3(0.0f, trunkH * 0.75f, 0.0f));
        branch = rotate(branch, radians(branchAngles[b]), vec3(0.0f, 1.0f, 0.0f));
        branch = rotate(branch, radians(38.0f), vec3(1.0f, 0.0f, 0.0f));
        branch = translate(branch, vec3(0.0f, 1.15f, 0.0f));
        mat4 bM = scale(branch, vec3(0.18f, 2.3f, 0.18f));
        Primitives::drawCylinder(shader, bM, trunkCol);

        // Secondary branchlet reaching outward into peripheral foliage
        mat4 subB = branch;
        subB = translate(subB, vec3(0.0f, 0.7f, 0.0f));
        subB = rotate(subB, radians(28.0f), vec3(0.0f, 0.0f, 1.0f));
        subB = translate(subB, vec3(0.0f, 0.45f, 0.0f));
        subB = scale(subB, vec3(0.11f, 0.95f, 0.11f));
        Primitives::drawCylinder(shader, subB, trunkCol);
    }

    // ── Volumetric Canopy: Billowing Cloud Spheres (Primitives::drawSphere) ──
    // Multi-tiered arrangement creating a lush, organic, dome-shaped mango tree canopy
    struct FoliageSphere {
        float x, y, z;
        float rx, ry, rz;
        vec3 color;
    };

    static const FoliageSphere canopySpheres[] = {
        // ── 1. Upper Central Crown & Apex ──
        {  0.10f, 6.10f, -0.10f,  1.55f, 1.10f, 1.55f,  foliageSunlit },
        {  0.00f, 5.45f,  0.00f,  2.20f, 1.50f, 2.20f,  foliageTop },
        { -0.25f, 5.15f,  0.30f,  1.90f, 1.35f, 1.90f,  foliageTop },
        {  0.25f, 5.20f, -0.25f,  1.85f, 1.30f, 1.85f,  foliageSunlit },

        // ── 2. Upper Spreading Bough Cloud Lobes ──
        // East / North-East bough lobes
        {  2.10f, 4.85f,  0.15f,  1.80f, 1.35f, 1.75f,  foliageTop },
        {  1.55f, 4.70f, -1.65f,  1.70f, 1.25f, 1.65f,  foliageTop },
        {  2.35f, 5.10f,  0.55f,  1.15f, 0.90f, 1.15f,  youngFlush },
        // South / South-East bough lobes
        {  0.20f, 4.75f,  2.15f,  1.85f, 1.35f, 1.80f,  foliageMid },
        {  1.60f, 4.55f,  1.50f,  1.65f, 1.25f, 1.60f,  foliageTop },
        {  0.85f, 4.35f,  2.25f,  1.45f, 1.10f, 1.40f,  foliageSunlit },
        // West / South-West bough lobes
        { -2.10f, 4.75f, -0.10f,  1.80f, 1.35f, 1.75f,  foliageMid },
        { -1.55f, 4.50f,  1.55f,  1.65f, 1.25f, 1.60f,  foliageDark },
        { -2.30f, 5.05f, -0.55f,  1.15f, 0.85f, 1.15f,  youngFlush2 },
        // North / North-West bough lobes
        {  0.15f, 4.90f, -2.10f,  1.80f, 1.30f, 1.75f,  foliageSunlit },
        { -1.60f, 4.65f, -1.55f,  1.65f, 1.25f, 1.60f,  foliageMid },

        // ── 3. Lower Drooping Canopy Skirts (Softening lower perimeter) ──
        {  2.25f, 3.65f,  0.20f,  1.45f, 1.05f, 1.40f,  foliageDark },
        {  1.75f, 3.50f, -1.45f,  1.40f, 1.00f, 1.35f,  foliageDark },
        {  1.70f, 3.45f,  1.65f,  1.40f, 0.95f, 1.35f,  foliageMid },
        {  0.10f, 3.55f,  2.35f,  1.50f, 1.05f, 1.45f,  foliageMid },
        { -1.65f, 3.40f,  1.70f,  1.40f, 0.95f, 1.35f,  foliageDark },
        { -2.25f, 3.60f, -0.15f,  1.45f, 1.00f, 1.40f,  foliageDark },
        { -1.65f, 3.55f, -1.60f,  1.40f, 0.95f, 1.35f,  foliageDark },
        {  0.15f, 3.65f, -2.25f,  1.45f, 1.00f, 1.40f,  foliageMid },

        // ── 4. Interior Core Volume Spheres (Zero hollow gaps, rich solid body) ──
        {  0.00f, 4.55f,  0.00f,  2.40f, 1.60f, 2.40f,  foliageMid },
        {  0.65f, 4.10f,  0.55f,  1.50f, 1.20f, 1.50f,  foliageCore },
        { -0.65f, 4.15f, -0.55f,  1.50f, 1.20f, 1.50f,  foliageCore },
        { -0.55f, 4.05f,  0.65f,  1.50f, 1.20f, 1.50f,  foliageCore },
        {  0.55f, 4.15f, -0.65f,  1.50f, 1.20f, 1.50f,  foliageCore }
    };

    for (const auto& fs : canopySpheres) {
        mat4 sm = model;
        sm = translate(sm, vec3(fs.x, fs.y, fs.z));
        sm = scale(sm, vec3(fs.rx, fs.ry, fs.rz));
        Primitives::drawSphere(shader, sm, fs.color);
    }

    // ── 5. Drooping Lanceolate Mango Leaf Sprays along Perimeter ──
    // Adds delicate botanical leaf detail around the spherical canopy silhouette
    const int numSprays = 16;
    for (int s = 0; s < numSprays; s++) {
        float sa = (float)s * (2.0f * PI / (float)numSprays) + 0.18f;
        float sr = 2.40f + (float)(s % 3) * 0.22f;
        float sy = 3.25f + (float)(s % 2) * 0.35f;
        vec3 leafC = (s % 4 == 0) ? youngFlush : ((s % 2 == 0) ? foliageTop : foliageMid);

        mat4 mSpr = model;
        mSpr = translate(mSpr, vec3(cosf(sa) * sr, sy, sinf(sa) * sr));
        mSpr = rotate(mSpr, atan2f(sinf(sa), cosf(sa)), vec3(0.0f, 1.0f, 0.0f));
        mSpr = rotate(mSpr, radians(38.0f + (float)(s % 3) * 6.0f), vec3(1.0f, 0.0f, 0.0f));
        drawLeaflet(shader, mSpr, 0.34f, 0.068f, 14.0f, leafC);

        // Small lateral secondary leaflet for full spray appearance
        mat4 mSide = mSpr;
        mSide = translate(mSide, vec3(0.035f, 0.10f, 0.02f));
        mSide = rotate(mSide, radians(22.0f), vec3(0.0f, 0.0f, 1.0f));
        drawLeaflet(shader, mSide, 0.24f, 0.052f, 18.0f, leafC * 0.94f);
    }

    // ── 6. Golden Ripe Mangoes (Paka Aam / পাকা আম) dangling under canopy ──
    struct MangoPos { float x, y, z; float stemH; vec3 col; };
    static const MangoPos mangoes[] = {
        // Front-facing clusters
        {  0.65f, 2.50f,  3.10f, 0.26f, vec3(0.98f, 0.72f, 0.12f) },
        {  0.92f, 2.40f,  2.95f, 0.34f, vec3(0.92f, 0.50f, 0.12f) }, // twin pair
        { -0.45f, 2.55f,  3.00f, 0.24f, vec3(0.98f, 0.76f, 0.14f) },
        {  1.65f, 2.50f,  2.40f, 0.28f, vec3(0.94f, 0.60f, 0.12f) },
        {  1.92f, 2.38f,  2.15f, 0.36f, vec3(0.90f, 0.46f, 0.10f) }, // twin pair
        { -1.45f, 2.58f,  2.00f, 0.26f, vec3(0.96f, 0.68f, 0.12f) },
        // Side and rear clusters
        {  2.35f, 2.65f,  0.80f, 0.28f, vec3(0.96f, 0.70f, 0.12f) },
        {  2.50f, 2.52f,  0.55f, 0.36f, vec3(0.92f, 0.54f, 0.12f) }, // twin pair
        { -2.15f, 2.68f,  0.50f, 0.26f, vec3(0.95f, 0.64f, 0.14f) },
        {  0.55f, 2.35f,  2.20f, 0.32f, vec3(0.97f, 0.72f, 0.12f) },
        { -1.85f, 2.60f, -1.10f, 0.28f, vec3(0.96f, 0.68f, 0.12f) },
        {  1.40f, 2.55f, -1.80f, 0.30f, vec3(0.94f, 0.58f, 0.12f) }
    };

    vec3 leafAccent(0.18f, 0.48f, 0.14f);
    for (const auto& mp : mangoes) {
        // Slender curved stem connecting up to the canopy branch
        mat4 stem = model;
        stem = translate(stem, vec3(mp.x, mp.y + mp.stemH * 0.5f, mp.z));
        stem = scale(stem, vec3(0.016f, mp.stemH, 0.016f));
        Primitives::drawCylinder(shader, stem, stemCol);

        // Dark green mango leaf attached to stem joint
        mat4 mLeaf = model;
        mLeaf = translate(mLeaf, vec3(mp.x + 0.042f, mp.y + mp.stemH * 0.82f, mp.z));
        drawLeaflet(shader, mLeaf, 0.16f, 0.048f, 32.0f, leafAccent);

        // Distinctive ripe Bengali mango fruit (kidney/ovoid shape with slight asymmetry)
        mat4 aam = model;
        aam = translate(aam, vec3(mp.x, mp.y, mp.z));
        aam = rotate(aam, radians(12.0f), vec3(0.0f, 0.0f, 1.0f));
        aam = scale(aam, vec3(0.14f, 0.21f, 0.13f));
        Primitives::drawSphere(shader, aam, mp.col);
    }
}

// ─── 4. Dense Bamboo Grove (Bansher Jhar / বাঁশঝাড়) ───────────────────
// Modeled with segmented arching culms, culm joint nodes, lateral branchlets,
// and cascading feathery lanceolate leaves branching from upper nodes and crown.
// All leaves constructed purely from unit cubes and unit triangles.
static void drawBamboo(Shader& shader, const mat4& model)
{
    shader.setInt("uUseTexture", 0); // vibrant golden-green bamboo poles and delicate foliage

    vec3 stalkCol(0.36f, 0.52f, 0.20f); // yellowish-green bamboo stalk
    vec3 nodeCol (0.24f, 0.36f, 0.14f); // dark bamboo culm node/joint
    vec3 leafCol (0.20f, 0.52f, 0.16f); // feathery lanceolate bamboo leaves
    vec3 tipCol  (0.28f, 0.60f, 0.18f); // fresh lime-sheen highlights

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

        // Feathery lanceolate bamboo leaves branching off upper 2 nodes
        for (int nd = numNodes - 2; nd <= numNodes - 1; nd++) {
            if (nd < 2) continue;
            float ny = (float)nd * 0.65f;
            for (int side = -1; side <= 1; side += 2) {
                mat4 spray = sm;
                spray = translate(spray, vec3(0.0f, ny, 0.0f));
                spray = rotate(spray, radians((float)side * 65.0f + (float)nd * 35.0f), vec3(0.0f, 1.0f, 0.0f));
                spray = rotate(spray, radians(42.0f), vec3(1.0f, 0.0f, 0.0f));

                // Slender branchlet twig (unit cube)
                mat4 twig = scale(spray, vec3(0.015f, 0.32f, 0.015f));
                Primitives::drawCube(shader, twig, stalkCol);

                // Fan spray of 3 delicate lanceolate leaves (unit cube + triangle)
                mat4 leafBase = spray;
                leafBase = translate(leafBase, vec3(0.0f, 0.32f, 0.0f));
                for (int lf = -1; lf <= 1; lf++) {
                    mat4 lM = leafBase;
                    lM = rotate(lM, radians((float)lf * 22.0f), vec3(0.0f, 0.0f, 1.0f));
                    drawLeaflet(shader, lM, 0.36f, 0.036f, 20.0f, (lf == 0) ? leafCol : tipCol);
                }
            }
        }

        // Crowning fountain spray of feathery bamboo leaves at the apex
        mat4 crown = sm;
        crown = translate(crown, vec3(0.0f, bs.h, 0.0f));
        const int numCrownLeaves = 7;
        for (int l = 0; l < numCrownLeaves; l++) {
            float lAngle = (float)l * (360.0f / (float)numCrownLeaves) + 12.0f;
            mat4 leaf = crown;
            leaf = rotate(leaf, radians(lAngle), vec3(0.0f, 1.0f, 0.0f));
            drawLeaflet(shader, leaf, 0.46f, 0.040f, 32.0f + (float)(l % 3) * 6.0f, (l % 2 == 0) ? leafCol : tipCol);
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
