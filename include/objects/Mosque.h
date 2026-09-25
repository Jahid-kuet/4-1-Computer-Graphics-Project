// Mosque.h — Historic Old-Style Bengali Rural Village Mosque (ঐতিহাসিক প্রাচীন গ্রামীণ মসজিদ).
// Inspired by ancient Sultanate & Mughal Bengal terracotta brick architecture (e.g. Goaldi & Bagerhat).
// Procedurally constructed using OpenGL 3.3 canonical geometric primitives.
#pragma once

#include "Shader.h"
#include "mathutil.h"

namespace Mosque {
    // Draws an authentic historic rural Bengali village mosque complete with
    // raised terracotta brick plinth, curved Bengal Sultanate roof cornice (Dochala curve),
    // four sturdy octagonal corner buttress towers (Burj) with molded ring bands,
    // central grand terracotta dome with lotus Kalasa, multi-cusped pointed entrance portal
    // with rosette spandrels and hanging lantern, terracotta jali openwork windows,
    // projecting western Mehrab bay, and masonry ablution cistern (Paka Houz) with clay Bodnas.
    void draw(Shader& shader, const math::mat4& model);
}
