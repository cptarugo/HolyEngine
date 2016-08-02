#include "Model.h"



Model::Model()
{
	nModelVert = 0;
	nModelIndex = 0;

	modelVertices = NULL;
	modelIndices = NULL;

	worldMatrix = XMMatrixIdentity();
}


Model::~Model()
{
	ReleaseCOM(modelVertices);
	ReleaseCOM(modelIndices);
}


void Model::OpenTXT(char * file_name, ID3D11Device* device)
{
	

	std::fstream fin(file_name);

	if (!fin) {
		MessageBox(0, L"Erro ao carregar o arquivo de modelo (Arquivo nao existe ou esta corrompido).", L"Model Error", 0);
		return;
	}

	std::string ignore;
	fin >> ignore >> nModelVert;
	fin >> ignore >> nModelIndex;
	fin >> ignore;


	vertices.resize(nModelVert);
	for (int i = 0; i < nModelVert; ++i) {
		fin >> vertices[i].Pos.x >> vertices[i].Pos.y >> vertices[i].Pos.z;
		fin >> vertices[i].Normal.x >> vertices[i].Normal.y >> vertices[i].Normal.z;
		//vertices[i].Cor.w = 1.0f;
	}


	fin >> ignore >> ignore;

	indices.resize(nModelIndex);
	UINT tNumber = nModelIndex;
	for (int i = 0; i < tNumber / 3; ++i) {
		fin >> indices[i * 3 + 0] >> indices[i * 3 + 1] >> indices[i * 3 + 2];
	}

	fin.close();




	Build(device);
}





void Model::OpenBookTXT(char * file_name, ID3D11Device * device)
{

	std::fstream fin(file_name);

	if (!fin) {
		MessageBox(0, L"Erro ao carregar o arquivo de modelo (Arquivo nao existe ou esta corrompido).", L"Model Error", 0);
		return;
	}

	std::string ignore;
	fin >> ignore >> nModelVert;
	fin >> ignore >> nModelIndex;
	fin >> ignore >> ignore >> ignore >> ignore;


	vertices.resize(nModelVert);
	for (int i = 0; i < nModelVert; ++i) {
		fin >> vertices[i].Pos.x >> vertices[i].Pos.y >> vertices[i].Pos.z;

		//Seria a normal no lugar da cor aqui
		fin >> vertices[i].Normal.x >> vertices[i].Normal.y >> vertices[i].Normal.z;
	}


	fin >> ignore >> ignore >> ignore;


	UINT mTriangleCount = nModelIndex;
	nModelIndex *= 3;
	indices.resize(nModelIndex);
	for (int i = 0; i < mTriangleCount; ++i) {
		fin >> indices[i * 3 + 0] >> indices[i * 3 + 1] >> indices[i * 3 + 2];
	}

	fin.close();




	Build(device);
}




void Model::Build(ID3D11Device * device)
{
	//
	//	Criando a descricao para o buffer dos vertices
	//

	D3D11_BUFFER_DESC bdVertex;
	ZeroMemory(&bdVertex, sizeof(bdVertex));
	bdVertex.Usage = D3D11_USAGE_IMMUTABLE;
	bdVertex.ByteWidth = sizeof(Vertice) * nModelVert;
	bdVertex.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bdVertex.CPUAccessFlags = 0;
	bdVertex.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA sdVertex;
	ZeroMemory(&sdVertex, sizeof(sdVertex));
	sdVertex.pSysMem = &vertices[0];


	//
	//	Criando a descricao para o buffer dos indices
	//

	D3D11_BUFFER_DESC bdIndex;
	ZeroMemory(&bdIndex, sizeof(bdIndex));
	bdIndex.Usage = D3D11_USAGE_IMMUTABLE;
	bdIndex.ByteWidth = sizeof(UINT) * nModelIndex;
	bdIndex.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bdIndex.CPUAccessFlags = 0;
	bdIndex.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA sdIndex;
	ZeroMemory(&sdIndex, sizeof(sdIndex));
	sdIndex.pSysMem = &indices[0];




	//
	//	Criando os buffers
	//

	HRESULT hr = device->CreateBuffer(&bdVertex, &sdVertex, &modelVertices);
	if (FAILED(hr)) {
		MessageBox(0, L"CreateBuffer falhou.", L"Erro Fatal", 0);
		return;
	}

	hr = device->CreateBuffer(&bdIndex, &sdIndex, &modelIndices);
	if (FAILED(hr)) {
		MessageBox(0, L"CreateBuffer falhou.", L"Erro Fatal", 0);
		return;
	}
}



