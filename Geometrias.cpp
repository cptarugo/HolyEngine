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



void Geometrias::Height(float width, float depth, UINT m, UINT n, Model & modelo)
{
	CriarGrid(width, depth, m, n, modelo);


	std::vector<Vertice> vertices(modelo.vertices.size());

	for (size_t i = 0; i < modelo.vertices.size(); ++i) {
		
		XMFLOAT3 p = modelo.vertices[i].Pos;

		p.y = GetHeight(p.x, p.z);

		vertices[i].Pos = p;


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
	}

	modelo.vertices = vertices;
}



float Geometrias::GetHeight(float x, float z) {
	return 0.3f*(z*sinf(0.1f*x) + x*cosf(0.1f*z));
}