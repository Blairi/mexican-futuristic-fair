#pragma once
#include "Light.h"

class DirectionalLight :
	public Light
{
public:
	DirectionalLight();
	DirectionalLight(GLfloat red, GLfloat green, GLfloat blue, 
					GLfloat aIntensity, GLfloat dIntensity,
					GLfloat xDir, GLfloat yDir, GLfloat zDir);

	void UseLight(GLfloat ambientIntensityLocation, GLfloat ambientcolorLocation,
		GLfloat diffuseIntensityLocation, GLfloat directionLocation);

	void setDirection(const glm::vec3& dir) {
		direction = glm::normalize(dir);
	}
	void setAmbientIntensity(GLfloat amb) {
		ambientIntensity = amb;
	}
	void setDiffuseIntensity(GLfloat diff) {
		diffuseIntensity = diff;
	}

	~DirectionalLight();

private:
	glm::vec3 direction;
};

