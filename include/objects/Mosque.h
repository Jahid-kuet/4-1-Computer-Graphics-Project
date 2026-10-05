// Mosque.h — Historic Terracotta Brick & Old Red Stone Mosque (সুলতানি আমলের লাল ইট ও প্রাচীন লাল পাথরের ঐতিহ্যবাহী গ্রামীণ মসজিদ).
// Inspired by historic 15th-century Bengal Sultanate architecture (Shat Gombuj, Goaldi, Bagha, Atia):
// Weathered red terracotta brick walls, ancient carved red sandstone foundation plinth & columns,
// central ribbed terracotta dome with antique bronze Crescent Moon & Star (Chand-Tara),
// soaring red brick Azaan Minaret with 4 Quad Horn Loudspeakers (চোঙা মাইক),
// front 3-bay multi-cusped terracotta arched veranda with shoe shelf, stepped Kangura parapet merlons,
// arched seasoned timber double doors with warm golden interior prayer glow,
// projecting western Mehrab bay, and red brick ablution platform (Paka Ozukhana) with cistern, brass taps & Bodnas.
// Procedurally constructed using OpenGL 3.3 canonical geometric primitives.
#pragma once

#include "Shader.h"
#include "mathutil.h"

namespace Mosque {
    void draw(Shader& shader, const math::mat4& model);
}
