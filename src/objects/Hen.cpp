// Hen.cpp — Village hen built from sphere body, head, cone beak,
//           red comb, thin cylinder legs, and tail feathers.

#include "objects/Hen.h"
#include "Primitives.h"

using namespace math;

namespace Hen {

void draw(Shader& shader, const mat4& model)
{
    vec3 bodyCol (0.52f, 0.24f, 0.08f);   // reddish-brown
    vec3 wingCol (0.42f, 0.18f, 0.06f);   // darker folded wing
    vec3 headCol (0.46f, 0.20f, 0.07f);
    vec3 combCol (0.85f, 0.10f, 0.05f);   // bright red comb
    vec3 beakCol (0.90f, 0.65f, 0.15f);   // orange beak
    vec3 legCol  (0.85f, 0.65f, 0.20f);   // yellow-orange legs
    vec3 tailCol (0.28f, 0.10f, 0.04f);   // dark brown tail

    // ── Body (compact plump hen body) ───────────────────────────
    mat4 body = model;
    body = translate(body, vec3(0.0f, 0.18f, 0.0f));
    body = scale(body, vec3(0.12f, 0.11f, 0.16f));
    Primitives::drawSphere(shader, body, bodyCol);

    // ── Wings on sides ──────────────────────────────────────────
    for (int side = -1; side <= 1; side += 2) {
        float fside = (float)side;
        mat4 wing = model;
        wing = translate(wing, vec3(fside * 0.11f, 0.18f, -0.01f));
        wing = rotate(wing, radians(fside * 12.0f), vec3(0.0f, 0.0f, 1.0f));
        wing = scale(wing, vec3(0.025f, 0.075f, 0.12f));
        Primitives::drawCube(shader, wing, wingCol);
    }

    // ── Head ────────────────────────────────────────────────────
    mat4 head = model;
    head = translate(head, vec3(0.0f, 0.27f, 0.11f));
    head = scale(head, vec3(0.06f, 0.065f, 0.06f));
    Primitives::drawSphere(shader, head, headCol);

    // ── Comb ────────────────────────────────────────────────────
    mat4 comb = model;
    comb = translate(comb, vec3(0.0f, 0.335f, 0.11f));
    comb = scale(comb, vec3(0.02f, 0.035f, 0.045f));
    Primitives::drawSphere(shader, comb, combCol);

    // ── Beak ────────────────────────────────────────────────────
    mat4 beak = model;
    beak = translate(beak, vec3(0.0f, 0.26f, 0.17f));
    beak = rotate(beak, radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
    beak = scale(beak, vec3(0.016f, 0.045f, 0.018f));
    Primitives::drawCone(shader, beak, beakCol);

    // ── Legs & Claws ────────────────────────────────────────────
    for (int side = -1; side <= 1; side += 2) {
        float lx = (float)side * 0.045f;
        mat4 leg = model;
        leg = translate(leg, vec3(lx, 0.055f, 0.0f));
        leg = scale(leg, vec3(0.012f, 0.11f, 0.012f));
        Primitives::drawCylinder(shader, leg, legCol);

        mat4 foot = model;
        foot = translate(foot, vec3(lx, 0.008f, 0.02f));
        foot = scale(foot, vec3(0.03f, 0.012f, 0.04f));
        Primitives::drawCube(shader, foot, legCol);
    }

    // ── Tail Feathers ───────────────────────────────────────────
    mat4 tail = model;
    tail = translate(tail, vec3(0.0f, 0.22f, -0.13f));
    tail = rotate(tail, radians(-45.0f), vec3(1.0f, 0.0f, 0.0f));
    tail = scale(tail, vec3(0.04f, 0.13f, 0.06f));
    Primitives::drawCone(shader, tail, tailCol);
}

} // namespace Hen
