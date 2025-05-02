#pragma once
#pragma once
#include "Camera.h"
#include <vector>

class ObjectFocusCamera : public Camera {
public:
    ObjectFocusCamera(glm::vec3 up, const std::vector<glm::vec3*>& objects);

    glm::mat4 getViewMatrix() override;
    void      update(float deltaTime) override;
    void      nextObject();

    // **Nuevos**:
    glm::vec3 getCameraPosition() const override;
    glm::vec3 getCameraDirection() const override;

private:
    std::vector<glm::vec3*> targets;
    int currentTarget;
};