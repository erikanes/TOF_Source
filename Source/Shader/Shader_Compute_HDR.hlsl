/*
SV_GroupID : ��ü �׷쿡 ���� �ε���
(Dispatch(2,1,1) ȣ�� ��, 0,0,0 / 1,0,0 ����

SV_GroupIndex : ������ SV_GroupThreadID�� 1���� ������ �ٲ�
0 ~ (x * y * z) - 1

SV_GroupThreadID : �������� ������ �׷� ���� ���� �����忡 ���� �ε���
numtreads(3,2,1)�� ������ ��� �Է� ������ ���� ������ (0-2,0-1,0)�� ����

SV_DispatchThreadID : ���յ� ������ �� ������ �׷쿡 ���� �ε���
numtreads(3,3,3)�� �ִ� CS���� Dispatch(2,2,2) ȣ�� ��, �� ������ ���� 0~5�� �ε����� ����
*/

// General
Texture2D<float4>	g_InputTexture;
RWTexture2D<float4> g_OutputTexture;

// HDR
#define MAX_HDR_GROUPS 64
const static float4 LUMEN_FACTOR = float4(0.299f, 0.587f, 0.114f, 0.f);
StructuredBuffer<float>		g_AverageValues1D;
RWStructuredBuffer<float>	g_AverageLumenFinal; // pass output
RWStructuredBuffer<float>	g_PrevAverageLumen;

groupshared float	g_SharedPosition[1024]; // first pass to second pass
groupshared float	g_HDR_Average_Cache[MAX_HDR_GROUPS]; // SharedAvgFinal / second pass

float				g_fAdaptation;

int					g_iWidth;
int					g_iHeight;
int					g_iDomain;
int					g_iGroupSize;
bool				g_bEnableAdaptation;

float2				g_vAverageLuminanceRange;

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{

}

float DownScale4x4(uint2 CurrentPixel, uint GroupThreadID)
{
	// 1920x1080 �ؽ��ĸ� 480x270���� �ٿ� �����ϸ�
	float fAverageLuminance = 0.f;

	// y�� 270���� ���� ��쿡�� ����
	if (CurrentPixel.y < (uint)g_iHeight)
	{
		int3 iFullResolutionPos = int3(CurrentPixel * 4, 0);
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
		g_OutputTexture[CurrentPixel] = vDownScaled;

		// ����� �ȼ��� ��� �ֵ��� ���
		fAverageLuminance = dot(vDownScaled, LUMEN_FACTOR);

		// ���� �޸𸮿� ��� ���
		// GroupThreadID = x�� 1024�̹Ƿ� 0~1023 ���
		g_SharedPosition[GroupThreadID] = fAverageLuminance;
	}

	GroupMemoryBarrierWithGroupSync();

	return fAverageLuminance;
}

float DownScale1024To4(uint DispatchThreadID, uint GroupThreadID, float AverageLumen)
{
	for (uint iGroupSize = 4, iStep1 = 1, iStep2 = 2, iStep3 = 3;
		iGroupSize < 1024;
		iGroupSize *= 4, iStep1 *= 4, iStep2 *= 4, iStep3 *= 4)
	{
		if (0 == GroupThreadID % iGroupSize)
		{
			float fStepAverageLumen = AverageLumen;

			fStepAverageLumen += DispatchThreadID + iStep1 < (uint)g_iDomain ? g_SharedPosition[GroupThreadID + iStep1] : AverageLumen;
			fStepAverageLumen += DispatchThreadID + iStep2 < (uint)g_iDomain ? g_SharedPosition[GroupThreadID + iStep2] : AverageLumen;
			fStepAverageLumen += DispatchThreadID + iStep3 < (uint)g_iDomain ? g_SharedPosition[GroupThreadID + iStep3] : AverageLumen;

			AverageLumen = fStepAverageLumen;
			g_SharedPosition[GroupThreadID] = fStepAverageLumen;
		}

		GroupMemoryBarrierWithGroupSync();
	}

	return AverageLumen;
}

void DownScale4To1(uint DispatchThreadID, uint GroupThreadID, uint GroupID, float AverageLumen)
{
	if (0 == GroupThreadID)
	{
		float fFinalAverageLumen = AverageLumen;

		fFinalAverageLumen += DispatchThreadID.x + 256 < (uint)g_iDomain ? g_SharedPosition[GroupThreadID + 256] : AverageLumen;
		fFinalAverageLumen += DispatchThreadID.x + 512 < (uint)g_iDomain ? g_SharedPosition[GroupThreadID + 512] : AverageLumen;
		fFinalAverageLumen += DispatchThreadID.x + 768 < (uint)g_iDomain ? g_SharedPosition[GroupThreadID + 768] : AverageLumen;

		fFinalAverageLumen /= 1024.f;

		g_AverageLumenFinal[GroupID] = fFinalAverageLumen;
	}
}

