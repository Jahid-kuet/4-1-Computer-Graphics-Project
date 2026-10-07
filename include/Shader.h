// Shader.h — Compiles and links GLSL vertex/fragment shaders.
// Provides helpers to set uniforms (mat4, vec3, float, int).
#pragma once

#include "mathutil.h"
#include <unordered_map>
#include <string>
#include <string_view>

struct TransparentStringHash {
    using is_transparent = void;
    size_t operator()(std::string_view sv) const noexcept {
        return std::hash<std::string_view>{}(sv);
    }
};

struct TransparentStringEqual {
    using is_transparent = void;
    bool operator()(std::string_view lhs, std::string_view rhs) const noexcept {
        return lhs == rhs;
    }
};

class Shader {
public:
    unsigned int ID = 0;

    // Pre-cached high-frequency uniform locations
    int locModel = -1;
    int locObjectColor = -1;

    Shader() = default;

    // Compile and link shaders from source strings
    void init(const char* vertexSrc, const char* fragmentSrc);

    // Activate this shader program
    void use() const;

    // Fast cached uniform location query
    int getUniformLocation(const char* name) const;

    // Ultra-fast direct uniform setters (bypassing all string lookups)
    void setFastModel(const math::mat4& mat) const;
    void setFastColor(const math::vec3& v) const;
    void setFastColor(float x, float y, float z) const;

    // Uniform getters
    int  getInt  (const char* name) const;

    // Uniform setters (accelerated with cached locations)
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
    mutable std::unordered_map<std::string, int, TransparentStringHash, TransparentStringEqual> m_uniformLocations;
};

