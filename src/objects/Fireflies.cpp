// Fireflies.cpp — Bioluminescent Fireflies (Jonaki Poka / জোনাকি পোকা)
// Features authentic ecological clustering across the village landscape:
// bamboo groves, riverbanks, water lilies, paddy fields, courtyards, and trees.
// Driven by 3D harmonic Lissajous drift and non-linear enzymatic bioluminescent pulsation.

#include "objects/Fireflies.h"
#include "Primitives.h"
#include <cmath>

using namespace math;

namespace Fireflies {

struct FireflyDef {
    vec3  basePos;
    float flightSpeed;
    float flightRadius;
    float pulseFreq;
    float pulsePhase;
    float coreRadius;
    vec3  color;
};

// Deterministic pseudo-random hash
static inline float hash(int seed, float salt)
{
    float v = sinf((float)seed * 12.9898f + salt * 78.233f) * 43758.5453f;
    return v - floorf(v);
}

// Generate the 160 village fireflies deterministically
static const int TOTAL_FIREFLIES = 160;

static FireflyDef generateFirefly(int i)
{
    FireflyDef f;
    f.flightSpeed  = 0.45f + hash(i, 1.1f) * 0.55f;
    f.flightRadius = 0.35f + hash(i, 2.2f) * 0.45f;
    f.pulseFreq    = 1.4f  + hash(i, 3.3f) * 2.2f;
    f.pulsePhase   = hash(i, 4.4f) * 2.0f * PI;
    f.coreRadius   = 0.045f + hash(i, 5.5f) * 0.035f;

    // Radiant Sun-Yellow and Warm Golden Solar Bioluminescence (No blue, no green)
    float yellowTone = hash(i, 6.6f);
    if (yellowTone < 0.40f) {
        f.color = vec3(1.0f, 0.92f, 0.16f);  // bright radiant sun yellow
    } else if (yellowTone < 0.75f) {
        f.color = vec3(1.0f, 0.85f, 0.08f);  // warm golden solar amber
    } else {
        f.color = vec3(1.0f, 0.96f, 0.26f);  // brilliant warm sun spark
    }

    // Partition into authentic rural ecological clusters:
    int cluster = i % 6;

    if (cluster == 0) {
        // Cluster 0: Courtyard Gathering, Foreground Rustic Fence & Banana Plants
        // Fluttering around sitting elders, reading children, straw stack & foreground bamboo fence
        float x = -5.2f + hash(i, 7.1f) * 6.5f;
        float y =  0.25f + hash(i, 8.2f) * 1.6f;
        float z =  0.2f  + hash(i, 9.3f) * 4.2f;
        f.basePos = vec3(x, y, z);
    }
    else if (cluster == 1) {
        // Cluster 1: Bamboo Grove & Orchard
        // Jonaki love dark dense bamboo foliage and fruit tree canopies
        float x = -13.5f + hash(i, 7.1f) * 8.0f;
        float y =   0.60f + hash(i, 8.2f) * 2.8f;
        float z = -17.5f + hash(i, 9.3f) * 10.0f;
        f.basePos = vec3(x, y, z);
    }
    else if (cluster == 2) {
        // Cluster 2: River Shore, Kashbon Reeds & Water Lilies (Shapla)
        // Dancing low over water ripples and catkin plumes along riverbank
        float z = -16.0f + hash(i, 9.3f) * 32.0f;
        float x =   4.2f + hash(i, 7.1f) * 4.0f;
        float y =   0.25f + hash(i, 8.2f) * 1.6f;
        f.basePos = vec3(x, y, z);
    }
    else if (cluster == 3) {
        // Cluster 3: Terraced Paddy Fields & Earthen Dikes (Aal)
        // Gliding over rows of rice plants and irrigation dikes
        float x = -18.0f + hash(i, 7.1f) * 10.0f;
        float y =   0.35f + hash(i, 8.2f) * 1.5f;
        float z = -24.0f + hash(i, 9.3f) * 11.0f;
        f.basePos = vec3(x, y, z);
    }
    else if (cluster == 4) {
        // Cluster 4: Historic Village Mosque Perimeter & River Bend Shallows
        // Fluttering near stone steps, ablution tank, date palms and river bend
        float x =  -6.5f + hash(i, 7.1f) * 6.5f;
        float y =   0.45f + hash(i, 8.2f) * 2.2f;
        float z = -21.0f + hash(i, 9.3f) * 8.0f;
        f.basePos = vec3(x, y, z);
    }
    else {
        // Cluster 5: North Homestead, Cow Shed & Canopy Air Drift
        float x = -12.0f + hash(i, 7.1f) * 16.0f;
        float y =   1.2f + hash(i, 8.2f) * 3.2f;
        float z =  -8.0f + hash(i, 9.3f) * 22.0f;
        f.basePos = vec3(x, y, z);
    }

    return f;
}

void draw(Shader& shader, float animTime, bool isNight)
{
    // Fireflies only emerge and blink during the night!
    if (!isNight) return;

    // Emissive mode bypasses lighting calculations so fireflies radiate pure luminous glow
    shader.setFloat("emissive", 1.0f);

    for (int i = 0; i < TOTAL_FIREFLIES; i++) {
        FireflyDef f = generateFirefly(i);

        // 1. Organic 3D Lissajous floating drift
        float t = animTime * f.flightSpeed;
        float dx = f.flightRadius * (0.65f * sinf(t + f.pulsePhase) + 0.35f * cosf(t * 1.63f + f.pulsePhase * 0.8f));
        float dy = (f.flightRadius * 0.55f) * (0.70f * sinf(t * 1.37f + f.pulsePhase * 1.4f) + 0.30f * cosf(t * 2.81f));
        float dz = f.flightRadius * (0.65f * cosf(t * 0.91f + f.pulsePhase * 0.7f) + 0.35f * sinf(t * 2.19f));

        vec3 currentPos = f.basePos + vec3(dx, dy, dz);

        // 2. Realistic nocturnal bioluminescent blink cycle
        // Real fireflies spend ~75% of time OFF, blinking ON with sharp, luminous sun flashes
        float period = 1.8f + hash(i, 3.3f) * 1.6f; // each fly has an individual 1.8s - 3.4s cycle
        float cyclePos = fmodf(animTime + f.pulsePhase * period, period) / period; // [0, 1)

        float flash = 0.0f;
        bool isDoubleBlinker = (hash(i, 11.1f) < 0.30f); // 30% of fireflies exhibit quick double-winks

        if (isDoubleBlinker) {
            // First pulse: cyclePos in [0.0, 0.11]
            if (cyclePos < 0.11f) {
                float u = cyclePos / 0.11f;
                flash = powf(sinf(u * (float)PI), 1.5f);
            }
            // Inter-pulse dark gap: [0.11, 0.15] (dark)
            // Second pulse: [0.15, 0.26]
            else if (cyclePos >= 0.15f && cyclePos < 0.26f) {
                float u = (cyclePos - 0.15f) / 0.11f;
                flash = powf(sinf(u * (float)PI), 1.5f);
            }
            // Remaining 74% of cycle: completely OFF
        } else {
            // Single bright luminous flash: first 22% of cycle
            const float FLASH_WINDOW = 0.22f;
            if (cyclePos < FLASH_WINDOW) {
                float u = cyclePos / FLASH_WINDOW;
                flash = powf(sinf(u * (float)PI), 1.5f);
            }
            // Remaining 78% of cycle: completely OFF
        }

        // When not flashing, the firefly is dark / dormant in the night
        if (flash < 0.015f) continue;

        // 3. Render luminous warm sun-yellow spark
        float currentRadius = f.coreRadius * (0.85f + 0.65f * flash);
        vec3 finalColor = f.color * (0.45f + 0.55f * flash);

        mat4 coreM = mat4::identity();
        coreM = translate(coreM, currentPos);
        coreM = scale(coreM, vec3(currentRadius));
        Primitives::drawSphere(shader, coreM, finalColor);
    }

    // Restore standard shading mode for subsequent scene rendering
    shader.setFloat("emissive", 0.0f);
}

} // namespace Fireflies
