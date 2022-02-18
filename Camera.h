#pragma once
#define _USE_MATH_DEFINES

#include "Input.h";
#include "Transform.h";
#include <DirectXMath.h>
#include <math.h>

class Camera {
public:
	Camera(float aspectRatio, DirectX::XMFLOAT3 position);
	Camera(float aspectRatio, DirectX::XMFLOAT3 position, float fov, float nearClip, float farClip, float movementSpeed, float lookSpeed);

	DirectX::XMFLOAT4X4 GetViewMatrix();
	DirectX::XMFLOAT4X4 GetProjectionMatrix();

	void UpdateProjectionMatrix(float aspectRatio);
	void UpdateViewMatrix();
	void Update(float dt);

private:
	Transform transform;
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;

	float fov;
	float nearClip;
	float farClip;
	float movementSpeed;
	float lookSpeed;
};