// Mosque.cpp — Historic Terracotta Brick & Old Red Stone Mosque (সুলতানি আমলের লাল ইট ও প্রাচীন লাল পাথরের ঐতিহ্যবাহী গ্রামীণ মসজিদ)
// Authentic Bengal Sultanate & Mughal terracotta brick architecture:
// 1. Weathered red terracotta brick walls (পোড়ামাটির লাল ইট) with carved relief moldings & kiln variations
// 2. Majestic Central Weathered Red Terracotta Dome (পোড়ামাটির লাল গম্বুজ) with antique bronze Crescent Moon & Star (চাঁদ-তারা)
// 3. Soaring tall Red Brick Azaan Minaret (আজানের মিনার) with 4 Quad Horn Loudspeakers (চারটি চোঙা মাইক)
// 4. Three decorative corner minaret pinnacles (Guldasta) capped with terracotta cupolas & bronze spires
// 5. Front Veranda / Colonnade (মসজিদের বারান্দা) with multi-cusped terracotta arches & shoe shelf
// 6. Stepped terracotta roof parapet merlons / crenellations (কাঙ্গুরা / Kangura) along the roofline
// 7. Arched seasoned dark timber double doors with antique studs & warm golden interior prayer glow
// 8. Arched Islamic windows with terracotta jali screens & radiant interior golden candlelight glow
// 9. Projecting western Mehrab bay on the rear Qibla wall with terracotta domelet
// 10. Traditional red brick masonry ablution area (পাকা ওজুখানা ও হাউজ) with stone bench, brass taps, and handcrafted Bodnas
// Procedurally constructed in OpenGL 3.3 Core Profile using canonical geometric primitives.

#include "objects/Mosque.h"
#include "objects/Charpai.h"
#include "Primitives.h"
#include <cmath>

using namespace math;

