#ifndef _LUZ_H
#define _LUZ_H

#pragma once

#include <Windows.h>
#include <xnamath.h>


struct LuzDirecionada {

	LuzDirecionada() { ZeroMemory(this, sizeof(this)); }

	XMFLOAT4 Ambiente;
	XMFLOAT4 Difuso;
	XMFLOAT4 Especular;
	XMFLOAT3 Direcao;
	float Pad;

};


struct PontoLuz {

	PontoLuz() { ZeroMemory(this, sizeof(this)); }

	XMFLOAT4 Ambiente;
	XMFLOAT4 Difuso;
	XMFLOAT4 Especular;

	XMFLOAT3 Posicao;
	float Alcance;

	XMFLOAT3 Att;
	float Pad;

};


struct LuzFocada {

	LuzFocada() { ZeroMemory(this, sizeof(this)); }

	XMFLOAT4 Ambiente;
	XMFLOAT4 Difuso;
	XMFLOAT4 Especular;

	XMFLOAT3 Posicao;
	float Alcance;

	XMFLOAT3 Direcao;
	float Spot;

	XMFLOAT3 Att;
	float Pad;
};


struct Material {
	
	Material() { ZeroMemory(this, sizeof(this)); }

	XMFLOAT4 Ambiente;
	XMFLOAT4 Difuso;
	XMFLOAT4 Especular;
	XMFLOAT4 Refletivo;
};

#endif
