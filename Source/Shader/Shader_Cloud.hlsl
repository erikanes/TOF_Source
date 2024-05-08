#include "Shader_Defines.hlsli"
#include "Shader_Material_Defines.hlsli"
#include "Shader_Variable_Defines.hlsli"

texture2D g_DissolveTexture;
float g_fTimeDelta;
float g_fScale;
float g_fBrightness;

struct VS_IN
{
	float3	vPosition : POSITION;
	float2	vTexCoord : TEXCOORD0;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
	float2 vTexCoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	matrix matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matrix matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexCoord = In.vTexCoord;
	Out.vTexCoord.x *= 2.f;

	return Out;
}

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexCoord : TEXCOORD0;
};

struct PS_OUT
{
	float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float2 vTexCoord = In.vTexCoord;
	vTexCoord.x += g_fTimeDelta;

	float4 vMask = g_MaskMapTexture.Sample(LinearSampler, vTexCoord);
	vMask *= g_fScale;
	vMask.xy += vTexCoord + g_fTimeDelta * 2.f;

	In.vTexCoord.x /= 2.f;

	float4 vCloudColor = g_DiffuseTexture.Sample(LinearSampler, vMask.xy);

	Out.vColor = vCloudColor * g_fBrightness;

	clip(Out.vColor.a <= 0.f ? -1.f : 1.f);

	return Out;
}


technique11 Cloud
{
	pass Cloud
	{
		SetRasterizerState(RS_NoCullSolid);
		SetBlendState(BS_RGBBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_VFX, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
};