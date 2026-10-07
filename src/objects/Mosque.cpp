// Mosque.cpp — Historic Terracotta Brick & Old Red Stone Mosque (টিনের ও পাকা গ্রামীণ মসজিদ)
// Traditional Bangladeshi Village Mosque with Flat Concrete Roof (চ্যাপ্টা ছাদ),
// turned brass top spires without crescent/star (চাঁদ-তারা বিহীন চূড়া), and Ahuja sound system (চোঙা মাইক).

#include "objects/Mosque.h"
#include "objects/Charpai.h"
#include "Primitives.h"
#include "Texture.h"
#include <cmath>

using namespace math;

namespace Mosque {

// Helper 1: Traditional Mosque Spire / Finial (গম্বুজ ও মিনারের কলস / চূড়া - Starless Finial)
static void drawTopSpire(Shader& shader, const mat4& parentModel, const vec3& pos, float scaleVal, const vec3& goldCol)
{
    shader.setInt("uUseTexture", 0);
    mat4 baseM = parentModel;
    baseM = translate(baseM, pos);
    baseM = scale(baseM, vec3(scaleVal));

    // Base collar / lotus ring
    mat4 collar = baseM;
    collar = translate(collar, vec3(0.0f, 0.035f, 0.0f));
    collar = scale(collar, vec3(0.09f, 0.04f, 0.09f));
    Primitives::drawCylinder(shader, collar, goldCol);

    // Lower orb bead
    mat4 orbLower = baseM;
    orbLower = translate(orbLower, vec3(0.0f, 0.12f, 0.0f));
    orbLower = scale(orbLower, vec3(0.065f, 0.065f, 0.065f));
    Primitives::drawSphere(shader, orbLower, goldCol);

    // Middle neck spindle
    mat4 neck = baseM;
    neck = translate(neck, vec3(0.0f, 0.22f, 0.0f));
    neck = scale(neck, vec3(0.032f, 0.14f, 0.032f));
    Primitives::drawCylinder(shader, neck, goldCol);

    // Main central spherical orb
    mat4 orbMain = baseM;
    orbMain = translate(orbMain, vec3(0.0f, 0.33f, 0.0f));
    orbMain = scale(orbMain, vec3(0.085f, 0.085f, 0.085f));
    Primitives::drawSphere(shader, orbMain, goldCol);

    // Upper spire shaft
    mat4 spireShaft = baseM;
    spireShaft = translate(spireShaft, vec3(0.0f, 0.47f, 0.0f));
    spireShaft = scale(spireShaft, vec3(0.024f, 0.20f, 0.024f));
    Primitives::drawCylinder(shader, spireShaft, goldCol);

    // Sharp conical spire tip (starless finial)
    mat4 tip = baseM;
    tip = translate(tip, vec3(0.0f, 0.62f, 0.0f));
    tip = scale(tip, vec3(0.038f, 0.18f, 0.038f));
    Primitives::drawCone(shader, tip, goldCol);
}

// Helper 2: Traditional Village Azaan Horn Loudspeaker (চোঙা মাইক - Ahuja Sound System)
static void drawLoudspeaker(Shader& shader, const mat4& parentModel, const vec3& pos, float yawDeg)
{
    shader.setInt("uUseTexture", 0);
    const vec3 hornDark  (0.25f, 0.27f, 0.29f); // Dark slate metal body & bracket
    const vec3 hornMetal (0.50f, 0.53f, 0.56f); // Metallic horn bell
    const vec3 hornInner (0.15f, 0.16f, 0.18f); // Inner acoustic plug

    mat4 sm = parentModel;
    sm = translate(sm, pos);
    sm = rotate(sm, radians(yawDeg), vec3(0.0f, 1.0f, 0.0f));

    // Mounting U-bracket arm
    mat4 arm = sm;
    arm = translate(arm, vec3(0.0f, 0.0f, 0.18f));
    arm = scale(arm, vec3(0.024f, 0.024f, 0.36f));
    Primitives::drawCube(shader, arm, hornDark);

    // Swivel clamp
    mat4 clamp = sm;
    clamp = translate(clamp, vec3(0.0f, 0.0f, 0.34f));
    clamp = scale(clamp, vec3(0.065f, 0.075f, 0.05f));
    Primitives::drawCube(shader, clamp, hornDark);

    // Magnet driver housing
    mat4 driver = sm;
    driver = translate(driver, vec3(0.0f, 0.0f, 0.40f));
    driver = rotate(driver, radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
    driver = scale(driver, vec3(0.075f, 0.09f, 0.075f));
    Primitives::drawCylinder(shader, driver, hornDark);

    // Throat
    mat4 throat = sm;
    throat = translate(throat, vec3(0.0f, 0.0f, 0.48f));
    throat = rotate(throat, radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
    throat = scale(throat, vec3(0.06f, 0.10f, 0.06f));
    Primitives::drawCone(shader, throat, hornMetal);

    // Horn flared acoustic bell
    mat4 bell = sm;
    bell = translate(bell, vec3(0.0f, 0.0f, 0.58f));
    bell = rotate(bell, radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
    bell = scale(bell, vec3(0.18f, 0.22f, 0.18f));
    Primitives::drawCone(shader, bell, hornMetal);

    // Rim band
    mat4 rim = sm;
    rim = translate(rim, vec3(0.0f, 0.0f, 0.80f));
    rim = rotate(rim, radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
    rim = scale(rim, vec3(0.182f, 0.018f, 0.182f));
    Primitives::drawCylinder(shader, rim, hornDark);

    // Phase plug
    mat4 plug = sm;
    plug = translate(plug, vec3(0.0f, 0.0f, 0.72f));
    plug = rotate(plug, radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
    plug = scale(plug, vec3(0.045f, 0.08f, 0.045f));
    Primitives::drawCone(shader, plug, hornInner);
}

// Helper 3: Traditional Handcrafted Clay/Plastic Ablution Ewer (বদনা / Bodna)
static void drawBodna(Shader& shader, const mat4& parentModel, const vec3& pos, float scaleVal, const vec3& ewerCol)
{
    shader.setInt("uUseTexture", 0);
    mat4 bm = parentModel;
    bm = translate(bm, pos);
    bm = scale(bm, vec3(scaleVal));

    mat4 body = bm;
    body = translate(body, vec3(0.0f, 0.10f, 0.0f));
    body = scale(body, vec3(0.12f, 0.10f, 0.12f));
    Primitives::drawSphere(shader, body, ewerCol);

    mat4 base = bm;
    base = translate(base, vec3(0.0f, 0.015f, 0.0f));
    base = scale(base, vec3(0.075f, 0.03f, 0.075f));
    Primitives::drawCylinder(shader, base, ewerCol);

    mat4 neck = bm;
    neck = translate(neck, vec3(0.0f, 0.18f, 0.0f));
    neck = scale(neck, vec3(0.045f, 0.06f, 0.045f));
    Primitives::drawCylinder(shader, neck, ewerCol);

    mat4 rim = bm;
    rim = translate(rim, vec3(0.0f, 0.22f, 0.0f));
    rim = scale(rim, vec3(0.068f, 0.02f, 0.068f));
    Primitives::drawCylinder(shader, rim, ewerCol);

    mat4 spout = bm;
    spout = translate(spout, vec3(0.08f, 0.14f, 0.0f));
    spout = rotate(spout, radians(-45.0f), vec3(0.0f, 0.0f, 1.0f));
    spout = scale(spout, vec3(0.026f, 0.12f, 0.026f));
    Primitives::drawCone(shader, spout, ewerCol);

    mat4 handle = bm;
    handle = translate(handle, vec3(-0.08f, 0.15f, 0.0f));
    handle = scale(handle, vec3(0.022f, 0.10f, 0.022f));
    Primitives::drawCube(shader, handle, ewerCol);
}

// Helper 4: Organic Living Moss Cushion & Biological Weathering Patch (মখমলে সবুজ শ্যাওলা)
static void drawMossCushion(Shader& shader, const mat4& parentModel, const vec3& pos, const vec3& dims, float rotYDeg, const vec3& col)
{
    shader.setInt("uUseTexture", 0);
    mat4 m = parentModel;
    m = translate(m, pos);
    if (std::abs(rotYDeg) > 0.01f) {
        m = rotate(m, radians(rotYDeg), vec3(0.0f, 1.0f, 0.0f));
    }
    mat4 mCushion = m;
    mCushion = scale(mCushion, dims);
    Primitives::drawCube(shader, mCushion, col);

    mat4 mLobe = m;
    mLobe = translate(mLobe, vec3(dims.x * 0.20f, dims.y * 0.18f, dims.z * 0.14f));
    mLobe = scale(mLobe, vec3(dims.x * 0.65f, dims.y * 0.85f, dims.z * 0.65f));
    Primitives::drawSphere(shader, mLobe, col * 0.88f);
    shader.setInt("uUseTexture", 2);
}

// Helper 5: Vertical Rainwater Algae / Slime Drip Streak (বৃষ্টির জলের ছোপ ও কালচে শেওলার রেখা)
static void drawAlgaeDrip(Shader& shader, const mat4& parentModel, const vec3& topPos, float length, float width, const vec3& col)
{
    shader.setInt("uUseTexture", 0);
    mat4 m = parentModel;
    m = translate(m, topPos - vec3(0.0f, length * 0.5f, 0.0f));
    m = scale(m, vec3(width, length, 0.022f));
    Primitives::drawCube(shader, m, col);

    mat4 mTip = parentModel;
    mTip = translate(mTip, topPos - vec3(0.0f, length + width * 0.5f, 0.0f));
    mTip = scale(mTip, vec3(width * 0.55f, width * 1.1f, 0.022f));
    Primitives::drawCone(shader, mTip, col * 0.76f);
}

// Helper 6: Horizontal Algae & Slime Damp Perimeter Band (ভিটি ও দেওয়ালের স্যাঁতসেঁতে স্তর)
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
    shader.setInt("uUseTexture", 0);

    // ── Traditional Bangladeshi Village Mosque Palette ──
    const vec3 plinthEarth    (0.52f, 0.42f, 0.28f); // packed earthen foundation plinth (মাটির ভিটি)
    const vec3 plinthBrick    (0.62f, 0.28f, 0.16f); // brick-soling perimeter border & steps (ইটের গাঁথুনি)
    const vec3 timberPost     (0.42f, 0.26f, 0.14f); // seasoned dark Sal timber posts, purlins & frames
    const vec3 timberTrim     (0.62f, 0.40f, 0.18f); // warm jackfruit wood window/door frames & decorative trim
    const vec3 tinWallBase    (0.68f, 0.72f, 0.70f); // galvanized corrugated silver-gray tin wall sheet (ঢেউটিন)
    const vec3 tinWallRib     (0.78f, 0.82f, 0.80f); // 3D corrugated vertical wave ridge highlight
    const vec3 tinAccentGreen (0.16f, 0.48f, 0.38f); // traditional green painted tin border trim & plaques
    const vec3 roofConcrete   (0.55f, 0.60f, 0.58f); // sealed flat concrete roof slab (সমতল ছাদ)
    const vec3 antiqueBrass   (0.82f, 0.65f, 0.26f); // weathered turned brass spire finial (চাঁদ-তারা বিহীন কলস)
    const vec3 interiorGlow   (0.98f, 0.78f, 0.28f); // radiant warm golden candlelight prayer glow inside
    const vec3 waterCol       (0.12f, 0.54f, 0.60f); // tranquil blue-green ablution cistern water
    const vec3 bodnaRed       (0.78f, 0.22f, 0.16f); // traditional crimson clay ewer
    const vec3 bodnaClay      (0.74f, 0.38f, 0.20f); // natural terracotta Bodna
    const vec3 bodnaGreen     (0.16f, 0.42f, 0.25f); // ceramic green Bodna

    // Ozukhana & Masonry Accents
    const vec3 terracottaBrick (0.64f, 0.25f, 0.15f);
    const vec3 burntBrickDark  (0.34f, 0.12f, 0.07f);
    const vec3 oldRedStone     (0.50f, 0.25f, 0.20f);
    const vec3 algaeSlime      (0.10f, 0.20f, 0.08f);
    const vec3 slimeWet        (0.07f, 0.15f, 0.06f);

    // Primary Dimensions (Gramin Masjid)
    const float hallW   = 5.6f;   // width along X
    const float hallH   = 3.2f;   // wall height
    const float hallD   = 5.6f;   // depth along Z
    const float plinthH = 0.38f;  // raised earthen/brick plinth foundation
    const float halfW   = hallW * 0.5f; // 2.80f
    const float halfD   = hallD * 0.5f; // 2.80f

    // ── 1. Raised Earthen & Brick Plinth Foundation & Stepped Flight ──────
    mat4 plinth = model;
    plinth = translate(plinth, vec3(0.0f, plinthH * 0.5f, 0.0f));
    plinth = scale(plinth, vec3(hallW + 1.80f, plinthH, hallD + 2.00f));
    Primitives::drawCube(shader, plinth, plinthEarth);

    // Weathered red brick border coping along the plinth edge
    mat4 plinthBorder = model;
    plinthBorder = translate(plinthBorder, vec3(0.0f, plinthH - 0.02f, 0.0f));
    plinthBorder = scale(plinthBorder, vec3(hallW + 1.86f, 0.06f, hallD + 2.06f));
    Primitives::drawCube(shader, plinthBorder, plinthBrick);

    // Front Stepped Flight (red brick steps leading to the veranda)
    for (int s = 0; s < 3; ++s) {
        float sH = plinthH * (1.0f - (float)s * 0.28f);
        float sW = 2.6f + (float)s * 0.28f;
        float sZ = halfD + 1.15f + (float)s * 0.28f;
        mat4 step = model;
        step = translate(step, vec3(0.0f, sH * 0.5f, sZ));
        step = scale(step, vec3(sW, sH, 0.28f));
        Primitives::drawCube(shader, step, plinthBrick);
    }

    // Wooden Shoe Shelf beside the front steps
    mat4 shoeRack = model;
    shoeRack = translate(shoeRack, vec3(-1.85f, plinthH + 0.28f, halfD + 0.85f));
    shoeRack = scale(shoeRack, vec3(0.85f, 0.55f, 0.35f));
    Primitives::drawCube(shader, shoeRack, timberPost);

    for (int sh = 1; sh <= 2; ++sh) {
        mat4 shelf = model;
        shelf = translate(shelf, vec3(-1.85f, plinthH + (float)sh * 0.18f, halfD + 0.85f));
        shelf = scale(shelf, vec3(0.88f, 0.025f, 0.36f));
        Primitives::drawCube(shader, shelf, timberPost * 0.85f);
    }

    // ── 2. Timber Framework & Corrugated Tin Walls ──
    float wallCenterY = plinthH + hallH * 0.5f;

    // Corner Wooden Structural Posts
    const float postSize = 0.10f;
    for (float px : { -halfW + postSize * 0.5f, halfW - postSize * 0.5f }) {
        for (float pz : { -halfD + postSize * 0.5f, halfD - postSize * 0.5f }) {
            mat4 post = model;
            post = translate(post, vec3(px, wallCenterY, pz));
            post = scale(post, vec3(postSize, hallH, postSize));
            Primitives::drawCube(shader, post, timberPost);
        }
    }

    // Horizontal Timber Belt Courses
    for (float hRatio : { 0.12f, 0.50f, 0.94f }) {
        float hY = plinthH + hallH * hRatio;
        mat4 beltN = model;
        beltN = translate(beltN, vec3(0.0f, hY, halfD));
        beltN = scale(beltN, vec3(hallW + 0.06f, 0.06f, 0.06f));
        Primitives::drawCube(shader, beltN, timberPost);

        mat4 beltS = model;
        beltS = translate(beltS, vec3(0.0f, hY, -halfD));
        beltS = scale(beltS, vec3(hallW + 0.06f, 0.06f, 0.06f));
        Primitives::drawCube(shader, beltS, timberPost);

        mat4 beltW = model;
        beltW = translate(beltW, vec3(-halfW, hY, 0.0f));
        beltW = scale(beltW, vec3(0.06f, 0.06f, hallD + 0.06f));
        Primitives::drawCube(shader, beltW, timberPost);

        mat4 beltE = model;
        beltE = translate(beltE, vec3(halfW, hY, 0.0f));
        beltE = scale(beltE, vec3(0.06f, 0.06f, hallD + 0.06f));
        Primitives::drawCube(shader, beltE, timberPost);
    }

    // A. Left Wall (-X) Corrugated Tin Sheet & Wave Fluting
    mat4 wallLeft = model;
    wallLeft = translate(wallLeft, vec3(-halfW + 0.01f, wallCenterY, 0.0f));
    wallLeft = scale(wallLeft, vec3(0.022f, hallH, hallD - postSize * 2.0f));
    Primitives::drawCube(shader, wallLeft, tinWallBase);

    for (int r = 0; r < 24; ++r) {
        float rz = (-halfD + postSize + 0.10f) + (float)r * ((hallD - postSize * 2.0f - 0.20f) / 23.0f);
        mat4 rib = model;
        rib = translate(rib, vec3(-halfW - 0.005f, wallCenterY, rz));
        rib = scale(rib, vec3(0.018f, hallH, 0.032f));
        Primitives::drawCylinder(shader, rib, tinWallRib);
    }

    // B. Right Wall (+X) Corrugated Tin Sheet & Wave Fluting
    mat4 wallRight = model;
    wallRight = translate(wallRight, vec3(halfW - 0.01f, wallCenterY, 0.0f));
    wallRight = scale(wallRight, vec3(0.022f, hallH, hallD - postSize * 2.0f));
    Primitives::drawCube(shader, wallRight, tinWallBase);

    for (int r = 0; r < 24; ++r) {
        float rz = (-halfD + postSize + 0.10f) + (float)r * ((hallD - postSize * 2.0f - 0.20f) / 23.0f);
        mat4 rib = model;
        rib = translate(rib, vec3(halfW + 0.005f, wallCenterY, rz));
        rib = scale(rib, vec3(0.018f, hallH, 0.032f));
        Primitives::drawCylinder(shader, rib, tinWallRib);
    }

    // C. Rear Wall (-Z) Corrugated Tin Sheet & Wave Fluting
    mat4 wallRear = model;
    wallRear = translate(wallRear, vec3(0.0f, wallCenterY, -halfD + 0.01f));
    wallRear = scale(wallRear, vec3(hallW - postSize * 2.0f, hallH, 0.022f));
    Primitives::drawCube(shader, wallRear, tinWallBase);

    for (int r = 0; r < 24; ++r) {
        float rx = (-halfW + postSize + 0.10f) + (float)r * ((hallW - postSize * 2.0f - 0.20f) / 23.0f);
        mat4 rib = model;
        rib = translate(rib, vec3(rx, wallCenterY, -halfD - 0.005f));
        rib = scale(rib, vec3(0.032f, hallH, 0.018f));
        Primitives::drawCylinder(shader, rib, tinWallRib);
    }

    // D. Front Wall (+Z) Corrugated Tin Sheet & Wave Fluting
    mat4 wallFront = model;
    wallFront = translate(wallFront, vec3(0.0f, wallCenterY, halfD - 0.01f));
    wallFront = scale(wallFront, vec3(hallW - postSize * 2.0f, hallH, 0.022f));
    Primitives::drawCube(shader, wallFront, tinWallBase);

    for (int r = 0; r < 24; ++r) {
        float rx = (-halfW + postSize + 0.10f) + (float)r * ((hallW - postSize * 2.0f - 0.20f) / 23.0f);
        mat4 rib = model;
        rib = translate(rib, vec3(rx, wallCenterY, halfD + 0.005f));
        rib = scale(rib, vec3(0.032f, hallH, 0.018f));
        Primitives::drawCylinder(shader, rib, tinWallRib);
    }

    // ── 3. Covered Front Veranda Colonnade ────────
    const float porchDepth = 1.15f;
    const float porchZ     = halfD + porchDepth * 0.5f;
    const float porchFrontZ = halfD + porchDepth;

    // Raised veranda wooden floor deck
    mat4 porchFloor = model;
    porchFloor = translate(porchFloor, vec3(0.0f, plinthH + 0.02f, porchZ));
    porchFloor = scale(porchFloor, vec3(hallW * 0.88f, 0.04f, porchDepth));
    Primitives::drawCube(shader, porchFloor, timberPost);

    // Flat Veranda Canopy Ceiling/Roof (সমতল বারান্দার ছাদ)
    float vRoofY = plinthH + hallH * 0.86f;
    mat4 vRoof = model;
    vRoof = translate(vRoof, vec3(0.0f, vRoofY, porchZ + 0.05f));
    vRoof = scale(vRoof, vec3(hallW * 0.94f, 0.08f, porchDepth + 0.25f));
    Primitives::drawCube(shader, vRoof, roofConcrete);

    // Front Veranda Roof Parapet Trim
    mat4 vRoofTrim = model;
    vRoofTrim = translate(vRoofTrim, vec3(0.0f, vRoofY + 0.06f, porchFrontZ + 0.12f));
    vRoofTrim = scale(vRoofTrim, vec3(hallW * 0.96f, 0.05f, 0.06f));
    Primitives::drawCube(shader, vRoofTrim, tinAccentGreen);

    // 4 Seasoned Timber Veranda Pillars
    const float colX[4] = { -halfW * 0.40f, -halfW * 0.14f, halfW * 0.14f, halfW * 0.40f };
    const float colH = hallH * 0.82f;

    for (int c = 0; c < 4; ++c) {
        float cx = colX[c];
        mat4 pillar = model;
        pillar = translate(pillar, vec3(cx, plinthH + colH * 0.5f, porchFrontZ));
        pillar = scale(pillar, vec3(0.09f, colH, 0.09f));
        Primitives::drawCylinder(shader, pillar, timberPost);

        mat4 pCap = model;
        pCap = translate(pCap, vec3(cx, plinthH + colH - 0.04f, porchFrontZ));
        pCap = scale(pCap, vec3(0.14f, 0.08f, 0.14f));
        Primitives::drawCube(shader, pCap, timberTrim);
    }

    // Hanging Hurricane Lantern (Hariken)
    mat4 mChain = model;
    mChain = translate(mChain, vec3(0.0f, plinthH + hallH * 0.78f, porchZ));
    mChain = scale(mChain, vec3(0.015f, 0.22f, 0.015f));
    Primitives::drawCylinder(shader, mChain, antiqueBrass);

    mat4 mLantern = model;
    mLantern = translate(mLantern, vec3(0.0f, plinthH + hallH * 0.78f - 0.24f, porchZ));
    mLantern = scale(mLantern, vec3(0.85f, 0.85f, 0.85f));
    Charpai::drawLantern(shader, mLantern);

    // ── 4. Main Entrance Portal & Double Timber Doors ──
    const float doorW = 1.35f;
    const float doorH = 2.05f;
    const float doorZ = halfD;

    // Timber door frame
    mat4 dFrame = model;
    dFrame = translate(dFrame, vec3(0.0f, plinthH + doorH * 0.5f, doorZ + 0.04f));
    dFrame = scale(dFrame, vec3(doorW + 0.14f, doorH + 0.10f, 0.08f));
    Primitives::drawCube(shader, dFrame, timberPost);

    // Double timber door leaves
    for (int side = -1; side <= 1; side += 2) {
        float leafX = (float)side * (doorW * 0.245f);
        mat4 dLeaf = model;
        dLeaf = translate(dLeaf, vec3(leafX, plinthH + doorH * 0.5f, doorZ + 0.05f));
        dLeaf = scale(dLeaf, vec3(doorW * 0.48f, doorH, 0.045f));
        Primitives::drawCube(shader, dLeaf, timberTrim);

        for (int r = 1; r <= 3; ++r) {
            float sy = plinthH + (float)r * (doorH / 4.0f);
            mat4 strap = model;
            strap = translate(strap, vec3(leafX, sy, doorZ + 0.075f));
            strap = scale(strap, vec3(doorW * 0.42f, 0.03f, 0.015f));
            Primitives::drawCube(shader, strap, timberPost);

            mat4 stud = model;
            stud = translate(stud, vec3(leafX, sy, doorZ + 0.085f));
            stud = scale(stud, vec3(0.022f, 0.022f, 0.022f));
            Primitives::drawSphere(shader, stud, antiqueBrass);
        }

        mat4 handle = model;
        handle = translate(handle, vec3((float)side * 0.10f, plinthH + doorH * 0.50f, doorZ + 0.09f));
        handle = scale(handle, vec3(0.035f, 0.065f, 0.035f));
        Primitives::drawCylinder(shader, handle, antiqueBrass);
    }

    // Islamic Plaque over main door
    float transomH = 0.48f;
    float transomY = plinthH + doorH + 0.05f + transomH * 0.5f;

    mat4 plaqueBorder = model;
    plaqueBorder = translate(plaqueBorder, vec3(0.0f, transomY, doorZ + 0.045f));
    plaqueBorder = scale(plaqueBorder, vec3(doorW * 0.90f, transomH, 0.035f));
    Primitives::drawCube(shader, plaqueBorder, tinAccentGreen);

    shader.setFloat("emissive", 0.32f);
    mat4 tGlow = model;
    tGlow = translate(tGlow, vec3(0.0f, transomY, doorZ + 0.065f));
    tGlow = scale(tGlow, vec3(doorW * 0.78f, transomH * 0.76f, 0.015f));
    Primitives::drawCube(shader, tGlow, interiorGlow);
    shader.setFloat("emissive", 0.0f);

    // ── 5. FIXED FLAT ROOF SYSTEM (মসজিদের সম্পূর্ণ সমতল পাকার ছাদ - Flat Chad) ──
    float flatRoofY = plinthH + hallH; // exact top height of hall walls (Y = 3.58m)

    // Solid Interior Ceiling Panel (Seals any wall gaps completely)
    mat4 ceilingSlab = model;
    ceilingSlab = translate(ceilingSlab, vec3(0.0f, flatRoofY + 0.02f, 0.0f));
    ceilingSlab = scale(ceilingSlab, vec3(hallW + 0.12f, 0.05f, hallD + 0.12f));
    Primitives::drawCube(shader, ceilingSlab, timberPost);

    // Main Flat Roof Slab (ছাদ - Flat Roof Slab)
    const float roofOverhang = 0.50f;
    mat4 flatRoofSlab = model;
    flatRoofSlab = translate(flatRoofSlab, vec3(0.0f, flatRoofY + 0.09f, 0.0f));
    flatRoofSlab = scale(flatRoofSlab, vec3(hallW + roofOverhang, 0.14f, hallD + roofOverhang));
    Primitives::drawCube(shader, flatRoofSlab, roofConcrete);

    // Parapet Wall Border & Decorative Trim Coping around the 4 roof edges (ছাদের কার্নিশ ও প্যারাপেট)
    const float parapetH = 0.22f;
    const float parapetT = 0.10f;
    const float outerW = (hallW + roofOverhang) * 0.5f;
    const float outerD = (hallD + roofOverhang) * 0.5f;

    // Front Parapet (+Z)
    mat4 parFront = model;
    parFront = translate(parFront, vec3(0.0f, flatRoofY + 0.16f + parapetH * 0.5f, outerD - parapetT * 0.5f));
    parFront = scale(parFront, vec3(hallW + roofOverhang, parapetH, parapetT));
    Primitives::drawCube(shader, parFront, tinAccentGreen);

    // Rear Parapet (-Z)
    mat4 parRear = model;
    parRear = translate(parRear, vec3(0.0f, flatRoofY + 0.16f + parapetH * 0.5f, -outerD + parapetT * 0.5f));
    parRear = scale(parRear, vec3(hallW + roofOverhang, parapetH, parapetT));
    Primitives::drawCube(shader, parRear, tinAccentGreen);

    // Left Parapet (-X)
    mat4 parLeft = model;
    parLeft = translate(parLeft, vec3(-outerW + parapetT * 0.5f, flatRoofY + 0.16f + parapetH * 0.5f, 0.0f));
    parLeft = scale(parLeft, vec3(parapetT, parapetH, hallD + roofOverhang));
    Primitives::drawCube(shader, parLeft, tinAccentGreen);

    // Right Parapet (+X)
    mat4 parRight = model;
    parRight = translate(parRight, vec3(outerW - parapetT * 0.5f, flatRoofY + 0.16f + parapetH * 0.5f, 0.0f));
    parRight = scale(parRight, vec3(parapetT, parapetH, hallD + roofOverhang));
    Primitives::drawCube(shader, parRight, tinAccentGreen);

    // Parapet Molded Cap Trim
    mat4 parCap = model;
    parCap = translate(parCap, vec3(0.0f, flatRoofY + 0.16f + parapetH + 0.02f, 0.0f));
    parCap = scale(parCap, vec3(hallW + roofOverhang + 0.08f, 0.04f, hallD + roofOverhang + 0.08f));
    Primitives::drawCube(shader, parCap, timberTrim);

    // ── CENTRAL DOME SITUATED ON FLAT ROOF (সমতল ছাদের উপর কেন্দ্রীয় গম্বুজ) ──
    float domeBaseY = flatRoofY + 0.16f;

    // Octagonal Drum Collar Base
    mat4 drumBase = model;
    drumBase = translate(drumBase, vec3(0.0f, domeBaseY + 0.18f, 0.0f));
    drumBase = scale(drumBase, vec3(1.55f, 0.36f, 1.55f));
    Primitives::drawCylinder(shader, drumBase, tinAccentGreen);

    mat4 drumRim = model;
    drumRim = translate(drumRim, vec3(0.0f, domeBaseY + 0.37f, 0.0f));
    drumRim = scale(drumRim, vec3(1.65f, 0.06f, 1.65f));
    Primitives::drawCylinder(shader, drumRim, timberTrim);

    // Hemispherical Main Dome (সুন্দর গম্বুজ)
    mat4 mainDome = model;
    mainDome = translate(mainDome, vec3(0.0f, domeBaseY + 0.40f, 0.0f));
    mainDome = scale(mainDome, vec3(1.48f, 1.35f, 1.48f));
    Primitives::drawHemisphere(shader, mainDome, roofConcrete);

    // Decorative vertical ribbing across the main dome
    for (int dr = 0; dr < 8; ++dr) {
        float dAng = (float)dr * 45.0f;
        mat4 dRib = model;
        dRib = translate(dRib, vec3(0.0f, domeBaseY + 0.40f, 0.0f));
        dRib = rotate(dRib, radians(dAng), vec3(0.0f, 1.0f, 0.0f));
        dRib = translate(dRib, vec3(0.74f, 0.45f, 0.0f));
        dRib = scale(dRib, vec3(0.035f, 0.92f, 0.035f));
        Primitives::drawCylinder(shader, dRib, tinAccentGreen);
    }

    // Crowning Turned Brass Spire Finial (চাঁদ-তারা ছাড়া চূড়া / Top Spire Without Star)
    float mainSpireY = domeBaseY + 0.40f + 1.35f;
    drawTopSpire(shader, model, vec3(0.0f, mainSpireY, 0.0f), 1.45f, antiqueBrass);

    // ── 6. Azaan Minaret with Ahuja Horn Loudspeaker Sound System ──
    const float minX = -halfW - 0.15f;
    const float minZ =  halfD + 0.15f;
    const float minR = 0.42f;

    // Minaret Base
    mat4 mBase = model;
    mBase = translate(mBase, vec3(minX, plinthH + 0.75f, minZ));
    mBase = scale(mBase, vec3(1.10f, 1.50f, 1.10f));
    Primitives::drawCube(shader, mBase, plinthBrick);

    mat4 mCornice1 = model;
    mCornice1 = translate(mCornice1, vec3(minX, plinthH + 1.54f, minZ));
    mCornice1 = scale(mCornice1, vec3(1.18f, 0.08f, 1.18f));
    Primitives::drawCube(shader, mCornice1, timberPost);

    // Minaret Shaft
    const float shaftH = 4.80f;
    mat4 mShaft = model;
    mShaft = translate(mShaft, vec3(minX, plinthH + 1.58f + shaftH * 0.5f, minZ));
    mShaft = scale(mShaft, vec3(minR, shaftH, minR));
    Primitives::drawCylinder(shader, mShaft, tinWallBase);

    // Minaret Shaft Belt Rings
    for (int r = 1; r <= 3; ++r) {
        float rY = plinthH + 1.58f + shaftH * ((float)r / 4.0f);
        mat4 ring = model;
        ring = translate(ring, vec3(minX, rY, minZ));
        ring = scale(ring, vec3(minR * 1.16f, 0.08f, minR * 1.16f));
        Primitives::drawCylinder(shader, ring, timberPost);
    }

    // Minaret Azaan Balcony / Gallery
    float galleryY = plinthH + 1.58f + shaftH;

    mat4 galleryDeck = model;
    galleryDeck = translate(galleryDeck, vec3(minX, galleryY, minZ));
    galleryDeck = scale(galleryDeck, vec3(minR * 1.65f, 0.10f, minR * 1.65f));
    Primitives::drawCylinder(shader, galleryDeck, timberPost);

    mat4 railing = model;
    railing = translate(railing, vec3(minX, galleryY + 0.30f, minZ));
    railing = scale(railing, vec3(minR * 1.62f, 0.50f, minR * 1.62f));
    Primitives::drawCylinder(shader, railing, tinAccentGreen);

    // ── SOUND SYSTEM: 4 DIRECTIONAL AHUJA HORN LOUDSPEAKERS (চোঙা মাইক) ──
    float speakerY = galleryY + 0.38f;
    drawLoudspeaker(shader, model, vec3(minX, speakerY, minZ),   0.0f); // North
    drawLoudspeaker(shader, model, vec3(minX, speakerY, minZ),  90.0f); // East
    drawLoudspeaker(shader, model, vec3(minX, speakerY, minZ), 180.0f); // South
    drawLoudspeaker(shader, model, vec3(minX, speakerY, minZ), 270.0f); // West

    // Minaret Cupola Roof & Crowning Starless Spire
    float pavilH = 1.05f;
    for (int p = 0; p < 8; ++p) {
        float pAng = (float)p * 45.0f;
        float px = minX + cosf(radians(pAng)) * (minR * 0.88f);
        float pz = minZ + sinf(radians(pAng)) * (minR * 0.88f);

        mat4 pPillar = model;
        pPillar = translate(pPillar, vec3(px, galleryY + 0.55f + pavilH * 0.5f, pz));
        pPillar = scale(pPillar, vec3(0.045f, pavilH, 0.045f));
        Primitives::drawCylinder(shader, pPillar, timberPost);
    }

    float cupolaBaseY = galleryY + 0.55f + pavilH;
    mat4 minCupola = model;
    minCupola = translate(minCupola, vec3(minX, cupolaBaseY + 0.08f, minZ));
    minCupola = scale(minCupola, vec3(minR * 1.15f, minR * 1.25f, minR * 1.15f));
    Primitives::drawHemisphere(shader, minCupola, roofConcrete);

    // Minaret Top Spire (Starless Spire)
    drawTopSpire(shader, model, vec3(minX, cupolaBaseY + 0.08f + minR * 1.25f, minZ), 1.12f, antiqueBrass);

    // ── 7. Three Corner Decorative Pinnacles (Starless Top Spire) ──────────
    const float turrCorners[3][2] = {
        {  halfW + 0.10f,  halfD + 0.10f }, // Front-right
        { -halfW - 0.10f, -halfD - 0.10f }, // Rear-left
        {  halfW + 0.10f, -halfD - 0.10f }  // Rear-right
    };
    const float turrH = hallH + 1.10f;
    const float turrR = 0.25f;

    for (int t = 0; t < 3; ++t) {
        float tx = turrCorners[t][0];
        float tz = turrCorners[t][1];

        mat4 tShaft = model;
        tShaft = translate(tShaft, vec3(tx, plinthH + turrH * 0.5f, tz));
        tShaft = scale(tShaft, vec3(turrR, turrH, turrR));
        Primitives::drawCylinder(shader, tShaft, tinWallBase);

        float tCornY = plinthH + turrH;
        mat4 tCupola = model;
        tCupola = translate(tCupola, vec3(tx, tCornY + 0.08f, tz));
        tCupola = scale(tCupola, vec3(turrR * 1.12f, turrR * 1.18f, turrR * 1.12f));
        Primitives::drawHemisphere(shader, tCupola, roofConcrete);

        // Corner Turret Spire (Starless Spire)
        drawTopSpire(shader, model, vec3(tx, tCornY + 0.08f + turrR * 1.18f, tz), 0.82f, antiqueBrass);
    }

    // ── 8. Louvered Wooden Windows with Golden Interior Prayer Glow ─────────
    const float winW = 0.82f;
    const float winH = 1.15f;
    const float winY = plinthH + hallH * 0.50f;

    auto drawGraminWindow = [&](const vec3& pos, float rotYDeg) {
        mat4 wm = model;
        wm = translate(wm, pos);
        wm = rotate(wm, radians(rotYDeg), vec3(0.0f, 1.0f, 0.0f));

        mat4 casing = wm;
        casing = translate(casing, vec3(0.0f, 0.0f, 0.02f));
        casing = scale(casing, vec3(winW + 0.14f, winH + 0.14f, 0.05f));
        Primitives::drawCube(shader, casing, timberTrim);

        shader.setFloat("emissive", 0.32f);
        mat4 glow = wm;
        glow = translate(glow, vec3(0.0f, 0.0f, -0.01f));
        glow = scale(glow, vec3(winW - 0.08f, winH - 0.08f, 0.02f));
        Primitives::drawCube(shader, glow, interiorGlow);
        shader.setFloat("emissive", 0.0f);

        for (int v = -2; v <= 2; ++v) {
            mat4 barV = wm;
            barV = translate(barV, vec3((float)v * (winW * 0.14f), 0.0f, 0.03f));
            barV = scale(barV, vec3(0.032f, winH * 0.82f, 0.025f));
            Primitives::drawCube(shader, barV, timberPost);
        }
        for (int h = -2; h <= 2; ++h) {
            mat4 barH = wm;
            barH = translate(barH, vec3(0.0f, (float)h * (winH * 0.15f), 0.03f));
            barH = scale(barH, vec3(winW * 0.82f, 0.032f, 0.025f));
            Primitives::drawCube(shader, barH, timberPost);
        }
    };

    // Windows placement
    drawGraminWindow(vec3(-halfW * 0.62f, winY, halfD + 0.02f), 0.0f);
    drawGraminWindow(vec3( halfW * 0.62f, winY, halfD + 0.02f), 0.0f);

    drawGraminWindow(vec3(-halfW - 0.04f, winY, -1.2f), -90.0f);
    drawGraminWindow(vec3(-halfW - 0.04f, winY,  1.2f), -90.0f);

    drawGraminWindow(vec3( halfW + 0.04f, winY, -1.2f),  90.0f);
    drawGraminWindow(vec3( halfW + 0.04f, winY,  1.2f),  90.0f);

    // ── 9. Western Mehrab Bay ────────────
    float mehrabW = 1.65f;
    float mehrabH = 2.40f;
    float mehrabD = 0.68f;

    mat4 mehrab = model;
    mehrab = translate(mehrab, vec3(0.0f, plinthH + mehrabH * 0.5f, -halfD - mehrabD * 0.5f));
    mehrab = scale(mehrab, vec3(mehrabW, mehrabH, mehrabD));
    Primitives::drawCube(shader, mehrab, tinWallBase);

    mat4 mehrabFrame = model;
    mehrabFrame = translate(mehrabFrame, vec3(0.0f, plinthH + mehrabH * 0.5f, -halfD - mehrabD * 0.5f));
    mehrabFrame = scale(mehrabFrame, vec3(mehrabW + 0.08f, mehrabH + 0.06f, mehrabD + 0.06f));
    Primitives::drawCube(shader, mehrabFrame, timberPost);

    mat4 mehrabDome = model;
    mehrabDome = translate(mehrabDome, vec3(0.0f, plinthH + mehrabH + 0.12f, -halfD - mehrabD * 0.5f));
    mehrabDome = scale(mehrabDome, vec3(0.48f, 0.40f, 0.48f));
    Primitives::drawHemisphere(shader, mehrabDome, roofConcrete);

    drawTopSpire(shader, model, vec3(0.0f, plinthH + mehrabH + 0.52f, -halfD - mehrabD * 0.5f), 0.55f, antiqueBrass);

    // ── 10. Traditional Ablution Area ────
    float ozuX = halfW + 1.25f;
    float ozuZ = 0.5f;

    // Cistern walls
    mat4 ozuTank = model;
    ozuTank = translate(ozuTank, vec3(ozuX, 0.32f, ozuZ));
    ozuTank = scale(ozuTank, vec3(1.40f, 0.64f, 2.30f));
    Primitives::drawCube(shader, ozuTank, terracottaBrick);

    mat4 ozuCoping = model;
    ozuCoping = translate(ozuCoping, vec3(ozuX, 0.65f, ozuZ));
    ozuCoping = scale(ozuCoping, vec3(1.52f, 0.06f, 2.42f));
    Primitives::drawCube(shader, ozuCoping, burntBrickDark);

    // Ablution water surface
    shader.setInt("uUseTexture", 0);
    mat4 ozuWater = model;
    ozuWater = translate(ozuWater, vec3(ozuX, 0.652f, ozuZ));
    ozuWater = scale(ozuWater, vec3(1.22f, 1.0f, 2.12f));
    Primitives::drawPlane(shader, ozuWater, waterCol);
    shader.setInt("uUseTexture", 2);

    // Washing bench
    mat4 bench = model;
    bench = translate(bench, vec3(ozuX + 1.05f, 0.20f, ozuZ));
    bench = scale(bench, vec3(0.48f, 0.40f, 2.30f));
    Primitives::drawCube(shader, bench, oldRedStone);

    mat4 benchTop = model;
    benchTop = translate(benchTop, vec3(ozuX + 1.05f, 0.41f, ozuZ));
    benchTop = scale(benchTop, vec3(0.52f, 0.03f, 2.34f));
    Primitives::drawCube(shader, benchTop, burntBrickDark);

    // 3 Brass Water Taps
    for (int tap = -1; tap <= 1; ++tap) {
        float tz = ozuZ + (float)tap * 0.68f;

        shader.setInt("uUseTexture", 0);
        mat4 pipe = model;
        pipe = translate(pipe, vec3(ozuX + 0.68f, 0.48f, tz));
        pipe = scale(pipe, vec3(0.12f, 0.022f, 0.022f));
        Primitives::drawCube(shader, pipe, antiqueBrass);

        mat4 spout = model;
        spout = translate(spout, vec3(ozuX + 0.74f, 0.44f, tz));
        spout = scale(spout, vec3(0.022f, 0.06f, 0.022f));
        Primitives::drawCylinder(shader, spout, antiqueBrass);

        drawAlgaeDrip(shader, model, vec3(ozuX + 0.71f, 0.42f, tz), 0.38f, 0.12f, slimeWet);
        drawMossCushion(shader, model, vec3(ozuX + 0.74f, 0.06f, tz), vec3(0.16f, 0.06f, 0.22f), 0.0f, algaeSlime);
    }

    drawSlimeBand(shader, model, vec3(ozuX + 0.82f, 0.025f, ozuZ), vec3(0.35f, 0.02f, 2.10f), slimeWet);

    // 3 Handcrafted Bodnas
    drawBodna(shader, model, vec3(ozuX + 1.05f, 0.42f, ozuZ - 0.68f), 0.98f, bodnaRed);
    drawBodna(shader, model, vec3(ozuX + 1.05f, 0.42f, ozuZ),         0.92f, bodnaGreen);
    drawBodna(shader, model, vec3(ozuX + 1.05f, 0.42f, ozuZ + 0.68f), 0.96f, bodnaClay);
}

} // namespace Mosque
