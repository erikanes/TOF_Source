#include "Shader_Defines.hlsli"
#include "Shader_Variable_Defines.hlsli"
#include "Shader_Material_Defines.hlsli"
#include "Shader_Client_Functions.hlsli"

matrix g_BoneMatricies[256];

// 비등방성(이방성) 헤어
float g_fAnisoHairFresnelPow = 5.f;
float g_fAnisoHairFresnelIntensity = 5.f;

float3 g_vCharacterForward;
float3 g_vCharacterRight;

// RimLight : Emissive 타겟에 기록됨
bool g_bUseRimLight = false;
float g_fRimPower = 5.f;
float4 g_vRimColor = float4(0.3f, 0.3f, 0.3f, 0.f);

// Custom Lighting
float g_fLightSmooth = 0.1f;
float4 g_vBaseColor = float4(1.f, 1.f, 1.f, 0.f);

float4 g_vShadowColor = float4(0.9f, 0.9f, 0.9f, 0.f);
float g_fShadowAttenuation = 0.1f;
float g_fReceiveShadowMappingAmount = 1.f;

// Camera Distance Noise
texture2D g_CameraNoiseTexture;

float IsInInterval(float A, float B, float X)
{
	return step(A, X) * (1.f - step(B, X));
}

void OutlineCheck(in float2 TexCoord, in float Weight, in float Base, inout float N)
{
	float4 vDiffuse = g_DiffuseTexture.Sample(LinearSampler, TexCoord);
	float fDepth = vDiffuse.a;

	N += Weight * (1.f - IsInInterval(Base - 0.004f, Base + 0.004f, fDepth));
}

float Outline(in float2 TexCoord, in float Base)
{
	float2 vResolution = float2(1920.f, 1080.f);
	float2 vUVPixel = 1.f / vResolution;
	float fN = 0.f;

	float WO_0 = 1.f / 8.f;
	float WO_1 = 1.f / 8.f;

	OutlineCheck(TexCoord + float2(1.f, 0.f) * vUVPixel, WO_1, Base, fN);
	OutlineCheck(TexCoord + float2(0.f, 1.f) * vUVPixel, WO_1, Base, fN);
	OutlineCheck(TexCoord + float2(0.f, -1.f) * vUVPixel, WO_1, Base, fN);
	OutlineCheck(TexCoord + float2(-1.f, 0.f) * vUVPixel, WO_1, Base, fN);

	OutlineCheck(TexCoord + float2(1.f, 1.f) * vUVPixel, WO_0, Base, fN);
	OutlineCheck(TexCoord + float2(1.f, -1.f) * vUVPixel, WO_0, Base, fN);
	OutlineCheck(TexCoord + float2(-1.f, 1.f)* vUVPixel, WO_0, Base, fN);
	OutlineCheck(TexCoord + float2(-1.f, -1.f) * vUVPixel, WO_0, Base, fN);

	return fN;
}

struct VS_CHARACTER_IN
{
	float3 vPosition : POSITION0;
	float3 vNormal : NORMAL0;
	float2 vTexCoord : TEXCOORD0;
	float3 vTangent : TANGENT0;
	float4 vVertexColor : TEXCOORD1;
	uint4 vBlendIndices : BLENDINDICES0;
	float4 vBlendWeights : BLENDWEIGHT0;

	row_major matrix matWorld : WORLD;
	row_major matrix matPivot : PIVOT;
};

struct VS_CHARACTER_OUT
{
	float4 vPosition : SV_POSITION;
	float4 vNormal : NORMAL;
	float2 vTexCoord : TEXCOORD0;
	float4 vWorldPos : TEXCOORD1;
	float4 vProjPos : TEXCOORD2;
	float3 vTangent : TANGENT;
	float3 vBiNormal : BINORMAL;

	float3 vCamDirection : TEXCOORD3;
	float fCamDistance : TEXCOORD4;
};

struct VS_SHADOW_OUT
{
	float4 vPosition : SV_POSITION;
	float4 vProjPos : TEXCOORD0;
};

struct VS_OUTLINE_OUT
{
	float4	vPosition : SV_POSITION;
	float2	vTexCoord : TEXCOORD0;
	float4	vProjPos : TEXCOORD1;
	float	fCamDistance : TEXCOORD2;
};

