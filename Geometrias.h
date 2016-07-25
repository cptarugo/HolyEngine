#ifndef _GEOMETRIAS_H
#define _GEOMETRIAS_H

#pragma once

#include "Model.h"
#include "MathHead.h"

class Geometrias
{
public:

	//
	//	Sem uso POR ENQUANTO
	//
	struct Vertex
	{
		Vertex() {}
		Vertex(const XMFLOAT3& p, const XMFLOAT3& n, const XMFLOAT3& t, const XMFLOAT2& uv)
			: Position(p), Normal(n), TangentU(t), TexC(uv) {}
		Vertex(
			float px, float py, float pz,
			float nx, float ny, float nz,
			float tx, float ty, float tz,
			float u, float v)
			: Position(px, py, pz), Normal(nx, ny, nz),
			TangentU(tx, ty, tz), TexC(u, v) {}

		XMFLOAT3 Position;
		XMFLOAT3 Normal;
		XMFLOAT3 TangentU;
		XMFLOAT2 TexC;
	};

	void CriarGrid(float width, float depth, UINT m, UINT n, Model &modelo);
	void CriarCilindro(float raioBase, float raioTopo, float altura, UINT nCortes, UINT nStack, Model &modelo);
	void Geosfera(float raio, UINT nSubdivisao, Model &modelo);
	void Height(float width, float depth, UINT m, UINT n, Model &modelo);	//Height seria montanha na verdade. So n to afim de mudar o nome



	//
	//	Utils
	//
	void Subdividir(Model &modelo);


	void BuildCilindroTopo(float raioBase, float raioTopo, float altura, UINT nCortes, UINT nStack, Model& modelo);
	void BuildCilindroBase(float raioBase, float raioTopo, float altura, UINT nCortes, UINT nStack, Model& modelo);
	float GetHeight(float x, float z);
};

#endif