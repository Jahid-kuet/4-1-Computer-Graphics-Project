// Shader.h — Compiles and links GLSL vertex/fragment shaders.
// Provides helpers to set uniforms (mat4, vec3, float, int).
#pragma once

#include "mathutil.h"

class Shader {
public:
    unsigned int ID = 0;

    Shader() = default;

    // Compile and link shaders from source strings
    void init(const char* vertexSrc, const char* fragmentSrc);

    // Activate this shader program
    void use() const;

    // Uniform setters
    void setBool (const char* name, bool  value) const;
    void setInt  (const char* name, int   value) const;
    void setFloat(const char* name, float value) const;
    void setVec3 (const char* name, const math::vec3& v) const;
    void setVec3 (const char* name, float x, float y, float z) const;
    void setMat4 (const char* name, const math::mat4& mat) const;

    // Pre-built Phong shader sources (defined in Shader.cpp)
    static const char* phongVertexSrc;
    static const char* phongFragmentSrc;

private:
    void checkCompileErrors(unsigned int shader, const char* type) const;
};
