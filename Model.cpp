#include "Model.h"



Model::Model()
{
	nModelVert = 0;
	nModelIndex = 0;

	//modelVertices = NULL;
	//modelIndices = NULL;
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


	std::vector<Vertice> vertices(nModelVert);
	for (int i = 0; i < nModelVert; ++i) {
		fin >> vertices[i].Pos.x >> vertices[i].Pos.y >> vertices[i].Pos.z;
		vertices[i].Cor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	}


	fin >> ignore >> ignore;

	std::vector<UINT> indices(nModelIndex);
	UINT tNumber = nModelIndex;
	for (int i = 0; i < tNumber / 3; ++i) {
		fin >> indices[i * 3 + 0] >> indices[i * 3 + 1] >> indices[i * 3 + 2];
	}

	fin.close();


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


void Model::Render(ID3D11DeviceContext *conDevice, ID3D11RasterizerState *rasterizerState)
{
	UINT strides = sizeof(Vertice);
	UINT offset = 0;

	conDevice->IASetVertexBuffers(0, 1, &modelVertices, &strides, &offset);
	conDevice->IASetIndexBuffer(modelIndices, DXGI_FORMAT_R32_UINT, 0);
	conDevice->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	conDevice->RSSetState(rasterizerState);
}


