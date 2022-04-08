#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <memory>
#include <fstream>
#include <vector>
#include "Vertex.h"
#include "BufferStructs.h"
#include <DirectXMath.h>
#include "Transform.h"
#include "Camera.h"

class Mesh {

public:
	Mesh(Vertex* verticies, int vertexCount, unsigned int* indicies, int indexCount, Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
	Mesh(const char* filename, Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
	~Mesh();

	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();
	int GetIndexCount();
	void Draw(Transform transform, std::shared_ptr<Camera>);
private:
	void CreateBuffers(Vertex* vertices, int vertexCount, unsigned int* indices, int indexCount, Microsoft::WRL::ComPtr<ID3D11Device> device);
	void CalculateTangents(Vertex* verts, int numVerts, unsigned int* indices, int numIndices);

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer, indexBuffer, constantBufferVS;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;

	int indexCount;

};