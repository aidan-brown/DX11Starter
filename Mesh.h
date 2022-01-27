#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include "Vertex.h"
#include "BufferStructs.h"
#include <DirectXMath.h>

class Mesh {

public:
	Mesh(Vertex* verticies, int vertexCount, UINT* indicies, int indexCount, DirectX::XMFLOAT4 colorTint, DirectX::XMFLOAT3 offset, Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
	~Mesh();

	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();
	int GetIndexCount();
	void Draw();
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer, indexBuffer, constantBufferVS;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;

	VertexShaderExternalData vsData;

	int indexCount;

};