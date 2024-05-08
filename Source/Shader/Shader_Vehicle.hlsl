#include "Shader_Defines.hlsli"
#include "Shader_Variable_Defines.hlsli"
#include "Shader_Material_Defines.hlsli"
#include "Shader_Client_Functions.hlsli"

matrix g_BoneMatricies[256];

// Custom Lighting
float g_fLightSmooth = 0.1f;
float4 g_vBaseColor = float4(1.f, 1.f, 1.f, 0.f);
float4 g_vShadowColor = float4(0.9f, 0.9f, 0.9f, 0.f);
float g_fShadowAttenuation = 0.1f;

float g_fDissolveAlpha;
float4 g_vDissolveColor;
texture2D g_DissolveTexture;

struct VS_IN
{
	float3 vPosition : POSITION0;
	float3 vNormal : NORMAL0;
	float2 vTexCoord : TEXCOORD0;
	float3 vTangent : TANGENT0;
	float4 vVertexColor : TEXCOORD1;
	uint4 vBlendIndices : BLENDINDICES0;
	float4 vBlendWeights : BLENDWEIGHT0;

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

	float3 vCamDirection : TEXCOORD3;
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

	matrix matBone =
		g_BoneMatricies[In.vBlendIndices.x] * In.vBlendWeights.x +
		g_BoneMatricies[In.vBlendIndices.y] * In.vBlendWeights.y +
		g_BoneMatricies[In.vBlendIndices.z] * In.vBlendWeights.z +
		g_BoneMatricies[In.vBlendIndices.w] * In.vBlendWeights.w;

	vector vPosition = mul(vector(In.vPosition, 1.f), matBone);
	vector vNormal = mul(vector(In.vNormal, 0.f), matBone);

	matrix matWVP = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWVP, g_ProjMatrix);

	Out.vPosition = mul(vPosition, matWVP);
	Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
	Out.vTexCoord = In.vTexCoord;
	Out.vWorldPos = mul(vPosition, g_WorldMatrix);
	Out.vProjPos = Out.vPosition;

	Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix)).xyz;
	Out.vBiNormal = normalize(cross(Out.vNormal.xyz, Out.vTangent));
	
	Out.vCamDirection = normalize(Out.vWorldPos.xyz - g_vCamPosition.xyz);

	return Out;
}

VS_SHADOW_OUT VS_SHADOW(VS_IN In)
{
	VS_SHADOW_OUT Out = (VS_SHADOW_OUT)0;

	matrix matBone =
		g_BoneMatricies[In.vBlendIndices.x] * In.vBlendWeights.x +
		g_BoneMatricies[In.vBlendIndices.y] * In.vBlendWeights.y +
		g_BoneMatricies[In.vBlendIndices.z] * In.vBlendWeights.z +
		g_BoneMatricies[In.vBlendIndices.w] * In.vBlendWeights.w;

	vector vPosition = mul(vector(In.vPosition, 1.f), matBone);

	matrix matWVP = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWVP, g_ProjMatrix);

	Out.vPosition = mul(vPosition, matWVP);
	Out.vProjPos = Out.vPosition;

	return Out;
}

VS_OUTLINE_OUT VS_OUTLINE(VS_IN In)
{
	VS_OUTLINE_OUT Out = (VS_OUTLINE_OUT)0;

	matrix matBone =
		g_BoneMatricies[In.vBlendIndices.x] * In.vBlendWeights.x +
		g_BoneMatricies[In.vBlendIndices.y] * In.vBlendWeights.y +
		g_BoneMatricies[In.vBlendIndices.z] * In.vBlendWeights.z +
		g_BoneMatricies[In.vBlendIndices.w] * In.vBlendWeights.w;

	vector vPosition = mul(vector(In.vPosition + In.vNormal * g_fOutlineWidth, 1.f), matBone);

	matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
	matrix matWVP = mul(g_WorldMatrix, matVP);

	Out.vPosition = mul(vPosition, matWVP);
	Out.vTexCoord = In.vTexCoord;
	Out.vProjPos = Out.vPosition;

	return Out;
}


struct PS_IN
{
	vector vPosition : SV_POSITION;
	vector vNormal : NORMAL;
	float2 vTexCoord : TEXCOORD0;
	vector vWorldPos : TEXCOORD1;
	vector vProjPos : TEXCOORD2;
	float3 vTangent : TANGENT;
	float3 vBiNormal : BINORMAL;

	float3 vCamDirection : TEXCOORD3;
};

struct PS_OUT
{
	vector vDiffuse : SV_TARGET0;
	vector vNormal : SV_TARGET1;
	vector vDepth : SV_TARGET2;
	vector vVehicleDepth : SV_TARGET3;
	vector vEmissive : SV_TARGET4;
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
	float4 vVehicleDepth : SV_TARGET3;
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

// 탈것은 노말이 없음. _d, _id, _m 3개가 있는데 현재 _d, _m 두개만 바인딩 한 상태
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float4 vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexCoord);
	float4 vNormal = GetNormal(In.vTexCoord, In.vTangent, In.vBiNormal, In.vNormal.xyz);
	float4 vMask = g_MaskMapTexture.Sample(LinearSampler, In.vTexCoord);

	float fSpecular = GetAnisoSpecular(vNormal, g_vCamDirection, g_vLightDirection.xyz, vMask.r, vMask.a, 0.3f, 5.f);

	Out.vDiffuse.rgb = GetCustomShadow(vNormal, g_vLightDirection, g_fLightSmooth, g_vBaseColor, g_vShadowColor) * vDiffuse.rgb + fSpecular;
	Out.vDiffuse.a = 1.f;

	Out.vNormal = vNormal * 0.5f + 0.5f;

	Out.vDiffuse.rgb *= clamp(vMask.g, 0.8f, 1.f);

	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCameraFar, 1.f, 1.f);
	Out.vVehicleDepth.r = Out.vDepth.g;

	Out.vEmissive = vector(0.f, 0.f, 0.f, 0.f);

	if (1.f > g_fDissolveAlpha)
	{
		float fDissolve = g_DissolveTexture.Sample(LinearSampler, In.vTexCoord).r;

		if (g_fDissolveAlpha < fDissolve)
			discard;

		float fStep = smoothstep(g_fDissolveAlpha, g_fDissolveAlpha + 0.1f, fDissolve);

		if (1.f > fStep)
			Out.vEmissive.rgb = g_vDissolveColor;
	}

	if (g_bUseEmissiveTexture)
		Out.vEmissive += g_EmissiveTexture.Sample(LinearSampler, In.vTexCoord);

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

	Out.vColor.rgb = vDiffuse.rgb * 0.5f;
	Out.vColor.a = 1.f;

	Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCameraFar, 1.f, 1.f);
	Out.vVehicleDepth.r = Out.vDepth.g;

	return Out;
}

technique11 Default
{
	pass Vehicle
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