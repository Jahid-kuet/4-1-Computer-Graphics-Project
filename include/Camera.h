// Camera.h — Simple orbit camera for the object test scene.
// Orbits around a target point. Controlled by mouse drag + scroll.
#pragma once

#include "mathutil.h"

class Camera {
public:
    // Orbit parameters
    math::vec3 target;      // look-at target
    float yaw;              // horizontal angle (radians)
    float pitch;            // vertical angle (radians, clamped)
    float distance;         // distance from target

    // Projection
    float fov;              // field of view (radians)
    float aspectRatio;
    float nearPlane;
    float farPlane;

    // Computed from orbit params
    math::vec3 position;

    Camera();

    // Call after changing yaw/pitch/distance to recompute position
    void updatePosition();

    math::mat4 getViewMatrix()       const;
    math::mat4 getProjectionMatrix() const;

    // Input handlers (dx/dy in pixels, yoffset from scroll)
    void processMouseDrag(float dx, float dy, float sensitivity = 0.005f);
    void processScroll(float yoffset);

    // Keyboard movement (W/S for forward/back, A/D for left/right)
    void processKeyboardMovement(float forward, float right, float dt, float speed = 14.0f);
};