VS_CHARACTER_OUT VS_MAIN(VS_CHARACTER_IN In)
{
	VS_CHARACTER_OUT Out = (VS_CHARACTER_OUT)0;

	matrix matBone =
		g_BoneMatricies[In.vBlendIndices.x] * In.vBlendWeights.x +
		g_BoneMatricies[In.vBlendIndices.y] * In.vBlendWeights.y +
		g_BoneMatricies[In.vBlendIndices.z] * In.vBlendWeights.z +
		g_BoneMatricies[In.vBlendIndices.w] * In.vBlendWeights.w;

	vector vPosition = mul(vector(In.vPosition, 1.f), matBone);
	vector vNormal = mul(vector(In.vNormal, 0.f), matBone);

	matrix matWVP = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWVP, g_ProjMatrix);

	Out.vPosition = mul(vPosition, matWVP);
	Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
	Out.vTexCoord = In.vTexCoord;
	Out.vWorldPos = mul(vPosition, g_WorldMatrix);
	Out.vProjPos = Out.vPosition;

	Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix)).xyz;
	Out.vBiNormal = normalize(cross(Out.vNormal.xyz, Out.vTangent));
	
	Out.vCamDirection = normalize(Out.vWorldPos.xyz - g_vCamPosition.xyz);
	Out.fCamDistance = distance(Out.vWorldPos.xyz, g_vCamPosition.xyz);

	return Out;
}

VS_SHADOW_OUT VS_SHADOW(VS_CHARACTER_IN In)
{
	VS_SHADOW_OUT Out = (VS_SHADOW_OUT)0;

	matrix matBone =
		g_BoneMatricies[In.vBlendIndices.x] * In.vBlendWeights.x +
		g_BoneMatricies[In.vBlendIndices.y] * In.vBlendWeights.y +
		g_BoneMatricies[In.vBlendIndices.z] * In.vBlendWeights.z +
		g_BoneMatricies[In.vBlendIndices.w] * In.vBlendWeights.w;

	vector vPosition = mul(vector(In.vPosition, 1.f), matBone);

	matrix matWVP = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWVP, g_ProjMatrix);

	Out.vPosition = mul(vPosition, matWVP);
	Out.vProjPos = Out.vPosition;

	return Out;
}

VS_OUTLINE_OUT VS_OUTLINE(VS_CHARACTER_IN In)
{
	VS_OUTLINE_OUT Out = (VS_OUTLINE_OUT)0;

	matrix matBone =
		g_BoneMatricies[In.vBlendIndices.x] * In.vBlendWeights.x +
		g_BoneMatricies[In.vBlendIndices.y] * In.vBlendWeights.y +
		g_BoneMatricies[In.vBlendIndices.z] * In.vBlendWeights.z +
		g_BoneMatricies[In.vBlendIndices.w] * In.vBlendWeights.w;

	vector vPosition = mul(vector(In.vPosition + In.vNormal * g_fOutlineWidth, 1.f), matBone);

	matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
	matrix matWVP = mul(g_WorldMatrix, matVP);

	Out.vPosition = mul(vPosition, matWVP);
	Out.vTexCoord = In.vTexCoord;
	Out.vProjPos = Out.vPosition;
	Out.fCamDistance = distance(mul(vPosition, g_WorldMatrix).xyz, g_vCamPosition.xyz);

	return Out;
}

struct PS_CHARACTER_IN
{
	vector vPosition : SV_POSITION;
	vector vNormal : NORMAL;
	float2 vTexCoord : TEXCOORD0;
	vector vWorldPos : TEXCOORD1;
	vector vProjPos : TEXCOORD2;
	float3 vTangent : TANGENT;
	float3 vBiNormal : BINORMAL;

	float3 vCamDirection : TEXCOORD3;
	float fCamDistance : TEXCOORD4;
};

struct PS_CHARACTER_OUT
{
	vector vDiffuse : SV_TARGET0;
	vector vNormal : SV_TARGET1;
	vector vDepth : SV_TARGET2;
	vector vCharacterDepth : SV_TARGET3;
	vector vEmissive : SV_TARGET4;
};

struct PS_SHADOW_IN
{
	vector vPosition : SV_POSITION;
	vector vProjPos : TEXCOORD0;
};

struct PS_SHADOW_OUT
{
	float fDepth : SV_TARGET0;
};

struct PS_OUTLINE_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexCoord : TEXCOORD0;
	float4 vProjPos : TEXCOORD1;
	float fCamDistance : TEXCOORD2;
};