/*
void Model::LoadCube(ID3D11Device * device)
{
	Vertice vertices[] =
	{
		//FRONT
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) }, //0
		{ XMFLOAT3(-1.0f,  1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) }, //1
		{ XMFLOAT3(1.0f,  1.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) }, //2
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) }, //3

																	 //BACK
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) }, //4
		{ XMFLOAT3(-1.0f,  1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) }, //5
		{ XMFLOAT3(1.0f,  1.0f, -1.0f), XMFLOAT4(0.4f, 0.2f, 0.0f, 1.0f) }, //6
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(0.8f, 1.0f, 0.25f, 1.0f) }, //7

																	  //TOP
		{ XMFLOAT3(-1.0f,  1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) }, //1
		{ XMFLOAT3(-1.0f,  1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) }, //5
		{ XMFLOAT3(1.0f,  1.0f, -1.0f), XMFLOAT4(0.4f, 0.2f, 0.0f, 1.0f) }, //6
		{ XMFLOAT3(1.0f,  1.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) }, //2

																	 //BOTTOM
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) }, //4
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) }, //0
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) }, //3
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(0.8f, 1.0f, 0.25f, 1.0f) }, //7

																	  //RIGHT
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) }, //3
		{ XMFLOAT3(1.0f,  1.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) }, //2
		{ XMFLOAT3(1.0f,  1.0f, -1.0f), XMFLOAT4(0.4f, 0.2f, 0.0f, 1.0f) }, //6
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(0.8f, 1.0f, 0.25f, 1.0f) }, //7
	
																	  //LEFT
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) }, //4
		{ XMFLOAT3(-1.0f,  1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) }, //5
		{ XMFLOAT3(-1.0f,  1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) }, //1
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) }, //0
	};
	nModelVert = 24;

	UINT indices[] =
	{
		//FRONT
		0, 2, 1,
		0, 3, 2,

		//BACK
		7, 5, 6,
		7, 4, 5,

		//TOP
		8, 10, 9,
		8, 11, 10,

		//BOTTOM
		12, 14, 13,
		12, 15, 14,

		//RIGHT
		16, 18, 17,
		16, 19, 18,

		//LEFT
		20, 22, 21,
		20, 23, 22,
	};
	nModelIndex = 36;

	//VERTEX BUFFER
	D3D11_BUFFER_DESC vertexDesc;
	ZeroMemory(&vertexDesc, sizeof(D3D11_BUFFER_DESC));
	vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexDesc.ByteWidth = sizeof(Vertice) * nModelVert;
	vertexDesc.CPUAccessFlags = 0;
	vertexDesc.MiscFlags = 0;
	vertexDesc.StructureByteStride = 0;
	vertexDesc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA resourceData;
	ZeroMemory(&resourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	resourceData.pSysMem = vertices;

	HRESULT hr = device->CreateBuffer(&vertexDesc, &resourceData, &modelVertices);
	if (FAILED(hr))
		return;

	//INDEX BUFFER
	D3D11_BUFFER_DESC indexDesc;
	ZeroMemory(&indexDesc, sizeof(D3D11_BUFFER_DESC));
	indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexDesc.ByteWidth = sizeof(UINT) * nModelIndex;
	indexDesc.CPUAccessFlags = 0;
	indexDesc.MiscFlags = 0;
	indexDesc.StructureByteStride = 0;
	indexDesc.Usage = D3D11_USAGE_DEFAULT;
	resourceData.pSysMem = indices;

	hr = device->CreateBuffer(&indexDesc, &resourceData, &modelIndices);
	if (FAILED(hr))
		return;
}
*/


void Model::Render(ID3D11DeviceContext *conDevice, ID3D11RasterizerState *rasterizerState)
{
	conDevice->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	conDevice->RSSetState(rasterizerState);


	UINT strides = sizeof(Vertice);
	UINT offset = 0;
	conDevice->IASetVertexBuffers(0, 1, &modelVertices, &strides, &offset);
	conDevice->IASetIndexBuffer(modelIndices, DXGI_FORMAT_R32_UINT, 0);
}

void Model::Update(float dt)
{
	XMMATRIX rotMatrix = XMMatrixRotationRollPitchYaw(dt, dt, dt);
	worldMatrix = ((XMMATRIX)rotMatrix) * ((XMMATRIX)worldMatrix);
}

void Model::SetWorldMatrix(const XMMATRIX& newWorld)
{
	worldMatrix = newWorld;
}

XMMATRIX Model::GetWorldMatrix()
{
	return worldMatrix;
}

void Model::SetMaterial(XMFLOAT4 Ambiente, XMFLOAT4 Difuso, XMFLOAT4 Especular, XMFLOAT4 Refletivo)
{
	material.Ambiente = Ambiente;
	material.Difuso = Difuso;
	material.Especular = Especular;
	material.Refletivo = Refletivo;
}

void Model::SetMaterial(Material newMat)
{
	material = newMat;
}

Material Model::GetMaterial()
{
	return material;
}


