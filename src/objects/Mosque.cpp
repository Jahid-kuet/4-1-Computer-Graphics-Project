// Mosque.cpp — Historic Old-Style Bengali Rural Village Mosque (ঐতিহাসিক প্রাচীন গ্রামীণ মসজিদ)
// Inspired by ancient Sultanate & Mughal Bengal terracotta brick mosques
// (e.g., Goaldi Mosque, Sonargaon; Chunakhola & Bagerhat Heritage Mosques).
// Procedurally constructed in OpenGL 3.3 Core Profile using canonical geometric primitives.
// Features:
// 1. Weathered terracotta clay brick masonry with kiln variations & lime-surki mortar
// 2. The signature curved Bengal Sultanate roof cornice (বাঙালি বাঁকানো কার্নিশ / Dochala curve)
// 3. Sturdy multi-tiered octagonal corner buttress towers (Burj) with molded ring bands
// 4. Central grand terracotta brick dome on an octagonal transition drum with lotus Kalasa
// 5. Multi-cusped pointed terracotta arched entrance portal with carved rosette spandrels
// 6. Historic dark Sal timber double doors with forged iron fittings & hanging Hariken
// 7. Terracotta perforated lattice windows (Jali) with warm golden interior prayer glow
// 8. Projecting semi-octagonal western Mehrab bay on the rear Qibla wall
// 9. Traditional brick-paved courtyard & open masonry ablution cistern (Paka Houz) with clay Bodnas

#include "objects/Mosque.h"
#include "objects/Charpai.h"
#include "Primitives.h"
#include <cmath>

using namespace math;

