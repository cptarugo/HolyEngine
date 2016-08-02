#include "Geometrias.h"



void Geometrias::CriarGrid(float width, float depth, UINT m, UINT n, Model & modelo)
{
	UINT nVertice = m*n;
	UINT nFaces = (m - 1)*(n - 1) * 2;

	modelo.nModelVert = nVertice;
	modelo.nModelIndex = nFaces * 3;

	float widthMetade = width*0.5f;
	float depthMetade = depth*0.5f;

	float dx = width / (n - 1);
	float dz = depth / (m - 1);


	//
	//	Criando os vertices
	//

	modelo.vertices.resize(nVertice);
	for (UINT i = 0; i < m; ++i) {

		float z = depthMetade - i * dz;

		for (UINT j = 0; j < n; ++j) {

			float x = -widthMetade + j * dx;
			
			modelo.vertices[i*n + j].Pos = XMFLOAT3(x, 0.0f, z);

		}
	}



	//
	//	Criando os indices
	//

	modelo.indices.resize(nFaces * 3);
	
	UINT k = 0;
	for (UINT i = 0; i < m-1; ++i) {
		for (UINT j = 0; j < n-1; ++j) {

			modelo.indices[k] = i*n + j;
			modelo.indices[k + 1] = i*n + j + 1;
			modelo.indices[k + 2] = (i + 1)*n + j;
			modelo.indices[k + 3] = (i + 1)*n + j;
			modelo.indices[k + 4] = i*n + j + 1;
			modelo.indices[k + 5] = (i + 1)*n + j + 1;
			k += 6; // next quad

		}
	}
}






void Geometrias::CriarCilindro(float raioBase, float raioTopo, float altura, UINT nCortes, UINT nStack, Model & modelo)
{
	if (raioTopo > raioBase) {
		float aux = raioBase;
		raioBase = raioTopo;
		raioTopo = aux;
	}

	modelo.vertices.clear();
	modelo.indices.clear();

	float stackAltura = altura / nStack;

	//Numero que determina o quanto eh preciso incrementar o raio, a cada vez q movemos para um level acima nos stacks.
	float deltaR = (raioBase - raioTopo) / nStack;


	UINT numAnel = nStack + 1;

	for (UINT i = 0; i < numAnel; ++i) {

		float y = -0.5f*altura + i * stackAltura;
		float r = raioBase + i * deltaR;


		//Criando os vertices no anel
		float dTheta = 2.0f*XM_PI / nCortes;

		for (UINT j = 0; j <= nCortes; ++j) {

			Vertice vertex;
			float c = cosf(j*dTheta);
			float s = sinf(j*dTheta);

			vertex.Pos = XMFLOAT3(r*c, y, r*s);
			///vertex.Cor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

			modelo.vertices.push_back(vertex);

		}
	}






	//
	//	Criando os indices
	//

	UINT anelVertexN = nCortes + 1;

	for (UINT i = 0; i < nStack; ++i) {
		for (UINT j = 0; j < nCortes; ++j) {
			modelo.indices.push_back(i*anelVertexN + j);
			modelo.indices.push_back((i + 1)*anelVertexN + j);
			modelo.indices.push_back((i + 1)*anelVertexN + j + 1);
			modelo.indices.push_back(i*anelVertexN + j);	
			modelo.indices.push_back((i + 1)*anelVertexN + j + 1);
			modelo.indices.push_back(i*anelVertexN + j + 1);
		}
	}

	BuildCilindroTopo(raioBase, raioTopo, altura, nCortes, nStack, modelo);
	BuildCilindroBase(raioBase, raioTopo, altura, nCortes, nStack, modelo);

	modelo.nModelVert = modelo.vertices.size();
	modelo.nModelIndex = modelo.indices.size();
}