namespace Mosque {

// Helper 1: Islamic Crescent Moon & Five-Pointed Star Finial (চাঁদ ও তারা / Chand-Tara)
static void drawChandTara(Shader& shader, const mat4& parentModel, const vec3& pos, float scaleVal, const vec3& goldCol)
{
    shader.setInt("uUseTexture", 0);
    mat4 baseM = parentModel;
    baseM = translate(baseM, pos);
    baseM = scale(baseM, vec3(scaleVal));

    // Base lotus plate / collar
    mat4 collar = baseM;
    collar = translate(collar, vec3(0.0f, 0.035f, 0.0f));
    collar = scale(collar, vec3(0.09f, 0.04f, 0.09f));
    Primitives::drawCylinder(shader, collar, goldCol);

    // Spire shaft
    mat4 spire = baseM;
    spire = translate(spire, vec3(0.0f, 0.15f, 0.0f));
    spire = scale(spire, vec3(0.028f, 0.22f, 0.028f));
    Primitives::drawCylinder(shader, spire, goldCol);

    // Center spherical bead
    mat4 orb = baseM;
    orb = translate(orb, vec3(0.0f, 0.27f, 0.0f));
    orb = scale(orb, vec3(0.058f, 0.058f, 0.058f));
    Primitives::drawSphere(shader, orb, goldCol);

    // 3D Crescent Moon (Hilal) standing upright facing +Z
    const float moonR = 0.165f;
    const float moonCenterY = 0.45f;
    const int numSegs = 15;
    for (int i = 0; i < numSegs; ++i) {
        float t = (float)i / (float)(numSegs - 1);
        float angle = radians(-105.0f + t * 210.0f);
        float cx = -sinf(angle) * moonR;
        float cy = moonCenterY + cosf(angle) * moonR;

        // Thickness varies: widest at center belly, tapering sharply to tips
        float thick = 0.026f * cosf(radians(-90.0f + t * 180.0f));
        if (thick < 0.006f) thick = 0.006f;

        mat4 mSeg = baseM;
        mSeg = translate(mSeg, vec3(cx, cy, 0.0f));
        mSeg = scale(mSeg, vec3(thick, thick * 1.12f, 0.018f));
        Primitives::drawSphere(shader, mSeg, goldCol);
    }

    // Five-pointed Star inside the crescent curve (at +X from crescent center)
    const float starX = 0.055f;
    const float starY = moonCenterY;
    mat4 starCenter = baseM;
    starCenter = translate(starCenter, vec3(starX, starY, 0.0f));
    starCenter = scale(starCenter, vec3(0.034f, 0.034f, 0.016f));
    Primitives::drawSphere(shader, starCenter, goldCol);

    // 5 pointed star rays
    for (int p = 0; p < 5; ++p) {
        float pAngle = (float)p * 72.0f;
        mat4 ray = baseM;
        ray = translate(ray, vec3(starX, starY, 0.0f));
        ray = rotate(ray, radians(pAngle), vec3(0.0f, 0.0f, 1.0f));
        ray = translate(ray, vec3(0.0f, 0.038f, 0.0f));
        ray = scale(ray, vec3(0.016f, 0.042f, 0.012f));
        Primitives::drawCone(shader, ray, goldCol);
    }
}

// Helper 2: Traditional Village Azaan Horn Loudspeaker (চোঙা মাইক)
static void drawLoudspeaker(Shader& shader, const mat4& parentModel, const vec3& pos, float yawDeg)
{
    shader.setInt("uUseTexture", 0);
    const vec3 hornMetal (0.80f, 0.83f, 0.86f); // light grey-white industrial acoustic metal
    const vec3 hornDark  (0.24f, 0.25f, 0.28f); // dark magnetic driver housing & mount
    const vec3 hornTrim  (0.35f, 0.38f, 0.42f); // rim clamp band

    mat4 sm = parentModel;
    sm = translate(sm, pos);
    sm = rotate(sm, radians(yawDeg), vec3(0.0f, 1.0f, 0.0f));

    // Steel bracket arm extending outward from the minaret gallery
    mat4 arm = sm;
    arm = translate(arm, vec3(0.0f, 0.0f, 0.18f));
    arm = scale(arm, vec3(0.024f, 0.024f, 0.36f));
    Primitives::drawCube(shader, arm, hornDark);

    // Swivel mount clamp
    mat4 clamp = sm;
    clamp = translate(clamp, vec3(0.0f, 0.0f, 0.34f));
    clamp = scale(clamp, vec3(0.06f, 0.07f, 0.05f));
    Primitives::drawCube(shader, clamp, hornDark);

    // Driver unit (heavy rear compression magnet housing)
    mat4 driver = sm;
    driver = translate(driver, vec3(0.0f, 0.0f, 0.40f));
    driver = rotate(driver, radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
    driver = scale(driver, vec3(0.065f, 0.09f, 0.065f));
    Primitives::drawCylinder(shader, driver, hornDark);

    // Throat adapter
    mat4 throat = sm;
    throat = translate(throat, vec3(0.0f, 0.0f, 0.48f));
    throat = rotate(throat, radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
    throat = scale(throat, vec3(0.055f, 0.08f, 0.055f));
    Primitives::drawCone(shader, throat, hornMetal);

    // Flared acoustic bell horn (classic megaphone flare)
    mat4 bell = sm;
    bell = translate(bell, vec3(0.0f, 0.0f, 0.54f));
    bell = rotate(bell, radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
    bell = scale(bell, vec3(0.14f, 0.18f, 0.14f));
    Primitives::drawCone(shader, bell, hornMetal);

    // Flared bell mouth rim band
    mat4 rim = sm;
    rim = translate(rim, vec3(0.0f, 0.0f, 0.72f));
    rim = rotate(rim, radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
    rim = scale(rim, vec3(0.142f, 0.015f, 0.142f));
    Primitives::drawCylinder(shader, rim, hornTrim);

    // Center acoustic phasing bullet (re-entrant horn center plug)
    mat4 plug = sm;
    plug = translate(plug, vec3(0.0f, 0.0f, 0.65f));
    plug = rotate(plug, radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
    plug = scale(plug, vec3(0.038f, 0.07f, 0.038f));
    Primitives::drawCone(shader, plug, hornDark);
}

// Helper 3: Traditional Handcrafted Clay/Plastic Ablution Ewer (বদনা / Bodna)
static void drawBodna(Shader& shader, const mat4& parentModel, const vec3& pos, float scaleVal, const vec3& ewerCol)
{
    shader.setInt("uUseTexture", 0);
    mat4 bm = parentModel;
    bm = translate(bm, pos);
    bm = scale(bm, vec3(scaleVal));

    // Bulbous water reservoir belly
    mat4 body = bm;
    body = translate(body, vec3(0.0f, 0.10f, 0.0f));
    body = scale(body, vec3(0.12f, 0.10f, 0.12f));
    Primitives::drawSphere(shader, body, ewerCol);

    // Base pedestal
    mat4 base = bm;
    base = translate(base, vec3(0.0f, 0.015f, 0.0f));
    base = scale(base, vec3(0.075f, 0.03f, 0.075f));
    Primitives::drawCylinder(shader, base, ewerCol);

    // Narrow neck
    mat4 neck = bm;
    neck = translate(neck, vec3(0.0f, 0.18f, 0.0f));
    neck = scale(neck, vec3(0.045f, 0.06f, 0.045f));
    Primitives::drawCylinder(shader, neck, ewerCol);

    // Flared mouth rim
    mat4 rim = bm;
    rim = translate(rim, vec3(0.0f, 0.22f, 0.0f));
    rim = scale(rim, vec3(0.068f, 0.02f, 0.068f));
    Primitives::drawCylinder(shader, rim, ewerCol);

    // Characteristic long curved angled spout
    mat4 spout = bm;
    spout = translate(spout, vec3(0.08f, 0.14f, 0.0f));
    spout = rotate(spout, radians(-45.0f), vec3(0.0f, 0.0f, 1.0f));
    spout = scale(spout, vec3(0.026f, 0.12f, 0.026f));
    Primitives::drawCone(shader, spout, ewerCol);

    // Arched carry handle
    mat4 handle = bm;
    handle = translate(handle, vec3(-0.08f, 0.15f, 0.0f));
    handle = scale(handle, vec3(0.022f, 0.10f, 0.022f));
    Primitives::drawCube(shader, handle, ewerCol);
}

// Helper 4: Stepped Islamic Roof Merlons / Crenellations (কাঙ্গুরা / Kangura)
static void drawKanguraRow(Shader& shader, const mat4& model, float startX, float endX, float yPos, float zPos, float stepSize, const vec3& baseCol, const vec3& peakCol)
{
    float span = endX - startX;
    int count = (int)(span / stepSize);
    for (int i = 0; i < count; ++i) {
        float x = startX + ((float)i + 0.5f) * stepSize;
        // Lower terracotta crenellation base
        mat4 mLower = model;
        mLower = translate(mLower, vec3(x, yPos + 0.06f, zPos));
        mLower = scale(mLower, vec3(stepSize * 0.70f, 0.12f, 0.06f));
        Primitives::drawCube(shader, mLower, baseCol);

        // Upper stepped terracotta peak
        mat4 mUpper = model;
        mUpper = translate(mUpper, vec3(x, yPos + 0.15f, zPos));
        mUpper = scale(mUpper, vec3(stepSize * 0.38f, 0.08f, 0.06f));
        Primitives::drawCube(shader, mUpper, peakCol);
    }
}

// Helper 5: Organic Living Moss Cushion & Biological Weathering Patch (মখমলে সবুজ শ্যাওলা)
static void drawMossCushion(Shader& shader, const mat4& parentModel, const vec3& pos, const vec3& dims, float rotYDeg, const vec3& col)
{
    shader.setInt("uUseTexture", 0);
    mat4 m = parentModel;
    m = translate(m, pos);
    if (std::abs(rotYDeg) > 0.01f) {
        m = rotate(m, radians(rotYDeg), vec3(0.0f, 1.0f, 0.0f));
    }
    // Base low-profile organic cushion
    mat4 mCushion = m;
    mCushion = scale(mCushion, dims);
    Primitives::drawCube(shader, mCushion, col);

    // Overlapping softer sub-lobe for organic moss velvety irregularity
    mat4 mLobe = m;
    mLobe = translate(mLobe, vec3(dims.x * 0.20f, dims.y * 0.18f, dims.z * 0.14f));
    mLobe = scale(mLobe, vec3(dims.x * 0.65f, dims.y * 0.85f, dims.z * 0.65f));
    Primitives::drawSphere(shader, mLobe, col * 0.88f);
}

// Helper 6: Vertical Rainwater Algae / Slime Drip Streak (বৃষ্টির জলের ছোপ ও কালচে শেওলার রেখা)
static void drawAlgaeDrip(Shader& shader, const mat4& parentModel, const vec3& topPos, float length, float width, const vec3& col)
{
    shader.setInt("uUseTexture", 0);
    // Vertical moisture streak strip
    mat4 m = parentModel;
    m = translate(m, topPos - vec3(0.0f, length * 0.5f, 0.0f));
    m = scale(m, vec3(width, length, 0.022f));
    Primitives::drawCube(shader, m, col);

    // Lower tapered condensation drip droplet
    mat4 mTip = parentModel;
    mTip = translate(mTip, topPos - vec3(0.0f, length + width * 0.5f, 0.0f));
    mTip = scale(mTip, vec3(width * 0.55f, width * 1.1f, 0.022f));
    Primitives::drawCone(shader, mTip, col * 0.76f);
}

// Helper 7: Horizontal Algae & Slime Damp Perimeter Band (ভিটি ও দেওয়ালের স্যাঁতসেঁতে স্তর)
static void drawSlimeBand(Shader& shader, const mat4& parentModel, const vec3& center, const vec3& size, const vec3& col)
{
    shader.setInt("uUseTexture", 0);
    mat4 m = parentModel;
    m = translate(m, center);
    m = scale(m, size);
    Primitives::drawCube(shader, m, col);
}

void draw(Shader& shader, const mat4& model)
{
    // ── Historic Bengal Terracotta Brick & Old Red Stone Mosque Palette ──
    // Inspired by iconic 15th-century Bengal Sultanate architecture (Shat Gombuj, Goaldi, Bagha, Atia):
    const vec3 terracottaBrick (0.68f, 0.28f, 0.17f); // Warm weathered red terracotta brick (পোড়ামাটির লাল ইট)
    const vec3 burntBrickDark  (0.38f, 0.14f, 0.08f); // Deep burnt kiln-fired brick / recessed relief shadows
    const vec3 ochreBrickLight (0.76f, 0.38f, 0.20f); // Sun-baked warm terracotta brick highlights
    const vec3 oldRedStone     (0.56f, 0.28f, 0.22f); // Ancient carved red sandstone plinth & heavy piers
    const vec3 agedStoneBase   (0.38f, 0.18f, 0.14f); // Weathered dark red foundation stone blocks
    const vec3 terracottaTrim  (0.70f, 0.32f, 0.20f); // Carved terracotta floral rosettes & relief friezes
    const vec3 antiqueBronze   (0.82f, 0.65f, 0.28f); // Weathered antique brass / bronze finials (Chand-Tara)
    const vec3 darkSalTimber   (0.24f, 0.13f, 0.07f); // Seasoned dark ancient Sal timber doors & frames
    const vec3 interiorGlow    (0.98f, 0.78f, 0.28f); // Radiant warm golden candlelight / lantern prayer glow
    const vec3 waterCol        (0.12f, 0.54f, 0.60f); // Tranquil blue-green ablution cistern water
    const vec3 bodnaRed        (0.78f, 0.22f, 0.16f); // Traditional crimson clay-glazed ewer
    const vec3 bodnaClay       (0.74f, 0.38f, 0.20f); // Handcrafted natural terracotta Bodna
    const vec3 bodnaGreen      (0.16f, 0.42f, 0.25f); // Deep ceramic green Bodna

    // ── Biological Weathering Palette (Moss, Algae, Slime & Lichen) ───────
    const vec3 mossVelvet   (0.24f, 0.38f, 0.16f); // Living velvety green moss (মখমলে সবুজ শ্যাওলা)
    const vec3 mossDark     (0.14f, 0.25f, 0.09f); // Deep damp shadow moss (স্যাঁতসেঁতে কালচে শ্যাওলা)
    const vec3 algaeSlime   (0.10f, 0.20f, 0.08f); // Slippery dark green algae/slime (পিচ্ছিল শেওলা ও জলজ স্তর)
    const vec3 moistureDrip (0.19f, 0.11f, 0.07f); // Rainwater moisture stain / damp streak
    const vec3 lichenDry    (0.36f, 0.44f, 0.28f); // Ancient crusty dry lichen (শুকনো ছাতাপড়া শেওলা)
    const vec3 slimeWet     (0.07f, 0.15f, 0.06f); // Dripping wet algae slime for water-dripping zones

    const int activeTexMode = shader.getInt("uUseTexture");

    // Primary Dimensions
    const float hallW   = 5.6f;   // width along X
    const float hallH   = 3.3f;   // wall height
    const float hallD   = 5.8f;   // depth along Z
    const float plinthH = 0.45f;  // raised foundation plinth height
    const float halfW   = hallW * 0.5f; // 2.80f
    const float halfD   = hallD * 0.5f; // 2.90f

    // ── 1. Raised Foundation Plinth & Stepped Flight (প্রাচীন লাল পাথরের পাকা ভিটি ও সিঁড়ি) ──
    shader.setInt("uUseTexture", 0);
    // Stepped raised foundation platform in ancient dressed red stone
    mat4 plinth = model;
    plinth = translate(plinth, vec3(0.0f, plinthH * 0.5f, 0.0f));
    plinth = scale(plinth, vec3(hallW + 2.0f, plinthH, hallD + 2.2f));
    Primitives::drawCube(shader, plinth, oldRedStone);

    // Weathered dark red stone perimeter border
    mat4 plinthBorder = model;
    plinthBorder = translate(plinthBorder, vec3(0.0f, plinthH + 0.015f, 0.0f));
    plinthBorder = scale(plinthBorder, vec3(hallW + 2.05f, 0.03f, hallD + 2.25f));
    Primitives::drawCube(shader, plinthBorder, burntBrickDark);

    // ── Plinth Ground-Contact Damp Moss Skirts & Slime Bands (ভিটির স্যাঁতসেঁতে শ্যাওলা ও শেওলার স্তর) ──
    // Front (+Z) soil-contact damp algae slime band
    drawSlimeBand(shader, model, vec3(0.0f, 0.06f, halfD + 1.11f), vec3(hallW + 1.94f, 0.12f, 0.03f), algaeSlime);
    // Shaded Rear (-Z) heavier damp moss & slime band
    drawSlimeBand(shader, model, vec3(0.0f, 0.08f, -halfD - 1.11f), vec3(hallW + 1.94f, 0.16f, 0.03f), mossDark);
    // West (-X) and East (+X) flanks
    drawSlimeBand(shader, model, vec3(-halfW - 1.01f, 0.06f, 0.0f), vec3(0.03f, 0.12f, hallD + 2.14f), algaeSlime);
    drawSlimeBand(shader, model, vec3( halfW + 1.01f, 0.06f, 0.0f), vec3(0.03f, 0.12f, hallD + 2.14f), algaeSlime);

    // 4 Corner Creeping Moss Clusters wrapping around ancient dressed stone plinth
    drawMossCushion(shader, model, vec3( halfW + 0.96f, 0.09f,  halfD + 1.06f), vec3(0.38f, 0.16f, 0.36f),  25.0f, mossVelvet);
    drawMossCushion(shader, model, vec3(-halfW - 0.96f, 0.10f,  halfD + 1.06f), vec3(0.42f, 0.18f, 0.40f), -35.0f, mossDark);
    drawMossCushion(shader, model, vec3( halfW + 0.96f, 0.11f, -halfD - 1.06f), vec3(0.44f, 0.20f, 0.42f),  40.0f, mossVelvet);
    drawMossCushion(shader, model, vec3(-halfW - 0.96f, 0.12f, -halfD - 1.06f), vec3(0.48f, 0.22f, 0.45f), -15.0f, mossDark);

    // Intermediate plinth perimeter moss cushions
    drawMossCushion(shader, model, vec3(-2.6f, 0.08f,  halfD + 1.11f), vec3(0.32f, 0.14f, 0.16f), 0.0f, mossVelvet);
    drawMossCushion(shader, model, vec3( 2.6f, 0.07f,  halfD + 1.11f), vec3(0.30f, 0.13f, 0.15f), 0.0f, lichenDry);
    drawMossCushion(shader, model, vec3(-2.4f, 0.10f, -halfD - 1.11f), vec3(0.36f, 0.18f, 0.18f), 0.0f, mossDark);
    drawMossCushion(shader, model, vec3( 2.4f, 0.11f, -halfD - 1.11f), vec3(0.38f, 0.19f, 0.19f), 0.0f, mossVelvet);
    drawMossCushion(shader, model, vec3(-halfW - 1.01f, 0.08f, -1.5f), vec3(0.18f, 0.15f, 0.35f), 0.0f, mossDark);
    drawMossCushion(shader, model, vec3(-halfW - 1.01f, 0.09f,  1.5f), vec3(0.16f, 0.16f, 0.32f), 0.0f, mossVelvet);
    drawMossCushion(shader, model, vec3( halfW + 1.01f, 0.08f, -1.5f), vec3(0.17f, 0.14f, 0.33f), 0.0f, algaeSlime);
    drawMossCushion(shader, model, vec3( halfW + 1.01f, 0.09f,  1.5f), vec3(0.19f, 0.16f, 0.36f), 0.0f, mossVelvet);

    // Front Stepped Flight (4 wide terracotta brick and red stone stairs leading up to entrance veranda at +Z)
    for (int s = 0; s < 4; ++s) {
        float sH = plinthH * (1.0f - (float)s * 0.22f);
        float sW = 2.8f + (float)s * 0.32f;
        float sZ = halfD + 1.25f + (float)s * 0.30f;

        mat4 step = model;
        step = translate(step, vec3(0.0f, sH * 0.5f, sZ));
        step = scale(step, vec3(sW, sH, 0.32f));
        Primitives::drawCube(shader, step, (s % 2 == 0) ? oldRedStone : burntBrickDark);
    }

    // Step Edge Crevices: Moss cushions nestling in outer stone corners where worshippers do not step
    drawMossCushion(shader, model, vec3(-1.38f, 0.32f, halfD + 1.28f), vec3(0.24f, 0.07f, 0.16f),  10.0f, mossVelvet);
    drawMossCushion(shader, model, vec3( 1.38f, 0.30f, halfD + 1.28f), vec3(0.22f, 0.07f, 0.16f), -12.0f, mossDark);
    drawMossCushion(shader, model, vec3(-1.54f, 0.22f, halfD + 1.58f), vec3(0.26f, 0.07f, 0.18f),  15.0f, algaeSlime);
    drawMossCushion(shader, model, vec3( 1.52f, 0.21f, halfD + 1.56f), vec3(0.25f, 0.07f, 0.17f),  -8.0f, mossVelvet);
    drawMossCushion(shader, model, vec3(-1.70f, 0.12f, halfD + 1.88f), vec3(0.28f, 0.07f, 0.20f),  12.0f, mossDark);
    drawMossCushion(shader, model, vec3( 1.68f, 0.11f, halfD + 1.86f), vec3(0.26f, 0.07f, 0.19f), -18.0f, algaeSlime);

    // Shoe Shelf (জুতা রাখার তাক) on the plinth beside the stairs for worshippers' slippers
    mat4 shoeRack = model;
    shoeRack = translate(shoeRack, vec3(-1.85f, plinthH + 0.30f, halfD + 0.85f));
    shoeRack = scale(shoeRack, vec3(0.85f, 0.60f, 0.35f));
    Primitives::drawCube(shader, shoeRack, darkSalTimber);

    // Shelves inside rack
    for (int sh = 1; sh <= 2; ++sh) {
        mat4 shelf = model;
        shelf = translate(shelf, vec3(-1.85f, plinthH + (float)sh * 0.20f, halfD + 0.85f));
        shelf = scale(shelf, vec3(0.88f, 0.025f, 0.36f));
        Primitives::drawCube(shader, shelf, burntBrickDark);
    }

    // ── 2. Main Red Terracotta Brick Prayer Hall (লাল পোড়ামাটির ইটের নামাজ ঘর) ──
    shader.setInt("uUseTexture", activeTexMode);
    float wallCenterY = plinthH + hallH * 0.5f;
    mat4 walls = model;
    walls = translate(walls, vec3(0.0f, wallCenterY, 0.0f));
    walls = scale(walls, vec3(hallW, hallH, hallD));
    Primitives::drawCube(shader, walls, terracottaBrick);

    // Molded Burnt Brick Base Skirting Band
    mat4 brickBase = model;
    brickBase = translate(brickBase, vec3(0.0f, plinthH + 0.12f, 0.0f));
    brickBase = scale(brickBase, vec3(hallW + 0.08f, 0.24f, hallD + 0.08f));
    Primitives::drawCube(shader, brickBase, burntBrickDark);

    // ── Lower Brick Wall Base Moisture-Wicking Slime Skirt (ইটের দেওয়ালের নোনা ধরা ও শ্যাওলা) ──
    drawSlimeBand(shader, model, vec3(0.0f, plinthH + 0.10f,  halfD + 0.043f), vec3(hallW * 0.94f, 0.16f, 0.022f), algaeSlime);
    drawSlimeBand(shader, model, vec3(0.0f, plinthH + 0.12f, -halfD - 0.043f), vec3(hallW * 0.94f, 0.20f, 0.022f), mossDark);
    drawSlimeBand(shader, model, vec3(-halfW - 0.043f, plinthH + 0.11f, 0.0f), vec3(0.022f, 0.18f, hallD * 0.94f), algaeSlime);
    drawSlimeBand(shader, model, vec3( halfW + 0.043f, plinthH + 0.11f, 0.0f), vec3(0.022f, 0.18f, hallD * 0.94f), algaeSlime);

    // Creeping moss cushions hugging the terracotta brick base
    drawMossCushion(shader, model, vec3(-2.15f, plinthH + 0.16f,  halfD + 0.048f), vec3(0.34f, 0.22f, 0.08f), 0.0f, mossVelvet);
    drawMossCushion(shader, model, vec3(-1.18f, plinthH + 0.12f,  halfD + 0.048f), vec3(0.28f, 0.18f, 0.08f), 0.0f, mossDark);
    drawMossCushion(shader, model, vec3( 1.18f, plinthH + 0.14f,  halfD + 0.048f), vec3(0.30f, 0.19f, 0.08f), 0.0f, mossVelvet);
    drawMossCushion(shader, model, vec3( 2.15f, plinthH + 0.18f,  halfD + 0.048f), vec3(0.36f, 0.24f, 0.08f), 0.0f, algaeSlime);

    // Rear wall heavily shaded moss colonization
    drawMossCushion(shader, model, vec3(-2.0f, plinthH + 0.22f, -halfD - 0.048f), vec3(0.40f, 0.28f, 0.08f), 0.0f, mossDark);
    drawMossCushion(shader, model, vec3(-0.9f, plinthH + 0.18f, -halfD - 0.048f), vec3(0.32f, 0.20f, 0.08f), 0.0f, mossVelvet);
    drawMossCushion(shader, model, vec3( 0.9f, plinthH + 0.20f, -halfD - 0.048f), vec3(0.35f, 0.24f, 0.08f), 0.0f, mossDark);
    drawMossCushion(shader, model, vec3( 2.0f, plinthH + 0.25f, -halfD - 0.048f), vec3(0.42f, 0.30f, 0.08f), 0.0f, algaeSlime);

    // West and East lateral wall base moss cushions
    drawMossCushion(shader, model, vec3(-halfW - 0.048f, plinthH + 0.18f, -1.8f), vec3(0.08f, 0.24f, 0.36f), 0.0f, mossVelvet);
    drawMossCushion(shader, model, vec3(-halfW - 0.048f, plinthH + 0.15f,  0.0f), vec3(0.08f, 0.20f, 0.32f), 0.0f, mossDark);
    drawMossCushion(shader, model, vec3(-halfW - 0.048f, plinthH + 0.20f,  1.8f), vec3(0.08f, 0.25f, 0.34f), 0.0f, algaeSlime);
    drawMossCushion(shader, model, vec3( halfW + 0.048f, plinthH + 0.16f, -1.8f), vec3(0.08f, 0.22f, 0.34f), 0.0f, mossVelvet);
    drawMossCushion(shader, model, vec3( halfW + 0.048f, plinthH + 0.14f,  0.0f), vec3(0.08f, 0.18f, 0.30f), 0.0f, algaeSlime);
    drawMossCushion(shader, model, vec3( halfW + 0.048f, plinthH + 0.19f,  1.8f), vec3(0.08f, 0.26f, 0.38f), 0.0f, mossDark);

    // Dual Burnt Brick Horizontal Belt Moldings (মধ্যভাগের ঐতিহ্যবাহী টেরাকোটা কার্নিশ বেল্ট)
    shader.setInt("uUseTexture", activeTexMode);
    mat4 belt1 = model;
    belt1 = translate(belt1, vec3(0.0f, plinthH + hallH * 0.48f, 0.0f));
    belt1 = scale(belt1, vec3(hallW + 0.06f, 0.08f, hallD + 0.06f));
    Primitives::drawCube(shader, belt1, burntBrickDark);

    mat4 belt2 = model;
    belt2 = translate(belt2, vec3(0.0f, plinthH + hallH * 0.52f, 0.0f));
    belt2 = scale(belt2, vec3(hallW + 0.04f, 0.05f, hallD + 0.04f));
    Primitives::drawCube(shader, belt2, terracottaTrim);

    // Belt molding damp ledges
    drawSlimeBand(shader, model, vec3(0.0f, plinthH + hallH * 0.48f + 0.045f, halfD + 0.035f), vec3(hallW * 0.88f, 0.02f, 0.02f), mossDark);
    drawSlimeBand(shader, model, vec3(0.0f, plinthH + hallH * 0.48f + 0.045f, -halfD - 0.035f), vec3(hallW * 0.88f, 0.02f, 0.02f), mossDark);

    // ── 3. Roofline Parapet & Islamic Stepped Merlons (কাঙ্গুরা / Kangura) ──
    // Upper roof projecting curved terracotta cornice in burnt brick
    mat4 roofCornice = model;
    roofCornice = translate(roofCornice, vec3(0.0f, plinthH + hallH + 0.06f, 0.0f));
    roofCornice = scale(roofCornice, vec3(hallW + 0.26f, 0.12f, hallD + 0.26f));
    Primitives::drawCube(shader, roofCornice, burntBrickDark);

    // Flat roof deck platform in old red stone
    mat4 roofDeck = model;
    roofDeck = translate(roofDeck, vec3(0.0f, plinthH + hallH + 0.12f, 0.0f));
    roofDeck = scale(roofDeck, vec3(hallW + 0.10f, 0.05f, hallD + 0.10f));
    Primitives::drawCube(shader, roofDeck, oldRedStone);

    // ── Rainwater Runoff Algae Drips & Slime Streaks Weeping from Roof Cornice ──
    // Front (+Z) vertical algae runoff
    drawAlgaeDrip(shader, model, vec3(-2.20f, plinthH + hallH,  halfD + 0.05f), 0.65f, 0.09f, algaeSlime);
    drawAlgaeDrip(shader, model, vec3(-1.35f, plinthH + hallH,  halfD + 0.05f), 0.48f, 0.07f, moistureDrip);
    drawAlgaeDrip(shader, model, vec3( 1.35f, plinthH + hallH,  halfD + 0.05f), 0.52f, 0.08f, algaeSlime);
    drawAlgaeDrip(shader, model, vec3( 2.20f, plinthH + hallH,  halfD + 0.05f), 0.70f, 0.10f, mossDark);

    // Shaded Rear (-Z) heavy weeping algae runoff
    drawAlgaeDrip(shader, model, vec3(-2.10f, plinthH + hallH, -halfD - 0.05f), 0.78f, 0.10f, mossDark);
    drawAlgaeDrip(shader, model, vec3(-0.95f, plinthH + hallH, -halfD - 0.05f), 0.55f, 0.08f, algaeSlime);
    drawAlgaeDrip(shader, model, vec3( 0.95f, plinthH + hallH, -halfD - 0.05f), 0.62f, 0.08f, moistureDrip);
    drawAlgaeDrip(shader, model, vec3( 2.10f, plinthH + hallH, -halfD - 0.05f), 0.82f, 0.11f, mossDark);

    // Lateral West (-X) wall rain drip streaks
    drawAlgaeDrip(shader, model, vec3(-halfW - 0.05f, plinthH + hallH, -1.9f), 0.72f, 0.09f, algaeSlime);
    drawAlgaeDrip(shader, model, vec3(-halfW - 0.05f, plinthH + hallH, -0.4f), 0.50f, 0.07f, moistureDrip);
    drawAlgaeDrip(shader, model, vec3(-halfW - 0.05f, plinthH + hallH,  0.4f), 0.58f, 0.08f, mossDark);
    drawAlgaeDrip(shader, model, vec3(-halfW - 0.05f, plinthH + hallH,  1.9f), 0.68f, 0.09f, algaeSlime);

    // Lateral East (+X) wall rain drip streaks
    drawAlgaeDrip(shader, model, vec3( halfW + 0.05f, plinthH + hallH, -1.9f), 0.65f, 0.08f, algaeSlime);
    drawAlgaeDrip(shader, model, vec3( halfW + 0.05f, plinthH + hallH, -0.4f), 0.45f, 0.07f, moistureDrip);
    drawAlgaeDrip(shader, model, vec3( halfW + 0.05f, plinthH + hallH,  0.4f), 0.52f, 0.08f, mossDark);
    drawAlgaeDrip(shader, model, vec3( halfW + 0.05f, plinthH + hallH,  1.9f), 0.70f, 0.10f, algaeSlime);

    // Decorative stepped Islamic crenellations along roof parapet edges (Kangura)
    float parapetY = plinthH + hallH + 0.12f;
    // Front and Back parapets
    drawKanguraRow(shader, model, -halfW + 0.5f, halfW - 0.5f, parapetY,  halfD + 0.05f, 0.42f, burntBrickDark, ochreBrickLight);
    drawKanguraRow(shader, model, -halfW + 0.5f, halfW - 0.5f, parapetY, -halfD - 0.05f, 0.42f, burntBrickDark, ochreBrickLight);
    // Lateral side parapets (East and West)
    for (float z = -halfD + 0.6f; z <= halfD - 0.6f; z += 0.48f) {
        // West side
        mat4 kw = model;
        kw = translate(kw, vec3(-halfW - 0.05f, parapetY + 0.06f, z));
        kw = scale(kw, vec3(0.06f, 0.12f, 0.32f));
        Primitives::drawCube(shader, kw, burntBrickDark);
        // East side
        mat4 ke = model;
        ke = translate(ke, vec3(halfW + 0.05f, parapetY + 0.06f, z));
        ke = scale(ke, vec3(0.06f, 0.12f, 0.32f));
        Primitives::drawCube(shader, ke, burntBrickDark);
    }

    // ── 4. Front Entrance Veranda Colonnade (মসজিদের টেরাকোটা বারান্দা ও খিলান) ───
    shader.setInt("uUseTexture", 0);
    // Shaded 3-bay Islamic arched porch at +Z
    const float porchDepth = 1.15f;
    const float porchZ     = halfD + porchDepth * 0.5f;
    const float porchFrontZ = halfD + porchDepth;

    // Raised veranda floor in old red stone
    mat4 porchFloor = model;
    porchFloor = translate(porchFloor, vec3(0.0f, plinthH + 0.02f, porchZ));
    porchFloor = scale(porchFloor, vec3(hallW * 0.86f, 0.04f, porchDepth));
    Primitives::drawCube(shader, porchFloor, oldRedStone);

    // Veranda flat roof slab in burnt brick
    mat4 porchRoof = model;
    porchRoof = translate(porchRoof, vec3(0.0f, plinthH + hallH * 0.85f, porchZ));
    porchRoof = scale(porchRoof, vec3(hallW * 0.88f, 0.10f, porchDepth + 0.12f));
    Primitives::drawCube(shader, porchRoof, burntBrickDark);

    // Front Colonnade: 4 Old Red Stone & Terracotta Brick Piers
    const float colX[4] = { -halfW * 0.40f, -halfW * 0.14f, halfW * 0.14f, halfW * 0.40f };
    const float colH = hallH * 0.80f;

    for (int c = 0; c < 4; ++c) {
        float cx = colX[c];

        // Dressed stone base plinth
        mat4 cBase = model;
        cBase = translate(cBase, vec3(cx, plinthH + 0.12f, porchFrontZ));
        cBase = scale(cBase, vec3(0.24f, 0.24f, 0.24f));
        Primitives::drawCube(shader, cBase, agedStoneBase);

        // Moss cushions hugging base of colonnade stone pillars
        drawMossCushion(shader, model, vec3(cx, plinthH + 0.06f, porchFrontZ + 0.125f), vec3(0.20f, 0.10f, 0.06f), 0.0f, (c % 2 == 0) ? mossVelvet : mossDark);
        drawMossCushion(shader, model, vec3(cx + 0.125f, plinthH + 0.05f, porchFrontZ), vec3(0.06f, 0.09f, 0.20f), 0.0f, algaeSlime);

        // Old red stone pillar shaft
        mat4 cShaft = model;
        cShaft = translate(cShaft, vec3(cx, plinthH + colH * 0.5f, porchFrontZ));
        cShaft = scale(cShaft, vec3(0.18f, colH, 0.18f));
        Primitives::drawCube(shader, cShaft, oldRedStone);

        // Carved terracotta molded capital
        mat4 cCap = model;
        cCap = translate(cCap, vec3(cx, plinthH + colH - 0.08f, porchFrontZ));
        cCap = scale(cCap, vec3(0.26f, 0.16f, 0.26f));
        Primitives::drawCube(shader, cCap, terracottaTrim);
    }

    // 3 Multi-cusped Pointed Arches across the front colonnade bays in burnt terracotta brick
    shader.setInt("uUseTexture", activeTexMode);
    for (int a = 0; a < 3; ++a) {
        float midX = (colX[a] + colX[a+1]) * 0.5f;
        float span = colX[a+1] - colX[a];

        mat4 archM = model;
        archM = translate(archM, vec3(midX, plinthH + colH * 0.72f, porchFrontZ));
        archM = scale(archM, vec3(span * 0.94f, 0.70f, 0.12f));
        Primitives::drawArch(shader, archM, burntBrickDark);

        // Warm ochre terracotta inner arch soffit
        mat4 archInner = model;
        archInner = translate(archInner, vec3(midX, plinthH + colH * 0.72f, porchFrontZ));
        archInner = scale(archInner, vec3(span * 0.82f, 0.60f, 0.14f));
        Primitives::drawArch(shader, archInner, ochreBrickLight);

        // Carved terracotta rosette medallion in arch spandrel (টেরাকোটা পদ্মফুল)
        mat4 rosette = model;
        rosette = translate(rosette, vec3(midX, plinthH + colH * 0.86f, porchFrontZ + 0.06f));
        rosette = scale(rosette, vec3(0.10f, 0.10f, 0.03f));
        Primitives::drawSphere(shader, rosette, terracottaTrim);

        // Weeping algae drip below rosette medallion
        drawAlgaeDrip(shader, model, vec3(midX, plinthH + colH * 0.81f, porchFrontZ + 0.065f), 0.38f, 0.06f, algaeSlime);

        // Springline moss/lichen cushions where arches meet pillar capitals
        drawMossCushion(shader, model, vec3(midX - span * 0.42f, plinthH + colH * 0.52f, porchFrontZ + 0.06f), vec3(0.09f, 0.08f, 0.06f), 0.0f, lichenDry);
        drawMossCushion(shader, model, vec3(midX + span * 0.42f, plinthH + colH * 0.52f, porchFrontZ + 0.06f), vec3(0.09f, 0.08f, 0.06f), 0.0f, mossVelvet);
    }

    // ── 5. Main Entrance Portal & Double Wooden Doors (মসজিদের প্রধান দরজা) ───
    shader.setInt("uUseTexture", 0);
    const float doorW = 1.35f;
    const float doorH = 2.10f;
    const float doorZ = halfD;

    // Seasoned dark Sal timber door frame
    mat4 dFrame = model;
    dFrame = translate(dFrame, vec3(0.0f, plinthH + doorH * 0.5f, doorZ + 0.04f));
    dFrame = scale(dFrame, vec3(doorW + 0.14f, doorH + 0.10f, 0.08f));
    Primitives::drawCube(shader, dFrame, darkSalTimber);

    // Double timber door panels
    for (int side = -1; side <= 1; side += 2) {
        float leafX = (float)side * (doorW * 0.245f);
        mat4 dLeaf = model;
        dLeaf = translate(dLeaf, vec3(leafX, plinthH + doorH * 0.5f, doorZ + 0.05f));
        dLeaf = scale(dLeaf, vec3(doorW * 0.48f, doorH, 0.045f));
        Primitives::drawCube(shader, dLeaf, darkSalTimber);

        // Antique iron / bronze reinforcement studs & straps
        for (int r = 1; r <= 3; ++r) {
            float sy = plinthH + (float)r * (doorH / 4.0f);
            mat4 strap = model;
            strap = translate(strap, vec3(leafX, sy, doorZ + 0.075f));
            strap = scale(strap, vec3(doorW * 0.42f, 0.03f, 0.015f));
            Primitives::drawCube(shader, strap, burntBrickDark);

            mat4 stud = model;
            stud = translate(stud, vec3(leafX, sy, doorZ + 0.085f));
            stud = scale(stud, vec3(0.022f, 0.022f, 0.022f));
            Primitives::drawSphere(shader, stud, antiqueBronze);
        }

        // Antique brass door pull ring
        mat4 handle = model;
        handle = translate(handle, vec3((float)side * 0.10f, plinthH + doorH * 0.50f, doorZ + 0.09f));
        handle = scale(handle, vec3(0.035f, 0.065f, 0.035f));
        Primitives::drawCylinder(shader, handle, antiqueBronze);
    }

    // Arched Fanlight Transom above door with warm golden prayer glow & terracotta jali lattice
    float transomH = 0.55f;
    float transomY = plinthH + doorH + 0.05f + transomH * 0.5f;

    shader.setFloat("emissive", 0.85f);
    mat4 tGlow = model;
    tGlow = translate(tGlow, vec3(0.0f, transomY, doorZ + 0.03f));
    tGlow = scale(tGlow, vec3(doorW * 0.85f, transomH, 0.02f));
    Primitives::drawCube(shader, tGlow, interiorGlow);
    shader.setFloat("emissive", 0.0f);

    // Burnt terracotta jali lattice bars on transom
    for (int v = -2; v <= 2; ++v) {
        mat4 tBar = model;
        tBar = translate(tBar, vec3((float)v * 0.14f, transomY, doorZ + 0.045f));
        tBar = scale(tBar, vec3(0.025f, transomH * 0.88f, 0.025f));
        Primitives::drawCube(shader, tBar, burntBrickDark);
    }

    // Carved terracotta arch molding crowning the portal
    shader.setInt("uUseTexture", activeTexMode);
    mat4 portalArch = model;
    portalArch = translate(portalArch, vec3(0.0f, transomY + 0.12f, doorZ + 0.05f));
    portalArch = scale(portalArch, vec3(doorW * 1.05f, 0.60f, 0.06f));
    Primitives::drawArch(shader, portalArch, burntBrickDark);

    // Hanging Hurricane Lantern (Hariken) suspended under veranda ceiling
    shader.setInt("uUseTexture", 0);
    mat4 mChain = model;
    mChain = translate(mChain, vec3(0.0f, plinthH + hallH * 0.78f, porchZ));
    mChain = scale(mChain, vec3(0.015f, 0.22f, 0.015f));
    Primitives::drawCylinder(shader, mChain, antiqueBronze);

    mat4 mLantern = model;
    mLantern = translate(mLantern, vec3(0.0f, plinthH + hallH * 0.78f - 0.24f, porchZ));
    mLantern = scale(mLantern, vec3(0.85f, 0.85f, 0.85f));
    Charpai::drawLantern(shader, mLantern);

    // ── 6. Central Grand Red Terracotta Dome (ঐতিহাসিক পোড়ামাটির লাল গম্বুজ) ──────────
    shader.setInt("uUseTexture", 0);
    float domeBaseY = plinthH + hallH + 0.12f;

    // Octagonal Terracotta Brick Transition Drum
    mat4 drum = model;
    drum = translate(drum, vec3(0.0f, domeBaseY + 0.32f, 0.0f));
    drum = scale(drum, vec3(1.85f, 0.64f, 1.85f));
    Primitives::drawCylinder(shader, drum, terracottaBrick);

    // Burnt brick cornice band on drum
    mat4 drumBand = model;
    drumBand = translate(drumBand, vec3(0.0f, domeBaseY + 0.62f, 0.0f));
    drumBand = scale(drumBand, vec3(1.92f, 0.08f, 1.92f));
    Primitives::drawCylinder(shader, drumBand, burntBrickDark);

    // Stepped Lotus Petal Collar in carved terracotta (Padma Pith)
    mat4 lotusCollar = model;
    lotusCollar = translate(lotusCollar, vec3(0.0f, domeBaseY + 0.68f, 0.0f));
    lotusCollar = scale(lotusCollar, vec3(1.96f, 0.08f, 1.96f));
    Primitives::drawCylinder(shader, lotusCollar, ochreBrickLight);

    // ── Lotus Collar Rain Catchment Moss & Lichen Cushions ──
    for (int i = 0; i < 8; ++i) {
        float a = radians((float)i * 45.0f);
        vec3 mossPos = vec3(cosf(a) * 0.94f, domeBaseY + 0.72f, sinf(a) * 0.94f);
        drawMossCushion(shader, model, mossPos, vec3(0.24f, 0.08f, 0.24f), (float)(i * 45), (i % 2 == 0) ? mossVelvet : lichenDry);
    }

    // Vertical weeping algae drip streaks running down the drum facets
    for (int d = 0; d < 4; ++d) {
        float da = radians((float)d * 90.0f + 25.0f);
        vec3 dripTop = vec3(cosf(da) * 0.93f, domeBaseY + 0.60f, sinf(da) * 0.93f);
        drawAlgaeDrip(shader, model, dripTop, 0.32f, 0.07f, algaeSlime);
    }

    // Majestic Hemispherical Weathered Red Terracotta Dome (পোড়ামাটির লাল গম্বুজ)
    shader.setInt("uUseTexture", 0);
    mat4 dome = model;
    dome = translate(dome, vec3(0.0f, domeBaseY + 0.72f, 0.0f));
    dome = scale(dome, vec3(1.85f, 1.55f, 1.85f));
    Primitives::drawHemisphere(shader, dome, terracottaBrick);

    // Concentric burnt brick decorative flutes / ribs along the dome surface
    mat4 domeFlute1 = model;
    domeFlute1 = translate(domeFlute1, vec3(0.0f, domeBaseY + 1.25f, 0.0f));
    domeFlute1 = scale(domeFlute1, vec3(1.58f, 0.04f, 1.58f));
    Primitives::drawCylinder(shader, domeFlute1, burntBrickDark);

    // Shaded north slope moss cushions along lower dome flute
    drawMossCushion(shader, model, vec3( 0.25f, domeBaseY + 1.28f, -0.74f), vec3(0.22f, 0.05f, 0.12f),  15.0f, mossDark);
    drawMossCushion(shader, model, vec3(-0.35f, domeBaseY + 1.28f, -0.72f), vec3(0.25f, 0.06f, 0.14f), -20.0f, mossVelvet);

    mat4 domeFlute2 = model;
    domeFlute2 = translate(domeFlute2, vec3(0.0f, domeBaseY + 1.75f, 0.0f));
    domeFlute2 = scale(domeFlute2, vec3(1.15f, 0.04f, 1.15f));
    Primitives::drawCylinder(shader, domeFlute2, burntBrickDark);

    // Crowning Antique Bronze Kalasa Spire with Grand Islamic Crescent Moon & Star (চাঁদ-তারা)
    float kalasaY = domeBaseY + 0.72f + 1.55f;
    drawChandTara(shader, model, vec3(0.0f, kalasaY, 0.0f), 1.55f, antiqueBronze);

    // ── 7. Soaring Red Brick Azaan Minaret with Quad Horn Loudspeakers (আজানের মিনার ও চোঙা মাইক) ──
    shader.setInt("uUseTexture", 0);
    // Situated at the front-left corner (cx = -halfW - 0.15f, cz = halfD + 0.15f)
    const float minX = -halfW - 0.15f;
    const float minZ =  halfD + 0.15f;
    const float minR = 0.42f;

    // Square Old Red Stone & Brick Base Tier
    mat4 mBase = model;
    mBase = translate(mBase, vec3(minX, plinthH + 0.75f, minZ));
    mBase = scale(mBase, vec3(1.15f, 1.50f, 1.15f));
    Primitives::drawCube(shader, mBase, oldRedStone);

    // Minaret base creeping moss cushions hugging square stone plinth corners
    drawMossCushion(shader, model, vec3(minX - 0.58f, plinthH + 0.22f, minZ - 0.58f), vec3(0.26f, 0.22f, 0.26f), 0.0f, mossDark);
    drawMossCushion(shader, model, vec3(minX + 0.58f, plinthH + 0.20f, minZ - 0.58f), vec3(0.24f, 0.20f, 0.24f), 0.0f, mossVelvet);
    drawMossCushion(shader, model, vec3(minX - 0.58f, plinthH + 0.24f, minZ + 0.58f), vec3(0.28f, 0.24f, 0.28f), 0.0f, algaeSlime);
    drawMossCushion(shader, model, vec3(minX + 0.58f, plinthH + 0.18f, minZ + 0.58f), vec3(0.22f, 0.18f, 0.22f), 0.0f, mossVelvet);

    // Burnt brick corner pilasters on minaret base
    for (int px = -1; px <= 1; px += 2) {
        for (int pz = -1; pz <= 1; pz += 2) {
            mat4 pilaster = model;
            pilaster = translate(pilaster, vec3(minX + (float)px * 0.56f, plinthH + 0.75f, minZ + (float)pz * 0.56f));
            pilaster = scale(pilaster, vec3(0.08f, 1.52f, 0.08f));
            Primitives::drawCube(shader, pilaster, burntBrickDark);
        }
    }

    // Molded burnt brick transition cornice
    mat4 mCornice1 = model;
    mCornice1 = translate(mCornice1, vec3(minX, plinthH + 1.54f, minZ));
    mCornice1 = scale(mCornice1, vec3(1.22f, 0.08f, 1.22f));
    Primitives::drawCube(shader, mCornice1, burntBrickDark);

    // Slender Red Terracotta Octagonal Minaret Shaft (reaching 6.4m above plinth)
    shader.setInt("uUseTexture", 0);
    const float shaftH = 4.80f;
    mat4 mShaft = model;
    mShaft = translate(mShaft, vec3(minX, plinthH + 1.58f + shaftH * 0.5f, minZ));
    mShaft = scale(mShaft, vec3(minR, shaftH, minR));
    Primitives::drawCylinder(shader, mShaft, terracottaBrick);

    // 3 Decorative Burnt Brick Belt Rings dividing the minaret shaft
    for (int r = 1; r <= 3; ++r) {
        float rY = plinthH + 1.58f + shaftH * ((float)r / 4.0f);
        mat4 ring = model;
        ring = translate(ring, vec3(minX, rY, minZ));
        ring = scale(ring, vec3(minR * 1.18f, 0.09f, minR * 1.18f));
        Primitives::drawCylinder(shader, ring, burntBrickDark);

        // Arched niche slots in burnt terracotta on 4 faces
        for (int side = 0; side < 4; ++side) {
            float lAngle = (float)side * 90.0f;
            mat4 louver = model;
            louver = translate(louver, vec3(minX, rY - 0.28f, minZ));
            louver = rotate(louver, radians(lAngle), vec3(0.0f, 1.0f, 0.0f));
            louver = translate(louver, vec3(0.0f, 0.0f, minR * 0.98f));
            louver = scale(louver, vec3(0.12f, 0.22f, 0.03f));
            Primitives::drawCube(shader, louver, burntBrickDark);
        }
    }

    // Muazzin Balcony / Gallery (Azaan Platform at Y = 6.4m)
    shader.setInt("uUseTexture", 0);
    float galleryY = plinthH + 1.58f + shaftH;

    // Overhanging corbel brackets in burnt brick
    mat4 galleryCorbel = model;
    galleryCorbel = translate(galleryCorbel, vec3(minX, galleryY - 0.12f, minZ));
    galleryCorbel = scale(galleryCorbel, vec3(minR * 1.45f, 0.24f, minR * 1.45f));
    Primitives::drawCone(shader, galleryCorbel, burntBrickDark);

    // Weeping rainwater algae drip lines beneath the balcony corbels
    drawAlgaeDrip(shader, model, vec3(minX, galleryY - 0.20f, minZ + minR + 0.02f), 0.60f, 0.07f, algaeSlime);
    drawAlgaeDrip(shader, model, vec3(minX, galleryY - 0.20f, minZ - minR - 0.02f), 0.72f, 0.08f, mossDark);
    drawAlgaeDrip(shader, model, vec3(minX + minR + 0.02f, galleryY - 0.20f, minZ), 0.52f, 0.06f, moistureDrip);
    drawAlgaeDrip(shader, model, vec3(minX - minR - 0.02f, galleryY - 0.20f, minZ), 0.65f, 0.07f, algaeSlime);

    // Projecting octagonal balcony floor deck in old red stone
    mat4 galleryDeck = model;
    galleryDeck = translate(galleryDeck, vec3(minX, galleryY, minZ));
    galleryDeck = scale(galleryDeck, vec3(minR * 1.70f, 0.10f, minR * 1.70f));
    Primitives::drawCylinder(shader, galleryDeck, oldRedStone);

    // Protective balcony railing in terracotta pierced stone
    mat4 railing = model;
    railing = translate(railing, vec3(minX, galleryY + 0.32f, minZ));
    railing = scale(railing, vec3(minR * 1.68f, 0.54f, minR * 1.68f));
    Primitives::drawCylinder(shader, railing, terracottaTrim);

    mat4 railTop = model;
    railTop = translate(railTop, vec3(minX, galleryY + 0.60f, minZ));
    railTop = scale(railTop, vec3(minR * 1.74f, 0.06f, minR * 1.74f));
    Primitives::drawCylinder(shader, railTop, burntBrickDark);

    // ── THE 4 AZAAN HORN LOUDSPEAKERS (চারটি ঐতিহ্যবাহী চোঙা মাইক) ──
    // Mounted facing North (0 deg), East (90 deg), South (180 deg), and West (270 deg)
    float speakerY = galleryY + 0.38f;
    drawLoudspeaker(shader, model, vec3(minX, speakerY, minZ),   0.0f); // North
    drawLoudspeaker(shader, model, vec3(minX, speakerY, minZ),  90.0f); // East
    drawLoudspeaker(shader, model, vec3(minX, speakerY, minZ), 180.0f); // South
    drawLoudspeaker(shader, model, vec3(minX, speakerY, minZ), 270.0f); // West

    // Upper Pavilion (Chhatri) above gallery: 8 slender old red stone pillars
    float pavilH = 1.10f;
    for (int p = 0; p < 8; ++p) {
        float pAng = (float)p * 45.0f;
        float px = minX + cosf(radians(pAng)) * (minR * 0.88f);
        float pz = minZ + sinf(radians(pAng)) * (minR * 0.88f);

        mat4 pPillar = model;
        pPillar = translate(pPillar, vec3(px, galleryY + 0.62f + pavilH * 0.5f, pz));
        pPillar = scale(pPillar, vec3(0.05f, pavilH, 0.05f));
        Primitives::drawCylinder(shader, pPillar, oldRedStone);
    }

    // Minaret Cupola Roof Cornice in burnt brick
    float cupolaBaseY = galleryY + 0.62f + pavilH;
    mat4 minCornice = model;
    minCornice = translate(minCornice, vec3(minX, cupolaBaseY + 0.04f, minZ));
    minCornice = scale(minCornice, vec3(minR * 1.35f, 0.08f, minR * 1.35f));
    Primitives::drawCylinder(shader, minCornice, burntBrickDark);

    // Ribbed Terracotta Minaret Cupola Domelet
    shader.setInt("uUseTexture", 0);
    mat4 minCupola = model;
    minCupola = translate(minCupola, vec3(minX, cupolaBaseY + 0.10f, minZ));
    minCupola = scale(minCupola, vec3(minR * 1.15f, minR * 1.30f, minR * 1.15f));
    Primitives::drawHemisphere(shader, minCupola, terracottaBrick);

    // Minaret Spire & Crescent Moon & Star in antique bronze
    drawChandTara(shader, model, vec3(minX, cupolaBaseY + 0.10f + minR * 1.30f, minZ), 1.15f, antiqueBronze);

    // ── 8. Three Corner Decorative Pinnacles / Turrets (মিনার গম্বুজ / Guldasta) ──
    shader.setInt("uUseTexture", 0);
    // At the other 3 corners: Front-right, Rear-left, Rear-right
    const float turrCorners[3][2] = {
        {  halfW + 0.12f,  halfD + 0.12f }, // Front-right
        { -halfW - 0.12f, -halfD - 0.12f }, // Rear-left
        {  halfW + 0.12f, -halfD - 0.12f }  // Rear-right
    };
    const float turrH = hallH + 1.20f;
    const float turrR = 0.28f;

    for (int t = 0; t < 3; ++t) {
        float tx = turrCorners[t][0];
        float tz = turrCorners[t][1];

        // Turret base in burnt brick
        mat4 tBase = model;
        tBase = translate(tBase, vec3(tx, plinthH + 0.25f, tz));
        tBase = scale(tBase, vec3(turrR * 1.35f, 0.50f, turrR * 1.35f));
        Primitives::drawCylinder(shader, tBase, burntBrickDark);

        // Turret base moss cushion
        drawMossCushion(shader, model, vec3(tx, plinthH + 0.48f, tz + turrR + 0.03f), vec3(0.18f, 0.08f, 0.12f), 0.0f, mossVelvet);
        drawMossCushion(shader, model, vec3(tx + turrR + 0.03f, plinthH + 0.48f, tz), vec3(0.12f, 0.08f, 0.18f), 0.0f, algaeSlime);

        // Terracotta turret shaft
        mat4 tShaft = model;
        tShaft = translate(tShaft, vec3(tx, plinthH + turrH * 0.5f, tz));
        tShaft = scale(tShaft, vec3(turrR, turrH, turrR));
        Primitives::drawCylinder(shader, tShaft, terracottaBrick);

        // 2 Burnt brick belt rings
        for (int r = 1; r <= 2; ++r) {
            mat4 tRing = model;
            tRing = translate(tRing, vec3(tx, plinthH + turrH * ((float)r / 3.0f), tz));
            tRing = scale(tRing, vec3(turrR * 1.20f, 0.08f, turrR * 1.20f));
            Primitives::drawCylinder(shader, tRing, burntBrickDark);
        }

        // Bracketed cornice
        float tCornY = plinthH + turrH;
        mat4 tCorn = model;
        tCorn = translate(tCorn, vec3(tx, tCornY + 0.04f, tz));
        tCorn = scale(tCorn, vec3(turrR * 1.38f, 0.08f, turrR * 1.38f));
        Primitives::drawCylinder(shader, tCorn, burntBrickDark);

        // Terracotta cupola domelet
        mat4 tCupola = model;
        tCupola = translate(tCupola, vec3(tx, tCornY + 0.10f, tz));
        tCupola = scale(tCupola, vec3(turrR * 1.15f, turrR * 1.20f, turrR * 1.15f));
        Primitives::drawHemisphere(shader, tCupola, terracottaBrick);

        // Antique bronze Crescent & Star finial
        drawChandTara(shader, model, vec3(tx, tCornY + 0.10f + turrR * 1.20f, tz), 0.85f, antiqueBronze);
    }

    // ── 9. Arched Islamic Windows with Radiant Interior Glow (মসজিদের বাতায়ন) ──
    const float winW = 0.82f;
    const float winH = 1.20f;
    const float winY = plinthH + hallH * 0.52f;

    auto drawIslamicWindow = [&](const vec3& pos, float rotYDeg) {
        mat4 wm = model;
        wm = translate(wm, pos);
        wm = rotate(wm, radians(rotYDeg), vec3(0.0f, 1.0f, 0.0f));

        // Window surround casing in carved red stone
        shader.setInt("uUseTexture", 0);
        mat4 casing = wm;
        casing = translate(casing, vec3(0.0f, 0.0f, 0.02f));
        casing = scale(casing, vec3(winW + 0.16f, winH + 0.16f, 0.06f));
        Primitives::drawCube(shader, casing, oldRedStone);

        // Molded burnt brick window sill
        shader.setInt("uUseTexture", activeTexMode);
        mat4 sill = wm;
        sill = translate(sill, vec3(0.0f, -winH * 0.5f - 0.035f, 0.04f));
        sill = scale(sill, vec3(winW + 0.22f, 0.07f, 0.12f));
        Primitives::drawCube(shader, sill, burntBrickDark);

        // Window sill moss cushion and weeping rain algae drip
        drawMossCushion(shader, wm, vec3(-winW * 0.35f, -winH * 0.5f - 0.01f, 0.08f), vec3(0.12f, 0.04f, 0.08f), 0.0f, mossVelvet);
        drawAlgaeDrip(shader, wm, vec3(winW * 0.25f, -winH * 0.5f - 0.06f, 0.08f), 0.28f, 0.05f, algaeSlime);

        // Molded burnt brick lintel
        mat4 lintel = wm;
        lintel = translate(lintel, vec3(0.0f, winH * 0.5f + 0.035f, 0.03f));
        lintel = scale(lintel, vec3(winW + 0.18f, 0.07f, 0.10f));
        Primitives::drawCube(shader, lintel, burntBrickDark);

        // Pointed Islamic arch crown in carved terracotta
        mat4 arch = wm;
        arch = translate(arch, vec3(0.0f, winH * 0.44f, 0.03f));
        arch = scale(arch, vec3(winW * 0.92f, 0.46f, 0.09f));
        Primitives::drawArch(shader, arch, terracottaTrim);

        // Radiant warm golden prayer sanctuary glow radiating from inside
        shader.setFloat("emissive", 0.85f);
        mat4 glow = wm;
        glow = translate(glow, vec3(0.0f, 0.0f, -0.01f));
        glow = scale(glow, vec3(winW - 0.10f, winH - 0.10f, 0.02f));
        Primitives::drawCube(shader, glow, interiorGlow);
        shader.setFloat("emissive", 0.0f);

        // Burnt terracotta jali / louver lattice grid
        for (int v = -2; v <= 2; ++v) {
            mat4 barV = wm;
            barV = translate(barV, vec3((float)v * (winW * 0.14f), 0.0f, 0.03f));
            barV = scale(barV, vec3(0.032f, winH * 0.80f, 0.025f));
            Primitives::drawCube(shader, barV, burntBrickDark);
        }
        for (int h = -2; h <= 2; ++h) {
            mat4 barH = wm;
            barH = translate(barH, vec3(0.0f, (float)h * (winH * 0.15f), 0.03f));
            barH = scale(barH, vec3(winW * 0.80f, 0.032f, 0.025f));
            Primitives::drawCube(shader, barH, burntBrickDark);
        }
    };

    // Front facade windows (flanking the entrance portal)
    drawIslamicWindow(vec3(-halfW * 0.62f, winY, halfD + 0.02f), 0.0f);
    drawIslamicWindow(vec3( halfW * 0.62f, winY, halfD + 0.02f), 0.0f);

    // Left (West) wall windows
    drawIslamicWindow(vec3(-halfW - 0.04f, winY, -1.2f), -90.0f);
    drawIslamicWindow(vec3(-halfW - 0.04f, winY,  1.2f), -90.0f);

    // Right (East) wall windows
    drawIslamicWindow(vec3( halfW + 0.04f, winY, -1.2f),  90.0f);
    drawIslamicWindow(vec3( halfW + 0.04f, winY,  1.2f),  90.0f);

    // ── 10. Western Mehrab Bay (পশ্চিম দেওয়ালের মেহরাব) ────────────
    shader.setInt("uUseTexture", activeTexMode);
    // Semi-octagonal prayer niche projection on the rear Qibla wall (-Z)
    float mehrabW = 1.70f;
    float mehrabH = 2.50f;
    float mehrabD = 0.70f;

    mat4 mehrab = model;
    mehrab = translate(mehrab, vec3(0.0f, plinthH + mehrabH * 0.5f, -halfD - mehrabD * 0.5f));
    mehrab = scale(mehrab, vec3(mehrabW, mehrabH, mehrabD));
    Primitives::drawCube(shader, mehrab, terracottaBrick);

    // Burnt brick base & cornice trims on Mehrab
    mat4 mehrabBase = model;
    mehrabBase = translate(mehrabBase, vec3(0.0f, plinthH + 0.10f, -halfD - mehrabD * 0.5f));
    mehrabBase = scale(mehrabBase, vec3(mehrabW + 0.08f, 0.20f, mehrabD + 0.08f));
    Primitives::drawCube(shader, mehrabBase, burntBrickDark);

    // Shaded Mehrab base moss cushions
    drawMossCushion(shader, model, vec3(-0.55f, plinthH + 0.15f, -halfD - mehrabD - 0.02f), vec3(0.28f, 0.18f, 0.08f), 0.0f, mossDark);
    drawMossCushion(shader, model, vec3( 0.55f, plinthH + 0.16f, -halfD - mehrabD - 0.02f), vec3(0.30f, 0.20f, 0.08f), 0.0f, mossVelvet);

    mat4 mehrabCorn = model;
    mehrabCorn = translate(mehrabCorn, vec3(0.0f, plinthH + mehrabH + 0.06f, -halfD - mehrabD * 0.5f));
    mehrabCorn = scale(mehrabCorn, vec3(mehrabW + 0.18f, 0.12f, mehrabD + 0.14f));
    Primitives::drawCube(shader, mehrabCorn, burntBrickDark);

    // Vertical algae drips beneath Mehrab cornice
    drawAlgaeDrip(shader, model, vec3(-0.45f, plinthH + mehrabH, -halfD - mehrabD - 0.02f), 0.55f, 0.08f, algaeSlime);
    drawAlgaeDrip(shader, model, vec3( 0.45f, plinthH + mehrabH, -halfD - mehrabD - 0.02f), 0.60f, 0.08f, mossDark);

    // Mini terracotta domelet atop the Mehrab projection
    mat4 mehrabDome = model;
    mehrabDome = translate(mehrabDome, vec3(0.0f, plinthH + mehrabH + 0.14f, -halfD - mehrabD * 0.5f));
    mehrabDome = scale(mehrabDome, vec3(0.48f, 0.40f, 0.48f));
    Primitives::drawHemisphere(shader, mehrabDome, terracottaBrick);

    // Small antique bronze spire
    shader.setInt("uUseTexture", 0);
    mat4 mehrabSpire = model;
    mehrabSpire = translate(mehrabSpire, vec3(0.0f, plinthH + mehrabH + 0.56f, -halfD - mehrabD * 0.5f));
    mehrabSpire = scale(mehrabSpire, vec3(0.04f, 0.24f, 0.04f));
    Primitives::drawCone(shader, mehrabSpire, antiqueBronze);

    // ── 11. Traditional Ablution Area (পাকা ওজুখানা, হাউজ ও রঙিন বদনা) ────
    shader.setInt("uUseTexture", 0);
    // Located on the eastern courtyard flank of the mosque platform
    float ozuX = halfW + 1.25f;
    float ozuZ = 0.5f;

    // Raised red terracotta brick masonry cistern walls (পাকা হাউজ)
    mat4 ozuTank = model;
    ozuTank = translate(ozuTank, vec3(ozuX, 0.32f, ozuZ));
    ozuTank = scale(ozuTank, vec3(1.40f, 0.64f, 2.30f));
    Primitives::drawCube(shader, ozuTank, terracottaBrick);

    // Burnt brick stone coping ledge around the cistern
    mat4 ozuCoping = model;
    ozuCoping = translate(ozuCoping, vec3(ozuX, 0.65f, ozuZ));
    ozuCoping = scale(ozuCoping, vec3(1.52f, 0.06f, 2.42f));
    Primitives::drawCube(shader, ozuCoping, burntBrickDark);

    // Clean ablution water surface within cistern (visible inside coping rim)
    mat4 ozuWater = model;
    ozuWater = translate(ozuWater, vec3(ozuX, 0.652f, ozuZ));
    ozuWater = scale(ozuWater, vec3(1.22f, 1.0f, 2.12f));
    Primitives::drawPlane(shader, ozuWater, waterCol);

    // Low old red stone washing bench
    mat4 bench = model;
    bench = translate(bench, vec3(ozuX + 1.05f, 0.20f, ozuZ));
    bench = scale(bench, vec3(0.48f, 0.40f, 2.30f));
    Primitives::drawCube(shader, bench, oldRedStone);

    mat4 benchTop = model;
    benchTop = translate(benchTop, vec3(ozuX + 1.05f, 0.41f, ozuZ));
    benchTop = scale(benchTop, vec3(0.52f, 0.03f, 2.34f));
    Primitives::drawCube(shader, benchTop, burntBrickDark);

    // 3 Brass Water Taps (পানির কল) in antique bronze mounted along the cistern wall
    for (int tap = -1; tap <= 1; ++tap) {
        float tz = ozuZ + (float)tap * 0.68f;

        // Pipe stem
        mat4 pipe = model;
        pipe = translate(pipe, vec3(ozuX + 0.68f, 0.48f, tz));
        pipe = scale(pipe, vec3(0.12f, 0.022f, 0.022f));
        Primitives::drawCube(shader, pipe, antiqueBronze);

        // Downward tap spout
        mat4 spout = model;
        spout = translate(spout, vec3(ozuX + 0.74f, 0.44f, tz));
        spout = scale(spout, vec3(0.022f, 0.06f, 0.022f));
        Primitives::drawCylinder(shader, spout, antiqueBronze);

        // Wet, slippery dark algae & slime streaming down the terracotta brick cistern wall beneath tap
        drawAlgaeDrip(shader, model, vec3(ozuX + 0.71f, 0.42f, tz), 0.38f, 0.12f, slimeWet);
        drawMossCushion(shader, model, vec3(ozuX + 0.74f, 0.06f, tz), vec3(0.16f, 0.06f, 0.22f), 0.0f, algaeSlime);
    }

    // Horizontal wet slime coating on ground beneath water taps
    drawSlimeBand(shader, model, vec3(ozuX + 0.82f, 0.025f, ozuZ), vec3(0.35f, 0.02f, 2.10f), slimeWet);

    // Damp moss cushions clinging to stone washing bench ends
    drawMossCushion(shader, model, vec3(ozuX + 1.05f, 0.12f, ozuZ - 1.16f), vec3(0.22f, 0.14f, 0.12f), 0.0f, mossVelvet);
    drawMossCushion(shader, model, vec3(ozuX + 1.05f, 0.14f, ozuZ + 1.16f), vec3(0.24f, 0.15f, 0.12f), 0.0f, mossDark);

    // 3 Handcrafted Traditional Bodnas (classic red, ceramic green, and natural terracotta ewers) on the washing bench
    drawBodna(shader, model, vec3(ozuX + 1.05f, 0.42f, ozuZ - 0.68f), 0.98f, bodnaRed);
    drawBodna(shader, model, vec3(ozuX + 1.05f, 0.42f, ozuZ),         0.92f, bodnaGreen);
    drawBodna(shader, model, vec3(ozuX + 1.05f, 0.42f, ozuZ + 0.68f), 0.96f, bodnaClay);

    shader.setInt("uUseTexture", activeTexMode);
}

} // namespace Mosque
