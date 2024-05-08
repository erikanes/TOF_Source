#include "Shader_Defines.hlsli"
#include "Shader_Variable_Defines.hlsli"
#include "Shader_Material_Defines.hlsli"
#include "Shader_Client_Functions.hlsli"

float g_fLightSmooth = 0.1f;
float4 g_vBaseColor = float4(1.f, 1.f, 1.f, 0.f);
float4 g_vShadowColor = float4(0.9f, 0.9f, 0.9f, 0.f);

float g_fAlpha_Discard;
float g_fAlpha;

// Camera Distance Noise
texture2D g_CameraNoiseTexture;

// PBR Test
float g_fLightAttenuation = 2.f;
float3 g_vLightColor = float3(0.9f, 0.9f, 0.9f);
float g_fShadowAmount = 0.1f;
bool g_bUsePBR;

// Dissolve
float g_fDissolveAlpha;
float4 g_vDissolveColor;
texture2D g_DissolveTexture;

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
	float fToCameraDistance : TEXCOORD5;
};

struct VS_SHADOW_OUT
{
	float4 vPosition : SV_POSITION;
	float2 vTexCoord : TEXCOORD0;
	float4 vProjPos : TEXCOORD1;
};

struct VS_GRASS_OUT
{
	float4 vPosition : POSITION;
	float4 vNormal : NORMAL;
	float2 vTexCoord : TEXCOORD0;
	float4 vWorldPos : TEXCOORD1;
	float4 vProjPos : TEXCOORD2;
	float3 vTangent : TANGENT;
	float3 vBiNormal : BINORMAL;

	float3 vToCamera : TEXCOORD3;
	float3 vToLight : TEXCOORD4;
	float fToCameraDistance : TEXCOORD5;
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
	Out.vTexCoord = In.vTexCoord;
	Out.vWorldPos = mul(float4(In.vPosition, 1.f), matWorld);
	Out.vProjPos = vPosition;

	Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), matWorld)).xyz;
	Out.vBiNormal = normalize(cross(Out.vNormal.xyz, Out.vTangent));

	Out.vToCamera = normalize(g_vCamPosition.xyz - Out.vWorldPos.xyz);
	Out.vToLight = normalize(g_vLightPosition.xyz - Out.vWorldPos.xyz);
	Out.fToCameraDistance = distance(g_vCamPosition.xyz, Out.vWorldPos.xyz);

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

VS_GRASS_OUT VS_GRASS(VS_IN IN)
{
	VS_GRASS_OUT Out = (VS_GRASS_OUT)0;

	return Out;
}


struct GS_GRASS_IN
{
	float4 vPosition : POSITION;
	float4 vNormal : NORMAL;
	float2 vTexCoord : TEXCOORD0;
	float4 vWorldPos : TEXCOORD1;
	float4 vProjPos : TEXCOORD2;
	float3 vTangent : TANGENT;
	float3 vBiNormal : BINORMAL;

	float3 vToCamera : TEXCOORD3;
	float3 vToLight : TEXCOORD4;
	float fToCameraDistance : TEXCOORD5;
};

struct GS_GRASS_OUT
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
	float fToCameraDistance : TEXCOORD5;
};

