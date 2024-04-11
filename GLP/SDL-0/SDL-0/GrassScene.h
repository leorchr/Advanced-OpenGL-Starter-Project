#pragma once
#include "TessellationScene.h"
#include <Matrix4.h>
#include "Assets.h"
class GrassScene :public TessellationScene
{
public:
    GrassScene();
    ~GrassScene();
    void SetupScene();
    void UpdateScene();
    void HandleInputs() {}

private:
    virtual void LoadShaders();
    virtual void CreateGrassShaderPrograms();
    virtual void VerticeInformationSlicer();

    // Uniforms
    Matrix4 view;
    Matrix4 proj;

    GLuint texDisplacement;
    GLuint texColor;

    Shader m_vertexShader2, m_fragmentShader2;
    ShaderProgram m_shaderProgram2;
    unsigned int m_vbo2, m_vao2;

    // Inherited via Scene
};

