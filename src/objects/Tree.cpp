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

// ── Highly Realistic Botanical Mango Leaf (Aam Pata / আম পাতা) ──────────
// Features:
// 1. Swollen pulvinus base + slender petiole stalk (বোঁটা) connecting blade to twig
// 2. V-profile blade (two angled side laminae meeting at the central midrib for specular reflection)
// 3. Realistic two-segment longitudinal gravitational droop (graceful downward arch)
// 4. Sharp acuminate tapering pointed tip (চোক্কা পাতার ডগা)
// 5. Distinct raised lighter yellow-green central midrib vein
// 6. Natural color options: mature deep glossy emerald vs coppery-amber young flush (নবকিশলয়)
inline void drawMangoLeaf(Shader& shader, const mat4& base, float length, float width, float droopAngle, const vec3& leafCol, bool isYoungFlush = false)
{
    vec3 midribCol = isYoungFlush ? vec3(0.58f, 0.38f, 0.14f) : vec3(0.38f, 0.58f, 0.16f);
    vec3 laminaLeft  = leafCol;
    vec3 laminaRight = leafCol * 0.88f; // subtle tone contrast across V-keel fold for 3D realism

    // 1. Petiole (leaf stalk / বোঁটা) with swollen pulvinus joint
    float petioleLen = length * 0.15f;
    mat4 pM = base;
    pM = translate(pM, vec3(0.0f, petioleLen * 0.5f, 0.0f));
    pM = scale(pM, vec3(0.016f, petioleLen, 0.016f));
    Primitives::drawCylinder(shader, pM, vec3(0.32f, 0.24f, 0.12f));

    mat4 pulvinus = base;
    pulvinus = scale(pulvinus, vec3(0.024f, 0.035f, 0.024f));
    Primitives::drawSphere(shader, pulvinus, vec3(0.28f, 0.20f, 0.10f));

    mat4 bladeBase = translate(base, vec3(0.0f, petioleLen, 0.0f));

    // 2. Proximal Blade Segment (arching outward with initial downward droop)
    float seg1Len = length * 0.48f;
    mat4 seg1 = bladeBase;
    seg1 = rotate(seg1, radians(droopAngle * 0.42f), vec3(1.0f, 0.0f, 0.0f));

    // Left half-blade (angled slightly for V-profile dihedral fold)
    mat4 leftLam1 = seg1;
    leftLam1 = translate(leftLam1, vec3(-width * 0.24f, seg1Len * 0.5f, 0.0f));
    leftLam1 = rotate(leftLam1, radians(-14.0f), vec3(0.0f, 1.0f, 0.0f));
    leftLam1 = scale(leftLam1, vec3(width * 0.48f, seg1Len, 0.005f));
    Primitives::drawCube(shader, leftLam1, laminaLeft);

    // Right half-blade
    mat4 rightLam1 = seg1;
    rightLam1 = translate(rightLam1, vec3(width * 0.24f, seg1Len * 0.5f, 0.0f));
    rightLam1 = rotate(rightLam1, radians(14.0f), vec3(0.0f, 1.0f, 0.0f));
    rightLam1 = scale(rightLam1, vec3(width * 0.48f, seg1Len, 0.005f));
    Primitives::drawCube(shader, rightLam1, laminaRight);

    // Central raised midrib vein (proximal)
    mat4 midrib1 = seg1;
    midrib1 = translate(midrib1, vec3(0.0f, seg1Len * 0.5f, 0.003f));
    midrib1 = scale(midrib1, vec3(0.012f, seg1Len, 0.008f));
    Primitives::drawCube(shader, midrib1, midribCol);

    // 3. Distal Blade Segment (graceful cascading downward droop following gravity)
    float seg2Len = length * 0.38f;
    mat4 seg2 = seg1;
    seg2 = translate(seg2, vec3(0.0f, seg1Len, 0.0f));
    seg2 = rotate(seg2, radians(droopAngle * 0.58f), vec3(1.0f, 0.0f, 0.0f));

    // Left half-blade (distal, tapering)
    mat4 leftLam2 = seg2;
    leftLam2 = translate(leftLam2, vec3(-width * 0.20f, seg2Len * 0.5f, 0.0f));
    leftLam2 = rotate(leftLam2, radians(-12.0f), vec3(0.0f, 1.0f, 0.0f));
    leftLam2 = scale(leftLam2, vec3(width * 0.40f, seg2Len, 0.005f));
    Primitives::drawCube(shader, leftLam2, laminaLeft);

    // Right half-blade (distal, tapering)
    mat4 rightLam2 = seg2;
    rightLam2 = translate(rightLam2, vec3(width * 0.20f, seg2Len * 0.5f, 0.0f));
    rightLam2 = rotate(rightLam2, radians(12.0f), vec3(0.0f, 1.0f, 0.0f));
    rightLam2 = scale(rightLam2, vec3(width * 0.40f, seg2Len, 0.005f));
    Primitives::drawCube(shader, rightLam2, laminaRight);

    // Central raised midrib vein (distal)
    mat4 midrib2 = seg2;
    midrib2 = translate(midrib2, vec3(0.0f, seg2Len * 0.5f, 0.003f));
    midrib2 = scale(midrib2, vec3(0.010f, seg2Len, 0.007f));
    Primitives::drawCube(shader, midrib2, midribCol);

    // 4. Sharp Acuminate Leaf Tip (চোক্কা পাতার ডগা)
    float tipLen = length * 0.16f;
    mat4 tipM = seg2;
    tipM = translate(tipM, vec3(0.0f, seg2Len, 0.0f));
    tipM = scale(tipM, vec3(width * 0.36f, tipLen, 1.0f));
    Primitives::drawTriangle(shader, tipM, leafCol * 1.08f);
}

