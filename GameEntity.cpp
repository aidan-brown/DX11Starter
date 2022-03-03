#include "GameEntity.h"

GameEntity::GameEntity(Mesh* mesh, std::shared_ptr<Material> material)
{
	GameEntity::mesh = mesh;
	GameEntity::transform = Transform(DirectX::XMFLOAT3(0, 0, 0));
	GameEntity::material = material;
}

GameEntity::GameEntity(Mesh* mesh, std::shared_ptr<Material> material, DirectX::XMFLOAT3 position)
{
	GameEntity::mesh = mesh;
	GameEntity::transform = Transform(position);
	GameEntity::material = material;
}

GameEntity::~GameEntity()
{
}

Mesh* GameEntity::GetMesh()
{
	return GameEntity::mesh;
}

Transform* GameEntity::GetTransform()
{
	return &(GameEntity::transform);
}

std::shared_ptr<Material> GameEntity::GetMaterial()
{
	return GameEntity::material;
}

void GameEntity::SetMaterial(std::shared_ptr<Material> material)
{
	GameEntity::material = material;
}

void GameEntity::Draw(std::shared_ptr<Camera> camera)
{
	std::shared_ptr<SimpleVertexShader> vs = GameEntity::material->GetVertexShader();
	std::shared_ptr<SimplePixelShader> ps = GameEntity::material->GetPixelShader();

	vs->SetMatrix4x4("worldMatrix", GameEntity::transform.GetWorldMatrix());
	vs->SetMatrix4x4("viewMatrix", camera->GetViewMatrix());
	vs->SetMatrix4x4("projectionMatrix", camera->GetProjectionMatrix());

	ps->SetFloat4("colorTint", GameEntity::material->GetColorTint());

	vs->CopyAllBufferData();
	ps->CopyAllBufferData();

	vs->SetShader();
	ps->SetShader();

	GameEntity::mesh->Draw(GameEntity::transform, camera);
}
