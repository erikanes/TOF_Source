static float MUL_PI = 1.f / PI;

float3 GetCustomShadow(in float4 vNormal, in float4 vLightDir, in float fLightSmooth,
	in float4 vBaseColor, in float4 vShadowColor)
{
	float fNdotL = dot(normalize(-vLightDir), vNormal);
	float fSmooth = smoothstep(0.f, fLightSmooth, fNdotL);
	float3 vLerpColor = lerp(vShadowColor.xyz, vBaseColor.xyz, fSmooth);

	return vLerpColor;
}

float GetAnisoSpecular(in float4 vNormal, in float3 vViewDir, in float3 vLightDir, in float fSpecular,
	in float fMask,	in float fFresnelPower,	in float fFresnelIntensity)
{
	float fAnisoResult = 0.f;

	if (0.f < fSpecular)
	{
		float fNdotV = saturate(dot(vNormal.xyz, normalize(-vViewDir)));
		float fNdotL = saturate(dot(vNormal.xyz, normalize(-vLightDir.xyz)));

		float fAnisoFresnel = pow(1.f - fNdotV, fFresnelPower) * fFresnelIntensity;
 		fAnisoResult = saturate(1.f - fAnisoFresnel) * fSpecular * fNdotL * fMask;
	}

	return fAnisoResult;
}

float4 CalculateRimLight(in float3 vPos, in float3 vNormal, in float3 vViewPos, in float fRimPower, in float4 vRimColor)
{
	float3 vToCameraDir = normalize(vViewPos - vPos);

	float fNdotV = dot(vNormal, vToCameraDir);

	// RimPower의 값이 높을수록 적용 범위가 좁아짐
	fNdotV = saturate(pow(1.f - fNdotV, fRimPower));

	float4 vColor = vRimColor * fNdotV;
	vColor.a = 0.f;

	return vColor;
}