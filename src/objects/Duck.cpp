// Duck.cpp — Village duck built from sphere body/head, flat cone beak,
//            short cylinder legs. Wider and rounder than the hen.

#include "objects/Duck.h"
#include "Primitives.h"

using namespace math;

namespace Duck {

void draw(Shader& shader, const mat4& model)
{
    vec3 bodyCol (0.82f, 0.82f, 0.78f);   // off-white
    vec3 headCol (0.20f, 0.35f, 0.15f);   // greenish (male mallard head)
    vec3 beakCol (0.90f, 0.60f, 0.10f);   // orange
    vec3 legCol  (0.90f, 0.55f, 0.10f);   // orange
    vec3 tailCol (0.60f, 0.58f, 0.52f);   // grey-ish

    // ── Body (wider, flatter sphere) ────────────────────────────
    mat4 body = model;
    body = translate(body, vec3(0.0f, 0.25f, 0.0f));
    body = scale(body, vec3(0.28f, 0.20f, 0.35f));
    Primitives::drawSphere(shader, body, bodyCol);

    // ── Head (sphere, forward and up) ───────────────────────────
    mat4 head = model;
    head = translate(head, vec3(0.0f, 0.40f, 0.28f));
    head = scale(head, vec3(0.11f, 0.11f, 0.11f));
    Primitives::drawSphere(shader, head, headCol);

    // ── Beak (flat cone, pointing forward) ──────────────────────
    mat4 beak = model;
    beak = translate(beak, vec3(0.0f, 0.38f, 0.39f));
    beak = rotate(beak, radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
    beak = scale(beak, vec3(0.06f, 0.10f, 0.03f));   // wide and flat
    Primitives::drawCone(shader, beak, beakCol);

    // ── Legs (short cylinders, spread wider) ────────────────────
    for (int side = -1; side <= 1; side += 2) {
        mat4 leg = model;
        leg = translate(leg, vec3(side * 0.10f, 0.06f, 0.0f));
        leg = scale(leg, vec3(0.025f, 0.12f, 0.025f));
        Primitives::drawCylinder(shader, leg, legCol);
    }

    // ── Tail (small upward cone at back) ────────────────────────
    mat4 tail = model;
    tail = translate(tail, vec3(0.0f, 0.30f, -0.30f));
    tail = rotate(tail, radians(-50.0f), vec3(1.0f, 0.0f, 0.0f));
    tail = scale(tail, vec3(0.06f, 0.15f, 0.06f));
    Primitives::drawCone(shader, tail, tailCol);
}

} // namespace Duck
