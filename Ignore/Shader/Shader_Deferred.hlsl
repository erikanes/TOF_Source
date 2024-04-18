#include "Shader_Defines.hlsli"

// HDR
#define LUMEN_FACTOR float3(0.299f, 0.587f, 0.114f)

texture2D g_HDRTexture;

float g_fMiddleGrey;
float g_fLumWhiteSqr;
StructuredBuffer<float> g_AverageLumen;

// Generic
float g_fAlphaDiscard;
float g_fFadeAlpha;
float g_fDepthThreshold; // fog의 활성화 여부에 따라 dof에서 사용될 임계값

// Bloom
texture2D g_BloomTexture;
float g_fBloomScale;

// Blur
texture2D g_GlowTexture;
texture2D g_BlurTexture;

// Glow
float g_fGlowScale;


// Gamma Correction
bool g_bUseGammaCorrection;
float g_fGamma = 2.2f;

// Post Processing
texture2D g_SSAOTexture;

// DOF
float g_fFocus;
float g_fFocusRange;

// VFX
texture2D g_VFXDepthTexture;

// Fog
float g_fFogStart;
float g_fFogEnd;
float4 g_vFogColor;

float g_fFogHeightStart;
float g_fFogHeightEnd;
bool g_bUseHeightFog;

// Lensflare
texture2D g_LensflareTexture;

// SSLR
texture2D<float> g_SSLRTexture;
float g_fInitDecay;
float g_fDistDecay;
float g_fMaxDeltaLength;
float4 g_vRayColor;

RWTexture2D<float4> g_RWTexture;

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ProjMatrixInv, g_ViewMatrixInv;
matrix g_LightViewMatrix, g_LightProjMatrix;

texture2D g_Texture;
texture2D g_MotionBlurTexture;

vector g_vLightDirection;
vector g_vLightPosition;
float g_fLightRange;

vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;

vector g_vCamPosition;

vector g_vMtrlAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

float g_fBlurZDisable = false;
float g_fCameraFar;
float g_fCameraNear;

// Shadow
float g_fShadowBias;
float g_fWidth;
float g_fHeight;

texture2D g_DiffuseTexture; // diffuse target
texture2D g_NormalTexture; // normal target
texture2D g_DepthTexture; // depth target
texture2D g_CharacterDepthTexture; // character depth target
texture2D g_ShadeTexture; // shade target
texture2D g_SpecularTexture; // specular target
texture2D g_EmissiveTexture; // emissive target
texture2D<float> g_ShadowDepthTexture; // shadow depth target
texture2D g_BackBufferTexture; // back buffer target
texture2D g_VFXTexture; // vfx

texture2D g_DistortionTexture; // distortion target

float g_fLightSmooth = 0.1f;

static const float fWeights[13] =
{
	0.0561f, 0.1353f, 0.278f, 0.4868f, 0.7261f, 0.9231f,
	1.f,
	0.9231f, 0.7261f, 0.4868f, 0.278f, 0.1353f, 0.0561f
};

static const float fTotal = 6.2108f;

float3 ToneMapping(float3 vHDRColor)
{
	// 현재 픽셀에 대한 휘도 스케일 계산
	float fLScale = dot(vHDRColor, LUMEN_FACTOR);
	float fLumen = clamp(g_AverageLumen[0], 0.65f, 0.75f);

	fLScale *= g_fMiddleGrey / fLumen;
	fLScale = (fLScale + fLScale * fLScale / g_fLumWhiteSqr) / (1.f + fLScale);

	return vHDRColor * fLScale;
}

static const float e = 2.71828f;

float W_f(float x, float e0, float e1)
{
	if (x <= e0)
		return 0.f;

	if (x >= e1)
		return 1.f;

	float a = (x - e0) / (e1 - e0);

	return a * a * (3.f - 2.f * a);
}

float H_f(float x, float e0, float e1)
{
	if (x <= e0)
		return 0.f;

	if (x >= e1)
		return 1.f;

	return (x - e0) / (e1 - e0);
}

