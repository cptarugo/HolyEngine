#ifndef _CAMERA_H
#define _CAMERA_H

#pragma once


#include <d3dx11.h>
#include <xnamath.h>
#include "Definicoes.h";
#include "Mouse.h"

class Camera
{
public:
	Camera(Mouse *_mouse);
	~Camera();

	void Update(float deltaTime);
	void ChangePosition(float x, float y, float z);

	XMMATRIX GetCameraView();

	float raio;
private:
	float posX, posY, posZ, angulo;
	float mPhi, mTheta;

	XMMATRIX cameraViewMatrix;

	Mouse *mouse;
};

#endif