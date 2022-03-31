#include "Camera.h"
#include <stdio.h>

Camera::Camera(float aspectRatio, DirectX::XMFLOAT3 position)
{
	this->transform = Transform(position);
	this->fov = M_PI_2;
	this->nearClip = 0.01f;
	this->farClip = 100.0f;
	this->movementSpeed = 1.0f;
	this->lookSpeed = 1.0f;
	this->UpdateViewMatrix();
	this->UpdateProjectionMatrix(aspectRatio);
}

Camera::Camera(float aspectRatio, DirectX::XMFLOAT3 position, float fov, float nearClip, float farClip, float movementSpeed, float lookSpeed)
{
	this->transform = Transform(position);
	this->fov = fov;
	this->nearClip = nearClip;
	this->farClip = farClip;
	this->movementSpeed = movementSpeed;
	this->lookSpeed = lookSpeed;
	this->UpdateViewMatrix();
	this->UpdateProjectionMatrix(aspectRatio);
}

Transform Camera::GetTransform()
{
	return this->transform;
}

DirectX::XMFLOAT4X4 Camera::GetViewMatrix()
{
	return this->viewMatrix;
}

DirectX::XMFLOAT4X4 Camera::GetProjectionMatrix()
{
	return this->projectionMatrix;
}

void Camera::UpdateProjectionMatrix(float aspectRatio)
{
	DirectX::XMMATRIX projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(this->fov, aspectRatio, this->nearClip, this->farClip);
	DirectX::XMStoreFloat4x4(&(this->projectionMatrix), projectionMatrix);
}

void Camera::UpdateViewMatrix()
{
	DirectX::XMFLOAT3 positionFloat3 = this->transform.GetPosition();
	DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&positionFloat3);
	
	DirectX::XMFLOAT3 directionFloat3 = this->transform.GetForward();
	DirectX::XMVECTOR direction = DirectX::XMLoadFloat3(&directionFloat3);

	DirectX::XMVECTOR worldUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
	DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixLookToLH(position, direction, worldUp);
	DirectX::XMStoreFloat4x4(&(this->viewMatrix), viewMatrix);
}

void Camera::Update(float dt)
{
	Input& input = Input::GetInstance();

	if (input.KeyDown('W')) {
		this->transform.MoveRelative(0.0f, 0.0f, 1.0f * this->movementSpeed * dt);
	}
	if (input.KeyDown('S')) {
		this->transform.MoveRelative(0.0f, 0.0f, -1.0f * this->movementSpeed * dt);
	}
	if (input.KeyDown('D')) {
		this->transform.MoveRelative(1.0f * this->movementSpeed * dt, 0.0f, 0.0f);
	}
	if (input.KeyDown('A')) {
		this->transform.MoveRelative(-1.0f * this->movementSpeed * dt, 0.0f, 0.0f);
	}
	if (input.KeyDown(VK_SPACE)) {
		this->transform.MoveAbsolute(0.0f, 1.0f * this->movementSpeed * dt, 0.0f);
	}
	if (input.KeyDown(VK_SHIFT)) {
		this->transform.MoveAbsolute(0.0f, -1.0f * this->movementSpeed * dt, 0.0f);
	}
	if (input.MouseLeftDown()) {
		float cursorMovementX = input.GetMouseXDelta() * this->lookSpeed * dt;
		float cursorMovementY = input.GetMouseYDelta() * this->lookSpeed * dt;
		this->transform.Rotate(cursorMovementY, cursorMovementX, 0.0f);
	}
	this->UpdateViewMatrix();
}
