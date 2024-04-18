#include "Shader_Defines.hlsli"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

struct VS_IN
{
	float3 vPosition : POSITION;

	float3 vP1 : TEXCOORD0;
	float3 vP2 : TEXCOORD1;
	float3 vP3 : TEXCOORD2;
	float4 vColor : TEXCOORD3;

	uint vID : SV_VertexID;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
	float4 vColor : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	matrix matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	if (0 == In.vID)
		Out.vPosition = mul(vector(In.vP1, 1.f), matWVP);

	else if (1 == In.vID)
		Out.vPosition = mul(vector(In.vP2, 1.f), matWVP);

	else
		Out.vPosition = mul(vector(In.vP3, 1.f), matWVP);

	Out.vColor = In.vColor;

	return Out;
}

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float4 vColor : TEXCOORD0;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)Out;

	Out.vColor = In.vColor;

	return Out;
}


technique11		DefaultTechnique
{
	pass Instance_Cell
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
}