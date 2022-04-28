#include "ShaderStructs.hlsli"
#include "Lighting.hlsli"

Texture2D AlbedoMap			: register(t0);
Texture2D EmissiveMap		: register(t1);
Texture2D RoughMap			: register(t2);
Texture2D MetalMap			: register(t3);
Texture2D NormalMap			: register(t4);
SamplerState BasicSampler	: register(s0);

cbuffer ExternalData : register(b0) {
	Light lights[MAX_LIGHTS];
	int lightCount;

	float3 ambient;

	float3 cameraPosition;

	float4 colorTint;
	float2 uvScale;
	float2 uvOffset;
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
	input.normal = normalize(input.normal);
	input.tangent = normalize(input.tangent);
	input.uv = input.uv * uvScale + uvOffset;

	// Normal Mapping
	float3 normalMap = NormalMapping(NormalMap, BasicSampler, input.uv, input.normal, input.tangent);
	input.normal = normalMap;

	// Roughness Mapping
	float roughness = RoughMap.Sample(BasicSampler, input.uv).r;

	// Metal Mapping
	float metal = MetalMap.Sample(BasicSampler, input.uv).r;

	// Sample Texture
	float4 surfaceColor = AlbedoMap.Sample(BasicSampler, input.uv);
	surfaceColor.rgb = pow(surfaceColor.rgb, 2.2);

	// Specular Color
	float3 specColor = lerp(F0_NON_METAL.rrr, surfaceColor.rgb, metal);

	// Ambient Color
	float3 totalLight = EmissiveMap.Sample(BasicSampler, input.uv) + ambient * surfaceColor.rgb;

	// Lights
	for (int i = 0; i < lightCount; i++) {
		Light light = lights[i];
		light.Direction = normalize(light.Direction);

		switch (light.Type) {
			case LIGHT_TYPE_DIRECTIONAL:
				totalLight += DirLightPBR(light, input.normal, input.worldPos, cameraPosition, roughness, metal, surfaceColor.rgb, specColor);
				break;
			case LIGHT_TYPE_POINT:
				totalLight += PointLightPBR(light, input.normal, input.worldPos, cameraPosition, roughness, metal, surfaceColor.rgb, specColor);
				break;
			case LIGHT_TYPE_SPOT:
				totalLight += SpotLightPBR(light, input.normal, input.worldPos, cameraPosition, roughness, metal, surfaceColor.rgb, specColor);
				break;
		}
	}

	float3 final = pow(totalLight, 1.0f / 2.2f);
	return float4(final, 1);
}