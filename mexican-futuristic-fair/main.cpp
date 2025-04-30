/*
Proyecto Final Computacion Grafica
Autores:
	- 
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>
#include <map>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;

Texture ProjectDefaultFont;

Model AtomEve_torso;
Model AtomEve_cabeza;
Model AtomEve_brazo_der;
Model AtomEve_brazo_izq;
Model AtomEve_pie_der;
Model AtomEve_pie_izq;

Model Base;

Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;


GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";


//función de calculo de normales por promedio de vértices 
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}


void createInterface()
{
	unsigned int letrasIndices[] = {
		0, 1, 2,
		0, 2, 3,
	};

	GLfloat letrasVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.8164f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		0.1230f, 0.8164f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		0.1230f, 0.9707f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 0.9707f,		0.0f, -1.0f, 0.0f,
	};

	Mesh* obj5 = new Mesh();
	obj5->CreateMesh(letrasVertices, letrasIndices, 32, 6);
	meshList.push_back(obj5);
}

void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,
	};
	
	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}


int main()
{
	mainWindow = Window(1920, 1070); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	createInterface();
	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.0f, 0.0f, 0.3f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	AgaveTexture = Texture("Textures/Agave.tga");
	AgaveTexture.LoadTextureA();

	ProjectDefaultFont = Texture("Textures/fuente-proy.png");
	ProjectDefaultFont.LoadTextureA();

	Base = Model();
	Base.LoadModel("Models/base.obj");

	AtomEve_torso = Model();
	AtomEve_torso.LoadModel("Models/AtomEve/Body.obj");
	AtomEve_cabeza = Model();
	AtomEve_cabeza.LoadModel("Models/AtomEve/Head.obj");
	AtomEve_brazo_der = Model();
	AtomEve_brazo_der.LoadModel("Models/AtomEve/ArmR.obj");
	AtomEve_brazo_izq = Model();
	AtomEve_brazo_izq.LoadModel("Models/AtomEve/ArmL.obj");
	AtomEve_pie_der = Model();
	AtomEve_pie_der.LoadModel("Models/AtomEve/LegR.obj");
	AtomEve_pie_izq = Model();
	AtomEve_pie_izq.LoadModel("Models/AtomEve/LegL.obj");

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f, // intensidad ambiental (radiacion de la luz), intensidad difusa
		0.0f, -1.0f, 0.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaración de primer luz puntual
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		-6.0f, 1.5f, 1.5f, // pos
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	/*
	* Faros
	*/

	//luz fija
	spotLights[1] = SpotLight(0.0f, 0.0f, 0.0f,
		1.0f, 2.0f,
		-5.0f, 1.0f, -3.0f,
		-1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;

	spotLights[2] = SpotLight(0.0f, 0.0f, 1.0f,
		1.0f, 2.0f,
		-5.0f, 1.0f, 3.0f,
		-1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;
	
	//se crean mas luces puntuales y spotlight 

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset = 0;;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	////Loop mientras no se cierra la ventana
	int idPersonaje = 0;
	float rotacionCamara = 0.0f;
	float offsetFuenteProyU = 0.0f;
	//float offsetFuenteProyV = 0.8164f;
	float offsetFuenteProyV = 0.8164f;

	std::map<char, std::pair<float, float>> letrasOffset = {
		{ 'A', std::make_pair(0.0f, 0.8164f) },
		{ 'B', std::make_pair(0.125f, 0.8164f) },
		{ 'C', std::make_pair(0.2324f, 0.8164f) },
		{ 'D', std::make_pair(0.34375f, 0.8164f) },
		{ 'E', std::make_pair(0.4589f, 0.8164f) },
		{ 'F', std::make_pair(0.5550f, 0.8164f) },
		{ 'G', std::make_pair(0.6757f, 0.8164f) },
		{ 'H', std::make_pair(0.7890f, 0.8164f) },

		{ 'I', std::make_pair(0.0136f, 0.6015f) },
		{ 'J', std::make_pair(0.1191f, 0.6015f) },
		{ 'K', std::make_pair(0.2324f, 0.6015f) },
		{ 'L', std::make_pair(0.3515f, 0.6015f) },
		{ 'M', std::make_pair(0.4511f, 0.6015f) },
		{ 'N', std::make_pair(0.5664f, 0.6015f) },
		{ 'O', std::make_pair(0.7871f, 0.6015f) },

		{ 'P', std::make_pair(0.0175f, 0.3847f) },
		{ 'Q', std::make_pair(0.1210f, 0.3847f) },
		{ 'R', std::make_pair(0.2343f, 0.3847f) },
		{ 'S', std::make_pair(0.3457f, 0.3847f) },
		{ 'T', std::make_pair(0.4492f, 0.3847f) },
		{ 'U', std::make_pair(0.5703f, 0.3847f) },
		{ 'V', std::make_pair(0.6718f, 0.3847f) },
		{ 'W', std::make_pair(0.7832f, 0.3847f) },

		{ 'X', std::make_pair(0.0f, 0.1679f) },
		{ 'Y', std::make_pair(0.1171f, 0.1679f) },
		{ 'Z', std::make_pair(0.2382f, 0.1679f) },
	};

	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();

		rotacionCamara += 0.005f * deltaTime;

		if (mainWindow.isPersonajeSeleccionado()) {
			camera.keyControl(mainWindow.getsKeys(), deltaTime);
			camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
		}
		else {
			float x = 100 * cos(rotacionCamara);
			float z = 100 * sin(rotacionCamara);
			camera.setPosition(glm::vec3(x, 7.0f, z));
			camera.setDirection(glm::vec3(-x, 0.0f, -z));
		}

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		uniformTextureOffset = shaderList[0].getOffsetLocation();
		
		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, 
			camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		//sirve para que en tiempo de ejecución (dentro del while) se cambien propiedades de la luz
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		/*
		* eleccion de personaje
		*/
		if (!mainWindow.isPersonajeSeleccionado()) {
			idPersonaje = mainWindow.getIdPersonaje();
		}

		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec2 toffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Base.RenderModel();

		// variables auxiliares para la interfaz de selección de personaje
		glm::vec3 camPos = camera.getCameraPosition();
		glm::vec3 camFront = glm::normalize(camera.getCameraDirection());
		glm::vec3 modeloPos = camPos + camFront * 20.0f;

		model = glm::mat4(1.0f);
		model = glm::translate(model, modeloPos);
		model *= glm::inverse(glm::lookAt(glm::vec3(0.0f), camFront, glm::vec3(0.0f, 1.0f, 0.0f)));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AtomEve_torso.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 4.642f, -0.04f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AtomEve_cabeza.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.86f, 3.764f, -0.172f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AtomEve_brazo_der.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.899f, 3.647f, -0.304f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AtomEve_brazo_izq.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.594f, 0.26f, -0.052f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AtomEve_pie_der.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.135f, 0.016f, 0.001f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AtomEve_pie_izq.RenderModel();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		/*
		* Interfaz
		*/
		

		// Obtener vectores de la cámara
		glm::vec3 camRight = glm::normalize(glm::cross(camFront, glm::vec3(0.0f, 1.0f, 0.0f)));
		glm::vec3 camUp = glm::normalize(glm::cross(camRight, camFront));

		// Posicion frente a la camara (a 10 unidades)
		glm::vec3 basePos = camPos + camFront * 10.0f;

		std::string texto = "FERIA FUTURISTA";

		for (int i = 0; i < texto.size(); i++) {
			if (texto[i] == ' ') continue;

			float espaciado = 1.0f;
			glm::vec3 offsetPos = basePos + camRight * (-7.0f + espaciado * i) + camUp * 4.5f;

			// Configurar textura de letras
			toffset = glm::vec2(offsetFuenteProyU + letrasOffset[texto[i]].first, 
				(1 - offsetFuenteProyV) + letrasOffset[texto[i]].second);

			model = glm::mat4(1.0f);
			model = glm::translate(model, offsetPos);
			model *= glm::inverse(glm::lookAt(glm::vec3(0.0f), camFront, glm::vec3(0.0f, 1.0f, 0.0f)));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

			glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			ProjectDefaultFont.UseTexture();
			meshList[0]->RenderMesh();
		}

		glDisable(GL_BLEND);
		glUseProgram(0);
		mainWindow.swapBuffers();
	}

	return 0;
}
