TextureCube Skybox : register(t0);
SamplerState BasicSampler : register(s0);

struct VertexToPixel
{
	float4 position			: SV_POSITION;
	float3 sampleDir		: DIRECTION;
};

float4 main(VertexToPixel input) : SV_TARGET
{
	return Skybox.Sample(BasicSampler, input.sampleDir);
}