#pragma once
#include "Camera.h"
#include <glm.hpp>

class ThirdPersonCamera : public Camera {
public:
    ThirdPersonCamera(glm::vec3 offset,
        glm::vec3 up,
        glm::vec3* targetObject);

    glm::mat4 getViewMatrix() override;
    void      update(float deltaTime) override;
    void      keyControl(bool* keys, float deltaTime);
    void      mouseControl(float xChange, float yChange);

    // **Nuevos** getters:
    glm::vec3 getCameraPosition() const override;
    glm::vec3 getCameraDirection() const override;

private:
    void      calculateOffset();
    glm::vec3 offset;
    glm::vec3* target;
    float yaw, pitch, moveSpeed, turnSpeed;
};