void Geometrias::Geosfera(float raio, UINT nSubdivisoes, Model & modelo)
{
	nSubdivisoes = MathHead::Min(nSubdivisoes, 5u);

	//????
	const float X = 0.525731f;
	const float Z = 0.850651f;


	//Fazendo um icosaedro
	XMFLOAT3 pos[12] = {
		XMFLOAT3(-X, 0.0f, Z), XMFLOAT3(X, 0.0f, Z),
		XMFLOAT3(-X, 0.0f, -Z), XMFLOAT3(X, 0.0f, -Z),
		XMFLOAT3(0.0f, Z, X), XMFLOAT3(0.0f, Z, -X),
		XMFLOAT3(0.0f, -Z, X), XMFLOAT3(0.0f, -Z, -X),
		XMFLOAT3(Z, X, 0.0f), XMFLOAT3(-Z, X, 0.0f),
		XMFLOAT3(Z, -X, 0.0f), XMFLOAT3(-Z, -X, 0.0f)
	};

	UINT k[60] = {
		1,4,0, 4,9,0, 4,5,9, 8,5,4, 1,8,4,
		1,10,8, 10,3,8, 8,3,5, 3,2,5, 3,7,2,
		3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
		10,1,6, 11,0,9, 2,11,9, 5,2,9, 11,2,7
	};

	modelo.vertices.resize(12);
	modelo.nModelVert = 12;

	modelo.indices.resize(60);
	modelo.nModelIndex = 60;

	for (size_t i = 0; i < 12; ++i)
		modelo.vertices[i].Pos = pos[i];

	for (size_t i = 0; i < 60; ++i)
		modelo.indices[i] = k[i];

	for (size_t i = 0; i < nSubdivisoes; ++i)
		Subdividir(modelo);



	//Projetando vertices no modelo e escalonando.
	for (size_t i = 0; i < modelo.vertices.size(); ++i) {

		//Projetando na esfera unitaria
		XMVECTOR n = XMVector3Normalize(XMLoadFloat3(&modelo.vertices[i].Pos));

		//Projetando na esfera
		XMVECTOR p = raio * n;


		//
		//	Comente com /**/ para desativar cores aleatorias
		//

		//Cores aleatorias
		float r = 1.0f;//static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float g = 1.0f;//static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float b = 1.0f;//static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

		XMStoreFloat3(&modelo.vertices[i].Pos, p);
		///XMStoreFloat4(&modelo.vertices[i].Cor, XMVectorSet(r, g, b, 1.0f));
		//Setar a normal tbm

		//E lidar com a textura aqui
	}

	modelo.nModelVert = modelo.vertices.size();
	modelo.nModelIndex = modelo.indices.size();
}





void Geometrias::Height(float width, float depth, UINT m, UINT n, Model & modelo)
{
	CriarGrid(width, depth, m, n, modelo);


	std::vector<Vertice> vertices(modelo.vertices.size());

	for (size_t i = 0; i < modelo.vertices.size(); ++i) {
		
		XMFLOAT3 p = modelo.vertices[i].Pos;

		p.y = GetHeight(p.x, p.z);

		vertices[i].Pos = p;
		vertices[i].Normal = GetHillNormal(p.x, p.z);
		
		/*
		if (p.y < -10.0f) 
			vertices[i].Cor = XMFLOAT4(1.0f, 0.96f, 0.62f, 1.0f);
		else if (p.y < 5.0f)
			vertices[i].Cor = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
		else if (p.y < 12.0f)
			vertices[i].Cor = XMFLOAT4(0.1f, 0.48f, 0.19f, 1.0f);
		else if (p.y < 20.0f)
			vertices[i].Cor = XMFLOAT4(0.45f, 0.39f, 0.34f, 1.0f);
		else
			vertices[i].Cor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		*/
	}

	modelo.vertices = vertices;
}










void Geometrias::Subdividir(Model & modelo)
{
	Model copia = modelo;

	modelo.indices.clear();
	modelo.vertices.clear();

	UINT numTris = copia.indices.size() / 3;
	for (UINT i = 0; i < numTris; ++i)
	{
		
		Vertice v0 = copia.vertices[copia.indices[i * 3 + 0]];//	Pegando
		Vertice v1 = copia.vertices[copia.indices[i * 3 + 1]];//	os
		Vertice v2 = copia.vertices[copia.indices[i * 3 + 2]];//	triangulos


		//
		// Generate the midpoints.
		//

		Vertice m0, m1, m2;

		// For subdivision, we just care about the position component.  We derive the other
		// vertex components in CreateGeosphere.

		m0.Pos = XMFLOAT3(
			0.5f*(v0.Pos.x + v1.Pos.x),
			0.5f*(v0.Pos.y + v1.Pos.y),
			0.5f*(v0.Pos.z + v1.Pos.z));

		m1.Pos = XMFLOAT3(
			0.5f*(v1.Pos.x + v2.Pos.x),
			0.5f*(v1.Pos.y + v2.Pos.y),
			0.5f*(v1.Pos.z + v2.Pos.z));

		m2.Pos = XMFLOAT3(
			0.5f*(v0.Pos.x + v2.Pos.x),
			0.5f*(v0.Pos.y + v2.Pos.y),
			0.5f*(v0.Pos.z + v2.Pos.z));

		//
		// Add new geometry.
		//

		modelo.vertices.push_back(v0); // 0
		modelo.vertices.push_back(v1); // 1
		modelo.vertices.push_back(v2); // 2
		modelo.vertices.push_back(m0); // 3
		modelo.vertices.push_back(m1); // 4
		modelo.vertices.push_back(m2); // 5

		modelo.indices.push_back(i * 6 + 0);
		modelo.indices.push_back(i * 6 + 3);
		modelo.indices.push_back(i * 6 + 5);

		modelo.indices.push_back(i * 6 + 3);
		modelo.indices.push_back(i * 6 + 4);
		modelo.indices.push_back(i * 6 + 5);

		modelo.indices.push_back(i * 6 + 5);
		modelo.indices.push_back(i * 6 + 4);
		modelo.indices.push_back(i * 6 + 2);

		modelo.indices.push_back(i * 6 + 3);
		modelo.indices.push_back(i * 6 + 1);
		modelo.indices.push_back(i * 6 + 4);
	}
}










