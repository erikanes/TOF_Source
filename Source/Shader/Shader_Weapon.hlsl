#include "Shader_Defines.hlsli"
#include "Shader_Variable_Defines.hlsli"
#include "Shader_Material_Defines.hlsli"
#include "Shader_Client_Functions.hlsli"

// RimLight
bool g_bUseRimLight = false;
float g_fRimPower = 5.f;
float4 g_vRimColor = float4(0.3f, 0.3f, 0.3f, 0.f);

// Dissolve
texture2D g_DissolveTexture;
float g_fDissolveAlpha;
float4 g_vDissolveColor;

struct VS_IN
{
	float3 vPosition : POSITION;
	float3 vNormal : NORMAL;
	float2 vTexCoord : TEXCOORD0;
	float3 vTangent : TANGENT;
	float4 vVertexColor : TEXCOORD1;

	row_major matrix matWorld : WORLD;
	row_major matrix matPivot : PIVOT;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION0;
	float4 vNormal : NORMAL0;
	float2 vTexCoord : TEXCOORD0;
	float4 vWorldPos : TEXCOORD1;
	float4 vProjPos : TEXCOORD2;
	float3 vTangent : TANGENT;
	float3 vBiNormal : BINORMAL;
};

struct VS_SHADOW_OUT
{
	float4 vPosition : SV_POSITION;
	float4 vProjPos : TEXCOORD0;
};

struct VS_OUTLINE_OUT
{
	float4 vPosition : SV_POSITION;
	float2 vTexCoord : TEXCOORD0;
	float4 vProjPos : TEXCOORD1;
};


VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	matrix matWVP = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWVP, g_ProjMatrix);

	vector vPosition = vector(In.vPosition, 1.f);

	Out.vPosition = mul(vPosition, matWVP);
	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
	Out.vTexCoord = In.vTexCoord;
	Out.vWorldPos = mul(vPosition, g_WorldMatrix);
	Out.vProjPos = Out.vPosition;

	Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), g_WorldMatrix)).xyz;
	Out.vBiNormal = normalize(cross(Out.vNormal.xyz, Out.vTangent));

	return Out;
}

VS_SHADOW_OUT VS_SHADOW(VS_IN In)
{
	VS_SHADOW_OUT Out = (VS_SHADOW_OUT)0;

	matrix matWVP = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWVP, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vProjPos = Out.vPosition;

	return Out;
}

VS_OUTLINE_OUT VS_OUTLINE(VS_IN In)
{
	VS_OUTLINE_OUT Out = (VS_OUTLINE_OUT)0;

	vector vPosition = vector(In.vPosition + In.vNormal * g_fOutlineWidth, 1.f);

	matrix matWVP = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWVP, g_ProjMatrix);

	Out.vPosition = mul(vPosition, matWVP);
	Out.vTexCoord = In.vTexCoord;
	Out.vProjPos = Out.vPosition;

	return Out;
}

struct PS_IN
{
	float4 vPosition : SV_POSITION0;
	float4 vNormal : NORMAL0;
	float2 vTexCoord : TEXCOORD0;
	float4 vWorldPos : TEXCOORD1;
	float4 vProjPos : TEXCOORD2;
	float3 vTangent : TANGENT;
	float3 vBiNormal : BINORMAL;
};

struct PS_OUT
{
	float4 vDiffuse : SV_TARGET0;
	float4 vNormal : SV_TARGET1;
	float4 vDepth : SV_TARGET2;
	float4 vWeaponDepth : SV_TARGET3;
	float4 vEmissive : SV_TARGET4;
};

struct PS_SHADOW_IN
{
	vector vPosition : SV_POSITION;
	vector vProjPos : TEXCOORD0;
};

struct PS_SHADOW_OUT
{
	float fDepth : SV_TARGET0;
};

struct PS_OUTLINE_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexCoord : TEXCOORD0;
	float4 vProjPos : TEXCOORD1;
};

struct PS_OUTLINE_OUT
{
	float4 vColor : SV_TARGET0;
	float4 vDepth : SV_TARGET2;
	float4 vWeaponDepth : SV_TARGET3;
};

float4 GetNormal(in float2 vTexCoord, in float3 vTangent, in float3 vBiNormal, in float3 vNormal)
{
	float4 vOutNormal;

	if (g_bUseNormalTexture)
	{
		vOutNormal = g_NormalTexture.Sample(LinearSampler, vTexCoord);

		if (0.f <= vOutNormal.b)
			vOutNormal.b = sqrt(1 - saturate(dot(vOutNormal.rg, vOutNormal.rg)));

		vOutNormal = vOutNormal * 2.f - 1.f;

		float3x3 matWorld = float3x3(vTangent, vBiNormal, vNormal);
		vOutNormal = float4(normalize(mul(vOutNormal.xyz, matWorld)), 0.f);
	}
	else
		vOutNormal = float4(vNormal, 0.f);

	return vOutNormal;
}

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float4 vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexCoord);
	float4 vNormal = GetNormal(In.vTexCoord, In.vTangent, In.vBiNormal, In.vNormal.xyz);

	Out.vDiffuse = vDiffuse;
	Out.vDiffuse.a = 1.f;
	Out.vNormal = vNormal * 0.5f + 0.5f;
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCameraFar, 1.f, 1.f);
	Out.vWeaponDepth.r = Out.vDepth.g;
	Out.vEmissive = float4(0.f, 0.f, 0.f, 0.f);

	if (1.f > g_fDissolveAlpha)
	{
		float fDissolve = g_DissolveTexture.Sample(LinearSampler, In.vTexCoord).r;

		if (g_fDissolveAlpha < fDissolve)
			discard;

		float fStep = smoothstep(g_fDissolveAlpha, g_fDissolveAlpha + 0.1f, fDissolve);

		if (1.f > fStep)
			Out.vEmissive.rgb = g_vDissolveColor; // 무기 속성에 따라 색상 결정
	}

	if (g_bUseEmissiveTexture)
		Out.vEmissive += g_EmissiveTexture.Sample(LinearSampler, In.vTexCoord);

	if (g_bUseRimLight)
		Out.vEmissive += CalculateRimLight(In.vWorldPos.xyz, vNormal.xyz, g_vCamPosition.xyz, g_fRimPower, g_vRimColor);

	return Out;
}

PS_SHADOW_OUT PS_SHADOW(PS_SHADOW_IN In)
{
	PS_SHADOW_OUT Out = (PS_SHADOW_OUT)0;

	if (0 == g_iProjType) // Perspective
		Out.fDepth = In.vProjPos.w / g_fCameraFar;
	else
		Out.fDepth = In.vProjPos.z / In.vProjPos.w;

	return Out;
}

PS_OUTLINE_OUT PS_OUTLINE(PS_OUTLINE_IN In)
{
	PS_OUTLINE_OUT Out = (PS_OUTLINE_OUT)0;

	if (1.f > g_fDissolveAlpha)
	{
		float fDissolve = g_DissolveTexture.Sample(LinearSampler, In.vTexCoord).r;

		if (g_fDissolveAlpha < fDissolve)
			discard;
	}

	float4 vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexCoord);

	Out.vColor.rgb = vDiffuse.rgb * 0.7f;
	Out.vColor.a = 1.f;

	Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCameraFar, 1.f, 1.f);
	Out.vWeaponDepth.r = Out.vDepth.g;

	return Out;
}

technique11 Default
{
	pass Main
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

	pass Outline
	{
		SetRasterizerState(RS_FrontCull);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_OUTLINE();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_OUTLINE();
	}
}