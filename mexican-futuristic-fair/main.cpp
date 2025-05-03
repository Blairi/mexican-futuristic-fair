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
#include "ThirdPersonCamera.h"
#include "ObjectFocusCamera.h"
#include "TopDownCamera.h"
#include "FreeCamera.h"

#include "Texture.h"
#include "Sphere.h"
#include "Model.h"
#include "ModelJerarquia.h"
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


Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;

Texture ProjectDefaultFont;

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

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(letrasVertices, letrasIndices, 32, 6);
	meshList.push_back(obj1);

	unsigned int LogoInvencibleIndices[] = {
		0, 1, 2,
		0, 2, 3,
	};
	GLfloat LogoIncencibleVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,
	};
	Mesh* logoInvencible = new Mesh();
	logoInvencible->CreateMesh(LogoIncencibleVertices, LogoInvencibleIndices, 32, 6);
	meshList.push_back(logoInvencible);

	unsigned int PantallaInvencibleIndices[] = {
		0, 1, 2,
		0, 2, 3,
	};
	GLfloat PantallaInvencibleVertices[] = {
		-1.0f, 0.0f, 1.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f, // esquina izq sup
		1.0f, 0.0f, 1.5f,		0.2f, 0.0f,		0.0f, -1.0f, 0.0f, // esquina der sup
		1.0f, 0.0f, -1.5f,		0.2f, 1.0f,		0.0f, -1.0f, 0.0f, // esquina der inf
		-1.0f, 0.0f, -1.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f, // esquina izq inf
	};

	Mesh* pantalla = new Mesh();
	pantalla->CreateMesh(PantallaInvencibleVertices, PantallaInvencibleIndices, 32, 6);
	meshList.push_back(pantalla);
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

void SetPinesBoliche(GLuint &uniformModel, std::vector <glm::mat4> &pinesTrans, glm::mat4 modelroot, Model &pinBoliche) {

	GLfloat spaceBetwen_z = 0.0f, spaceBetwen_x = 0.0f;
	glm::mat4 modelaux(1.0);
	pinesTrans.clear();

	for (int i = 0; i < 4; i++) {

		for (int j = i; j < 4; j++) {
			
			modelaux = modelroot;
			modelaux = glm::translate(modelroot, glm::vec3(spaceBetwen_x, 0.0f, spaceBetwen_z));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelaux));
			pinBoliche.RenderModel();

			pinesTrans.push_back(modelaux);
			spaceBetwen_z -= 0.25;
		}
		spaceBetwen_x += 0.25;
		spaceBetwen_z = -0.125f * (i + 1);
	}
}


