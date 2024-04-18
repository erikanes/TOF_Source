#include "Shader_Compute_Defines.hlsli"

/*
SV_GroupID : 전체 그룹에 대한 인덱스
(Dispatch(2,1,1) 호출 시, 0,0,0 / 1,0,0 생성

SV_GroupIndex : 다차원 SV_GroupThreadID를 1차원 값으로 바꿈
0 ~ (x * y * z) - 1

SV_DispatchThreadID : 결합된 스레드 및 스레드 그룹에 대한 인덱스
numtreads(3,3,3)이 있는 CS에서 Dispatch(2,2,2) 호출 시, 각 차원에 대해 0~5의 인덱스를 가짐
*/

#define MAX_GROUPS 32

Texture2D<float4> g_DepthTexture;
Texture2D<float4> g_NormalTexture;

Texture2D<float4> g_DiffuseTexture;
Texture2D<float4> g_SSAOTexture;

RWTexture2D<float4> g_OutputTexture;

float g_fResX;
float g_fResY;
float g_fLength;
float g_fCameraFar;
float g_fThreshold;

const static float3 vRandom[16] =
{
	float3(0.2024537f,		0.841204f,		-0.9060141f),
	float3(-0.2200423f,		0.6282339f,		-0.8275437f),
	float3(0.3677659f,		0.1086345f,		-0.4466777f),
	float3(0.8775856f,		0.4617546f,		-0.6427765f),
	float3(0.7867433f,		-0.141479f,		-0.1567597f),
	float3(0.4839456f,		-0.8253108f,	-0.1563844f),
	float3(0.4401554f,		-0.4228428f,	-0.3300118f),
	float3(0.0019193f,		-0.8048455f,	0.0726584f),
	float3(-0.7578573f,		-0.5583301f,	0.2347527f),
	float3(-0.4540417f,		-0.252365f,		0.0694318f),
	float3(-0.0483353f,		-0.2527294f,	0.5924745f),
	float3(-0.4192392f,		0.2084218f,		-0.3672943f),
	float3(-0.8433938f,		0.1451271f,		0.2202872f),
	float3(-0.4037157f,		-0.8263387f,	0.4698132f),
	float3(-0.6657394f,		0.6298575f,		0.6342437f),
	float3(0.0001783f,		0.2834622f,		0.8343929f)
};

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
}

float3 RandomNormal(float2 UV)
{
	float fNoiseX = frac(sin(dot(UV, float2(15.8989f, 76.132f) * 1.f)) * 46336.23745f);
	float fNoiseY = frac(sin(dot(UV, float2(11.9899f, 62.223f) * 1.f)) * 34748.34744f);
	float fNoiseZ = frac(sin(dot(UV, float2(13.3238f, 63.122f) * 1.f)) * 59998.47362f);

	return normalize(float3(fNoiseX, fNoiseY, fNoiseZ));
}

[numthreads(MAX_GROUPS, MAX_GROUPS, 1)]
void CS_SSAO(int3 DispatchThreadID : SV_DispatchThreadID)
{
	float2 vSize = float2(g_fResX, g_fResY);
	float2 vIndex = min(DispatchThreadID.xy, vSize);
	float2 vTexCoord = vIndex / vSize;
	int2 vCurrentPixel = vIndex;

	float fColor = 0.f;

	// 노말맵 추출
	float4 vNormal = g_NormalTexture.Load(int3(vCurrentPixel, 0));
	vNormal = float4(normalize(vNormal.xyz * 2.f - 1.f), 0.f);

	// 깊이맵 추출
	float4 vOriginDepth = g_DepthTexture.Load(int3(vCurrentPixel, 0));

	float fViewZ = vOriginDepth.r; // 0~1정규화
	float fDepth = vOriginDepth.g * g_fCameraFar; // 뷰 공간에서의 z

	for (int i = 0; i < 16; ++i)
	{
		/*
		depth 텍스쳐
		r : 투영공간상의 z(0~1)
		g : 뷰 공간상의 z값을 far로 나눈 값
		*/

		half3 vRay = reflect(RandomNormal(vTexCoord), vRandom[i]);
		half3 vReflect = normalize(reflect(vRay, vNormal.xyz)) * g_fLength;
		vReflect.x *= -1.f;
		half2 vRandomUV = abs(float2(fmod(vReflect.x, 1.f), fmod(vReflect.y, 1.f))) * g_fLength;
		int2 vRandomPixel = int2(vRandomUV.x * g_fResX, vRandomUV.y * g_fResY);
		float4 vRandomDepth = g_DepthTexture.Load(int3(vRandomPixel, 0));
		float fOccNormal = vRandomDepth.g * g_fCameraFar;

		if (fOccNormal <= fDepth + 0.0003f)
			fColor += 1.f;
	}

	fColor = 1.f - abs(fColor / 16.f);

	if (g_fThreshold <= fColor)
		fColor = 1.f;

	g_OutputTexture[vCurrentPixel] = float4(fColor, fColor, fColor, 1.f);
}

[numthreads(MAX_GROUPS, MAX_GROUPS, 1)]
void CS_BIND_SSAO_TO_DIFFUSE(int3 DispatchThreadID : SV_DispatchThreadID)
{
	int2 vSize = int2(g_fResX, g_fResY);
	int2 vCurrentPixel = min(DispatchThreadID.xy, vSize);

	float4 vDiffuse = g_DiffuseTexture.Load(int3(vCurrentPixel, 0));
	float4 vSSAO = g_SSAOTexture.Load(int3(vCurrentPixel, 0));

	g_OutputTexture[vCurrentPixel] = vDiffuse * vSSAO;
}

technique11 SSAO
{
	pass SSAO
	{
		VertexShader = NULL;
		GeometryShader = NULL;
		ComputeShader = compile cs_5_0 CS_SSAO();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = NULL;
	}

	pass BIND
	{
		VertexShader = NULL;
		GeometryShader = NULL;
		ComputeShader = compile cs_5_0 CS_BIND_SSAO_TO_DIFFUSE();
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = NULL;
	}
};