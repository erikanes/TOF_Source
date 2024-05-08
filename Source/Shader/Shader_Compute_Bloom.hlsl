/*
SV_GroupID : 전체 그룹에 대한 인덱스
(Dispatch(2,1,1) 호출 시, 0,0,0 / 1,0,0 생성

SV_GroupIndex : 다차원 SV_GroupThreadID를 1차원 값으로 바꿈
0 ~ (x * y * z) - 1

SV_DispatchThreadID : 결합된 스레드 및 스레드 그룹에 대한 인덱스
numtreads(3,3,3)이 있는 CS에서 Dispatch(2,2,2) 호출 시, 각 차원에 대해 0~5의 인덱스를 가짐
*/

#include "Shader_Compute_Defines.hlsli"

#define MAX_GROUPS 256
const static float3 LUMEN_FACTOR = float3(0.299f, 0.587f, 0.114f);

Texture2D<float4>			g_InputTexture; // input. HDR을 거친 뒤의 백버퍼를 다운 스케일 한것
Texture2D<float4>			g_EmissiveTexture;
StructuredBuffer<float>		g_AverageLuminance; // input. HDR을 통해 생성된 값
Texture2D<float>			g_InputDepthTexture;

RWTexture2D<float4>			g_OutputTexture; // output
RWTexture2D<float>			g_OutputDepthTexture; // output depth

float g_fBloomThreshold; // 밝기 기준점. 이 값 이하는 버려짐
float g_fDepthThreshold; // fog의 활성화 여부에 따라 사용될 임계값

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
	// 1/4로 뎁스 타겟을 다운 스케일링
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