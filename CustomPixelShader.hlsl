#include "ShaderIncludes.hlsli"

cbuffer ExternalData : register(b0) {
	float4 colorTint;
}

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
	return float4(cos(input.screenPosition.x) * colorTint.x, colorTint.y, sin(input.screenPosition.y) * colorTint.z, 1);
}