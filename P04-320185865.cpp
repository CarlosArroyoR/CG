/*Práctica 4: Modelado Jerárquico.
Se implementa el uso de matrices adicionales para almacenar información de transformaciones geométricas que se quiere
heredar entre diversas instancias para que estén unidas
Teclas de la F a la K para rotaciones de articulaciones
*/
#include <stdio.h>
#include <string.h>
#include<cmath>
#include<vector>
#include <glew.h>
#include <glfw3.h>
//glm
#include<glm.hpp>
#include<gtc\matrix_transform.hpp>
#include<gtc\type_ptr.hpp>
#include <gtc\random.hpp>
//clases para dar orden y limpieza al còdigo
#include"Mesh.h"
#include"Shader.h"
#include"Sphere.h"
#include"Window.h"
#include"Camera.h"
//tecla E: Rotar sobre el eje X
//tecla R: Rotar sobre el eje Y
//tecla T: Rotar sobre el eje Z


using std::vector;

//Dimensiones de la ventana
const float toRadians = 3.14159265f / 180.0; //grados a radianes
const float PI = 3.14159265f;
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;
Camera camera;
Window mainWindow;
vector<Mesh*> meshList;
vector<Shader>shaderList;
//Vertex Shader
static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";
Sphere sp = Sphere(1.0, 20, 20); //recibe radio, slices, stacks




void CrearCubo()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// left
		4, 0, 3,
		3, 7, 4,
		// bottom
		4, 5, 1,
		1, 0, 4,
		// top
		3, 2, 6,
		6, 7, 3
	};

	GLfloat cubo_vertices[] = {
		// front
		-0.5f, -0.5f,  0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		// back
		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f
	};
	Mesh* cubo = new Mesh();
	cubo->CreateMesh(cubo_vertices, cubo_indices, 24, 36);
	meshList.push_back(cubo);
}

// Pirámide triangular regular
void CrearPiramideTriangular()
{
	unsigned int indices_piramide_triangular[] = {
			0,1,2,
			1,3,2,
			3,0,2,
			1,0,3

	};
	GLfloat vertices_piramide_triangular[] = {
		-0.5f, -0.5f,0.0f,	//0
		0.5f,-0.5f,0.0f,	//1
		0.0f,0.5f, -0.25f,	//2
		0.0f,-0.5f,-0.5f,	//3

	};
	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices_piramide_triangular, indices_piramide_triangular, 12, 12);
	meshList.push_back(obj1);

}
/*
Crear cilindro y cono con arreglos dinámicos vector creados en el Semestre 2023 - 1 : por Sánchez Pérez Omar Alejandro
*/
void CrearCilindro(int res, float R) {

	//constantes utilizadas en los ciclos for
	int n, i;
	//cálculo del paso interno en la circunferencia y variables que almacenarán cada coordenada de cada vértice
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;

	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	//ciclo for para crear los vértices de las paredes del cilindro
	for (n = 0; n <= (res); n++) {
		if (n != res) {
			x = R * cos((n)*dt);
			z = R * sin((n)*dt);
		}
		//caso para terminar el círculo
		else {
			x = R * cos((0) * dt);
			z = R * sin((0) * dt);
		}
		for (i = 0; i < 6; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(y);
				break;
			case 2:
				vertices.push_back(z);
				break;
			case 3:
				vertices.push_back(x);
				break;
			case 4:
				vertices.push_back(0.5);
				break;
			case 5:
				vertices.push_back(z);
				break;
			}
		}
	}

	//ciclo for para crear la circunferencia inferior
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(-0.5f);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}

	//ciclo for para crear la circunferencia superior
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(0.5);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}

	//Se generan los indices de los vértices
	for (i = 0; i < vertices.size(); i++) indices.push_back(i);

	//se genera el mesh del cilindro
	Mesh* cilindro = new Mesh();
	cilindro->CreateMeshGeometry(vertices, indices, vertices.size(), indices.size());
	meshList.push_back(cilindro);
}

