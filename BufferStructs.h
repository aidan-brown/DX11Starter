#pragma once
#include <DirectXMath.h>
#include "Transform.h"

struct VertexShaderExternalData {
	DirectX::XMFLOAT4 colorTint;
	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;
};