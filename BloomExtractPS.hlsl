cbuffer externalData : register(b0) {
	float bloomThreshold;
}

struct VertexToPixel {
	float4 position			: SV_POSITION;
	float uv				: TEXCOORD0;
};

Texture2D pixels			: register(t0);
SamplerState samplerOptions	: register(s0);

float4 main(VertexToPixel input) : SV_TARGET
{
	float4 pixelColor = pixels.Sample(samplerOptions, input.uv);

	return max(pixelColor - bloomThreshold, 0);
}