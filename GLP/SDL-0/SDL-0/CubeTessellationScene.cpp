#include "CubeTessellationScene.h"

#include "MeshObject.h"
#include "Window.h"

CubeTessellationScene::CubeTessellationScene() : TessellationScene()
{
}


void CubeTessellationScene::LoadShaders() {
	m_vertexShader.LoadFrom("cube.vert", VERTEX);
	m_fragmentShader.LoadFrom("cube.frag", FRAGMENT);
	m_tessControlShader.LoadFrom("cubeTC.shader", TESSELLATION_CONTROL);
	m_tessEvalShader.LoadFrom("cubeTE.shader", TESSELLATION_EVALUATION);
	glPatchParameteri(GL_PATCH_VERTICES, 3);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void CubeTessellationScene::CreateShaderPrograms()
{
	m_Program.Compose(
		vector<Shader*>{
		&m_vertexShader,
		& m_fragmentShader,
		& m_tessControlShader,
		& m_tessEvalShader
	});
}

void CubeTessellationScene::VerticeInformationSlicer()
{
	float* verticeArray = new float[m_vertices.size()]();
	std::copy(m_vertices.begin(), m_vertices.end(), verticeArray);

	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(float), verticeArray, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	delete[] verticeArray;
}

void CubeTessellationScene::SetupScene()
{
	m_vertices = { -0.25f,  0.25f, -0.25f,
			  -0.25f, -0.25f, -0.25f,
				0.25f, -0.25f, -0.25f,

				0.25f, -0.25f, -0.25f,
				0.25f,  0.25f, -0.25f,
			  -0.25f,  0.25f, -0.25f,

				0.25f, -0.25f, -0.25f,
				0.25f, -0.25f,  0.25f,
				0.25f,  0.25f, -0.25f,

				0.25f, -0.25f,  0.25f,
				0.25f,  0.25f,  0.25f,
				0.25f,  0.25f, -0.25f,

				0.25f, -0.25f,  0.25f,
			  -0.25f, -0.25f,  0.25f,
				0.25f,  0.25f,  0.25f,

			  -0.25f, -0.25f,  0.25f,
			  -0.25f,  0.25f,  0.25f,
				0.25f,  0.25f,  0.25f,

			  -0.25f, -0.25f,  0.25f,
			  -0.25f, -0.25f, -0.25f,
			  -0.25f,  0.25f,  0.25f,

			  -0.25f, -0.25f, -0.25f,
			  -0.25f,  0.25f, -0.25f,
			  -0.25f,  0.25f,  0.25f,

			  -0.25f, -0.25f,  0.25f,
				0.25f, -0.25f,  0.25f,
				0.25f, -0.25f, -0.25f,

				0.25f, -0.25f, -0.25f,
			  -0.25f, -0.25f, -0.25f,
			  -0.25f, -0.25f,  0.25f,

			  -0.25f,  0.25f, -0.25f,
				0.25f,  0.25f, -0.25f,
				0.25f,  0.25f,  0.25f,

				0.25f,  0.25f,  0.25f,
			  -0.25f,  0.25f,  0.25f,
			  -0.25f,  0.25f, -0.25f
	};
	LoadShaders();
	CreateShaderPrograms();
	projection = Matrix4::createPerspectiveFOV(70.0f, Window::Dimension.x, Window::Dimension.y, 0.1f, 1000.0f);
	position = Vector3(0, 0, -4.0f);
	mv = Matrix4::createTranslation(position);

	//Create one ID to be given at object generation and store it in m_vao via reference
	glGenVertexArrays(1, &m_vao);
	//Binding the VAO 
	//   /!\ If you have multiple VAOs, you should bind the correct VAO after each change
	glBindVertexArray(m_vao);

	//Binds the buffer linked to this ID to the vertex array buffer to be rendered. Put 0 instead of vbo to reset the value.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	VerticeInformationSlicer();
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
}

void CubeTessellationScene::UpdateScene() {
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


	float timeValue = (float)SDL_GetTicks() / 1000;
	float sinusoidValue = (sin(timeValue)) / 2.0f;

	position.y = sinusoidValue;
	m_Program.Use();
	mv = Matrix4::createTranslation(position - Vector3(0.5f, 0, 0));
	Matrix4 rotationX = Matrix4::createRotationX(0.01f);
	Matrix4 rotationY = Matrix4::createRotationY(0.005f);
	Matrix4 rotationZ = Matrix4::createRotationY(0.01f);
	rotation *= rotationX;
	rotation *= rotationY;
	rotation *= rotationZ;
	mv = mv * rotation;


	m_Program.setMatrix4("mv_matrix", mv);
	m_Program.setMatrix4("projection", projection);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	mv = Matrix4::createTranslation(position + Vector3(0.5f, 0, 0));
	mv = mv * rotation;
	m_Program.setMatrix4("mv_matrix", mv);

	float sin = sinf(timeValue) / 2.0f + 0.5f;
	m_Program.setFloat("outerRatio", sin * 5);
	float cos = cosf(timeValue) / 2.0f + 0.5f;
	m_Program.setFloat("innerRatio", cos * 5);


	glPointSize(5.0f);
	glDrawArrays(GL_PATCHES, 0, 36);
}