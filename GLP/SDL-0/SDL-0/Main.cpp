#include <SDL.h>
#include <glew.h>
#include <iostream>
#include <fstream>
#include <string>
#include "Window.h"
#include "Color.h"
#include "Shapes2D.h"
using namespace std;	

#define GLEW_STATIC

float updatePosX = 0.0f;
float updatePosY = 0.0f;
float paddleLOffsetX = -0.9f, paddleLPosY = 0.0f;
float paddleROffsetX = 0.85f, paddleRPosY = 0.0f;
float paddleSpeedY;
int playerPoints, aiPoints;
float speedX = 0.015, speedY = 0.01;
float maxX = 0.05f;
float maxY = 0.08f;

string LoadShader(string fileName);
bool leftPaddleCollision();
bool rightPaddleCollision();

int main(int argc, char* argv[])
{

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

	Window window(width, height, Color(0.0f, 0.0f, 0.2f, 1.0f));

	//vector<float> temp;
	//Shape2D::CreateRectangle(temp, Vector2(0, 0), Vector2(0.5, 0.2));
	//Shape2D::CreateRectangle(temp, Vector2(0,0), Vector2(0.2, 0.5));

	//
	/*float* vertices = new float[temp.size()];
	std::copy(temp.begin(), temp.end(), vertices);*/

	float vertices[] = {
			0.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
			0.0f, 0.08f, 0.0f,  0.0f, 1.0f, 0.0f,
			0.05f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
			0.05f, 0.08f, 0.0f,  0.0f, 0.0f, 1.0f,
			
			0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			0.05f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
			0.0f, 0.4f, 0.0f, 1.0f, 0.0f, 0.0f,
			0.05f, 0.4f, 0.0f,  0.0f, 1.0f, 0.0f,

			0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			0.05f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
			0.0f, 0.4f, 0.0f, 1.0f, 0.0f, 0.0f,
			0.05f, 0.4f, 0.0f,  0.0f, 1.0f, 0.0f,
	};

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
		//// Color attribute
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
		if (SDL_PollEvent(&event)) {
			if (event.type == SDL_KEYUP) {
				switch (event.key.keysym.sym) {
				case SDLK_z:
					paddleSpeedY = 0;
					break;
				case SDLK_s:
					paddleSpeedY = 0;
					break;
				case SDLK_UP:
					paddleSpeedY = 0;
					break;
				case SDLK_DOWN:
					paddleSpeedY = 0;
					break;
				default:
					break;
				}
			}
			if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) {
				case SDLK_z:
					if (paddleLPosY + 0.4 <= 1) paddleSpeedY = 0.01f;
					break;
				case SDLK_s:
					if (paddleLPosY >= -1) paddleSpeedY = -0.01f;
					break;
				case SDLK_UP:
					if (paddleLPosY + 0.4 <= 1) paddleSpeedY = 0.01f;
					break;
				case SDLK_DOWN:
					if (paddleLPosY >= -1) paddleSpeedY = -0.01f;
					break;
				default:
					break;
				}
			}
		}
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the screen


		glBindVertexArray(vao);
		glUseProgram(shaderProgram);


		//Move Ball
		updatePosX += speedX;
		updatePosY += speedY;
		if (updatePosY + maxY >= 1) speedY *= -1;
		if (updatePosY <= -1) speedY *= -1;

		//Check if paddle on screen and move left paddle
		if ((paddleLPosY + 0.4 < 1) && paddleSpeedY == 0.01f)  paddleLPosY += paddleSpeedY;
		if ((paddleLPosY >= -1) && paddleSpeedY == -0.01f)  paddleLPosY += paddleSpeedY;

		//Move AI Right Paddle
		if (updatePosY + 0.25f < 1 && updatePosY - 0.15f > -1) paddleRPosY = updatePosY - 0.15f;

		//Check Hit
		if (leftPaddleCollision()) {
			updatePosX = paddleLOffsetX + 0.05f;
			speedX = -speedX;
		}

		if (rightPaddleCollision()) {
			updatePosX = paddleROffsetX - 0.05f;
			speedX = -speedX;
		}

		//Check Victory Condition) 
		if (updatePosX <= -1.0f) { aiPoints++; updatePosX = 0; speedX = -speedX; }
		if (updatePosX + 0.05f >= 1.0f) { playerPoints++; updatePosX = 0; }
		if (playerPoints >= 5) window.Close();
		if (aiPoints >= 5) window.Close();


		int location = glGetUniformLocation(shaderProgram, "updatePos");
		glUniform2f(location, updatePosX, updatePosY);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glUniform2f(location, paddleLOffsetX, paddleLPosY);
		glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);

		glUniform2f(location, paddleROffsetX, paddleRPosY);
		glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
		//Shape2D::DrawRectangle(0);
		window.Update();


	}
	// Quit
	window.Close();


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


// rectA = left paddle
// rectB = ball

bool leftPaddleCollision() {
	float xMinA = paddleLOffsetX ;
	float xMaxA = paddleLOffsetX + 0.05f;
	float yMinA = paddleLPosY;
	float yMaxA = paddleLPosY + 0.4f;
	float xMinB = updatePosX;
	float xMaxB = updatePosX + 0.05f;
	float yMinB = updatePosY;
	float yMaxB = updatePosY + 0.08f;
	return!(xMinA > xMaxB || xMaxA < xMinB || yMinA > yMaxB || yMaxA < yMinB);
}

bool rightPaddleCollision() {
	float xMinA = paddleROffsetX;
	float xMaxA = paddleROffsetX + 0.05f;
	float yMinA = paddleRPosY;
	float yMaxA = paddleRPosY + 0.4f;
	float xMinB = updatePosX;
	float xMaxB = updatePosX + 0.05f;
	float yMinB = updatePosY;
	float yMaxB = updatePosY + 0.08f;
	return!(xMinA > xMaxB || xMaxA < xMinB || yMinA > yMaxB || yMaxA < yMinB);
}