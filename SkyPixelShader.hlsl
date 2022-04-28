#include "ShaderStructs.hlsli"

TextureCube Skybox : register(t0);
SamplerState BasicSampler : register(s0);

float4 main(VertexToPixel_Sky input) : SV_TARGET
{
	return Skybox.Sample(BasicSampler, input.sampleDir);
}