//función para crear un cono
void CrearCono(int res, float R) {

	//constantes utilizadas en los ciclos for
	int n, i;
	//cálculo del paso interno en la circunferencia y variables que almacenarán cada coordenada de cada vértice
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;

	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	//caso inicial para crear el cono
	vertices.push_back(0.0);
	vertices.push_back(0.5);
	vertices.push_back(0.0);

	//ciclo for para crear los vértices de la circunferencia del cono
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(y);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}
	vertices.push_back(R * cos(0) * dt);
	vertices.push_back(-0.5);
	vertices.push_back(R * sin(0) * dt);


	for (i = 0; i < res + 2; i++) indices.push_back(i);

	//se genera el mesh del cono
	Mesh* cono = new Mesh();
	cono->CreateMeshGeometry(vertices, indices, vertices.size(), res + 2);
	meshList.push_back(cono);
}

//función para crear pirámide cuadrangular unitaria
void CrearPiramideCuadrangular()
{
	vector<unsigned int> piramidecuadrangular_indices = {
		0,3,4,
		3,2,4,
		2,1,4,
		1,0,4,
		0,1,2,
		0,2,4

	};
	vector<GLfloat> piramidecuadrangular_vertices = {
		0.5f,-0.5f,0.5f,
		0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,0.5f,
		0.0f,0.5f,0.0f,
	};
	Mesh* piramide = new Mesh();
	piramide->CreateMeshGeometry(piramidecuadrangular_vertices, piramidecuadrangular_indices, 15, 18);
	meshList.push_back(piramide);
}



void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);

}


