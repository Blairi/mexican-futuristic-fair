#include "ObjectFocusCamera.h"
#include <gtc/matrix_transform.hpp>

ObjectFocusCamera::ObjectFocusCamera(glm::vec3 up,
    const std::vector<glm::vec3*>& objects)
    : Camera(glm::vec3(0.0f), up),
    targets(objects),
    currentTarget(0)
{}

glm::mat4 ObjectFocusCamera::getViewMatrix() {
    glm::vec3* tgt = tempTarget ? tempTarget : targets[currentTarget];

    position = *tgt + glm::vec3(0, 2, 5);

    return glm::lookAt(position, *tgt, up);

}

void ObjectFocusCamera::beginFocus(glm::vec3* tgt) { tempTarget = tgt; }
void ObjectFocusCamera::endFocus() { tempTarget = nullptr; }

void ObjectFocusCamera::update(float /*deltaTime*/) {}

void ObjectFocusCamera::nextObject() {
    if (isFocusing()) return;            // no cambies mientras hay diálogo
    currentTarget = (currentTarget + 1) % targets.size();
}

// **Implementar getters**
glm::vec3 ObjectFocusCamera::getCameraPosition() const {
    return position;
}
glm::vec3 ObjectFocusCamera::getCameraDirection() const {
    return glm::normalize((*targets[currentTarget] - position));
}