[maxvertexcount(6)]
void GS_GRASS(triangle GS_GRASS_IN In[3][2], inout TriangleStream<GS_GRASS_OUT> Triangles)
{
	GS_GRASS_OUT Out[4];

	/*
	In[0][0], In[1][0] 이 두개만 흔들면 됨
	*/

	Out[0] = (GS_GRASS_OUT)0;
	Out[1] = (GS_GRASS_OUT)0;
	Out[2] = (GS_GRASS_OUT)0;

	Triangles.Append(Out[0]);
	Triangles.Append(Out[1]);
	Triangles.Append(Out[2]);
	Triangles.RestartStrip();
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
	float fToCameraDistance : TEXCOORD5;
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


float4 GetSecondDiffuse(in float2 vTexCoord)
{
	float4 vSecondDiffuse = float4(0.f, 0.f, 0.f, 0.f);

	if (g_bUseSecondDiffuseTexture)
		vSecondDiffuse.xyz = g_SecondDiffuseTexture.Sample(LinearSampler, vTexCoord).xyz;
	
	return vSecondDiffuse;
}

float4 GetNormal(in float2 vTexCoord, in float3 vTangent, in float3 vBiNormal, in float3 vNormal)
{
	float4 vOutNormal = float4(vNormal, 0.f);

	if (g_bUseNormalTexture)
	{
		vOutNormal = g_NormalTexture.Sample(LinearSampler, vTexCoord);

		// b채널이 없는 경우를 대비
		if (0.f >= vOutNormal.b)
			vOutNormal.b = sqrt(1.f - saturate(dot(vOutNormal.rg, vOutNormal.rg)));

		vOutNormal = vOutNormal * 2.f - 1.f;

		float3x3 matWorld = float3x3(vTangent, vBiNormal, vNormal.xyz);
		vOutNormal = float4(normalize(mul(vOutNormal.xyz, matWorld)), 0.f);
	}

	return vOutNormal;
}

float4 GetSecondNormal(in float2 vTexCoord, in float3 vTangent, in float3 vBiNormal, in float3 vNormal)
{
	float4 vOutNormal = float4(vNormal, 0.f);

	if (g_bUseSecondNormalTexture)
	{
		vOutNormal = g_SecondNormalTexture.Sample(LinearSampler, vTexCoord);

		if (0.f <= vOutNormal.b)
			vOutNormal.b = sqrt(1 - saturate(dot(vOutNormal.rg, vOutNormal.rg)));

		vOutNormal = vOutNormal * 2.f - 1.f;

		float3x3 matWorld = float3x3(vTangent, vBiNormal, vNormal);
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



PS_OUT PS_MAIN(PS_IN In)
{
	float fCameraNoise = g_CameraNoiseTexture.Sample(LinearSampler, In.vTexCoord * 100.f).x;

	if (2.f >= In.fToCameraDistance && 0.1f >= fCameraNoise)
		discard;

	PS_OUT Out = (PS_OUT)0;

	float4 vDiffuse = float4(0.f, 0.f, 0.f, 0.f);

	if (g_bUseDiffuseTexture)
		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexCoord);

	vDiffuse.a = vDiffuse.a * g_fAlpha;

	if (dot(vDiffuse.rgb, vDiffuse.rgb) <= 0.f)
		discard;

	if (vDiffuse.a < g_fAlpha_Discard)
		discard;
	
	float4 vSecondDiffuse = GetSecondDiffuse(In.vTexCoord);

	float4 vNormal = GetNormal(In.vTexCoord, In.vTangent, In.vBiNormal, In.vNormal.xyz);
	float4 vSecondNormal = GetSecondNormal(In.vTexCoord, In.vTangent, In.vBiNormal, In.vNormal.xyz);

	float4 vNoise = GetNoise(In.vTexCoord);

	float4 vEmissive = float4(0.f, 0.f, 0.f, 0.f);
	float4 vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCameraFar, 0.5f, 1.f); // y 0.5 : 명암까지만 적용

	if (g_bUseEmissiveTexture)
		vEmissive += g_EmissiveTexture.Sample(LinearSampler, In.vTexCoord);

	Out.vDiffuse = vDiffuse;
	Out.vNormal = vNormal * 0.5f + 0.5f;
	Out.vDepth = vDepth;
	Out.vMapDepth = vDepth.g;
	Out.vEmissive = vEmissive;

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

PS_OUT PS_DISSOLVE(PS_IN In)
{
	float fCameraNoise = g_CameraNoiseTexture.Sample(LinearSampler, In.vTexCoord * 100.f).x;

	if (2.f >= In.fToCameraDistance && 0.1f >= fCameraNoise)
		discard;

	PS_OUT Out = (PS_OUT)0;

	float4 vDiffuse = float4(0.f, 0.f, 0.f, 0.f);

	if (g_bUseDiffuseTexture)
		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexCoord);

	vDiffuse.a = vDiffuse.a * g_fAlpha;

	if (dot(vDiffuse.rgb, vDiffuse.rgb) <= 0.f)
		discard;

	if (vDiffuse.a < g_fAlpha_Discard)
		discard;

	float4 vSecondDiffuse = GetSecondDiffuse(In.vTexCoord);

	float4 vNormal = GetNormal(In.vTexCoord, In.vTangent, In.vBiNormal, In.vNormal.xyz);
	float4 vSecondNormal = GetSecondNormal(In.vTexCoord, In.vTangent, In.vBiNormal, In.vNormal.xyz);

	float4 vNoise = GetNoise(In.vTexCoord);

	float4 vEmissive = float4(0.f, 0.f, 0.f, 0.f);
	float4 vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCameraFar, 0.5f, 1.f); // y 0.5 : 명암까지만 적용

	if (g_bUseEmissiveTexture)
		vEmissive += g_EmissiveTexture.Sample(LinearSampler, In.vTexCoord);

	Out.vDiffuse = vDiffuse;
	Out.vNormal = vNormal * 0.5f + 0.5f;
	Out.vDepth = vDepth;
	Out.vMapDepth = vDepth.g;
	Out.vEmissive = vEmissive;

	if (1.f > g_fDissolveAlpha)
	{
		float fDissolve = g_DissolveTexture.Sample(LinearSampler, In.vTexCoord).r;

		if (g_fDissolveAlpha < fDissolve)
			discard;

		float fStep = smoothstep(g_fDissolveAlpha, g_fDissolveAlpha + 0.1f, fDissolve);

		if (1.f > fStep)
			Out.vEmissive.rgb = g_vDissolveColor; // 무기 속성에 따라 색상 결정
	}

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

	pass Grass
	{
		SetRasterizerState(RS_Sky);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);
		VertexShader = compile vs_5_0 VS_GRASS();
		GeometryShader = compile gs_5_0 GS_GRASS();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

    pass BasketBallRoller
    {
        SetRasterizerState(RS_Sky);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetDepthStencilState(DSS_VFX, 0);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

	pass Dissolve
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DISSOLVE();
	}
}