cbuffer externalData : register(b0) {
	float2 pixelUVSize;
	float2 blurDirection;
}

struct VertexToPixel {
	float4 position		: SV_POSITION;
	float2 uv			: TEXCOORD0;
};

Texture2D pixels			: register(t0);
SamplerState samplerOptions	: register(s0);

float4 main(VertexToPixel input) : SV_TARGET
{
	#define NUM_SAMPLES 15

	const float weights[NUM_SAMPLES] = { 0.023089, 0.034587, 0.048689, 0.064408, 0.080066, 0.093531, 0.102673, 0.105915, 0.102673, 0.093531, 0.080066, 0.064408, 0.048689, 0.034587, 0.023089 };
	const float offsets[NUM_SAMPLES] = { -13.5f, -11.5f, -9.5f, -7.5f, -5.5f, -3.5f, -1.5f, 0, 1.5f, 3.5f, 5.5f, 7.5f, 9.5f, 11.5f, 13.5f };

	float3 colorTotal = float3(0, 0, 0);

	float2 uvOffset = blurDirection * pixelUVSize;

	for (int i = 0; i < NUM_SAMPLES; i++) {
		float2 uv = input.uv + (uvOffset * offsets[i]);
		colorTotal += pixels.Sample(samplerOptions, uv).rgb * weights[i];
	}

	return float4(colorTotal, 1);
}