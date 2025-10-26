#pragma once
#include<stdio.h>
#include<glew.h>
#include<glfw3.h>

class Window
{
public:
	Window();
	Window(GLint windowWidth, GLint windowHeight);
	int Initialise();
	GLfloat getBufferWidth() { return bufferWidth; }
	GLfloat getBufferHeight() { return bufferHeight; }
	GLfloat getXChange();
	GLfloat getYChange();
	GLfloat getmuevex() { return muevex; }
	bool getCofreOpen() const { return cofreOpen; }
	bool getLampOn() const { return lampOn; } //Para lampara

	bool getModelLightOn() const { return modelLightOn; } // luz puntual del modelo
	bool getSpotFrontOn() const { return spotFrontOn; }   // spotlight delantero del coche
	bool getSpotBackOn()  const { return spotBackOn; }    // spotlight trasero del coche

	bool getShouldClose() {
		return  glfwWindowShouldClose(mainWindow);}
	bool* getsKeys() { return keys; }
	void swapBuffers() { return glfwSwapBuffers(mainWindow); }
	
	~Window();
private: 
	GLFWwindow *mainWindow;
	GLint width, height;
	bool keys[1024];
	GLint bufferWidth, bufferHeight;
	void createCallbacks();
	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;
	GLfloat muevex;
	bool lampOn; //Para lampara
	bool cofreOpen;		//para rotacion del cofre
	bool modelLightOn;   // para la luz puntual del modelo elegido
	bool spotFrontOn;    // spotlight delantero del coche
	bool spotBackOn;     // spotlight trasero del coche

	bool mouseFirstMoved;
	static void ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode);
	static void ManejaMouse(GLFWwindow* window, double xPos, double yPos);

};