float ToneMapping_Granturismo(float x)
{
	float P = 1.f;
	float a = 1.f;
	float m = 0.22f;
	float l = g_AverageLumen[0];
	float c = 1.33f;
	float b = 0.f;
	float l0 = (P - m) * l / a;
	float L0 = m - m / a;
	float L1 = m + (1.f - m) / a;
	float L_x = m + a * (x - m);
	float T_x = m * pow(x / m, c) + b;
	float S0 = m + l0;
	float S1 = m + a * 10;
	float C2 = a * P / (P - S1);
	float S_x = P - (P - S1) * pow(e, -(C2 * (x - S0) / P));
	float w0_x = 1.f - W_f(x, 0.f, m);
	float w2_x = H_f(x, m + l0, m + l0);
	float w1_x = 1.f - w0_x - w2_x;
	float f_x = T_x * w0_x + L_x * w1_x + S_x * w2_x;
	
	return f_x;
}

float4 PixelToWorldPosition(in float2 vTexCoord, in float fZ, in float fViewZ)
{
	float4 vPosition = float4(0.f, 0.f, 0.f, 0.f);

	vPosition.x = vTexCoord.x * 2.f - 1.f;
	vPosition.x = vTexCoord.y * -2.f + 1.f;
	vPosition.z = fZ;
	vPosition.w = 1.f;

	vPosition *= fViewZ;

	vPosition = mul(vPosition, g_ProjMatrixInv);
	vPosition = mul(vPosition, g_ViewMatrixInv);

	return vPosition;
}


struct VS_IN
{
	float3 vPosition : POSITION;
	float2 vTexCoord : TEXCOORD0;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
	float2 vTexCoord : TEXCOORD0;
};

struct VS_BLUR_OUT
{
	float4 vPosition : SV_POSITION;
	float2 vTexCoord : TEXCOORD0;

	float2 vTexCoord1 : TEXCOORD1;
	float2 vTexCoord2 : TEXCOORD2;
	float2 vTexCoord3 : TEXCOORD3;
	float2 vTexCoord4 : TEXCOORD4;
	float2 vTexCoord5 : TEXCOORD5;
	float2 vTexCoord6 : TEXCOORD6;
	float2 vTexCoord7 : TEXCOORD7;
	float2 vTexCoord8 : TEXCOORD8;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT) 0;

	matrix matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexCoord = In.vTexCoord;

	return Out;
}

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexCoord : TEXCOORD0;
};

struct PS_OUT
{
	float4 vColor : SV_TARGET0;
};

struct PS_OUT_LIGHT
{
	float4 vShade : SV_TARGET0;
	float4 vSpecular : SV_TARGET1;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT) 0;

	Out.vColor = g_Texture.Sample(PointSampler, In.vTexCoord) * g_fFadeAlpha;

	return Out;
}

PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
	PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;

	float4 vNormal = g_NormalTexture.Sample(PointSampler, In.vTexCoord);
	vNormal = float4(vNormal.xyz * 2.f - 1.f, 0.f);

	float4 vDepth = g_DepthTexture.Sample(PointSampler, In.vTexCoord);
	float fViewZ = vDepth.y * g_fCameraFar;

	float fNdotL = saturate(dot(normalize(-g_vLightDirection), vNormal));
	fNdotL = max(fNdotL, 0.5f);

	Out.vShade = g_vLightDiffuse * fNdotL + g_vLightAmbient * g_vMtrlAmbient;

	vector vPosition;

	vPosition.x = In.vTexCoord.x * 2.f - 1.f;
	vPosition.y = In.vTexCoord.y * -2.f + 1.f;
	vPosition.z = vDepth.x;
	vPosition.w = 1.f;

	vPosition = vPosition * fViewZ;
	vPosition = mul(vPosition, g_ProjMatrixInv);

	vPosition = mul(vPosition, g_ViewMatrixInv);

	vector vLook = vPosition - g_vCamPosition;

	vector vReflect = normalize(reflect(normalize(g_vLightDirection), vNormal));

	Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) *
		pow(max(dot(normalize(vLook) * -1.f, vReflect), 0.f), 30.f);

	return Out;
}

PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
	PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;

	vector vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexCoord);
	float4 vNormal = float4(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexCoord);
	float fViewZ = vDepthDesc.y * g_fCameraFar;

	vector vPosition;

	vPosition.x = In.vTexCoord.x * 2.f - 1.f;
	vPosition.y = In.vTexCoord.y * -2.f + 1.f;
	vPosition.z = vDepthDesc.x;
	vPosition.w = 1.f;

	vPosition = vPosition * fViewZ;
	vPosition = mul(vPosition, g_ProjMatrixInv);

	vPosition = mul(vPosition, g_ViewMatrixInv);

	vector vLightDir = vPosition - g_vLightPosition;

	float fDistance = length(vLightDir);

	// 0.0 ~ 1.f
	float fAtt = max((g_fLightRange - fDistance) / g_fLightRange, 0.f);

	Out.vShade = (g_vLightDiffuse * saturate(max(dot(normalize(vLightDir) * -1.f, vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient))) * fAtt;

	vector vLook = vPosition - g_vCamPosition;

	vector vReflect = normalize(reflect(normalize(vLightDir), vNormal));

	Out.vSpecular = ((g_vLightSpecular * g_vMtrlSpecular) *
		pow(max(dot(normalize(vLook) * -1.f, vReflect), 0.f), 30.f)) * fAtt;

	return Out;
}

PS_OUT PS_MAIN_BLEND(PS_IN In)
{
	PS_OUT Out = (PS_OUT) 0;

	vector vDiffuse = g_DiffuseTexture.Sample(PointSampler, In.vTexCoord);

	if (0.f == vDiffuse.a)
		discard;

	vector vSpecular = g_SpecularTexture.Sample(PointSampler, In.vTexCoord);
	vector vShade = g_ShadeTexture.Sample(PointSampler, In.vTexCoord);
	vector vEmissive = vector(g_EmissiveTexture.Sample(PointSampler, In.vTexCoord).rgb, 0.f);
	vector vDepth = g_DepthTexture.Sample(PointSampler, In.vTexCoord);

	if (1.f == vDepth.b)
		Out.vColor = vDiffuse + vEmissive;
	else if (0.5f == vDepth.b)
		Out.vColor = vDiffuse * vShade + vEmissive;
	else
		Out.vColor = vDiffuse * vShade + vSpecular + vEmissive;

	float fViewZ = vDepth.g * g_fCameraFar;
	vector vPosition;

	vPosition.x = In.vTexCoord.x * 2.f - 1.f;
	vPosition.y = In.vTexCoord.y * -2.f + 1.f;
	vPosition.z = vDepth.x;
	vPosition.w = 1.f;
	vPosition *= fViewZ;

	vPosition = mul(vPosition, g_ProjMatrixInv);
	vPosition = mul(vPosition, g_ViewMatrixInv);

	float3 vLightToPixel = normalize(vPosition.xyz - g_vLightPosition.xyz);

	float fBias = clamp(1.f - dot(vLightToPixel, g_vLightDirection.xyz), 0.f, 0.0002f) + g_fShadowBias;

	vPosition = mul(vPosition, g_LightViewMatrix);

	float fDepth = (vPosition.z - g_fCameraNear) / g_fCameraFar;

	vector vUVPosition = mul(vPosition, g_LightProjMatrix);
	float2 vUV;
	vUVPosition /= vUVPosition.w;

	float fWidth = g_fWidth;
	float fHeight = g_fHeight;

	vUV.x = vUVPosition.x * 0.5f + 0.5f;
	vUV.y = vUVPosition.y * -0.5f + 0.5f;

	float2 vOffset = float2(1.f / fWidth, 1.f / fHeight);

	float2 vOffsets[9] =
	{
		float2(-vOffset.x, -vOffset.y), float2(0.f, -vOffset.y),	float2(vOffset.x, -vOffset.y),
		float2(-vOffset.x, 0.f),		float2(0.f, 0.f),			float2(vOffset.x, 0.f),
		float2(-vOffset.x, vOffset.y),	float2(0.f, vOffset.y),		float2(vOffset.x, vOffset.y)
	};

	float fPCF = 0.f;

	for (int i = 0; i < 9; ++i)
        fPCF += g_ShadowDepthTexture.SampleCmpLevelZero(ComparisonSampler, vUV + vOffsets[i], fDepth - g_fShadowBias);

	fPCF /= 9.f;

	Out.vColor.rgb *= clamp(fPCF, 0.8f, 1.f);
	Out.vColor = saturate(Out.vColor);

	return Out;
}

