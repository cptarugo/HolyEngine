#ifndef _MODEL_H
#define _MODEL_H

#pragma once
#include <d3dx11.h>
#include <xnamath.h>
#include "Definicoes.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "Vertice.h"
#include "Luz.h"


//Utilizar essa classe como base para qualquer objeto 3D
class Model
{
public:
	Model();
	~Model();



	UINT nModelVert;
	UINT nModelIndex;
	std::vector<Vertice> vertices;
	std::vector<UINT> indices;

	void OpenTXT(char *file_name, ID3D11Device *device);
	//Book TXT's
	void OpenBookTXT(char *file_name, ID3D11Device *device);
	void Build(ID3D11Device *device);
	/*
	void LoadCube(ID3D11Device *device);
	*/
	inline ID3D11Buffer *GetVerticesBuffer() {	return modelVertices; }
	inline ID3D11Buffer *GetIndicesBuffer() { return modelIndices;  }

	virtual void Render(ID3D11DeviceContext *conDevice, ID3D11RasterizerState *rasterizerState); //Sobrescrever esse metodo para cada modelo diferente

	void Update(float dt);

	void SetWorldMatrix(const XMMATRIX& newWorld);
	XMMATRIX GetWorldMatrix();

	void SetMaterial(XMFLOAT4 Ambiente, XMFLOAT4 Difuso, XMFLOAT4 Especular, XMFLOAT4 Refletivo);
	void SetMaterial(Material newMat);
	Material GetMaterial();
private:

	ID3D11Buffer *modelVertices;
	ID3D11Buffer *modelIndices;

	XMMATRIX worldMatrix;

	Material material;
};

#endif