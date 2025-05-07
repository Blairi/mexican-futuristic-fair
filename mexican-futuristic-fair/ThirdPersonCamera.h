#pragma once
#include "Camera.h"
#include <glm.hpp>
class ThirdPersonCamera : public Camera {
public:
    ThirdPersonCamera(glm::vec3 offset, glm::vec3 up, glm::vec3* targetObject);

    glm::mat4 getViewMatrix() override;
    void      update(float dt) override;
    void      mouseControl(float xDelta, float yDelta);

    // acceso
    float     getYaw()  const;
    glm::vec3 getCameraPosition()   const override;
    glm::vec3 getCameraDirection()  const override;
    glm::vec3 getHorizontalDirection() const;

    // zoom con la rueda
    void      addDistance(float d);

private:
    void      recalcOffsetDir();          // <— solo dirección
    glm::vec3* target;
    glm::vec3  offsetDir;                 // vector unitario
    float      distance;                  // radio constante
    float      yaw, pitch, turnSpeed;
};