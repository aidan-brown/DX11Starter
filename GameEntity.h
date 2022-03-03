#pragma once
#include <memory>
#include "Transform.h"
#include "Mesh.h"
#include "Camera.h"
#include "Material.h"

class GameEntity {
public:
	GameEntity(Mesh* mesh, std::shared_ptr<Material> material);
	GameEntity(Mesh* mesh, std::shared_ptr<Material> material, DirectX::XMFLOAT3 position);
	~GameEntity();

	Mesh* GetMesh();
	Transform* GetTransform();
	std::shared_ptr<Material> GetMaterial();

	void SetMaterial(std::shared_ptr<Material> material);
	void Draw(std::shared_ptr<Camera>);

private:
	Transform transform;
	Mesh* mesh;
	std::shared_ptr<Material> material;
};