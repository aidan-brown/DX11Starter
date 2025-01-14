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
#include "WICTextureLoader.h"
#include "Sky.h"

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

	// Post processing resources for bloom
	static const int MaxBloomLevels = 5;

	bool drawBloomTextures = true;
	int bloomLevels = 5;
	float bloomThreshold = 0.75f;
	float bloomLevelIntensities[MaxBloomLevels] = { 1, 1, 1, 1, 1 };

	Microsoft::WRL::ComPtr<ID3D11SamplerState> ppSampler; // Clamp sampler for post processing

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> ppRTV;		// Allows us to render to a texture
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ppSRV;		// Allows us to sample from the same texture

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> bloomExtractRTV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> bloomExtractSRV;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> blurHorizontalRTV[MaxBloomLevels];
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> blurHorizontalSRV[MaxBloomLevels];

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> blurVerticalRTV[MaxBloomLevels];
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> blurVerticalSRV[MaxBloomLevels];

	// Should we use vsync to limit the frame rate?
	bool vsync;

	// Initialization helper methods - feel free to customize, combine, etc.
	void LoadShaders(); 
	void LoadMeshes();
	void ResizeAllPostProcessResources();
	void ResizeOnePostProcessResource(Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& rtv, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& srv, float renderTargetScale, DXGI_FORMAT format);
	void BloomExtract();
	void SingleDirectionBlur(float renderTargetScale, DirectX::XMFLOAT2 blurDirection, Microsoft::WRL::ComPtr<ID3D11RenderTargetView> target, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> sourceTexture);
	void BloomCombine();

	// Helper for creating a cubemap from 6 individual textures
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> CreateCubemap(
		const wchar_t* right,
		const wchar_t* left,
		const wchar_t* up,
		const wchar_t* down,
		const wchar_t* front,
		const wchar_t* back);

	// Note the usage of ComPtr below
	//  - This is a smart pointer for objects that abide by the
	//    Component Object Model, which DirectX objects do
	//  - More info here: https://github.com/Microsoft/DirectXTK/wiki/ComPtr
	
	// Shaders and shader-related constructs
	std::shared_ptr<SimplePixelShader> pixelShader, skyPixelShader, gaussianBlurPS, bloomExtractPS, bloomCombinePS;
	std::shared_ptr<SimpleVertexShader> vertexShader, skyVertexShader, fullscreenVS;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBufferVS;

	std::shared_ptr<Mesh> cube, cylinder, helix, quad, quadDoubleSided, sphere, torus, shuttle;
	std::shared_ptr<Material> matStone, matMetal, matStarship;
	std::shared_ptr<Sky> sky;
	std::vector<std::shared_ptr<GameEntity>> gameEntities;
	std::shared_ptr<Camera> camera;

	DirectX::XMFLOAT3 ambientColor;
	std::vector<Light> lights;
};

