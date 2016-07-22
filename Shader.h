#ifndef _SHADER_H
#define _SHADER_H

#pragma once

#include <d3dx11.h>
#include <xnamath.h>
#include "Definicoes.h"

struct constantBufferShader {
	XMFLOAT4X4 worldViewProj;
};

struct cbPerObject {
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProj;
};

class Shader
{
public:
	Shader();
	~Shader();

	bool Iniciar(WCHAR *shader_hlsl_name, ID3D11Device *device);

	void Ativar(ID3D11DeviceContext *conDevice, constantBufferShader *gWorldViewProj);

	void Render(ID3D11DeviceContext *conDevice, UINT indicesCount);

private:

	//SHADER
	ID3D11VertexShader *vShader;
	ID3D11PixelShader *pShader;

	// INPUT LAYOUT PARA ESSE SHADER
	ID3D11InputLayout *inputLayout;


	//CONSTANT BUFFER PARA OS SHADERS
	ID3D11Buffer *cbWorldViewProj;


	bool StartShader(WCHAR *shader_hlsl_name, ID3D11Device *device);

	bool CriarConstantBuffer(ID3D11Device *device);
};

#endif