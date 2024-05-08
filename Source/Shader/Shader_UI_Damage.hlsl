#include "Shader_Defines.hlsli"
#include "Shader_Material_Defines.hlsli"

matrix g_ViewMatrix, g_ProjMatrix;

matrix g_OrthoProjMatrix;
float g_fWidth;
float g_fHeight;

struct VS_IN
{
	float3	vPosition			: POSITION;
	float2	vPsize				: PSIZE;

	float3	vInstancePosition	: TEXCOORD0;
	float2	vAddPosition		: TEXCOORD1;
	float2	vSizeAlpha			: TEXCOORD2;
	float4	vUV					: TEXCOORD3;
	float4	vColor				: TEXCOORD4;
	uint	iType				: TEXCOORD5;
};

struct VS_OUT
{
	float4	vPosition		: POSITION;
	float2	vAddPosition	: TEXCOORD0;
	float2	vSizeAlpha		: TEXCOORD1;
	float4	vUV				: TEXCOORD2;
	float4	vColor			: TEXCOORD3;
	uint	iType			: TEXCOORD4;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	// Instance Position = 월드 기준
	// vAddPosition = 스크린 기준

	matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
	vector vPosition = float4(In.vPosition + In.vInstancePosition, 1.f);
	
	Out.vPosition = mul(vPosition, matVP);
	Out.vPosition /= Out.vPosition.w;
	Out.vSizeAlpha = In.vSizeAlpha;
	Out.vAddPosition = In.vAddPosition;
	Out.vUV = In.vUV;
	Out.vColor = In.vColor;
	Out.iType = In.iType;

	return Out;
}

struct GS_IN
{
	float4	vPosition		: POSITION;
	float2	vAddPosition	: TEXCOORD0;
	float2	vSizeAlpha		: TEXCOORD1;
	float4	vUV				: TEXCOORD2;
	float4	vColor			: TEXCOORD3;
	uint	iType			: TEXCOORD4;
};

struct GS_OUT
{
	float4 vPosition	: SV_POSITION;
	float2 vTexCoord	: TEXCOORD0;
	float fAlpha		: TEXCOORD1;
	float4 vColor		: TEXCOORD2;
	uint	iType		: TEXCOORD3;
};

vector GetNDC(vector vPosition)
{
	vector vOut = vPosition;
	float fWidth = g_fWidth;
	float fHeight = g_fHeight;

	vOut.x = 2.f * vOut.x / fWidth - 1.f;
	vOut.y = 1.f - 2.f * vOut.y / fHeight;
	vOut.z = 0.f;
	vOut.w = 1.f;

	return vOut;
}

[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> Triangles)
{
	GS_OUT Out[4];

	float4 vUV = In[0].vUV;
	float4 vColor = In[0].vColor;
	float fAlpha = In[0].vSizeAlpha.y;
	float fSize = In[0].vSizeAlpha.x;
	uint iType = In[0].iType;

	vector vRight = vector(fSize * 0.5f, 0.f, 0.f, 0.f);
	vector vUp = vector(0.f, fSize * 0.5f, 0.f, 0.f);
	vector vPosition = In[0].vPosition;

	// 뷰포트 변환
	vPosition.x = (vPosition.x + 1.f) * 0.5f * g_fWidth;
	vPosition.y = (1.f - vPosition.y) * 0.5f * g_fHeight;
	vPosition.z = 0.f;
	vPosition.w = 1.f;

	vPosition.x += In[0].vAddPosition.x;
	vPosition.y += In[0].vAddPosition.y;

	Out[0].vPosition = GetNDC(vPosition - vRight - vUp);
	Out[0].vTexCoord = vUV.xy;
	Out[0].fAlpha = fAlpha;
	Out[0].vColor = vColor;
	Out[0].iType = iType;

	Out[1].vPosition = GetNDC(vPosition + vRight - vUp);
	Out[1].vTexCoord = vUV.zy;
	Out[1].fAlpha = fAlpha;
	Out[1].vColor = vColor;
	Out[1].iType = iType;

	Out[2].vPosition = GetNDC(vPosition + vRight + vUp);
	Out[2].vTexCoord = vUV.zw;
	Out[2].fAlpha = fAlpha;
	Out[2].vColor = vColor;
	Out[2].iType = iType;

	Out[3].vPosition = GetNDC(vPosition - vRight + vUp);
	Out[3].vTexCoord = vUV.xw;
	Out[3].fAlpha = fAlpha;
	Out[3].vColor = vColor;
	Out[3].iType = iType;

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
	float4	vPosition	: SV_POSITION;
	float2	vTexCoord	: TEXCOORD0;
	float	fAlpha		: TEXCOORD1;
	float4	vColor		: TEXCOORD2;
	uint	iType		: TEXCOORD3;
};

struct PS_OUT
{
	float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	if (0.f >= In.fAlpha)
		discard;

	float4 vColor = float4(0.f, 0.f, 0.f, 0.f);
	
	if (2 >= In.iType)
	{
		vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexCoord);

		if (0.1f >= vColor.b)
			discard;

		vColor.rgb = In.vColor.rgb * vColor.b;
	}
	else if (3 == In.iType)
	{
		vColor = g_NormalTexture.Sample(LinearSampler, In.vTexCoord);

		if (0.f >= vColor.a)
			discard;
	}

	Out.vColor = vColor;
	Out.vColor.a = In.fAlpha;

	return Out;
}

PS_OUT PS_CRITICAL(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	if (0.f >= In.fAlpha)
		discard;

	float4 vColor = g_NormalTexture.Sample(LinearSampler, In.vTexCoord);

	if (0.1f >= vColor.a)
		discard;

	Out.vColor = vColor;
	Out.vColor.a = In.fAlpha;

	return Out;
}

technique11 UIDamage
{
	pass Damage
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}