namespace Mosque {

// Helper: draws a traditional handcrafted clay ablution ewer (Matir Bodna)
static void drawClayBodna(Shader& shader, const mat4& parentModel, const vec3& pos, float scaleVal, const vec3& clayCol)
{
    mat4 bm = parentModel;
    bm = translate(bm, pos);
    bm = scale(bm, vec3(scaleVal));

    // Spherical / bulbous clay water reservoir body
    mat4 body = bm;
    body = translate(body, vec3(0.0f, 0.10f, 0.0f));
    body = scale(body, vec3(0.12f, 0.10f, 0.12f));
    Primitives::drawSphere(shader, body, clayCol);

    // Flared base rim
    mat4 base = bm;
    base = translate(base, vec3(0.0f, 0.015f, 0.0f));
    base = scale(base, vec3(0.07f, 0.03f, 0.07f));
    Primitives::drawCylinder(shader, base, clayCol);

    // Tapered neck
    mat4 neck = bm;
    neck = translate(neck, vec3(0.0f, 0.18f, 0.0f));
    neck = scale(neck, vec3(0.045f, 0.06f, 0.045f));
    Primitives::drawCylinder(shader, neck, clayCol);

    // Flared open mouth rim
    mat4 rim = bm;
    rim = translate(rim, vec3(0.0f, 0.22f, 0.0f));
    rim = scale(rim, vec3(0.065f, 0.02f, 0.065f));
    Primitives::drawCylinder(shader, rim, clayCol);

    // Curved angled pouring spout (characteristic Bengali Bodna spout)
    mat4 spout = bm;
    spout = translate(spout, vec3(0.07f, 0.14f, 0.0f));
    spout = rotate(spout, radians(-45.0f), vec3(0.0f, 0.0f, 1.0f));
    spout = scale(spout, vec3(0.025f, 0.11f, 0.025f));
    Primitives::drawCone(shader, spout, clayCol);

    // Arched loop handle
    mat4 handle = bm;
    handle = translate(handle, vec3(-0.08f, 0.15f, 0.0f));
    handle = scale(handle, vec3(0.02f, 0.09f, 0.02f));
    Primitives::drawCube(shader, handle, clayCol);
}

void draw(Shader& shader, const mat4& model)
{
    // ── Ancient Bengal Terracotta Color Palette ──────────────────
    const vec3 brickMain    (0.68f, 0.28f, 0.15f); // warm terracotta burnt red clay brick
    const vec3 brickDark    (0.50f, 0.19f, 0.10f); // deep weathered kiln-fired brick
    const vec3 brickRelief  (0.78f, 0.35f, 0.18f); // terracotta relief plaques & rosettes
    const vec3 brickDentil  (0.60f, 0.23f, 0.12f); // molded brick dentil courses
    const vec3 limeStucco   (0.78f, 0.74f, 0.66f); // weathered lime-surki mortar bands
    const vec3 domeBrick    (0.64f, 0.25f, 0.13f); // aged terracotta dome brickwork
    const vec3 domeTrim     (0.54f, 0.21f, 0.11f); // dome base collar ring
    const vec3 brassFinial  (0.82f, 0.66f, 0.22f); // antique brass/terracotta Kalasa spire
    const vec3 doorTimber   (0.24f, 0.13f, 0.06f); // weathered dark seasoned Sal timber
    const vec3 jaliScreen   (0.58f, 0.24f, 0.12f); // terracotta openwork lattice screen
    const vec3 windowGlow   (0.98f, 0.74f, 0.26f); // warm interior earthen chirag lamp glow
    const vec3 plinthBrick  (0.48f, 0.22f, 0.12f); // weathered foundation plinth brick
    const vec3 waterCol     (0.12f, 0.38f, 0.42f); // ablution cistern water
    const vec3 clayEwerCol  (0.74f, 0.36f, 0.19f); // terracotta Bodna ewer

    // Base Dimensions
    const float hallW   = 5.6f;   // width along X
    const float hallH   = 3.3f;   // wall height
    const float hallD   = 5.8f;   // depth along Z
    const float plinthH = 0.45f;  // raised foundation plinth height
    const float halfW   = hallW * 0.5f;
    const float halfD   = hallD * 0.5f;

    // ── 1. Raised Terracotta Brick Plinth & Stepped Flight ────────
    // Stepped foundation platform (Viti)
    mat4 plinth = model;
    plinth = translate(plinth, vec3(0.0f, plinthH * 0.5f, 0.0f));
    plinth = scale(plinth, vec3(hallW + 1.8f, plinthH, hallD + 1.8f));
    Primitives::drawCube(shader, plinth, plinthBrick);

    // Molded brick plinth top coping band
    mat4 plinthCoping = model;
    plinthCoping = translate(plinthCoping, vec3(0.0f, plinthH + 0.02f, 0.0f));
    plinthCoping = scale(plinthCoping, vec3(hallW + 1.9f, 0.04f, hallD + 1.9f));
    Primitives::drawCube(shader, plinthCoping, brickDark);

    // Front Stepped Flight (4 wide terracotta brick stairs leading up to entrance portal at +Z)
    for (int s = 0; s < 4; s++) {
        float sH = plinthH * (1.0f - (float)s * 0.22f);
        float sW = 2.6f + (float)s * 0.35f;
        float sZ = halfD + 0.20f + (float)s * 0.32f;

        mat4 step = model;
        step = translate(step, vec3(0.0f, sH * 0.5f, sZ));
        step = scale(step, vec3(sW, sH, 0.34f));
        Primitives::drawCube(shader, step, (s % 2 == 0) ? brickMain : brickDark);
    }

    // ── 2. Main Terracotta Brick Prayer Hall (Namaz Ghor) ─────────
    float wallCenterY = plinthH + hallH * 0.5f;
    mat4 walls = model;
    walls = translate(walls, vec3(0.0f, wallCenterY, 0.0f));
    walls = scale(walls, vec3(hallW, hallH, hallD));
    Primitives::drawCube(shader, walls, brickMain);

    // Molded terracotta base courses (Adhisthana moldings) around the base of the walls
    for (int b = 0; b < 3; b++) {
        float bY = plinthH + (float)b * 0.11f + 0.055f;
        float bExtra = 0.08f - (float)b * 0.025f;
        mat4 baseM = model;
        baseM = translate(baseM, vec3(0.0f, bY, 0.0f));
        baseM = scale(baseM, vec3(hallW + bExtra, 0.09f, hallD + bExtra));
        Primitives::drawCube(shader, baseM, (b == 1) ? limeStucco : brickDark);
    }

    // Mid-wall molded terracotta string courses (Dori / Chain moldings)
    mat4 midBand1 = model;
    midBand1 = translate(midBand1, vec3(0.0f, plinthH + hallH * 0.48f, 0.0f));
    midBand1 = scale(midBand1, vec3(hallW + 0.06f, 0.08f, hallD + 0.06f));
    Primitives::drawCube(shader, midBand1, brickDark);

    mat4 midBand2 = model;
    midBand2 = translate(midBand2, vec3(0.0f, plinthH + hallH * 0.52f, 0.0f));
    midBand2 = scale(midBand2, vec3(hallW + 0.04f, 0.05f, hallD + 0.04f));
    Primitives::drawCube(shader, midBand2, limeStucco);

    // Recessed Terracotta Wall Panels (Traditional Sultanate blind panels)
    auto drawWallPanels = [&](float zPos, float rotYDeg, float spanW) {
        int numPanels = 4;
        float panelW = (spanW - 1.2f) / (float)numPanels;
        for (int p = 0; p < numPanels; ++p) {
            float px = -spanW * 0.5f + 0.6f + ((float)p + 0.5f) * panelW;
            // Lower register panel
            mat4 pM1 = model;
            pM1 = translate(pM1, vec3(px, plinthH + hallH * 0.28f, zPos));
            pM1 = rotate(pM1, radians(rotYDeg), vec3(0.0f, 1.0f, 0.0f));
            pM1 = scale(pM1, vec3(panelW * 0.75f, hallH * 0.26f, 0.03f));
            Primitives::drawCube(shader, pM1, brickDark);

            // Inner terracotta carved rosette
            mat4 rM = pM1;
            rM = translate(rM, vec3(0.0f, 0.0f, 0.02f));
            rM = scale(rM, vec3(0.18f, 0.18f, 0.02f));
            Primitives::drawSphere(shader, rM, brickRelief);
        }
    };
    // Lateral panels on east and west walls
    drawWallPanels(-halfW - 0.01f, 90.0f, hallD);
    drawWallPanels( halfW + 0.01f, 90.0f, hallD);

    // ── 3. The Signature Curved Bengal Sultanate Cornice (বাঙালি বাঁকানো কার্নিশ) ──
    const float archH = 0.22f; // graceful parabolic sag-rise typical of Bengal Sultanate
    const int corniceSegments = 20;
    const float segStepX = (hallW + 0.20f) / (float)corniceSegments;

    for (int side = -1; side <= 1; side += 2) {
        float cz = (float)side * halfD;
        for (int i = 0; i < corniceSegments; ++i) {
            float x0 = -(hallW + 0.20f) * 0.5f + (float)i * segStepX;
            float x1 = x0 + segStepX;
            float xMid = (x0 + x1) * 0.5f;

            float normX = (2.0f * xMid) / hallW;
            float curveY = archH * (1.0f - normX * normX);
            float slopeAngle = std::atan(-8.0f * archH * xMid / (hallW * hallW));
            float yPos = plinthH + hallH + curveY;

            // Curved molded terracotta cornice beam (rotated to match arch tangent)
            mat4 cBeam = model;
            cBeam = translate(cBeam, vec3(xMid, yPos, cz + (float)side * 0.06f));
            cBeam = rotate(cBeam, slopeAngle, vec3(0.0f, 0.0f, 1.0f));
            cBeam = scale(cBeam, vec3(segStepX * 1.08f, 0.12f, 0.18f));
            Primitives::drawCube(shader, cBeam, brickDark);

            // Upper curved molded lime-stucco string course
            mat4 cTrim = model;
            cTrim = translate(cTrim, vec3(xMid, yPos + 0.08f, cz + (float)side * 0.07f));
            cTrim = rotate(cTrim, slopeAngle, vec3(0.0f, 0.0f, 1.0f));
            cTrim = scale(cTrim, vec3(segStepX * 1.08f, 0.04f, 0.20f));
            Primitives::drawCube(shader, cTrim, limeStucco);

            // Terracotta dentil tooth under the cornice
            if (i % 2 == 0) {
                mat4 dentil = model;
                dentil = translate(dentil, vec3(xMid, yPos - 0.08f, cz + (float)side * 0.08f));
                dentil = scale(dentil, vec3(segStepX * 0.55f, 0.05f, 0.10f));
                Primitives::drawCube(shader, dentil, brickDentil);
            }
        }
    }

    // Flat roof platform behind the curved parapets
    mat4 roof = model;
    roof = translate(roof, vec3(0.0f, plinthH + hallH + 0.02f, 0.0f));
    roof = scale(roof, vec3(hallW + 0.10f, 0.06f, hallD + 0.10f));
    Primitives::drawCube(shader, roof, brickDark);

    // ── 4. Four Sturdy Octagonal Corner Buttress Towers (ঐতিহাসিক কর্নার বুরুজ) ──
    const float minH = hallH + 1.25f; // 4.55m sturdy medieval buttress towers
    const float minR = 0.38f;        // thick robust medieval masonry radius
    const float corners[4][2] = {
        { -halfW - 0.08f,  halfD + 0.08f }, // Front-left tower
        {  halfW + 0.08f,  halfD + 0.08f }, // Front-right tower
        { -halfW - 0.08f, -halfD - 0.08f }, // Rear-left tower
        {  halfW + 0.08f, -halfD - 0.08f }  // Rear-right tower
    };

    for (int c = 0; c < 4; c++) {
        float cx = corners[c][0];
        float cz = corners[c][1];

        // Front-left tower is slightly taller for the traditional village Azaan platform
        bool isAzaanTower = (c == 0);
        float h = isAzaanTower ? (minH + 1.10f) : minH;

        // Stepped octagonal base
        mat4 tBase = model;
        tBase = translate(tBase, vec3(cx, plinthH + 0.20f, cz));
        tBase = scale(tBase, vec3(minR * 1.30f, 0.40f, minR * 1.30f));
        Primitives::drawCylinder(shader, tBase, brickDark);

        // Tower shaft
        mat4 tShaft = model;
        tShaft = translate(tShaft, vec3(cx, plinthH + h * 0.5f, cz));
        tShaft = scale(tShaft, vec3(minR, h, minR));
        Primitives::drawCylinder(shader, tShaft, brickMain);

        // Horizontal molded terracotta belt rings (Kumbha bands) dividing tower into 3 registers
        for (int r = 1; r <= 3; r++) {
            float rY = plinthH + h * ((float)r / 4.0f);
            mat4 ring = model;
            ring = translate(ring, vec3(cx, rY, cz));
            ring = scale(ring, vec3(minR * 1.24f, 0.09f, minR * 1.24f));
            Primitives::drawCylinder(shader, ring, (r == 2) ? limeStucco : brickDark);
        }

        // Bracketed overhanging cornice (Chhatri base)
        float cornY = plinthH + h;
        mat4 bCorn1 = model;
        bCorn1 = translate(bCorn1, vec3(cx, cornY + 0.04f, cz));
        bCorn1 = scale(bCorn1, vec3(minR * 1.35f, 0.08f, minR * 1.35f));
        Primitives::drawCylinder(shader, bCorn1, brickDark);

        mat4 bCorn2 = model;
        bCorn2 = translate(bCorn2, vec3(cx, cornY + 0.10f, cz));
        bCorn2 = scale(bCorn2, vec3(minR * 1.50f, 0.06f, minR * 1.50f));
        Primitives::drawCylinder(shader, bCorn2, limeStucco);

        // Crowning ribbed terracotta cupola dome (Chhatri / Gombuz)
        mat4 cupola = model;
        cupola = translate(cupola, vec3(cx, cornY + 0.13f, cz));
        cupola = scale(cupola, vec3(minR * 1.20f, minR * 1.25f, minR * 1.20f));
        Primitives::drawHemisphere(shader, cupola, domeBrick);

        // Kalasa lotus crest and spire
        mat4 kLotus = model;
        kLotus = translate(kLotus, vec3(cx, cornY + 0.13f + minR * 1.25f, cz));
        kLotus = scale(kLotus, vec3(minR * 0.45f, 0.06f, minR * 0.45f));
        Primitives::drawCylinder(shader, kLotus, brassFinial);

        mat4 kSpire = model;
        kSpire = translate(kSpire, vec3(cx, cornY + 0.13f + minR * 1.25f + 0.05f, cz));
        kSpire = scale(kSpire, vec3(0.065f, 0.38f, 0.065f));
        Primitives::drawCone(shader, kSpire, brassFinial);

        mat4 kTip = model;
        kTip = translate(kTip, vec3(cx, cornY + 0.13f + minR * 1.25f + 0.40f, cz));
        kTip = scale(kTip, vec3(0.06f, 0.06f, 0.06f));
        Primitives::drawSphere(shader, kTip, brassFinial);
    }

    // ── 5. Central Grand Terracotta Dome (Boro Gombuj) ────────────
    float domeBaseY = plinthH + hallH + 0.06f;

    // Octagonal transition drum with molded brick blind niches
    mat4 drum = model;
    drum = translate(drum, vec3(0.0f, domeBaseY + 0.28f, 0.0f));
    drum = scale(drum, vec3(1.75f, 0.56f, 1.75f));
    Primitives::drawCylinder(shader, drum, brickDark);

    // Decorative lime mortar band on drum
    mat4 drumBand = model;
    drumBand = translate(drumBand, vec3(0.0f, domeBaseY + 0.54f, 0.0f));
    drumBand = scale(drumBand, vec3(1.82f, 0.07f, 1.82f));
    Primitives::drawCylinder(shader, drumBand, limeStucco);

    // Circular stepped lotus collar (Padma pith)
    mat4 lotusCollar = model;
    lotusCollar = translate(lotusCollar, vec3(0.0f, domeBaseY + 0.58f, 0.0f));
    lotusCollar = scale(lotusCollar, vec3(1.86f, 0.08f, 1.86f));
    Primitives::drawCylinder(shader, lotusCollar, domeTrim);

    // Main Hemispherical Terracotta Dome
    mat4 dome = model;
    dome = translate(dome, vec3(0.0f, domeBaseY + 0.62f, 0.0f));
    dome = scale(dome, vec3(1.80f, 1.45f, 1.80f));
    Primitives::drawHemisphere(shader, dome, domeBrick);

    // Concentric terracotta ribbing on dome
    mat4 domeRib = model;
    domeRib = translate(domeRib, vec3(0.0f, domeBaseY + 1.25f, 0.0f));
    domeRib = scale(domeRib, vec3(1.45f, 0.05f, 1.45f));
    Primitives::drawCylinder(shader, domeRib, domeTrim);

    // Multi-tier Antique Brass Kalasa (Pot & Lotus Spire)
    float kalasaBaseY = domeBaseY + 0.62f + 1.45f;

    // Tier 1: Inverted lotus plate
    mat4 kalasaBase = model;
    kalasaBase = translate(kalasaBase, vec3(0.0f, kalasaBaseY + 0.06f, 0.0f));
    kalasaBase = scale(kalasaBase, vec3(0.38f, 0.10f, 0.38f));
    Primitives::drawCylinder(shader, kalasaBase, brassFinial);

    // Tier 2: Terracotta Ghata (round water pot form)
    mat4 kalasaPot = model;
    kalasaPot = translate(kalasaPot, vec3(0.0f, kalasaBaseY + 0.22f, 0.0f));
    kalasaPot = scale(kalasaPot, vec3(0.24f, 0.22f, 0.24f));
    Primitives::drawSphere(shader, kalasaPot, brassFinial);

    // Tier 3: Tapered slender spire cone
    mat4 kalasaSpire = model;
    kalasaSpire = translate(kalasaSpire, vec3(0.0f, kalasaBaseY + 0.35f, 0.0f));
    kalasaSpire = scale(kalasaSpire, vec3(0.12f, 0.70f, 0.12f));
    Primitives::drawCone(shader, kalasaSpire, brassFinial);

    // Tier 4: Apex lotus bud sphere
    mat4 kalasaApex = model;
    kalasaApex = translate(kalasaApex, vec3(0.0f, kalasaBaseY + 1.05f, 0.0f));
    kalasaApex = scale(kalasaApex, vec3(0.10f, 0.10f, 0.10f));
    Primitives::drawSphere(shader, kalasaApex, brassFinial);

    // ── 6. Multi-Cusped Terracotta Pointed Entrance Portal (ঐতিহাসিক খিলান তোরণ) ──
    // Authentic Bengal Sultanate arched Pishtaq with recessed wooden doorway (+Z)
    const float portalW = 2.15f;
    const float portalH = 2.55f;
    const float doorZ   = halfD;
    const float openW   = 1.28f;
    const float openH   = 2.05f;
    const float pierW   = (portalW - openW) * 0.5f; // 0.435m flanking pier width

    // Left and Right Projecting Terracotta Piers (Jambs)
    for (int side = -1; side <= 1; side += 2) {
        float px = (float)side * (openW * 0.5f + pierW * 0.5f);

        // Main pier brickwork
        mat4 pier = model;
        pier = translate(pier, vec3(px, plinthH + portalH * 0.5f, doorZ + 0.14f));
        pier = scale(pier, vec3(pierW, portalH, 0.28f));
        Primitives::drawCube(shader, pier, brickDark);

        // Molded terracotta plinth base for each pier
        mat4 pierBase = model;
        pierBase = translate(pierBase, vec3(px, plinthH + 0.07f, doorZ + 0.15f));
        pierBase = scale(pierBase, vec3(pierW + 0.06f, 0.14f, 0.30f));
        Primitives::drawCube(shader, pierBase, brickRelief);

        // Impost capital molding band (where the arch begins to spring)
        mat4 pierCap = model;
        pierCap = translate(pierCap, vec3(px, plinthH + openH - 0.52f, doorZ + 0.15f));
        pierCap = scale(pierCap, vec3(pierW + 0.05f, 0.08f, 0.30f));
        Primitives::drawCube(shader, pierCap, limeStucco);

        // Slender vertical molded pilaster relief on pier face
        mat4 pierRib = model;
        pierRib = translate(pierRib, vec3(px, plinthH + portalH * 0.5f, doorZ + 0.28f));
        pierRib = scale(pierRib, vec3(0.06f, portalH * 0.88f, 0.03f));
        Primitives::drawCube(shader, pierRib, brickRelief);
    }

    // Upper Entablature / Frieze (above the arched opening)
    float friezeH = portalH - openH;
    mat4 frieze = model;
    frieze = translate(frieze, vec3(0.0f, plinthH + openH + friezeH * 0.5f, doorZ + 0.14f));
    frieze = scale(frieze, vec3(portalW, friezeH, 0.28f));
    Primitives::drawCube(shader, frieze, brickDark);

    // Molded terracotta relief frieze band
    mat4 friezeBand = model;
    friezeBand = translate(friezeBand, vec3(0.0f, plinthH + openH + friezeH * 0.5f, doorZ + 0.285f));
    friezeBand = scale(friezeBand, vec3(portalW + 0.08f, 0.08f, 0.03f));
    Primitives::drawCube(shader, friezeBand, brickRelief);

    // Molded Lime-Stucco Framing Border around the Portal (Pishtaq Border)
    for (int side = -1; side <= 1; side += 2) {
        mat4 borderCol = model;
        borderCol = translate(borderCol, vec3((float)side * (portalW * 0.5f + 0.03f), plinthH + portalH * 0.5f, doorZ + 0.26f));
        borderCol = scale(borderCol, vec3(0.06f, portalH + 0.06f, 0.05f));
        Primitives::drawCube(shader, borderCol, limeStucco);
    }

    // Top projecting molded cornice crown over the portal
    mat4 pCrown = model;
    pCrown = translate(pCrown, vec3(0.0f, plinthH + portalH + 0.05f, doorZ + 0.14f));
    pCrown = scale(pCrown, vec3(portalW + 0.22f, 0.10f, 0.34f));
    Primitives::drawCube(shader, pCrown, limeStucco);

    // Multi-cusped Pointed Arch spanning the opening
    // Outer terracotta arch ring
    mat4 pArchOuter = model;
    pArchOuter = translate(pArchOuter, vec3(0.0f, plinthH + openH - 0.50f, doorZ + 0.22f));
    pArchOuter = scale(pArchOuter, vec3(openW * 1.02f, 1.00f, 0.14f));
    Primitives::drawArch(shader, pArchOuter, brickRelief);

    // Inner arch soffit
    mat4 pArchInner = model;
    pArchInner = translate(pArchInner, vec3(0.0f, plinthH + openH - 0.50f, doorZ + 0.13f));
    pArchInner = scale(pArchInner, vec3(openW * 0.88f, 0.86f, 0.22f));
    Primitives::drawArch(shader, pArchInner, brickDark);

    // Terracotta carved rosettes in the arch spandrels
    for (int side = -1; side <= 1; side += 2) {
        mat4 rosette = model;
        rosette = translate(rosette, vec3((float)side * (openW * 0.5f + 0.16f), plinthH + openH - 0.14f, doorZ + 0.275f));
        rosette = scale(rosette, vec3(0.13f, 0.13f, 0.035f));
        Primitives::drawSphere(shader, rosette, brickRelief);
    }

    // Deeply Recessed Wooden Doorway (Inside the entrance archway)
    const float dH = 1.60f;
    const float dW = 1.02f;

    // Carved brick threshold sill (Choukath Pith)
    mat4 dSill = model;
    dSill = translate(dSill, vec3(0.0f, plinthH + 0.035f, doorZ + 0.06f));
    dSill = scale(dSill, vec3(dW + 0.14f, 0.07f, 0.16f));
    Primitives::drawCube(shader, dSill, brickDark);

    // Heavy Sal Timber Frame (Choukath)
    for (int side = -1; side <= 1; side += 2) {
        mat4 cPost = model;
        cPost = translate(cPost, vec3((float)side * (dW * 0.5f + 0.035f), plinthH + dH * 0.5f, doorZ + 0.04f));
        cPost = scale(cPost, vec3(0.07f, dH, 0.07f));
        Primitives::drawCube(shader, cPost, doorTimber);
    }
    mat4 cLintel = model;
    cLintel = translate(cLintel, vec3(0.0f, plinthH + dH + 0.035f, doorZ + 0.04f));
    cLintel = scale(cLintel, vec3(dW + 0.14f, 0.07f, 0.07f));
    Primitives::drawCube(shader, cLintel, doorTimber);

    // Heavy weathered Sal timber double doors
    for (int side = -1; side <= 1; side += 2) {
        float leafX = (float)side * (dW * 0.245f);

        // Timber leaf
        mat4 doorLeaf = model;
        doorLeaf = translate(doorLeaf, vec3(leafX, plinthH + dH * 0.5f, doorZ + 0.04f));
        doorLeaf = scale(doorLeaf, vec3(dW * 0.48f, dH, 0.045f));
        Primitives::drawCube(shader, doorLeaf, doorTimber);

        // Forged iron reinforcement strap hinges (3 horizontal bands per leaf)
        for (int r = 1; r <= 3; ++r) {
            float strapY = plinthH + (float)r * (dH / 4.0f);
            mat4 strap = model;
            strap = translate(strap, vec3(leafX, strapY, doorZ + 0.068f));
            strap = scale(strap, vec3(dW * 0.44f, 0.032f, 0.015f));
            Primitives::drawCube(shader, strap, brickDark);

            // Forged iron studs along the strap
            for (int s = -1; s <= 1; ++s) {
                mat4 stud = model;
                stud = translate(stud, vec3(leafX + (float)s * 0.13f, strapY, doorZ + 0.078f));
                stud = scale(stud, vec3(0.022f, 0.022f, 0.022f));
                Primitives::drawSphere(shader, stud, brassFinial);
            }
        }
    }

    // Central forged iron meeting stile & locking bolt
    mat4 lockBar = model;
    lockBar = translate(lockBar, vec3(0.0f, plinthH + dH * 0.5f, doorZ + 0.068f));
    lockBar = scale(lockBar, vec3(0.032f, dH * 0.90f, 0.02f));
    Primitives::drawCube(shader, lockBar, brickDark);

    // Antique brass forged ring handles
    for (int side = -1; side <= 1; side += 2) {
        mat4 ringHandle = model;
        ringHandle = translate(ringHandle, vec3((float)side * 0.075f, plinthH + dH * 0.52f, doorZ + 0.082f));
        ringHandle = scale(ringHandle, vec3(0.032f, 0.065f, 0.032f));
        Primitives::drawCylinder(shader, ringHandle, brassFinial);
    }

    // Terracotta Perforated Lattice Transom / Tympanum (above door lintel, inside arch)
    float tympanumH = openH - dH - 0.10f;
    float tympanumY = plinthH + dH + 0.07f + tympanumH * 0.5f;

    // Warm golden prayer glow radiating through the transom
    shader.setFloat("emissive", 0.85f);
    mat4 tGlow = model;
    tGlow = translate(tGlow, vec3(0.0f, tympanumY, doorZ + 0.03f));
    tGlow = scale(tGlow, vec3(openW * 0.72f, tympanumH, 0.02f));
    Primitives::drawCube(shader, tGlow, windowGlow);
    shader.setFloat("emissive", 0.0f);

    // Transom terracotta lattice bars
    for (int v = -2; v <= 2; ++v) {
        mat4 tBarV = model;
        tBarV = translate(tBarV, vec3((float)v * 0.12f, tympanumY, doorZ + 0.045f));
        tBarV = scale(tBarV, vec3(0.025f, tympanumH * 0.90f, 0.025f));
        Primitives::drawCube(shader, tBarV, jaliScreen);
    }

    // Arched Chiragdan (recessed oil-lamp niche) on both sides of entrance
    for (int side = -1; side <= 1; side += 2) {
        float nx = (float)side * (portalW * 0.5f + 0.40f);
        float ny = plinthH + 1.25f;
        mat4 nicheFrame = model;
        nicheFrame = translate(nicheFrame, vec3(nx, ny, doorZ + 0.02f));
        nicheFrame = scale(nicheFrame, vec3(0.32f, 0.46f, 0.08f));
        Primitives::drawCube(shader, nicheFrame, brickDark);

        mat4 nicheArch = model;
        nicheArch = translate(nicheArch, vec3(nx, ny + 0.16f, doorZ + 0.02f));
        nicheArch = scale(nicheArch, vec3(0.28f, 0.22f, 0.09f));
        Primitives::drawArch(shader, nicheArch, brickRelief);

        // Glowing oil lamp inside niche
        shader.setFloat("emissive", 0.85f);
        mat4 lampGlow = model;
        lampGlow = translate(lampGlow, vec3(nx, ny - 0.06f, doorZ + 0.01f));
        lampGlow = scale(lampGlow, vec3(0.06f, 0.08f, 0.04f));
        Primitives::drawSphere(shader, lampGlow, windowGlow);
        shader.setFloat("emissive", 0.0f);
    }

    // Hanging Kerosene Lantern (Hariken) suspended under the entrance portal archway
    mat4 chain = model;
    chain = translate(chain, vec3(0.0f, plinthH + openH - 0.08f, doorZ + 0.18f));
    chain = scale(chain, vec3(0.015f, 0.18f, 0.015f));
    Primitives::drawCylinder(shader, chain, brassFinial);

    mat4 mLantern = model;
    mLantern = translate(mLantern, vec3(0.0f, plinthH + openH - 0.22f, doorZ + 0.18f));
    mLantern = scale(mLantern, vec3(0.85f, 0.85f, 0.85f));
    Charpai::drawLantern(shader, mLantern);

    // ── 7. Terracotta Jali Openwork Windows (পোড়ামাটির জালি বাতায়ন) ──
    const float winW = 0.78f;
    const float winH = 1.18f;
    const float winY = plinthH + hallH * 0.52f;

    auto drawTerracottaWindow = [&](const vec3& pos, float rotYDeg) {
        mat4 wm = model;
        wm = translate(wm, pos);
        wm = rotate(wm, radians(rotYDeg), vec3(0.0f, 1.0f, 0.0f));

        // Hollow molded terracotta outer rectangular casing
        // Left jamb
        mat4 jambL = wm;
        jambL = translate(jambL, vec3(-winW * 0.5f + 0.04f, 0.0f, 0.0f));
        jambL = scale(jambL, vec3(0.08f, winH, 0.10f));
        Primitives::drawCube(shader, jambL, brickDark);

        // Right jamb
        mat4 jambR = wm;
        jambR = translate(jambR, vec3(winW * 0.5f - 0.04f, 0.0f, 0.0f));
        jambR = scale(jambR, vec3(0.08f, winH, 0.10f));
        Primitives::drawCube(shader, jambR, brickDark);

        // Molded projecting window sill
        mat4 sill = wm;
        sill = translate(sill, vec3(0.0f, -winH * 0.5f - 0.035f, 0.02f));
        sill = scale(sill, vec3(winW + 0.10f, 0.07f, 0.15f));
        Primitives::drawCube(shader, sill, brickRelief);

        // Molded window lintel
        mat4 lintel = wm;
        lintel = translate(lintel, vec3(0.0f, winH * 0.5f + 0.035f, 0.01f));
        lintel = scale(lintel, vec3(winW + 0.06f, 0.07f, 0.13f));
        Primitives::drawCube(shader, lintel, brickRelief);

        // Pointed terracotta arch crowning the window top
        mat4 arch = wm;
        arch = translate(arch, vec3(0.0f, winH * 0.44f, 0.02f));
        arch = scale(arch, vec3(winW * 0.88f, 0.46f, 0.12f));
        Primitives::drawArch(shader, arch, brickRelief);

        // Glowing interior prayer sanctuary light behind jali
        shader.setFloat("emissive", 0.82f);
        mat4 glow = wm;
        glow = translate(glow, vec3(0.0f, 0.0f, -0.01f));
        glow = scale(glow, vec3(winW - 0.12f, winH - 0.12f, 0.02f));
        Primitives::drawCube(shader, glow, windowGlow);
        shader.setFloat("emissive", 0.0f);

        // Terracotta Jali (Perforated brick lattice grid)
        // 4 vertical bars
        for (int v = -2; v <= 2; ++v) {
            mat4 barV = wm;
            barV = translate(barV, vec3((float)v * (winW * 0.14f), 0.0f, 0.03f));
            barV = scale(barV, vec3(0.035f, winH * 0.80f, 0.03f));
            Primitives::drawCube(shader, barV, jaliScreen);
        }
        // 5 horizontal bars
        for (int h = -2; h <= 2; ++h) {
            mat4 barH = wm;
            barH = translate(barH, vec3(0.0f, (float)h * (winH * 0.15f), 0.03f));
            barH = scale(barH, vec3(winW * 0.78f, 0.035f, 0.03f));
            Primitives::drawCube(shader, barH, jaliScreen);
        }
    };

    // Front facade windows (flanking entrance portal)
    drawTerracottaWindow(vec3(-halfW * 0.60f, winY, doorZ + 0.02f), 0.0f);
    drawTerracottaWindow(vec3( halfW * 0.60f, winY, doorZ + 0.02f), 0.0f);

    // Left (West) wall windows (2 along -X)
    drawTerracottaWindow(vec3(-halfW - 0.04f, winY, -1.2f), -90.0f);
    drawTerracottaWindow(vec3(-halfW - 0.04f, winY,  1.2f), -90.0f);

    // Right (East) wall windows (2 along +X)
    drawTerracottaWindow(vec3( halfW + 0.04f, winY, -1.2f),  90.0f);
    drawTerracottaWindow(vec3( halfW + 0.04f, winY,  1.2f),  90.0f);

    // ── 8. Projecting Western Mehrab Bay (পশ্চিম দেওয়ালের মেহরাব) ──
    // Authentic semi-octagonal projection on the rear wall (-Z)
    float mehrabW = 1.65f;
    float mehrabH = 2.40f;
    float mehrabD = 0.65f;
    mat4 mehrab = model;
    mehrab = translate(mehrab, vec3(0.0f, plinthH + mehrabH * 0.5f, -halfD - mehrabD * 0.5f));
    mehrab = scale(mehrab, vec3(mehrabW, mehrabH, mehrabD));
    Primitives::drawCube(shader, mehrab, brickDark);

    // Curved cornice crowning the Mehrab projection
    mat4 mehrabCornice = model;
    mehrabCornice = translate(mehrabCornice, vec3(0.0f, plinthH + mehrabH + 0.06f, -halfD - mehrabD * 0.5f));
    mehrabCornice = scale(mehrabCornice, vec3(mehrabW + 0.20f, 0.12f, mehrabD + 0.15f));
    Primitives::drawCube(shader, mehrabCornice, limeStucco);

    // Mini decorative cupola atop the Mehrab projection
    mat4 mehrabCupola = model;
    mehrabCupola = translate(mehrabCupola, vec3(0.0f, plinthH + mehrabH + 0.16f, -halfD - mehrabD * 0.5f));
    mehrabCupola = scale(mehrabCupola, vec3(0.42f, 0.35f, 0.42f));
    Primitives::drawHemisphere(shader, mehrabCupola, domeBrick);

    // ── 9. Traditional Brick-Paved Uthan & Open Masonry Ablution Houz (পাকা হাউজ ও মাটির বদনা) ──
    // Located on the eastern courtyard flank of the mosque platform
    float ozuX = halfW + 1.20f;
    float ozuZ = 0.6f;

    // Raised red brick ablution cistern walls (Paka Houz)
    mat4 ozuTank = model;
    ozuTank = translate(ozuTank, vec3(ozuX, 0.30f, ozuZ));
    ozuTank = scale(ozuTank, vec3(1.35f, 0.60f, 2.20f));
    Primitives::drawCube(shader, ozuTank, brickDark);

    // Molded terracotta coping ledge around the cistern
    mat4 ozuCoping = model;
    ozuCoping = translate(ozuCoping, vec3(ozuX, 0.62f, ozuZ));
    ozuCoping = scale(ozuCoping, vec3(1.48f, 0.06f, 2.32f));
    Primitives::drawCube(shader, ozuCoping, brickMain);

    // Clean ablution water surface within cistern
    mat4 ozuWater = model;
    ozuWater = translate(ozuWater, vec3(ozuX, 0.54f, ozuZ));
    ozuWater = scale(ozuWater, vec3(1.15f, 1.0f, 1.95f));
    Primitives::drawPlane(shader, ozuWater, waterCol);

    // Low brick seating bench for worshippers
    mat4 bench = model;
    bench = translate(bench, vec3(ozuX + 0.95f, 0.18f, ozuZ));
    bench = scale(bench, vec3(0.42f, 0.36f, 2.20f));
    Primitives::drawCube(shader, bench, brickDark);

    // 3 Handcrafted Clay Ablution Ewers (Matir Bodna) resting on the brick washing bench
    drawClayBodna(shader, model, vec3(ozuX + 0.95f, 0.36f, ozuZ - 0.65f), 1.0f, clayEwerCol);
    drawClayBodna(shader, model, vec3(ozuX + 0.95f, 0.36f, ozuZ),         0.92f, brickMain);
    drawClayBodna(shader, model, vec3(ozuX + 0.95f, 0.36f, ozuZ + 0.65f), 0.96f, clayEwerCol);
}

} // namespace Mosque
