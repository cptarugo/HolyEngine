#ifndef _MODEL_H
#define _MODEL_H

#pragma once
#include <d3dx11.h>
#include "Definicoes.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "Vertice.h"



//Utilizar essa classe como base para qualquer objeto 3D
class Model
{
public:
	Model();
	~Model();



	UINT nModelVert;
	UINT nModelIndex;

	void OpenTXT(char *file_name, ID3D11Device *device);

	inline ID3D11Buffer *GetVerticesBuffer() {	return modelVertices; }
	inline ID3D11Buffer *GetIndicesBuffer() { return modelIndices;  }

	virtual void Render(ID3D11DeviceContext *conDevice, ID3D11RasterizerState *rasterizerState); //Sobrescrever esse metodo para cada modelo diferente

private:

	ID3D11Buffer *modelVertices;
	ID3D11Buffer *modelIndices;
	

};

#endif