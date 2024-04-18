/*
SV_GroupID : ��ü �׷쿡 ���� �ε���
(Dispatch(2,1,1) ȣ�� ��, 0,0,0 / 1,0,0 ����

SV_GroupIndex : ������ SV_GroupThreadID�� 1���� ������ �ٲ�
0 ~ (x * y * z) - 1

SV_DispatchThreadID : ���յ� ������ �� ������ �׷쿡 ���� �ε���
numtreads(3,3,3)�� �ִ� CS���� Dispatch(2,2,2) ȣ�� ��, �� ������ ���� 0~5�� �ε����� ����
*/

#include "Shader_Compute_Defines.hlsli"

#define MAX_GROUPS 256
const static float3 LUMEN_FACTOR = float3(0.299f, 0.587f, 0.114f);

Texture2D<float4>			g_InputTexture; // input. HDR�� ��ģ ���� ����۸� �ٿ� ������ �Ѱ�
Texture2D<float4>			g_EmissiveTexture;
StructuredBuffer<float>		g_AverageLuminance; // input. HDR�� ���� ������ ��
Texture2D<float>			g_InputDepthTexture;

RWTexture2D<float4>			g_OutputTexture; // output
RWTexture2D<float>			g_OutputDepthTexture; // output depth

float g_fBloomThreshold; // ��� ������. �� �� ���ϴ� ������
float g_fDepthThreshold; // fog�� Ȱ��ȭ ���ο� ���� ���� �Ӱ谪

// Glow
float g_fGlowAverageLuminance;
float g_fGlowThreshold;

uint g_iResWidth;
uint g_iResHeight;

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
}

[numthreads(1024, 1, 1)]
void CS_BLOOM(uint3 GroupID : SV_GroupID, uint3 DispatchThreadID : SV_DispatchThreadID, uint3 GroupThreadID : SV_GroupThreadID)
{
	uint2 vCurrentPixel = uint2(DispatchThreadID.x % g_iResWidth, DispatchThreadID.x / g_iResWidth);

	if (vCurrentPixel.y < g_iResHeight)
	{
		float fDepth = g_InputDepthTexture.Load(uint3(vCurrentPixel, 0));
		float4 vColor = g_InputTexture.Load(uint3(vCurrentPixel, 0));
		float fLuminance = dot(vColor.rgb, LUMEN_FACTOR);
		float fAverageLuminance = g_AverageLuminance[0];

		float fColorScale = saturate(fLuminance - fAverageLuminance * g_fBloomThreshold);

		fColorScale = saturate(g_fDepthThreshold) <= fDepth ? 0.f : fColorScale;

		g_OutputTexture[vCurrentPixel] = vColor * fColorScale;
	}
}

[numthreads(1024, 1, 1)]
void CS_GLOW(uint3 GroupID : SV_GroupID, uint3 DispatchThreadID : SV_DispatchThreadID, uint3 GroupThreadID : SV_GroupThreadID)
{
	uint2 vCurrentPixel = uint2(DispatchThreadID.x % g_iResWidth, DispatchThreadID.y);

	if (vCurrentPixel.y < g_iResHeight)
	{
		float4 vColor = g_InputTexture.Load(uint3(vCurrentPixel, 0));
		float4 vEmissive = g_EmissiveTexture.Load(uint3(vCurrentPixel, 0));

		vColor = dot(vColor.rgb, LUMEN_FACTOR) > dot(vEmissive.rgb, LUMEN_FACTOR) ? vColor : vEmissive;
		float fLScale = dot(vColor.rgb, LUMEN_FACTOR);

		fLScale *= 1.f / g_fGlowAverageLuminance; // grayscale
		fLScale = (fLScale + fLScale * fLScale) / (1.f + fLScale); // luminance white sqrt

		vColor *= fLScale;

		float fLuminance = dot(vColor.rgb, LUMEN_FACTOR);
		float fColorScale = saturate(fLuminance - g_fGlowAverageLuminance * g_fGlowThreshold);

		g_OutputTexture[vCurrentPixel] = float4(vColor.rgb * fColorScale, vColor.a);
	}
}

[numthreads(1024, 1, 1)]
void CS_DOWNSCALE_4X(uint3 GroupID : SV_GroupID, uint3 DispatchThreadID : SV_DispatchThreadID, uint3 GroupThreadID : SV_GroupThreadID)
{
	// 1/4�� ���� Ÿ���� �ٿ� �����ϸ�
	uint2 vCurrentPixel = uint2(DispatchThreadID.x % g_iResWidth, DispatchThreadID.x / g_iResWidth);

	if (vCurrentPixel.y < (uint)g_iResHeight)
	{
		int3 iFullResolutionPos = int3(vCurrentPixel * 4, 0);
		float fMinDepth = 1.f;

		[unroll]
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				float fCurDepth = g_InputTexture.Load(iFullResolutionPos, int2(j, i)).g;
				fMinDepth = min(fMinDepth, fCurDepth);
			}
		}

		g_OutputDepthTexture[vCurrentPixel] = fMinDepth;
	}
}

technique11 Compute
{
	pass Bloom
	{
		VertexShader = NULL;
		GeometryShader = NULL;
		ComputeShader = compile cs_5_0 CS_BLOOM();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = NULL;
	}

	pass Glow
	{
		VertexShader = NULL;
		GeometryShader = NULL;
		ComputeShader = compile cs_5_0 CS_GLOW();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = NULL;
	}

	pass DepthDownScale
	{
		VertexShader = NULL;
		GeometryShader = NULL;
		ComputeShader = compile cs_5_0 CS_DOWNSCALE_4X();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = NULL;
	}
};