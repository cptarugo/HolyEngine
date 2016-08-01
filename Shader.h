#ifndef _SHADER_H
#define _SHADER_H

#pragma once

#include <d3dx11.h>
#include <xnamath.h>
#include "Definicoes.h"
#include "Luz.h"


//cbPerObject
struct constantBufferShader {
	XMFLOAT4X4 worldViewProj;
	XMFLOAT4X4 gWorld;
	XMFLOAT4X4 gWorldInvTranspose;
	Material gMaterial;
};



///__declspec(align(16))		///Deixando a struct alinhada com 16 bytes (senao o CreateBuffer ira falhar por causa do tamanho da struct nao ser multiplo de 16)
struct cbPerFrame {
	LuzDirecionada gLuzDir;
	PontoLuz gLuzPt;
	LuzFocada gLuzFoco;
	XMFLOAT3 gPosOlhoW;
	float pad;
};

class Shader
{
public:
	Shader();
	~Shader();

	bool Iniciar(WCHAR *shader_hlsl_name, ID3D11Device *device);

	void AtivarPerObject(ID3D11DeviceContext *conDevice, constantBufferShader *gWorldViewProj);
	void AtivarPerFrame(ID3D11DeviceContext *conDevice, cbPerFrame *gPerFrame);

	void Render(ID3D11DeviceContext *conDevice, UINT indicesCount);

private:

	//SHADER
	ID3D11VertexShader *vShader;
	ID3D11PixelShader *pShader;

	// INPUT LAYOUT PARA ESSE SHADER
	ID3D11InputLayout *inputLayout;


	//CONSTANT BUFFER PARA OS SHADERS
	ID3D11Buffer *cbWorldViewProj;
	ID3D11Buffer *cbPerFrameBuff;


	bool StartShader(WCHAR *shader_hlsl_name, ID3D11Device *device);

	bool CriarConstantBuffer(ID3D11Device *device);
};

#endif