#pragma once

#include <SDL.h>
#include "glew.h"

#include "Vector2.h"
#include "Color.h"

class Window
{
private:
	SDL_Window* m_window;
	SDL_GLContext m_context;
	Color m_backgroundColor;


public:
	static Vector2 Dimension;

	Window(float width=1000.0f, float height=700.0f, Color colorP = Color(Color::BLUE));
	~Window() {}

	void ChangeBackgroundColor(Color color);
	void ChangeBackgroundColor(float rP, float gP, float bP, float aP);
	void PaintBackground();
	void Update();
	void Close();
};

