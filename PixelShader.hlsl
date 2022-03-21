#include "ShaderIncludes.hlsli"

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

	float3 V = normalize(cameraPosition - input.worldPosition);
	float specExponent = (1.0f - roughness) * MAX_SPECULAR_EXPONENT;
	float3 finalColor = (ambient * colorTint);
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
		float spec = pow(saturate(dot(R, V)), specExponent);
		float3 dirToLight = -dirFromLight;
		float3 diffuse = Diffuse(input.normal, dirToLight);
		finalColor += ((diffuse + spec) * light.Color * colorTint * attenuate);
	}

	return float4(finalColor, 1);
}