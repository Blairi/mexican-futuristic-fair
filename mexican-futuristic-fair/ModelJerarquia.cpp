#include "ModelJerarquia.h"

ModelJerarquia::ModelJerarquia(const std::string& filePath) {
	
	NameItems[0] = "/Head.obj";
	NameItems[1] = "/Body.obj";
	NameItems[2] = "/ArmL.obj";
	NameItems[3] = "/ArmR.obj";
	NameItems[4] = "/LegL.obj";
	NameItems[5] = "/LegR.obj";

	modelPath = filePath;
	modelroot = glm::mat4(1.0);
}

void ModelJerarquia::InitModel(glm::vec3 initialPos) {

	actualPos = initialPos;

	for (int i = 0; i < 6; i++) {
		ModelItmes.push_back(Model());
		ModelItmes[i].LoadModel(modelPath + NameItems[i]);
	}

	// Definimos el Nodo Padre de la Gerarquia
	modelroot = glm::translate(modelroot, actualPos);
	
	for (int i = 0; i < 6; i++) {
		MatrixModels.push_back(glm::mat4(1.0));
		MatrixModels[i] = modelroot;
	}

}

void ModelJerarquia::RenderModelJ(const GLuint& uniformModel) {

	for (int i = 0; i < 6; i++) {
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(MatrixModels[i]));
		ModelItmes[i].RenderModel();
	}
}

void ModelJerarquia::TransformHead(glm::vec3 tras, glm::vec3 rot, GLfloat angulo, glm::vec3 sca) {
	MatrixModels[0] = modelroot;
	MatrixModels[0] = glm::translate(MatrixModels[0], tras);
	MatrixModels[0] = glm::rotate(MatrixModels[0], glm::radians(angulo), rot);
	MatrixModels[0] = glm::scale(MatrixModels[0], sca);
}

void ModelJerarquia::TransformBody(glm::vec3 tras, glm::vec3 rot, GLfloat angulo, glm::vec3 sca) {
	MatrixModels[1] = modelroot;
	MatrixModels[1] = glm::translate(MatrixModels[1], tras);
	MatrixModels[1] = glm::rotate(MatrixModels[1], glm::radians(angulo), rot);
	MatrixModels[1] = glm::scale(MatrixModels[1], sca);
}

void ModelJerarquia::TransformArmL(glm::vec3 tras, glm::vec3 rot, GLfloat angulo, glm::vec3 sca) {
	MatrixModels[2] = modelroot;
	MatrixModels[2] = glm::translate(MatrixModels[2], tras);
	MatrixModels[2] = glm::rotate(MatrixModels[2], glm::radians(angulo), rot);
	MatrixModels[2] = glm::scale(MatrixModels[2], sca);
}

void ModelJerarquia::TransformArmR(glm::vec3 tras, glm::vec3 rot, GLfloat angulo, glm::vec3 sca) {
	MatrixModels[3] = modelroot;
	MatrixModels[3] = glm::translate(MatrixModels[3], tras);
	MatrixModels[3] = glm::rotate(MatrixModels[3], glm::radians(angulo), rot);
	MatrixModels[3] = glm::scale(MatrixModels[3], sca);
}

void ModelJerarquia::TransformLegL(glm::vec3 tras, glm::vec3 rot, GLfloat angulo, glm::vec3 sca) {
	MatrixModels[4] = modelroot;
	MatrixModels[4] = glm::translate(MatrixModels[4], tras);
	MatrixModels[4] = glm::rotate(MatrixModels[4], glm::radians(angulo), rot);
	MatrixModels[4] = glm::scale(MatrixModels[4], sca);
}

void ModelJerarquia::TransformLegR(glm::vec3 tras, glm::vec3 rot, GLfloat angulo, glm::vec3 sca) {
	MatrixModels[5] = modelroot;
	MatrixModels[5] = glm::translate(MatrixModels[5], tras);
	MatrixModels[5] = glm::rotate(MatrixModels[5], glm::radians(angulo), rot);
	MatrixModels[5] = glm::scale(MatrixModels[5], sca);
}

void ModelJerarquia::MovFullModel(glm::vec3 mueve) {
	
	glm::vec3 nuevaPos;
	nuevaPos.x = actualPos.x + mueve.x;
	nuevaPos.y = actualPos.y + mueve.y;
	nuevaPos.z = actualPos.z + mueve.z;

	modelroot = glm::mat4(1.0);
	modelroot = glm::translate(modelroot, nuevaPos);

	for (int i = 0; i < 6; i++) {
		MatrixModels[i] = modelroot;
	}
}

ModelJerarquia::~ModelJerarquia()
{
}