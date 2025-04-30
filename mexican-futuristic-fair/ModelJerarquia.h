#include "Model.h" 
#include <vector>
#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>

class ModelJerarquia: public Model {
	
	public: 
		
		ModelJerarquia(const std::string& filePath);
		
		void InitModel(glm::vec3 initialPos = glm::vec3(0.0f, 0.0f, 0.0f));
		void RenderModelJ(const GLuint& uniformModel);
		
		void TransformHead(
			glm::vec3 tras = glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3 rot = glm::vec3(0.0f, 0.0f, 1.0f),
			GLfloat angulo = 0.0f,
			glm::vec3 sca = glm::vec3(1.0f, 1.0f, 1.0f)
		);
		void TransformBody(
			glm::vec3 tras = glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3 rot = glm::vec3(0.0f, 0.0f, 1.0f),
			GLfloat angulo = 0.0f,
			glm::vec3 sca = glm::vec3(1.0f, 1.0f, 1.0f)
		);
		void TransformArmL(
			glm::vec3 tras = glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3 rot = glm::vec3(0.0f, 0.0f, 1.0f),
			GLfloat angulo = 0.0f,
			glm::vec3 sca = glm::vec3(1.0f, 1.0f, 1.0f)
		);
		void TransformArmR(
			glm::vec3 tras = glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3 rot = glm::vec3(0.0f, 0.0f, 1.0f),
			GLfloat angulo = 0.0f,
			glm::vec3 sca = glm::vec3(1.0f, 1.0f, 1.0f)
		);
		void TransformLegL(
			glm::vec3 tras = glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3 rot = glm::vec3(0.0f, 0.0f, 1.0f),
			GLfloat angulo = 0.0f,
			glm::vec3 sca = glm::vec3(1.0f, 1.0f, 1.0f)
		);
		void TransformLegR(
			glm::vec3 tras = glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3 rot = glm::vec3(0.0f, 0.0f, 1.0f),
			GLfloat angulo = 0.0f,
			glm::vec3 sca = glm::vec3(1.0f, 1.0f, 1.0f)
		);

		void MovFullModel(glm::vec3 mueve);

		~ModelJerarquia();

	private:
		std::string modelPath;
		std::vector<Model> ModelItmes;
		std::vector<glm::mat4> MatrixModels;
		std::string NameItems[6];
		glm::mat4 modelroot;
		glm::vec3 actualPos;

		
};