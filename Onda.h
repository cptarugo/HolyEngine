#ifndef _ONDA_H
#define _ONDA_H

#pragma once

#include <Windows.h>
#include <xnamath.h>
#include <d3dx11.h>

class Onda
{
public:
	Onda();
	~Onda();

	UINT RowCount()const;
	UINT ColumnCount()const;
	UINT VertexCount()const;
	UINT TriangleCount()const;

	// Returns the solution at the ith grid point.
	const XMFLOAT3& operator[](int i)const { return mCurrSolution[i]; }

	void Init(UINT m, UINT n, float dx, float dt, float speed, float damping, ID3D11Device *device);
	void Draw(ID3D11DeviceContext *context);
	void Update(float dt);
	void Disturb(UINT i, UINT j, float magnitude);


	void SetWorldMatrix(const XMMATRIX& mWorld);
	XMMATRIX GetWorldMatrix();

	UINT nIndices;

private:
	UINT mNumRows;
	UINT mNumCols;

	UINT mVertexCount;
	UINT mTriangleCount;

	ID3D11Buffer *vertexBuffer;
	ID3D11Buffer *indexBuffer;

	XMMATRIX worldMatrix;

	// Simulation constants we can precompute.
	float mK1;
	float mK2;
	float mK3;

	float mTimeStep;
	float mSpatialStep;

	XMFLOAT3* mPrevSolution;
	XMFLOAT3* mCurrSolution;
};

#endif