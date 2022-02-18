#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include "Vertex.h"
#include "BufferStructs.h"
#include <DirectXMath.h>
#include "Transform.h"
#include "Camera.h"

class Mesh {

public:
	Mesh(Vertex* verticies, int vertexCount, UINT* indicies, int indexCount, DirectX::XMFLOAT4 colorTint, Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
	~Mesh();

	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();
	int GetIndexCount();
	void Draw(Transform transform, Camera* camera);
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer, indexBuffer, constantBufferVS;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;

	VertexShaderExternalData vsData;

	int indexCount;

};