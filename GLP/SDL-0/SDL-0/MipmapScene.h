#pragma once
#include "Scene.h"
#include "ShaderProgram.h"
#include "Shapes2D.h"
#include "Vector2.h"
#include "glew.h"
#include <string>


class MipmapScene : public Scene
{
protected:
	Shader m_vertexShader, m_fragmentShader;
	ShaderProgram m_program;

	virtual void LoadShaders();
	virtual void CreateShaderPrograms();
	virtual void VerticeInformationSlicer();
public:
	MipmapScene();
	~MipmapScene() {}
	void SetupScene();
	void UpdateScene();
};