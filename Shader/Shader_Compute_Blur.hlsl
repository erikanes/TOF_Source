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
groupshared float4 sharedCache[266];
groupshared float4 SharedInput[MAX_GROUPS];

int g_iWidth;
int g_iHeight;

const static float fBlurWeights[13] =
{
	0.002216f, 0.008764f, 0.026995f, 0.064759f, 0.120985f, 0.176033f,
	0.199471f,
	0.176033f, 0.120985f, 0.064759f, 0.026995f, 0.008764f, 0.002216f
};

Texture2D<float4> g_InputTexture;
RWTexture2D<float4> g_OutputTexture;

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
}

[numthreads(1024, 1, 1)]
void CS_DOWNSCALE_4X(uint3 GroupID : SV_GroupID, uint3 DispatchThreadID : SV_DispatchThreadID, uint3 GroupThreadID : SV_GroupThreadID)
{
	// 1920x1080 �ؽ��ĸ� 480x270���� �ٿ� �����ϸ�
	uint2 vCurrentPixel = uint2(DispatchThreadID.x % g_iWidth, DispatchThreadID.x / g_iWidth);

	// y�� 270���� ���� ��쿡�� ����
	if (vCurrentPixel.y < (uint)g_iHeight)
	{
		int3 iFullResolutionPos = int3(vCurrentPixel * 4, 0);
		float4 vDownScaled = float4(0.f, 0.f, 0.f, 0.f);

		// 4x4 �ȼ��� ������ ��� ���Ѵ�
		[unroll]
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
				vDownScaled += g_InputTexture.Load(iFullResolutionPos, int2(j, i));
		}

		// �ϳ��� �ȼ��� ����
		vDownScaled /= 16.f;

		// 1/4 ������ HDR �ؽ��� ����
		g_OutputTexture[vCurrentPixel] = vDownScaled;
	}
}

[numthreads(MAX_GROUPS, 1, 1)]
void CS_BLUR_HORIZONTAL(uint3 GroupID : SV_GroupID, uint GroupIndex : SV_GroupIndex)
{
	int2 vCoord = int2(GroupIndex - KERNEL_HALF + (MAX_GROUPS - KERNEL_HALF * 2) * GroupID.x, GroupID.y);
	vCoord = clamp(vCoord, int2(0, 0), int2(g_iWidth - 1, g_iHeight - 1));
	SharedInput[GroupIndex] = g_InputTexture.Load(int3(vCoord, 0));

	GroupMemoryBarrierWithGroupSync();

	if (GroupIndex >= KERNEL_HALF && GroupIndex < (MAX_GROUPS - KERNEL_HALF) &&
		((GroupID.x * (MAX_GROUPS - 2 * KERNEL_HALF) + GroupIndex - KERNEL_HALF) < (uint)g_iWidth))
	{
		float4 vOut = 0.f;

		[unroll]
		for (int i = -KERNEL_HALF; i <= KERNEL_HALF; ++i)
			vOut += SharedInput[GroupIndex + i] * fBlurWeights[i + KERNEL_HALF];

		g_OutputTexture[vCoord] = vOut;
	}
}

[numthreads(MAX_GROUPS, 1, 1)]
void CS_BLUR_VERTICAL(uint3 GroupID : SV_GroupID, uint GroupIndex : SV_GroupIndex)
{
	int2 vCoord = int2(GroupID.x, GroupIndex - KERNEL_HALF + (MAX_GROUPS - KERNEL_HALF * 2) * GroupID.y);
	vCoord = clamp(vCoord, int2(0, 0), int2(g_iWidth, g_iHeight));
	SharedInput[GroupIndex] = g_InputTexture.Load(int3(vCoord, 0));

	GroupMemoryBarrierWithGroupSync();

	if (GroupIndex >= KERNEL_HALF && GroupIndex < (MAX_GROUPS - KERNEL_HALF) &&
		((GroupIndex - KERNEL_HALF + (MAX_GROUPS - KERNEL_HALF * 2) * GroupID.y) < (uint)g_iHeight))
	{
		float4 vOut = 0.f;

		[unroll]
		for (int i = -KERNEL_HALF; i <= KERNEL_HALF; ++i)
			vOut += SharedInput[GroupIndex + i] * fBlurWeights[i + KERNEL_HALF];

		g_OutputTexture[vCoord] = vOut;
	}
}

technique11 Compute
{
	pass Blur_Horizontal
	{
		VertexShader = NULL;
		GeometryShader = NULL;
		ComputeShader = compile cs_5_0 CS_BLUR_HORIZONTAL();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = NULL;
	}

	pass Blur_Vertical
	{
		VertexShader = NULL;
		GeometryShader = NULL;
		ComputeShader = compile cs_5_0 CS_BLUR_VERTICAL();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = NULL;
	}

	pass DownScale_4X
	{
		GeometryShader = NULL;
		ComputeShader = compile cs_5_0 CS_DOWNSCALE_4X();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = NULL;
	}
};