int main()
{
	mainWindow = Window(1920, 1070); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	createInterface();
	CreateObjects();
	CreateShaders();

	glm::vec3 avatarPos(0.0f, 0.0f, 0.0f);
	// Ejemplo de un par de atracciones clave:
	glm::vec3 posCabina(-25.0f, 0.0f, 11.0f);
	glm::vec3 posBoliche(-3.68f, 0.0f, 28.85f);

	std::vector<glm::vec3*> objetos = {
		// Ejemplo: &posPuestoTortas, &posRevientaGlobos, ...
		 &posCabina, &posBoliche // (sólo placeholder)
	};

	// --- INSTANCIAR CÁMARAS ---
	FreeCamera freeCam(
		glm::vec3(0, 0, 0), glm::vec3(0, 1, 0),  // posición y up
		0.0f, 0.0f,                          // yaw, pitch
		0.1f, 0.5f                          // moveSpeed, turnSpeed
	);
	ThirdPersonCamera thirdCam(
		glm::vec3(0, 3, 8),                  // offset detrás del avatar
		glm::vec3(0, 1, 0),                  // up vector
		&avatarPos                         // target del avatar
	);
	ObjectFocusCamera objFocusCam(
		glm::vec3(0, 1, 0),                  // up
		objetos                            // lista de punteros a posiciones
	);
	TopDownCamera topDownCam(
		glm::vec3(0, 0, 0), 60.0f,            // centro y altura
		glm::vec3(0, 0, -1)                   // up del top-down
	);

	// Cámara activa
	Camera* activeCamera = &freeCam;




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

	Texture LogoInvencible = Texture("Textures/Invincible_comic_series_logo.png");
	LogoInvencible.LoadTextureA();
	Texture FramesInvencible = Texture("Textures/frames-invencible.png");
	FramesInvencible.LoadTextureA();

	Texture LogoBatman = Texture("Textures/BatmanBeyondLogo.png");
	LogoBatman.LoadTextureA();

	Texture LogoDany = Texture("Textures/DanyPhantomLogo.png");
	LogoDany.LoadTextureA();


	Base = Model();
	Base.LoadModel("Models/base.obj");

	/*
	* Puestos de comida
	*/
	Model TortasInvencible = Model();
	TortasInvencible.LoadModel("Models/PuestoTortasInvencible/puesto-tortas-invencible.obj");

	/*
	* Atracciones
	*/
	Model RevientaGlobosInvencible = Model();
	RevientaGlobosInvencible.LoadModel("Models/RevientaGlobos/invencible-revienta-globos.obj");


	Model RuedaFortunaInvencibleBase = Model();
	RuedaFortunaInvencibleBase.LoadModel("Models/RuedaFortuna/rueda-fortuna-base.obj");
	Model RuedaFortunaInvencibleWheel = Model();
	RuedaFortunaInvencibleWheel.LoadModel("Models/RuedaFortuna/rueda-fortuna-wheel.obj");
	Model RuedaFortunaInvencibleCabina = Model();
	RuedaFortunaInvencibleCabina.LoadModel("Models/RuedaFortuna/rueda-fortuna-cabina.obj");

	Model ProyectorInvencible = Model();
	ProyectorInvencible.LoadModel("Models/Proyector/screen_1.obj");

	// Boliche 
	Model SueloBoliche = Model();
	SueloBoliche.LoadModel("Models/AtraccionBoliche/sueloBoliche.obj");

	Model paredBoliche = Model();
	paredBoliche.LoadModel("Models/AtraccionBoliche/paredBoliche.obj");

	Model recibidorBoliche = Model();
	recibidorBoliche.LoadModel("Models/AtraccionBoliche/recibidorBoliche.obj");

	Model pinBoliche = Model();
	pinBoliche.LoadModel("Models/AtraccionBoliche/pinBoliche.obj");
	std::vector <glm::mat4> pinesTrans;

	Model LineasBoliche = Model();
	LineasBoliche.LoadModel("Models/AtraccionBoliche/lineasBoliche.obj");

	Model MaquinaBolasBolicheRoja = Model();
	MaquinaBolasBolicheRoja.LoadModel("Models/AtraccionBoliche/maquinaBolasRoja.obj");

	Model MaquinaBolasBolicheAzul = Model();
	MaquinaBolasBolicheAzul.LoadModel("Models/AtraccionBoliche/maquinaBolasAzul.obj");

	Model BolaVerde = Model();
	BolaVerde.LoadModel("Models/AtraccionBoliche/BolaVerde.obj");

	Model BolaRoja = Model();
	BolaRoja.LoadModel("Models/AtraccionBoliche/BolaRoja.obj");

	Model BolaAzul = Model();
	BolaAzul.LoadModel("Models/AtraccionBoliche/BolaAzul.obj");

	/*
	* Escenario de Musica
	*/
	Model Stage = Model();
	Stage.LoadModel("Models/StageEmber/concertStage.obj");

	Model BocinasStageEmber = Model();
	BocinasStageEmber.LoadModel("Models/StageEmber/BocinasEmber.obj");

	/*
	* Zona Invencible
	*/
	Model ShowSuit = Model();
	ShowSuit.LoadModel("Models/ZonaInvencible/show-suit.obj");
	Model KidOmniMan_M = Model();
	KidOmniMan_M.LoadModel("Models/ZonaInvencible/KidOmniman.obj");
	Model Duplikate_M = Model();
	Duplikate_M.LoadModel("Models/ZonaInvencible/Duplikate.obj");
	Model Powerplex_M = Model();
	Powerplex_M.LoadModel("Models/ZonaInvencible/Powerplex.obj");
	Model Cecil_M = Model();
	Cecil_M.LoadModel("Models/ZonaInvencible/cecil-model.obj");
	Model Conquest_M = Model();
	Conquest_M.LoadModel("Models/ZonaInvencible/Conquest.obj");

	Model Batimovil = Model();
	Batimovil.LoadModel("Models/Batimovil.obj");

	//------------------ ATRACCION DADOS ------------------------

	//CARPA

	Model carpa = Model();
	carpa.LoadModel("Models/carpa.obj");

	/*
	* Avatares
	*/
	ModelJerarquia DannyPhantom_M = ModelJerarquia("Models/DannyPhantom");
	DannyPhantom_M.InitModel(glm::vec3(0.0f, 0.5f, 0.0f));
	ModelJerarquia Invencible_M = ModelJerarquia("Models/Invencible");
	Invencible_M.InitModel(glm::vec3(0.0f, 0.0f, 0.0f));
	ModelJerarquia Batman_M = ModelJerarquia("Models/BatmanBeyond");
	Batman_M.InitModel(glm::vec3(0.0f, -0.75f, 0.0f));

	/*-------
	* NPC
	* Agregar modelos de los NPC aqui
	* ------
	*/
	ModelJerarquia AtomEve_M = ModelJerarquia("Models/AtomEve");
	AtomEve_M.InitModel(glm::vec3(0.0f, 0.0f, 0.0f));
	ModelJerarquia OmniMan_M = ModelJerarquia("Models/OmniMan");
	OmniMan_M.InitModel(glm::vec3(0.0f, 0.0f, 0.0f));

	Model Ember_M = Model();
	Ember_M.LoadModel("Models/Ember/Ember.obj");

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

	float offsetFrames = 0.0f;

	std::map<char, std::pair<float, float>> letrasOffset = {
		{ 'A', std::make_pair(0.0f, 0.8164f) },
		{ 'B', std::make_pair(0.125f, 0.8164f) },
		{ 'C', std::make_pair(0.2324f, 0.8164f) },
		{ 'D', std::make_pair(0.34375f, 0.8164f) },
		{ 'E', std::make_pair(0.4589f, 0.8164f) },
		{ 'F', std::make_pair(0.5609f, 0.8164f) },
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

	/*
	* variables auxiliares para las animaciones
	*/
	float girarRueda = 0.0f;
	float animaAtomGlobos = 0.0f;
	float animarZonaTrajes = 0.0f;
	GLfloat lastTimeProy = 0.0f;


	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();

		// — Cambiar cámara con teclas 1–4 —
		if (mainWindow.getsKeys()[GLFW_KEY_1]) activeCamera = &freeCam;
		if (mainWindow.getsKeys()[GLFW_KEY_2]) activeCamera = &thirdCam;
		static bool prevKey3 = false;
		bool currKey3 = mainWindow.getsKeys()[GLFW_KEY_3];

		// Si acabas de pulsar (borde de subida)
		if (currKey3 && !prevKey3) {
			// 1) cambiamos la cámara activa a ObjectFocusCamera
			activeCamera = &objFocusCam;
			// 2) avanzamos al siguiente objeto
			objFocusCam.nextObject();
		}
		prevKey3 = currKey3;

		if (mainWindow.getsKeys()[GLFW_KEY_4]) activeCamera = &topDownCam;

		// Actualiza la cámara elegida
		activeCamera->update(deltaTime);

		// Sólo la freeCam y thirdCam usan WASD+mouse:
		if (activeCamera == &freeCam || activeCamera == &thirdCam) {
			activeCamera->keyControl(mainWindow.getsKeys(), deltaTime);
			activeCamera->mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
		}

		glm::mat4 viewMatrix = activeCamera->getViewMatrix();



		// controlar la velocidad con la que rota la camara de seleccion de personaje
		rotacionCamara += 0.005f * deltaTime; // TODO: evitar desbordamiento de int

		float x = 30.0f * cos(rotacionCamara);
		float z = 30.0f * sin(rotacionCamara);

		if (mainWindow.isPersonajeSeleccionado()) {
			activeCamera->keyControl(mainWindow.getsKeys(), deltaTime * 5);
			activeCamera->mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
		}
		else {
			// en la fase de selección, forzamos la cámara libre:
			freeCam.setPosition(glm::vec3(x, 7.0f, z));
			freeCam.setDirection(glm::vec3(-x, 0, -z));
		}

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(viewMatrix, projection);
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

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));

		// Para la linterna o efectos que necesitan posición:
		glm::vec3 camPos = activeCamera->getCameraPosition();
		glm::vec3 camDir = activeCamera->getCameraDirection();
		glUniform3f(uniformEyePosition, camPos.x, camPos.y, camPos.z);
		spotLights[0].SetFlash(camPos, camDir);



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
		glm::mat4 modelrootBoliche(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec2 toffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));

		model = glm::mat4(1.0);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Base.RenderModel();

		/*
		* ------------------
		* Ambientacion 
		* Aqui renderizar todos los modelos decorativos
		* ------------------
		*/

		// puesto de tortas invencible
		glm::vec3 posPuestoTortas(-7.366f, 0.0f, -16.264f);
		model = glm::mat4(1.0);
		model = glm::translate(model, posPuestoTortas);
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
		model = glm::rotate(model, glm::radians(22.004f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		TortasInvencible.RenderModel();


		//Batimovil
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-15.0f, 0.0f, -6.494f));
		//model = glm::rotate(model, glm::radians(22.004f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Batimovil.RenderModel();

		/*
		* Feria de la fortuna
		*/
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-0.876f, 0.0f, 18.489f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		RuedaFortunaInvencibleBase.RenderModel();

		girarRueda += 0.5 * deltaTime; // TODO: evitar desbordamiento 

		model = glm::translate(model, glm::vec3(0.0f, 7.187f, 0.0f));
		model = glm::rotate(model, glm::radians(girarRueda), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		RuedaFortunaInvencibleWheel.RenderModel();

		std::vector<std::pair<float, float>> posCabinas = {
			//				x,      y
			std::make_pair(-5.782f, 0.0f),
			std::make_pair(-4.8f, 3.478f),
			std::make_pair(-2.527, 5.258f),
			std::make_pair(0.0f, 5.656f),
			std::make_pair(2.546f, 5.258f),
			std::make_pair(4.63f, 3.478f),
			std::make_pair(5.815f, 0.0f),
			std::make_pair(4.7f, -3.317f),
			std::make_pair(2.553f, -5.169f),
			std::make_pair(0.0f, -5.792f),
			std::make_pair(-2.521f, -5.169f),
			std::make_pair(-4.701f, -3.317f),
		};

		for (int i = 0; i < posCabinas.size(); i++) {
			model = modelaux;
			model = glm::translate(model, glm::vec3(posCabinas[i].first, posCabinas[i].second, 0.0f));
			model = glm::rotate(model, glm::radians(-girarRueda), glm::vec3(0.0f, 0.0f, 1.0f));
			if(i % 2 == 0)
				color = glm::vec3(0.14509f, 0.58823f, 0.74509f); // azul invencible
			else
				color = glm::vec3(0.9215f, 0.3647f, 0.4823f); // rosa atom eve
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			RuedaFortunaInvencibleCabina.RenderModel();
		}
		// reiniciar color blanco
		glUniform3fv(uniformColor, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));

		/*
		* Zona Invencible
		* Se renderizan modelos de personajes con un showcase
		*/
		float posShowCases[5][4] = {
			// x,        y,    z,     rotacion
			{19.501f, 0.0f, -13.491f, -19.852f},
			{21.367f, 0.0f, -12.757f, -29.925f},
			{22.677f, 0.0f, -11.303f, -63.702f},
			{23.371f, 0.0f, -9.75f, -90.554f},
			{23.356f, 0.0f, -7.766f, -95.562f},
		};

		Model personajes[5] = {
			Cecil_M, Conquest_M, Duplikate_M, KidOmniMan_M, Powerplex_M,
		};

		animarZonaTrajes += 0.5 * deltaTime; //TODO: evitar desbordamiento de float
		for (int i = 0; i < 5; i++) {
			// renderiza showcase
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(posShowCases[i][0], 0.0f, posShowCases[i][2]));
			model = glm::rotate(model, glm::radians(posShowCases[i][3]), glm::vec3(0.0f, 1.0f, 0.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			ShowSuit.RenderModel();
			// renderiza personajes dentro del showcase
			model = glm::translate(model, glm::vec3(0.0f, 0.12f, 0.0f));
			if(i == 1) // escalar Conquest
				model = glm::scale(model, glm::vec3(0.8f));
			model = glm::scale(model, glm::vec3(1.0f) * (1.0f + 0.05f * sin(animarZonaTrajes*0.1f)));
			model = glm::rotate(model, glm::radians(animarZonaTrajes), glm::vec3(0.0f, 1.0f, 0.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			personajes[i].RenderModel();
		}

		/*
		* Pantalla
		*/
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(27.441f, 0.0f, -13.418f));
		model = glm::rotate(model, glm::radians(-44.398f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.1f, 0.1f, 0.1f); // negro
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		ProyectorInvencible.RenderModel();

		// animacion para cambiar entre frames
		now = glfwGetTime();
		if (now - lastTimeProy >= 3.0f) {
			offsetFrames += 0.2f;
			if (offsetFrames >= 1.0f)
				offsetFrames = 0.0f;
			lastTimeProy = now;
		}
		model = glm::translate(model, glm::vec3(0.0f, 5.5f, 1.3f));
		model = glm::scale(model, glm::vec3(8.0f, 2.0f, 2.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(glm::vec2(offsetFrames, 0.0f)));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		FramesInvencible.UseTexture();
		meshList[2]->RenderMesh();

		// reiniciar offset de texturas
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(glm::vec2(0.0f)));
		/*
		* Escenario Ember
		* Se renderiza Escenario, Bocinas.
		*/
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f,0.0f,-30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Stage.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BocinasStageEmber.RenderModel();


		/*
		* ------------------
		* Atracciones
		* Aqui renderizar todos los modelos de atracciones
		* ------------------
		*/

		


		/*
		* Revienta Globos - A5
		*/
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-25.0f, 0.0f, 11.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		RevientaGlobosInvencible.RenderModel();

		/*
		* Boliche - A2
		*/
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-3.68f,0.0f, 28.854f));
		modelrootBoliche = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LineasBoliche.RenderModel();

		model = glm::translate(model, glm::vec3(8.5f, 0.0f, -0.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		SueloBoliche.RenderModel();

		model = modelrootBoliche;
		model = glm::translate(model, glm::vec3(8.5f, 0.0f, -0.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		paredBoliche.RenderModel();

		// Recepcion
		model = modelrootBoliche;
		model = glm::translate(model, glm::vec3(17.0, 0.0f, 1.8f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		recibidorBoliche.RenderModel();

		// Maquinas de Bolas
		model = modelrootBoliche;
		model = glm::translate(model, glm::vec3(11.8745f, 0.0f, -2.0));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		MaquinaBolasBolicheRoja.RenderModel();

		// Bola Verde
		model = glm::translate(model, glm::vec3(0.65f, 0.494f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BolaVerde.RenderModel();

		// Bola Roja
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.4f, 0.494f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BolaRoja.RenderModel();

		// Bola Azul
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.494f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BolaAzul.RenderModel();

		// ------------

		model = modelrootBoliche;
		model = glm::translate(model, glm::vec3(11.8745f, 0.0f, -0.45f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		MaquinaBolasBolicheAzul.RenderModel();

		// Bola Azul
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.65f, 0.494f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BolaAzul.RenderModel();

		// Bola Roja
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.4f, 0.494f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BolaRoja.RenderModel();

		// Bola Verde
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.494f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BolaVerde.RenderModel();

		// ------------

		model = modelrootBoliche;
		model = glm::translate(model, glm::vec3(11.8745f, 0.0f, 1.2f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		MaquinaBolasBolicheRoja.RenderModel();

		// Bola Roja
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.65f, 0.494f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BolaRoja.RenderModel();

		// Bola Verde
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.4f, 0.494f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BolaVerde.RenderModel();

		// Bola Azul
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.494f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BolaAzul.RenderModel();

		/*
		* Pines de Boliche por Linea
		*/

		// Linea 1
		model = modelrootBoliche;
		model = glm::translate(model, glm::vec3(-5.5f, 0.0f, -1.0f));
		SetPinesBoliche(uniformModel, pinesTrans, model, pinBoliche);

		// Linea 3
		model = modelrootBoliche;
		model = glm::translate(model, glm::vec3(-5.5f, 0.0f, 2.3f));
		SetPinesBoliche(uniformModel, pinesTrans, model, pinBoliche);

		// Linea 2
		model = modelrootBoliche;
		model = glm::translate(model, glm::vec3(-5.5f, 0.0f, 0.7f));
		SetPinesBoliche(uniformModel, pinesTrans, model, pinBoliche);

		// -----------------------


		/*
		* Fin Boliche - A2
		*/



		/*
		* ------------------
		* NPC de atracciones
		* Aqui renderizar todos los NPC de atracciones
		* ------------------
		*/
		// Atom eve - revienta globos
		animaAtomGlobos += 0.1f * deltaTime;
		AtomEve_M.MovFullModel(glm::vec3(-25.7f, 0.95f, 10.5f), 
			glm::vec3(0.0, 1.0f, 0.0f), 135.753f);
		AtomEve_M.TransformHead(glm::vec3(0.0f, 0.567f, -0.015f), 
			glm::vec3(1.0f, 0.0f, 0.0f), 5*sin(animaAtomGlobos));
		AtomEve_M.TransformLegR(glm::vec3(-0.065f, 0.03f, -0.008f));
		AtomEve_M.TransformLegL(glm::vec3(0.015f, 0.01f, -0.003f));
		AtomEve_M.TransformArmR(glm::vec3(-0.103f, 0.462f, -0.018f));
		AtomEve_M.TransformArmL(glm::vec3(0.108f, 0.444f, -0.039f),
			glm::vec3(1.0f, 0.0f, 0.0f), 50 * cos(animaAtomGlobos));
		AtomEve_M.RenderModelJ(uniformModel);

		// Ember del Escenario
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Ember_M.RenderModel();

		/*
		* ------------------
		* NPC de ambiente
		* Aqui renderizar todos los NPC de ambiente
		* ------------------
		*/

		// omniman - tortas invencible
		OmniMan_M.MovFullModel(posPuestoTortas + glm::vec3(-0.6f, 0.9f, -0.6f),
			glm::vec3(0.0f, 1.0f, 0.0f), 32.953f);
		OmniMan_M.TransformHead(glm::vec3(0.0f, 0.683f, -0.015f));
		OmniMan_M.TransformLegR(glm::vec3(-0.081f, 0.033f, 0.017f));
		OmniMan_M.TransformLegL(glm::vec3(0.075f, 0.036f, -0.003f));
		OmniMan_M.TransformArmR(glm::vec3(-0.163f, 0.549f, -0.037f));
		OmniMan_M.TransformArmL(glm::vec3(0.177f, 0.551f, -0.034f));
		OmniMan_M.RenderModelJ(uniformModel);

		// variables auxiliares para la interfaz de selección de personaje
		glm::vec3 camFront = glm::normalize(freeCam.getCameraDirection());
		glm::mat4 orientacion = glm::inverse(glm::lookAt(glm::vec3(0.0f), camFront, glm::vec3(0.0f, 1.0f, 0.0f)));

		/*
		* TODO: abstraer a una clase para no usar tantos IFs
		*/
		int idPersonaje = mainWindow.getIdPersonaje();
		if (!mainWindow.isPersonajeSeleccionado() && idPersonaje == 0) {
			glm::vec3 modeloPos = camPos + camFront * 5.0f; // distancia a la camara
			DannyPhantom_M.TranformFullModel(glm::vec3(modeloPos), orientacion);
			DannyPhantom_M.TransformHead(glm::vec3(0.0f, 0.21f, 0.0f));
			DannyPhantom_M.TransformLegR(glm::vec3(-0.04f, -0.2f, 0.0f));
			DannyPhantom_M.TransformLegL(glm::vec3(0.035f, -0.2f, 0.0f));
			DannyPhantom_M.TransformArmR(glm::vec3(-0.1445f, 0.123f, 0.0f));
			DannyPhantom_M.TransformArmL(glm::vec3(0.13f, 0.12f, 0.0f));
			DannyPhantom_M.RenderModelJ(uniformModel);
		}
		else if (!mainWindow.isPersonajeSeleccionado() && idPersonaje == 1) {
			glm::vec3 modeloPos = camPos + camFront * 3.0f; // distancia a la camara
			Invencible_M.TranformFullModel(glm::vec3(modeloPos), orientacion);
			Invencible_M.TransformHead(glm::vec3(0.005f, 0.548f, -0.011f));
			Invencible_M.TransformLegR(glm::vec3(-0.011f, 0.061f, 0.002f));
			Invencible_M.TransformLegL(glm::vec3(0.007f, 0.071f, 0.005f));
			Invencible_M.TransformArmR(glm::vec3(-0.121f, 0.393f, -0.023f));
			Invencible_M.TransformArmL(glm::vec3(0.13f, 0.397f, -0.028f));
			Invencible_M.RenderModelJ(uniformModel);
		}
		
		else if (!mainWindow.isPersonajeSeleccionado() && idPersonaje == 2) {
			glm::vec3 modeloPos = camPos + camFront * 4.0f; // distancia a la camara
			Batman_M.TranformFullModel(glm::vec3(modeloPos), orientacion);
			Batman_M.TransformHead(glm::vec3(0.0f, 0.0f, 0.0f));
			Batman_M.TransformLegR(glm::vec3(-0.011f, 0.061f, 0.002f));
			Batman_M.TransformLegL(glm::vec3(0.007f, 0.071f, 0.005f));
			Batman_M.TransformArmR(glm::vec3(-0.010f, 0.025f, -0.023f));
			Batman_M.TransformArmL(glm::vec3(0.010f, 0.025f, -0.028f));
			Batman_M.RenderModelJ(uniformModel);
		}
		// TODO: BMO


		// batman flotando
		/*
		Batman_M.MovFullModel(glm::vec3(0.0f, 0.0f,0.0f));
		Batman_M.TransformHead(glm::vec3(0.005f, 0.548f, -0.011f));
		Batman_M.TransformLegR(glm::vec3(-0.011f, 0.061f, 0.002f));
		Batman_M.TransformLegL(glm::vec3(0.007f, 0.071f, 0.005f));
		Batman_M.TransformArmR(glm::vec3(-0.121f, 0.393f, -0.023f));
		Batman_M.TransformArmL(glm::vec3(0.13f, 0.397f, -0.028f));
		Batman_M.RenderModelJ(uniformModel);*/

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// desaparecer interfaz si el usuario ya selecciono un avatar
		if (mainWindow.isPersonajeSeleccionado()) {
			glDisable(GL_BLEND);
			glUseProgram(0);
			mainWindow.swapBuffers();
			continue;
		}

		/*
		* Interfaz ligada a la camara
		*/

		// Obtener vectores de la camara
		glm::vec3 camRight = glm::normalize(glm::cross(camFront, glm::vec3(0.0f, 1.0f, 0.0f)));
		glm::vec3 camUp = glm::normalize(glm::cross(camRight, camFront));

		// Posicion frente a la camara (a 10 unidades)
		glm::vec3 basePos = camPos + camFront * 10.0f;

		// renderizar encabezados
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

		/*
		* Renderizar logo/letrero de personajes
		*/

		if (idPersonaje == 0) {
			glm::vec3 offsetPos = basePos + camRight * 0.0f + camUp * -2.5f;
			model = glm::mat4(1.0f);
			model = glm::translate(model, offsetPos);
			model *= glm::inverse(glm::lookAt(glm::vec3(0.0f), camFront, glm::vec3(0.0f, 1.0f, 0.0f)));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(10.0f, 1.0f, 3.0f));
			glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			LogoDany.UseTexture();
			meshList[1]->RenderMesh();
		}

		if (idPersonaje == 1) {
			glm::vec3 offsetPos = basePos + camRight * 0.0f + camUp * -2.5f;
			model = glm::mat4(1.0f);
			model = glm::translate(model, offsetPos);
			model *= glm::inverse(glm::lookAt(glm::vec3(0.0f), camFront, glm::vec3(0.0f, 1.0f, 0.0f)));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(10.0f, 1.0f, 1.0f));
			glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			LogoInvencible.UseTexture();
			meshList[1]->RenderMesh();
		}

		if (idPersonaje == 2) {
			glm::vec3 offsetPos = basePos + camRight * 0.0f + camUp * -2.5f;
			model = glm::mat4(1.0f);
			model = glm::translate(model, offsetPos);
			model *= glm::inverse(glm::lookAt(glm::vec3(0.0f), camFront, glm::vec3(0.0f, 1.0f, 0.0f)));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(8.0f, 1.0f, 2.5f));
			glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			color = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			LogoBatman.UseTexture();
			meshList[1]->RenderMesh();
		}
		// TODO: renderizar condicionalmente los demas logos

		glDisable(GL_BLEND);
		glUseProgram(0);
		mainWindow.swapBuffers();
	}

	return 0;
}
