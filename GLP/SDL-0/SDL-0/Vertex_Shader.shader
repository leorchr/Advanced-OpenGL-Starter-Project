#version 330 core
in vec3 pos;
in vec3 color;
out vec4 ourColor;
uniform float updatePosX;
uniform float updatePosY;
out vec4 ourPosition;

void main()
{
    gl_Position = vec4(pos.x + updatePosX,  pos.y + updatePosY, pos.z, 1.0);
    ourColor = vec4(color, 1.0);
    ourPosition = gl_Position;
}