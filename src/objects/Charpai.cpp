// Charpai.cpp — Traditional woven bed and hand-fan built from
//               cylinder legs, cuboid frame, criss-cross weave, and fan blades.

#include "objects/Charpai.h"
#include "Primitives.h"

using namespace math;

namespace Charpai {

void draw(Shader& shader, const mat4& model)
{
    vec3 legCol     (0.38f, 0.24f, 0.10f); // seasoned dark timber
    vec3 ringCol    (0.48f, 0.32f, 0.14f); // turned collar highlight
    vec3 frameCol   (0.44f, 0.28f, 0.12f); // timber mortise frame rails
    vec3 weaveCol   (0.64f, 0.56f, 0.36f); // golden-brown natural jute twine
    vec3 weaveDark  (0.52f, 0.44f, 0.26f); // shadow weave strips
    vec3 kanthaCol  (0.68f, 0.16f, 0.14f); // folded crimson Nakshi Kantha
    vec3 kanthaTrim (0.92f, 0.88f, 0.82f); // white embroidered border stitch

    // Charpai dimensions
    float bedW = 1.2f;   // width (X)
    float bedL = 2.0f;   // length (Z)
    float bedH = 0.5f;   // height (Y)
    float legR = 0.044f;

    // ── 1. Four Lathe-Turned Timber Legs with Bell Collars ──────
    float hx = bedW * 0.5f - 0.05f;
    float hz = bedL * 0.5f - 0.05f;
    float legPos[4][2] = { {-hx, -hz}, {hx, -hz}, {hx, hz}, {-hx, hz} };

    for (int i = 0; i < 4; i++) {
        float lx = legPos[i][0];
        float lz = legPos[i][1];

        // Main leg shaft
        mat4 leg = model;
        leg = translate(leg, vec3(lx, bedH * 0.48f, lz));
        leg = scale(leg, vec3(legR, bedH * 0.96f, legR));
        Primitives::drawCylinder(shader, leg, legCol);

        // Lower turned bell footing
        mat4 footRing = model;
        footRing = translate(footRing, vec3(lx, 0.06f, lz));
        footRing = scale(footRing, vec3(legR * 1.35f, 0.040f, legR * 1.35f));
        Primitives::drawCylinder(shader, footRing, ringCol);

        // Mid-height decorative carved collar
        mat4 midRing = model;
        midRing = translate(midRing, vec3(lx, bedH * 0.45f, lz));
        midRing = scale(midRing, vec3(legR * 1.38f, 0.045f, legR * 1.38f));
        Primitives::drawCylinder(shader, midRing, ringCol);

        // Upper neck bead
        mat4 neckRing = model;
        neckRing = translate(neckRing, vec3(lx, bedH * 0.82f, lz));
        neckRing = scale(neckRing, vec3(legR * 1.25f, 0.035f, legR * 1.25f));
        Primitives::drawCylinder(shader, neckRing, ringCol);

        // Corner post tenon cap extending above frame rail
        mat4 cap = model;
        cap = translate(cap, vec3(lx, bedH + 0.025f, lz));
        cap = scale(cap, vec3(0.068f, 0.050f, 0.068f));
        Primitives::drawCube(shader, cap, legCol);

        mat4 capTop = model;
        capTop = translate(capTop, vec3(lx, bedH + 0.050f, lz));
        capTop = scale(capTop, vec3(0.034f, 0.020f, 0.034f));
        Primitives::drawSphere(shader, capTop, ringCol);
    }

    // ── 2. Heavy Timber Frame Rails with Mortise Joints ─────────
    // Long sides (Z direction)
    for (int side = -1; side <= 1; side += 2) {
        mat4 rail = model;
        rail = translate(rail, vec3(side * hx, bedH, 0.0f));
        rail = scale(rail, vec3(0.052f, 0.055f, bedL));
        Primitives::drawCube(shader, rail, frameCol);
    }
    // Short sides (X direction)
    for (int side = -1; side <= 1; side += 2) {
        mat4 rail = model;
        rail = translate(rail, vec3(0.0f, bedH, side * hz));
        rail = scale(rail, vec3(bedW, 0.055f, 0.052f));
        Primitives::drawCube(shader, rail, frameCol);
    }

    // ── 3. Dense Realistic Jute Rope Webbing (Dori-r Buno) ───────
    // Transverse webbing straps (22 bands along Z)
    int numZ = 22;
    for (int i = 0; i < numZ; i++) {
        float t = (float)i / (numZ - 1);
        float z = -hz + 0.06f + t * (bedL - 0.22f);
        mat4 strip = model;
        strip = translate(strip, vec3(0.0f, bedH - 0.015f, z));
        strip = scale(strip, vec3(bedW * 0.88f, 0.012f, 0.028f));
        Primitives::drawCube(shader, strip, (i % 2 == 0) ? weaveCol : weaveDark);
    }

    // Longitudinal webbing straps (14 bands along X)
    int numX = 14;
    for (int i = 0; i < numX; i++) {
        float t = (float)i / (numX - 1);
        float x = -hx + 0.05f + t * (bedW - 0.10f);
        mat4 strip = model;
        strip = translate(strip, vec3(x, bedH - 0.010f, 0.0f));
        strip = scale(strip, vec3(0.026f, 0.012f, bedL * 0.88f));
        Primitives::drawCube(shader, strip, (i % 2 == 0) ? weaveDark : weaveCol);
    }

    // ── 4. Traditional Folded Bengali Quilt (Nakshi Kantha) ─────
    mat4 kantha = model;
    kantha = translate(kantha, vec3(0.0f, bedH + 0.018f, -hz + 0.24f));
    kantha = scale(kantha, vec3(bedW * 0.84f, 0.036f, 0.38f));
    Primitives::drawCube(shader, kantha, kanthaCol);

    // Embroidered white running-stitch border on folded kantha
    mat4 kanthaStitch = model;
    kanthaStitch = translate(kanthaStitch, vec3(0.0f, bedH + 0.037f, -hz + 0.24f));
    kanthaStitch = scale(kanthaStitch, vec3(bedW * 0.78f, 0.005f, 0.32f));
    Primitives::drawCube(shader, kanthaStitch, kanthaTrim);
}

void drawFan(Shader& shader, const mat4& model, float)
{
    // Authentic Traditional Bangladeshi Handmade Fan (Nakshi Haat Pakha / নকশী হাতপাখা)
    // Modeled procedurally from canonical geometric primitives matching the traditional Bengali cloth-ruffled fan:
    // 1. Bamboo Handle with wrapped red cotton grip, decorative white ferrules, and structural spine
    // 2. Wide Accordion-Pleated White Cloth Frills / Ruffles (ঝালর) radiating around the outer rim
    // 3. Circular Fan Disc (crimson red woven fabric) facing the viewer (XY plane)
    // 4. Outer Bent Split-Bamboo Binding Hoop (outer rim)
    // 5. Traditional Concentric White Nakshi / Alpana Ornamental Bands & Center Rosette

    // Traditional Color Palette
    vec3 handleWood  (0.48f, 0.35f, 0.18f); // natural seasoned bamboo cane
    vec3 gripRed     (0.80f, 0.18f, 0.14f); // crimson cotton wrapped grip
    vec3 whiteTrim   (0.95f, 0.95f, 0.97f); // white pleated cloth ruffles & Nakshi paint
    vec3 discRed     (0.82f, 0.16f, 0.12f); // vibrant crimson red fabric disc
    vec3 rimBamboo   (0.58f, 0.44f, 0.22f); // split-bamboo binding hoop

    float fanCenterY = 0.14f;
    float discRadius = 0.188f;

    // ── 1. Bamboo Handle, Grip Wrap & Structural Spine ─────────────────────────
    // Exposed lower bamboo handle tip
    mat4 handleTip = model;
    handleTip = translate(handleTip, vec3(0.0f, -0.30f, 0.0f));
    handleTip = scale(handleTip, vec3(0.010f, 0.040f, 0.010f));
    Primitives::drawCylinder(shader, handleTip, handleWood);

    // Wrapped red cotton grip section (where the hand holds the fan)
    mat4 grip = model;
    grip = translate(grip, vec3(0.0f, -0.19f, 0.0f));
    grip = scale(grip, vec3(0.013f, 0.180f, 0.013f));
    Primitives::drawCylinder(shader, grip, gripRed);

    // Lower decorative white ferrule / binding band
    mat4 ferruleBot = model;
    ferruleBot = translate(ferruleBot, vec3(0.0f, -0.275f, 0.0f));
    ferruleBot = scale(ferruleBot, vec3(0.0145f, 0.012f, 0.0145f));
    Primitives::drawCylinder(shader, ferruleBot, whiteTrim);

    // Upper decorative white ferrule / binding band
    mat4 ferruleTop = model;
    ferruleTop = translate(ferruleTop, vec3(0.0f, -0.105f, 0.0f));
    ferruleTop = scale(ferruleTop, vec3(0.0145f, 0.012f, 0.0145f));
    Primitives::drawCylinder(shader, ferruleTop, whiteTrim);

    // Bamboo shaft neck connecting grip to the fan disc
    mat4 neck = model;
    neck = translate(neck, vec3(0.0f, -0.065f, 0.0f));
    neck = scale(neck, vec3(0.011f, 0.070f, 0.011f));
    Primitives::drawCylinder(shader, neck, handleWood);

    // Structural bamboo spine running up through the back of the disc
    mat4 spine = model;
    spine = translate(spine, vec3(0.0f, fanCenterY, -0.007f));
    spine = scale(spine, vec3(0.009f, discRadius * 2.05f, 0.007f));
    Primitives::drawCylinder(shader, spine, handleWood);

    // ── 2. Wide Accordion-Pleated Cloth Frills / Ruffles (ঝালর) ────────────────
    // Radiating pleated ruffles surrounding the full 360-degree perimeter
    const int numPleats = 36;
    float ruffMidR = discRadius + 0.032f; // center of the ruffle band
    float ruffLen  = 0.064f;             // radial length of each ruffle
    float ruffWid  = 0.022f;             // tangential width
    float ruffThk  = 0.003f;             // thickness

    for (int i = 0; i < numPleats; i++) {
        float theta = (float)i * (2.0f * PI / (float)numPleats);
        float px = cosf(theta) * ruffMidR;
        float py = fanCenterY + sinf(theta) * ruffMidR;

        // Alternating accordion pleat angle (+18 deg / -18 deg)
        float pleatTilt = (i % 2 == 0) ? 18.0f : -18.0f;

        mat4 pleat = model;
        pleat = translate(pleat, vec3(px, py, 0.0f));
        // Rotate so pleat length points radially outward along angle theta
        pleat = rotate(pleat, theta - (PI * 0.5f), vec3(0.0f, 0.0f, 1.0f));
        // Accordion tilt along radial axis for true 3D cloth folding facets
        pleat = rotate(pleat, radians(pleatTilt), vec3(0.0f, 1.0f, 0.0f));
        pleat = scale(pleat, vec3(ruffWid, ruffLen, ruffThk));
        Primitives::drawCube(shader, pleat, whiteTrim);
    }

    // ── 3. Main Circular Fan Disc (Crimson Red Fabric / Woven Ground) ──────────
    // Rotated 90 degrees around X so circular face lies directly in the XY plane!
    mat4 disc = model;
    disc = translate(disc, vec3(0.0f, fanCenterY, 0.0f));
    disc = rotate(disc, radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
    disc = scale(disc, vec3(discRadius, 0.006f, discRadius));
    Primitives::drawCylinder(shader, disc, discRed);

    // ── 4. Outer Split-Bamboo Binding Hoop (Rim Border) ────────────────────────
    // Upper and lower semi-circular arches forming the circular rim border
    mat4 hoopTop = model;
    hoopTop = translate(hoopTop, vec3(0.0f, fanCenterY, 0.0f));
    hoopTop = scale(hoopTop, vec3(discRadius * 2.0f, discRadius * 2.0f, 0.009f));
    Primitives::drawArch(shader, hoopTop, rimBamboo);

    mat4 hoopBot = model;
    hoopBot = translate(hoopBot, vec3(0.0f, fanCenterY, 0.0f));
    hoopBot = rotate(hoopBot, radians(180.0f), vec3(0.0f, 0.0f, 1.0f));
    hoopBot = scale(hoopBot, vec3(discRadius * 2.0f, discRadius * 2.0f, 0.009f));
    Primitives::drawArch(shader, hoopBot, rimBamboo);

    // ── 5. Traditional Nakshi / Alpana Ornamental Embroidery (White on Red) ───
    // Outer white beaded border ring (R = 0.170m)
    mat4 ring1Top = model;
    ring1Top = translate(ring1Top, vec3(0.0f, fanCenterY, 0.004f));
    ring1Top = scale(ring1Top, vec3(0.340f, 0.340f, 0.003f));
    Primitives::drawArch(shader, ring1Top, whiteTrim);

    mat4 ring1Bot = model;
    ring1Bot = translate(ring1Bot, vec3(0.0f, fanCenterY, 0.004f));
    ring1Bot = rotate(ring1Bot, radians(180.0f), vec3(0.0f, 0.0f, 1.0f));
    ring1Bot = scale(ring1Bot, vec3(0.340f, 0.340f, 0.003f));
    Primitives::drawArch(shader, ring1Bot, whiteTrim);

    // Outer white dotted pearl ring (24 pearls around R = 0.174m, matching traditional Nakshi border)
    for (int d = 0; d < 24; d++) {
        float da = (float)d * (2.0f * PI / 24.0f);
        float dx = cosf(da) * 0.174f;
        float dy = fanCenterY + sinf(da) * 0.174f;
        mat4 dot = model;
        dot = translate(dot, vec3(dx, dy, 0.0045f));
        dot = scale(dot, vec3(0.007f, 0.007f, 0.003f));
        Primitives::drawSphere(shader, dot, whiteTrim);
    }

    // Middle ornamental white ring (R = 0.130m)
    mat4 ring2Top = model;
    ring2Top = translate(ring2Top, vec3(0.0f, fanCenterY, 0.0045f));
    ring2Top = scale(ring2Top, vec3(0.260f, 0.260f, 0.003f));
    Primitives::drawArch(shader, ring2Top, whiteTrim);

    mat4 ring2Bot = model;
    ring2Bot = translate(ring2Bot, vec3(0.0f, fanCenterY, 0.0045f));
    ring2Bot = rotate(ring2Bot, radians(180.0f), vec3(0.0f, 0.0f, 1.0f));
    ring2Bot = scale(ring2Bot, vec3(0.260f, 0.260f, 0.003f));
    Primitives::drawArch(shader, ring2Bot, whiteTrim);

    // Middle ornamental white leaf/scroll motifs (12 accents around R = 0.130m)
    for (int m = 0; m < 12; m++) {
        float ma = (float)m * (2.0f * PI / 12.0f);
        float mx = cosf(ma) * 0.130f;
        float my = fanCenterY + sinf(ma) * 0.130f;
        mat4 motif = model;
        motif = translate(motif, vec3(mx, my, 0.0048f));
        motif = rotate(motif, ma, vec3(0.0f, 0.0f, 1.0f));
        motif = scale(motif, vec3(0.016f, 0.007f, 0.003f));
        Primitives::drawCube(shader, motif, whiteTrim);
    }

    // Inner white medallion disc (R = 0.082m)
    mat4 innerWhite = model;
    innerWhite = translate(innerWhite, vec3(0.0f, fanCenterY, 0.005f));
    innerWhite = rotate(innerWhite, radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
    innerWhite = scale(innerWhite, vec3(0.082f, 0.002f, 0.082f));
    Primitives::drawCylinder(shader, innerWhite, whiteTrim);

    // Red inner motif field (R = 0.074m)
    mat4 innerRed = model;
    innerRed = translate(innerRed, vec3(0.0f, fanCenterY, 0.0055f));
    innerRed = rotate(innerRed, radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
    innerRed = scale(innerRed, vec3(0.074f, 0.002f, 0.074f));
    Primitives::drawCylinder(shader, innerRed, discRed);

    // Center white Paisley / Peacock Feather (Kalka) rosette hub (R = 0.038m)
    mat4 kalkaHub = model;
    kalkaHub = translate(kalkaHub, vec3(0.0f, fanCenterY, 0.006f));
    kalkaHub = rotate(kalkaHub, radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
    kalkaHub = scale(kalkaHub, vec3(0.038f, 0.002f, 0.038f));
    Primitives::drawCylinder(shader, kalkaHub, whiteTrim);

    // Radial white Nakshi decorative accent petals around the kalka medallion
    for (int p = 0; p < 8; p++) {
        float pa = (float)p * (2.0f * PI / 8.0f);
        float px = cosf(pa) * 0.054f;
        float py = fanCenterY + sinf(pa) * 0.054f;
        mat4 petal = model;
        petal = translate(petal, vec3(px, py, 0.0062f));
        petal = scale(petal, vec3(0.010f, 0.010f, 0.003f));
        Primitives::drawSphere(shader, petal, whiteTrim);
    }
}

void drawLantern(Shader& shader, const mat4& model)
{
    // Authentic Tubular Kerosene Hurricane Lantern (Traditional Rural Bengali Hariken)
    // Modeled procedurally from canonical geometric primitives matching vintage tubular lanterns:
    // 1. Broad Flanged Base Rim & Tiered Fount (Oil Reservoir Tank) with Angled Fuel Filler Cap
    // 2. Brass Burner Throat with Wick Thumbwheel Adjuster Dial & Globe Gallery Basket
    // 3. Bulbous Glass Globe (Chimney) with Luminous Kerosene Flame & Inner Incandescent Core
    // 4. Protective Wire Guard Cage (Horizontal Hoops & Crossed Struts)
    // 5. Dual Tubular Side Air Tubes (Left & Right) with Handle Eyelet Brackets
    // 6. Tiered Ventilated Chimney Cowl (Lower Skirt, Louvered Stack, Upper Canopy & Top Pull Ring)
    // 7. Tall Arched Wire Bail Handle

    // Vintage weathered patinated materials
    vec3 metalCol  (0.20f, 0.25f, 0.26f); // antique patinated oxidized tin/slate-metal
    vec3 rustCol   (0.38f, 0.26f, 0.16f); // weathered rust/bronze rim accents
    vec3 brassCol  (0.64f, 0.50f, 0.22f); // aged brass (burner, filler cap, wick dial)
    vec3 glassCol  (0.96f, 0.86f, 0.56f); // warm glowing translucent kerosene glass globe
    vec3 flameCol  (1.00f, 0.82f, 0.22f); // glowing golden kerosene flame
    vec3 flameCore (1.00f, 0.96f, 0.65f); // hot white-gold flame core
    vec3 wireCol   (0.18f, 0.17f, 0.16f); // dark spring-steel wire (cage & bail handle)

    // ── 1. Base / Oil Reservoir Tank (Fount) ─────────────────────────────────
    // Flanged bottom base rim (wide & thin disk)
    mat4 baseRim = model;
    baseRim = translate(baseRim, vec3(0.0f, 0.007f, 0.0f));
    baseRim = scale(baseRim, vec3(0.098f, 0.014f, 0.098f));
    Primitives::drawCylinder(shader, baseRim, rustCol);

    // Main cylindrical oil tank body
    mat4 fountBase = model;
    fountBase = translate(fountBase, vec3(0.0f, 0.024f, 0.0f));
    fountBase = scale(fountBase, vec3(0.090f, 0.022f, 0.090f));
    Primitives::drawCylinder(shader, fountBase, metalCol);

    // Domed upper shoulder of the fount
    mat4 fountDome = model;
    fountDome = translate(fountDome, vec3(0.0f, 0.035f, 0.0f));
    fountDome = scale(fountDome, vec3(0.088f, 0.022f, 0.088f));
    Primitives::drawHemisphere(shader, fountDome, metalCol);

    // Stepped collar ring on top of fount
    mat4 fountCollar = model;
    fountCollar = translate(fountCollar, vec3(0.0f, 0.052f, 0.0f));
    fountCollar = scale(fountCollar, vec3(0.062f, 0.012f, 0.062f));
    Primitives::drawCylinder(shader, fountCollar, rustCol);

    // Angled fuel filler spout and knurled cap (on front shoulder)
    mat4 filler = model;
    filler = translate(filler, vec3(0.0f, 0.044f, 0.064f));
    filler = rotate(filler, radians(30.0f), vec3(1.0f, 0.0f, 0.0f));
    filler = scale(filler, vec3(0.018f, 0.012f, 0.018f));
    Primitives::drawCylinder(shader, filler, brassCol);

    mat4 fillerCap = model;
    fillerCap = translate(fillerCap, vec3(0.0f, 0.048f, 0.070f));
    fillerCap = rotate(fillerCap, radians(30.0f), vec3(1.0f, 0.0f, 0.0f));
    fillerCap = scale(fillerCap, vec3(0.022f, 0.005f, 0.022f));
    Primitives::drawCylinder(shader, fillerCap, brassCol);

    // ── 2. Burner Mechanism & Globe Gallery ──────────────────────────────────
    // Burner throat
    mat4 burner = model;
    burner = translate(burner, vec3(0.0f, 0.066f, 0.0f));
    burner = scale(burner, vec3(0.042f, 0.018f, 0.042f));
    Primitives::drawCylinder(shader, burner, brassCol);

    // Perforated burner gallery basket (supporting the glass globe)
    mat4 gallery = model;
    gallery = translate(gallery, vec3(0.0f, 0.079f, 0.0f));
    gallery = scale(gallery, vec3(0.054f, 0.010f, 0.054f));
    Primitives::drawCylinder(shader, gallery, rustCol);

    // Wick adjuster knob (thumbwheel dial extending to the left)
    mat4 dialShaft = model;
    dialShaft = translate(dialShaft, vec3(-0.038f, 0.072f, 0.0f));
    dialShaft = rotate(dialShaft, radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
    dialShaft = scale(dialShaft, vec3(0.004f, 0.018f, 0.004f));
    Primitives::drawCylinder(shader, dialShaft, brassCol);

    mat4 dialWheel = model;
    dialWheel = translate(dialWheel, vec3(-0.048f, 0.072f, 0.0f));
    dialWheel = rotate(dialWheel, radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
    dialWheel = scale(dialWheel, vec3(0.014f, 0.005f, 0.014f));
    Primitives::drawCylinder(shader, dialWheel, brassCol);

    // ── 3. Luminous Kerosene Flame & Wick ────────────────────────────────────
    // Wick guide
    mat4 wick = model;
    wick = translate(wick, vec3(0.0f, 0.088f, 0.0f));
    wick = scale(wick, vec3(0.010f, 0.008f, 0.010f));
    Primitives::drawCylinder(shader, wick, vec3(0.24f, 0.18f, 0.12f));

    // Outer warm glowing teardrop flame (Emissive)
    shader.setFloat("emissive", 1.0f);
    mat4 flame = model;
    flame = translate(flame, vec3(0.0f, 0.094f, 0.0f));
    flame = scale(flame, vec3(0.022f, 0.065f, 0.022f));
    Primitives::drawCone(shader, flame, flameCol);

    // Hot incandescent white-gold core
    mat4 flameIn = model;
    flameIn = translate(flameIn, vec3(0.0f, 0.094f, 0.0f));
    flameIn = scale(flameIn, vec3(0.012f, 0.040f, 0.012f));
    Primitives::drawCone(shader, flameIn, flameCore);
    shader.setFloat("emissive", 0.0f);

    // ── 4. Bulbous Glass Globe (Chimney) ─────────────────────────────────────
    // Lower glass seating neck
    mat4 glassNeckB = model;
    glassNeckB = translate(glassNeckB, vec3(0.0f, 0.088f, 0.0f));
    glassNeckB = scale(glassNeckB, vec3(0.044f, 0.010f, 0.044f));
    Primitives::drawCylinder(shader, glassNeckB, glassCol);

    // Iconic swelling bulbous glass belly
    mat4 globe = model;
    globe = translate(globe, vec3(0.0f, 0.148f, 0.0f));
    globe = scale(globe, vec3(0.064f, 0.070f, 0.064f));
    Primitives::drawSphere(shader, globe, glassCol);

    // Upper glass throat collar
    mat4 glassNeckT = model;
    glassNeckT = translate(glassNeckT, vec3(0.0f, 0.208f, 0.0f));
    glassNeckT = scale(glassNeckT, vec3(0.044f, 0.010f, 0.044f));
    Primitives::drawCylinder(shader, glassNeckT, glassCol);

    // ── 5. Protective Wire Guard Cage around Globe ───────────────────────────
    // Three horizontal guard wire rings hugging the bulbous glass (lower, middle, upper)
    float guardY[3] = { 0.112f, 0.148f, 0.180f };
    float guardD[3] = { 0.118f, 0.134f, 0.120f };
    for (int g = 0; g < 3; g++) {
        // Front semi-circle (+Z)
        mat4 arcF = model;
        arcF = translate(arcF, vec3(0.0f, guardY[g], 0.0f));
        arcF = rotate(arcF, radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
        arcF = scale(arcF, vec3(guardD[g], guardD[g], 0.0035f));
        Primitives::drawArch(shader, arcF, wireCol);

        // Back semi-circle (-Z)
        mat4 arcB = model;
        arcB = translate(arcB, vec3(0.0f, guardY[g], 0.0f));
        arcB = rotate(arcB, radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
        arcB = scale(arcB, vec3(guardD[g], guardD[g], 0.0035f));
        Primitives::drawArch(shader, arcB, wireCol);
    }

    // Four vertical guard cage ribs flanking the perimeter of the globe
    float ribOffsets[4][2] = {
        { -0.046f,  0.046f },
        {  0.046f,  0.046f },
        { -0.046f, -0.046f },
        {  0.046f, -0.046f }
    };
    for (int r = 0; r < 4; r++) {
        mat4 rib = model;
        rib = translate(rib, vec3(ribOffsets[r][0], 0.148f, ribOffsets[r][1]));
        rib = scale(rib, vec3(0.0035f, 0.120f, 0.0035f));
        Primitives::drawCylinder(shader, rib, wireCol);
    }

    // Diagonal crossing guard wires across front (+Z) and back (-Z)
    float strutZ[2] = { 0.059f, -0.059f };
    for (int s = 0; s < 2; s++) {
        float z = strutZ[s];
        mat4 st1 = model;
        st1 = translate(st1, vec3(0.0f, 0.148f, z));
        st1 = rotate(st1, radians(28.0f), vec3(0.0f, 0.0f, 1.0f));
        st1 = scale(st1, vec3(0.003f, 0.118f, 0.003f));
        Primitives::drawCylinder(shader, st1, wireCol);

        mat4 st2 = model;
        st2 = translate(st2, vec3(0.0f, 0.148f, z));
        st2 = rotate(st2, radians(-28.0f), vec3(0.0f, 0.0f, 1.0f));
        st2 = scale(st2, vec3(0.003f, 0.118f, 0.003f));
        Primitives::drawCylinder(shader, st2, wireCol);
    }

    // ── 6. Signature Dual Tubular Side Air Tubes (Left & Right) ─────────────
    float tubeSigns[2] = { -1.0f, 1.0f };
    for (int t = 0; t < 2; t++) {
        float sign = tubeSigns[t];

        // Lower elbow connecting into fount shoulder
        mat4 tLow = model;
        tLow = translate(tLow, vec3(sign * 0.071f, 0.050f, 0.0f));
        tLow = rotate(tLow, radians(-sign * 24.0f), vec3(0.0f, 0.0f, 1.0f));
        tLow = scale(tLow, vec3(0.009f, 0.028f, 0.009f));
        Primitives::drawCylinder(shader, tLow, metalCol);

        // Main vertical upright side tube
        mat4 tMid = model;
        tMid = translate(tMid, vec3(sign * 0.078f, 0.138f, 0.0f));
        tMid = scale(tMid, vec3(0.009f, 0.150f, 0.009f));
        Primitives::drawCylinder(shader, tMid, metalCol);

        // Stamped handle pivot eyelet bracket
        mat4 eyelet = model;
        eyelet = translate(eyelet, vec3(sign * 0.084f, 0.185f, 0.0f));
        eyelet = scale(eyelet, vec3(0.008f, 0.012f, 0.008f));
        Primitives::drawCube(shader, eyelet, rustCol);

        // Upper elbow curving into chimney cowl
        mat4 tHigh = model;
        tHigh = translate(tHigh, vec3(sign * 0.062f, 0.222f, 0.0f));
        tHigh = rotate(tHigh, radians(sign * 38.0f), vec3(0.0f, 0.0f, 1.0f));
        tHigh = scale(tHigh, vec3(0.009f, 0.040f, 0.009f));
        Primitives::drawCylinder(shader, tHigh, metalCol);
    }

    // ── 7. Tiered Ventilated Chimney Cowl / Smoke Cap (Hood) ─────────────────
    // Glass upper retainer collar
    mat4 cowlBase = model;
    cowlBase = translate(cowlBase, vec3(0.0f, 0.218f, 0.0f));
    cowlBase = scale(cowlBase, vec3(0.048f, 0.012f, 0.048f));
    Primitives::drawCylinder(shader, cowlBase, rustCol);

    // Flared lower cowl skirt / baffle
    mat4 cowlSkirt = model;
    cowlSkirt = translate(cowlSkirt, vec3(0.0f, 0.230f, 0.0f));
    cowlSkirt = scale(cowlSkirt, vec3(0.066f, 0.010f, 0.066f));
    Primitives::drawCylinder(shader, cowlSkirt, metalCol);

    // Ventilator column stack with perforated draft rings
    mat4 ventStack = model;
    ventStack = translate(ventStack, vec3(0.0f, 0.252f, 0.0f));
    ventStack = scale(ventStack, vec3(0.046f, 0.036f, 0.046f));
    Primitives::drawCylinder(shader, ventStack, metalCol);

    // Stepped louver ribs around ventilator stack
    mat4 ventRib1 = model;
    ventRib1 = translate(ventRib1, vec3(0.0f, 0.244f, 0.0f));
    ventRib1 = scale(ventRib1, vec3(0.050f, 0.005f, 0.050f));
    Primitives::drawCylinder(shader, ventRib1, rustCol);

    mat4 ventRib2 = model;
    ventRib2 = translate(ventRib2, vec3(0.0f, 0.260f, 0.0f));
    ventRib2 = scale(ventRib2, vec3(0.050f, 0.005f, 0.050f));
    Primitives::drawCylinder(shader, ventRib2, rustCol);

    // Upper canopy roof / shade disk
    mat4 canopyDisk = model;
    canopyDisk = translate(canopyDisk, vec3(0.0f, 0.274f, 0.0f));
    canopyDisk = scale(canopyDisk, vec3(0.062f, 0.008f, 0.062f));
    Primitives::drawCylinder(shader, canopyDisk, rustCol);

    // Sloping conical hood roof
    mat4 hoodRoof = model;
    hoodRoof = translate(hoodRoof, vec3(0.0f, 0.278f, 0.0f));
    hoodRoof = scale(hoodRoof, vec3(0.058f, 0.018f, 0.058f));
    Primitives::drawCone(shader, hoodRoof, metalCol);

    // Top domed crown cap
    mat4 topDome = model;
    topDome = translate(topDome, vec3(0.0f, 0.296f, 0.0f));
    topDome = scale(topDome, vec3(0.034f, 0.016f, 0.034f));
    Primitives::drawHemisphere(shader, topDome, metalCol);

    // Globe lifting ring / pull loop (vertical loop for lighting the wick)
    mat4 liftRing = model;
    liftRing = translate(liftRing, vec3(0.0f, 0.312f, 0.0f));
    liftRing = scale(liftRing, vec3(0.026f, 0.034f, 0.005f));
    Primitives::drawArch(shader, liftRing, brassCol);

    // ── 8. Tall Arched Wire Bail Handle ──────────────────────────────────────
    // Left upright wire arm
    mat4 armL = model;
    armL = translate(armL, vec3(-0.082f, 0.255f, 0.0f));
    armL = scale(armL, vec3(0.004f, 0.140f, 0.004f));
    Primitives::drawCylinder(shader, armL, wireCol);

    // Right upright wire arm
    mat4 armR = model;
    armR = translate(armR, vec3(0.082f, 0.255f, 0.0f));
    armR = scale(armR, vec3(0.004f, 0.140f, 0.004f));
    Primitives::drawCylinder(shader, armR, wireCol);

    // Top semi-circular arched wire bail connecting the two arms
    mat4 topArch = model;
    topArch = translate(topArch, vec3(0.0f, 0.325f, 0.0f));
    topArch = scale(topArch, vec3(0.164f, 0.150f, 0.005f));
    Primitives::drawArch(shader, topArch, wireCol);

    // Apex suspension loop / notch at very top
    mat4 apexNotch = model;
    apexNotch = translate(apexNotch, vec3(0.0f, 0.400f, 0.0f));
    apexNotch = scale(apexNotch, vec3(0.016f, 0.016f, 0.005f));
    Primitives::drawArch(shader, apexNotch, wireCol);
}

// ─── 4. Traditional Rural Bengali Woven Bamboo Stool (Mora / মোড়া) ───
void drawMora(Shader& shader, const mat4& model)
{
    vec3 bambooDark (0.42f, 0.30f, 0.16f); // dark seasoned bamboo rim hoops
    vec3 caneWeave  (0.68f, 0.55f, 0.32f); // natural golden woven cane / rattan ribs
    vec3 seatWeave  (0.62f, 0.48f, 0.28f); // woven rattan seating disk
    vec3 tieCord    (0.32f, 0.20f, 0.10f); // jute cord lashings

    float moraH  = 0.24f; // 24cm traditional low stool height
    float rBase  = 0.19f; // base ring radius
    float rWaist = 0.135f; // narrowed hourglass waist radius
    float rTop   = 0.18f; // top seating rim radius

    // A. Bottom thick circular bamboo base ring resting on ground
    mat4 baseRing = model;
    baseRing = translate(baseRing, vec3(0.0f, 0.020f, 0.0f));
    baseRing = scale(baseRing, vec3(rBase * 2.0f, 0.038f, rBase * 2.0f));
    Primitives::drawCylinder(shader, baseRing, bambooDark);

    // Inner hollow rim indent
    mat4 baseHollow = model;
    baseHollow = translate(baseHollow, vec3(0.0f, 0.022f, 0.0f));
    baseHollow = scale(baseHollow, vec3(rBase * 1.70f, 0.040f, rBase * 1.70f));
    Primitives::drawCylinder(shader, baseHollow, tieCord);

    // B. Central Hourglass Core Column (tapered waist)
    mat4 waist = model;
    waist = translate(waist, vec3(0.0f, moraH * 0.50f, 0.0f));
    waist = scale(waist, vec3(rWaist * 2.0f, moraH * 0.70f, rWaist * 2.0f));
    Primitives::drawCylinder(shader, waist, caneWeave);

    // C. 16 Diagonal Crossing Bamboo / Cane Slat Ribs (forming the iconic Mora weave)
    const int numSlats = 14;
    for (int i = 0; i < numSlats; ++i) {
        float angle = (float)i * (2.0f * PI / numSlats);
        float bx = cosf(angle) * rBase * 0.95f;
        float bz = sinf(angle) * rBase * 0.95f;
        float tx = cosf(angle + 0.35f) * rTop * 0.95f;
        float tz = sinf(angle + 0.35f) * rTop * 0.95f;

        // Positive spiral rib
        mat4 rib1 = model;
        rib1 = translate(rib1, vec3((bx + tx) * 0.5f, moraH * 0.5f, (bz + tz) * 0.5f));
        rib1 = scale(rib1, vec3(0.014f, moraH * 0.95f, 0.014f));
        Primitives::drawCylinder(shader, rib1, caneWeave);

        // Counter-diagonal spiral rib
        float tx2 = cosf(angle - 0.35f) * rTop * 0.95f;
        float tz2 = sinf(angle - 0.35f) * rTop * 0.95f;
        mat4 rib2 = model;
        rib2 = translate(rib2, vec3((bx + tx2) * 0.5f, moraH * 0.5f, (bz + tz2) * 0.5f));
        rib2 = scale(rib2, vec3(0.014f, moraH * 0.95f, 0.014f));
        Primitives::drawCylinder(shader, rib2, caneWeave);
    }

    // D. Narrowed Waist Bamboo Binding Ring
    mat4 waistRing = model;
    waistRing = translate(waistRing, vec3(0.0f, moraH * 0.50f, 0.0f));
    waistRing = scale(waistRing, vec3(rWaist * 2.15f, 0.024f, rWaist * 2.15f));
    Primitives::drawCylinder(shader, waistRing, tieCord);

    // E. Top Circular Bamboo Rim Hoop
    mat4 topRim = model;
    topRim = translate(topRim, vec3(0.0f, moraH - 0.015f, 0.0f));
    topRim = scale(topRim, vec3(rTop * 2.05f, 0.030f, rTop * 2.05f));
    Primitives::drawCylinder(shader, topRim, bambooDark);

    // F. Top Woven Rattan Webbing Seat (circular woven disc)
    mat4 seatDisc = model;
    seatDisc = translate(seatDisc, vec3(0.0f, moraH - 0.002f, 0.0f));
    seatDisc = scale(seatDisc, vec3(rTop * 1.95f, 0.008f, rTop * 1.95f));
    Primitives::drawCylinder(shader, seatDisc, seatWeave);

    // Central floral woven star button on the seat
    mat4 starButton = model;
    starButton = translate(starButton, vec3(0.0f, moraH + 0.002f, 0.0f));
    starButton = scale(starButton, vec3(0.045f, 0.004f, 0.045f));
    Primitives::drawCylinder(shader, starButton, bambooDark);
}

// ─── 5. Traditional Village Timber Bench (Kath-er Bench / কাঠের বেঞ্চ) ───
void drawBench(Shader& shader, const mat4& model)
{
    vec3 timberDark (0.32f, 0.20f, 0.10f); // seasoned dark jackfruit / sal timber legs
    vec3 plankCol   (0.44f, 0.28f, 0.14f); // hand-planed seat planks
    vec3 pegCol     (0.22f, 0.14f, 0.06f); // dark wooden joinery pegs

    float benchW = 0.36f; // width across X (36cm)
    float benchL = 0.95f; // length along Z (95cm)
    float benchH = 0.32f; // height to seat top (32cm)
    float legR   = 0.035f; // square leg thickness (3.5cm)

    float hx = benchW * 0.5f - 0.045f;
    float hz = benchL * 0.5f - 0.055f;

    // A. Four Sturdy Square Timber Post Legs
    float legs[4][2] = { {-hx, -hz}, {hx, -hz}, {-hx, hz}, {hx, hz} };
    for (int i = 0; i < 4; ++i) {
        mat4 leg = model;
        leg = translate(leg, vec3(legs[i][0], benchH * 0.48f, legs[i][1]));
        leg = scale(leg, vec3(legR, benchH * 0.96f, legR));
        Primitives::drawCube(shader, leg, timberDark);
    }

    // B. Upper Apron Stretchers (framing directly under seat planks)
    // Long side stretchers (along Z)
    for (int side = -1; side <= 1; side += 2) {
        mat4 stZ = model;
        stZ = translate(stZ, vec3((float)side * hx, benchH - 0.045f, 0.0f));
        stZ = scale(stZ, vec3(0.024f, 0.055f, benchL - 0.10f));
        Primitives::drawCube(shader, stZ, timberDark);
    }
    // Short end stretchers (along X)
    for (int side = -1; side <= 1; side += 2) {
        mat4 stX = model;
        stX = translate(stX, vec3(0.0f, benchH - 0.045f, (float)side * hz));
        stX = scale(stX, vec3(benchW - 0.08f, 0.055f, 0.024f));
        Primitives::drawCube(shader, stX, timberDark);
    }

    // C. Lower Cross-Stretcher Footrests (for strength and feet)
    mat4 footRail = model;
    footRail = translate(footRail, vec3(0.0f, 0.09f, 0.0f));
    footRail = scale(footRail, vec3(0.022f, 0.028f, benchL - 0.10f));
    Primitives::drawCube(shader, footRail, timberDark);

    for (int side = -1; side <= 1; side += 2) {
        mat4 footEnd = model;
        footEnd = translate(footEnd, vec3(0.0f, 0.09f, (float)side * hz));
        footEnd = scale(footEnd, vec3(benchW - 0.08f, 0.028f, 0.022f));
        Primitives::drawCube(shader, footEnd, timberDark);
    }

    // D. Three Weathered Timber Seat Planks across the top (with slight gaps)
    float plankW = (benchW - 0.03f) / 3.0f; // ~11cm per plank
    for (int p = -1; p <= 1; ++p) {
        float px = (float)p * (plankW + 0.008f);
        mat4 plank = model;
        plank = translate(plank, vec3(px, benchH - 0.010f, 0.0f));
        plank = scale(plank, vec3(plankW, 0.022f, benchL));
        Primitives::drawCube(shader, plank, plankCol);

        // Wooden pegs on each plank end
        for (int e = -1; e <= 1; e += 2) {
            mat4 peg = model;
            peg = translate(peg, vec3(px, benchH + 0.001f, (float)e * (hz - 0.015f)));
            peg = scale(peg, vec3(0.008f, 0.003f, 0.008f));
            Primitives::drawCylinder(shader, peg, pegCol);
        }
    }
}

} // namespace Charpai


