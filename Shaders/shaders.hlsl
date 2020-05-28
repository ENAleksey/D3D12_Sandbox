
cbuffer SceneConstantBuffer : register(b0)
{
	float4x4 mWorldViewProj;
	float4x4 mWorld;
	float4 materialColor;
	float3 cameraPos;
};

//cbuffer PSConstants : register(b1)
//{
//	float4 materialColor;
//	float4 materialParams;
//	float3 lightColor;
//	float3 lightDir;
//	float3 ambientColor;
//	float3 cameraPos;
//};

Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);


struct VSInput
{
	float3 position : POSITION;
	float2 texCoord : TEXCOORD;
	float3 normal : NORMAL;
	float4 color : COLOR;
};

struct PSInput
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
	float3 worldPos : TEXCOORD1;
	float3 normal : NORMAL;
	float4 color : COLOR;
};


PSInput VSMain(VSInput input)
{
	PSInput result;

	result.position = mul(mWorldViewProj, float4(input.position.xyz, 1.0f));
	result.texCoord = input.texCoord;
	result.worldPos = mul(mWorld, float4(input.position.xyz, 1.0f)).xyz;
	result.normal = mul((float3x3)mWorld, input.normal);
	result.color = input.color;

	return result;
}




static const float PI = 3.1415926535897932f;

float Pow2(float x)
{
	return (x * x);
}

float Pow5(float x)
{
	float x2 = x * x;
	return x2 * x2 * x;
}


float3 Diffuse_Burley(float3 diffuseColor, float roughness, float NoV, float NoL, float VoH)
{
	float FD90 = 0.5f + 2.0f * VoH * VoH * roughness;
	float FdV = 1.0f + (FD90 - 1.0f) * Pow5(1.0f - NoV);
	float FdL = 1.0f + (FD90 - 1.0f) * Pow5(1.0f - NoL);
	return NoL * diffuseColor * ((1.0f / PI) * FdV * FdL);
}

float3 Diffuse_Lambert(float3 diffuseColor)
{
	return diffuseColor * (1.0f / PI);
}


float D_GGX(float a2, float NoH)
{
	float d = Pow2(NoH) * (a2 - 1.0f) + 1.0f;
	return a2 * rcp(PI * d * d);
}

float Vis_SmithJointApprox(float a, float NoV, float NoL)
{
	float Vis_SmithV = NoL * (NoV * (1.0f - a) + a);
	float Vis_SmithL = NoV * (NoL * (1.0f - a) + a);
	return 0.5f * rcp(Vis_SmithV + Vis_SmithL);
}

float3 F_Schlick(float3 SpecularColor, float VoH)
{
	float Fc = Pow5(1.0f - VoH);
	return saturate(50.0f * SpecularColor.g) * Fc + (1.0f - Fc) * SpecularColor;
}

float3 StandardShading(float3 albedoColor, float3 specularColor, float roughness, float3 normal, float3 lightDir, float3 viewDir)
{
	const float a = roughness * roughness;
	const float a2 = a * a;

	float NoL = dot(normal, lightDir);
	float NoV = dot(normal, viewDir);
	float LoV = dot(lightDir, viewDir);
	float InvLenH = rsqrt(2.0f + 2.0f * LoV);
	float NoH = saturate((NoL + NoV) * InvLenH);
	float VoH = saturate(InvLenH + InvLenH * LoV);
	NoL = saturate(NoL);
	NoV = saturate(abs(NoV) + 1e-5f);

	float D = D_GGX(a2, NoH);
	float Vis = Vis_SmithJointApprox(a, NoV, NoL);
	float3 F = F_Schlick(specularColor, VoH);
	float3 specular = D * Vis * F;

	float3 diffuse = Diffuse_Burley(albedoColor, roughness, NoV, NoL, VoH);
	//float3 diffuse = Diffuse_Lambert(albedoColor);

	return (diffuse + specular) * NoL;
}


float3 DirectionalLight(float3 albedoColor, float3 specularColor, float roughness, float3 normal, float3 lightDir, float3 viewDir, float3 lightColor, float fShadowTerm)
{
	float3 light = 0.0f;
	//if (g_bPBS)
		light = StandardShading(albedoColor, specularColor, roughness, normal, lightDir, viewDir);
	//else
	//	light = albedoColor * saturate(dot(normal, lightDir));

	light *= lightColor * fShadowTerm;

	return light;
}

float4 PSMain(PSInput input) : SV_TARGET
{
	float g_fMaterialRoughness = 0.4f;
	float g_fMaterialMetallic = 0.0f;
	float3 albedoColor = materialColor.rgb; //input.color.rgb;
	float3 ambientColor = float3(0.1f, 0.1f, 0.1f);
	float3 lightColor = float3(1.0f, 1.0f, 1.0f) * 4.0f;
	float3 lightDir = normalize(float3(0.1f, 0.6f, -0.5f));
	float fShadowTerm = 1.0f;
	float alpha = input.color.a * materialColor.a;

	float3 viewDir = normalize(cameraPos - input.worldPos);
	float3 normal = normalize(input.normal);

	float3 adjustedWorldPos = floor(input.worldPos * 1.99f);
	float checkboard = saturate(frac((adjustedWorldPos.x + adjustedWorldPos.y + adjustedWorldPos.z) * 0.5f) * 2.0f);
	g_fMaterialRoughness = lerp(0.45f, 0.5f, checkboard);
	albedoColor = lerp(albedoColor, float3(0.1f, 0.1f, 0.1f), checkboard);
	albedoColor = sin((input.texCoord.x * 30 + sin(input.texCoord.y * 40)) * 3);
	albedoColor = sin((input.texCoord.x + distance(input.texCoord, float2(0.5f, 0.5f)) * 30) * 4);

	const float roughness = max(0.08f, g_fMaterialRoughness);
	const float3 specularColor = lerp(0.04f, albedoColor, g_fMaterialMetallic);
	albedoColor *= (1.0f - g_fMaterialMetallic);

	float3 light = 0;
	light += ambientColor * albedoColor;
	light += DirectionalLight(albedoColor, specularColor, roughness, normal, lightDir, viewDir, lightColor, fShadowTerm);

	float3 outColor = light;

	//return float4(input.texCoord, 0.0f, 1.0f);
	//return g_texture.Sample(g_sampler, input.texCoord);
	return float4(outColor, alpha);
}