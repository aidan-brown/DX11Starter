#include "Camera.h"
#include <stdio.h>

Camera::Camera(float aspectRatio, DirectX::XMFLOAT3 position)
{
	Camera::transform = Transform(position);
	Camera::fov = M_PI_2;
	Camera::nearClip = 0.01f;
	Camera::farClip = 100.0f;
	Camera::movementSpeed = 1.0f;
	Camera::lookSpeed = 1.0f;
	Camera::UpdateViewMatrix();
	Camera::UpdateProjectionMatrix(aspectRatio);
}

Camera::Camera(float aspectRatio, DirectX::XMFLOAT3 position, float fov, float nearClip, float farClip, float movementSpeed, float lookSpeed)
{
	Camera::transform = Transform(position);
	Camera::fov = fov;
	Camera::nearClip = nearClip;
	Camera::farClip = farClip;
	Camera::movementSpeed = movementSpeed;
	Camera::lookSpeed = lookSpeed;
	Camera::UpdateViewMatrix();
	Camera::UpdateProjectionMatrix(aspectRatio);
}

Transform Camera::GetTransform()
{
	return Camera::transform;
}

DirectX::XMFLOAT4X4 Camera::GetViewMatrix()
{
	return Camera::viewMatrix;
}

DirectX::XMFLOAT4X4 Camera::GetProjectionMatrix()
{
	return Camera::projectionMatrix;
}

void Camera::UpdateProjectionMatrix(float aspectRatio)
{
	DirectX::XMMATRIX projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(Camera::fov, aspectRatio, Camera::nearClip, Camera::farClip);
	DirectX::XMStoreFloat4x4(&(Camera::projectionMatrix), projectionMatrix);
}

void Camera::UpdateViewMatrix()
{
	DirectX::XMFLOAT3 positionFloat3 = Camera::transform.GetPosition();
	DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&positionFloat3);
	
	DirectX::XMFLOAT3 directionFloat3 = Camera::transform.GetForward();
	DirectX::XMVECTOR direction = DirectX::XMLoadFloat3(&directionFloat3);

	DirectX::XMVECTOR worldUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
	DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixLookToLH(position, direction, worldUp);
	DirectX::XMStoreFloat4x4(&(Camera::viewMatrix), viewMatrix);
}

void Camera::Update(float dt)
{
	Input& input = Input::GetInstance();

	if (input.KeyDown('W')) {
		Camera::transform.MoveRelative(0.0f, 0.0f, 1.0f * Camera::movementSpeed * dt);
	}
	if (input.KeyDown('S')) {
		Camera::transform.MoveRelative(0.0f, 0.0f, -1.0f * Camera::movementSpeed * dt);
	}
	if (input.KeyDown('D')) {
		Camera::transform.MoveRelative(1.0f * Camera::movementSpeed * dt, 0.0f, 0.0f);
	}
	if (input.KeyDown('A')) {
		Camera::transform.MoveRelative(-1.0f * Camera::movementSpeed * dt, 0.0f, 0.0f);
	}
	if (input.KeyDown(VK_SPACE)) {
		Camera::transform.MoveAbsolute(0.0f, 1.0f * Camera::movementSpeed * dt, 0.0f);
	}
	if (input.KeyDown(VK_SHIFT)) {
		Camera::transform.MoveAbsolute(0.0f, -1.0f * Camera::movementSpeed * dt, 0.0f);
	}
	if (input.MouseLeftDown()) {
		float cursorMovementX = input.GetMouseXDelta() * Camera::lookSpeed * dt;
		float cursorMovementY = input.GetMouseYDelta() * Camera::lookSpeed * dt;
		Camera::transform.Rotate(cursorMovementY, cursorMovementX, 0.0f);
	}
	Camera::UpdateViewMatrix();
}
