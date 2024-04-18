/*
SV_GroupID : ��ü �׷쿡 ���� �ε���
(Dispatch(2,1,1) ȣ�� ��, 0,0,0 / 1,0,0 ����

SV_GroupIndex : ������ SV_GroupThreadID�� 1���� ������ �ٲ�. 0 ~ (x * y * z)-1

SV_DispatchThreadID : ���յ� ������ �� ������ �׷쿡 ���� �ε���
numtreads(3,3,3)�� �ִ� CS���� Dispatch(2,2,2) ȣ�� ��, �� ������ ���� 0~5�� �ε����� ����
*/

#include "Shader_Compute_Defines.hlsli"

#define MAX_GROUPS 128
#define KERNEL_HALF 6

int g_iWidth;
int g_iHeight;

Texture2D<float4> g_InputDepthTexture;
RWTexture2D<float> g_OutputDepthTexture;

Texture2D<float> g_InputTexture;
RWTexture2D<float> g_OutputOcclusion;

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
}

[numthreads(1024, 1, 1)]
void CS_OCCLUSION(uint DispatchThreadID : SV_DispatchThreadID)
{
	uint3 vCurrentPixel = uint3(DispatchThreadID.x % g_iWidth, DispatchThreadID.x / g_iWidth, 0);

	if (vCurrentPixel.y < (uint)g_iHeight)
	{
		float fCurDepth = g_InputTexture.Load(vCurrentPixel);

		g_OutputOcclusion[vCurrentPixel.xy] = fCurDepth > 0.99f ? 1.f : 0.f;
	}
}

[numthreads(1024, 1, 1)]
void CS_DOWNSCALE_2X(uint3 GroupID : SV_GroupID, uint3 DispatchThreadID : SV_DispatchThreadID, uint3 GroupThreadID : SV_GroupThreadID)
{
	// �������� �ٿ� �����ϸ�
	uint2 vCurrentPixel = uint2(DispatchThreadID.x % g_iWidth, DispatchThreadID.x / g_iWidth);

	if (vCurrentPixel.y < (uint)g_iHeight)
	{
		int3 iFullResolutionPos = int3(vCurrentPixel * 2, 0);
		float fMinDepth = 1.f;

		[unroll]
		for (int i = 0; i < 2; ++i)
		{
			for (int j = 0; j < 2; ++j)
			{
				float fCurDepth = g_InputDepthTexture.Load(iFullResolutionPos, int2(j, i)).g;
				fMinDepth = min(fMinDepth, fCurDepth);
			}
		}

		g_OutputDepthTexture[vCurrentPixel] = fMinDepth;
	}
}

technique11 SSLR
{
	pass Occlusion
	{
		GeometryShader = NULL;
		ComputeShader = compile cs_5_0 CS_OCCLUSION();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = NULL;
	}

	pass DownScale_2X
	{
		GeometryShader = NULL;
		ComputeShader = compile cs_5_0 CS_DOWNSCALE_2X();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = NULL;
	}
};