PS_OUT PS_MAIN_DISTORTION(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vDistortion = g_DistortionTexture.Sample(PointSampler, In.vTexCoord);
    float2 vNewUV = In.vTexCoord + vDistortion.xy * 5.f;
	vector vBackBuffer = g_BackBufferTexture.Sample(PointSampler, vNewUV);

	Out.vColor = vBackBuffer;

	return Out;
}

PS_OUT PS_BLUR_VERTICAL(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vColor = vector(0.f, 0.f, 0.f, 0.f);
	vector vDepth = g_DepthTexture.Sample(PointSampler, In.vTexCoord);

	float fPow = 0.002f <= vDepth.x ? 0.f : 1.f;

	float fV = 1.f / 900.f;

	[unroll]
	for (int i = -6; i < 6; ++i)
	{
		float2 uv = In.vTexCoord + float2(0.f, fV * (float)i) * fPow;
		vColor += g_BackBufferTexture.Sample(LinearClampSampler, uv) * fWeights[6 + i];
	}

	vColor /= fTotal;
	Out.vColor = vColor;

	return Out;
}

PS_OUT PS_BLUR_HORIZONTAL(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vColor = vector(0.f, 0.f, 0.f, 0.f);
	vector vDepth = g_DepthTexture.Sample(PointSampler, In.vTexCoord);

	float fPow = 0.002f <= vDepth.x ? 0.f : 1.f;

	float fU = 1.f / 1600.f;

	[unroll]
	for (int i = -6; i < 6; ++i)
	{
		float2 uv = In.vTexCoord + float2(fU * (float)i, 0.f) * fPow;
		vColor += g_BackBufferTexture.Sample(LinearClampSampler, uv) * fWeights[6 + i];
	}

	vColor /= fTotal;
	Out.vColor = vColor;

	return Out;
}

PS_OUT PS_MAIN_POSTPROCESSING(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float4 vColor = g_HDRTexture.Sample(PointSampler, In.vTexCoord);
    float4 vVFX = g_VFXTexture.Sample(PointSampler, In.vTexCoord);
	float4 vLensflare = g_LensflareTexture.Sample(PointSampler, In.vTexCoord);
	float vSSLR = g_SSLRTexture.Sample(LinearSampler, In.vTexCoord);
	
    float4 vVFXDepth = g_VFXDepthTexture.Sample(PointSampler, In.vTexCoord);
	float3 vNoiseColor = float3(0.2f, 0.2f, 0.2f) * vSSLR;

	if (1.f <= vVFXDepth.z)
        vColor.rgb += vVFX.rgb;
	
	else
        vColor.rgb = vVFX.rgb;

	vLensflare.rgb *= vLensflare.a;

	vColor.rgb += vLensflare.rgb;
	vColor.rgb += vNoiseColor;

	Out.vColor = saturate(vColor);

	return Out;
}

PS_OUT PS_HDR(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float3 vColor = g_Texture.Sample(PointSampler, In.vTexCoord).rgb;
	vColor += g_BloomTexture.Sample(LinearSampler, In.vTexCoord).rgb * g_fBloomScale;

	vColor = ToneMapping(vColor);

	Out.vColor = float4(vColor, 1.f);
	
	return Out;
}