// ── Whorled Terminal Rosette of Mango Leaves (আম পাতার থোকা / Pollob) ───
// Botanical arrangement: 10 drooping lanceolate leaves arranged in two spiraling
// tiers that hang downward naturally under gravity, forming an umbrella spray.
inline void drawMangoLeafCluster(Shader& shader, const mat4& twigBase, float clusterScale, const vec3& baseColor, bool hasYoungFlush = false)
{
    // Central terminal shoot twiglet
    mat4 twig = twigBase;
    twig = scale(twig, vec3(0.024f, 0.28f * clusterScale, 0.024f));
    Primitives::drawCylinder(shader, twig, vec3(0.34f, 0.24f, 0.12f));

    // Botanical arrangement: 10 leaves in 2 cascading tiers
    struct LeafInCluster {
        float yaw;
        float pitch;      // Base outward tilt away from branch axis
        float droop;      // Additional downward gravitational droop along blade
        float lengthMult;
        float widthMult;
    };

    static const LeafInCluster clusterLeaves[10] = {
        // Tier 1: Inner / Upper whorl (4 leaves, spreading outward and drooping)
        {   0.0f, 52.0f, 38.0f, 0.88f, 0.95f },
        {  90.0f, 48.0f, 42.0f, 0.84f, 0.92f },
        { 180.0f, 54.0f, 40.0f, 0.88f, 0.95f },
        { 270.0f, 50.0f, 44.0f, 0.84f, 0.92f },

        // Tier 2: Outer / Lower cascading whorl (6 mature leaves weeping steeply downward)
        {  25.0f, 74.0f, 52.0f, 1.05f, 1.00f },
        {  85.0f, 78.0f, 58.0f, 0.98f, 0.96f },
        { 145.0f, 72.0f, 50.0f, 1.08f, 1.02f },
        { 205.0f, 76.0f, 56.0f, 1.02f, 0.98f },
        { 265.0f, 80.0f, 60.0f, 1.00f, 0.96f },
        { 325.0f, 74.0f, 54.0f, 1.06f, 1.00f }
    };

    vec3 flushCol(0.48f, 0.24f, 0.12f); // tender coppery-bronze young shoot (নবকিশলয়)

    for (int i = 0; i < 10; i++) {
        const auto& lic = clusterLeaves[i];
        mat4 leafM = twigBase;
        float attachY = (i < 4 ? (0.16f + (float)i * 0.025f) : (0.06f + (float)(i - 4) * 0.020f)) * clusterScale;
        leafM = translate(leafM, vec3(0.0f, attachY, 0.0f));
        leafM = rotate(leafM, radians(lic.yaw), vec3(0.0f, 1.0f, 0.0f));
        leafM = rotate(leafM, radians(lic.pitch), vec3(1.0f, 0.0f, 0.0f));

        bool isFlush = (hasYoungFlush && (i == 0 || i == 1 || i == 4));
        vec3 col = isFlush ? flushCol : (baseColor * (0.90f + (float)(i % 4) * 0.07f));
        float leafLen = 0.72f * clusterScale * lic.lengthMult;
        float leafWid = 0.14f * clusterScale * lic.widthMult;

        drawMangoLeaf(shader, leafM, leafLen, leafWid, lic.droop, col, isFlush);
    }

    // Terminal apex bud / young shoot sprouts (মুকুল / নতুন কচি পাতার কুঁড়ি)
    mat4 bud = twigBase;
    bud = translate(bud, vec3(0.0f, 0.28f * clusterScale, 0.0f));
    bud = scale(bud, vec3(0.026f * clusterScale, 0.070f * clusterScale, 0.026f * clusterScale));
    Primitives::drawCone(shader, bud, hasYoungFlush ? flushCol : vec3(0.34f, 0.54f, 0.15f));

    // Two tiny emerging coppery leaflets at the apex bud
    if (hasYoungFlush) {
        for (int e = -1; e <= 1; e += 2) {
            mat4 emLeaf = twigBase;
            emLeaf = translate(emLeaf, vec3(0.0f, 0.27f * clusterScale, 0.0f));
            emLeaf = rotate(emLeaf, radians((float)e * 45.0f), vec3(0.0f, 1.0f, 0.0f));
            emLeaf = rotate(emLeaf, radians(32.0f), vec3(1.0f, 0.0f, 0.0f));
            drawMangoLeaf(shader, emLeaf, 0.26f * clusterScale, 0.06f * clusterScale, 28.0f, flushCol, true);
        }
    }
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

    static const vec3 trunkBase(0.42f, 0.52f, 0.22f); // pale lime succulent pseudostem
    static const vec3 trunkRing(0.32f, 0.40f, 0.16f); // leaf sheath overlap ring
    static const vec3 leafCol  (0.15f, 0.48f, 0.13f); // rich lush emerald banana leaf blade
    static const vec3 leafMid  (0.38f, 0.58f, 0.20f); // yellow-green central leaf midrib
    static const vec3 leafTip  (0.22f, 0.56f, 0.18f); // fresh bright leaf tip
    static const vec3 stalkCol (0.28f, 0.44f, 0.16f); // fruit bunch stalk
    static const vec3 fruitCol (0.32f, 0.52f, 0.16f); // green baby bananas (Kolar Fona)
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

// ─── 3. Majestic Branching Mango Tree (Aam Gach / আম গাছ) ─────────────
// Modeled with sturdy gnarled trunk, buttress root flares, 4 radiating primary limbs,
// secondary woody branchlets, and a 100% pure botanical foliage canopy constructed
// from layered cascading rosettes of authentic lanceolate mango leaves (Aam Pata).
// Zero smooth spherical balloon surfaces! Every leaf features raised central midrib veins,
// 3D dihedral V-profile folds, acuminate tips, tender coppery-red young flushes (নবকিশলয়),
// and golden ripe Bengali mango fruits (Paka Aam) dangling beneath the leaves.
static void drawGeneral(Shader& shader, const mat4& model)
{
    shader.setInt("uUseTexture", 0); // vibrant procedural canopy with authentic botanical mango foliage

    vec3 trunkCol (0.36f, 0.26f, 0.16f); // gnarled woody bark
    vec3 barkDark (0.24f, 0.16f, 0.08f); // dark bark crevices

    // Botanical mango foliage palette
    vec3 leafSunlit  (0.24f, 0.54f, 0.16f); // sunlit golden-green apex leaves
    vec3 leafLush    (0.16f, 0.46f, 0.12f); // rich mature emerald mango leaves
    vec3 leafDeep    (0.11f, 0.38f, 0.10f); // mid-canopy deep forest green
    vec3 leafDark    (0.06f, 0.24f, 0.07f); // shaded inner/lower foliage green
    vec3 stemCol     (0.36f, 0.26f, 0.14f); // woody fruit stalk

    // ── 1. Sturdy Gnarled Trunk ──────────────────────────────────
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
        root = scale(root, vec3(0.16f, 0.85f, 0.14f));
        Primitives::drawCube(shader, root, barkDark);
    }

    // ── 2. Primary Spreading Limbs & Secondary Woody Boughs ──────
    float branchAngles[] = { 18.0f, 108.0f, 198.0f, 288.0f };
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
        subB = translate(subB, vec3(0.0f, 0.70f, 0.0f));
        subB = rotate(subB, radians(28.0f), vec3(0.0f, 0.0f, 1.0f));
        subB = translate(subB, vec3(0.0f, 0.45f, 0.0f));
        mat4 subBM = scale(subB, vec3(0.11f, 0.95f, 0.11f));
        Primitives::drawCylinder(shader, subBM, trunkCol);

        // Tertiary branchlet twig
        mat4 subB2 = branch;
        subB2 = translate(subB2, vec3(0.0f, 0.95f, 0.0f));
        subB2 = rotate(subB2, radians(-32.0f), vec3(0.0f, 0.0f, 1.0f));
        subB2 = translate(subB2, vec3(0.0f, 0.38f, 0.0f));
        mat4 subB2M = scale(subB2, vec3(0.07f, 0.80f, 0.07f));
        Primitives::drawCylinder(shader, subB2M, trunkCol);
    }

    // ── 3. 100% Botanical Foliage Canopy: Layered Rosettes of Mango Leaves ──
    // Zero smooth spheres! The canopy is built purely out of cascading leaf rosettes
    // (Aam Pata) organized into 5 natural anatomical zones:
    // A: Interior Shadow Core (replaces spheres with deep-shaded inner leaves)
    // B: Apex Crown (sunlit golden-green with coppery young shoots)
    // C: High Spreading Boughs
    // D: Mid-Canopy Full Foliage Terraces
    // E: Lower Drooping Perimeter Skirt
    struct FoliageCluster {
        float x, y, z;
        float pitch, yaw, roll;
        float scale;
        vec3 color;
        bool isYoungFlush;
    };

    static const FoliageCluster clusters[] = {
        // ── A. Interior Density Core (Deep shaded green leaves nestled inside branch forks) ──
        {  0.00f, 4.30f,  0.00f,  15.0f,   0.0f,   0.0f, 1.35f, leafDark, false },
        {  0.55f, 4.05f,  0.50f,  20.0f,  45.0f,  10.0f, 1.25f, leafDark, false },
        { -0.55f, 4.00f, -0.50f,  18.0f, 225.0f, -10.0f, 1.25f, leafDark, false },
        { -0.50f, 3.95f,  0.55f,  22.0f, 135.0f,   8.0f, 1.25f, leafDark, false },
        {  0.50f, 4.00f, -0.55f,  18.0f, 315.0f,  -8.0f, 1.25f, leafDark, false },
        {  0.00f, 4.60f,  0.55f,  25.0f,  90.0f,   0.0f, 1.28f, leafDeep, false },
        {  0.00f, 4.55f, -0.55f,  25.0f, 270.0f,   0.0f, 1.28f, leafDeep, false },

        // ── B. Crown & Apex Tier (Sunlit golden-green & coppery young shoots) ──
        {  0.00f, 6.10f,  0.00f,  10.0f,   0.0f,   0.0f, 1.20f, leafSunlit, true  },
        {  0.55f, 5.75f,  0.35f,  28.0f,  30.0f,  12.0f, 1.15f, leafSunlit, false },
        { -0.55f, 5.70f, -0.35f,  26.0f, 210.0f, -12.0f, 1.15f, leafSunlit, true  },
        { -0.45f, 5.80f,  0.45f,  30.0f, 135.0f,  10.0f, 1.12f, leafSunlit, false },
        {  0.45f, 5.65f, -0.45f,  28.0f, 315.0f, -10.0f, 1.12f, leafSunlit, false },
        {  0.00f, 5.85f,  0.75f,  32.0f,  85.0f,   0.0f, 1.16f, leafSunlit, false },
        {  0.00f, 5.80f, -0.75f,  30.0f, 265.0f,   0.0f, 1.16f, leafSunlit, true  },

        // ── C. Upper Canopy Spreading Boughs (Lush mature emerald mango leaves) ──
        {  1.65f, 5.10f,  0.25f,  38.0f,  15.0f,  15.0f, 1.22f, leafLush,   false },
        {  1.35f, 4.90f, -1.20f,  36.0f, 310.0f, -14.0f, 1.18f, leafLush,   false },
        {  1.20f, 5.00f,  1.25f,  38.0f,  75.0f,  16.0f, 1.20f, leafSunlit, true  },
        {  0.20f, 5.15f,  1.75f,  40.0f,  90.0f,   8.0f, 1.22f, leafLush,   false },
        { -1.20f, 4.90f,  1.30f,  37.0f, 150.0f, -15.0f, 1.18f, leafLush,   false },
        { -1.70f, 5.05f,  0.20f,  38.0f, 195.0f, -12.0f, 1.22f, leafLush,   true  },
        { -1.30f, 4.85f, -1.25f,  36.0f, 240.0f,  14.0f, 1.18f, leafDeep,   false },
        {  0.18f, 5.10f, -1.70f,  38.0f, 280.0f,  -8.0f, 1.20f, leafLush,   false },
        {  2.05f, 4.80f, -0.50f,  42.0f, 340.0f,  18.0f, 1.15f, leafLush,   false },
        {  0.75f, 5.25f,  1.65f,  40.0f,  65.0f,  12.0f, 1.18f, leafSunlit, false },
        { -1.95f, 4.75f, -0.65f,  42.0f, 220.0f, -16.0f, 1.15f, leafDeep,   false },
        { -0.55f, 5.20f, -1.65f,  40.0f, 255.0f, -10.0f, 1.18f, leafLush,   false },

        // ── D. Mid-Canopy Spreading Terrace (Dense full foliage canopy) ──
        {  2.45f, 4.30f,  0.20f,  46.0f,  10.0f,  20.0f, 1.25f, leafLush,   false },
        {  1.90f, 4.15f, -1.55f,  44.0f, 305.0f, -18.0f, 1.20f, leafDeep,   false },
        {  1.80f, 4.20f,  1.65f,  46.0f,  70.0f,  18.0f, 1.22f, leafLush,   true  },
        {  0.30f, 4.35f,  2.40f,  48.0f,  90.0f,  10.0f, 1.26f, leafLush,   false },
        { -1.75f, 4.10f,  1.85f,  44.0f, 145.0f, -18.0f, 1.20f, leafDeep,   false },
        { -2.45f, 4.25f,  0.25f,  46.0f, 190.0f, -16.0f, 1.25f, leafLush,   false },
        { -1.85f, 4.05f, -1.65f,  44.0f, 235.0f,  18.0f, 1.20f, leafDeep,   false },
        {  0.20f, 4.30f, -2.35f,  48.0f, 275.0f, -10.0f, 1.25f, leafDeep,   false },
        {  2.60f, 3.90f,  0.85f,  52.0f,  35.0f,  22.0f, 1.18f, leafDeep,   false },
        {  1.05f, 4.00f,  2.45f,  50.0f,  80.0f,  14.0f, 1.22f, leafLush,   false },
        { -1.05f, 3.95f,  2.40f,  50.0f, 115.0f, -14.0f, 1.22f, leafLush,   false },
        { -2.55f, 3.90f, -0.75f,  52.0f, 215.0f, -20.0f, 1.18f, leafDeep,   false },
        { -0.95f, 4.05f, -2.30f,  50.0f, 260.0f, -12.0f, 1.20f, leafDeep,   false },

        // ── E. Lower Drooping Canopy Skirts (Graceful downward cascading leaves) ──
        {  2.30f, 3.35f,  0.30f,  58.0f,  15.0f,  24.0f, 1.22f, leafDark,   false },
        {  1.70f, 3.20f, -1.40f,  56.0f, 310.0f, -22.0f, 1.16f, leafDark,   false },
        {  1.60f, 3.25f,  1.50f,  58.0f,  65.0f,  20.0f, 1.18f, leafDark,   false },
        {  0.25f, 3.40f,  2.25f,  60.0f,  90.0f,  12.0f, 1.24f, leafDark,   false },
        { -1.60f, 3.15f,  1.70f,  56.0f, 140.0f, -20.0f, 1.18f, leafDark,   false },
        { -2.30f, 3.30f,  0.30f,  58.0f, 195.0f, -22.0f, 1.22f, leafDark,   false },
        { -1.70f, 3.10f, -1.50f,  56.0f, 240.0f,  22.0f, 1.16f, leafDark,   false },
        {  0.18f, 3.35f, -2.20f,  60.0f, 280.0f, -12.0f, 1.22f, leafDark,   false },
        {  2.00f, 3.05f,  1.05f,  62.0f,  40.0f,  25.0f, 1.14f, leafDark,   false },
        {  0.85f, 3.10f,  2.05f,  60.0f,  85.0f,  15.0f, 1.16f, leafDark,   false },
        { -2.10f, 3.00f, -0.60f,  62.0f, 210.0f, -24.0f, 1.14f, leafDark,   false },
        {  0.75f, 3.15f, -1.95f,  60.0f, 275.0f, -14.0f, 1.16f, leafDark,   false }
    };

    for (const auto& fc : clusters) {
        mat4 cM = model;
        cM = translate(cM, vec3(fc.x, fc.y, fc.z));
        cM = rotate(cM, radians(fc.yaw), vec3(0.0f, 1.0f, 0.0f));
        cM = rotate(cM, radians(fc.pitch), vec3(1.0f, 0.0f, 0.0f));
        cM = rotate(cM, radians(fc.roll), vec3(0.0f, 0.0f, 1.0f));
        drawMangoLeafCluster(shader, cM, fc.scale, fc.color, fc.isYoungFlush);
    }

    // ── 4. Golden Ripe Mangoes (Paka Aam / পাকা আম) Dangling Beneath Foliage ──
    struct MangoPos { float x, y, z; float stemH; vec3 col; };
    static const MangoPos mangoes[] = {
        // Front-facing clusters
        {  0.55f, 2.65f,  2.60f, 0.28f, vec3(0.98f, 0.70f, 0.12f) },
        {  0.80f, 2.55f,  2.45f, 0.36f, vec3(0.92f, 0.48f, 0.10f) }, // twin pair
        { -0.38f, 2.70f,  2.50f, 0.26f, vec3(0.98f, 0.74f, 0.14f) },
        {  1.45f, 2.65f,  2.00f, 0.30f, vec3(0.94f, 0.58f, 0.12f) },
        { -1.25f, 2.72f,  1.75f, 0.28f, vec3(0.96f, 0.66f, 0.12f) },
        // Side and rear clusters
        {  2.05f, 2.78f,  0.65f, 0.30f, vec3(0.96f, 0.68f, 0.12f) },
        {  2.20f, 2.65f,  0.45f, 0.38f, vec3(0.92f, 0.52f, 0.12f) }, // twin pair
        { -1.85f, 2.80f,  0.45f, 0.28f, vec3(0.95f, 0.62f, 0.14f) },
        { -1.55f, 2.75f, -0.95f, 0.30f, vec3(0.96f, 0.66f, 0.12f) },
        {  1.20f, 2.70f, -1.55f, 0.32f, vec3(0.94f, 0.56f, 0.12f) }
    };

    vec3 leafAccent(0.16f, 0.44f, 0.12f);
    for (const auto& mp : mangoes) {
        // Slender curved pedicel connecting up to the foliage branch
        mat4 stem = model;
        stem = translate(stem, vec3(mp.x, mp.y + mp.stemH * 0.5f, mp.z));
        stem = scale(stem, vec3(0.016f, mp.stemH, 0.016f));
        Primitives::drawCylinder(shader, stem, stemCol);

        // Dark green mango leaf attached to stem joint
        mat4 mLeaf = model;
        mLeaf = translate(mLeaf, vec3(mp.x + 0.038f, mp.y + mp.stemH * 0.82f, mp.z));
        drawMangoLeaf(shader, mLeaf, 0.32f, 0.075f, 38.0f, leafAccent);

        // Distinctive ripe Bengali mango fruit (kidney/ovoid shape with slight asymmetry)
        mat4 aam = model;
        aam = translate(aam, vec3(mp.x, mp.y, mp.z));
        aam = rotate(aam, radians(14.0f), vec3(0.0f, 0.0f, 1.0f));
        aam = scale(aam, vec3(0.12f, 0.18f, 0.11f));
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
