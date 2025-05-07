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

#include "MeshBuilder.h"

// main.cpp (era después de los includes)
struct AppPointers {
	Window* win;
	ThirdPersonCamera* cam3p;
};

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

const float DISTANCIA_PARA_ACTIVAR = 5.0F;

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";

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

void swapSpotLight(SpotLight* spotLights, std::vector<int> ligths, int numLights) {

	SpotLight itemAux;

	for (int i = 0; i < numLights; i++) {
		itemAux = spotLights[i];
		spotLights[i] = spotLights[ligths[i]];
		spotLights[ligths[i]] = itemAux;
	}

}

int main()
{
	mainWindow = Window(1920, 1070); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	/*
	* mesh[i]->renderMesh()
	* 0 -> letras
	* 1 -> logo invencible
	* 2 -> pantalla proyector
	*/
	MeshBuilder meshBuilder = MeshBuilder();

	CreateShaders();

	// Posición Postes de Luz:
	bool isDay = false;
	std::vector <glm::vec3> posPostesLuz;
	posPostesLuz.push_back(glm::vec3(8.072f, 5.03f, 15.362f));
	posPostesLuz.push_back(glm::vec3(12.429f, 5.03f, -8.4947f));
	posPostesLuz.push_back(glm::vec3(-16.142f, 5.03f, -1.0662f));
	posPostesLuz.push_back(glm::vec3(-19.785f, 5.03f, -25.709f));

	glm::vec3 avatarPos(0.0f, 0.0f, 0.0f);

	// banderas para interactuar con las atracciones, cada indice es la atraccion correspondiente
	glm::vec3 posLanzaHacha(13.4928f, 1.445f, 18.9578f); // 0 - A1
	glm::vec3 posBoliche(-3.68f, 0.0f, 28.85f); // 1 - A2
	glm::vec3 posLanzaDados(-30.304f, 0.482451f, 0.799495f); // 2 - A3
	glm::vec3 posJaulaBateo(22.7181f, 1.24362f, 21.3295f); // 3 - A4
	glm::vec3 posRevientaGlobos(-25.0f, 0.0f, 11.0f); // 4 - A5
	glm::vec3 posmaquinaWhack(-22.0786f, 0.0f, -7.53293f); // 5 - A6
	bool activarAtraccionAnimacion[6] = { false, false, false, false, false, false };
	glm::vec3 posicionAtracciones[6] = { posLanzaHacha, posBoliche, posLanzaDados,
		posJaulaBateo, posRevientaGlobos, posmaquinaWhack, };

	// Ejemplo de un par de atracciones clave:
	glm::vec3 posCabina(-25.0f, 0.0f, 11.0f);

	// Posición de Luces Escenario
	glm::vec3 posLuz1(0.34811f, 4.82231f, -30.285066f);
	glm::vec3 posLuz2(-1.73563f, 4.82231f, -30.285066f);
	glm::vec3 posLuz3(2.40227f, 4.82231f, -30.285066f);

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
		glm::vec3(0, 5, 16),    // offset detrás del avatar
		glm::vec3(0, 1, 0),               // up vector
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

	auto* ptrs = new AppPointers{ &mainWindow, &thirdCam };
	GLFWwindow* glfwWin = mainWindow.getGLFWwindow();
	glfwSetWindowUserPointer(glfwWin, ptrs);           // <— ¡una sola vez!

	// --------------------------------------------------------------
	//                 CALLBACK DEL SCROLL (zoom)
	// --------------------------------------------------------------
	glfwSetScrollCallback(glfwWin,
		[](GLFWwindow* w, double /*x*/, double y)
		{
			auto* p = static_cast<AppPointers*>(glfwGetWindowUserPointer(w));
	if (p && p->cam3p)
		p->cam3p->addDistance(-static_cast<float>(y)); // ↑ acerca, ↓ aleja
		});

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

	Model SkayBoxDia = Model();
	SkayBoxDia.LoadModel("Models/SkyBoxNubesDia.obj");

	Base = Model();
	Base.LoadModel("Models/BaseTexturizada/base.obj");
	
	//Model Arbol

	Model arbol = Model();
	arbol.LoadModel("Models/HoraAventura/casaArbol.obj");

	/*
	* Puestos de comida
	*/
	Model TortasInvencible = Model();
	TortasInvencible.LoadModel("Models/PuestoTortasInvencible/puesto-tortas-invencible.obj");

	
	/*Model PuestoElotes = Model();
	PuestoElotes.LoadModel("Models/PuestoBatielotes/PuestoElotes.obj");*/

	/*
	* Ambientación 
	*/

	// Bancas
	Model Banca = Model();
	Banca.LoadModel("Models/Ambientacion/Banca.obj");

	Model BancaTecho = Model();
	BancaTecho.LoadModel("Models/Ambientacion/bancaTecho.obj");

	// Postes de Luz
	Model PosteLampara = Model();
	PosteLampara.LoadModel("Models/Ambientacion/posteLampara.obj");
	Model Lampara = Model();
	Lampara.LoadModel("Models/Ambientacion/lampara.obj");

	//Batimovil
	/*Model batimovil = Model();
	batimovil.LoadModel("Models/Ambientacion/batimovil.obj");*/

	/*
	* Atracciones
	*/
	Model RevientaGlobosInvencible = Model();
	RevientaGlobosInvencible.LoadModel("Models/RevientaGlobos/invencible-revienta-globos.obj");
	Model Globo = Model();
	Globo.LoadModel("Models/RevientaGlobos/globo.obj");

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

	//------------------ ATRACCION DADOS ------------------------

	//CARPA

	Model carpa = Model();
	carpa.LoadModel("Models/AtraccionDados/carpa.obj");

	//Mesa dados

	Model mesa_dados = Model();
	mesa_dados.LoadModel("Models/AtraccionDados/mesa_dados.obj");

	//dado
	Model dado = Model();
	dado.LoadModel("Models/AtraccionDados/dado.obj");

	//------------------ ATRACCION Whack A Mole ------------------------

	Model maquinaWhack = Model();
	maquinaWhack.LoadModel("Models/AtraccionWhackAMole/WhackAMole.obj");

	Model topo = Model();
	topo.LoadModel("Models/AtraccionWhackAMole/topo.obj");

	Model martillo_topos = Model();
	martillo_topos.LoadModel("Models/AtraccionWhackAMole/martillo_topos.obj");

	//------------------ ATRACCION ZONA BATEO ------------------------

	Model zonabateo = Model();
	zonabateo.LoadModel("Models/AtraccionBateo/zonabateo.obj");

	Model maquinaTenis = Model();
	maquinaTenis.LoadModel("Models/AtraccionBateo/maquinaTenis.obj");

	Model pelotaTenis = Model();
	pelotaTenis.LoadModel("Models/AtraccionBateo/pelotaTenis.obj");

	Model bat = Model();
	bat.LoadModel("Models/AtraccionBateo/bat.obj");

	//------------------ ATRACCION LANZAMIENTO DE HACHA ------------------------

	Model LanzamientoDeHacha = Model();
	LanzamientoDeHacha.LoadModel("Models/AtraccionHacha/LanzamientoDeHacha.obj");

	Model batgarang = Model();
	batgarang.LoadModel("Models/AtraccionHacha/batgarang.obj");

	/*
	* Escenario de Musica
	*/
	Model Stage = Model();
	Stage.LoadModel("Models/StageEmber/concertStage.obj");

	Model BocinasStageEmber = Model();
	BocinasStageEmber.LoadModel("Models/StageEmber/BocinasEmber.obj");

	Model LuzEscenario = Model();
	LuzEscenario.LoadModel("Models/StageEmber/luzEscenario.obj");

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

	
	
	

	/*
	* Avatares
	*/
	ModelJerarquia DannyPhantom_M = ModelJerarquia("Models/DannyPhantom");
	DannyPhantom_M.InitModel(glm::vec3(0.0f, 0.5f, 0.0f));
	ModelJerarquia Invencible_M = ModelJerarquia("Models/Invencible");
	Invencible_M.InitModel(glm::vec3(0.0f, 0.0f, 0.0f));
	ModelJerarquia Batman_M = ModelJerarquia("Models/BatmanBeyond");
	Batman_M.InitModel(glm::vec3(0.0f, -0.75f, 0.0f));

	ModelJerarquia BMO_M("Models/HoraAventura/BMO");
	BMO_M.InitModel(glm::vec3(0.0f, 0.0f, 0.0f));

	/*-------
	* NPC
	* Agregar modelos de los NPC aqui
	* ------
	*/
	ModelJerarquia AtomEve_M = ModelJerarquia("Models/AtomEve");
	AtomEve_M.InitModel(glm::vec3(0.0f, 0.0f, 0.0f));

	ModelJerarquia OmniMan_M = ModelJerarquia("Models/OmniMan");
	OmniMan_M.InitModel(glm::vec3(0.0f, 0.0f, 0.0f));

	ModelJerarquia HarleyQuinn = ModelJerarquia("Models/HarleyQuinn");
	HarleyQuinn.InitModel(glm::vec3(0.0f, 0.0f, 0.0f));

	ModelJerarquia Joker = ModelJerarquia("Models/Joker");
	Joker.InitModel(glm::vec3(0.0f, 1.1065f, 0.0f));

	ModelJerarquia Ember_M = ModelJerarquia("Models/Ember");
	Ember_M.InitModel(glm::vec3(0.0f, 0.0f,0.0f));
	Model GuitarEmber = Model();
	GuitarEmber.LoadModel("Models/Ember/Guitar.obj");

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

	/* --------------------------------------------
	*			Ciclo Día/Noche
	*///-------------------------------------------

	// Duración del "día" en segundos reales:
	const float DAY_LENGTH = 780.0f;    // por ejemplo, 1 minuto = 24 h
	float timeOfDay = 0.0f;           // contador que recorre [0, DAY_LENGTH)

	// Intensidades escalar día/noche:
	const GLfloat ambientDay = 0.4f;   // antes 0.3
	const GLfloat diffuseDay = 1.0f;   // antes 0.8
	const GLfloat ambientNight = 0.02f;  // antes 0.05
	const GLfloat diffuseNight = 0.05f;  // antes 0.1



	// Luz Direccional para Día/Noche
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
								 0.3f, 0.8f, // intensidad ambiental (radiacion de la luz), intensidad difusa
								 0.0f, -1.0f, 0.0f);

	// --------------------------------------------------------------------


	//contador de luces puntuales
	unsigned int pointLightCount = 0;

	// Luces Puntuales
	pointLights[0] = PointLight(1.0f, 1.0f, 1.0f,
								7.0f, 5.5f,				// Ambiental | Difuso 
								posPostesLuz[0].x, posPostesLuz[0].y, posPostesLuz[0].z, // pos
								0.3f, 0.5f, 0.1f);		// Atenuación cons, lin, exp
	pointLightCount++;

	pointLights[1] = PointLight(1.0f, 1.0f, 1.0f,
								7.0f, 5.5f,				// Ambiental | Difuso 
								posPostesLuz[1].x, posPostesLuz[1].y, posPostesLuz[1].z, // pos
								0.3f, 0.5f, 0.1f);		// Atenuación cons, lin, exp
	pointLightCount++;

	pointLights[2] = PointLight(1.0f, 1.0f, 1.0f,
								7.0f, 5.5f,				// Ambiental | Difuso 
								posPostesLuz[2].x, posPostesLuz[2].y, posPostesLuz[2].z, // pos
								0.3f, 0.5f, 0.1f);		// Atenuación cons, lin, exp
	pointLightCount++;

	pointLights[3] = PointLight(1.0f, 1.0f, 1.0f,
								7.0f, 5.5f,				// Ambiental | Difuso 
								posPostesLuz[3].x, posPostesLuz[3].y, posPostesLuz[3].z, // pos
								0.3f, 0.5f, 0.1f);		// Atenuación cons, lin, exp
	pointLightCount++;


	// Contador Luces SpotLight
	unsigned int spotLightCount = 0;
	
	// Luces SpotLight
	spotLights[0] = SpotLight(1.0f, 0.0f, 0.0f,
							  20.0f, 15.0f,				// Ambiental | Difuso 
						      0.0f, 20.0f, 0.0f,			// Posición 
							  0.0f, -1.0f, 0.0f,        // Dirección
							  0.3f, 0.8f, 0.0f,			// Atenuación cons, lin, exp
							  5.0f);					// Angulo Apertura
	spotLightCount++;

	spotLights[1] = SpotLight(0.0f, 0.0f, 1.0f,
							  20.0f, 15.0f,				// Ambiental | Difuso 
							  0.0f, 20.0f, 0.0f,			// Posición 
							  0.0f, -1.0f, 0.0f,        // Dirección
							  0.3f, 0.8f, 0.0f,			// Atenuación cons, lin, exp
							  5.0f);					// Angulo Apertura
	spotLightCount++;

	spotLights[2] = SpotLight(0.0f, 1.0f, 0.0f,
							  20.0f, 15.0f,				// Ambiental | Difuso 
							  0.0f, 20.0f, 0.0f,			// Posición 
							  0.0f, -1.0f, 0.0f,        // Dirección
							  0.3f, 0.8f, 0.0f,			// Atenuación cons, lin, exp
							  5.0f);					// Angulo Apertura
	spotLightCount++;

	// Arreglo Copia de Luces SpotLigt
	SpotLight copiespotLights[MAX_SPOT_LIGHTS];
	unsigned int indexSpotLight = 0;
	std::vector <int> indexSpotLights;

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
	float animaLampara = 0.0;
	GLfloat lastTimeProy = 0.0f;
	GLfloat anguloLaparaEscenario = 0.0f;
	GLfloat anguloLuzEscenario = 0.0f;
	GLfloat anguloBrazoEmber = 0.0f, anguloCabezaEmber = 0.0f;

	// animacion atracccion dados
	float animarDados = 0.0f;

	// animacion atraccion topos
	float animarTopos = 0.0f;

	// animacion atraccion hachas
	float animarHachas = 0.0f;

	//animacion atraccion jaula de bateo
	float animarBateo = 0.0f;
	float animarBola = 0.0f;

	//animacion revienta globos
	float animarLanzarDardo = 0.0f;
	float animarDardo = 0.0f;
	std::vector<glm::vec3> posGlobos = {
		glm::vec3(0.662f, 1.334f, 1.465),
		glm::vec3(0.006f, 1.334f, 1.465),
		glm::vec3(-0.653f, 1.334f, 1.465),
		glm::vec3(-0.653f, 1.885f, 1.465),
		glm::vec3(0.006f, 1.885f, 1.465),
		glm::vec3(0.662f, 1.885f, 1.465),
	};
	std::vector<float> tiempoPorGlobo(posGlobos.size(), 0.0f);

	//animacion boliche
	float animarLanzaBola = 0.0f;
	std::vector<glm::vec3> posBolas = {
		glm::vec3(8.373f, 0.123f, 1.888),
		glm::vec3(8.373f, 0.123f, 0.258),
		glm::vec3(8.373f, 0.123f, -1.432),
	};
	float animarPines = 0.0f;
	float animarBolaboliche = 0.0f;
	bool pinesDesaparecieron = false;
	float tiempoReinicio = 0.0f;
	const float TIEMPO_REAPARICION = 3.0f;  // segundos después de desaparecer

	// Posición del Avatar
	bool hadSelected = false;

	GLfloat timer = 0.0f, delay = 2.0f, movementLigts = -5.0f;
	bool reverse = false, luzReverse = true;
	copiespotLights[0] = spotLights[0];
	copiespotLights[1] = spotLights[1];
	copiespotLights[2] = spotLights[2];
	//variables auxiliares para animacion de caminata
	bool avatarCaminando = false;
	float animarCaminata = 0.0f;


	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		/*
		*  Ciclo Día / Noche 
		*/

		timeOfDay += deltaTime;
		if (timeOfDay >= DAY_LENGTH) timeOfDay -= DAY_LENGTH;

		// Normalizar t ∈ [0,1]
		float t = timeOfDay / DAY_LENGTH;

		// Calcular ángulo del sol
		float sunAngle = glm::radians(t * 360.0f - 90.0f);

		// Factor de interpolación basado en la altura del sol
		float f = glm::clamp(sin(sunAngle) * 0.5f + 0.5f, 0.0f, 1.0f);

		// Interpolación lineal de las intensidades escalares
		GLfloat ambient = ambientNight + f * (ambientDay - ambientNight);
		GLfloat diffuse = diffuseNight + f * (diffuseDay - diffuseNight);

		// Actualizar la luz direccional
		glm::vec3 sunDir = glm::normalize(glm::vec3(-cos(sunAngle), -sin(sunAngle), 0.0f));

		mainLight.setDirection(sunDir);
		mainLight.setAmbientIntensity(ambient);
		mainLight.setDiffuseIntensity(diffuse);

		// ----------------------------------------------------------------------------------------------------------
		

		// Animación Luces Escenario
		if (mainWindow.getisEscenarioOn()) {
			if (luzReverse) {
				if (anguloLaparaEscenario > 100.0f) {
					luzReverse = false;
				}
				else anguloLaparaEscenario += 2.5f * deltaTime;

			}
			else {
				if (anguloLaparaEscenario < 0.0f) {
					luzReverse = true;
				}
				else anguloLaparaEscenario -= 2.5f * deltaTime;

			}
		}
		// ----------------------------------------------------------------------------------------------------------

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

		bool nowSelected = mainWindow.isPersonajeSeleccionado();
		if (nowSelected && !hadSelected) {
			// justo al pulsar “Select”: inicializamos posición y saltamos a 3ª p.
			avatarPos = glm::vec3(0.0f, 0.0f, 0.0f);
			activeCamera = &thirdCam;
			hadSelected = true;
		}
		hadSelected = nowSelected;


		// Sólo la freeCam y thirdCam usan WASD+mouse:
		if (nowSelected && activeCamera == &thirdCam) {
			float speed = 0.1f;                         // ajusta a tu gusto
			float vel = speed * deltaTime;

			glm::vec3 forward = thirdCam.getCameraDirection();
			forward.y = 0.0f;
			forward = glm::normalize(forward);

			glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));

			// WASD
			if (mainWindow.getsKeys()[GLFW_KEY_W] || mainWindow.getsKeys()[GLFW_KEY_UP]) {
				avatarPos += forward * vel;
				avatarCaminando = true;
				animarCaminata += 0.5f * deltaTime;
			}
			if (mainWindow.getsKeys()[GLFW_KEY_S] || mainWindow.getsKeys()[GLFW_KEY_DOWN]) {
				avatarPos -= forward * vel;
				avatarCaminando = true;
				animarCaminata += 0.5f * deltaTime;
			}
			if (mainWindow.getsKeys()[GLFW_KEY_A] || mainWindow.getsKeys()[GLFW_KEY_LEFT]) {
				avatarPos -= right * vel;
				avatarCaminando = true;
				animarCaminata += 0.5f * deltaTime;
			}
			if (mainWindow.getsKeys()[GLFW_KEY_D] || mainWindow.getsKeys()[GLFW_KEY_RIGHT]) {
				avatarPos += right * vel;
				avatarCaminando = true;
				animarCaminata += 0.5f * deltaTime;
			}

			if (!mainWindow.getsKeys()[GLFW_KEY_W] && !mainWindow.getsKeys()[GLFW_KEY_UP] &&
				!mainWindow.getsKeys()[GLFW_KEY_S] && !mainWindow.getsKeys()[GLFW_KEY_DOWN] &&
				!mainWindow.getsKeys()[GLFW_KEY_A] && !mainWindow.getsKeys()[GLFW_KEY_LEFT] &&
				!mainWindow.getsKeys()[GLFW_KEY_D] && !mainWindow.getsKeys()[GLFW_KEY_RIGHT]
				) {
				avatarCaminando = false;
				animarCaminata = 0.0f;
			}

			// Forzamos Y=0 para “simular” suelo
			avatarPos.y = 0.0f;

			// Actualizamos el target de la cámara
			// (ya lo tienes apuntando a &avatarPos)
			thirdCam.update(deltaTime);
			thirdCam.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
		}
		else if (activeCamera == &freeCam) {
			// aquí dejas libreCam como antes
			freeCam.keyControl(mainWindow.getsKeys(), deltaTime);
			freeCam.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
		}

		glm::mat4 viewMatrix = activeCamera->getViewMatrix();



		// controlar la velocidad con la que rota la camara de seleccion de personaje
		rotacionCamara += 0.005f * deltaTime; // TODO: evitar desbordamiento de int

		float x = 30.0f * cos(rotacionCamara);
		float z = 30.0f * sin(rotacionCamara);

		

		if (mainWindow.isPersonajeSeleccionado()) {
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
		//spotLights[0].SetFlash(camPos, camDir);

		isDay = sin(sunAngle) < 0; // True = noche | False = dia


		// -------- Información al shader de fuentes de iluminación ---------

		// Para ciclo Día/Noche
		shaderList[0].SetDirectionalLight(&mainLight);


		// Para Postes de Luz
		if (isDay) {
			shaderList[0].SetPointLights(pointLights, pointLightCount);
		}
		else shaderList[0].SetPointLights(pointLights, 0);


		// Ambientación Rueda de la Fortuna
		copiespotLights[0] = spotLights[0];
		copiespotLights[1] = spotLights[1];
		copiespotLights[2] = spotLights[2];

		if (!mainWindow.getisEscenarioOn()) {
			if (isDay) {
				if (deltaTime < 2.0) {
					if (reverse) {
						if (movementLigts > 6.5f) {
							reverse = false;
						}
						else { movementLigts += 0.2 * deltaTime; }
					}
					else {
						if (movementLigts < -6.5f) {
							reverse = true;
						}
						else { movementLigts -= 0.2 * deltaTime; }
					}
				}
				// Para Ilumiinar Rueda de la Fortuna
				copiespotLights[0].SetPos(glm::vec3(movementLigts, 20.0f, 18.489f));
				copiespotLights[1].SetPos(glm::vec3(-1 * movementLigts, 20.0f, 18.489f));
				copiespotLights[2].SetPos(glm::vec3(-0.876f, 20.0f + movementLigts * 1.5, 18.489f));

				timer += 0.1 * deltaTime;
				if (timer >= delay) {
					shaderList[0].SetSpotLights(copiespotLights, spotLightCount - 1);
					timer = 0;
				}
				else shaderList[0].SetSpotLights(copiespotLights, spotLightCount);
			}
			else shaderList[0].SetSpotLights(copiespotLights, 0);
		}
		else {

			// Se activan luces de Escenario
			anguloLuzEscenario += 2.0 * deltaTime;

			// Luz central (roja)
			copiespotLights[0].setAmbientIntensity(0.5f);
			copiespotLights[0].setDiffuseIntensity(1.0f);
			copiespotLights[0].setAtenuacion(0.0f, 0.3f, 0.7f),
			copiespotLights[0].SetFlash(posLuz1, glm::vec3(0.0f, -1.0, cos(glm::radians(anguloLaparaEscenario))));

			// Luz izquierda (azul)
			copiespotLights[1].setAmbientIntensity(0.5f);
			copiespotLights[1].setDiffuseIntensity(1.0f);
			copiespotLights[1].setAtenuacion(0.0f, 0.3f, 0.7f),
			copiespotLights[1].SetFlash(posLuz2, glm::vec3(sin(glm::radians(-anguloLaparaEscenario)), -1.0, cos(glm::radians(-anguloLaparaEscenario))));

			// Luz derecha (verde)
			copiespotLights[2].setAmbientIntensity(0.5f);
			copiespotLights[2].setDiffuseIntensity(1.0f);
			copiespotLights[2].setAtenuacion(0.0f, 0.3f, 0.7f),
			copiespotLights[2].SetFlash(posLuz3, glm::vec3(sin(glm::radians(anguloLaparaEscenario)), -1.0, cos(glm::radians(anguloLaparaEscenario))));
			shaderList[0].SetSpotLights(copiespotLights, spotLightCount);

			 
		}
		


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
		SkayBoxDia.RenderModel();

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

		// Batimovil
		//model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(-21.8945f, 0.120827f, 21.5915f));
		////model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//batimovil.RenderModel();


		// Bancas Frontales 
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-3.156f, 0.0f, 11.62f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Banca.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(3.156f, 0.0f, 11.62f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Banca.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-3.156f, 0.0f, -11.65f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Banca.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(3.156f, 0.0f, -11.65f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Banca.RenderModel();

		// Bancas Laterales 
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-10.543f, 0.0f, 2.305f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Banca.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-10.543f, 0.0f, -2.805f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BancaTecho.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(13.3f, 0.0f, 2.305f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BancaTecho.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(13.3f, 0.0f, -2.805f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Banca.RenderModel();

		// BancasTecho
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 11.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BancaTecho.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -11.65f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BancaTecho.RenderModel();


		// Bancas Dsipersas
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-14.39f, 0.0f, 20.322f));
		model = glm::rotate(model, glm::radians(135.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BancaTecho.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-16.012f, 0.0f, 7.7102f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		BancaTecho.RenderModel();


		/*
		* Postes de Luz
		*/
		animaLampara += 0.06 * deltaTime;

		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(8.072f, 0.0f, 15.362f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PosteLampara.RenderModel();

		model = glm::translate(model, glm::vec3(0.0f, 5.02f + sin(animaLampara) / 15.0, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Lampara.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(12.429f, 0.0f, -8.4947f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PosteLampara.RenderModel();

		model = glm::translate(model, glm::vec3(0.0f, 5.02f + sin(animaLampara) / 15.0, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Lampara.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-16.142f, 0.0f, -1.0662f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PosteLampara.RenderModel();

		model = glm::translate(model, glm::vec3(0.0f, 5.02f + sin(animaLampara) / 15.0, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Lampara.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-19.785f, 0.0f, -25.709f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		PosteLampara.RenderModel();

		model = glm::translate(model, glm::vec3(0.0f, 5.02f + sin(animaLampara) / 15.0, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Lampara.RenderModel();

		// puesto de tortas invencible
		glm::vec3 posPuestoTortas(-7.366f, 0.0f, -16.264f);
		model = glm::mat4(1.0);
		model = glm::translate(model, posPuestoTortas);
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
		model = glm::rotate(model, glm::radians(22.004f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		TortasInvencible.RenderModel();

		//glm::vec3 posPuestoElotes(8.28819f, 0.008754f, -15.0227f);
		//model = glm::mat4(1.0);
		//model = glm::translate(model, posPuestoElotes);
		////model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
		////model = glm::rotate(model, glm::radians(22.004f), glm::vec3(0.0f, 1.0f, 0.0f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//PuestoElotes.RenderModel(); 
		

		


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
		meshBuilder.meshList[2]->RenderMesh();

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

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.34811f, 5.02231f, -30.285066f));
		model = glm::rotate(model, glm::radians(anguloLaparaEscenario), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LuzEscenario.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-1.73563f, 5.02231f, -30.285066f));
		model = glm::rotate(model, glm::radians(-anguloLaparaEscenario), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LuzEscenario.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(2.40227f, 5.02231f, -30.285066f));
		model = glm::rotate(model, glm::radians(anguloLaparaEscenario), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LuzEscenario.RenderModel();

		// —––– Árbol de Aventura —–––
		model = glm::mat4(1.0f);
		model = glm::translate(model,
			glm::vec3(0.49539f, -1.48f, -1.227f));
		model = glm::rotate(model,
			glm::radians(90.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model,
			glm::vec3(0.033f, 0.042f, 0.040f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		arbol.RenderModel();




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
		model = glm::translate(model, posRevientaGlobos);
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		RevientaGlobosInvencible.RenderModel();
		if (activarAtraccionAnimacion[4]) {
			animarLanzarDardo += 0.1 * deltaTime;
			for (int i = 0; i < posGlobos.size(); i++) {
				tiempoPorGlobo[i] += 0.01f * deltaTime;
				model = modelaux;
				model = glm::translate(model, glm::vec3(posGlobos[i].x, posGlobos[i].y, posGlobos[i].z));
				float scaleFactor = 1.0f;
				if (tiempoPorGlobo[i] < 1.0f) {
					scaleFactor = tiempoPorGlobo[i];  // escalar desde 0 a 1
				}
				// Fase de mantener visible
				else if (tiempoPorGlobo[i] < 2.0f + i) {
					scaleFactor = 1.0f;  // completamente visible
				}
				else {
					scaleFactor = 0.0f;    // escalar hacia 0
				}
				model = glm::scale(model, glm::vec3(scaleFactor));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				Globo.RenderModel();
				printf("funciona globos\n");
			}

			animarDardo += 0.02 * deltaTime;
			model = modelaux;
			model = glm::translate(model, glm::vec3(0.0f, 1.898f, -1.418 + animarDardo));
			model = glm::scale(model, glm::vec3(0.5));
			model = glm::rotate(model, glm::radians(animarDardo * 1000.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			if (animarDardo >= 3.0f) { // terminar animacion
				animarDardo = 0;
			}
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			batgarang.RenderModel();
		}

		/*
		* Boliche - A2
		*/
		model = glm::mat4(1.0);
		model = glm::translate(model, posBoliche);
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

		//// Bola Azul
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

		//// ------------

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

		///*
		//* Pines de Boliche por Linea
		//*/
		if (activarAtraccionAnimacion[1]) {
			// Mover la bola
			if (!pinesDesaparecieron) {
				animarBolaboliche += 0.1f * deltaTime;
				if (animarBolaboliche >= 14.0f) {
					animarPines = glm::clamp(animarPines + 0.5f * deltaTime, 0.0f, 1.0f);
				}
				if (animarPines >= 1.0f) {
					pinesDesaparecieron = true;
					tiempoReinicio = 0.0f;  // iniciar conteo
				}
			}

			// Contar tiempo para reaparecer
			if (pinesDesaparecieron) {
				tiempoReinicio += deltaTime;
				if (tiempoReinicio >= TIEMPO_REAPARICION) {
					// Reiniciar animaciones
					animarBolaboliche = 0.0f;
					animarPines = 0.0f;
					pinesDesaparecieron = false;
				}
			}

			float eased = animarPines * animarPines * (3 - 2 * animarPines);

			if (!pinesDesaparecieron) {
				// Línea 1
				model = modelrootBoliche;
				model = glm::translate(model, glm::vec3(-5.5f, 0.0f, -1.0f));
				model = glm::translate(model, glm::vec3(0.0f, -0.432f * eased, 0.0f));
				SetPinesBoliche(uniformModel, pinesTrans, model, pinBoliche);

				// Línea 2
				model = modelrootBoliche;
				model = glm::translate(model, glm::vec3(-5.5f, 0.0f, 0.7f));
				model = glm::translate(model, glm::vec3(0.0f, -0.432f * eased, 0.0f));
				SetPinesBoliche(uniformModel, pinesTrans, model, pinBoliche);

				// Línea 3
				model = modelrootBoliche;
				model = glm::translate(model, glm::vec3(-5.5f, 0.0f, 2.3f));
				model = glm::translate(model, glm::vec3(0.0f, -0.432f * eased, 0.0f));
				SetPinesBoliche(uniformModel, pinesTrans, model, pinBoliche);
			}

			for (int i = 0; i < 3; i++) {
				model = modelrootBoliche;
				model = glm::translate(model, glm::vec3(posBolas[i].x - animarBolaboliche, posBolas[i].y, posBolas[i].z));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				BolaAzul.RenderModel();
			}
		}

		// -----------------------


		/*
		* Fin Boliche - A2
		*/

		//ATRACCION DADOS --------------------------------------------------------------------------------
		if (activarAtraccionAnimacion[2])
			animarDados += 0.05 * deltaTime;
		model = glm::mat4(1.0);
		model = glm::translate(model, posLanzaDados);
		model = glm::scale(model, glm::vec3(0.6));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		carpa.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -0.085, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		mesa_dados.RenderModel();
		//DADO IZQUIERDO
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.285f, 0.643f, -0.307f));
		if (activarAtraccionAnimacion[2]) {
			model = glm::rotate(model, -sin(animarDados), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::translate(model, 0.05f * glm::vec3(0.0f, sin(animarDados), 0.0f));
		}
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dado.RenderModel();
		//DADO DERECHO
		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.248f, 0.643f, 0.062f));
		if (activarAtraccionAnimacion[2]) {
			model = glm::rotate(model, sin(animarDados), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::translate(model, 0.05f * glm::vec3(0.0f, -sin(animarDados), 0.0f));
		}
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dado.RenderModel();
		// FIN ATRACCION DADOS 

		//ATRACCION GUACAMOLE ----------------------------------------------------------------------
		model = glm::mat4(1.0);
		model = glm::translate(model, posmaquinaWhack);
		model = glm::scale(model, glm::vec3(0.3f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		maquinaWhack.RenderModel();

		// Topos delanteros de izquierda a derecha
		if (activarAtraccionAnimacion[5])
			animarTopos += 0.05 * deltaTime;

		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.599f, 2.174f, 0.0f));
		if (activarAtraccionAnimacion[5]) {
			model = glm::translate(model, glm::vec3(0.0f, sin(animarTopos * 1.5), 0.0f));
		}
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		topo.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.217f, 2.174f, 0.0f));
		if (activarAtraccionAnimacion[5]) {
			model = glm::translate(model, glm::vec3(0.0f, sin(animarTopos * 11.2), 0.0f));
		}
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		topo.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.181f, 2.174f, 0.0f));
		if (activarAtraccionAnimacion[5]) {
			model = glm::translate(model, glm::vec3(0.0f, sin(animarTopos * 1.2), 0.0f));
		}
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		topo.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.577f, 2.174f, 0.0f));
		if (activarAtraccionAnimacion[5]) {
			model = glm::translate(model, glm::vec3(0.0f, sin(animarTopos * 0.8), 0.0f));
		}
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		topo.RenderModel();

		//Topos traseros de izquierda a derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.619f, 2.174f, 0.385f));
		if (activarAtraccionAnimacion[5]) {
			model = glm::translate(model, glm::vec3(0.0f, sin(animarTopos * 0.6), 0.0f));
		}
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		topo.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.191f, 2.174f, 0.385f));
		if (activarAtraccionAnimacion[5]) {
			model = glm::translate(model, glm::vec3(0.0f, sin(animarTopos * 0.3), 0.0f));
		}
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		topo.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.206f, 2.174f, 0.385f));
		if (activarAtraccionAnimacion[5]) {
			model = glm::translate(model, glm::vec3(0.0f, sin(animarTopos * 0.1), 0.0f));
		}
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		topo.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.616f, 2.174f, 0.385f));
		if (activarAtraccionAnimacion[5]) {
			model = glm::translate(model, glm::vec3(0.0f, sin(animarTopos * 0.5), 0.0f));
		}
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		topo.RenderModel();

		// FIN ATRACCION GUACAMOLE ----------------------------------------------------------------------

		//ZONA BATEO ----------------------------------------------------------------------
		model = glm::mat4(1.0);
		model = glm::translate(model, posJaulaBateo);
		model = scale(model, glm::vec3(0.4f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		zonabateo.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.375f, -2.877f, -16.207f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		maquinaTenis.RenderModel();
		if (activarAtraccionAnimacion[3]) {
			animarBateo += 0.1 * deltaTime;
			animarBola += 0.1 * deltaTime;
			model = modelaux;
			model = glm::translate(model, glm::vec3(0.493f, 0.682f, -2.341f - animarBola));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			pelotaTenis.RenderModel();
			if (animarBola >= 14.0f) { // terminar animacion
				animarBola = 0;
			}
		}
		// FIN ZONA BATEO ----------------------------------------------------------------------

		//LANZAMIENTO DE HACHA -----------------------------------------------------------------
		model = glm::mat4(1.0);
		model = glm::translate(model, posLanzaHacha);
		model = glm::scale(model, glm::vec3(0.5f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LanzamientoDeHacha.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -6.642f));
		if (activarAtraccionAnimacion[0]) {
			animarHachas += 0.05 * deltaTime;

			model = glm::translate(model, glm::vec3(0.0f, 0.0f, animarHachas));
			model = glm::rotate(model, glm::radians(animarHachas * 1000.0f), glm::vec3(1.0f, 0.0f, 0.0f));

			if (animarHachas >= 8.0f) { // terminar animacion
				animarHachas = 0;
				activarAtraccionAnimacion[0] = false;
			}
		}

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		batgarang.RenderModel();

		/*model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-21.8945f, 0.120827f, 21.5915f));
		//model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Batimovil.RenderModel();*/


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
		anguloCabezaEmber += 0.2f * deltaTime;
		anguloBrazoEmber += 5.0f * deltaTime;
		Ember_M.MovFullModel(glm::vec3(0.0f, 2.3f, -28.0f));
		Ember_M.TransformHead(glm::vec3(0.0f, 0.059094f, 0.007059f), glm::vec3(1.0f, 0.0f, 0.0f), 10 * sin(anguloCabezaEmber));
		Ember_M.TransformLegR(glm::vec3(-0.048772f, -0.333642f, 0.0f));
		Ember_M.TransformLegL(glm::vec3(0.075321f, -0.383061f, -0.008162f));
		Ember_M.TransformArmR(glm::vec3(-0.074838f, 0.000344f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), -80.0f + 60.0f * sin(glm::radians(anguloBrazoEmber)) / 2);
		Ember_M.TransformArmL(glm::vec3(0.074127f, 0.000064f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), -80.0f + 60.f * sin(glm::radians(-anguloBrazoEmber)) / 2);
		Ember_M.RenderModelJ(uniformModel);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 2.3f, -28.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GuitarEmber.RenderModel();

		/*
		* ------------------
		* NPC de ambiente
		* Aqui renderizar todos los NPC de ambiente
		* ------------------
		*/
		//Harley Quinn -  Zona Bateo
		HarleyQuinn.MovFullModel(glm::vec3(25.2693f, 0.967349f, 5.02014f));
		HarleyQuinn.TransformHead(glm::vec3(0.0f, 0.683806f, 0.021197f));//
		HarleyQuinn.TransformLegR(glm::vec3(0.053826f, 0.139596f, 0.0f));//
		HarleyQuinn.TransformLegL(glm::vec3(-0.053826f, 0.139596f, 0.0f));//
		HarleyQuinn.TransformArmR(glm::vec3(0.189628f, 0.573444f, 0.023411f));//
		HarleyQuinn.TransformArmL(glm::vec3(-0.189628f, 0.573444f, 0.023411f));//
		HarleyQuinn.RenderModelJ(uniformModel);

		//Joker -  Bati elotes
		Joker.MovFullModel(glm::vec3(10.49f, 0.0f, -12.8796f));
		Joker.TransformHead(glm::vec3(0.0f, 0.931009f, -0.113821f));//
		Joker.TransformLegR(glm::vec3(-0.165213f, 0.009248f, 0.0f));//
		Joker.TransformLegL(glm::vec3(0.230091f, 0.033922f, 0.0f));//
		Joker.TransformArmR(glm::vec3(-0.251991f, 0.831474f, -0.053465f));//
		Joker.TransformArmL(glm::vec3(0.251991f, 0.831474f, -0.053465f));//
		Joker.RenderModelJ(uniformModel);

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
			Batman_M.TransformHead(glm::vec3(0.0f, 0.765671f, -0.10585f));
			Batman_M.TransformLegR(glm::vec3(-0.206243f, 0.01851f, 0.0f));
			Batman_M.TransformLegL(glm::vec3(0.174945f, 0.073709f, 0.0f));
			Batman_M.TransformArmR(glm::vec3(-0.293808f, 0.658702f, -0.050691f));
			Batman_M.TransformArmL(glm::vec3(0.293808f, 0.658702f, -0.050691f));
			Batman_M.RenderModelJ(uniformModel);
		}
		else if (!mainWindow.isPersonajeSeleccionado() && idPersonaje == 3) {
			glm::vec3 modeloPos = camPos + camFront * 3.0f;
			BMO_M.TranformFullModel(modeloPos, orientacion);
			BMO_M.RenderModelJ(uniformModel);
		}

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		/*
		* variales utiles para interfaces
		*/
		// Obtener vectores de la camara
		glm::vec3 camRight = glm::normalize(glm::cross(camFront, glm::vec3(0.0f, 1.0f, 0.0f)));
		glm::vec3 camUp = glm::normalize(glm::cross(camRight, camFront));
		// Posicion frente a la camara (a 10 unidades)
		glm::vec3 basePos = camPos + camFront * 10.0f;
		std::string texto = "";
		/*
		* Interaccion con las atracciones
		*/
		for (int i = 0; i < 6; i++) {
			if (glm::distance(avatarPos, posicionAtracciones[i]) <= DISTANCIA_PARA_ACTIVAR) {
				if (mainWindow.getsKeys()[GLFW_KEY_E])
					activarAtraccionAnimacion[i] = true;

				//renderizar texto
				texto = "PARA JUGAR PRESIONA E";
				for (int i = 0; i < texto.size(); i++) {
					if (texto[i] == ' ') continue;

					float espaciado = 1.0f;
					glm::vec3 offsetPos = basePos + camRight * (-7.0f + espaciado * i) + camUp * -4.5f;
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
					meshBuilder.meshList[0]->RenderMesh();
				}
			}
			else
				activarAtraccionAnimacion[i] = false;
		}

		// reiniciar offset
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(glm::vec2(0.0f)));
		/*
		* TODO: abstraer a una clase para no usar tantos IFs
		*/
		// desaparecer interfaz si el usuario ya selecciono un avatar
		float movArticulacion = 0.05f * sin(animarCaminata);
		float rotArticulacion = 25 * sin(animarCaminata);
		glm::vec3 forward = thirdCam.getHorizontalDirection();
		if (mainWindow.isPersonajeSeleccionado()) {
			switch (idPersonaje) {
			case 0:
				DannyPhantom_M.MovFullModel(glm::vec3(avatarPos.x, avatarPos.y + 0.4f, avatarPos.z),
					glm::vec3(0, 1, 0), glm::degrees(atan2(forward.x, forward.z)));
				DannyPhantom_M.TransformHead(glm::vec3(0.0f, 0.21f, 0.0f));
				DannyPhantom_M.TransformLegR(glm::vec3(-0.04f, -0.2f, 0.0f));
				if (avatarCaminando) {
					DannyPhantom_M.TransformLegR(
						glm::vec3(-0.04f, -0.2f + movArticulacion, 0.0f),
						glm::vec3(1.0f, 0.0f, 0.0f),
						rotArticulacion
					);
				}
				DannyPhantom_M.TransformLegL(glm::vec3(0.035f, -0.2f, 0.0f));
				if (avatarCaminando) {
					DannyPhantom_M.TransformLegL(
						glm::vec3(0.035f, -0.2 + -movArticulacion, 0.0f),
						glm::vec3(1.0f, 0.0f, 0.0f),
						-rotArticulacion
					);
				}
				DannyPhantom_M.TransformArmR(glm::vec3(-0.1445f, 0.123f, 0.0f));
				if (avatarCaminando) {
					DannyPhantom_M.TransformArmR(
						glm::vec3(-0.1445f, 0.123f, 0.0f),
						glm::vec3(1.0f, 0.0f, 0.0f),
						-rotArticulacion
					);
				}
				if (activarAtraccionAnimacion[2]) { // dados
					DannyPhantom_M.TransformArmR(
						glm::vec3(-0.1445f, 0.123f, 0.0f),
						glm::vec3(1.0f, 0.0f, 0.0f),
						-100.0f - 50 * sin(animarDados));
					model = DannyPhantom_M.getModelMatrixs()[3];
					model = glm::translate(model, glm::vec3(-0.272f, -0.527f, -0.224f));
					model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
					model = glm::scale(model, glm::vec3(0.6f));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
					dado.RenderModel();
				}
				if (activarAtraccionAnimacion[3]) { // jaula de bateo
					DannyPhantom_M.TransformArmR(
						glm::vec3(-0.1445f, 0.123f, 0.0f),
						glm::vec3(1.0f, 0.0f, 0.0f),
						-50 * sin(animarBateo));
					model = DannyPhantom_M.getModelMatrixs()[3];
					model = glm::translate(model, glm::vec3(-0.339f, -0.473f, -0.094f));
					model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
					model = glm::scale(model, glm::vec3(0.8f));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
					bat.RenderModel();
				}
				if (activarAtraccionAnimacion[4]) { // batrang
					DannyPhantom_M.TransformArmR(
						glm::vec3(-0.1445f, 0.123f, 0.0f),
						glm::vec3(1.0f, 0.0f, 0.0f),
						-144.377 - 50 * sin(animarLanzarDardo));
					model = DannyPhantom_M.getModelMatrixs()[3];
					model = glm::translate(model, glm::vec3(-0.259f, -0.463f, -0.123f));
					model = glm::scale(model, glm::vec3(0.5f));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
					batgarang.RenderModel();
				}
				if (activarAtraccionAnimacion[5]) { // topos
					DannyPhantom_M.TransformArmR(
						glm::vec3(-0.1445f, 0.123f, 0.0f),
						glm::vec3(1.0f, 0.0f, 0.0f),
						-50 * sin(animarTopos));
					model = DannyPhantom_M.getModelMatrixs()[3];
					model = glm::translate(model, glm::vec3(-0.272f, -0.527f, -0.224f));
					model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
					model = glm::scale(model, glm::vec3(0.3f));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
					martillo_topos.RenderModel();
				}
				DannyPhantom_M.TransformArmL(glm::vec3(0.13f, 0.12f, 0.0f));
				if (avatarCaminando) {
					DannyPhantom_M.TransformArmL(
						glm::vec3(0.13f, 0.12f, 0.0f),
						glm::vec3(1.0f, 0.0f, 0.0f),
						rotArticulacion
					);
				}
				DannyPhantom_M.RenderModelJ(uniformModel);
				break;
			case 1:
				Invencible_M.MovFullModel(glm::vec3(avatarPos.x, avatarPos.y + 0.65f, avatarPos.z),
					glm::vec3(0, 1, 0), glm::degrees(atan2(forward.x, forward.z)));
				Invencible_M.TransformHead(glm::vec3(0.005f, 0.548f, -0.011f));
				Invencible_M.TransformLegR(glm::vec3(-0.011f, 0.061f, 0.002f));
				if (avatarCaminando) {
					Invencible_M.TransformLegR(
						glm::vec3(-0.011f, 0.061f + movArticulacion, 0.002f),
						glm::vec3(1.0f, 0.0f, 0.0f),
						rotArticulacion
					);
				}
				Invencible_M.TransformLegL(glm::vec3(0.007f, 0.071f, 0.005f));
				if (avatarCaminando) {
					Invencible_M.TransformLegL(
						glm::vec3(0.007f, 0.071f - movArticulacion, 0.005f),
						glm::vec3(1.0f, 0.0f, 0.0f),
						-rotArticulacion
					);
				}
				Invencible_M.TransformArmR(glm::vec3(-0.121f, 0.393f, -0.023f));
				if (avatarCaminando) {
					Invencible_M.TransformArmR(
						glm::vec3(-0.121f, 0.393f, -0.023f),
						glm::vec3(1.0f, 0.0f, 0.0f),
						-rotArticulacion
					);
				}
				if (activarAtraccionAnimacion[5]) { // topos
					Invencible_M.TransformArmR(
						glm::vec3(-0.121f, 0.393f, -0.023f),
						glm::vec3(1.0f, 0.0f, 0.0f),
						-50 * sin(animarTopos)
					);
					model = Invencible_M.getModelMatrixs()[3];
					model = glm::translate(model, glm::vec3(-0.317f, -0.348f, 0.127f));
					model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
					model = glm::scale(model, glm::vec3(0.3f));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
					martillo_topos.RenderModel();
				}
				if (activarAtraccionAnimacion[3]) { // jaula de bateo
					Invencible_M.TransformArmR(
						glm::vec3(-0.121f, 0.393f, -0.023f),
						glm::vec3(1.0f, 0.0f, 0.0f),
						-50 * sin(animarBateo));
					model = Invencible_M.getModelMatrixs()[3];
					model = glm::translate(model, glm::vec3(-0.327f, -0.285f, 0.184f));
					model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
					model = glm::scale(model, glm::vec3(0.8f));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
					bat.RenderModel();
				}
				if (activarAtraccionAnimacion[4]) { // batrang
					Invencible_M.TransformArmR(
						glm::vec3(-0.121f, 0.393f, -0.023f),
						glm::vec3(1.0f, 0.0f, 0.0f),
						-144.377 - 50 * sin(animarLanzarDardo));
					model = Invencible_M.getModelMatrixs()[3];
					model = glm::translate(model, glm::vec3(-0.259f, -0.292f, 0.124f));
					model = glm::scale(model, glm::vec3(0.5f));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
					batgarang.RenderModel();
				}
				if (activarAtraccionAnimacion[2]) { // dados
					Invencible_M.TransformArmR(
						glm::vec3(-0.121f, 0.393f, -0.023f),
						glm::vec3(1.0f, 0.0f, 0.0f),
						-100.0f - 50 * sin(animarDados));
					model = Invencible_M.getModelMatrixs()[3];
					model = glm::translate(model, glm::vec3(-0.275f, -0.315f, 0.054f));
					model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
					model = glm::scale(model, glm::vec3(0.6f));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
					dado.RenderModel();
				}
				Invencible_M.TransformArmL(glm::vec3(0.13f, 0.397f, -0.028f));
				if (avatarCaminando) {
					Invencible_M.TransformArmL(
						glm::vec3(0.13f, 0.397f, -0.028f),
						glm::vec3(1.0f, 0.0f, 0.0f),
						rotArticulacion
					);
				}
				Invencible_M.RenderModelJ(uniformModel);
				break;
			case 2:
				Batman_M.MovFullModel(glm::vec3(avatarPos.x, avatarPos.y + 1.7f, avatarPos.z),
					glm::vec3(0, 1, 0), glm::degrees(atan2(forward.x, forward.z)));
				Batman_M.TransformHead(glm::vec3(0.0f, 0.765671f, -0.10585f));
				Batman_M.TransformLegR(glm::vec3(-0.206243f, 0.01851f, 0.0f));
				if (avatarCaminando) {
					Batman_M.TransformLegR(
						glm::vec3(-0.206243f, 0.01851f + movArticulacion, 0.0f),
						glm::vec3(1.0f, 0.0f, 0.0f),
						rotArticulacion
					);
				}
				Batman_M.TransformLegL(glm::vec3(0.174945f, 0.073709f, 0.0f));
				if (avatarCaminando) {
					Batman_M.TransformLegL(
						glm::vec3(0.174945f, 0.073709f - movArticulacion, 0.0f),
						glm::vec3(1.0f, 0.0f, 0.0f),
						-rotArticulacion
					);
				}
				Batman_M.TransformArmR(glm::vec3(-0.293808f, 0.658702f, -0.050691f));
				if (avatarCaminando) {
					Batman_M.TransformArmR(
						glm::vec3(-0.293808f, 0.658702f, -0.050691f),
						glm::vec3(1.0f, 0.0f, 0.0f),
						-rotArticulacion
					);
				}
				if (activarAtraccionAnimacion[2]) { // dados
					Batman_M.TransformArmR(
						glm::vec3(-0.293808f, 0.658702f, -0.050691f),
						glm::vec3(1.0f, 0.0f, 0.0f),
						-100.0f - 50 * sin(animarDados));
					model = Batman_M.getModelMatrixs()[3];
					model = glm::translate(model, glm::vec3(-0.1f, -0.624f, 0.016f));
					model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
					model = glm::scale(model, glm::vec3(0.6f));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
					dado.RenderModel();
				}
				if (activarAtraccionAnimacion[3]) { // jaula de bateo
					Batman_M.TransformArmR(
						glm::vec3(-0.293808f, 0.658702f, -0.050691f),
						glm::vec3(1.0f, 0.0f, 0.0f),
						-50 * sin(animarBateo));
					model = Batman_M.getModelMatrixs()[3];
					model = glm::translate(model, glm::vec3(-0.162f, -0.599f, 0.048f));
					model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
					model = glm::scale(model, glm::vec3(0.8f));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
					bat.RenderModel();
				}
				if (activarAtraccionAnimacion[4]) { // batrang
					Batman_M.TransformArmR(
						glm::vec3(-0.293808f, 0.658702f, -0.050691f),
						glm::vec3(1.0f, 0.0f, 0.0f),
						-144.377 - 50 * sin(animarLanzarDardo));
					model = Batman_M.getModelMatrixs()[3];
					model = glm::translate(model, glm::vec3(-0.09f, -0.602f, -0.005f));
					model = glm::scale(model, glm::vec3(0.5f));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
					batgarang.RenderModel();
				}
				if (activarAtraccionAnimacion[5]) { // topos
					Batman_M.TransformArmR(
						glm::vec3(-0.293808f, 0.658702f, -0.050691f),
						glm::vec3(1.0f, 0.0f, 0.0f),
						-50 * sin(animarTopos)
					);
					model = Batman_M.getModelMatrixs()[3];
					model = glm::translate(model, glm::vec3(-0.108f, -0.599f, -0.009f));
					model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
					model = glm::scale(model, glm::vec3(0.3f));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
					martillo_topos.RenderModel();
				}

				Batman_M.TransformArmL(glm::vec3(0.293808f, 0.658702f, -0.050691f));
				if (avatarCaminando) {
					Batman_M.TransformArmL(
						glm::vec3(0.293808f, 0.658702f, -0.050691f),
						glm::vec3(1.0f, 0.0f, 0.0f),
						rotArticulacion
					);
				}
				Batman_M.RenderModelJ(uniformModel);
				break;
				// más cases según los avatares...
			}
			glDisable(GL_BLEND);
			glUseProgram(0);
			mainWindow.swapBuffers();
			continue;
		}

		/*
		* Interfaz ligada a la camara
		*/

		// Obtener vectores de la camara
		camRight = glm::normalize(glm::cross(camFront, glm::vec3(0.0f, 1.0f, 0.0f)));
		camUp = glm::normalize(glm::cross(camRight, camFront));

		// Posicion frente a la camara (a 10 unidades)
		basePos = camPos + camFront * 10.0f;

		// renderizar encabezados
		texto = "FERIA FUTURISTA";
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
			meshBuilder.meshList[0]->RenderMesh();
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
			meshBuilder.meshList[1]->RenderMesh();
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
			meshBuilder.meshList[1]->RenderMesh();
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
			meshBuilder.meshList[1]->RenderMesh();
		}
		// TODO: renderizar condicionalmente los demas logos

		glDisable(GL_BLEND);
		glUseProgram(0);
		mainWindow.swapBuffers();
	}

	return 0;
}
