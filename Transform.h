#pragma once
#define _USE_MATH_DEFINES

#include <DirectXMath.h>
#include <math.h>

class Transform {
public:
	Transform();
	Transform(DirectX::XMFLOAT3 position);
	Transform(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 scale);
	Transform(DirectX::XMFLOAT3 position, DirectX::XMFLOAT4 rotation);
	Transform(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT4 rotation);
	~Transform();

	void SetPosition(float x, float y, float z);
	void SetRotation(float pitch, float yaw, float roll);
	void SetScale(float x, float y, float z);

	void MoveAbsolute(float x, float y, float z);
	void MoveRelative(float x, float y, float z);
	void Rotate(float pitch, float yaw, float roll);
	void Scale(float x, float y, float z);

	void ClampPitch(float min, float max);

	DirectX::XMFLOAT3 GetPosition();
	DirectX::XMFLOAT4 GetRotation();
	DirectX::XMFLOAT3 GetScale();
	DirectX::XMFLOAT3 GetRight();
	DirectX::XMFLOAT3 GetUp();
	DirectX::XMFLOAT3 GetForward();

	DirectX::XMFLOAT4X4 GetWorldMatrix();
	DirectX::XMFLOAT4X4 GetWorldInverseTransposeMatrix();

private:
	void UpdateMatrices();
	DirectX::XMMATRIX XMMatrixTranslation();
	DirectX::XMMATRIX XMMatrixScaling();
	DirectX::XMMATRIX XMMatrixRotationRollPitchYaw();

	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT4X4 worldInverseTransposeMatrix;

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT4 rotation;
};