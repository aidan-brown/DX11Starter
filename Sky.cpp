#include "Sky.h"

Sky::Sky(std::shared_ptr<Mesh> mesh, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> skySRV, std::shared_ptr<SimpleVertexShader> vertexShader, std::shared_ptr<SimplePixelShader> pixelShader, Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState, Microsoft::WRL::ComPtr<ID3D11Device> device) {
	this->mesh = mesh;
	this->skySRV = skySRV;
	this->samplerState = samplerState;
	this->vertexShader = vertexShader;
	this->pixelShader = pixelShader;

	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_FRONT;
	device->CreateRasterizerState(&rasterizerDesc, this->rasterizerState.GetAddressOf());

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	device->CreateDepthStencilState(&depthStencilDesc, this->depthStencilState.GetAddressOf());
}

Sky::~Sky()
{
}

void Sky::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, std::shared_ptr<Camera> camera)
{
	context->RSSetState(this->rasterizerState.Get());
	context->OMSetDepthStencilState(this->depthStencilState.Get(), 0);

	this->vertexShader->SetMatrix4x4("viewMatrix", camera->GetViewMatrix());
	this->vertexShader->SetMatrix4x4("projectionMatrix", camera->GetProjectionMatrix());
	this->vertexShader->CopyAllBufferData();

	this->pixelShader->SetShaderResourceView("Skybox", skySRV);
	this->pixelShader->SetSamplerState("BasicSampler", samplerState);
	this->pixelShader->CopyAllBufferData();

	this->vertexShader->SetShader();
	this->pixelShader->SetShader();

	this->mesh->Draw(camera->GetTransform(), camera);

	context->RSSetState(nullptr);
	context->OMSetDepthStencilState(nullptr, 0);
}
