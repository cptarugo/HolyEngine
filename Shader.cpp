#include "Shader.h"



Shader::Shader()
{
	vShader = NULL;
	pShader = NULL;
	inputLayout = NULL;
	cbWorldViewProj = NULL;
}


Shader::~Shader()
{
	ReleaseCOM(vShader);
	ReleaseCOM(pShader);
	ReleaseCOM(inputLayout);
	ReleaseCOM(cbWorldViewProj);
}

bool Shader::Iniciar(WCHAR *shader_hlsl_name, ID3D11Device *device)
{
	if (!StartShader(shader_hlsl_name, device))
		return false;

	if (!CriarConstantBuffer(device))
		return false;

	return true;
}


void Shader::Ativar(ID3D11DeviceContext *conDevice, constantBufferShader *gWorldViewProj)
{
	///conDevice->UpdateSubresource(cbWorldViewProj, 0, 0, gWorldViewProj, 0, 0);
	D3D11_MAPPED_SUBRESOURCE ms;
	ZeroMemory(&ms, sizeof(ms));


	//Transposing a matriz
	XMMATRIX worldVProjTransposed = XMLoadFloat4x4(&gWorldViewProj->worldViewProj);
	XMStoreFloat4x4(&gWorldViewProj->worldViewProj, XMMatrixTranspose(worldVProjTransposed));

	conDevice->Map(cbWorldViewProj, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
	memcpy(ms.pData, &gWorldViewProj->worldViewProj, sizeof(gWorldViewProj->worldViewProj));
	conDevice->Unmap(cbWorldViewProj, 0);

	conDevice->VSSetConstantBuffers(0, 1, &cbWorldViewProj);


}

void Shader::Render(ID3D11DeviceContext * conDevice, UINT indicesCount)
{
	conDevice->IASetInputLayout(inputLayout);
	conDevice->VSSetShader(vShader, 0, 0);
	conDevice->PSSetShader(pShader, 0, 0);

	conDevice->DrawIndexed(indicesCount, 0, 0);
	///conDevice->Draw(3, 0);
}





bool Shader::StartShader(WCHAR *shader_hlsl_name, ID3D11Device * device)
{
	ID3D10Blob *vertexCompiled;
	ID3D10Blob *vertexErrorMsg;

	HRESULT hr = D3DX11CompileFromFile(shader_hlsl_name, 0, 0, "VS", "vs_4_0", 0, 0, 0, &vertexCompiled, &vertexErrorMsg, 0);
	if (FAILED(hr)) {
		if (vertexErrorMsg != 0) {
			MessageBox(0, (LPCWSTR)vertexErrorMsg->GetBufferPointer(), 0, 0);
			ReleaseCOM(vertexErrorMsg);
			ReleaseCOM(vertexCompiled);
			return false;
		}
		MessageBox(0, L"D3DX11CompileFromFile falhou.", L"Erro Fatal", 0);
		ReleaseCOM(vertexErrorMsg);
		ReleaseCOM(vertexCompiled);
		return false;
	}
	ReleaseCOM(vertexErrorMsg);


	ID3D10Blob *pixelCompiled;
	ID3D10Blob *pixelErrorMsg;

	hr = D3DX11CompileFromFile(shader_hlsl_name, 0, 0, "PS", "ps_4_0", 0, 0, 0, &pixelCompiled, &pixelErrorMsg, 0);
	if (FAILED(hr)) {
		if (pixelErrorMsg != 0) {
			MessageBox(0, (LPCWSTR)pixelErrorMsg->GetBufferPointer(), 0, 0);
			ReleaseCOM(pixelErrorMsg);
			ReleaseCOM(pixelCompiled);
			return false;
		}
		MessageBox(0, L"D3DX11CompileFromFile falhou.", L"Erro Fatal", 0);
		ReleaseCOM(pixelErrorMsg);
		ReleaseCOM(pixelCompiled);
		return false;
	}
	ReleaseCOM(pixelErrorMsg);




	hr = device->CreateVertexShader(vertexCompiled->GetBufferPointer(), vertexCompiled->GetBufferSize(), 0, &vShader);
	if (FAILED(hr)) {
		MessageBox(0, L"CreateVertexShader falhou.", L"Erro Fatal", 0);
		return false;
	}

	hr = device->CreatePixelShader(pixelCompiled->GetBufferPointer(), pixelCompiled->GetBufferSize(), 0, &pShader);
	if (FAILED(hr)) {
		MessageBox(0, L"CreatePixelShader falhou.", L"Erro Fatal", 0);
		return false;
	}



	//
	//	Criando o Input Layout
	//
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};


	hr = device->CreateInputLayout(vertexDesc, 2, vertexCompiled->GetBufferPointer(), vertexCompiled->GetBufferSize(), &inputLayout);
	if (FAILED(hr)) {
		MessageBox(0, L"CreateInputLayout falhou.", 0, 0);
		return false;
	}

	ReleaseCOM(vertexCompiled);
	ReleaseCOM(pixelCompiled);

	return true;
}

bool Shader::CriarConstantBuffer(ID3D11Device * device)
{
	D3D11_BUFFER_DESC cbBD;
	ZeroMemory(&cbBD, sizeof(cbBD));
	cbBD.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbBD.ByteWidth = sizeof(constantBufferShader);
	cbBD.Usage = D3D11_USAGE_DYNAMIC; //Acho q seria dynamic
	cbBD.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbBD.MiscFlags = 0;
	cbBD.StructureByteStride = 0;
	
	D3D11_SUBRESOURCE_DATA constantBufferInit;
	XMFLOAT4X4 pos;
	XMStoreFloat4x4(&pos, XMMatrixIdentity());
	ZeroMemory(&constantBufferInit, sizeof(constantBufferInit));
	constantBufferInit.pSysMem = &pos;

	HRESULT hr = device->CreateBuffer(&cbBD, &constantBufferInit, &cbWorldViewProj);
	if (FAILED(hr)) {
		MessageBox(0, L"CreateBuffer (Constant) falhou.", 0, 0);
		return false;
	}

	return true;
}
