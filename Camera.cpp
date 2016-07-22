#include "Camera.h"



Camera::Camera(Mouse *_mouse) : posX(0.0f), posY(0.0f), posZ(0.0f), angulo(0.0f), raio(15.0f), mPhi(0.25f * XM_PI), mTheta(1.5f * XM_PI)
{
	mouse = _mouse;

	XMVECTOR pos = XMVectorSet(50.0f, 5.0f, 50.0f, 1.0f);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	cameraViewMatrix = XMMatrixLookAtLH(pos, XMVectorZero(), up);
}


Camera::~Camera()
{

}

void Camera::Update(float deltaTime)
{
	angulo += 0.025 * deltaTime;
	
	float x = sinf(angulo) * raio;
	float y = sinf(angulo) * raio;
	float z = cosf(angulo) * raio;
	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR alvo = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	cameraViewMatrix = XMMatrixLookAtLH(pos, alvo, up);
	
	/*
	XMVECTOR pos = XMVectorSet(posX, posY, posZ, 1.0f);
	XMVECTOR targ = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	cameraViewMatrix = XMMatrixLookAtLH(pos, targ, up);
	*/
	
}

void Camera::ChangePosition(float x, float y, float z)
{
	posX = x;
	posY = y;
	posZ = z;
}

XMMATRIX Camera::GetCameraView()
{
	return cameraViewMatrix;
}
