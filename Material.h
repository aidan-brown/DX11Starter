#pragma once
#include <memory>
#include "DXCore.h"
#include "SimpleShader.h"

class Material {
public:
	Material(DirectX::XMFLOAT4 colorTint, float roughness, std::shared_ptr<SimpleVertexShader> vertexShader, std::shared_ptr<SimplePixelShader> pixelShader);
	~Material();

	void SetColorTint(DirectX::XMFLOAT4 colorTint);
	void SetRoughness(float roughness);
	void SetVertexShader(std::shared_ptr<SimpleVertexShader> vertexShader);
	void SetPixelShader(std::shared_ptr<SimplePixelShader> pixelShader);

	DirectX::XMFLOAT4 GetColorTint();
	float GetRoughness();
	std::shared_ptr<SimpleVertexShader> GetVertexShader();
	std::shared_ptr<SimplePixelShader> GetPixelShader();

	void AddTextureSRV(std::string shaderName, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRV);
	void AddSampler(std::string shaderName, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler);
	void PrepareMaterial();

private:
	DirectX::XMFLOAT4 colorTint;
	std::shared_ptr<SimpleVertexShader> vertexShader;
	std::shared_ptr<SimplePixelShader> pixelShader;
	float roughness;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> textureSRVs;
	std::unordered_map < std::string, Microsoft::WRL::ComPtr<ID3D11SamplerState>> samplers;
};