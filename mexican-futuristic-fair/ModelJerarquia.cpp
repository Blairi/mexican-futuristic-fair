#include "ModelJerarquia.h"
#include <iostream> // Para manejo de errores
#include <fstream> // Para verificar si existe el archivo


ModelJerarquia::ModelJerarquia(const std::string& filePath) {
	
	NameItems[0] = "/Head.obj";
	NameItems[1] = "/Body.obj";
	NameItems[2] = "/ArmL.obj";
	NameItems[3] = "/ArmR.obj";
	NameItems[4] = "/LegL.obj";
	NameItems[5] = "/LegR.obj";

	modelPath = filePath;
	modelroot = glm::mat4(1.0);
	initAngulo = 0.0f;
	globalScale = glm::vec3(1.0f, 1.0f, 1.0f); // Inicializa escala a 1
	hasHead = true;
}

void ModelJerarquia::SetGlobalScale(glm::vec3 scale) {
	globalScale = scale;
}

void ModelJerarquia::InitModel(glm::vec3 initialPos) {
	actualPos = initialPos;
	ModelItmes.clear(); // Limpiar cualquier modelo previo

	// Preparamos 6 espacios para los modelos
	for (int i = 0; i < 6; i++) {
		ModelItmes.push_back(Model());
	}

	// Verificamos si existe el archivo de cabeza
	std::string headPath = modelPath + NameItems[0];
	std::ifstream file(headPath);
	hasHead = file.good();
	file.close();

	// Cargamos los modelos que existan
	for (int i = 0; i < 6; i++) {
		if (i == 0 && !hasHead) {
			// Saltamos la cabeza si no existe
			continue;
		}

		try {
			std::string modelFile = modelPath + NameItems[i];
			ModelItmes[i].LoadModel(modelFile);
		}
		catch (...) {
			std::cout << "Warning: No se pudo cargar " << NameItems[i] << " para " << modelPath << std::endl;
			// Continuamos con el siguiente modelo
		}
	}

	// Definimos el Nodo Padre de la Gerarquia
	modelroot = glm::mat4(1.0);
	modelroot = glm::translate(modelroot, actualPos);
	modelroot = glm::scale(modelroot, globalScale); // Aplicamos escala global

	MatrixModels.clear();
	for (int i = 0; i < 6; i++) {
		MatrixModels.push_back(modelroot);
	}
}


void ModelJerarquia::RenderModelJ(const GLuint& uniformModel) {
	for (int i = 0; i < 6; i++) {
		if (i == 0 && !hasHead) {
			// Saltamos la cabeza si no existe
			continue;
		}

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(MatrixModels[i]));
		ModelItmes[i].RenderModel();
	}
}

// Modificar los métodos de transformación para aplicar la escala global
void ModelJerarquia::TransformHead(glm::vec3 tras, glm::vec3 rot, GLfloat angulo, glm::vec3 sca) {
	if (!hasHead) return; // Saltamos si no hay cabeza

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


void ModelJerarquia::TranformFullModel(glm::vec3 mueve, glm::mat4 extraTransform) {
	glm::vec3 nuevaPos = actualPos + mueve;

	modelroot = glm::mat4(1.0f);
	modelroot = glm::translate(modelroot, nuevaPos);
	modelroot *= extraTransform; // APLICA transformaciones extras
	modelroot = glm::scale(modelroot, globalScale); // Aplicamos escala global


	for (int i = 0; i < 6; i++) {
		MatrixModels[i] = modelroot;
	}
}

void ModelJerarquia::MovFullModel(glm::vec3 mueve, glm::vec3 rota, GLfloat angulo) {

	glm::vec3 nuevaPos;
	nuevaPos.x = actualPos.x + mueve.x;
	nuevaPos.y = actualPos.y + mueve.y;
	nuevaPos.z = actualPos.z + mueve.z;

	modelroot = glm::mat4(1.0);
	modelroot = glm::translate(modelroot, nuevaPos);
	modelroot = glm::rotate(modelroot, glm::radians(initAngulo + angulo), rota);
	modelroot = glm::scale(modelroot, globalScale); // Aplicamos escala global


	for (int i = 0; i < 6; i++) {
		MatrixModels[i] = modelroot;
	}
}

void ModelJerarquia::OrientToCamera(glm::vec3 camFront)
{
	// Calcular la matriz de rotacion que hace que el modelo apunte hacia camFront
	glm::mat4 orient = glm::inverse(glm::lookAt(glm::vec3(0.0f), camFront, glm::vec3(0.0f, 1.0f, 0.0f)));

	// Aplicar la orientacion a cada parte de la jerarqu�a
	for (int i = 0; i < 6; i++) {
		MatrixModels[i] *= orient;
	}
}

std::vector<glm::mat4> ModelJerarquia::getModelMatrixs() {
	return this->MatrixModels;
}

ModelJerarquia::~ModelJerarquia()
{
}