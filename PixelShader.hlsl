#include "ShaderIncludes.hlsli"

Texture2D DiffuseMap : register(t0);
Texture2D SpecularMap : register(t1);
Texture2D RoughMap : register(t2);
Texture2D NormalMap : register(t3);
SamplerState BasicSampler : register(s0);

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
	float roughness;
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
	float3 surfaceColor = DiffuseMap.Sample(BasicSampler, input.uv).rgb;
	surfaceColor *= colorTint;

	// Normal
	float3 unpackedNormal = NormalMap.Sample(BasicSampler, input.uv).rgb * 2 - 1;
	float3 N = normalize(input.normal);
	float3 T = normalize(input.tangent);
	T = normalize(T - N * dot(T, N));
	float3 B = cross(T, N);
	float3x3 TBN = float3x3(T, B, N);
	input.normal = mul(unpackedNormal, TBN);

	// Ambient
	float3 finalColor = (ambient * surfaceColor);

	// Specular/Rough
	float specExponent = (1.0f - RoughMap.Sample(BasicSampler, input.uv).r) * MAX_SPECULAR_EXPONENT;
	float specSample = SpecularMap.Sample(BasicSampler, input.uv).r;

	// Lights
	float3 V = normalize(cameraPosition - input.worldPosition);
	for (int i = 0; i < 5; i++) {
		Light light = lights[i];
		float3 dirFromLight = (0, 1, 0);
		float attenuate = 1;
		if (light.Type == LIGHT_TYPE_DIRECTIONAL) {
			dirFromLight = normalize(light.Direction);
		}
		else {
			dirFromLight = normalize(input.worldPosition - light.Position);
			attenuate = Attenuate(light, input.worldPosition);
		}
		float3 R = reflect(dirFromLight, input.normal);
		float spec = specSample * pow(saturate(dot(R, V)), specExponent);
		float3 dirToLight = -dirFromLight;
		float3 diffuse = Diffuse(input.normal, dirToLight);
		spec *= any(diffuse);
		finalColor += ((diffuse + spec) * light.Color * surfaceColor * attenuate);
	}

	return float4(finalColor, 1);
}