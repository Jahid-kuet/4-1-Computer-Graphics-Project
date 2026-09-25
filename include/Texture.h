// Texture.h — Procedural 2D Texture Generator & OpenGL Texture Manager.
// Generates and manages authentic GPU textures (Wood, Brick, Bamboo/Thatch, Fabric)
// in pure OpenGL 3.3 Core Profile without external image file dependencies.
#pragma once

#include <glad/glad.h>
#include "Shader.h"

enum TextureType {
    TEX_WOOD   = 0,  // Concentric tree growth rings & longitudinal timber grain
    TEX_BRICK  = 1,  // Running-bond terracotta clay bricks with mortar joints
    TEX_BAMBOO = 2,  // Traditional woven bamboo mat / thatch split pattern (Chatai)
    TEX_FABRIC = 3   // Authentic rural Bengali checked lungi / gamcha plaid pattern
};

namespace Texture {
    // Generate and upload all procedural 2D textures to the GPU.
    // Call once during application startup after GLAD is initialized.
    void init();

    // Release all GPU texture objects during application cleanup.
    void cleanup();

    // Bind the specified texture type to the given OpenGL texture unit.
    void bind(TextureType type, unsigned int unit = 0);

    // Retrieve the OpenGL texture handle (GLuint) for a given texture type.
    GLuint getID(TextureType type);
}