//int main() { /////////////////////////////////////////////////////////MAIN DE LA GRUA//////////////////////////////////////////////////////////////
//	mainWindow = Window(800, 600);
//	mainWindow.Initialise();
//
//	CrearCubo();                 // meshList[0]
//	CrearPiramideTriangular();   // meshList[1]
//	CrearCilindro(16, 1.0f);      // meshList[2]
//	CrearCono(25, 2.0f);         // meshList[3]
//	CrearPiramideCuadrangular(); // meshList[4]
//	CreateShaders();
//
//	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f),
//		glm::vec3(0.0f, 1.0f, 0.0f),
//		-60.0f, 0.0f, 2.0f, 0.5f);
//
//	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformColor = 0;
//
//	float aspect = (float)mainWindow.getBufferWidth() / (float)mainWindow.getBufferHeight();
//	glm::mat4 projection = glm::perspective(glm::radians(60.0f), aspect, 0.1f, 100.0f);
//
//	glEnable(GL_DEPTH_TEST);
//
//	sp.init();
//	sp.load();
//
//	glm::vec3 color(0.0f, 0.0f, 0.0f);
//
//	float deltaTime = 0.0f;
//	float lastTime = 0.0f;
//
//	while (!mainWindow.getShouldClose()) {
//		float now = (float)glfwGetTime();
//		deltaTime = now - lastTime;
//		lastTime = now;
//
//		glfwPollEvents();
//		camera.keyControl(mainWindow.getsKeys(), deltaTime);
//		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
//
//		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//		shaderList[0].useShader();
//		uniformModel = shaderList[0].getModelLocation();
//		uniformProjection = shaderList[0].getProjectLocation();
//		uniformView = shaderList[0].getViewLocation();
//		uniformColor = shaderList[0].getColorLocation();
//
//		glm::mat4 root(1.0f);
//		root = glm::translate(root, glm::vec3(0.0f, 5.5f, -4.0f));
//
//		glm::mat4 model(1.0f);
//		glm::mat4 modelaux(1.0f);
//		glm::mat4 otromodelo(1.0f);
//
//		// cabina
//		model = root;
//		modelaux = model;
//		model = glm::scale(model, glm::vec3(5.0f, 3.0f, 3.0f));
//		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
//		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
//		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
//		color = glm::vec3(0.85f, 0.85f, 0.90f);
//		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
//		meshList[0]->RenderMesh();
//
//		// articulación 1
//		model = modelaux;
//		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion1()), glm::vec3(0.0f, 0.0f, 1.0f));
//		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
//		color = glm::vec3(1.0f, 0.0f, 0.0f);
//		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
//		sp.render();
//
//		// brazo 1
//		model = glm::rotate(model, glm::radians(135.0f), glm::vec3(0.0f, 0.0f, 1.0f));
//		model = glm::translate(model, glm::vec3(2.5f, 0.0f, 0.0f));
//		modelaux = model;
//		model = glm::scale(model, glm::vec3(5.0f, 1.0f, 1.0f));
//		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
//		color = glm::vec3(1.0f, 0.0f, 1.0f);
//		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
//		meshList[0]->RenderMesh();
//
//		// articulación 2
//		model = modelaux;
//		model = glm::translate(model, glm::vec3(2.5f, 0.0f, 0.0f));
//		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion2()), glm::vec3(0.0f, 0.0f, 1.0f));
//		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
//		sp.render();
//
//		// brazo 2
//		model = glm::translate(model, glm::vec3(0.0f, -2.5f, 0.0f));
//		modelaux = model;
//		model = glm::scale(model, glm::vec3(1.0f, 5.0f, 1.0f));
//		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
//		color = glm::vec3(0.0f, 1.0f, 0.0f);
//		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
//		meshList[0]->RenderMesh();
//
//		// articulación 3
//		model = modelaux;
//		model = glm::translate(model, glm::vec3(0.0f, -2.5f, 0.0f));
//		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion3()), glm::vec3(0.0f, 0.0f, 1.0f));
//		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
//		sp.render();
//
//		// brazo 3
//		model = glm::translate(model, glm::vec3(0.0f, -2.5f, 0.0f));
//		modelaux = model;
//		model = glm::scale(model, glm::vec3(1.0f, 5.0f, 1.0f));
//		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
//		color = glm::vec3(0.0f, 1.0f, 1.0f);
//		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
//		meshList[0]->RenderMesh();
//
//		// articulación 4
//		model = modelaux;
//		model = glm::translate(model, glm::vec3(0.0f, -2.5f, 0.0f));
//		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion4()), glm::vec3(1.0f, 0.0f, 0.0f));
//		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
//		sp.render();
//
//		// canasta
//		model = glm::translate(model, glm::vec3(0.0f, -2.5f, 0.0f));
//		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
//		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
//		color = glm::vec3(1.0f, 1.0f, 1.0f);
//		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
//		meshList[0]->RenderMesh();
//
//		// base
//		otromodelo = root;
//		otromodelo = glm::translate(otromodelo, glm::vec3(0.0f, -1.5f, 0.0f));
//		otromodelo = glm::scale(otromodelo, glm::vec3(5.0f, 3.0f, 3.5f));
//		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(otromodelo));
//		color = glm::vec3(0.6f, 0.6f, 0.6f);
//		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
//		meshList[4]->RenderMesh();
//
//		// rueda izquierda trasera
//		glm::mat4 rueda = root;
//		rueda = glm::translate(rueda, glm::vec3(+2.0f, -2.5f, +2.0f));
//		rueda = glm::rotate(rueda, glm::radians(90.0f), glm::vec3(0, 0, 1));
//		rueda = glm::rotate(rueda, glm::radians(90.0f), glm::vec3(1, 0, 0));
//		rueda = glm::rotate(rueda, glm::radians(mainWindow.getarticulacion5()), glm::vec3(0, 1, 0)); // giro con K
//		rueda = glm::scale(rueda, glm::vec3(1.0f, 0.7f, 1.0f));
//		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(rueda));
//		color = glm::vec3(0.1f, 0.1f, 0.1f);
//		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
//		meshList[2]->RenderMeshGeometry();
//
//		// rueda derecha trasera
//		glm::mat4 rueda2 = root;
//		rueda2 = glm::translate(rueda2, glm::vec3(-2.0f, -2.5f, +2.0f));
//		rueda2 = glm::rotate(rueda2, glm::radians(90.0f), glm::vec3(0, 0, 1));
//		rueda2 = glm::rotate(rueda2, glm::radians(90.0f), glm::vec3(1, 0, 0));
//		rueda2 = glm::rotate(rueda2, glm::radians(mainWindow.getarticulacion6()), glm::vec3(0, 1, 0));
//		rueda2 = glm::scale(rueda2, glm::vec3(1.0f, 0.7f, 1.0f));
//		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(rueda2));
//		color = glm::vec3(0.1f, 0.1f, 0.1f);
//		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
//		meshList[2]->RenderMeshGeometry();
//
//		// rueda izquierda delantera
//		glm::mat4 rueda3 = root;
//		rueda3 = glm::translate(rueda3, glm::vec3(+2.0f, -2.5f, -2.0f));
//		rueda3 = glm::rotate(rueda3, glm::radians(90.0f), glm::vec3(0, 0, 1));
//		rueda3 = glm::rotate(rueda3, glm::radians(90.0f), glm::vec3(1, 0, 0));
//		rueda3 = glm::rotate(rueda3, glm::radians(mainWindow.getarticulacion4()), glm::vec3(0, 1, 0));
//		rueda3 = glm::scale(rueda3, glm::vec3(1.0f, 0.7f, 1.0f));
//		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(rueda3));
//		color = glm::vec3(0.1f, 0.1f, 0.1f);
//		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
//		meshList[2]->RenderMeshGeometry();
//
//		// rueda derecha delantera
//		glm::mat4 rueda4 = root;
//		rueda4 = glm::translate(rueda4, glm::vec3(-2.0f, -2.5f, -2.0f));
//		rueda4 = glm::rotate(rueda4, glm::radians(90.0f), glm::vec3(0, 0, 1));
//		rueda4 = glm::rotate(rueda4, glm::radians(90.0f), glm::vec3(1, 0, 0));
//		rueda4 = glm::rotate(rueda4, glm::radians(mainWindow.getarticulacion3()), glm::vec3(0, 1, 0));
//		rueda4 = glm::scale(rueda4, glm::vec3(1.0f, 0.7f, 1.0f));
//		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(rueda4));
//		color = glm::vec3(0.1f, 0.1f, 0.1f);
//		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
//		meshList[2]->RenderMeshGeometry();
//
//
//		glUseProgram(0);
//		mainWindow.swapBuffers();
//	}
//	return 0;
//}

