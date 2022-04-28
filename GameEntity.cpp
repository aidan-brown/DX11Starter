#include "GameEntity.h"

GameEntity::GameEntity(Mesh* mesh, std::shared_ptr<Material> material)
{
	this->mesh = mesh;
	this->transform = Transform(DirectX::XMFLOAT3(0, 0, 0));
	this->material = material;
}

GameEntity::GameEntity(Mesh* mesh, std::shared_ptr<Material> material, DirectX::XMFLOAT3 position)
{
	this->mesh = mesh;
	this->transform = Transform(position);
	this->material = material;
}

GameEntity::~GameEntity()
{
}

Mesh* GameEntity::GetMesh()
{
	return this->mesh;
}

Transform* GameEntity::GetTransform()
{
	return &(this->transform);
}

std::shared_ptr<Material> GameEntity::GetMaterial()
{
	return this->material;
}

void GameEntity::SetMaterial(std::shared_ptr<Material> material)
{
	this->material = material;
}

void GameEntity::Draw(std::shared_ptr<Camera> camera)
{
	std::shared_ptr<SimpleVertexShader> vs = this->material->GetVertexShader();
	std::shared_ptr<SimplePixelShader> ps = this->material->GetPixelShader();

	vs->SetMatrix4x4("worldMatrix", this->transform.GetWorldMatrix());
	vs->SetMatrix4x4("viewMatrix", camera->GetViewMatrix());
	vs->SetMatrix4x4("projectionMatrix", camera->GetProjectionMatrix());
	vs->SetMatrix4x4("worldInvTranspose", this->transform.GetWorldInverseTransposeMatrix());

	ps->SetFloat4("colorTint", this->material->GetColorTint());
	ps->SetFloat3("cameraPosition", camera->GetTransform().GetPosition());
	ps->SetFloat2("uvScale", this->material->GetUVScale());
	ps->SetFloat2("uvOffset", this->material->GetUVOffset());

	this->material->PrepareMaterial();

	vs->CopyAllBufferData();
	ps->CopyAllBufferData();

	vs->SetShader();
	ps->SetShader();

	this->mesh->Draw(this->transform, camera);
}
