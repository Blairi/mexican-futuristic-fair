#pragma once
#include "PointLight.h"
class SpotLight :
	public PointLight
{
public:
	SpotLight();

	SpotLight(GLfloat red, GLfloat green, GLfloat blue,
		GLfloat aIntensity, GLfloat dIntensity,
		GLfloat xPos, GLfloat yPos, GLfloat zPos,
		GLfloat xDir, GLfloat yDir, GLfloat zDir,
		GLfloat con, GLfloat lin, GLfloat exp,
		GLfloat edg);

	void UseLight(GLuint ambientIntensityLocation, GLuint ambientColourLocation,
		GLuint diffuseIntensityLocation, GLuint positionLocation, GLuint directionLocation,
		GLuint constantLocation, GLuint linearLocation, GLuint exponentLocation,
		GLuint edgeLocation);

	void SetFlash(glm::vec3 pos, glm::vec3 dir);
	void SetPos(glm::vec3 pos);

	void setAmbientIntensity(GLfloat amb) {
		ambientIntensity = amb;
	}
	void setDiffuseIntensity(GLfloat diff) {
		diffuseIntensity = diff;
	}

	void setAtenuacion(GLfloat con, GLfloat lin, GLfloat exp) {
		constant = con;
		linear = lin;
		exponent = exp;
	}

	~SpotLight();

private:
	glm::vec3 direction;

	GLfloat edge, procEdge;
};