int main() {
	// ventana
	mainWindow = Window(800, 600);
	mainWindow.Initialise();

	// geometrías
	CrearCubo();              // meshList[0] = cubo
	CrearCilindro(48, 0.55f); // meshList[1] = cilindro
	CreateShaders();

	// cámara
	camera = Camera(glm::vec3(0.0f, 1.2f, 6.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		-90.0f, 0.0f, 3.0f, 0.8f);

	GLuint uProj = 0, uModel = 0, uView = 0, uColor = 0;
	float aspecto = (float)mainWindow.getBufferWidth() / (float)mainWindow.getBufferHeight();
	glm::mat4 proy = glm::perspective(glm::radians(60.0f), aspecto, 0.1f, 100.0f);

	glEnable(GL_DEPTH_TEST);

	glm::vec3 color(1, 1, 1);
	float dt = 0.0f, last = 0.0f;

	while (!mainWindow.getShouldClose()) {
		float now = (float)glfwGetTime();
		dt = now - last; last = now;

		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), dt);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		glClearColor(0.05f, 0.06f, 0.08f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderList[0].useShader();
		uModel = shaderList[0].getModelLocation();
		uProj = shaderList[0].getProjectLocation();
		uView = shaderList[0].getViewLocation();
		uColor = shaderList[0].getColorLocation();

		glUniformMatrix4fv(uProj, 1, GL_FALSE, glm::value_ptr(proy));
		glUniformMatrix4fv(uView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

		glm::mat4 root(1.0f);
		root = glm::translate(root, glm::vec3(0.0f, 1.8f, -6.0f));

		glm::mat4 model(1.0f), base(1.0f);

		// torso
		model = root; base = model;
		model = glm::scale(model, glm::vec3(6.0f, 2.6f, 3.2f));
		glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.92f, 0.92f, 0.92f); glUniform3fv(uColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		float hx = 3.0f, hy = 1.3f, hz = 1.6f;

		// ángulos (F/G/H/J/K)
		float angCabeza = mainWindow.getarticulacion1();
		float angSup = mainWindow.getarticulacion2();
		float angInf = mainWindow.getarticulacion3();
		float angCola1 = mainWindow.getarticulacion4();
		float angCola2 = mainWindow.getarticulacion5();

		// cabeza
		model = base;
		model = glm::translate(model, glm::vec3(+hx + 1.0f, +hy - 0.2f, 0.0f));
		model = glm::rotate(model, glm::radians(-angCabeza), glm::vec3(0, 1, 0));
		model = glm::scale(model, glm::vec3(1.9f, 1.6f, 1.6f));
		glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.97f, 0.97f, 0.97f); glUniform3fv(uColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		// patas (dos segmentos)
		float largoSup = 1.6f, largoInf = 1.6f, radio = 0.55f;

		auto pata = [&](glm::vec3 p) {
			glm::mat4 cadera = base;
			cadera = glm::translate(cadera, p);
			cadera = glm::rotate(cadera, glm::radians(angSup), glm::vec3(0, 0, 1)); // igual que patas

			glm::mat4 muslo = cadera;
			muslo = glm::translate(muslo, glm::vec3(0, -largoSup * 0.5f, 0));
			muslo = glm::scale(muslo, glm::vec3(radio, largoSup, radio));
			glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(muslo));
			color = glm::vec3(0.15f, 0.15f, 0.15f); glUniform3fv(uColor, 1, glm::value_ptr(color));
			meshList[1]->RenderMeshGeometry();

			glm::mat4 rodilla = cadera;
			rodilla = glm::translate(rodilla, glm::vec3(0, -largoSup, 0));
			rodilla = glm::rotate(rodilla, glm::radians(angInf), glm::vec3(0, 0, 1)); // mismo eje

			glm::mat4 pierna = rodilla;
			pierna = glm::translate(pierna, glm::vec3(0, -largoInf * 0.5f, 0));
			pierna = glm::scale(pierna, glm::vec3(radio, largoInf, radio));
			glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(pierna));
			color = glm::vec3(0.18f, 0.18f, 0.18f); glUniform3fv(uColor, 1, glm::value_ptr(color));
			meshList[1]->RenderMeshGeometry();
			};

		pata(glm::vec3(+hx - 0.5f, -hy, -hz + 0.55f));
		pata(glm::vec3(+hx - 0.5f, -hy, +hz - 0.55f));
		pata(glm::vec3(-hx + 0.5f, -hy, -hz + 0.55f));
		pata(glm::vec3(-hx + 0.5f, -hy, +hz - 0.55f));

		// cola articulada (mismo eje que patas: Z)
		float l1 = 1.5f, l2 = 1.2f, rCola = 0.35f, inclCola = 15.0f;

		glm::mat4 colaBase = base;
		colaBase = glm::translate(colaBase, glm::vec3(-hx - 0.15f, +hy - 0.4f, 0.0f));
		colaBase = glm::rotate(colaBase, glm::radians(inclCola), glm::vec3(0, 0, 1));     // inclinada
		colaBase = glm::rotate(colaBase, glm::radians(angCola1), glm::vec3(0, 0, 1));     // giro como patas

		glm::mat4 cola1 = colaBase;
		cola1 = glm::translate(cola1, glm::vec3(-l1 * 0.5f, 0.0f, 0.0f));
		cola1 = glm::scale(cola1, glm::vec3(l1, rCola, rCola));
		glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(cola1));
		color = glm::vec3(0.20f, 0.20f, 0.20f); glUniform3fv(uColor, 1, glm::value_ptr(color));
		meshList[0]->RenderMesh();

		glm::mat4 colaMedia = colaBase;
		colaMedia = glm::translate(colaMedia, glm::vec3(-l1, 0.0f, 0.0f));
		colaMedia = glm::rotate(colaMedia, glm::radians(angCola2), glm::vec3(0, 0, 1));   // mismo eje Z

		glm::mat4 cola2 = colaMedia;
		cola2 = glm::translate(cola2, glm::vec3(-l2 * 0.5f, 0.0f, 0.0f));
		cola2 = glm::scale(cola2, glm::vec3(l2, rCola * 0.9f, rCola * 0.9f));
		glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(cola2));
		meshList[0]->RenderMesh();

		glUseProgram(0);
		mainWindow.swapBuffers();
	}
	return 0;
}







