#include "MipmapScene.h"
using namespace std;

const int POSITION = 0;
const int POSITION_SIZE = 3; // 3 position infos per vertex

MipmapScene::MipmapScene() {
	m_vertices = {};
}

//Load the correct shaders from your files

void MipmapScene::LoadShaders() {
	m_vertexShader.LoadFrom("mipmapVertex.shader", VERTEX);
	m_fragmentShader.LoadFrom("simpleFragment.shader", FRAGMENT);
}

void MipmapScene::CreateShaderPrograms() {
	m_program.Compose(vector<Shader*>{&m_vertexShader, & m_fragmentShader});
	m_program.Use();
}

void MipmapScene::VerticeInformationSlicer() {
	//Put the vector information in a float array, then in the array buffer 
	float* verticeArray = new float[m_vertices.size()]();
	std::copy(m_vertices.begin(), m_vertices.end(), verticeArray);

	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(float), verticeArray, GL_STATIC_DRAW);

	glEnableVertexAttribArray(POSITION);
	// Position attribut
	glVertexAttribPointer(POSITION, POSITION_SIZE, GL_FLOAT, GL_FALSE, POSITION_SIZE * sizeof(float), (void*)0);


	delete[] verticeArray;
}

void MipmapScene::SetupScene()
{
	Shape2D::CreateRectangle(m_vertices, Vector2(-0.5, -0.5), Vector2(0.4, 0.4));
	Shape2D::CreateRectangle(m_vertices, Vector2(0.5, -0.5), Vector2(0.4, 0.4));
	Shape2D::CreateRectangle(m_vertices, Vector2(-0.5, 0.5), Vector2(0.4, 0.4));
	Shape2D::CreateRectangle(m_vertices, Vector2(0.5, 0.5), Vector2(0.4, 0.4));
	LoadShaders();
	CreateShaderPrograms();

	//Create one ID to be given at object generation and store it in m_vao via reference
	glGenVertexArrays(1, &m_vao);
	//Binding the VAO 
	//   /!\ If you have multiple VAOs, you should bind the correct VAO after each change
	glBindVertexArray(m_vao);

	//Binds the buffer linked to this ID to the vertex array buffer to be rendered. Put 0 instead of vbo to reset the value.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	VerticeInformationSlicer();
}

void MipmapScene::UpdateScene()
{
	m_program.Use();
	Shape2D::DrawRectangle(0);
	Shape2D::DrawRectangle(6);
	Shape2D::DrawRectangle(12);
	Shape2D::DrawRectangle(18);
}


