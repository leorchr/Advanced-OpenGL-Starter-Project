#include <SDL.h>
#include <glew.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include "Window.h"
#include "Color.h"
#include "Shapes2D.h"
#include "Timer.h"
using namespace std;	

#define GLEW_STATIC

// Ball
float ballPosX = 0.0f;
float ballPosY = 0.0f;
const float sizeBallX = 0.05f, sizeBallY = 0.08f;

// Paddles
float paddleLOffsetX = -0.9f, paddleLPosY = 0.0f;
float paddleROffsetX = 0.85f, paddleRPosY = 0.0f;
const float sizePaddleX = 0.05f, sizePaddleY = 0.4f;
float paddleSpeedY;
float speedX = 0.01f, speedY = 0.01f;

// Score
int playerPoints, aiPoints;

// Functions
string LoadShader(string fileName);
bool LeftPaddleCollision();
bool RightPaddleCollision();
void MoveBall(float dt);
void RandomizeBallYSpeed();

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

	//Tentative utilisation Shapes2D.h
	//vector<float> temp;
	//Shape2D::CreateRectangle(temp, Vector2(0, 0), Vector2(0.5, 0.2));
	//Shape2D::CreateRectangle(temp, Vector2(0,0), Vector2(0.2, 0.5));

	//
	/*float* vertices = new float[temp.size()];
	std::copy(temp.begin(), temp.end(), vertices);*/

	float vertices[] = {
			0.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
			0.0f, sizeBallY, 0.0f,  0.0f, 1.0f, 0.0f,
			sizeBallX, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
			sizeBallX, sizeBallY, 0.0f,  0.0f, 0.0f, 1.0f,
			
			0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			sizePaddleX, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
			0.0f, sizePaddleY, 0.0f, 1.0f, 0.0f, 0.0f,
			sizePaddleX, sizePaddleY, 0.0f,  0.0f, 1.0f, 0.0f,

			0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			sizePaddleX, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
			0.0f, sizePaddleY, 0.0f, 1.0f, 0.0f, 0.0f,
			sizePaddleX, sizePaddleY, 0.0f,  0.0f, 1.0f, 0.0f,
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
	glClearColor(1.0f, 0.8f, 1.0f, 1.0f);

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
	Timer timer;
	float dt = 0;
	while (isRunning) {
		float dt = timer.computeDeltaTime() / 10.0f;
		// Inputs
		SDL_Event event;
		if (SDL_PollEvent(&event)) {
			if (event.type == SDL_KEYUP) {
				switch (event.key.keysym.sym) {
				case SDLK_z:
					if (paddleSpeedY == 0.01f) paddleSpeedY = 0;
					break;
				case SDLK_s:
					if (paddleSpeedY == -0.01f) paddleSpeedY = 0;
					break;
				case SDLK_UP:
					if (paddleSpeedY == 0.01f) paddleSpeedY = 0;
					break;
				case SDLK_DOWN:
					if(paddleSpeedY == -0.01f) paddleSpeedY = 0;
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

		MoveBall(dt);

		//Check if paddle on screen and move player paddle
		if ((paddleLPosY + sizePaddleY < 1) && paddleSpeedY > 0)  paddleLPosY += paddleSpeedY * dt;
		if ((paddleLPosY >= -1) && paddleSpeedY < 0)  paddleLPosY += paddleSpeedY * dt;

		//Move AI Right Paddle
		if (ballPosY + sizeBallY/2 + sizePaddleY/2 < 1 && ballPosY + sizeBallY/2 - sizePaddleY/2 > -1) paddleRPosY = ballPosY + sizeBallY/2 - sizePaddleY/2;

		//Check Victory Condition 
		if (ballPosX <= -1.0f) { aiPoints++; ballPosX = 0; speedX = -speedX; }
		if (ballPosX + sizeBallX >= 1.0f) { playerPoints++; ballPosX = 0; }
		if (playerPoints >= 5) window.Close(); // Cette ligne de code ne sert à rien, tu ne peux pas gagner de toute façon
		if (aiPoints >= 5) {
			cout << "Bouhouhou tu as perdu ! De toute facon cette IA est imbattable.\n";
			isRunning = false;
		}

		int location = glGetUniformLocation(shaderProgram, "updatePos");
		glUniform2f(location, ballPosX, ballPosY);
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

bool LeftPaddleCollision() {
	//Check collisions between left paddle and ball
	float xMinA = paddleLOffsetX ;
	float xMaxA = paddleLOffsetX + sizePaddleX;
	float yMinA = paddleLPosY;
	float yMaxA = paddleLPosY + sizePaddleY;
	float xMinB = ballPosX;
	float xMaxB = ballPosX + sizeBallX;
	float yMinB = ballPosY;
	float yMaxB = ballPosY + sizeBallY;
	return!(xMinA > xMaxB || xMaxA < xMinB || yMinA > yMaxB || yMaxA < yMinB);
}

bool RightPaddleCollision() {
	//Check collisions between right paddle and ball
	float xMinA = paddleROffsetX;
	float xMaxA = paddleROffsetX + sizePaddleX;
	float yMinA = paddleRPosY;
	float yMaxA = paddleRPosY + sizePaddleY;
	float xMinB = ballPosX;
	float xMaxB = ballPosX + sizeBallX;
	float yMinB = ballPosY;
	float yMaxB = ballPosY + sizeBallY;
	return!(xMinA > xMaxB || xMaxA < xMinB || yMinA > yMaxB || yMaxA < yMinB);
}

void MoveBall(float dt) {
	ballPosX += speedX * dt;
	ballPosY += speedY * dt;
	if (ballPosY + sizeBallY > 1) speedY *= -1;
	if (ballPosY < -1) speedY *= -1;

	//Check collisions
	if (LeftPaddleCollision()) {
		ballPosX = paddleLOffsetX + sizePaddleX;
		RandomizeBallYSpeed();
		speedX = -speedX;
	}

	if (RightPaddleCollision()) {
		ballPosX = paddleROffsetX - sizeBallX;
		RandomizeBallYSpeed();
		speedX = -speedX;
	}
}

void RandomizeBallYSpeed() {
	// Y speed and direction randomizer
	srand(time(NULL));
	if (rand() % 2 == 1) speedY = ((rand() % 5 + 11.0f) / 1000.0f);
	if (rand() % 2 == 0) speedY = -((rand() % 5 + 11.0f) / 1000.0f);
}