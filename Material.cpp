#include "Material.h"

Material::Material(std::shared_ptr<SimpleVertexShader> vertexShader, std::shared_ptr<SimplePixelShader> pixelShader, DirectX::XMFLOAT4 colorTint, DirectX::XMFLOAT2 uvScale, DirectX::XMFLOAT2 uvOffset)
{
	this->vertexShader = vertexShader;
	this->pixelShader = pixelShader;
	this->colorTint = colorTint;
	this->uvScale = uvScale;
	this->uvOffset = uvOffset;
}

Material::~Material()
{
}

DirectX::XMFLOAT4 Material::GetColorTint()
{
	return this->colorTint;
}

DirectX::XMFLOAT2 Material::GetUVScale()
{
	return this->uvScale;
}

DirectX::XMFLOAT2 Material::GetUVOffset()
{
	return this->uvOffset;
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

void Material::SetUVScale(DirectX::XMFLOAT2 uvScale)
{
	this->uvScale = uvScale;
}

void Material::SetUVOffset(DirectX::XMFLOAT2 uvOffset)
{
	this->uvOffset = uvOffset;
}

void Material::SetVertexShader(std::shared_ptr<SimpleVertexShader> vertexShader)
{
	this->vertexShader = vertexShader;
}

void Material::SetPixelShader(std::shared_ptr<SimplePixelShader> pixelShader)
{
	this->pixelShader = pixelShader;
}
