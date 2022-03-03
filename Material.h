#pragma once
#include <memory>
#include "DXCore.h"
#include "SimpleShader.h"

class Material {
public:
	Material(DirectX::XMFLOAT4 colorTint, std::shared_ptr<SimpleVertexShader> vertexShader, std::shared_ptr<SimplePixelShader> pixelShader);
	~Material();

	void SetColorTint(DirectX::XMFLOAT4 colorTint);
	void SetVertexShader(std::shared_ptr<SimpleVertexShader> vertexShader);
	void SetPixelShader(std::shared_ptr<SimplePixelShader> pixelShader);

	DirectX::XMFLOAT4 GetColorTint();
	std::shared_ptr<SimpleVertexShader> GetVertexShader();
	std::shared_ptr<SimplePixelShader> GetPixelShader();

private:
	DirectX::XMFLOAT4 colorTint;
	std::shared_ptr<SimpleVertexShader> vertexShader;
	std::shared_ptr<SimplePixelShader> pixelShader;
};