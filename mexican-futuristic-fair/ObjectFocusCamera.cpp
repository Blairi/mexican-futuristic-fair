#include "ObjectFocusCamera.h"
#include <gtc/matrix_transform.hpp>

ObjectFocusCamera::ObjectFocusCamera(glm::vec3 up,
    const std::vector<glm::vec3*>& objects)
    : Camera(glm::vec3(0.0f), up),
    targets(objects),
    currentTarget(0)
{}

glm::mat4 ObjectFocusCamera::getViewMatrix() {
    position = *targets[currentTarget] + glm::vec3(0, 2, 5);
    return glm::lookAt(position, *targets[currentTarget], up);
}

void ObjectFocusCamera::update(float /*deltaTime*/) {}

void ObjectFocusCamera::nextObject() {
    currentTarget = (currentTarget + 1) % targets.size();
}

// **Implementar getters**
glm::vec3 ObjectFocusCamera::getCameraPosition() const {
    return position;
}
glm::vec3 ObjectFocusCamera::getCameraDirection() const {
    return glm::normalize((*targets[currentTarget] - position));
}
