#pragma once

#include "DXCore.h"
#include <DirectXMath.h>
#include <wrl/client.h> // Used for ComPtr - a smart pointer for COM objects
#include <memory>
#include "Mesh.h"
#include "BufferStructs.h"
#include "GameEntity.h"
#include "Camera.h"
#include "SimpleShader.h"
#include "Material.h"
#include "Lights.h"

class Game 
	: public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

private:

	// Should we use vsync to limit the frame rate?
	bool vsync;

	// Initialization helper methods - feel free to customize, combine, etc.
	void LoadShaders(); 
	void LoadMeshes();

	// Note the usage of ComPtr below
	//  - This is a smart pointer for objects that abide by the
	//    Component Object Model, which DirectX objects do
	//  - More info here: https://github.com/Microsoft/DirectXTK/wiki/ComPtr
	
	// Shaders and shader-related constructs
	std::shared_ptr<SimplePixelShader> pixelShader, customPixelShader;
	std::shared_ptr<SimpleVertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBufferVS;

	std::shared_ptr<Mesh> cube, cylinder, helix, quad, quadDoubleSided, sphere, torus;
	std::shared_ptr<Material> matWhite, matRed, matGreen, matBlue;
	std::vector<std::shared_ptr<GameEntity>> gameEntities;
	std::shared_ptr<Camera> camera;

	DirectX::XMFLOAT3 ambientColor;
	std::vector<Light> lights;
};