[numthreads(1024, 1, 1)]
void CS_HDR_DOWNSCALE_FIRST(uint3 GroupID : SV_GroupID, uint3 DispatchThreadID : SV_DispatchThreadID, uint3 GroupThreadID : SV_GroupThreadID)
{
	uint2 vCurrentPixel = uint2(DispatchThreadID.x % g_iWidth, DispatchThreadID.x / g_iWidth);

	float fAverageLumen = DownScale4x4(vCurrentPixel, GroupThreadID.x);
	fAverageLumen = DownScale1024To4(DispatchThreadID.x, GroupThreadID.x, fAverageLumen);

	DownScale4To1(DispatchThreadID.x, GroupThreadID.x, GroupID.x, fAverageLumen);
}

[numthreads(MAX_HDR_GROUPS, 1, 1)]
void CS_HDR_DOWNSCALE_SECOND(uint3 GroupID : SV_GroupID, uint3 DispatchThreadID : SV_DispatchThreadID, uint3 GroupThreadID : SV_GroupThreadID)
{
	float fAverageLumen = 0.f;

	if (DispatchThreadID.x < (uint)g_iGroupSize)
		fAverageLumen = g_AverageValues1D[DispatchThreadID.x];

	g_HDR_Average_Cache[DispatchThreadID.x] = fAverageLumen;

	GroupMemoryBarrierWithGroupSync();
	
	// 64 to 16 down scale
	if (0 == DispatchThreadID.x % 4)
	{
		float fStepAverageLumen = fAverageLumen;

		fStepAverageLumen += DispatchThreadID.x + 1 < (uint)g_iGroupSize ? g_HDR_Average_Cache[DispatchThreadID.x + 1] : fAverageLumen;
		fStepAverageLumen += DispatchThreadID.x + 2 < (uint)g_iGroupSize ? g_HDR_Average_Cache[DispatchThreadID.x + 2] : fAverageLumen;
		fStepAverageLumen += DispatchThreadID.x + 3 < (uint)g_iGroupSize ? g_HDR_Average_Cache[DispatchThreadID.x + 3] : fAverageLumen;

		fAverageLumen = fStepAverageLumen;

		g_HDR_Average_Cache[DispatchThreadID.x] = fStepAverageLumen;
	}

	GroupMemoryBarrierWithGroupSync();

	if (0 == DispatchThreadID.x % 16)
	{
		float fStepAverageLumen = fAverageLumen;

		fStepAverageLumen += DispatchThreadID.x + 4 < (uint)g_iGroupSize ? g_HDR_Average_Cache[DispatchThreadID.x + 4] : fAverageLumen;
		fStepAverageLumen += DispatchThreadID.x + 8 < (uint)g_iGroupSize ? g_HDR_Average_Cache[DispatchThreadID.x + 8] : fAverageLumen;
		fStepAverageLumen += DispatchThreadID.x + 12 < (uint)g_iGroupSize ? g_HDR_Average_Cache[DispatchThreadID.x + 12] : fAverageLumen;

		fAverageLumen = fStepAverageLumen;
		g_HDR_Average_Cache[DispatchThreadID.x] = fStepAverageLumen;
	}

	GroupMemoryBarrierWithGroupSync();

	// 4 to 1 down scale
	if (0 == DispatchThreadID.x)
	{
		float fFinalLumenValue = fAverageLumen;

		fFinalLumenValue += DispatchThreadID.x + 16 < (uint)g_iGroupSize ? g_HDR_Average_Cache[DispatchThreadID.x + 16] : fAverageLumen;
		fFinalLumenValue += DispatchThreadID.x + 32 < (uint)g_iGroupSize ? g_HDR_Average_Cache[DispatchThreadID.x + 32] : fAverageLumen;
		fFinalLumenValue += DispatchThreadID.x + 48 < (uint)g_iGroupSize ? g_HDR_Average_Cache[DispatchThreadID.x + 48] : fAverageLumen;

		fFinalLumenValue /= 64.f;

		if (g_bEnableAdaptation)
			fFinalLumenValue = lerp(g_PrevAverageLumen[0], fFinalLumenValue, g_fAdaptation);

		g_AverageLumenFinal[0] = clamp(max(fFinalLumenValue, 0.0001f), g_vAverageLuminanceRange.x, g_vAverageLuminanceRange.y);
	}
}

technique11 Compute
{
	pass DownScale_First
	{
		VertexShader = NULL;
		GeometryShader = NULL;
		ComputeShader = compile cs_5_0 CS_HDR_DOWNSCALE_FIRST();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = NULL;
	}

	pass HDownScale_Second
	{
		VertexShader = NULL;
		GeometryShader = NULL;
		ComputeShader = compile cs_5_0 CS_HDR_DOWNSCALE_SECOND();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = NULL;
	}
};