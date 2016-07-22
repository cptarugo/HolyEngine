#ifndef _MOUSE_H
#define _MOUSE_H

#pragma once

#include <d3dx11.h>

struct Position {
	int x;
	int y;
};

class Mouse
{
public:

	Position posicao;
	Position lastMousePos;

	float dx, dy;

	Mouse() : dx(0.0f), dy(0.0f)
	{
		posicao.x = 0;
		posicao.y = 0;

		lastMousePos.x = 0;
		lastMousePos.y = 0;
	}

	~Mouse()
	{
	}

	void SetPos(int _x, int _y) {
		posicao.x = _x;
		posicao.y = _y;
	}

};

#endif