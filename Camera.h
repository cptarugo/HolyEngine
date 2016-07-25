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
	void IncPosition(float x, float y, float z);
	void MoverFrente(bool keyDown);
	void RotacionarEsq();

	XMMATRIX GetCameraView();

	float raio;
	float mPhi, mTheta;
private:
	float posX, posY, posZ, angulo, forwardVelocity;
	float rotX, rotY, rotZ, rotVelocity;
	
	float frameRate;

	XMMATRIX cameraViewMatrix;

	Mouse *mouse;
};

#endif