struct PS_OUTLINE_OUT
{
	float4 vColor : SV_TARGET0;
	float4 vDepth : SV_TARGET2;
	float4 vCharacterDepth : SV_TARGET3;
};

float4 GetDiffuse(in float2 vTexCoord)
{
	float4 vDiffuse = float4(1.f, 1.f, 1.f, 1.f);

	if (g_bUseDiffuseTexture)
		vDiffuse = g_DiffuseTexture.Sample(LinearSampler, vTexCoord);

	else
		vDiffuse = g_vDiffuseColor;

	return vDiffuse;
}

float4 GetNormal(in float2 vTexCoord, in float3 vTangent, in float3 vBiNormal, in float3 vNormal)
{
	float4 vOutNormal;

	if (g_bUseNormalTexture)
	{
		vOutNormal = g_NormalTexture.Sample(LinearSampler, vTexCoord);
		
		if (0.f <= vOutNormal.b)
			vOutNormal.b = sqrt(1 - saturate(dot(vOutNormal.rg, vOutNormal.rg)));

		vOutNormal = vOutNormal * 2.f - 1.f;

		float3x3 matWorld = float3x3(vTangent, vBiNormal, vNormal);
		vOutNormal = float4(normalize(mul(vOutNormal.xyz, matWorld)), 0.f);
	}
	else
		vOutNormal = float4(vNormal, 0.f);

	return vOutNormal;
}

float4 GetMask(in float2 vTexCoord)
{
	float4 vMask = float4(1.f, 1.f, 0.f, 0.f);

	if (g_bUseMaskMapTexture)
		vMask = g_MaskMapTexture.SampleLevel(LinearSampler, vTexCoord, 0);

	return vMask;
}

// body, eyebrow, eyebrush
PS_CHARACTER_OUT PS_MAIN(PS_CHARACTER_IN In)
{
	PS_CHARACTER_OUT Out = (PS_CHARACTER_OUT)0;

	float4 vDiffuse = GetDiffuse(In.vTexCoord);
	float4 vNormal = GetNormal(In.vTexCoord, In.vTangent, In.vBiNormal, In.vNormal.xyz);
	float4 vMask = GetMask(In.vTexCoord);

	float fSpecular = GetAnisoSpecular(vNormal, g_vCamDirection, g_vLightDirection.xyz, vMask.r, vMask.a, 0.3f, 5.f);

	Out.vDiffuse.rgb = GetCustomShadow(vNormal, g_vLightDirection, g_fLightSmooth, g_vBaseColor, g_vShadowColor) * vDiffuse.rgb + fSpecular;
	Out.vDiffuse.a = 1.f;

	Out.vNormal = vNormal * 0.5f + 0.5f;

	Out.vDiffuse.rgb *= clamp(vMask.g, 0.8f, 1.f);

	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCameraFar, 1.f, 1.f);
	Out.vCharacterDepth.r = Out.vDepth.g;

	Out.vEmissive = vector(0.f, 0.f, 0.f, 0.f);

	if (g_bUseEmissiveTexture)
		Out.vEmissive += g_EmissiveTexture.Sample(LinearSampler, In.vTexCoord);

	if (g_bUseRimLight)
		Out.vEmissive += CalculateRimLight(In.vWorldPos.xyz, vNormal.xyz, g_vCamPosition.xyz, g_fRimPower, g_vRimColor);

	return Out;
}

