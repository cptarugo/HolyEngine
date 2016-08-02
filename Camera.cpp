#include "Camera.h"



Camera::Camera(Mouse *_mouse) : posX(0.0f), posY(15.0f), posZ(15.0f), angulo(0.0f), raio(15.0f), forwardVelocity(0.0f), rotVelocity(0.0f), frameRate(0.0f), mPhi(0.25f * XM_PI), mTheta(1.5f * XM_PI),
								rotX(0.0f), rotY(0.0f), rotZ(0.0f)
{
	mouse = _mouse;

	XMVECTOR pos = XMVectorSet(0.0f, 15.0f, 15.0f, 1.0f);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);


	cameraViewMatrix = XMMatrixLookAtLH(pos, XMVectorZero(), up);
}


Camera::~Camera()
{

}

void Camera::Update(float deltaTime)
{
	frameRate = deltaTime;

	
	float x = raio*sinf(mPhi)*cosf(mTheta);
	float z = raio*sinf(mPhi)*sinf(mTheta);
	float y = raio*cosf(mPhi);

	// Build the view matrix.
	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMStoreFloat3(&gEyeW, XMVector3Normalize(target - pos));
	cameraViewMatrix = XMMatrixLookAtLH(pos, target, up) * XMMatrixTranslation(posX, posY, posZ);
}




void Camera::MoverFrente(bool keyDown)
{
	if (keyDown) {
		forwardVelocity += frameRate * 0.001f;
		if (forwardVelocity > frameRate * 0.05f)
			forwardVelocity = frameRate * 0.05f;
	}
	else {
		forwardVelocity += frameRate * 0.0008f;
		if (forwardVelocity < 0.0f)
			forwardVelocity = 0.0f;
	}

	float radians = XMConvertToRadians(rotY);

	posX += sinf(radians) * forwardVelocity;
	posZ += cosf(radians) * forwardVelocity;
}



void Camera::RotacionarEsq()
{
	rotVelocity += 0.00025 * frameRate;
	if (rotVelocity > frameRate * 0.25f)
		rotVelocity = frameRate * 0.25f;

	rotY += rotVelocity;
	if (rotY > 360.0f)
		rotY -= 360.0f;
}

XMFLOAT3 Camera::ReturnPosicao()
{
	return XMFLOAT3(posX, posY, posZ);
}

//Retorna a direcao da camera (onde esta olhando)
XMFLOAT3 Camera::ReturnGEyeW()
{
	return gEyeW;
}




XMMATRIX Camera::GetCameraView()
{
	return cameraViewMatrix;
}

void Camera::ChangePosition(float x, float y, float z)
{
	posX = x;
	posY = y;
	posZ = z;
}


void Camera::IncPosition(float x, float y, float z)
{
	posX += x;
	posY += y;
	posZ += z;
}