#include "Shader_Defines.hlsli"
#include "Shader_Variable_Defines.hlsli"
#include "Shader_Material_Defines.hlsli"
#include "Shader_Client_Functions.hlsli"

float g_fLightSmooth = 0.1f;
float4 g_vBaseColor = float4(1.f, 1.f, 1.f, 0.f);
float4 g_vShadowColor = float4(0.9f, 0.9f, 0.9f, 0.f);

float g_fRadians[2048];

float g_fAlpha_Discard;

// PBR Test
float g_fLightAttenuation = 2.f;
float3 g_vLightColor = float3(0.9f, 0.9f, 0.9f);
float g_fShadowAmount = 0.1f;
bool g_bUsePBR;

struct VS_IN
{
	float3 vPosition : POSITION0;
	float3 vNormal : NORMAL0;
	float2 vTexCoord : TEXCOORD0;
	float3 vTangent : TANGENT0;
	float4 vVertexColor : TEXCOORD1;

	row_major matrix matWorld : WORLD;
	row_major matrix matPivot : PIVOT;

	uint iInstanceID : SV_InstanceID;
};

struct VS_OUT
{
	float4 vPosition : POSITION;
	float4 vNormal : NORMAL;
	float2 vTexCoord : TEXCOORD0;
	float3 vTangent : TANGENT;

	matrix matWorld : WORLD;

	uint iInstanceID : SV_InstanceID;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	matrix matWorld;

	if (g_bIsInstanced)
		matWorld = mul(In.matPivot, In.matWorld);
	else
		matWorld = g_WorldMatrix;

	Out.vPosition = vector(In.vPosition, 1.f);
	Out.vNormal = vector(In.vNormal, 0.f);
	Out.vTexCoord = In.vTexCoord;
	Out.vTangent = In.vTangent;
	Out.matWorld = matWorld;
	Out.iInstanceID = In.iInstanceID;

	return Out;
}

struct GS_IN
{
	float4 vPosition : POSITION;
	float4 vNormal : NORMAL;
	float2 vTexCoord : TEXCOORD0;
	float3 vTangent : TANGENT;

	matrix matWorld : WORLD;

	uint iInstanceID : SV_InstanceID;
};

struct GS_OUT
{
	float4 vPosition : SV_POSITION;
	float4 vNormal : NORMAL;
	float2 vTexCoord : TEXCOORD0;
	float4 vWorldPos : TEXCOORD1;
	float4 vProjPos : TEXCOORD2;
	float3 vTangent : TANGENT;
	float3 vBiNormal : BINORMAL;
};

[maxvertexcount(3)]
void GS_MAIN(triangle GS_IN In[3], inout TriangleStream<GS_OUT> Triangles)
{
	float fX = sin(g_fRadians[In[0].iInstanceID]) * 10.f;

	[unroll]
	for (int i = 0; i < 3; ++i)
	{
		if (In[i].vPosition.y > 0.f)
			In[i].vPosition.x += fX;

		GS_OUT Out = (GS_OUT)0;
		matrix matWVP = mul(In[i].matWorld, g_ViewMatrix);
		matWVP = mul(matWVP, g_ProjMatrix);

		Out.vPosition = mul(In[i].vPosition, matWVP);
		Out.vNormal = mul(In[i].vNormal, In[i].matWorld);
		Out.vTexCoord = In[i].vTexCoord;
		Out.vWorldPos = mul(In[i].vPosition, In[i].matWorld);
		Out.vProjPos = Out.vPosition;

		Out.vTangent = normalize(mul(vector(In[i].vTangent, 0.f), In[i].matWorld)).xyz;
		Out.vBiNormal = normalize(cross(Out.vNormal.xyz, Out.vTangent));

		Triangles.Append(Out);
	}

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
};

struct PS_OUT
{
	float4 vDiffuse : SV_TARGET0;
	float4 vNormal : SV_TARGET1;
	float4 vDepth : SV_TARGET2;
	float4 vGrassDepth : SV_TARGET3;
	float4 vEmissive : SV_TARGET4;
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
	PS_OUT Out = (PS_OUT)0;

	float4 vDiffuse = float4(0.f, 0.f, 0.f, 0.f);

	if (g_bUseDiffuseTexture)
		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexCoord);

	if (dot(vDiffuse.rgb, vDiffuse.rgb) <= 0.f)
		discard;

	if (vDiffuse.a < g_fAlpha_Discard)
		discard;

	float4 vNormal = GetNormal(In.vTexCoord, In.vTangent, In.vBiNormal, In.vNormal.xyz);
	float4 vEmissive = float4(0.f, 0.f, 0.f, 0.f);
	float4 vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCameraFar, 0.5f, 1.f); // y 0.5 : 명암까지만 적용

	if (g_bUseEmissiveTexture)
		vEmissive += g_EmissiveTexture.Sample(LinearSampler, In.vTexCoord);

	Out.vDiffuse = vDiffuse;
	Out.vNormal = vNormal * 0.5f + 0.5f;
	Out.vDepth = vDepth;
	Out.vGrassDepth = vDepth.g;
	Out.vEmissive = vEmissive;

	return Out;
}

technique11 Grass
{
	pass Default
	{
		SetRasterizerState(RS_Sky);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}