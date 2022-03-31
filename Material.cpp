#include "Material.h"

Material::Material(DirectX::XMFLOAT4 colorTint, float roughness, std::shared_ptr<SimpleVertexShader> vertexShader, std::shared_ptr<SimplePixelShader> pixelShader)
{
	this->colorTint = colorTint;
	this->roughness = roughness;
	this->vertexShader = vertexShader;
	this->pixelShader = pixelShader;
}

Material::~Material()
{
}

DirectX::XMFLOAT4 Material::GetColorTint()
{
	return this->colorTint;
}

float Material::GetRoughness()
{
	return this->roughness;
}

std::shared_ptr<SimpleVertexShader> Material::GetVertexShader()
{
	return this->vertexShader;
}

std::shared_ptr<SimplePixelShader> Material::GetPixelShader()
{
	return this->pixelShader;
}

void Material::AddTextureSRV(std::string shaderName, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRV)
{
	this->textureSRVs.insert({ shaderName, textureSRV });
}

void Material::AddSampler(std::string shaderName, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler)
{
	this->samplers.insert({ shaderName, sampler });
}

void Material::PrepareMaterial()
{
	for (auto& t : this->textureSRVs) { this->pixelShader->SetShaderResourceView(t.first.c_str(), t.second); }
	for (auto& s : this->samplers) { this->pixelShader->SetSamplerState(s.first.c_str(), s.second); }
}

void Material::SetColorTint(DirectX::XMFLOAT4 colorTint)
{
	this->colorTint = colorTint;
}

void Material::SetRoughness(float roughness)
{
	this->roughness = roughness;
}

void Material::SetVertexShader(std::shared_ptr<SimpleVertexShader> vertexShader)
{
	this->vertexShader = vertexShader;
}

void Material::SetPixelShader(std::shared_ptr<SimplePixelShader> pixelShader)
{
	this->pixelShader = pixelShader;
}
