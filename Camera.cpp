#include "Camera.h"



Camera::Camera() : posX(0.0f), posY(0.0f), posZ(0.0f)
{

}


Camera::~Camera()
{
}

void Camera::Update()
{
	XMVECTOR pos = XMVectorSet(posX, posY, posZ, 1.0f);
	XMVECTOR targ = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	cameraViewMatrix = XMMatrixLookAtLH(pos, targ, up);
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
