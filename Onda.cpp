#include "Onda.h"
#include <algorithm>
#include <vector>
#include <cassert>
#include "Vertice.h"
#include "Definicoes.h"
#include "MathHead.h"

Onda::Onda()
	: mNumRows(0), mNumCols(0), mVertexCount(0), mTriangleCount(0),
	mK1(0.0f), mK2(0.0f), mK3(0.0f), mTimeStep(0.0f), mSpatialStep(0.0f),
	mPrevSolution(0), mCurrSolution(0), nIndices(0)
{
}

Onda::~Onda()
{
	delete[] mPrevSolution;
	delete[] mCurrSolution;

	ReleaseCOM(vertexBuffer);
	ReleaseCOM(indexBuffer);
}

UINT Onda::RowCount()const
{
	return mNumRows;
}

UINT Onda::ColumnCount()const
{
	return mNumCols;
}

UINT Onda::VertexCount()const
{
	return mVertexCount;
}

UINT Onda::TriangleCount()const
{
	return mTriangleCount;
}

void Onda::Init(UINT m, UINT n, float dx, float dt, float speed, float damping, ID3D11Device *device)
{
	mNumRows = m;
	mNumCols = n;

	mVertexCount = m*n;
	mTriangleCount = (m - 1)*(n - 1) * 2;

	mTimeStep = dt;
	mSpatialStep = dx;

	float d = damping*dt + 2.0f;
	float e = (speed*speed)*(dt*dt) / (dx*dx);
	mK1 = (damping*dt - 2.0f) / d;
	mK2 = (4.0f - 8.0f*e) / d;
	mK3 = (2.0f*e) / d;

	// In case Init() called again.
	delete[] mPrevSolution;
	delete[] mCurrSolution;

	mPrevSolution = new XMFLOAT3[m*n];
	mCurrSolution = new XMFLOAT3[m*n];

	// Generate grid vertices in system memory.

	float halfWidth = (n - 1)*dx*0.5f;
	float halfDepth = (m - 1)*dx*0.5f;
	for (UINT i = 0; i < m; ++i)
	{
		float z = halfDepth - i*dx;
		for (UINT j = 0; j < n; ++j)
		{
			float x = -halfWidth + j*dx;

			mPrevSolution[i*n + j] = XMFLOAT3(x, 0.0f, z);
			mCurrSolution[i*n + j] = XMFLOAT3(x, 0.0f, z);
		}
	}












	//
	//	Building
	//

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(Vertice) * VertexCount();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;
	device->CreateBuffer(&vbd, 0, &vertexBuffer);


	// Create the index buffer.  The index buffer is fixed, so we only 
	// need to create and set once.

	std::vector<UINT> indices(3 * TriangleCount()); // 3 indices per face

														   // Iterate over each quad.
	UINT mm = RowCount();
	UINT nn = ColumnCount();
	int k = 0;
	for (UINT i = 0; i < mm - 1; ++i)
	{
		for (DWORD j = 0; j < nn - 1; ++j)
		{
			indices[k] = i*nn + j;
			indices[k + 1] = i*nn + j + 1;
			indices[k + 2] = (i + 1)*nn + j;

			indices[k + 3] = (i + 1)*nn + j;
			indices[k + 4] = i*nn + j + 1;
			indices[k + 5] = (i + 1)*nn + j + 1;

			k += 6; // next quad
		}
	}


	nIndices = indices.size();
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * indices.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	device->CreateBuffer(&ibd, &iinitData, &indexBuffer);
}

void Onda::Draw(ID3D11DeviceContext * context)
{

	//
	// Update the wave vertex buffer with the new solution.
	//

	D3D11_MAPPED_SUBRESOURCE mappedData;
	context->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);

	Vertice* v = reinterpret_cast<Vertice*>(mappedData.pData);
	for (UINT i = 0; i < VertexCount(); ++i)
	{
		v[i].Pos = mCurrSolution[i];
		v[i].Cor = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	}

	context->Unmap(vertexBuffer, 0);

	UINT strides = sizeof(Vertice);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 0, &vertexBuffer, &strides, &offset);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
}

void Onda::Update(float dt)
{
	static float t = 0;

	// Accumulate time.
	t += dt;

	// Only update the simulation at the specified time step.
	if (t >= mTimeStep)
	{
		// Only update interior points; we use zero boundary conditions.
		for (DWORD i = 1; i < mNumRows - 1; ++i)
		{
			for (DWORD j = 1; j < mNumCols - 1; ++j)
			{
				// After this update we will be discarding the old previous
				// buffer, so overwrite that buffer with the new update.
				// Note how we can do this inplace (read/write to same element) 
				// because we won't need prev_ij again and the assignment happens last.

				// Note j indexes x and i indexes z: h(x_j, z_i, t_k)
				// Moreover, our +z axis goes "down"; this is just to 
				// keep consistent with our row indices going down.

				mPrevSolution[i*mNumCols + j].y =
					mK1*mPrevSolution[i*mNumCols + j].y +
					mK2*mCurrSolution[i*mNumCols + j].y +
					mK3*(mCurrSolution[(i + 1)*mNumCols + j].y +
						mCurrSolution[(i - 1)*mNumCols + j].y +
						mCurrSolution[i*mNumCols + j + 1].y +
						mCurrSolution[i*mNumCols + j - 1].y);
			}
		}

		// We just overwrote the previous buffer with the new data, so
		// this data needs to become the current solution and the old
		// current solution becomes the new previous solution.
		std::swap(mPrevSolution, mCurrSolution);

		t = 0.0f; // reset time
	}
}

void Onda::Disturb(UINT i, UINT j, float magnitude)
{
	// Don't disturb boundaries.
	assert(i > 1 && i < mNumRows - 2);
	assert(j > 1 && j < mNumCols - 2);

	float halfMag = 0.5f*magnitude;

	// Disturb the ijth vertex height and its neighbors.
	mCurrSolution[i*mNumCols + j].y += magnitude;
	mCurrSolution[i*mNumCols + j + 1].y += halfMag;
	mCurrSolution[i*mNumCols + j - 1].y += halfMag;
	mCurrSolution[(i + 1)*mNumCols + j].y += halfMag;
	mCurrSolution[(i - 1)*mNumCols + j].y += halfMag;
}

void Onda::SetWorldMatrix(const XMMATRIX& mWorld)
{
	worldMatrix = mWorld;
}

XMMATRIX Onda::GetWorldMatrix()
{
	return worldMatrix;
}

