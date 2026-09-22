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

    // ── 1. Anatomical Tapered Torso (Cotton Panjabi / Kurta) ──────
    // Upper Chest & Pectorals (broader across shoulders)
    mat4 chest = model;
    chest = translate(chest, vec3(0.0f, torsoBase + torsoH * 0.72f, 0.0f));
    chest = scale(chest, vec3(chestW, torsoH * 0.56f, chestD));
    Primitives::drawCylinder(shader, chest, p.shirtColor);

    // Lean Waist & Abdomen (smooth anatomical taper toward belt/hips)
    mat4 abdomen = model;
    abdomen = translate(abdomen, vec3(0.0f, torsoBase + torsoH * 0.28f, 0.0f));
    abdomen = scale(abdomen, vec3(waistW, torsoH * 0.56f, waistD));
    Primitives::drawCylinder(shader, abdomen, p.shirtColor);

    // Shoulder deltoid caps (smooth shoulder transition, eliminates floating arm gap)
    for (int side = -1; side <= 1; side += 2) {
        float fside = (float)side;
        mat4 shoulderCap = model;
        shoulderCap = translate(shoulderCap, vec3(fside * (chestW * 0.95f), torsoTop - 0.045f, 0.0f));
        shoulderCap = scale(shoulderCap, vec3(0.038f, 0.038f, 0.038f));
        Primitives::drawSphere(shader, shoulderCap, p.shirtColor);
    }

    // Traditional Mandarin / Ban Collar at neckline
    mat4 collar = model;
    collar = translate(collar, vec3(0.0f, torsoTop + 0.008f, 0.005f));
    collar = scale(collar, vec3(0.058f, 0.022f, 0.058f));
    Primitives::drawCylinder(shader, collar, p.shirtColor * 0.92f);

    // Front Button Placket (iconic Bengali Kurta vertical button strip)
    mat4 placket = model;
    placket = translate(placket, vec3(0.0f, torsoBase + torsoH * 0.68f, chestD + 0.002f));
    placket = scale(placket, vec3(0.016f, torsoH * 0.44f, 0.004f));
    Primitives::drawCube(shader, placket, p.shirtColor * 0.82f);

    // Tailored Kurta / Panjabi Tunic Hem (falls neatly over hips, no bulky barrel)
    mat4 tunicHem = model;
    tunicHem = translate(tunicHem, vec3(0.0f, torsoBase - 0.035f, 0.0f));
    tunicHem = scale(tunicHem, vec3(waistW * 1.08f, 0.12f, waistD * 1.08f));
    Primitives::drawCylinder(shader, tunicHem, p.shirtColor);

    // ── 2. Traditional Draped Gamcha (Cotton Towel over shoulder) ─
    if (p.hasGamcha) {
        mat4 shawlFront = model;
        shawlFront = translate(shawlFront, vec3(chestW * 0.60f, torsoBase + torsoH * 0.55f, chestD + 0.006f));
        shawlFront = scale(shawlFront, vec3(0.060f, torsoH * 0.65f, 0.016f));
        Primitives::drawCube(shader, shawlFront, p.gamchaColor);

        mat4 shawlTop = model;
        shawlTop = translate(shawlTop, vec3(chestW * 0.60f, torsoTop + 0.012f, 0.0f));
        shawlTop = scale(shawlTop, vec3(0.065f, 0.024f, chestD * 1.80f));
        Primitives::drawCube(shader, shawlTop, p.gamchaColor);

        mat4 shawlBack = model;
        shawlBack = translate(shawlBack, vec3(chestW * 0.60f, torsoBase + torsoH * 0.60f, -chestD - 0.006f));
        shawlBack = scale(shawlBack, vec3(0.060f, torsoH * 0.55f, 0.016f));
        Primitives::drawCube(shader, shawlBack, p.gamchaColor);
    }

    // ── 3. Neck & Head ───────────────────────────────────────────
    float headCenterY = torsoTop + headR + 0.020f;
    mat4 neck = model;
    neck = translate(neck, vec3(0.0f, torsoTop + 0.020f, 0.0f));
    neck = scale(neck, vec3(0.038f, 0.048f, 0.038f));
    Primitives::drawCylinder(shader, neck, p.skinColor);

    mat4 head = model;
    head = translate(head, vec3(0.0f, headCenterY, 0.0f));
    head = scale(head, vec3(headR * 0.90f, headR * 1.05f, headR * 0.92f));
    Primitives::drawSphere(shader, head, p.skinColor);

    // ── 4. Hair & Elder Beard ────────────────────────────────────
    vec3 actualHairCol = p.isElder ? vec3(0.90f, 0.90f, 0.90f) : p.hairColor;

    mat4 hair = model;
    hair = translate(hair, vec3(0.0f, headCenterY + headR * 0.25f, -headR * 0.15f));
    hair = scale(hair, vec3(headR * 0.96f, headR * 0.88f, headR * 0.98f));
    Primitives::drawSphere(shader, hair, actualHairCol);

    if (p.isElder) {
        mat4 beard = model;
        beard = translate(beard, vec3(0.0f, headCenterY - headR * 0.85f, headR * 0.38f));
        beard = rotate(beard, radians(15.0f), vec3(1.0f, 0.0f, 0.0f));
        beard = scale(beard, vec3(0.052f, 0.13f, 0.052f));
        Primitives::drawCone(shader, beard, actualHairCol);
    }

    // ── 5. Slender Anatomical Arms & Hands ─────────────────────────
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

                // Upper arm
                mat4 upper = shoulder;
                upper = translate(upper, vec3(0.0f, -upperArmH * 0.5f, 0.0f));
                upper = scale(upper, vec3(armR, upperArmH, armR));
                Primitives::drawCylinder(shader, upper, p.shirtColor);

                // Elbow: bent forward and upward to raise the hand holding the fan
                mat4 elbow = shoulder;
                elbow = translate(elbow, vec3(0.0f, -upperArmH, 0.0f));
                elbow = rotate(elbow, radians(-65.0f), vec3(1.0f, 0.0f, 0.0f)); // raised forward
                elbow = rotate(elbow, radians(-18.0f), vec3(0.0f, 1.0f, 0.0f)); // angled slightly inward

                // Forearm
                mat4 lower = elbow;
                lower = translate(lower, vec3(0.0f, -lowerArmH * 0.5f, 0.0f));
                lower = scale(lower, vec3(forearmR, lowerArmH, forearmR));
                Primitives::drawCylinder(shader, lower, p.shirtColor);

                // Sleeve cuff
                mat4 cuff = elbow;
                cuff = translate(cuff, vec3(0.0f, -lowerArmH, 0.0f));
                cuff = scale(cuff, vec3(forearmR * 1.08f, 0.015f, forearmR * 1.08f));
                Primitives::drawCylinder(shader, cuff, p.shirtColor * 0.88f);

                // Hand firmly grasping the bamboo handle
                mat4 hand = elbow;
                hand = translate(hand, vec3(0.0f, -lowerArmH - 0.020f, 0.005f));
                hand = scale(hand, vec3(0.024f, 0.032f, 0.026f));
                Primitives::drawSphere(shader, hand, p.skinColor);

                // Traditional Handmade Fan (Haat Pakha / হাতপাখা) in the Elder's hand!
                mat4 fan = elbow;
                // Position grip in palm
                fan = translate(fan, vec3(0.0f, -lowerArmH - 0.020f, 0.005f));
                // Align fan handle with forearm/hand direction, blade extending upward & forward
                fan = rotate(fan, radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
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
            // Standing villager: slender, natural human arms with forward elbow bend
            float fwdSwing = -armAngle;

            mat4 shoulder = model;
            shoulder = translate(shoulder, vec3(shoulderX, torsoTop - 0.05f, 0.0f));
            shoulder = rotate(shoulder, fwdSwing, vec3(1.0f, 0.0f, 0.0f));
            shoulder = rotate(shoulder, radians(fside * -4.0f), vec3(0.0f, 0.0f, 1.0f));

            // Upper arm
            mat4 upper = shoulder;
            upper = translate(upper, vec3(0.0f, -upperArmH * 0.5f, 0.0f));
            upper = scale(upper, vec3(armR, upperArmH, armR));
            Primitives::drawCylinder(shader, upper, p.shirtColor);

            // Elbow: bends naturally forward (+Z)
            mat4 elbow = shoulder;
            elbow = translate(elbow, vec3(0.0f, -upperArmH, 0.0f));
            elbow = rotate(elbow, radians(-14.0f), vec3(1.0f, 0.0f, 0.0f));
            elbow = rotate(elbow, radians(fside * -3.0f), vec3(0.0f, 1.0f, 0.0f));

            // Forearm
            mat4 lower = elbow;
            lower = translate(lower, vec3(0.0f, -lowerArmH * 0.5f, 0.0f));
            lower = scale(lower, vec3(forearmR, lowerArmH, forearmR));
            Primitives::drawCylinder(shader, lower, p.shirtColor);

            // Wrist cuff trim
            mat4 cuff = elbow;
            cuff = translate(cuff, vec3(0.0f, -lowerArmH, 0.0f));
            cuff = scale(cuff, vec3(forearmR * 1.06f, 0.014f, forearmR * 1.06f));
            Primitives::drawCylinder(shader, cuff, p.shirtColor * 0.88f);

            // Hand
            mat4 hand = elbow;
            hand = translate(hand, vec3(0.0f, -lowerArmH - 0.020f, 0.005f));
            hand = scale(hand, vec3(0.020f, 0.034f, 0.026f));
            Primitives::drawSphere(shader, hand, p.skinColor);
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
        // TWO DISTINCT SLENDER LEGS DRAPED IN LUNGI!
        // No more bulky single-cylinder oil-drum!
        float legSpacing = 0.062f; // distance of each leg from body centerline
        float lungiTopY  = torsoBase + 0.01f;
        float lungiBotY  = 0.11f;
        float lungiH     = lungiTopY - lungiBotY;
        float lungiMidY  = (lungiTopY + lungiBotY) * 0.5f;

        // Central Pelvis drape (connects waist neatly to the two leg columns)
        mat4 pelvis = model;
        pelvis = translate(pelvis, vec3(0.0f, torsoBase - 0.045f, 0.0f));
        pelvis = scale(pelvis, vec3(waistW * 1.05f, 0.12f, waistD * 1.02f));
        Primitives::drawCylinder(shader, pelvis, p.pantsColor);

        // Center front pleat / crease (traditional "Kocha" fold of the Bengali Lungi)
        mat4 pleat = model;
        pleat = translate(pleat, vec3(0.0f, lungiMidY, waistD * 0.65f));
        pleat = scale(pleat, vec3(0.022f, lungiH * 0.95f, 0.014f));
        Primitives::drawCube(shader, pleat, p.pantsColor * 0.85f);

        // Two distinct, slender draped leg columns!
        for (int side = -1; side <= 1; side += 2) {
            float fside = (float)side;
            float lx = fside * legSpacing;

            // Slender leg column (thigh to ankle)
            mat4 legCol = model;
            legCol = translate(legCol, vec3(lx, lungiMidY, 0.0f));
            legCol = scale(legCol, vec3(0.046f, lungiH, 0.050f));
            Primitives::drawCylinder(shader, legCol, p.pantsColor);

            // Lungi hem border trim around ankle
            mat4 hem = model;
            hem = translate(hem, vec3(lx, lungiBotY, 0.0f));
            hem = scale(hem, vec3(0.048f, 0.020f, 0.052f));
            Primitives::drawCylinder(shader, hem, p.pantsColor * 0.82f);

            // Exposed slender ankle below lungi
            mat4 ankle = model;
            ankle = translate(ankle, vec3(lx, 0.065f, 0.0f));
            ankle = scale(ankle, vec3(0.024f, 0.090f, 0.024f));
            Primitives::drawCylinder(shader, ankle, p.skinColor);

            // Sandaled foot
            mat4 foot = model;
            foot = translate(foot, vec3(lx, 0.015f, 0.028f));
            foot = scale(foot, vec3(0.036f, 0.026f, 0.082f));
            Primitives::drawCube(shader, foot, vec3(0.28f, 0.18f, 0.10f));
        }
    }
}

} // namespace Person
