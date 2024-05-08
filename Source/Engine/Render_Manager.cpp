#include "Engine_pch.h"
#include "Render_Manager.h"

// Component
#include "Renderer.h"

#include "Structured_Buffer.h"

IMPLEMENT_SINGLETON(CRender_Manager)

CRender_Manager::CRender_Manager()
{
	m_fMiddleGreyRange = _float2(0.f, 5.f);
	m_fLuminanceWhiteRange = _float2(0.f, 10.f);
}

HRESULT CRender_Manager::Register_RenderFunction(function<HRESULT(list<CGameObject*>&)> _renderFunc, _uint _eGroup)
{
	return m_pRenderer->Register_RenderFunction(_renderFunc, static_cast<CRenderer::RENDERGROUP>(_eGroup));
}

void CRender_Manager::Reset_RenderFunction()
{
	m_pRenderer->Reset_RenderFunction();
}

void CRender_Manager::Enable_HDR()
{
	m_pRenderer->m_tRenderAttributes.bEnableHDR = true;
}

void CRender_Manager::Disable_HDR()
{
	m_pRenderer->m_tRenderAttributes.bEnableHDR = false;
}

void CRender_Manager::Enable_Adaptation()
{
	m_pRenderer->m_tRenderAttributes.bEnableAdaptation = true;
}

void CRender_Manager::Disable_Adaptation()
{
	m_pRenderer->m_tRenderAttributes.bEnableAdaptation = false;
}

void CRender_Manager::Enable_Bloom()
{
	m_pRenderer->m_tRenderAttributes.bEnableBloom = true;
}

void CRender_Manager::Disable_Bloom()
{
	m_pRenderer->m_tRenderAttributes.bEnableBloom = false;
}

void CRender_Manager::Enable_Glow()
{
	m_pRenderer->m_tRenderAttributes.bEnableGlow = true;
}

void CRender_Manager::Disable_Glow()
{
	m_pRenderer->m_tRenderAttributes.bEnableGlow = false;
}

void CRender_Manager::Enable_Blur()
{
	m_pRenderer->m_tRenderAttributes.bEnableBlur = true;
}

void CRender_Manager::Disable_Blur()
{
	m_pRenderer->m_tRenderAttributes.bEnableBlur = false;
}

void CRender_Manager::Enable_SSAO()
{
	m_pRenderer->m_tRenderAttributes.bEnableSSAO = true;
}

void CRender_Manager::Disable_SSAO()
{
	m_pRenderer->m_tRenderAttributes.bEnableSSAO = false;
}

void CRender_Manager::Enable_EdgeHighlight()
{
	m_pRenderer->m_tRenderAttributes.bEnableEdgeHighlight = true;
}
void CRender_Manager::Disable_EdgeHighlight()
{
	m_pRenderer->m_tRenderAttributes.bEnableEdgeHighlight = false;
}

void CRender_Manager::Enable_DepthOfField()
{
	m_pRenderer->m_tRenderAttributes.bEnableDepthOfField = true;
}

void CRender_Manager::Disable_DepthOfField()
{
	m_pRenderer->m_tRenderAttributes.bEnableDepthOfField = false;
}

void CRender_Manager::Enable_Fog()
{
	m_pRenderer->m_tRenderAttributes.bEnableFog = true;
}

void CRender_Manager::Disable_Fog()
{
	m_pRenderer->m_tRenderAttributes.bEnableFog = false;
}

void CRender_Manager::Enable_SSLR()
{
	m_pRenderer->m_tRenderAttributes.bEnableSSLR = true;
}

void CRender_Manager::Disable_SSLR()
{
	m_pRenderer->m_tRenderAttributes.bEnableSSLR = false;
}

void CRender_Manager::Begin_Fade(FADE_TYPE _eType, _float _fSecond)
{
	if (FADE_TYPE::FADE_IN == _eType)
	{
		m_pRenderer->m_tRenderAttributes.fFadeProgressTime = 0.f;
		m_pRenderer->m_tRenderAttributes.fFadeTime = _fSecond;
		m_pRenderer->m_tRenderAttributes.fFadeValue = 0.f;
	}

	else if (FADE_TYPE::FADE_OUT == _eType)
	{
		m_pRenderer->m_tRenderAttributes.fFadeProgressTime = 0.f;
		m_pRenderer->m_tRenderAttributes.fFadeTime = _fSecond;
		m_pRenderer->m_tRenderAttributes.fFadeValue = 1.f;
	}
}

void CRender_Manager::Reset_Fade()
{
	m_pRenderer->m_tRenderAttributes.fFadeProgressTime = 1.f;
	m_pRenderer->m_tRenderAttributes.fFadeTime = 1.f;
	m_pRenderer->m_tRenderAttributes.fFadeValue = 0.f;
}

#pragma region Getter
_float CRender_Manager::Get_MiddleGrey() const
{
	return m_pRenderer->m_tRenderAttributes.fMiddleGrey;
}

_float CRender_Manager::Get_LuminanceWhite() const
{
	return m_pRenderer->m_tRenderAttributes.fLuminanceWhite;
}

_float CRender_Manager::Get_BloomThreshold() const
{
	return m_pRenderer->m_tRenderAttributes.fBloomThreshold;
}

any CRender_Manager::Get_RenderAttributes() const
{
	return m_pRenderer->m_tRenderAttributes;
}
#pragma endregion

#pragma region Setter

void CRender_Manager::Set_Renderer(CRenderer* _pRenderer)
{
	if (nullptr == _pRenderer)
	{
		if (nullptr != m_pRenderer)
			Safe_Release(m_pRenderer);

		return;
	}

	m_pRenderer = _pRenderer;

	Safe_AddRef(m_pRenderer);
}

