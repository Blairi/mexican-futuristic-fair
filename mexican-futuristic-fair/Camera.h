#pragma once
#include <glew.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <glfw3.h>

class Camera {
public:
    Camera(glm::vec3 startPosition, glm::vec3 startUp)
        : position(startPosition), up(startUp) {}
    virtual ~Camera() {}

    virtual glm::mat4 getViewMatrix() = 0;
    virtual void    update(float deltaTime) = 0;
    virtual void    keyControl(bool* keys, float deltaTime) {}
    virtual void    mouseControl(float xChange, float yChange) {}

    // **Nuevos** métodos puros para que ninguno de los derivados quede abstracto:
    virtual glm::vec3 getCameraPosition() const = 0;
    virtual glm::vec3 getCameraDirection() const = 0;

protected:
    glm::vec3 position;
    glm::vec3 up;
};
