#include "GrassScene.h"
#include <iostream>

const int POSITION = 0;
const int POSITION_SIZE = 3; // 3 position infos per vertex

GrassScene::GrassScene()
{
	glGenBuffers(1, &m_vbo2);
	m_vertices = {
	-1.0f, -1.0f, 0.0f, // Left bottom corner
	 1.0f, -1.0f, 0.0f, // Right bottom corner
	 0.0f,  1.0f, 0.0f  // Top center
	};
}

GrassScene::~GrassScene() {
	glDeleteVertexArrays(1, &m_vao2);
}


void GrassScene::LoadShaders() {
	m_vertexShader.LoadFrom("terrainTessVertex.shader", VERTEX);
	m_fragmentShader.LoadFrom("terrainTessFragment.shader", FRAGMENT);
	m_tessControlShader.LoadFrom("terrainTessControl.shader", TESSELLATION_CONTROL);
	m_tessEvalShader.LoadFrom("terrainTessEvaluation.shader", TESSELLATION_EVALUATION);

	m_vertexShader2.LoadFrom("grassVertex.shader", VERTEX);
	std::cout<<m_vertexShader2.GetCode()<<std::endl;
	m_fragmentShader2.LoadFrom("simpleFragment.shader", FRAGMENT);
}

void GrassScene::CreateGrassShaderPrograms() {
	m_shaderProgram2.Compose(vector<Shader*>{&m_vertexShader2, & m_fragmentShader2});
	m_shaderProgram2.Use();
}

void GrassScene::VerticeInformationSlicer() {
	//Put the vector information in a float array, then in the array buffer 
	float* verticeArray = new float[m_vertices.size()]();
	std::copy(m_vertices.begin(), m_vertices.end(), verticeArray);

	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(float), verticeArray, GL_STATIC_DRAW);

	glEnableVertexAttribArray(POSITION);
	// Position attribut
	glVertexAttribPointer(POSITION, POSITION_SIZE, GL_FLOAT, GL_FALSE, POSITION_SIZE * sizeof(float), (void*)0);


	delete[] verticeArray;
}


void GrassScene::SetupScene() {
	LoadShaders();
	CreateShaderPrograms();
	CreateGrassShaderPrograms();

	//Create one ID to be given at object generation and store it in m_vao via reference
	glGenVertexArrays(1, &m_vao);
	//Binding the VAO 
	//   /!\ If you have multiple VAOs, you should bind the correct VAO after each change
	glBindVertexArray(m_vao);

	//Binds the buffer linked to this ID to the vertex array buffer to be rendered. Put 0 instead of vbo to reset the value.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	//glPatchParameteri(GL_PATCH_VERTICES, 4);

	//glEnable(GL_CULL_FACE);
	//TEXTURES
	Assets::loadTextureKtx("./Textures/terragen1.ktx", "terragen1");
	texDisplacement = Assets::getTextureKtx("terragen1").id;
	glBindTexture(GL_TEXTURE_2D, texDisplacement);

	glActiveTexture(GL_TEXTURE2);
	Assets::loadTextureKtx("./Textures/terragen_color.ktx", "terragen_color");
	texColor = Assets::getTextureKtx("terragen_color").id;
	glBindTexture(GL_TEXTURE_2D, texColor);


	glGenVertexArrays(1, &m_vao2);
	//Binding the VAO 
	//   /!\ If you have multiple VAOs, you should bind the correct VAO after each change
	glBindVertexArray(m_vao2);

	//Binds the buffer linked to this ID to the vertex array buffer to be rendered. Put 0 instead of vbo to reset the value.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo2);
	VerticeInformationSlicer();

}

void GrassScene::UpdateScene() {

	Matrix4 transform = Matrix4::createTranslation(Vector3(-0.5f, 0.0f, -10.0f));
	Matrix4 rot = Matrix4::createRotationY(Maths::piOver2);
	transform = transform * rot;
	Matrix4 scale = Matrix4::createScale(Vector3(30.0f, 1.0f, 1.0f));
	scale = scale * transform;


	proj = Matrix4::createPerspectiveFOV(70.0f, Window::Dimension.x, Window::Dimension.y, 0.1f, 1000.0f);
	view = Matrix4::identity;
	Matrix4 mvp = proj * view;
	mvp = mvp * transform;
	m_shaderProgram.Use();
	m_shaderProgram.setMatrix4("mv_matrix", view);
	m_shaderProgram.setMatrix4("proj_matrix", proj);
	m_shaderProgram.setMatrix4("mvp_matrix", proj * view);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArraysInstanced(GL_PATCHES, 0, 4, 64 * 64);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo2);

	glBindVertexArray(m_vao2);
	m_shaderProgram2.Use();
	m_shaderProgram2.setMatrix4("uViewProj",  mvp);

	glDrawArrays(GL_TRIANGLES, 0, 3);
}



