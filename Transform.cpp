#include "Transform.h"
#include <algorithm>

Transform::Transform()
{
	this->position = DirectX::XMFLOAT3(0, 0, 0);
	this->scale = DirectX::XMFLOAT3(1, 1, 1);
	this->rotation = DirectX::XMFLOAT4(0, 0, 0, 1);

	DirectX::XMStoreFloat4x4(&worldMatrix, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&worldInverseTransposeMatrix, DirectX::XMMatrixIdentity());
}

Transform::Transform(DirectX::XMFLOAT3 position)
{
	this->position = position;
	this->scale = DirectX::XMFLOAT3(1, 1, 1);
	this->rotation = DirectX::XMFLOAT4(0, 0, 0, 1);
	
	DirectX::XMStoreFloat4x4(&worldMatrix, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&worldInverseTransposeMatrix, DirectX::XMMatrixIdentity());
}

Transform::Transform(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 scale)
{
	this->position = position;
	this->scale = scale;
	this->rotation = DirectX::XMFLOAT4(0, 0, 0, 1);
	
	DirectX::XMStoreFloat4x4(&worldMatrix, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&worldInverseTransposeMatrix, DirectX::XMMatrixIdentity());
}

Transform::Transform(DirectX::XMFLOAT3 position, DirectX::XMFLOAT4 rotation)
{
	this->position = position;
	this->scale = DirectX::XMFLOAT3(1, 1, 1);
	this->rotation = rotation;
	
	DirectX::XMStoreFloat4x4(&worldMatrix, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&worldInverseTransposeMatrix, DirectX::XMMatrixIdentity());
}

Transform::Transform(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT4 rotation)
{
	this->position = position;
	this->scale = scale;
	this->rotation = rotation;
	
	DirectX::XMStoreFloat4x4(&worldMatrix, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&worldInverseTransposeMatrix, DirectX::XMMatrixIdentity());
}

Transform::~Transform()
{
}

void Transform::SetPosition(float x, float y, float z)
{
	this->position = DirectX::XMFLOAT3(x, y, z);
}

void Transform::SetRotation(float pitch, float yaw, float roll)
{
	this->rotation = DirectX::XMFLOAT4(pitch, yaw, roll, 1);
}

void Transform::SetScale(float x, float y, float z)
{
	this->scale = DirectX::XMFLOAT3(x, y, z);
}

void Transform::MoveAbsolute(float x, float y, float z)
{
	this->position = DirectX::XMFLOAT3(this->position.x + x, this->position.y + y, this->position.z + z);
}

void Transform::MoveRelative(float x, float y, float z)
{
	DirectX::XMVECTOR direction = DirectX::XMVectorSet(x, y, z, 1.0f);
	DirectX::XMVECTOR rotation = DirectX::XMQuaternionRotationRollPitchYaw(this->rotation.x, this->rotation.y, this->rotation.z);
	DirectX::XMVECTOR rotatedDirection = DirectX::XMVector3Rotate(direction, rotation);
	rotatedDirection = DirectX::XMVectorAdd(rotatedDirection, DirectX::XMLoadFloat3(&(this->position)));
	DirectX::XMStoreFloat3(&(this->position), rotatedDirection);
}

void Transform::Rotate(float pitch, float yaw, float roll)
{	
	this->rotation = DirectX::XMFLOAT4(this->rotation.x + pitch, this->rotation.y + yaw, this->rotation.z + roll, 1);
}

void Transform::Scale(float x, float y, float z)
{
	this->scale = DirectX::XMFLOAT3(this->scale.x + x, this->scale.y + y, this->scale.z + z);
}

DirectX::XMFLOAT3 Transform::GetPosition()
{
	return this->position;
}

DirectX::XMFLOAT4 Transform::GetRotation()
{
	return this->rotation;
}

DirectX::XMFLOAT3 Transform::GetScale()
{
	return this->scale;
}

DirectX::XMFLOAT3 Transform::GetRight()
{
	DirectX::XMVECTOR worldRight = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f);
	DirectX::XMVECTOR rotation = DirectX::XMQuaternionRotationRollPitchYaw(this->rotation.x, this->rotation.y, this->rotation.z);
	DirectX::XMVECTOR localRight = DirectX::XMVector3Rotate(worldRight, rotation);
	DirectX::XMFLOAT3 localRightFloat3;
	DirectX::XMStoreFloat3(&localRightFloat3, localRight);
	return localRightFloat3;
}

DirectX::XMFLOAT3 Transform::GetUp()
{
	DirectX::XMVECTOR worldUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
	DirectX::XMVECTOR rotation = DirectX::XMQuaternionRotationRollPitchYaw(this->rotation.x, this->rotation.y, this->rotation.z);
	DirectX::XMVECTOR localUp = DirectX::XMVector3Rotate(worldUp, rotation);
	DirectX::XMFLOAT3 localUpFloat3;
	DirectX::XMStoreFloat3(&localUpFloat3, localUp);
	return localUpFloat3;
}

DirectX::XMFLOAT3 Transform::GetForward()
{
	DirectX::XMVECTOR worldForward = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);
	DirectX::XMVECTOR rotation = DirectX::XMQuaternionRotationRollPitchYaw(this->rotation.x, this->rotation.y, this->rotation.z);
	DirectX::XMVECTOR localForward = DirectX::XMVector3Rotate(worldForward, rotation);
	DirectX::XMFLOAT3 localForwardFloat3;
	DirectX::XMStoreFloat3(&localForwardFloat3, localForward);
	return localForwardFloat3;
}

DirectX::XMFLOAT4X4 Transform::GetWorldMatrix()
{
	this->UpdateMatrices();
	return worldMatrix;
}

DirectX::XMFLOAT4X4 Transform::GetWorldInverseTransposeMatrix()
{
	this->UpdateMatrices();
	return worldInverseTransposeMatrix;
}

void Transform::UpdateMatrices()
{
	DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslation(this->position.x, this->position.y, this->position.z);
	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(this->rotation.x, this->rotation.y, this->rotation.z);
	DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScaling(this->scale.x, this->scale.y, this->scale.z);
	DirectX::XMMATRIX world = scaleMatrix * rotationMatrix * translationMatrix;

	DirectX::XMStoreFloat4x4(&worldMatrix, world);
	DirectX::XMStoreFloat4x4(&worldInverseTransposeMatrix, DirectX::XMMatrixInverse(0, DirectX::XMMatrixTranspose(world)));
}

void Transform::ClampPitch(float min, float max) 
{
	float pitch = this->rotation.x;
	pitch = std::max(min, std::min(pitch, max));
	this->rotation.x = pitch;
}