#pragma once

#include "Base.h"

BEGIN(Engine)

class CRenderer;

class ENGINE_DLL CRender_Manager final : public CBase
{
public:
	enum FADE_TYPE { FADE_IN, FADE_OUT, FADE_END };

private:
	DECLARE_SINGLETON(CRender_Manager)

private:
	CRender_Manager();
	virtual ~CRender_Manager() = default;

public:
	HRESULT Register_RenderFunction(function<HRESULT(list<CGameObject*>&)> _renderFunc, _uint _eGroup);
	void Reset_RenderFunction();

	// HDR
	void Enable_HDR();
	void Disable_HDR();
	
	// Adaptation
	void Enable_Adaptation();
	void Disable_Adaptation();

	// Bloom
	void Enable_Bloom();
	void Disable_Bloom();

	// Glow
	void Enable_Glow();
	void Disable_Glow();

	// Blur
	void Enable_Blur();
	void Disable_Blur();

	// SSAO
	void Enable_SSAO();
	void Disable_SSAO();

	// Rim Light
	void Enable_RimLight() { m_bEnableRimLight = true; }
	void Disable_RimLight() { m_bEnableRimLight = false; }
	_bool Is_Enable_RimLight() { return m_bEnableRimLight; }

	// Edge Highlight (Sobel)
	void Enable_EdgeHighlight();
	void Disable_EdgeHighlight();

	// DOF
	void Enable_DepthOfField();
	void Disable_DepthOfField();

	// Fog
	void Enable_Fog();
	void Disable_Fog();

	// SSLR
	void Enable_SSLR();
	void Disable_SSLR();

	// Fade
	void Begin_Fade(FADE_TYPE _eType, _float _fSecond);
	void Reset_Fade();

#pragma region Getter
	CRenderer* Get_Renderer() const { return m_pRenderer; }

	_float Get_MiddleGrey() const;
	_float Get_LuminanceWhite() const;
	_float Get_BloomThreshold() const;

	const _float2& Get_MiddleGreyRange() const { return m_fMiddleGreyRange; }
	const _float2& Get_LuminanceWhiteRange() const { return m_fLuminanceWhiteRange; }

	// For ImGui
	any Get_RenderAttributes() const;
#pragma endregion

#pragma region Setter
	void Set_Renderer(CRenderer* _pRenderer);

	// HDR
	void Set_MiddleGrey(_float _fMiddleGrey);
	void Set_LuminanceWhite(_float _fLuminanceWhite);
	void Set_AverageLuminanceRange(_float2 _vLuminanceRange);

	// Adaptation
	void Set_AdaptationCoefficient(_float _fCoefficient);
	void Set_AdaptationElapsedTime(_float _fElapsedTime);

	// Bloom
	void Set_Bloom_Threshold(_float _fThreshold);
	void Set_Bloom_Scale(_float _fScale);

	void Set_Glow_AverageLuminance(_float _fLuminance);
	void Set_Glow_Threshold(_float _fThreshold);

	// SSAO
	void Set_SSAO_Length(_float _fLength);
	void Set_SSAO_Threshold(_float _fThreshold);
	void Set_SSAO_BlurPass(_int _iPass);

	// Edge Highlight (Sobel)
	void Set_EdgeHighlight_Thickness(_float _fThickness);
	void Set_EdgeHighlight_Color(const _float3& _vColor);

	// Depth Of Field
	void Set_DepthOfField_Focus(_float _fFocus);
	void Set_DepthOfField_FocusRange(_float _fRange);

	// Fog
	void Set_FogStart(_float _fStart);
	void Set_FogEnd(_float _fEnd);
	void Set_FogColor(const _float4 _vColor);

	void Set_UseHeightFog(_bool _bUse);
	void Set_HeightFogStart(_float _fStart);
	void Set_HeightFogEnd(_float _fEnd);
#pragma endregion

#pragma region Debug
	_uint Get_BatchCount() const;
	void Add_BatchCount();

	_float Get_NavCell_CullingDistance() const { return m_fNavCellCullingDistance; }
	void Set_NavCell_CullingDistance(_float _fDistance) { m_fNavCellCullingDistance = _fDistance; }

	void Enable_PBR() { m_bUsePBR = true; }
	void Disable_PBR() { m_bUsePBR = false; }
	_bool IsEnable_PBR() const { return m_bUsePBR; }

	void Enable_ClearDepthStencilView_ForDebug();
	void Disable_ClearDepthStencilView_ForDebug();
	_bool IsEnable_ClearDepthStencilView_ForDebug();

#ifdef _DEBUG
	_bool IsEnableRenderMap() const;
	void Set_EnableRenderMap(_bool _b);

	void Set_ShadowBias(_float _f);
	_float Get_ShadowBias() const;

	_float Get_AverageLuminance() const;
#endif
#pragma endregion

public:
	virtual void Free() override;

private:
	CRenderer* m_pRenderer = { nullptr };

	_float2 m_fMiddleGreyRange = SimpleMath::Vector2::One;
	_float2 m_fLuminanceWhiteRange = SimpleMath::Vector2::One;

	_float m_fNavCellCullingDistance = { 200.f };

	_bool m_bEnableRimLight = { false };

#pragma region DEBUG
	_bool m_bUsePBR = { false };
#pragma endregion
};

END