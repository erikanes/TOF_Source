#include "Shader_Defines.hlsli"
#include "Shader_Material_Defines.hlsli"

texture2D g_Texture[11];

float4 g_vPosition[11]; // ndc로 이미 변환된 상태
float4 g_vScaleRotate[11];
float4 g_vColor[11];
int g_iTextureIndex[11];
float g_fBrightness;

float g_fWidth;
float g_fHeight;

/*
플레어 개수 11개
플레어 위치 (2차원 공간)
플레어 색상
플레어 크기, 회전 (2차원 공간)
*/

struct VS_IN
{
	float3				vPosition : POSITION;
};

struct VS_OUT
{
	float4 vPosition : POSITION;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	Out.vPosition = float4(In.vPosition, 1.f);

	return Out;
}

struct GS_FLARE_IN
{
	float4 vPosition : POSITION;
};

struct GS_FLARE_OUT
{
	float4 vPosition : SV_POSITION;
	float2 vTexCoord : TEXCOORD0;
	float4 vColor : TEXCOORD1;
	int iTextureIndex : TEXCOORD2;
};

vector Rotate(vector vPosition, float2 vRadians)
{
	float2 vPos = vPosition.xy;

	vPosition.x = vPos.x * cos(vRadians.x) - vPos.y * sin(vRadians.x);
	vPosition.y = vPos.x * sin(vRadians.y) + vPos.y * cos(vRadians.y);
	vPosition.z = vPosition.z;
	vPosition.w = 1.f;

	return vPosition;
}

vector ToScreen(vector vPosition)
{
	vPosition.x = (vPosition.x + 1.f) * 0.5f * g_fWidth;
	vPosition.y = (1.f - vPosition.y) * 0.5f * g_fHeight;
	vPosition.z = vPosition.z;
	vPosition.w = 1.f;

	return vPosition;
}

vector ToNDC(vector vPosition)
{
	vPosition.x = 2.f * vPosition.x / g_fWidth - 1.f;
	vPosition.y = 1.f - 2.f * vPosition.y / g_fHeight;
	vPosition.z = vPosition.z;
	vPosition.w = 1.f;

	return vPosition;
}

vector CalculateSRT(float2 vScale, float2 vRotate, vector vTranslation)
{
	vector vOrigin = vector(0.f, 0.f, 0.f, 1.f);
	vOrigin = Rotate(vector(vScale * g_fWidth * 0.5f, 0.f, 1.f), vRotate);

	vTranslation = vOrigin + ToScreen(vTranslation);
	vTranslation.w = 1.f;

	return ToNDC(vTranslation);
}

[maxvertexcount(6)]
void GS_CORONA(point GS_FLARE_IN In[1], inout TriangleStream<GS_FLARE_OUT> Triangles)
{
	GS_FLARE_OUT Out[4];

	vector vOrigin = vector(0.f, 0.f, 0.f, 1.f);
	vector vPosition = vector(g_vPosition[0].xyz, 1.f);
	float2 vScale = float2(g_vScaleRotate[0].x * 0.5f, g_vScaleRotate[0].x * 0.5f);
	float2 vRotate = g_vScaleRotate[0].y;
	float4 vColor = g_vColor[0];
	float fAspect = g_vScaleRotate[0].z;

	Out[0].vPosition = CalculateSRT(float2(-vScale.x, vScale.y), vRotate, vPosition);
	Out[0].vTexCoord = float2(0.f, 0.f);
	Out[0].vColor = vColor;
	Out[0].iTextureIndex = 0;

	Out[1].vPosition = CalculateSRT(float2(vScale.x, vScale.y), vRotate, vPosition);
	Out[1].vTexCoord = float2(1.f, 0.f);
	Out[1].vColor = vColor;
	Out[1].iTextureIndex = 0;

	Out[2].vPosition = CalculateSRT(float2(vScale.x, -vScale.y), vRotate, vPosition);
	Out[2].vTexCoord = float2(1.f, 1.f);
	Out[2].vColor = vColor;
	Out[2].iTextureIndex = 0;

	Out[3].vPosition = CalculateSRT(float2(-vScale.x, -vScale.y), vRotate, vPosition);
	Out[3].vTexCoord = float2(0.f, 1.f);
	Out[3].vColor = vColor;
	Out[3].iTextureIndex = 0;

	Triangles.Append(Out[0]);
	Triangles.Append(Out[1]);
	Triangles.Append(Out[2]);
	Triangles.RestartStrip();

	Triangles.Append(Out[0]);
	Triangles.Append(Out[2]);
	Triangles.Append(Out[3]);
	Triangles.RestartStrip();
}

