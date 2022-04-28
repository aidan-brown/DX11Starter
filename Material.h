#pragma once
#include <memory>
#include "DXCore.h"
#include "SimpleShader.h"

class Material {
public:
	Material(std::shared_ptr<SimpleVertexShader> vertexShader, std::shared_ptr<SimplePixelShader> pixelShader, DirectX::XMFLOAT4 colorTint, DirectX::XMFLOAT2 uvScale, DirectX::XMFLOAT2 uvOffset);
	~Material();

	void SetColorTint(DirectX::XMFLOAT4 colorTint);
	void SetUVScale(DirectX::XMFLOAT2 uvScale);
	void SetUVOffset(DirectX::XMFLOAT2 uvOffset);
	void SetVertexShader(std::shared_ptr<SimpleVertexShader> vertexShader);
	void SetPixelShader(std::shared_ptr<SimplePixelShader> pixelShader);

	DirectX::XMFLOAT4 GetColorTint();
	DirectX::XMFLOAT2 GetUVScale();
	DirectX::XMFLOAT2 GetUVOffset();
	std::shared_ptr<SimpleVertexShader> GetVertexShader();
	std::shared_ptr<SimplePixelShader> GetPixelShader();

	void AddTextureSRV(std::string shaderName, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRV);
	void AddSampler(std::string shaderName, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler);
	void PrepareMaterial();

private:
	DirectX::XMFLOAT4 colorTint;
	DirectX::XMFLOAT2 uvOffset;
	DirectX::XMFLOAT2 uvScale;
	std::shared_ptr<SimpleVertexShader> vertexShader;
	std::shared_ptr<SimplePixelShader> pixelShader;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> textureSRVs;
	std::unordered_map < std::string, Microsoft::WRL::ComPtr<ID3D11SamplerState>> samplers;
};