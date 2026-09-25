// Person.cpp — Procedural Bengali villager rig:
// Features anatomically lean, natural human proportions with fully modest, authentic Bengali attire:
// 1. Natural tapered torso (broad chest tapering to a lean waist) with tailored Panjabi/Kurta tunic.
// 2. Standing villagers have TWO DISTINCT slender leg columns draped in Lungi (no bulky cylinder/oil drum).
// 3. Seated elder on charpai has two distinct thighs and draped vertical shins with knees bent over edge.
// 4. Seated elder holds the traditional handmade Haat Pakha (fan) directly in his right hand.
// 5. Slender athletic limbs, contoured shoulders, natural head/neck, elder white hair and beard, and gamcha.

#include "objects/Person.h"
#include "objects/Charpai.h"
#include "Primitives.h"

using namespace math;

namespace Person {

void draw(Shader& shader, const mat4& model, const PersonParams& p)
{
    shader.setInt("uUseTexture", 0); // smooth natural skin tones, white cotton kurta, beard, and lungi

    // ── Slender, Anatomical Proportions ──────────────────────────
    const float headR     = 0.110f;
    const float torsoH    = 0.420f;
    const float chestW    = 0.150f; // upper chest half-width (30cm across shoulders/chest)
    const float chestD    = 0.092f; // chest depth (18.4cm front-to-back)
    const float waistW    = 0.120f; // lean natural waist half-width (24cm)
    const float waistD    = 0.078f; // waist depth (15.6cm)
    const float upperArmH = 0.240f;
    const float lowerArmH = 0.220f;
    const float armR      = 0.027f; // lean upper arm
    const float forearmR  = 0.023f; // lean tapered forearm
    const float upperLegH = 0.280f;
    const float lowerLegH = 0.260f;

    float torsoBase = 0.0f;
    if (p.seated || p.crossLegged) {
        torsoBase = 0.05f; // pelvis resting directly on seat/ground
    } else {
        torsoBase = upperLegH + lowerLegH; // ≈ 0.54m
    }
    float torsoTop = torsoBase + torsoH;

    // ── 1. Clean Explainable Geometric Torso (Unit Cube) ───────────
    mat4 torso = model;
    torso = translate(torso, vec3(0.0f, torsoBase + torsoH * 0.5f, 0.0f));
    torso = scale(torso, vec3(chestW * 2.0f, torsoH, chestD * 2.0f));
    Primitives::drawCube(shader, torso, p.shirtColor);

    // ── 2. Traditional Gamcha (Unit Cube draped over shoulder) ────
    if (p.hasGamcha) {
        mat4 shawl = model;
        shawl = translate(shawl, vec3(chestW * 0.65f, torsoBase + torsoH * 0.5f, 0.0f));
        shawl = scale(shawl, vec3(0.065f, torsoH * 1.05f, chestD * 2.15f));
        Primitives::drawCube(shader, shawl, p.gamchaColor);
    }

    // ── Kurta Collar Band & Front Placket Trim ───────────────────
    mat4 collar = model;
    collar = translate(collar, vec3(0.0f, torsoTop + 0.010f, 0.0f));
    collar = scale(collar, vec3(0.058f, 0.022f, 0.058f));
    Primitives::drawCylinder(shader, collar, p.shirtColor * 0.90f);

    mat4 placket = model;
    placket = translate(placket, vec3(0.0f, torsoTop - torsoH * 0.20f, chestD + 0.002f));
    placket = scale(placket, vec3(0.020f, torsoH * 0.38f, 0.005f));
    Primitives::drawCube(shader, placket, p.shirtColor * 0.85f);

    // ── Kurta Hem & Side Slits (Chak / চাক) ──────────────────────
    if (!p.seated && !p.crossLegged) {
        mat4 kurtaHem = model;
        kurtaHem = translate(kurtaHem, vec3(0.0f, torsoBase - 0.045f, 0.0f));
        kurtaHem = scale(kurtaHem, vec3(chestW * 2.08f, 0.12f, chestD * 2.06f));
        Primitives::drawCube(shader, kurtaHem, p.shirtColor);

        // Center hem split / trim
        mat4 hemSplit = model;
        hemSplit = translate(hemSplit, vec3(0.0f, torsoBase - 0.075f, chestD + 0.003f));
        hemSplit = scale(hemSplit, vec3(0.008f, 0.065f, 0.005f));
        Primitives::drawCube(shader, hemSplit, p.shirtColor * 0.85f);
    }

    // ── 3. Neck (Unit Cylinder) & Head (Unit Sphere) ──────────────
    float headCenterY = torsoTop + headR + 0.030f;

    mat4 neck = model;
    neck = translate(neck, vec3(0.0f, torsoTop + 0.020f, 0.0f));
    neck = scale(neck, vec3(0.040f, 0.050f, 0.040f));
    Primitives::drawCylinder(shader, neck, p.skinColor);

    mat4 head = model;
    head = translate(head, vec3(0.0f, headCenterY, 0.0f));
    head = scale(head, vec3(headR, headR, headR));
    Primitives::drawSphere(shader, head, p.skinColor);

    // ── Expressive Authentic Facial Features ─────────────────────
    vec3 eyeWhite(0.95f, 0.95f, 0.92f);
    vec3 eyePupil(0.10f, 0.08f, 0.06f);

    // 1. Nose (defined bridge and tip extending forward along +Z)
    mat4 nose = model;
    nose = translate(nose, vec3(0.0f, headCenterY - 0.008f, headR * 0.92f));
    nose = rotate(nose, radians(15.0f), vec3(1.0f, 0.0f, 0.0f));
    nose = scale(nose, vec3(0.018f, 0.038f, 0.032f));
    Primitives::drawCone(shader, nose, p.skinColor * 0.94f);

    // 2. Pair of expressive eyes (sclera + dark iris) & ears
    for (int s = -1; s <= 1; s += 2) {
        float fside = (float)s;
        // Eye white
        mat4 eyeW = model;
        eyeW = translate(eyeW, vec3(fside * 0.038f, headCenterY + 0.016f, headR * 0.88f));
        eyeW = scale(eyeW, vec3(0.016f, 0.012f, 0.014f));
        Primitives::drawSphere(shader, eyeW, eyeWhite);

        // Pupil / iris
        mat4 pupil = model;
        pupil = translate(pupil, vec3(fside * 0.038f, headCenterY + 0.016f, headR * 0.94f));
        pupil = scale(pupil, vec3(0.009f, 0.009f, 0.009f));
        Primitives::drawSphere(shader, pupil, eyePupil);

        // Eyebrow
        mat4 brow = model;
        brow = translate(brow, vec3(fside * 0.038f, headCenterY + 0.030f, headR * 0.86f));
        brow = rotate(brow, radians(fside * -6.0f), vec3(0.0f, 0.0f, 1.0f));
        brow = scale(brow, vec3(0.024f, 0.006f, 0.012f));
        Primitives::drawCube(shader, brow, p.isElder ? vec3(0.90f) : p.hairColor);

        // Ear lobes on sides of head
        mat4 ear = model;
        ear = translate(ear, vec3(fside * (headR * 0.96f), headCenterY, 0.0f));
        ear = scale(ear, vec3(0.014f, 0.030f, 0.020f));
        Primitives::drawSphere(shader, ear, p.skinColor);
    }

    // 3. Traditional Bengali Mustache (Gnof / গোঁফ) for adult villagers
    if (!p.isElder && !p.crossLegged && !p.isWoman) {
        for (int s = -1; s <= 1; s += 2) {
            float fside = (float)s;
            mat4 stache = model;
            stache = translate(stache, vec3(fside * 0.022f, headCenterY - 0.030f, headR * 0.88f));
            stache = rotate(stache, radians(fside * -18.0f), vec3(0.0f, 0.0f, 1.0f));
            stache = scale(stache, vec3(0.026f, 0.008f, 0.012f));
            Primitives::drawCube(shader, stache, p.hairColor);
        }
    }

    // ── Authentic Hair, Venerable Elder Beard & Prayer Cap ────────
    if (p.isElder) {
        // Venerable Elder (Murobbi / মুরুব্বি)
        vec3 beardCol(0.92f, 0.91f, 0.88f); // soft white cotton beard & hair
        vec3 tupiCol (0.98f, 0.98f, 0.96f); // white cotton prayer cap (Tupi)

        // 1. Soft white hair fringe around temples and back of head
        mat4 hair = model;
        hair = translate(hair, vec3(0.0f, headCenterY + 0.010f, -0.016f));
        hair = scale(hair, vec3(headR * 1.02f, headR * 0.72f, headR * 0.96f));
        Primitives::drawSphere(shader, hair, beardCol);

        // 2. Full chin goatee / beard (Paka Daari / পাকা দাড়ি)
        mat4 chinBeard = model;
        chinBeard = translate(chinBeard, vec3(0.0f, headCenterY - headR * 0.72f, headR * 0.50f));
        chinBeard = rotate(chinBeard, radians(-18.0f), vec3(1.0f, 0.0f, 0.0f));
        chinBeard = scale(chinBeard, vec3(0.072f, 0.098f, 0.062f));
        Primitives::drawSphere(shader, chinBeard, beardCol);

        // 3. Jawline beard contours on left and right
        for (int side = -1; side <= 1; side += 2) {
            float fside = (float)side;
            mat4 jaw = model;
            jaw = translate(jaw, vec3(fside * 0.058f, headCenterY - headR * 0.42f, headR * 0.20f));
            jaw = rotate(jaw, radians(fside * 18.0f), vec3(0.0f, 0.0f, 1.0f));
            jaw = scale(jaw, vec3(0.038f, 0.075f, 0.060f));
            Primitives::drawSphere(shader, jaw, beardCol);
        }

        // 4. Traditional White Cotton Prayer Cap (Tupi / টুপি)
        mat4 tupiBand = model;
        tupiBand = translate(tupiBand, vec3(0.0f, headCenterY + headR * 0.65f, 0.0f));
        tupiBand = scale(tupiBand, vec3(headR * 0.98f, 0.040f, headR * 0.98f));
        Primitives::drawCylinder(shader, tupiBand, tupiCol * 0.92f);

        mat4 tupiCrown = model;
        tupiCrown = translate(tupiCrown, vec3(0.0f, headCenterY + headR * 0.82f, 0.0f));
        tupiCrown = scale(tupiCrown, vec3(headR * 0.90f, 0.038f, headR * 0.90f));
        Primitives::drawSphere(shader, tupiCrown, tupiCol);
    } else if (!p.isWoman) {
        // Natural dark hair cap on crown for adult villagers and child
        mat4 hair = model;
        hair = translate(hair, vec3(0.0f, headCenterY + 0.025f, -0.012f));
        hair = scale(hair, vec3(headR * 1.02f, headR * 0.85f, headR * 0.98f));
        Primitives::drawSphere(shader, hair, p.hairColor);
    }

    // ── 4. Slender Anatomical Arms & Hands ─────────────────────────
    for (int side = -1; side <= 1; side += 2) {
        float fside = (float)side;
        float armAngle = (side == -1) ? p.leftArmAngle : p.rightArmAngle;
        float shoulderX = fside * (chestW + armR * 0.50f);

        if (p.seated) {
            // Seated Elder on Charpai
            bool isHoldingFan = (side == 1) && (p.hasFan || p.isElder);

            if (isHoldingFan) {
                // Right arm: holds the handmade fan (Haat Pakha) in hand!
                mat4 shoulder = model;
                shoulder = translate(shoulder, vec3(shoulderX, torsoTop - 0.05f, 0.0f));
                shoulder = rotate(shoulder, radians(-22.0f), vec3(1.0f, 0.0f, 0.0f));
                shoulder = rotate(shoulder, radians(14.0f), vec3(0.0f, 0.0f, 1.0f));

                // Upper arm: Unit Cylinder
                mat4 upper = shoulder;
                upper = translate(upper, vec3(0.0f, -upperArmH * 0.5f, 0.0f));
                upper = scale(upper, vec3(armR, upperArmH, armR));
                Primitives::drawCylinder(shader, upper, p.shirtColor);

                // Elbow: bent forward and upward to raise the hand holding the fan
                mat4 elbow = shoulder;
                elbow = translate(elbow, vec3(0.0f, -upperArmH, 0.0f));
                elbow = rotate(elbow, radians(-65.0f), vec3(1.0f, 0.0f, 0.0f)); // raised forward
                elbow = rotate(elbow, radians(-18.0f), vec3(0.0f, 1.0f, 0.0f)); // angled slightly inward

                // Forearm: Unit Cylinder
                mat4 lower = elbow;
                lower = translate(lower, vec3(0.0f, -lowerArmH * 0.5f, 0.0f));
                lower = scale(lower, vec3(forearmR, lowerArmH, forearmR));
                Primitives::drawCylinder(shader, lower, p.shirtColor);

                // Hand: Unit Sphere firmly grasping the bamboo handle
                mat4 hand = elbow;
                hand = translate(hand, vec3(0.0f, -lowerArmH - 0.020f, 0.005f));
                hand = scale(hand, vec3(0.028f, 0.032f, 0.028f));
                Primitives::drawSphere(shader, hand, p.skinColor);

                // Traditional Handmade Fan (Haat Pakha / হাতপাখা) — Pointing UPWARD / ABOVE!
                mat4 fan = elbow;
                // Position grip in palm
                fan = translate(fan, vec3(0.0f, -lowerArmH - 0.020f, 0.005f));
                // Rotate +85 degrees around X so the fan blade stands UPWARD ("above direction")
                fan = rotate(fan, radians(85.0f), vec3(1.0f, 0.0f, 0.0f));
                fan = rotate(fan, radians(15.0f), vec3(0.0f, 0.0f, 1.0f));
                // Gentle fanning oscillation
                fan = rotate(fan, p.fanSway, vec3(0.0f, 1.0f, 0.0f));
                // Center the grip on the handle (handle extends below blade)
                fan = translate(fan, vec3(0.0f, 0.16f, 0.0f));
                Charpai::drawFan(shader, fan, 0.0f);
            }
            else {
                // Left arm (or seated person without fan): rests on knee or follows armAngle (e.g. boatman)
                float pitch = (armAngle != 0.0f) ? -armAngle : radians(-28.0f);
                mat4 shoulder = model;
                shoulder = translate(shoulder, vec3(shoulderX, torsoTop - 0.05f, 0.0f));
                shoulder = rotate(shoulder, pitch, vec3(1.0f, 0.0f, 0.0f));
                shoulder = rotate(shoulder, radians(fside * -7.0f), vec3(0.0f, 0.0f, 1.0f));

                mat4 upper = shoulder;
                upper = translate(upper, vec3(0.0f, -upperArmH * 0.5f, 0.0f));
                upper = scale(upper, vec3(armR, upperArmH, armR));
                Primitives::drawCylinder(shader, upper, p.shirtColor);

                mat4 elbow = shoulder;
                elbow = translate(elbow, vec3(0.0f, -upperArmH, 0.0f));
                elbow = rotate(elbow, radians(-50.0f), vec3(1.0f, 0.0f, 0.0f));
                elbow = rotate(elbow, radians(fside * 14.0f), vec3(0.0f, 1.0f, 0.0f));

                mat4 lower = elbow;
                lower = translate(lower, vec3(0.0f, -lowerArmH * 0.5f, 0.0f));
                lower = scale(lower, vec3(forearmR, lowerArmH, forearmR));
                Primitives::drawCylinder(shader, lower, p.shirtColor);

                mat4 cuff = elbow;
                cuff = translate(cuff, vec3(0.0f, -lowerArmH, 0.0f));
                cuff = scale(cuff, vec3(forearmR * 1.08f, 0.015f, forearmR * 1.08f));
                Primitives::drawCylinder(shader, cuff, p.shirtColor * 0.88f);

                mat4 hand = elbow;
                hand = translate(hand, vec3(0.0f, -lowerArmH - 0.020f, 0.015f));
                hand = scale(hand, vec3(0.028f, 0.016f, 0.042f));
                Primitives::drawSphere(shader, hand, p.skinColor);
            }
        }
        else if (p.crossLegged) {
            // Seated child
            mat4 shoulder = model;
            shoulder = translate(shoulder, vec3(shoulderX, torsoTop - 0.05f, 0.0f));
            shoulder = rotate(shoulder, radians(-32.0f), vec3(1.0f, 0.0f, 0.0f));
            shoulder = rotate(shoulder, radians(fside * -8.0f), vec3(0.0f, 0.0f, 1.0f));

            mat4 upper = shoulder;
            upper = translate(upper, vec3(0.0f, -upperArmH * 0.5f, 0.0f));
            upper = scale(upper, vec3(armR, upperArmH, armR));
            Primitives::drawCylinder(shader, upper, p.shirtColor);

            mat4 elbow = shoulder;
            elbow = translate(elbow, vec3(0.0f, -upperArmH, 0.0f));
            elbow = rotate(elbow, radians(-45.0f), vec3(1.0f, 0.0f, 0.0f));
            elbow = rotate(elbow, radians(fside * 16.0f), vec3(0.0f, 1.0f, 0.0f));

            mat4 lower = elbow;
            lower = translate(lower, vec3(0.0f, -lowerArmH * 0.5f, 0.0f));
            lower = scale(lower, vec3(forearmR, lowerArmH, forearmR));
            Primitives::drawCylinder(shader, lower, p.shirtColor);

            mat4 hand = elbow;
            hand = translate(hand, vec3(0.0f, -lowerArmH - 0.020f, 0.0f));
            hand = scale(hand, vec3(0.024f, 0.016f, 0.034f));
            Primitives::drawSphere(shader, hand, p.skinColor);
        }
        else {
            // Standing villager: hands held politely and naturally in front of the body
            float fwdSwing = -armAngle;

            // Upper arm hangs naturally down alongside the torso
            float shoulderPitch = radians(-12.0f) + fwdSwing * 0.35f;
            float shoulderRoll  = radians(fside * -5.0f);
            // Internal shoulder rotation angles the elbow hinge inward across the abdomen
            float shoulderYaw   = radians(fside * -36.0f);

            mat4 shoulder = model;
            shoulder = translate(shoulder, vec3(shoulderX, torsoTop - 0.05f, 0.0f));
            shoulder = rotate(shoulder, shoulderPitch, vec3(1.0f, 0.0f, 0.0f));
            shoulder = rotate(shoulder, shoulderRoll, vec3(0.0f, 0.0f, 1.0f));
            shoulder = rotate(shoulder, shoulderYaw, vec3(0.0f, 1.0f, 0.0f));

            // Upper arm (slender kurta sleeve)
            mat4 upper = shoulder;
            upper = translate(upper, vec3(0.0f, -upperArmH * 0.5f, 0.0f));
            upper = scale(upper, vec3(armR, upperArmH, armR));
            Primitives::drawCylinder(shader, upper, p.shirtColor);

            // Elbow: pure flexion hinge (-68 deg) swinging forearm across the abdomen
            mat4 elbow = shoulder;
            elbow = translate(elbow, vec3(0.0f, -upperArmH, 0.0f));
            elbow = rotate(elbow, radians(-68.0f), vec3(1.0f, 0.0f, 0.0f));

            // Forearm
            mat4 lower = elbow;
            lower = translate(lower, vec3(0.0f, -lowerArmH * 0.5f, 0.0f));
            lower = scale(lower, vec3(forearmR, lowerArmH, forearmR));
            Primitives::drawCylinder(shader, lower, p.shirtColor);

            // Wrist cuff trim
            mat4 cuff = elbow;
            cuff = translate(cuff, vec3(0.0f, -lowerArmH, 0.0f));
            cuff = scale(cuff, vec3(forearmR * 1.08f, 0.014f, forearmR * 1.08f));
            Primitives::drawCylinder(shader, cuff, p.shirtColor * 0.88f);

            // Respectful Bengali standing posture (Haat badha): hands folded gracefully in front of abdomen
            float zOffset = (fside < 0) ? 0.014f : -0.008f;
            float yOffset = (fside < 0) ? 0.008f : -0.004f;

            // 1. Palm (flat box resting cleanly in front of belly)
            mat4 palm = elbow;
            palm = translate(palm, vec3(0.0f, -lowerArmH - 0.018f + yOffset, zOffset));
            palm = scale(palm, vec3(0.020f, 0.038f, 0.030f));
            Primitives::drawCube(shader, palm, p.skinColor);

            // 2. Curled fingers extending toward midline
            mat4 fingers = elbow;
            fingers = translate(fingers, vec3(0.0f, -lowerArmH - 0.044f + yOffset, zOffset - 0.003f));
            fingers = rotate(fingers, radians(15.0f), vec3(1.0f, 0.0f, 0.0f));
            fingers = scale(fingers, vec3(0.018f, 0.026f, 0.028f));
            Primitives::drawCube(shader, fingers, p.skinColor * 0.94f);

            // 3. Thumb tucked naturally along the inner edge
            mat4 thumb = elbow;
            thumb = translate(thumb, vec3(0.0f, -lowerArmH - 0.022f + yOffset, zOffset + 0.014f));
            thumb = rotate(thumb, radians(10.0f), vec3(1.0f, 0.0f, 0.0f));
            thumb = scale(thumb, vec3(0.016f, 0.022f, 0.010f));
            Primitives::drawCube(shader, thumb, p.skinColor);
        }
    }

    // ── 6. Modest Traditional Lungi / Lower Body ──────────────────
    if (p.crossLegged) {
        // Child seated cross-legged on swept yard
        mat4 lap = model;
        lap = translate(lap, vec3(0.0f, torsoBase + 0.04f, 0.12f));
        lap = scale(lap, vec3(waistW * 1.60f, 0.10f, 0.28f));
        Primitives::drawCube(shader, lap, p.pantsColor);

        for (int side = -1; side <= 1; side += 2) {
            float fside = (float)side;
            mat4 calf = model;
            calf = translate(calf, vec3(fside * 0.07f, torsoBase + 0.03f, 0.24f));
            calf = rotate(calf, radians(fside * -75.0f), vec3(0.0f, 1.0f, 0.0f));
            calf = rotate(calf, radians(88.0f), vec3(1.0f, 0.0f, 0.0f));
            calf = scale(calf, vec3(0.040f, 0.20f, 0.040f));
            Primitives::drawCylinder(shader, calf, p.pantsColor);
        }
    }
    else if (p.seated) {
        // Seated Elder on Charpai:
        // Proper human anatomy: TWO DISTINCT FORWARD THIGHS + DRAPED VERTICAL SHINS!
        float thighSpacing = 0.068f;

        // Left & Right forward thighs
        for (int side = -1; side <= 1; side += 2) {
            float fside = (float)side;
            float tx = fside * thighSpacing;

            // Horizontal thigh extending forward (+Z) over bed
            mat4 thigh = model;
            thigh = translate(thigh, vec3(tx, torsoBase + 0.040f, upperLegH * 0.46f));
            thigh = scale(thigh, vec3(0.052f, 0.055f, upperLegH * 0.90f));
            Primitives::drawCube(shader, thigh, p.pantsColor);

            // Rounded knee cap at the front edge of the charpai
            mat4 knee = model;
            knee = translate(knee, vec3(tx, torsoBase + 0.040f, upperLegH * 0.88f));
            knee = scale(knee, vec3(0.050f, 0.050f, 0.050f));
            Primitives::drawSphere(shader, knee, p.pantsColor);

            // Vertical draped shin hanging down the front of the charpai
            mat4 shin = model;
            shin = translate(shin, vec3(tx, torsoBase - lowerLegH * 0.42f, upperLegH * 0.88f));
            shin = scale(shin, vec3(0.048f, lowerLegH * 0.82f, 0.052f));
            Primitives::drawCube(shader, shin, p.pantsColor);

            // Sandaled foot peeking out at ground level
            mat4 foot = model;
            foot = translate(foot, vec3(tx, torsoBase - lowerLegH, upperLegH * 0.88f + 0.035f));
            foot = scale(foot, vec3(0.036f, 0.026f, 0.078f));
            Primitives::drawCube(shader, foot, vec3(0.28f, 0.18f, 0.10f));
        }

        // Lungi fabric drape across the lap between thighs
        mat4 lapDrape = model;
        lapDrape = translate(lapDrape, vec3(0.0f, torsoBase + 0.025f, upperLegH * 0.44f));
        lapDrape = scale(lapDrape, vec3(thighSpacing * 1.30f, 0.042f, upperLegH * 0.80f));
        Primitives::drawCube(shader, lapDrape, p.pantsColor);

        // Lungi fabric drape between the hanging shins
        mat4 skirtDrape = model;
        skirtDrape = translate(skirtDrape, vec3(0.0f, torsoBase - lowerLegH * 0.42f, upperLegH * 0.86f));
        skirtDrape = scale(skirtDrape, vec3(thighSpacing * 1.30f, lowerLegH * 0.76f, 0.035f));
        Primitives::drawCube(shader, skirtDrape, p.pantsColor);
    }
    else {
        // Standing Villager:
        // Authentic Bangladeshi cotton Lungi (লুঙ্গি) — continuous tubular drape!
        float legSpacing = 0.052f;
        float lungiTopY  = torsoBase + 0.01f;
        float lungiBotY  = 0.12f;
        float lungiH     = lungiTopY - lungiBotY;
        float lungiMidY  = (lungiTopY + lungiBotY) * 0.5f;

        // 1. Continuous draped cotton tubular skirt enclosing both legs
        mat4 lungiWrap = model;
        lungiWrap = translate(lungiWrap, vec3(0.0f, lungiMidY, 0.0f));
        lungiWrap = scale(lungiWrap, vec3(waistW * 1.08f, lungiH, waistD * 1.10f));
        Primitives::drawCylinder(shader, lungiWrap, p.pantsColor);

        // 2. Traditional center front fold / pleat (Kocha / কোঁচা)
        mat4 pleat = model;
        pleat = translate(pleat, vec3(0.0f, lungiMidY, waistD * 1.04f));
        pleat = scale(pleat, vec3(0.032f, lungiH * 0.98f, 0.020f));
        Primitives::drawCube(shader, pleat, p.pantsColor * 0.82f);

        // 3. Side vertical drape creases for fabric depth
        for (int side = -1; side <= 1; side += 2) {
            float fside = (float)side;
            mat4 fold = model;
            fold = translate(fold, vec3(fside * (waistW * 0.72f), lungiMidY, 0.0f));
            fold = scale(fold, vec3(0.022f, lungiH * 0.95f, waistD * 1.04f));
            Primitives::drawCube(shader, fold, p.pantsColor * 0.90f);
        }

        // 4. Lungi bottom hem border trim (Paar / পাড়)
        mat4 hem = model;
        hem = translate(hem, vec3(0.0f, lungiBotY, 0.0f));
        hem = scale(hem, vec3(waistW * 1.10f, 0.022f, waistD * 1.12f));
        Primitives::drawCylinder(shader, hem, p.pantsColor * 0.75f);

        // 5. Exposed ankles & feet with traditional leather sandals (Chappal / স্যান্ডেল)
        for (int side = -1; side <= 1; side += 2) {
            float fside = (float)side;
            float lx = fside * legSpacing;

            // Slender exposed ankle
            mat4 ankle = model;
            ankle = translate(ankle, vec3(lx, 0.065f, 0.0f));
            ankle = scale(ankle, vec3(0.024f, 0.090f, 0.024f));
            Primitives::drawCylinder(shader, ankle, p.skinColor);

            // Sandal sole
            mat4 sole = model;
            sole = translate(sole, vec3(lx, 0.010f, 0.025f));
            sole = scale(sole, vec3(0.038f, 0.016f, 0.088f));
            Primitives::drawCube(shader, sole, vec3(0.24f, 0.16f, 0.10f));

            // Sandal upper strap (leather Y-strap)
            mat4 strap = model;
            strap = translate(strap, vec3(lx, 0.024f, 0.020f));
            strap = scale(strap, vec3(0.036f, 0.012f, 0.040f));
            Primitives::drawCube(shader, strap, vec3(0.18f, 0.10f, 0.06f));
        }
    }
}

void drawBook(Shader& shader, const mat4& model)
{
    // Open Bengali Schoolbook / Storybook (বই)
    // 1. Hardcover / binding underneath (dark navy blue cloth)
    // 2. Left Page Block & Right Page Block (warm natural off-white paper) angled open
    // 3. Central Spine Gutter / Dividing Crease Line (dark shadow seam separating pages)
    // 4. Printed horizontal text lines on both pages

    vec3 coverCol   (0.18f, 0.26f, 0.44f); // dark navy blue cloth binding
    vec3 pageCol    (0.95f, 0.94f, 0.89f); // warm natural book paper
    vec3 creaseCol  (0.22f, 0.18f, 0.14f); // dark binding gutter / crease shadow
    vec3 textCol    (0.42f, 0.40f, 0.38f); // printed ink text lines

    float bookW   = 0.36f;  // total open width along X
    float bookD   = 0.26f;  // height/depth of page along Z
    float pageThk = 0.024f; // thickness of page leaf block
    float pageW   = 0.165f; // width of each open page

    // ── 1. Book Cover & Spine ──────────────────────────────────────
    // Hardcover backing slightly larger than pages
    mat4 cover = model;
    cover = translate(cover, vec3(0.0f, -0.005f, 0.0f));
    cover = scale(cover, vec3(bookW + 0.024f, 0.010f, bookD + 0.016f));
    Primitives::drawCube(shader, cover, coverCol);

    // Rounded spine at center bottom
    mat4 spine = model;
    spine = translate(spine, vec3(0.0f, -0.006f, 0.0f));
    spine = rotate(spine, radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
    spine = scale(spine, vec3(0.010f, bookD + 0.016f, 0.010f));
    Primitives::drawCylinder(shader, spine, coverCol);

    // ── 2. Two Distinct Open Pages (Left & Right Leaf Blocks) ──────
    // Left Page (tilted up slightly at +3.5 deg for natural open book curve)
    mat4 pageL = model;
    pageL = translate(pageL, vec3(-pageW * 0.5f - 0.003f, pageThk * 0.5f, 0.0f));
    pageL = rotate(pageL, radians(3.5f), vec3(0.0f, 0.0f, 1.0f));
    pageL = scale(pageL, vec3(pageW, pageThk, bookD));
    Primitives::drawCube(shader, pageL, pageCol);

    // Right Page (tilted up slightly at -3.5 deg for natural open book curve)
    mat4 pageR = model;
    pageR = translate(pageR, vec3(pageW * 0.5f + 0.003f, pageThk * 0.5f, 0.0f));
    pageR = rotate(pageR, radians(-3.5f), vec3(0.0f, 0.0f, 1.0f));
    pageR = scale(pageR, vec3(pageW, pageThk, bookD));
    Primitives::drawCube(shader, pageR, pageCol);

    // ── 3. Prominent Center Crease / Page Divider Line ─────────────
    // Dark recessed shadow seam running right down the center along Z
    mat4 crease = model;
    crease = translate(crease, vec3(0.0f, pageThk + 0.002f, 0.0f));
    crease = scale(crease, vec3(0.009f, 0.006f, bookD + 0.004f));
    Primitives::drawCube(shader, crease, creaseCol);

    // ── 4. Printed Horizontal Text Lines on Left and Right Pages ───
    float lineZOffsets[4] = { -0.075f, -0.025f, 0.025f, 0.075f };
    float lineW = pageW * 0.72f;

    for (int i = 0; i < 4; i++) {
        float lz = lineZOffsets[i];

        // Left page text line
        mat4 lineL = model;
        lineL = translate(lineL, vec3(-pageW * 0.5f - 0.003f, pageThk + 0.003f, lz));
        lineL = rotate(lineL, radians(3.5f), vec3(0.0f, 0.0f, 1.0f));
        lineL = scale(lineL, vec3(lineW, 0.0025f, 0.016f));
        Primitives::drawCube(shader, lineL, textCol);

        // Right page text line
        mat4 lineR = model;
        lineR = translate(lineR, vec3(pageW * 0.5f + 0.003f, pageThk + 0.003f, lz));
        lineR = rotate(lineR, radians(-3.5f), vec3(0.0f, 0.0f, 1.0f));
        lineR = scale(lineR, vec3(lineW, 0.0025f, 0.016f));
        Primitives::drawCube(shader, lineR, textCol);
    }
}

void drawRehal(Shader& shader, const mat4& model)
{
    // Traditional Rural Wooden Folding Bookstand (Rehal / রেহাল)
    // Modeled from two interlocking carved seasoned timber planks forming an X-cradle
    vec3 woodCol   (0.42f, 0.26f, 0.12f); // seasoned dark teak/rosewood
    vec3 carvedCol (0.34f, 0.20f, 0.08f); // carved relief shadow

    float plankL = 0.38f;  // length of each crossing plank
    float plankW = 0.28f;  // width along Z
    float plankT = 0.020f; // thickness

    // Plank 1 (sloping from lower-left to upper-right at 36 deg)
    mat4 p1 = model;
    p1 = translate(p1, vec3(0.0f, 0.11f, 0.0f));
    p1 = rotate(p1, radians(36.0f), vec3(0.0f, 0.0f, 1.0f));
    mat4 p1S = scale(p1, vec3(plankL, plankT, plankW));
    Primitives::drawCube(shader, p1S, woodCol);

    // Plank 2 (sloping from lower-right to upper-left at -36 deg)
    mat4 p2 = model;
    p2 = translate(p2, vec3(0.0f, 0.11f, 0.0f));
    p2 = rotate(p2, radians(-36.0f), vec3(0.0f, 0.0f, 1.0f));
    mat4 p2S = scale(p2, vec3(plankL, plankT, plankW));
    Primitives::drawCube(shader, p2S, woodCol);

    // Central interlocking hinge pivot cylinder
    mat4 pivot = model;
    pivot = translate(pivot, vec3(0.0f, 0.11f, 0.0f));
    pivot = rotate(pivot, radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
    mat4 pivotS = scale(pivot, vec3(0.022f, plankW + 0.010f, 0.022f));
    Primitives::drawCylinder(shader, pivotS, carvedCol);

    // 2 Carved ornamental foot arches at base
    float footOffsets[2] = { -0.13f, 0.13f };
    for (int i = 0; i < 2; i++) {
        mat4 foot = model;
        foot = translate(foot, vec3(footOffsets[i], 0.015f, 0.0f));
        foot = scale(foot, vec3(0.035f, 0.030f, plankW * 0.95f));
        Primitives::drawCube(shader, foot, carvedCol);
    }
}

} // namespace Person