void CRender_Manager::Set_MiddleGrey(_float _fMiddleGrey)
{
	m_pRenderer->m_tRenderAttributes.fMiddleGrey = clamp(_fMiddleGrey, m_fMiddleGreyRange.x, m_fMiddleGreyRange.y);
}

void CRender_Manager::Set_LuminanceWhite(_float _fWhiteLuminance)
{
	m_pRenderer->m_tRenderAttributes.fLuminanceWhite = clamp(_fWhiteLuminance, m_fLuminanceWhiteRange.x, m_fLuminanceWhiteRange.y);
}

void CRender_Manager::Set_AverageLuminanceRange(_float2 _vLuminanceRange)
{
	m_pRenderer->m_tRenderAttributes.vAverageLuminanceRange = _vLuminanceRange;
}

void CRender_Manager::Set_AdaptationCoefficient(_float _fCoefficient)
{
	m_pRenderer->m_tRenderAttributes.fAdaptationCoefficient = _fCoefficient;
}

void CRender_Manager::Set_AdaptationElapsedTime(_float _fElapsedTime)
{
	m_pRenderer->m_tRenderAttributes.fAdaptationElapsedTime = _fElapsedTime;
}

void CRender_Manager::Set_Bloom_Threshold(_float _fThreshold)
{
	m_pRenderer->m_tRenderAttributes.fBloomThreshold = _fThreshold;
}

void CRender_Manager::Set_Bloom_Scale(_float _fScale)
{
	m_pRenderer->m_tRenderAttributes.fBloomScale = _fScale;
}

void CRender_Manager::Set_Glow_AverageLuminance(_float _fLuminance)
{
	m_pRenderer->m_tRenderAttributes.fGlowAverageLuminance = _fLuminance;
}

void CRender_Manager::Set_Glow_Threshold(_float _fThreshold)
{
	m_pRenderer->m_tRenderAttributes.fGlowThreshold = _fThreshold;
}

void CRender_Manager::Set_SSAO_Length(_float _fLength)
{
	m_pRenderer->m_tRenderAttributes.fSSAOLength = _fLength;
}

void CRender_Manager::Set_SSAO_Threshold(_float _fThreshold)
{
	m_pRenderer->m_tRenderAttributes.fSSAOThreshold = _fThreshold;
}

void CRender_Manager::Set_SSAO_BlurPass(_int _iPass)
{
	m_pRenderer->m_tRenderAttributes.iSSAOBlurPass = _iPass;
}

void CRender_Manager::Set_EdgeHighlight_Thickness(_float _fThickness)
{
	m_pRenderer->m_tRenderAttributes.fEdgeHighlightThickness = _fThickness;
}

void CRender_Manager::Set_EdgeHighlight_Color(const _float3& _vColor)
{
	m_pRenderer->m_tRenderAttributes.vEdgeHighlightColor = _vColor;
}

void CRender_Manager::Set_DepthOfField_Focus(_float _fFocus)
{
	m_pRenderer->m_tRenderAttributes.fFocus = _fFocus;
}

void CRender_Manager::Set_DepthOfField_FocusRange(_float _fRange)
{
	m_pRenderer->m_tRenderAttributes.fFocusRange = _fRange;
}

void CRender_Manager::Set_FogStart(_float _fStart)
{
	m_pRenderer->m_tRenderAttributes.fFogStart = _fStart;
}

void CRender_Manager::Set_FogEnd(_float _fEnd)
{
	m_pRenderer->m_tRenderAttributes.fFogEnd = _fEnd;
}

void CRender_Manager::Set_FogColor(const _float4 _vColor)
{
	m_pRenderer->m_tRenderAttributes.vFogColor = _vColor;
}

void CRender_Manager::Set_UseHeightFog(_bool _bUse)
{
	m_pRenderer->m_tRenderAttributes.bUseHeightFog = _bUse;
}

void CRender_Manager::Set_HeightFogStart(_float _fStart)
{
	m_pRenderer->m_tRenderAttributes.fFogHeightStart = _fStart;
}

void CRender_Manager::Set_HeightFogEnd(_float _fEnd)
{
	m_pRenderer->m_tRenderAttributes.fFogHeightEnd = _fEnd;
}

void CRender_Manager::Enable_ClearDepthStencilView_ForDebug()
{
	m_pRenderer->m_bEnableClearDepthStencilView_ForDebug = true;
}

void CRender_Manager::Disable_ClearDepthStencilView_ForDebug()
{
	m_pRenderer->m_bEnableClearDepthStencilView_ForDebug = false;
}

_bool CRender_Manager::IsEnable_ClearDepthStencilView_ForDebug()
{
	return m_pRenderer->m_bEnableClearDepthStencilView_ForDebug;
}

_uint CRender_Manager::Get_BatchCount() const
{
	return m_pRenderer->m_iBatchCount;
}

void CRender_Manager::Add_BatchCount()
{
	if (nullptr == m_pRenderer)
		return;

	++m_pRenderer->m_iBatchCount;
}

#ifdef _DEBUG
_bool CRender_Manager::IsEnableRenderMap() const
{
	return m_pRenderer->m_bEnableRenderMap;
}

void CRender_Manager::Set_EnableRenderMap(_bool _b)
{
	m_pRenderer->m_bEnableRenderMap = _b;
}
void CRender_Manager::Set_ShadowBias(_float _f)
{
	m_pRenderer->m_fShadowBias = _f;
}
_float CRender_Manager::Get_ShadowBias() const
{
	return m_pRenderer->m_fShadowBias;
}
_float CRender_Manager::Get_AverageLuminance() const
{
	return m_pRenderer->_Get_AverageLuminance();
}
#endif

#pragma endregion

void CRender_Manager::Free()
{
	Safe_Release(m_pRenderer);
}