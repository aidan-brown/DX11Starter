#pragma once
#include "Transform.h"
#include "Mesh.h"

class GameEntity {
public:
	GameEntity(Mesh* mesh);
	~GameEntity();

	Mesh* GetMesh();
	Transform* GetTransform();

	void Draw();

private:
	Transform transform;
	Mesh* mesh;

};