#include "ShaderStructs.hlsli"

cbuffer ExternalData : register(b0) {
	matrix viewMatrix;
	matrix projectionMatrix;
}

VertexToPixel_Sky main(VertexShaderInput input)
{
	VertexToPixel_Sky output;

	matrix viewNoTranslation = viewMatrix;
	viewNoTranslation._14 = 0;
	viewNoTranslation._24 = 0;
	viewNoTranslation._34 = 0;

	output.position = mul(mul(projectionMatrix, viewNoTranslation), float4(input.localPosition, 1.0f));
	output.position.z = output.position.w;

	output.sampleDir = input.localPosition;

	return output;
}