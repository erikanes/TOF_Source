SamplerState LinearSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = clamp;
	AddressV = clamp;
};

SamplerState PointSampler
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = wrap;
	AddressV = wrap;
};