[maxvertexcount(66)]
void GS_FLARE(point GS_FLARE_IN In[1], inout TriangleStream<GS_FLARE_OUT> Triangles)
{
	GS_FLARE_OUT Out[44];
	int iIndex = 0;

	if (g_fBrightness > 0.f)
	{
		[unroll]
		for (int i = 0; i < 11; ++i)
		{
			vector vOrigin = vector(0.f, 0.f, 0.f, 1.f);
			vector vPosition = vector(g_vPosition[i].xyz, 1.f);
			float2 vScale = float2(g_vScaleRotate[i].x * 0.5f, g_vScaleRotate[i].x * 0.5f);
			float2 vRotate = g_vScaleRotate[i].y;

			float4 vColor = g_vColor[i];
			int iTextureIndex = g_iTextureIndex[i];

			Out[iIndex].vPosition = CalculateSRT(float2(-vScale.x, vScale.y), vRotate, vPosition);
			Out[iIndex].vTexCoord = float2(0.f, 0.f);
			Out[iIndex].vColor = vColor;
			Out[iIndex].iTextureIndex = iTextureIndex;

			Out[iIndex + 1].vPosition = CalculateSRT(float2(vScale.x, vScale.y), vRotate, vPosition);
			Out[iIndex + 1].vTexCoord = float2(1.f, 0.f);
			Out[iIndex + 1].vColor = vColor;
			Out[iIndex + 1].iTextureIndex = iTextureIndex;

			Out[iIndex + 2].vPosition = CalculateSRT(float2(vScale.x, -vScale.y), vRotate, vPosition);
			Out[iIndex + 2].vTexCoord = float2(1.f, 1.f);
			Out[iIndex + 2].vColor = vColor;
			Out[iIndex + 2].iTextureIndex = iTextureIndex;

			Out[iIndex + 3].vPosition = CalculateSRT(float2(-vScale.x, -vScale.y), vRotate, vPosition);
			Out[iIndex + 3].vTexCoord = float2(0.f, 1.f);
			Out[iIndex + 3].vColor = vColor;
			Out[iIndex + 3].iTextureIndex = iTextureIndex;

			Triangles.Append(Out[iIndex]);
			Triangles.Append(Out[iIndex + 1]);
			Triangles.Append(Out[iIndex + 2]);
			Triangles.RestartStrip();

			Triangles.Append(Out[iIndex]);
			Triangles.Append(Out[iIndex + 2]);
			Triangles.Append(Out[iIndex + 3]);
			Triangles.RestartStrip();

			iIndex += 4;
		}
	}
}

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexCoord : TEXCOORD0;
	float4 vColor : TEXCOORD1;
	int iTextureIndex : TEXCOORD2;
};

struct PS_OUT
{
	float4 vColor : SV_TARGET0;
};

PS_OUT PS_CORONA(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = g_Texture[0].Sample(LinearSampler, In.vTexCoord);

	if (0.f >= Out.vColor.r)
		discard;

	Out.vColor *= In.vColor;
	Out.vColor.a = 1.f;

	return Out;
}

PS_OUT PS_FLARE(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;
	int iTextureIndex = In.iTextureIndex;

	if (1 == iTextureIndex)
		Out.vColor = g_Texture[1].Sample(LinearSampler, In.vTexCoord);
	else if (2 == iTextureIndex)
		Out.vColor = g_Texture[2].Sample(LinearSampler, In.vTexCoord);
	else if (3 == iTextureIndex)
		Out.vColor = g_Texture[3].Sample(LinearSampler, In.vTexCoord);
	else if (4 == iTextureIndex)
		Out.vColor = g_Texture[4].Sample(LinearSampler, In.vTexCoord);
	else if (5 == iTextureIndex)
		Out.vColor = g_Texture[5].Sample(LinearSampler, In.vTexCoord);
	else if (6 == iTextureIndex)
		Out.vColor = g_Texture[6].Sample(LinearSampler, In.vTexCoord);
	else if (7 == iTextureIndex)
		Out.vColor = g_Texture[7].Sample(LinearSampler, In.vTexCoord);
	else if (8 == iTextureIndex)
		Out.vColor = g_Texture[8].Sample(LinearSampler, In.vTexCoord);
	else if (9 == iTextureIndex)
		Out.vColor = g_Texture[9].Sample(LinearSampler, In.vTexCoord);
	else if (10 == iTextureIndex)
		Out.vColor = g_Texture[10].Sample(LinearSampler, In.vTexCoord);

	if (0.01f >= Out.vColor.a)
		discard;

	if (0.f >= dot(Out.vColor.rgb, Out.vColor.rgb))
		discard;

	Out.vColor.rgb *= In.vColor.rgb;
	Out.vColor.a *= g_fBrightness;

	return Out;
}


DepthStencilState DDS_Lensflare_Corona
{
	DepthEnable = true;
	DepthWriteMask = zero;
	DepthFunc = less_equal;
};

BlendState BS_Lensflare
{
	BlendEnable[0] = true;

	SrcBlend = one;
	DestBlend = one;
	BlendOp = Add;
};

technique11 LensFlare
{
	pass Corona
	{
		SetRasterizerState(RS_NoCullSolid);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DDS_Lensflare_Corona, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_CORONA();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_CORONA();
	}

	pass Flares
	{
		SetRasterizerState(RS_NoCullSolid);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Sky, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_FLARE();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_FLARE();
	}
};