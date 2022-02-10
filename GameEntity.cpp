#include "GameEntity.h"

GameEntity::GameEntity(Mesh* mesh)
{
	GameEntity::mesh = mesh;
	GameEntity::transform = Transform(DirectX::XMFLOAT3(0, 0, 0));
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

void GameEntity::Draw()
{
	GameEntity::mesh->Draw(GameEntity::transform);
}
