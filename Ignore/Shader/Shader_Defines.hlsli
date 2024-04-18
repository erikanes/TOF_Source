#include "Shader_Struct.hlsli"

static float PI = 3.14159265358979323846f;

sampler LinearSampler = sampler_state
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = wrap;
	AddressV = wrap;
};

sampler PointSampler = sampler_state
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = wrap;
	AddressV = wrap;
};

sampler LinearClampSampler = sampler_state
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = clamp;
	AddressV = clamp;
};

sampler PointClampSampler = sampler_state
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = clamp;
	AddressV = clamp;
};


sampler LinearWrapSampler = sampler_state
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = wrap;
	AddressV = wrap;
};

sampler LinearBorderSampler = sampler_state
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = border;
	AddressV = border;

	BorderColor = float4(0.f, 0.f, 0.f, 0.f);
};

sampler PointBorderSampler = sampler_state
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = border;
	AddressV = border;

	BorderColor = float4(1.f, 1.f, 1.f, 1.f);
};

//sampler ComparisonSampler = sampler_state
SamplerComparisonState ComparisonSampler
{
	Filter = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	AddressU = BORDER;
	AddressV = BORDER;
	AddressW = BORDER;

	BorderColor = float4(1.f, 1.f, 1.f, 1.f);

	ComparisonFunc = LESS_EQUAL;
};


RasterizerState RS_Default
{
	FillMode = solid;
	CullMode = back;
	FrontCounterClockwise = false;
};

RasterizerState RS_Wireframe
{
	FillMode = Wireframe;
	CullMode = back;
	FrontCounterClockwise = false;
};

RasterizerState RS_Sky
{
	FillMode = solid;
	CullMode = none;
	FrontCounterClockwise = false;
};

RasterizerState RS_Skydome
{
	FillMode = solid;
	CullMode = back;
	FrontCounterClockwise = false;
};

RasterizerState RS_NoCullSolid
{
	FillMode = Solid;
	CullMode = none;
};

RasterizerState RS_FrontCull
{
	FillMode = solid;
	CullMode = front;
	FrontCounterClockwise = false;
};


BlendState BS_Default
{
	BlendEnable[0] = false;
};

BlendState BS_Default_0
{
    BlendEnable[0] = false;
    BlendEnable[1] = true;
};

BlendState BS_RGBBlend
{
	BlendEnable[0] = true;
	BlendEnable[1] = true;

	SrcBlend = one;
	DestBlend = one;
	BlendOp = Add;

	SrcBlendAlpha = one;
	DestBlendAlpha = one;
	BlendOp = Add;
};

BlendState BS_VFXBlend
{
	// Diffuse target
	BlendEnable[0] = true;

	SrcBlend[0] = ONE; // 그리려는 색상
	DestBlend[0] = ONE; // 이미 그려진 타겟
	BlendOp[0] = ADD;

	SrcBlendAlpha[0] = ONE;
	DestBlendAlpha[0] = ONE;
	BlendOpAlpha[0] = MAX;

	BlendEnable[1] = true;

	// Depth target
	SrcBlend[1] = ONE;
	DestBlend[1] = ONE;
	BlendOP[1] = MIN;
};

BlendState BS_RGBBlend_SingleTarget
{
	BlendEnable[0] = true;

	SrcBlend = one;
	DestBlend = one;
	BlendOp = Add;
};

BlendState BS_AlphaBlend
{
	BlendEnable[0] = true;
	SrcBlend = Src_Alpha;
	DestBlend = Inv_Src_Alpha;
	BlendOp = Add;
};


DepthStencilState DSS_Default
{
	DepthEnable = true;
	DepthWriteMask = all;
	DepthFunc = less_equal;
};

DepthStencilState DSS_Sky
{
	DepthEnable = false;
	DepthWriteMask = zero;
};

DepthStencilState DSS_VFX
{
	DepthEnable = true;
	DepthWriteMask = zero;
	DepthFunc = less_equal;
};

DepthStencilState DSS_Particle
{
	DepthEnable = true;
	DepthWriteMask = zero;
	DepthFunc = less_equal;
};