#pragma once
#include "Camera.h"
#include <glew.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <glfw3.h>

class FreeCamera : public Camera {
public:
    // Firma del ctor llama base Camera(startPosition, up)
    FreeCamera(glm::vec3 startPosition,
        glm::vec3 startUp,
        GLfloat  startYaw,
        GLfloat  startPitch,
        GLfloat  startMoveSpeed,
        GLfloat  startTurnSpeed);

    // Overrides
    glm::mat4 getViewMatrix() override;
    void      update(float deltaTime) override;
    void      keyControl(bool* keys, float deltaTime) override;
    void      mouseControl(float xChange, float yChange) override;

    // setters opcionales
    void setPosition(const glm::vec3& pos);
    void setDirection(const glm::vec3& dir);

    // getters que el base exige
    glm::vec3 getCameraPosition() const override;
    glm::vec3 getCameraDirection() const override;

private:
    // renombramos esta para no chocar con update(float)
    void updateCameraVectors();

    glm::vec3 front, right, worldUp;
    GLfloat   yaw, pitch;
    GLfloat   moveSpeed, turnSpeed;
};