PS_CHARACTER_OUT PS_ANISOTROPIC_HAIR(PS_CHARACTER_IN In)
{
	// anisotropic : 빛을 받을때만 조명 반사
	// fresnel : 측면 제거

	//_m 텍스쳐 r 채널 : specular

	PS_CHARACTER_OUT Out = (PS_CHARACTER_OUT)0;

	float4 vNormal = GetNormal(In.vTexCoord, In.vTangent, In.vBiNormal, In.vNormal.xyz);
	vector vHairMask = g_MaskMapTexture.Sample(LinearSampler, In.vTexCoord);
	float fSpecular = GetAnisoSpecular(vNormal, g_vCamDirection, g_vLightDirection.xyz, vHairMask.r, vHairMask.a, g_fAnisoHairFresnelPow, g_fAnisoHairFresnelIntensity);
	float4 vLerpColor = float4(GetCustomShadow(vNormal, g_vLightDirection, g_fLightSmooth, float4(1.f, 1.f, 1.f, 1.f), g_vShadowColor), 1.f);

	Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexCoord) * g_vDiffuseColor * vLerpColor + fSpecular;
	Out.vDiffuse.a = 1.f;

	Out.vDiffuse.rgb *= clamp(vHairMask.g, 0.8f, 1.f);

	Out.vNormal = vNormal * 0.5f + 0.5f;

	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCameraFar, 1.f, 1.f);
	Out.vCharacterDepth.r = Out.vDepth.g;

	Out.vEmissive = vector(0.f, 0.f, 0.f, 0.f);

	if (g_bUseEmissiveTexture)
		Out.vEmissive += g_EmissiveTexture.Sample(LinearSampler, In.vTexCoord);

	if (g_bUseRimLight)
		Out.vEmissive += CalculateRimLight(In.vWorldPos.xyz, vNormal.xyz, g_vCamPosition.xyz, g_fRimPower, g_vRimColor);

	return Out;
}

// normal : face shadow
PS_CHARACTER_OUT PS_FACE(PS_CHARACTER_IN In)
{
	PS_CHARACTER_OUT Out = (PS_CHARACTER_OUT)0;

	float2 vRight = normalize(g_vCharacterRight.xz);
	float2 vForward = normalize(g_vCharacterForward.xz);
	float2 vLightDir = normalize(-g_vLightDirection.xz);

	float fRdotL = dot(vRight, vLightDir);
	float fFdotL = dot(vForward, vLightDir);
	float fAngle = acos(fRdotL) / PI * 2.f; // 0 <= fAngle <= PI --> 0 <= fAngle <= 2
	
	float2 vShadowTexCoord = fRdotL > 0.f ? In.vTexCoord : float2(1.f - In.vTexCoord.x, In.vTexCoord.y);
	float fShadow = g_NormalTexture.Sample(LinearSampler, vShadowTexCoord).r;

	fAngle = fRdotL > 0.f ? 1.f - fAngle : fAngle - 1.f;

	fShadow = step(fAngle, fShadow) * step(0.f, fFdotL);

	float4 vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexCoord);
	vDiffuse = lerp(vDiffuse, vDiffuse * float4(g_vShadowColor), 1.f - fShadow);

	Out.vDiffuse = vDiffuse;
	Out.vDiffuse.a = 1.f;
	Out.vNormal = In.vNormal * 0.5f + 0.5f;
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCameraFar, 1.f, 1.f);
	Out.vCharacterDepth.r = Out.vDepth.g;
	
	Out.vEmissive = 0.f;
	
	if (g_bUseEmissiveTexture)
		Out.vEmissive += g_EmissiveTexture.Sample(LinearSampler, In.vTexCoord);

	if (g_bUseRimLight)
		Out.vEmissive += CalculateRimLight(In.vWorldPos.xyz, In.vNormal.xyz, g_vCamPosition.xyz, g_fRimPower, g_vRimColor);

	return Out;
}

PS_SHADOW_OUT PS_SHADOW(PS_SHADOW_IN In)
{
	PS_SHADOW_OUT Out = (PS_SHADOW_OUT)0;

	if (0 == g_iProjType) // Perspective
		Out.fDepth = In.vProjPos.w / g_fCameraFar;
	else
		Out.fDepth = In.vProjPos.z / In.vProjPos.w;

	return Out;
}

PS_OUTLINE_OUT PS_OUTLINE(PS_OUTLINE_IN In)
{
	PS_OUTLINE_OUT Out = (PS_OUTLINE_OUT)0;

	float4 vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexCoord);

	Out.vColor.rgb = vDiffuse.rgb * 0.5f;
	Out.vColor.a = 1.f;

	Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCameraFar, 1.f, 1.f);
	Out.vCharacterDepth.r = Out.vDepth.g;

	return Out;
}

technique11 Default
{
	pass Character
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Anisotropic_Hair
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_ANISOTROPIC_HAIR();
	}

	pass Character_Face
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_FACE();
	}

	pass Character_Shadow
	{
		SetRasterizerState(RS_Default);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_SHADOW();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_SHADOW();
	}

	pass Character_Outline
	{
		SetRasterizerState(RS_FrontCull);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		SetDepthStencilState(DSS_Default, 0);

		VertexShader = compile vs_5_0 VS_OUTLINE();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_OUTLINE();
	}
}