PS_OUT PS_SSAO(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float3 vColor = g_DiffuseTexture.Sample(PointSampler, In.vTexCoord).rgb;
	vColor *= g_SSAOTexture.Sample(PointSampler, In.vTexCoord).rgb;

	Out.vColor = float4(vColor, 1.f);

	return Out;
}

float g_fEdgeHighlightThickness = 0.0005f;
float4 g_vEdgeHighlightColor = float4(1.f, 1.f, 1.f, 1.f);

static const float2 g_vSobelSamplePoints[9] =
{
	float2(-1.f, 1.f), float2(0.f, 1.f), float2(1.f, 1.f),
	float2(-1.f, 0.f), float2(0.f, 0.f), float2(1.f, 0.f),
	float2(-1.f, -1.f), float2(0.f, -1.f), float2(1.f, 1.f)
};

static const float g_fSobelXMatrix[9] =
{
	1.f, 0.f, -1.f,
	2.f, 0.f, -2.f,
	1.f, 0.f, -1.f,
};

static const float g_fSobelYMatrix[9] =
{
	1.f, 2.f, 1.f,
	0.f, 0.f, 0.f,
	-1.f, -2.f, -1.f,
};

PS_OUT PS_OUTLINE_SOBEL(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float fDepth = g_DepthTexture.Sample(PointClampSampler, In.vTexCoord).g;
	float fCharacterDepth = g_CharacterDepthTexture.Sample(PointClampSampler, In.vTexCoord).r;

	if (fDepth < fCharacterDepth)
		discard;

	float2 vSobel = 0.f;

	for (int i = 0; i < 9; ++i)
	{
		float2 vCoord = In.vTexCoord + g_vSobelSamplePoints[i] * g_fEdgeHighlightThickness;
		float fDepth = g_CharacterDepthTexture.Sample(PointClampSampler, vCoord).r;
		vSobel += fDepth * float2(g_fSobelXMatrix[i], g_fSobelYMatrix[i]);
	}

	float fLength = length(vSobel);

	Out.vColor.rgb = g_vEdgeHighlightColor.rgb * fLength;

	if (0.5f >= fLength)
		discard;

	return Out;
}

PS_OUT PS_BLOOM(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float4 vColor = g_Texture.Sample(PointSampler, In.vTexCoord);
	vColor.rgb += g_BloomTexture.Sample(PointSampler, In.vTexCoord).rgb * g_fBloomScale;

	Out.vColor = vColor;
	Out.vColor.a = 1.f;

	return Out;
}

PS_OUT PS_MOTIONBLUR(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float4 vBackBuffer = g_Texture.Sample(PointSampler, In.vTexCoord);
	float4 vBeforeBackBuffer = g_Texture.Sample(PointSampler, In.vTexCoord);

	Out.vColor = (vBackBuffer + vBeforeBackBuffer) / 2.f;

	Out.vColor.a = 1.f;

	return Out;
}

PS_OUT PS_ALPHA_DISCARD(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float4 vColor = g_Texture.Sample(PointSampler, In.vTexCoord);

	if (g_fAlphaDiscard >= vColor.a)
		discard;

	Out.vColor = vColor;

	return Out;
}

PS_OUT PS_DEPTH_OF_FIELD(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float4 vSharp = g_Texture.Sample(PointSampler, In.vTexCoord);
	float4 vBlur = g_BlurTexture.Sample(LinearSampler, In.vTexCoord);

	float4 vDepth = g_DepthTexture.Sample(PointSampler, In.vTexCoord);
	float4 vVFXDepth = g_VFXDepthTexture.Sample(PointSampler, In.vTexCoord);
	float fMin = min(vDepth.g, vVFXDepth.r);
	fMin = saturate(g_fDepthThreshold) <= fMin ? 0.f : fMin;

	float fRate = saturate(g_fFocusRange * abs(g_fFocus - fMin));
	float4 vColor = lerp(vSharp, vBlur, fRate);

	Out.vColor = vColor;
	Out.vColor.a = 1.f;

	return Out;
}

