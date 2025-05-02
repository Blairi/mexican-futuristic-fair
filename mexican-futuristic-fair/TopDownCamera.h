#pragma once
#include "Camera.h"

class TopDownCamera : public Camera {
public:
    TopDownCamera(glm::vec3 center, float height, glm::vec3 up);

    glm::mat4 getViewMatrix() override;
    void update(float deltaTime) override;

    void moveHorizontal(float amount);
    void moveVertical(float amount);
    glm::vec3 getCameraPosition() const override;
    glm::vec3 getCameraDirection() const override;

private:
    glm::vec3 centerPoint;
};
