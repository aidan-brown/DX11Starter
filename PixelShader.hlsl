#include "ShaderIncludes.hlsli"

Texture2D AlbedoMap			: register(t0);
Texture2D RoughMap			: register(t1);
Texture2D MetalMap			: register(t2);
Texture2D NormalMap			: register(t3);
SamplerState BasicSampler	: register(s0);

float3 Diffuse(float3 normal, float3 dirToLight) {
	return saturate(dot(normal, dirToLight));
}

float Attenuate(Light light, float3 worldPos)
{
	float dist = distance(light.Position, worldPos);
	float att = saturate(1.0f - (dist * dist / (light.Range * light.Range)));
	return att * att;
}

cbuffer ExternalData : register(b0) {
	float4 colorTint;
	float3 cameraPosition;
	float3 ambient;
	Light lights[5];
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

	// Diffuse
	float3 albedoColor = pow(AlbedoMap.Sample(BasicSampler, input.uv).rgb, 2.2);
	albedoColor *= colorTint;

	// Normal
	float3 unpackedNormal = NormalMap.Sample(BasicSampler, input.uv).rgb * 2 - 1;
	float3 N = normalize(input.normal);
	float3 T = normalize(input.tangent);
	T = normalize(T - N * dot(T, N));
	float3 B = cross(T, N);
	float3x3 TBN = float3x3(T, B, N);
	input.normal = mul(unpackedNormal, TBN);

	// Ambient
	float3 finalColor = (ambient * albedoColor);

	// Specular/Rough
	float roughness = RoughMap.Sample(BasicSampler, input.uv).r;
	float metalness = MetalMap.Sample(BasicSampler, input.uv).r;
	float3 specularColor = lerp(F0_NON_METAL.rrr, albedoColor.rgb, metalness);

	// Lights
	float3 V = normalize(cameraPosition - input.worldPosition);
	for (int i = 0; i < 5; i++) {
		Light light = lights[i];
		float3 dirToLight = (0, 1, 0);
		float attenuate = 1;
		if (light.Type == LIGHT_TYPE_DIRECTIONAL) {
			dirToLight = normalize(-light.Direction);
		}
		else {
			dirToLight = normalize(light.Position - input.worldPosition);
			attenuate = Attenuate(light, input.worldPosition);
		}
		float diffuse = DiffusePBR(input.normal, dirToLight);
		float spec = MicrofacetBRDF(input.normal, dirToLight, V, roughness, specularColor);

		float3 balancedDiff = DiffuseEnergyConserve(diffuse, spec, metalness);
		float3 total = (balancedDiff * albedoColor + spec) * light.Intensity * light.Color * attenuate;

		finalColor += total;
	}

	return float4(pow(finalColor, 1.0 / 2.2), 1);
}