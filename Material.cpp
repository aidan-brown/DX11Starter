#include "Material.h"

Material::Material(DirectX::XMFLOAT4 colorTint, std::shared_ptr<SimpleVertexShader> vertexShader, std::shared_ptr<SimplePixelShader> pixelShader)
{
	Material::colorTint = colorTint;
	Material::vertexShader = vertexShader;
	Material::pixelShader = pixelShader;
}

Material::~Material()
{
}

DirectX::XMFLOAT4 Material::GetColorTint()
{
	return Material::colorTint;
}

std::shared_ptr<SimpleVertexShader> Material::GetVertexShader()
{
	return Material::vertexShader;
}

std::shared_ptr<SimplePixelShader> Material::GetPixelShader()
{
	return Material::pixelShader;
}

void Material::SetColorTint(DirectX::XMFLOAT4 colorTint)
{
	Material::colorTint = colorTint;
}

void Material::SetVertexShader(std::shared_ptr<SimpleVertexShader> vertexShader)
{
	Material::vertexShader = vertexShader;
}

void Material::SetPixelShader(std::shared_ptr<SimplePixelShader> pixelShader)
{
	Material::pixelShader = pixelShader;
}
