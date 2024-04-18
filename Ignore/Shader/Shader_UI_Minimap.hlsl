#include "Shader_Defines.hlsli"
#include "Shader_Material_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

float g_fWidth;
float g_fHeight;

float2 g_vSize; // 사각형 사이즈
float2 g_vPosition; // 미니맵의 스크린상 위치

float2 g_vCamDirection;
float2 g_vPlayerPos;
float2 g_vPlayerDir;

float4 g_vRange;

float g_fCameraFov;
float g_fUVSize;

struct VS_IN
{
	float3	vPosition	: POSITION;
	float2	vPsize		: PSIZE;
};

struct VS_OUT
{
	float4	vPosition	: POSITION;
	float2	vPsize		: PSIZE;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matVP);
	Out.vPsize = g_vSize;

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
	float2 vMaskTexCoord	: TEXCOORD1;
	float2 vCenter			: TEXCOORD2;
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
void GS_BACKGROUND(point GS_IN In[1], inout TriangleStream<GS_OUT> Triangles)
{
	GS_OUT Out[4];

	float2 vSize = In[0].vPsize;

	vector vRight = vector(vSize.x * 0.5f, 0.f, 0.f, 0.f);
	vector vUp = vector(0.f, vSize.y * 0.5f, 0.f, 0.f);

	vector vPosition = In[0].vPosition;
	vPosition.x = g_vPosition.x;
	vPosition.y = g_vPosition.y;
	vPosition.z = 0.f;
	vPosition.w = 1.f;

	float2 vPlayerPos = g_vPlayerPos;
	float4 vRange = g_vRange;

	float2 vTexCoord;

	vTexCoord.x = (-vPlayerPos.x - vRange.x) / (vRange.y - vRange.x);
	vTexCoord.y = (vPlayerPos.y - vRange.z) / (vRange.w - vRange.z);

	if (0.f > vTexCoord.x)
		vTexCoord.x += 1.f;

	/*
	0 : 우상
	1 : 좌상
	2 : 좌하
	3 : 우하
	*/

	float fSize = g_fUVSize;

	Out[0].vPosition = GetNDC(vPosition + vRight - vUp);
	Out[0].vTexCoord = float2(vTexCoord.x + fSize, vTexCoord.y - fSize);
	Out[0].vMaskTexCoord = float2(1.f, 0.f);
	Out[0].vCenter = vTexCoord;

	Out[1].vPosition = GetNDC(vPosition - vRight - vUp);
	Out[1].vTexCoord = float2(vTexCoord.x - fSize, vTexCoord.y - fSize);
	Out[1].vMaskTexCoord = float2(0.f, 0.f);
	Out[1].vCenter = vTexCoord;

	Out[2].vPosition = GetNDC(vPosition - vRight + vUp);
	Out[2].vTexCoord = float2(vTexCoord.x - fSize, vTexCoord.y + fSize);
	Out[2].vMaskTexCoord = float2(0.f, 1.f);
	Out[2].vCenter = vTexCoord;

	Out[3].vPosition = GetNDC(vPosition + vRight + vUp);
	Out[3].vTexCoord = float2(vTexCoord.x + fSize, vTexCoord.y + fSize);
	Out[3].vMaskTexCoord = float2(1.f, 1.f);
	Out[3].vCenter = vTexCoord;

	Triangles.Append(Out[0]);
	Triangles.Append(Out[1]);
	Triangles.Append(Out[2]);
	Triangles.RestartStrip();

	Triangles.Append(Out[0]);
	Triangles.Append(Out[2]);
	Triangles.Append(Out[3]);
	Triangles.RestartStrip();
}

