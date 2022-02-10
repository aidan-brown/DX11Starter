#include "Transform.h"

Transform::Transform()
{
	Transform::position = DirectX::XMFLOAT3(0, 0, 0);
	Transform::scale = DirectX::XMFLOAT3(1, 1, 1);
	Transform::rotation = DirectX::XMFLOAT4(0, 0, 0, 1);

	DirectX::XMStoreFloat4x4(&worldMatrix, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&worldInverseTransposeMatrix, DirectX::XMMatrixIdentity());
}

Transform::Transform(DirectX::XMFLOAT3 position)
{
	Transform::position = position;
	Transform::scale = DirectX::XMFLOAT3(1, 1, 1);
	Transform::rotation = DirectX::XMFLOAT4(0, 0, 0, 1);
	
	DirectX::XMStoreFloat4x4(&worldMatrix, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&worldInverseTransposeMatrix, DirectX::XMMatrixIdentity());
}

Transform::Transform(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 scale)
{
	Transform::position = position;
	Transform::scale = scale;
	Transform::rotation = DirectX::XMFLOAT4(0, 0, 0, 1);
	
	DirectX::XMStoreFloat4x4(&worldMatrix, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&worldInverseTransposeMatrix, DirectX::XMMatrixIdentity());
}

Transform::Transform(DirectX::XMFLOAT3 position, DirectX::XMFLOAT4 rotation)
{
	Transform::position = position;
	Transform::scale = DirectX::XMFLOAT3(1, 1, 1);
	Transform::rotation = rotation;
	
	DirectX::XMStoreFloat4x4(&worldMatrix, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&worldInverseTransposeMatrix, DirectX::XMMatrixIdentity());
}

Transform::Transform(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT4 rotation)
{
	Transform::position = position;
	Transform::scale = scale;
	Transform::rotation = rotation;
	
	DirectX::XMStoreFloat4x4(&worldMatrix, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&worldInverseTransposeMatrix, DirectX::XMMatrixIdentity());
}

Transform::~Transform()
{
}

void Transform::SetPosition(float x, float y, float z)
{
	Transform::position = DirectX::XMFLOAT3(x, y, z);
}

void Transform::SetRotation(float pitch, float yaw, float roll)
{
	Transform::rotation = DirectX::XMFLOAT4(pitch, yaw, roll, 1);
}

void Transform::SetScale(float x, float y, float z)
{
	Transform::scale = DirectX::XMFLOAT3(x, y, z);
}

void Transform::MoveAbsolute(float x, float y, float z)
{
	Transform::position = DirectX::XMFLOAT3(Transform::position.x + x, Transform::position.y + y, Transform::position.z + z);
}

void Transform::Rotate(float pitch, float yaw, float roll)
{
	Transform::rotation = DirectX::XMFLOAT4(Transform::rotation.x + pitch, Transform::rotation.y + yaw, Transform::rotation.z + roll, 1);
}

void Transform::Scale(float x, float y, float z)
{
	Transform::scale = DirectX::XMFLOAT3(Transform::scale.x + x, Transform::scale.y + y, Transform::scale.z + z);
}

DirectX::XMFLOAT3 Transform::GetPosition()
{
	return position;
}

DirectX::XMFLOAT4 Transform::GetRotation()
{
	return rotation;
}

DirectX::XMFLOAT3 Transform::GetScale()
{
	return scale;
}

DirectX::XMFLOAT4X4 Transform::GetWorldMatrix()
{
	Transform::UpdateMatrices();
	return worldMatrix;
}

DirectX::XMFLOAT4X4 Transform::GetWorldInverseTransposeMatrix()
{
	Transform::UpdateMatrices();
	return worldInverseTransposeMatrix;
}

void Transform::UpdateMatrices()
{
	DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslation(Transform::position.x, Transform::position.y, Transform::position.z);
	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(Transform::rotation.x, Transform::rotation.y, Transform::rotation.z);
	DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScaling(Transform::scale.x, Transform::scale.y, Transform::scale.z);
	DirectX::XMMATRIX world = scaleMatrix * rotationMatrix * translationMatrix;

	DirectX::XMStoreFloat4x4(&worldMatrix, world);
	DirectX::XMStoreFloat4x4(&worldInverseTransposeMatrix, DirectX::XMMatrixInverse(0, DirectX::XMMatrixTranspose(world)));
}