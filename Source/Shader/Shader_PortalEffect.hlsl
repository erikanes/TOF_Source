#include "Shader_Defines.hlsli"
#include "Shader_Material_Defines.hlsli"
#include "Shader_Variable_Defines.hlsli"

float g_fTime;
float g_fSize;
float g_fPower;

struct VS_IN
{
	float3 vPosition : POSITION;
	float2 vPSize : PSIZE;
};

struct VS_OUT
{
	float4 vPosition : POSITION;
	float2 vPSize : PSIZE;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	Out.vPosition = float4(In.vPosition, 1.f);
	Out.vPSize = In.vPSize;

	return Out;
}


struct GS_IN
{
	float4	vPosition		: POSITION;
	float2	vPsize			: PSIZE;
};

struct GS_OUT
{
	float4 vPosition		: SV_POSITION;
	float2 vTexCoord		: TEXCOORD0;
	float4 vProjPos			: TEXCOORD1;
};

[maxvertexcount(12)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> Triangles)
{
	GS_OUT Out[6];

	matrix matWVP = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWVP, g_ProjMatrix);

	/*
	* 
		0	1
	3			2
	4			5

	*/

	float fPX[6] = { -0.1f, 0.1f, 0.5f, -0.5f, -0.5f, 0.5f };
	float fPY[6] = { 1.35f, 1.35f, 1.2f, 1.2f, 0.f, 0.f };
	float2 vCoord[6] =
	{
		float2(0.3f, 0.f),
		float2(0.6f, 0.f),
		float2(1.f, 0.3f),
		float2(0.f, 0.3f),
		float2(0.f, 1.f),
		float2(1.f, 1.f)
	};

	[unroll]
	for (int i = 0; i < 6; ++i)
	{
		float4 vPosition = float4(fPX[i] * g_fSize, fPY[i] * g_fSize, 0.f, 1.f);
		Out[i].vPosition = mul(vPosition, matWVP);
		Out[i].vProjPos = Out[i].vPosition;
		Out[i].vTexCoord = vCoord[i];
	}

	Triangles.Append(Out[0]);
	Triangles.Append(Out[1]);
	Triangles.Append(Out[2]);
	Triangles.RestartStrip();

	Triangles.Append(Out[0]);
	Triangles.Append(Out[2]);
	Triangles.Append(Out[3]);
	Triangles.RestartStrip();

	Triangles.Append(Out[3]);
	Triangles.Append(Out[2]);
	Triangles.Append(Out[5]);
	Triangles.RestartStrip();

	Triangles.Append(Out[3]);
	Triangles.Append(Out[5]);
	Triangles.Append(Out[4]);
	Triangles.RestartStrip();
}


struct PS_IN
{
	float4 vPosition	: SV_POSITION;
	float2 vTexCoord	: TEXCOORD0;
	float4 vProjPos		: TEXCOORD1;
};

struct PS_OUT
{
	float4 vColor : SV_TARGET0;
	float4 vDepth : SV_TARGET1;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float2 vTexCoord = In.vTexCoord;
	vTexCoord.y += g_fTime; // g_fTime : 0~1

	Out.vColor = g_NoiseTexture.Sample(LinearSampler, vTexCoord);
	Out.vDepth.r = In.vProjPos.w / g_fCameraFar;

	Out.vColor.rg *= g_fPower;

	return Out;
}

technique11 PortalEffect
{
	pass Distortion
	{
		SetRasterizerState(RS_NoCullSolid);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_VFX, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}