void Geometrias::BuildCilindroTopo(float raioBase, float raioTopo, float altura, UINT nCortes, UINT nStack, Model & modelo)
{

	UINT baseIndex = (UINT)modelo.vertices.size();
	float y = 0.5f*altura;
	float dTheta = 2.0f*XM_PI / nCortes;

	Vertice vertex;
	///vertex.Cor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	for (UINT i = 0; i <= nCortes; ++i)
	{
		float x = raioTopo*cosf(i*dTheta);
		float z = raioTopo*sinf(i*dTheta);
		// Scale down by the height to try and make top cap
		// texture coord area proportional to base.
		float u = x / altura + 0.5f;
		float v = z / altura + 0.5f;
		
		vertex.Pos = XMFLOAT3(x, y, z);
		modelo.vertices.push_back(vertex);
				///Vertice(x, y, z,
				///0.0f, 1.0f, 0.0f,	///Normal
				///1.0f, 0.0f, 0.0f,	///T
				///u, v));				///UV
	}



	//
	//	Criando o vertice do meio (centro)
	//
	vertex.Pos = XMFLOAT3(0.0f, y, 0.0f);
	modelo.vertices.push_back(vertex);

	
	//Pegando o indice do vertice do centro
	UINT centerVertex = (UINT)modelo.vertices.size() - 1;


	for (UINT i = 0; i < nCortes; ++i) {
		modelo.indices.push_back(centerVertex);
		modelo.indices.push_back(baseIndex + i + 1);
		modelo.indices.push_back(baseIndex + i);
	}
}

void Geometrias::BuildCilindroBase(float raioBase, float raioTopo, float altura, UINT nCortes, UINT nStack, Model & modelo)
{
	UINT baseIndex = (UINT)modelo.vertices.size();
	float y = -0.5f*altura;
	float dTheta = 2.0f*XM_PI / nCortes;

	Vertice vertex;
	///vertex.Cor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	for (UINT i = 0; i <= nCortes; ++i)
	{
		float x = raioBase*cosf(i*dTheta);
		float z = raioBase*sinf(i*dTheta);
		// Scale down by the height to try and make top cap
		// texture coord area proportional to base.
		float u = x / altura + 0.5f;
		float v = z / altura + 0.5f;

		vertex.Pos = XMFLOAT3(x, y, z);
		modelo.vertices.push_back(vertex);
		///Vertice(x, y, z,
		///0.0f, -1.0f, 0.0f,	///Normal
		///1.0f, 0.0f, 0.0f,	///T
		///u, v));				///UV
	}



	//
	//	Criando o vertice do meio (centro)
	//
	vertex.Pos = XMFLOAT3(0.0f, y, 0.0f);
	modelo.vertices.push_back(vertex);


	//Pegando o indice do vertice do centro
	UINT centerVertex = (UINT)modelo.vertices.size() - 1;


	for (UINT i = 0; i < nCortes; ++i) {
		modelo.indices.push_back(centerVertex);
		modelo.indices.push_back(baseIndex + i);
		modelo.indices.push_back(baseIndex + i + 1);
	}
}

float Geometrias::GetHeight(float x, float z) {
	return 0.3f*(z*sinf(0.1f*x) + x*cosf(0.1f*z));
}

XMFLOAT3 Geometrias::GetHillNormal(float x, float z)
{
	// n = (-df/dx, 1, -df/dz)
	XMFLOAT3 n(
		-0.03f*z*cosf(0.1f*x) - 0.3f*cosf(0.1f*z),
		1.0f,
		-0.3f*sinf(0.1f*x) + 0.03f*x*sinf(0.1f*z));

	XMVECTOR unitNormal = XMVector3Normalize(XMLoadFloat3(&n));
	XMStoreFloat3(&n, unitNormal);

	return n;
}
