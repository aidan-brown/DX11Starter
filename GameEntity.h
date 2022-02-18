#pragma once
#include "Transform.h"
#include "Mesh.h"
#include "Camera.h"

class GameEntity {
public:
	GameEntity(Mesh* mesh);
	~GameEntity();

	Mesh* GetMesh();
	Transform* GetTransform();

	void Draw(Camera* camera);

private:
	Transform transform;
	Mesh* mesh;

};