[maxvertexcount(6)]
void GS_PLAYER(point GS_IN In[1], inout TriangleStream<GS_OUT> Triangles)
{
	GS_OUT Out[4];

	float2 vSize = In[0].vPsize;

	vector vRight = vector(vSize.x * 0.5f, 0.f, 0.f, 0.f);
	vector vUp = vector(0.f, vSize.y * 0.5f, 0.f, 0.f);

	float2 vAxis = float2(1.f, 0.f);
	float2 vPlayerDir = float2(-g_vPlayerDir.x, g_vPlayerDir.y);

	float fDot = dot(vAxis, vPlayerDir);
	float fRadians = acos(fDot);

	if (0.f < vPlayerDir.y)
		fRadians = 2.f * PI - fRadians;

	fRadians -= PI * 0.5f;

	float2x2 matRot;
	matRot[0] = float2(cos(fRadians), -sin(fRadians));
	matRot[1] = float2(sin(fRadians), cos(fRadians));

	vector vPosition[4];
	vPosition[0] = vRight - vUp;
	vPosition[1] = -vRight - vUp;
	vPosition[2] = -vRight + vUp;
	vPosition[3] = vRight + vUp;

	[unroll]
	for (int i = 0; i < 4; ++i)
		vPosition[i] = vector(mul(vPosition[i].xy, matRot) + g_vPosition, 0.f, 1.f);

	Out[0].vPosition = GetNDC(vPosition[0]);
	Out[0].vTexCoord = float2(1.f, 0.f);
	Out[0].vMaskTexCoord = float2(1.f, 0.f);
	Out[0].vCenter = float2(0.f, 0.f);

	Out[1].vPosition = GetNDC(vPosition[1]);
	Out[1].vTexCoord = float2(0.f, 0.f);
	Out[1].vMaskTexCoord = float2(0.f, 0.f);
	Out[1].vCenter = float2(0.f, 0.f);

	Out[2].vPosition = GetNDC(vPosition[2]);
	Out[2].vTexCoord = float2(0.f, 1.f);
	Out[2].vMaskTexCoord = float2(0.f, 1.f);
	Out[2].vCenter = float2(0.f, 0.f);

	Out[3].vPosition = GetNDC(vPosition[3]);
	Out[3].vTexCoord = float2(1.f, 1.f);
	Out[3].vMaskTexCoord = float2(1.f, 1.f);
	Out[3].vCenter = float2(0.f, 0.f);

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
	float4	vPosition		: SV_POSITION;
	float2	vTexCoord		: TEXCOORD0;
	float2	vMaskTexCoord	: TEXCOORD1;
	float2	vCenter			: TEXCOORD2;
};

struct PS_OUT
{
	float4 vColor : SV_TARGET0;
};

PS_OUT PS_BACKGROUND(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float4 vDiffuse = g_DiffuseTexture.Sample(LinearBorderSampler, In.vTexCoord);
	float fMask = g_MaskMapTexture.Sample(LinearSampler, In.vMaskTexCoord).x;

	vDiffuse.a *= fMask;

	if (0.f >= vDiffuse.a)
		discard;

	float2 vCamDir = float2(-g_vCamDirection.x, g_vCamDirection.y);
	float fHalfFov = g_fCameraFov * 0.5f;
	float2 vToPixel = In.vTexCoord - In.vCenter;
	float fDistance = length(vToPixel);
		
	float fDot = dot(vCamDir, normalize(vToPixel));

	Out.vColor = vDiffuse;
	Out.vColor.a *= 0.5f;

	if (acos(fDot) < fHalfFov)
	{
		float fRate = 1.f - saturate(fDistance / g_fUVSize);
		Out.vColor += Out.vColor * 0.5f * fRate;
	}

	return Out;
}

PS_OUT PS_PLAYER(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float4 vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexCoord);

	if (0.f >= vDiffuse.a)
		discard;

	Out.vColor = vDiffuse;

	return Out;
}

PS_OUT PS_FOV(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	return Out;
}

technique11 UIMinimap
{
	pass Background
	{
		SetRasterizerState(RS_NoCullSolid);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Sky, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_BACKGROUND();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_BACKGROUND();
	}

	pass Player
	{
		SetRasterizerState(RS_NoCullSolid);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Sky, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_PLAYER();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_PLAYER();
	}
}