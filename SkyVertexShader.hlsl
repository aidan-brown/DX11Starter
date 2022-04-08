struct VertexShaderInput
{
	float3 localPosition	: POSITION;
	float3 normal			: NORMAL;
	float3 tangent			: TANGENT;
	float3 uv				: TEXCOORD;
};

struct VertexToPixel
{
	float4 position			: SV_POSITION;
	float3 sampleDir		: DIRECTION;
};

cbuffer ExternalData : register(b0) {
	matrix viewMatrix;
	matrix projectionMatrix;
}

VertexToPixel main(VertexShaderInput input)
{
	VertexToPixel output;

	matrix viewNoTranslation = viewMatrix;
	viewNoTranslation._14 = 0;
	viewNoTranslation._24 = 0;
	viewNoTranslation._34 = 0;

	output.position = mul(mul(projectionMatrix, viewNoTranslation), float4(input.localPosition, 1.0f));
	output.position.z = output.position.w;

	output.sampleDir = input.localPosition;

	return output;
}