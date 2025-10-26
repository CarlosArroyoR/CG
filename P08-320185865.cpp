/*
Práctica 7: Iluminación 1 
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

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
Texture dadoTexture;        // textura del dado
int d8Index = -1;           // índice del mesh del dado en meshList

Model Kitt_M;
Model Llanta_M;
Model Blackhawk_M;
Model Lampara;
Model Pikachu;
Model Cofre;



Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;


//Sphere cabeza = Sphere(0.5, 20, 20);
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

void CrearDado() //Dado 8 caras
{
	// 8 caras = 8 triángulos = 24 vértices (cada triángulo con su normal plana)
	unsigned int d8_indices[] = {
		0,1,2,  3,4,5,  6,7,8,  9,10,11,
		12,13,14,  15,16,17,  18,19,20,  21,22,23
	};

	// stride = 8: 3 pos, 2 uv, 3 normal
	GLfloat d8_vertices[] = {
		// Cara arriba 1 (normal + + +)  -> UV celda 1
		 0, 1, 0,   0.167f, 1.000f,   0.577f, 0.577f, 0.577f,
		 0, 0, 1,   0.000f, 0.667f,   0.577f, 0.577f, 0.577f,
		 1, 0, 0,   0.333f, 0.667f,   0.577f, 0.577f, 0.577f,

		 // Cara arriba 2 (normal - + +)  -> UV celda 5
		  0, 1, 0,   0.500f, 1.000f,  -0.577f, 0.577f, 0.577f,
		 -1, 0, 0,   0.333f, 0.667f,  -0.577f, 0.577f, 0.577f,
		  0, 0, 1,   0.667f, 0.667f,  -0.577f, 0.577f, 0.577f,

		  // Cara arriba 3 (normal - + -)  -> UV celda 8
		   0, 1, 0,   0.833f, 1.000f,  -0.577f, 0.577f, -0.577f,
		   0, 0, -1,  0.667f, 0.667f,  -0.577f, 0.577f, -0.577f,
		  -1, 0, 0,   1.000f, 0.667f,  -0.577f, 0.577f, -0.577f,

		  // Cara arriba 4 (normal + + -)  -> UV celda 7
		   0, 1, 0,   0.833f, 0.667f,   0.577f, 0.577f, -0.577f,
		   1, 0, 0,   0.667f, 0.333f,   0.577f, 0.577f, -0.577f,
		   0, 0, -1,  1.000f, 0.333f,   0.577f, 0.577f, -0.577f,

		   // Cara abajo 5 (normal + - +)   -> UV celda 3
			0, -1, 0,  0.167f, 0.667f,   0.577f, -0.577f, 0.577f,
			1,  0, 0,  0.000f, 0.333f,   0.577f, -0.577f, 0.577f,
			0,  0, 1,  0.333f, 0.333f,   0.577f, -0.577f, 0.577f,

			// Cara abajo 6 (normal - - +)   -> UV celda 4
			 0, -1, 0,  0.500f, 0.667f,  -0.577f, -0.577f, 0.577f,
			 0,  0, 1,  0.333f, 0.333f,  -0.577f, -0.577f, 0.577f,
			-1,  0, 0,  0.667f, 0.333f,  -0.577f, -0.577f, 0.577f,

			// Cara abajo 7 (normal - - -)   -> UV celda 6
			 0, -1, 0,  0.167f, 0.333f,  -0.577f, -0.577f, -0.577f,
			-1,  0, 0,  0.000f, 0.000f,  -0.577f, -0.577f, -0.577f,
			 0,  0,-1,  0.333f, 0.000f,  -0.577f, -0.577f, -0.577f,

			 // Cara abajo 8 (normal + - -)   -> UV celda 2
			  0, -1, 0,  0.500f, 0.333f,   0.577f, -0.577f, -0.577f,
			  0,  0,-1,  0.333f, 0.000f,   0.577f, -0.577f, -0.577f,
			  1,  0, 0,  0.667f, 0.000f,   0.577f, -0.577f, -0.577f,
	};

	Mesh* d8 = new Mesh();
	d8->CreateMesh(d8_vertices, d8_indices, 24 * 8, 8 * 3);
	meshList.push_back(d8);
	d8Index = static_cast<int>(meshList.size()) - 1;  // guardamos índice
}

int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CrearDado();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

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
	dadoTexture = Texture("Textures/Numeros.png"); //Textura dado
	dadoTexture.LoadTextureA();

	Kitt_M = Model();
	Kitt_M.LoadModel("Models/Cuerpo.obj");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/Rueda.obj");
	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");
	Lampara = Model();
	Lampara.LoadModel("Models/lampara.obj");
	Pikachu = Model();
	Pikachu.LoadModel("Models/Pikachu.obj");
	Cofre = Model();
	Cofre.LoadModel("Models/Cofre.obj");
	

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

	int idxLampPL = -1; // lámpara (point light)
	int idxModelPL = -1; // point light del modelo
	int idxSpotFront = -1; // spotlight delantero del coche
	int idxSpotBack = -1; // spotlight trasero del coche
	int idxCofrePL = -1;
	int idxCofreSpot = -1;


	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;


	//Declaración de primer luz puntual (LAMPARA)
	idxLampPL = pointLightCount; //Guardo el indice
	pointLights[pointLightCount] = PointLight(1.0f, 1.0f, 1.0f,
		0.0f, 1.0f,
		-5.0f, 2.0f, 4.0f, //Posicion
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	
	//Luz Pikachu P (Prender)
	idxModelPL = pointLightCount;  // guarda el índice
	pointLights[pointLightCount] = PointLight(
		1.0f, 1.0f, 0.0f,   // color: amarillo
		0.1f, 1.0f,         
		-3.0f, -1.0f, -3.0f, 
		0.3f, 0.2f, 0.1f    
	);
	pointLightCount++;

	// Luz puntual del cofre 
	idxCofrePL = pointLightCount;
	pointLights[pointLightCount] = PointLight(
		1.0f, 0.8f, 0.4f, // color cálido
		0.05f, 1.0f,       
		0.0f, 0.0f, 0.0f,  
		0.3f, 0.2f, 0.1f   
	);
	pointLightCount++;

	//  SPOTLIGHTS 
	
	unsigned int spotLightCount = 0;

	// Spotlight delantero 
	idxSpotFront = spotLightCount;
	spotLights[spotLightCount] = SpotLight(
		1.0f, 1.0f, 0.0f,    // color amarillo
		0.0f, 2.0f,          
		0.0f, 0.0f, 0.0f,    
		0.0f, -1.0f, 0.0f,   
		1.0f, 0.1f, 0.0f,    
		15.0f               
	);
	spotLightCount++;

	// Spotlight trasero
	idxSpotBack = spotLightCount;
	spotLights[spotLightCount] = SpotLight(
		1.0f, 0.0f, 1.0f,    // color magenta 
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.1f, 0.0f,
		15.0f
	);
	spotLightCount++;

	// Spotlight del cofre 
	idxCofreSpot = spotLightCount;
	spotLights[spotLightCount] = SpotLight(
		1.0f, 0.85f, 0.4f,  
		0.0f, 2.0f,          
		0.0f, 0.0f, 0.0f,    
		0.0f, -1.0f, 0.0f,   
		1.0f, 0.1f, 0.0f,    
		20.0f                
	);
	spotLightCount++;


	//unsigned int spotLightCount = 0;
	////linterna
	//spotLights[0] = SpotLight(
	//	1.0f, 1.0f, 0.0f,     // color amarillo
	//	0.0f, 2.0f,           
	//	0.0f, 5.0f, 6.0f,     // posición inicial 
	//	0.0f, -1.0f, 0.0f,    // dirección hacia abajo 
	//	1.0f, 0.1f, 0.0f,     //(con, lin, exp)
	//	15.0f                 // ángulo en grados
	//);
	//spotLightCount++;

	////luz fija 
	//spotLights[1] = SpotLight(0.9f, 0.9f, 1.0f,
	//	1.0f, 2.0f,
	//	5.0f, 10.0f, 0.0f,
	//	-5.0f, 0.5f, 6.0f, //Posicion
	//	1.0f, 0.0f, 0.0f,
	//	15.0f);
	//spotLightCount++;
	
	//se crean mas luces puntuales y spotlight 

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	////Loop mientras no se cierra la ventana

	float cofreAngleDeg = 0.0f;            // ángulo actual
	const float COFRE_MAX_ANGLE = 60.0f;   // hasta dónde abre 
	const float COFRE_SPEED_DPS = 90.0f;   // velocidad de apertura/cierre

	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

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
		
		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		
		
		float carBaseX = 0.0f + mainWindow.getmuevex();
		float carBaseY = -0.8f;
		float carBaseZ = 0.0f;
		glm::vec3 carPos(carBaseX, carBaseY, carBaseZ);

		float yawCar = 0.0f * toRadians; //Rotacion de mis faros
		glm::vec3 forward(cosf(yawCar), 0.0f, sinf(yawCar));
		glm::vec3 back = -forward;

		float h = 0.4f;
		float dFront = 1.8f;
		float dBack = 1.6f;

		glm::vec3 frontPos = carPos + glm::vec3(0.0f, h, 0.0f) + forward * dFront;
		glm::vec3 backPos = carPos + glm::vec3(0.0f, h, 0.0f) + back * dBack;

		glm::vec3 frontDir = glm::normalize(forward + glm::vec3(0.0f, -0.15f, 0.0f));
		glm::vec3 backDir = glm::normalize(back + glm::vec3(0.0f, -0.15f, 0.0f));

		if (idxSpotFront >= 0) {
			spotLights[idxSpotFront].SetFlash(frontPos, frontDir);
		}
		if (idxSpotBack >= 0) {
			spotLights[idxSpotBack].SetFlash(backPos, backDir);
		}


		// PointLights activas
		PointLight activePL[MAX_POINT_LIGHTS];
		unsigned int activePLCount = 0;

		if (idxLampPL >= 0 && mainWindow.getLampOn()) {
			activePL[activePLCount++] = pointLights[idxLampPL];
		}
		if (idxModelPL >= 0 && mainWindow.getModelLightOn()) {
			activePL[activePLCount++] = pointLights[idxModelPL];
		}

		// SpotLights activas controladas por Y/U
		bool* keys = mainWindow.getsKeys();

		SpotLight activeSL[MAX_SPOT_LIGHTS];
		unsigned int activeSLCount = 0;

		if (idxSpotFront >= 0 && keys[GLFW_KEY_Y]) {
			activeSL[activeSLCount++] = spotLights[idxSpotFront];
		}
		if (idxSpotBack >= 0 && keys[GLFW_KEY_U]) {
			activeSL[activeSLCount++] = spotLights[idxSpotBack];
		}
		if (idxCofreSpot >= 0 ) {
			activeSL[activeSLCount++] = spotLights[idxCofreSpot];
		}

		// Envío al shader 
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(activePL, activePLCount);
		shaderList[0].SetSpotLights(activeSL, activeSLCount);



		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		/*shaderList[0].SetPointLights(pointLights, pointLightCount);*/
		
		
		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();

		//Instancia del coche 
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f + mainWindow.getmuevex(), -0.8f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Kitt_M.RenderModel();

		//Llanta delantera izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(-1.7f, 0.2f, 1.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		color = glm::vec3(0.5f, 0.5f, 0.5f);//llanta con color gris
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();

		//Llanta trasera izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(1.8f, 0.2f, 1.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();

		//Llanta delantera derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(-1.7f, 0.2f, -1.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();

		//Llanta trasera derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(1.8f, 0.2f, -1.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();

		//  Cofre 
		//
		// Animación suave de apertura/cierre
		float target = mainWindow.getCofreOpen() ? COFRE_MAX_ANGLE : 0.0f;
		if (cofreAngleDeg < target) {
			cofreAngleDeg = glm::min(cofreAngleDeg + COFRE_SPEED_DPS * deltaTime, target);
		}
		else if (cofreAngleDeg > target) {
			cofreAngleDeg = glm::max(cofreAngleDeg - COFRE_SPEED_DPS * deltaTime, target);
		}

		
		model = modelaux;
		model = glm::translate(model, glm::vec3(1.0f, 0.5f, 0.0f));  // posición base
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, -90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, 360.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, cofreAngleDeg * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));  // posición final

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Cofre.RenderModel();

		// Spotlight montado en la tapa del cofre borde frontal
		glm::vec4 cofreLocalPos(0.0f, 0.95f, -0.25f, 1.0f);    
		glm::vec3 cofreLocalDir(0.0f, -0.30f, -1.0f);          

		
		glm::vec3 cofreWorldPos = glm::vec3(model * cofreLocalPos);

		glm::vec3 cofreWorldDir = glm::normalize(glm::mat3(model) * cofreLocalDir);

		if (idxCofreSpot >= 0) {
			spotLights[idxCofreSpot].SetFlash(cofreWorldPos, cofreWorldDir);
		}



		
		//// DADO D8 (normales planas por cara)
		//model = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(-2.0f, 1.2f, 2.0f));
		//model = glm::scale(model, glm::vec3(1.2f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//color = glm::vec3(1.0f, 1.0f, 1.0f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		//dadoTexture.UseTexture();
		//Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//meshList[d8Index]->RenderMesh();



		//Lampara
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-5.0f, -1.0f, 6.0));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Lampara.RenderModel();

		// Pikachu
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-3.0f, -1.0f, -3.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Pikachu.RenderModel();
		


		/*model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f + mainWindow.getmuevex(), 5.0f, 6.0));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Blackhawk_M.RenderModel();*/

		//Agave ¿qué sucede si lo renderizan antes del coche y el helicóptero?
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, -4.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		
		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		glDisable(GL_BLEND);

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
