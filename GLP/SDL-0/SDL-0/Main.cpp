﻿#include <SDL.h>
#include <glew.h>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;	

#define GLEW_STATIC

string LoadShader(string fileName);

int main(int argc, char* argv[])
{
	float updatePosX = 0.0f;
	float updatePosY = 0.0f;
	float speedX = 0.02, speedY = 0.01;
	float maxX = 0.05f;
	float maxY = 0.08f;

	float vertices[] = {
			0.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
			0.0f, 0.08f, 0.0f,  0.0f, 1.0f, 0.0f,
			0.05f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
			0.05f, 0.08f, 0.0f,  0.0f, 0.0f, 1.0f
	};

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		cout << "SDL initialization failed. SDL Error: " << SDL_GetError();
	}
	else
	{
		cout << "SDL initialization succeeded!\n";
	}
	///////////SETTING UP SDL/////////////
	//Create a simple window

	int width = 800;
	int height = 500;
	unsigned int center = SDL_WINDOWPOS_CENTERED;
	SDL_Window* Window = SDL_CreateWindow("My window", center, center, width, height, SDL_WINDOW_OPENGL);
	//SDL_WINDOW_OPENGL is a u32 flag !


	//Create an OpenGL compatible context to let glew draw on it
	SDL_GLContext Context = SDL_GL_CreateContext(Window);

	/////////SETTING UP OPENGL WITH GLEW///
	//Initialize glew
	glewExperimental = GL_TRUE;
	if (glewInit() == GLEW_OK) {
		cout << "Glew initialized successfully\n";
	}


	//Set the viewing frame through which we will see the objects
	glViewport(0, 0, width, height);

	//Put the color you want here for the background
	glClearColor(0.9f, 0.7f, 0.9f, 1.0f);

	//Create an ID to be given at object generation
	unsigned int vbo;
	unsigned int shaderProgram;
	{

		//Pass how many buffers should be created and the reference of the ID to get the value set
		glGenBuffers(1, &vbo);

		string vs = LoadShader("Vertex_Shader.shader");
		const char* vertexShaderSource = vs.c_str();
		string fs = LoadShader("RGB_Frag.shader");
		const char* fragmentShaderSource = fs.c_str();


		unsigned int vertexShader;
		vertexShader = glCreateShader(GL_VERTEX_SHADER);


		//now that we have a vertex shader, let’s put the code text inside
		glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);


		//aaaaand… Compile !
		glCompileShader(vertexShader);


		//Do the same with the fragment shader !
		unsigned int fragmentShader;
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
		glCompileShader(fragmentShader);

		shaderProgram = glCreateProgram();



		//now attach shaders to use to the program
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);

		//and link it 
		glLinkProgram(shaderProgram);

		//now that the program is complete, we can use it 
		glUseProgram(shaderProgram);

	}

	//Create one ID to be given at object generation
	unsigned int vao;
	{
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);


		//Binds the buffer linked to this ID to the vertex array buffer to be rendered. Put 0 instead of vbo to reset the value.
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		//Finally send the vertices array in the array buffer 
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// Position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// Color attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}

	//Use depth management
	glEnable(GL_DEPTH_TEST);

	//0 is our origin, the higher the z, the farther the object
	glDepthFunc(GL_LESS);

	bool isRunning = true;
	while (isRunning) {
		// Inputs
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				isRunning = false;
				break;
			default:
				break;
			}
		}
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the screen


		updatePosX += speedX;
		updatePosY += speedY;
		if (updatePosX + maxX >= 1) speedX *= -1;
		if (updatePosX <= -1) speedX *= -1;
		if (updatePosY + maxY >= 1) speedY *= -1;
		if (updatePosY <= -1) speedY *= -1;

		int location = glGetUniformLocation(shaderProgram, "updatePos");
		glUniform2f(location, updatePosX, updatePosY);

		glUseProgram(shaderProgram);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		SDL_GL_SwapWindow(Window); // Swapbuffer


	}
	// Quit
	SDL_DestroyWindow(Window);
	SDL_GL_DeleteContext(Context);


	cin.get();
	return 0;
}

string LoadShader(string fileName) {
	ifstream myFile;
	myFile.open(fileName);
	if (myFile.fail()) {
		cerr << "Error - failed to open " << fileName << endl;
	}
	string fileText = "";
	string line = "";
	while (getline(myFile, line)) {
		fileText += line + '\n';
	}

	myFile.close();
	return fileText;
}
