#include "Shader_Defines.hlsli"
#include "Shader_Variable_Defines.hlsli"
#include "Shader_Material_Defines.hlsli"
#include "Shader_Client_Functions.hlsli"

float g_fLightSmooth = 0.1f;
float4 g_vShadowColor = float4(0.9f, 0.9f, 0.9f, 0.f);

float g_fAlpha_Discard;
float g_fAlpha;

// PBR Test
float g_fLightAttenuation = 2.f;
float3 g_vLightColor = float3(0.9f, 0.9f, 0.9f);
float g_fShadowAmount = 0.1f;
bool g_bUsePBR;

float g_fUV;
float4 g_vBaseColor;
float4 g_vBaseColorMultiply;
float4 g_vEmissiveColor;
float2 g_vDiffuseAddUV;
float2 g_vMaskAddUV;
float g_fBrightness = 1.f;

#define NUM_LIGHT 1

struct VS_IN
{
	float3 vPosition : POSITION0;
	float3 vNormal : NORMAL0;
	float2 vTexCoord : TEXCOORD0;
	float3 vTangent : TANGENT0;
	float4 vVertexColor : TEXCOORD1;

	row_major matrix matWorld : WORLD;
	row_major matrix matPivot : PIVOT;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
	float4 vNormal : NORMAL;
	float2 vTexCoord : TEXCOORD0;
	float4 vWorldPos : TEXCOORD1;
	float4 vProjPos : TEXCOORD2;
	float3 vTangent : TANGENT;
	float3 vBiNormal : BINORMAL;

	float3 vToCamera : TEXCOORD3;
	float3 vToLight : TEXCOORD4;
};

struct VS_SHADOW_OUT
{
	float4 vPosition : SV_POSITION;
	float2 vTexCoord : TEXCOORD0;
	float4 vProjPos : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	matrix matWorld;

	if (g_bIsInstanced)
		matWorld = mul(In.matPivot, In.matWorld);
	else
		matWorld = g_WorldMatrix;

	matrix matWV = mul(matWorld, g_ViewMatrix);
	matrix matWVP = mul(matWV, g_ProjMatrix);

	float4 vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	float4 vNormal = mul(float4(In.vNormal, 0.f), matWorld);

	Out.vPosition = vPosition;
	Out.vNormal = normalize(vNormal);
	Out.vTexCoord = In.vTexCoord * g_fUV;
	Out.vWorldPos = mul(float4(In.vPosition, 1.f), matWorld);
	Out.vProjPos = vPosition;

	Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), matWorld)).xyz;
	Out.vBiNormal = normalize(cross(Out.vNormal.xyz, Out.vTangent));

	Out.vToCamera = normalize(g_vCamPosition.xyz - vPosition.xyz);
	Out.vToLight = normalize(g_vLightPosition.xyz - vPosition.xyz);

	return Out;
}

VS_SHADOW_OUT VS_SHADOW(VS_IN In)
{
	VS_SHADOW_OUT Out = (VS_SHADOW_OUT)0;

	matrix matWorld;

	if (g_bIsInstanced)
		matWorld = mul(In.matPivot, In.matWorld);
	else
		matWorld = g_WorldMatrix;

	matrix matWV = mul(matWorld, g_ViewMatrix);
	matrix matWVP = mul(matWV, g_ProjMatrix);

	vector vPosition = mul(vector(In.vPosition, 1.f), matWVP);

	Out.vPosition = vPosition;
	Out.vTexCoord = In.vTexCoord;
	Out.vProjPos = Out.vPosition;

	return Out;
}


struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float4 vNormal : NORMAL;
	float2 vTexCoord : TEXCOORD0;
	float4 vWorldPos : TEXCOORD1;
	float4 vProjPos : TEXCOORD2;
	float3 vTangent : TANGENT;
	float3 vBiNormal : BINORMAL;

	float3 vToCamera : TEXCOORD3;
	float3 vToLight : TEXCOORD4;
};

struct PS_OUT
{
	float4 vDiffuse : SV_TARGET0;
	float4 vNormal : SV_TARGET1;
	float4 vDepth : SV_TARGET2;
	float4 vMapDepth : SV_TARGET3;
	float4 vEmissive : SV_TARGET4;
};

struct PS_SHADOW_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexCoord : TEXCOORD0;
	float4 vProjPos : TEXCOORD1;
};

struct PS_SHADOW_OUT
{
	float fDepth : SV_TARGET0;
};

