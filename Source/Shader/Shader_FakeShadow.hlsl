#include "Shader_Defines.hlsli"
#include "Shader_Material_Defines.hlsli"
#include "Shader_Variable_Defines.hlsli"

float2 g_vSize;

struct VS_IN
{
	float3 vPosition : POSITION;
	float2 pSize : PSIZE;
};

struct VS_OUT
{
	float3 vWorldPosition : POSITION;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	Out.vWorldPosition = g_WorldMatrix[3].xyz;

	return Out;
}

struct GS_IN
{
	float3 vWorldPosition : POSITION;
};

struct GS_OUT
{
	vector vPosition : SV_POSITION;
	float2 vTexCoord : TEXCOORD;
};

[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> Triangles)
{
	GS_OUT Out[4];

	float2 vSize = g_vSize;

	vector vWorldRight = vector(1.f, 0.f, 0.f, 0.f);
	vector vWorldUp = vector(0.f, 0.f, 1.f, 0.f);
	vector vWorldLook = vector(0.f, 1.f, 0.f, 0.f);
	vector vWorldPosition = vector(In[0].vWorldPosition, 1.f);

	vWorldPosition.y += 0.01f;

	matrix matWorld;
	matWorld[0] = vWorldRight;
	matWorld[1] = vWorldUp;
	matWorld[2] = vWorldLook;
	matWorld[3] = vWorldPosition;

	matrix matWVP = mul(matWorld, g_ViewMatrix);
	matWVP = mul(matWVP, g_ProjMatrix);

	vector vPosition = vector(0.f, 0.f, 0.f, 1.f);
	vector vRight = vector(vSize.x * 0.5f, 0.f, 0.f, 0.f);
	vector vUp = vector(0.f, vSize.y * 0.5f, 0.f, 0.f);

	Out[0].vPosition = mul((vPosition - vRight + vUp), matWVP);
	Out[0].vTexCoord = float2(0.f, 0.f);

	Out[1].vPosition = mul((vPosition + vRight + vUp), matWVP);
	Out[1].vTexCoord = float2(1.f, 0.f);

	Out[2].vPosition = mul((vPosition + vRight - vUp), matWVP);
	Out[2].vTexCoord = float2(1.f, 1.f);

	Out[3].vPosition = mul((vPosition - vRight - vUp), matWVP);
	Out[3].vTexCoord = float2(0.f, 1.f);

	Triangles.Append(Out[0]);
	Triangles.Append(Out[1]);
	Triangles.Append(Out[2]);
	Triangles.RestartStrip();

	Triangles.Append(Out[0]);
	Triangles.Append(Out[2]);
	Triangles.Append(Out[3]);
	Triangles.RestartStrip();
}

struct PS_IN
{
	vector vPosition : SV_POSITION;
	float2 vTexCoord : TEXCOORD0;
};

struct PS_OUT
{
	float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float4 vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexCoord);

	Out.vColor = float4(0.2f * vDiffuse.a, 0.2f * vDiffuse.a, 0.2f * vDiffuse.a, vDiffuse.a);

	return Out;
}

technique11 FakeShadow
{
	pass FakeShadow
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
};