PS_OUT PS_FOG(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float4 vDepth = g_DepthTexture.Sample(PointSampler, In.vTexCoord);

	float fViewZ = vDepth.g * g_fCameraFar;
	float4 vBackBuffer = g_Texture.Sample(PointSampler, In.vTexCoord);
	float fEZ = g_fFogEnd - fViewZ;
	float fES = g_fFogEnd - g_fFogStart;
	float fFogFactor = saturate(fEZ / fES);

	float4 vColor = fFogFactor * vBackBuffer + (1.f - fFogFactor) * g_vFogColor;

	if (fViewZ > g_fFogEnd)
		vColor = g_vFogColor;

	Out.vColor = vColor;

	return Out;
}

PS_OUT PS_MAIN_LINEAR(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexCoord) * g_fFadeAlpha;

	return Out;
}

static const int NUM_STEPS = 64;
static const float NUM_DELTA = 1.f / 63.f;

PS_OUT PS_SSLR(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float2 vToSun = g_vLightPosition.xy - In.vTexCoord;
	float fLengthToSun = length(vToSun);
	vToSun /= fLengthToSun;

	float fDeltaLength = min(g_fMaxDeltaLength, fLengthToSun * NUM_DELTA);
	float2 vRayDelta = vToSun * fDeltaLength;

	float fStepDecay = g_fDistDecay * fDeltaLength;

	float2 vRayOffset = float2(0.f, 0.f);
	float fDecay = g_fInitDecay;
	float fRayIntensity = 0.f;

	for (int i = 0; i < NUM_STEPS; ++i)
	{
		float2 vSamplePos = In.vTexCoord + vRayOffset;
		float fCurrentIntensity = g_SSLRTexture.Sample(LinearSampler, vSamplePos);

		fRayIntensity += fCurrentIntensity * fDecay;
		vRayOffset += vRayDelta;
		fDecay = saturate(fDecay - fStepDecay);
	}

	Out.vColor = float4(fRayIntensity, 0.f, 0.f, 0.f);

	return Out;
}

technique11 DefaultTechnique
{
	// 0
	pass Default
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Sky, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	// 1
	pass DirectionalLight
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_RGBBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Sky, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
	}

	// 2
	pass PointLight
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_RGBBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Sky, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_POINT();
	}

	// 3
	pass Deferred_Blend
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Sky, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_BLEND();
	}

	// 4
	pass Deferred_Distortion
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Sky, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DISTORTION();
	}

	// 5
	pass Deferred_Blur_Vertical
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Sky, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_BLUR_VERTICAL();
	}

	// 6
	pass Deferred_Blur_Horizontal
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Sky, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_BLUR_HORIZONTAL();
	}

	// 7
	pass Deferred_PostProcessing
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Sky, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_POSTPROCESSING();
	}
	
	// 8
	pass Deferred_HDR
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Sky, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_HDR();
	}

	// 9
	pass Deferred_SSAO
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Sky, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_SSAO();
	}

	// 10
	pass Deferred_Outline_Sobel
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Sky, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_OUTLINE_SOBEL();
	}

	// 11
	pass Deferred_Bloom
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Sky, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_BLOOM();
	}

	// 12
	pass Deferred_ScreenMotionBlur
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Sky, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MOTIONBLUR();
	}

	// 13
	pass Deferred_Default_Alpha_Discard
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Sky, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_ALPHA_DISCARD();
	}

	// 14
	pass Deferred_DepthOfField
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Sky, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_DEPTH_OF_FIELD();
	}

	// 15
	pass Deferred_Fog
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Sky, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_FOG();
	}

	// 16
	pass Deferred_Default_LinearSampler
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Sky, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_LINEAR();
	}

	// 17
	pass Deferred_SSLR
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Sky, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_SSLR();
	}

	// 18
	pass Deferred_Point_AlphaBlend
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Sky, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}