float4 GetNormal(in float2 vTexCoord, in float3 vTangent, in float3 vBiNormal, in float3 vNormal)
{
	float4 vOutNormal = float4(vNormal, 0.f);

	if (g_bUseNormalTexture)
	{
		vOutNormal = g_NormalTexture.SampleLevel(LinearSampler, vTexCoord, 0);

		// b채널이 없는 경우를 대비
		if (0.f >= vOutNormal.b)
			vOutNormal.b = sqrt(1.f - saturate(dot(vOutNormal.rg, vOutNormal.rg)));

		vOutNormal = vOutNormal * 2.f - 1.f;

		float3x3 matWorld = float3x3(vTangent, vBiNormal, vNormal.xyz);
		vOutNormal = float4(normalize(mul(vOutNormal.xyz, matWorld)), 0.f);
	}

	return vOutNormal;
}

float4 GetNoise(in float2 vTexCoord)
{
	float4 vNoise = float4(0.f, 0.f, 0.f, 0.f);

	if (g_bUseNoiseTexture)
		vNoise = g_NoiseTexture.Sample(LinearSampler, vTexCoord);

	return vNoise;
}

float4 GetMask(in float2 vTexCoord)
{
	float4 vMask = float4(0.05f, 1.f, 0.f, 0.f);

	if (g_bUseMaskMapTexture)
		vMask = g_MaskMapTexture.SampleLevel(LinearSampler, vTexCoord, 0);

	return vMask;
}

float4 GetEmissive(in float2 vTexCoord)
{
	float4 vEmissive = float4(0.f, 0.f, 0.f, 0.f);

	if (g_bUseEmissiveTexture)
		vEmissive = float4(g_EmissiveTexture.Sample(LinearSampler, vTexCoord).rgb, 0.f) * g_vEmissiveColor;
	else
		vEmissive = g_vEmissiveColor;

	return vEmissive;
}

float4 GetLightMap(in float2 vTexCoord)
{
	float4 vLightMap = float4(1.f, 1.f, 1.f, 1.f);

	if (g_bUseLightMapTexture)
		vLightMap = g_LightMapTexture.Sample(LinearSampler, vTexCoord);

	return vLightMap;
}

PS_OUT PS_MAIN(PS_IN In)
{
	/*
	베젤 : diffuse, normal, reflection (_d, _n, _m)
	전광판 : diffuse, ambient, displacement (_d, 나머지 두개는 도트)
	*/

	PS_OUT Out = (PS_OUT)0;

	float4 vDiffuse = float4(0.f, 0.f, 0.f, 0.f);

	if (g_bUseDiffuseTexture)
	{
		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexCoord / g_fUV + g_vDiffuseAddUV);
		vDiffuse.rgb *= g_vBaseColor.rgb;
	}

	if (dot(vDiffuse.rgb, vDiffuse.rgb) <= 0.f)
		discard;

	float3 vMasking = float3(1.f, 1.f, 1.f);

	if (g_bUseSecondDiffuseTexture)
		vMasking = g_SecondDiffuseTexture.Sample(LinearSampler, In.vTexCoord + g_vMaskAddUV).rgb;

	vDiffuse.rgb *= vMasking;

	float4 vMask = GetMask(In.vTexCoord);
	float4 vNormal = GetNormal(In.vTexCoord, In.vTangent, In.vBiNormal, In.vNormal.xyz);
	float4 vLightMap = GetLightMap(In.vTexCoord);

	float4 vEmissive = GetEmissive(In.vTexCoord);
	float4 vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCameraFar, 0.5f, 1.f); // z 0.5 : 명암까지만 적용


	Out.vDiffuse = float4(vDiffuse.rgb, 1.f);
	Out.vNormal = vNormal * 0.5f + 0.5f;
	Out.vDepth = vDepth;
	Out.vMapDepth = vDepth.g;
	Out.vEmissive = float4(vEmissive.rgb * g_fBrightness, 0.f);

	if (g_bUseSecondNormalTexture) // 전광판일 때만
		vEmissive.rgb *= 0.5f < vMasking.r ? 1.f : 0.f;

	return Out;
}

PS_SHADOW_OUT PS_SHADOW(PS_SHADOW_IN In)
{
	PS_SHADOW_OUT Out = (PS_SHADOW_OUT)0;

	if (g_bUseDiffuseTexture)
	{
		float fAlpha = g_DiffuseTexture.Sample(LinearSampler, In.vTexCoord).a;

		if (fAlpha < g_fAlpha_Discard)
			discard;
	}

	if (0 == g_iProjType) // Perspective
		Out.fDepth = In.vProjPos.w / g_fCameraFar;

	else
		Out.fDepth = In.vProjPos.z / In.vProjPos.w;

	return Out;
}

technique11 InstanceObject
{
	pass Default
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Shadow
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);
		VertexShader = compile vs_5_0 VS_SHADOW();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_SHADOW();
	}
}