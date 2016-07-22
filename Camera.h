#ifndef _CAMERA_H
#define _CAMERA_H

#pragma once


#include <d3dx11.h>
#include <xnamath.h>
#include "Definicoes.h";

class Camera
{
public:
	Camera();
	~Camera();

	void Update();
	void ChangePosition(float x, float y, float z);

	XMMATRIX GetCameraView();

private:
	float posX, posY, posZ;
	XMMATRIX cameraViewMatrix;

};

#endif