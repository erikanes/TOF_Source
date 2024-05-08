#pragma once

#include "Component.h"

BEGIN(Engine)

class CStructured_Buffer;
class CShader;
class CGameObject;

class ENGINE_DLL CRenderer final : public CComponent
{
private:
	friend class CRender_Manager;

	using CustomRenderer = function<HRESULT(list<CGameObject*>&)>;

public:
	enum RENDERGROUP
	{
		RENDER_PRIORITY = 0, // skybox
		RENDER_TERRAIN,
		RENDER_MAP, // almost static mesh
		RENDER_GRASS,
		RENDER_PLAYER,
		RENDER_CHARACTER, // character
		RENDER_NONBLEND, // etc
		RENDER_NONLIGHT,
		RENDER_LENSFLARE,

		RENDER_SHADOW,

		/* Deferred */
		RENDER_BLEND,
		RENDER_VFX, // effect
		RENDER_GLOW,
		/* Deferred */
		RENDER_BLOOM,
		RENDER_BLUR,
		RENDER_DISTORTION,
		RENDER_POSTPROCESSING, // blur, glow, distortion, noise ...
		RENDER_STATIC_UI, // static UI
		RENDER_UI, // 2D, 3D
		RENDER_FONT, // font
		RENDER_END
	};

	typedef struct tagRender_Attributes
	{
		/* Options */
		_bool bEnableHDR = { true };
		_bool bEnableBloom = { true };
		_bool bEnableGlow = { false };
		_bool bEnableBlur = { false };
		_bool bEnableSSAO = { false };
		_bool bEnableEdgeHighlight = { false };
		_bool bEnableAdaptation = { true };
		_bool bEnableDepthOfField = { true };
		_bool bEnableFog = { false };
		_bool bEnableSSLR = { false };

		// HDR
		_float fMiddleGrey = { 1.f };
		_float fLuminanceWhite = { 5.f };
		_float2 vAverageLuminanceRange = { 0.f, 1.f };

		// Bloom
		_float fBloomThreshold = { 0.8f };
		_float fBloomScale = { 1.f };

		// Blur
		_int iBlurPass = { 1 };

		// SSAO
		_float fSSAOLength = { 1.f };
		_float fSSAOThreshold = { 0.8f };
		_int iSSAOBlurPass = { 1 };

		// Edge Highlight (Sobel)
		_float fEdgeHighlightThickness = { 0.0005f };
		_float3 vEdgeHighlightColor = Vector3::One;

		// Adaptation
		_float fAdaptationCoefficient = { 0.f };
		_float fAdaptationElapsedTime = { 1.f };

		// Glow
		_float fGlowAverageLuminance = { 0.7f };
		_float fGlowThreshold = { 0.8f };
		_float fGlowScale = { 1.f };

		// DOF
		_float fFocus = { 0.f };
		_float fFocusRange = { 1.f };

		// Fog
		_uint iFogMode = { 0 }; // 0: linear, 1: exponential, 2: exponential squared
		_float fFogStart = { -200.f };
		_float fFogEnd = { 1000.f };
		_float4 vFogColor = { 0.5f, 0.5f, 0.5f, 0.8f };
		_float fFogProgressTime = { 0.f };
		_bool bUseHeightFog = { false };
		_float fFogHeightStart = 100.f;
		_float fFogHeightEnd = -100.f;

		// Fade
		_float fFadeAlpha = { 1.f };
		_float fFadeValue = { 0.f };
		_float fFadeTime = { 1.f };
		_float fFadeProgressTime = { 1.f };

	} RENDER_ATTRIBUTES;

private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;

public:
	HRESULT Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pGameObject);
	HRESULT Draw_RenderGroup(_float _fTimeDelta);

	HRESULT Register_RenderFunction(CustomRenderer _renderFunc, RENDERGROUP _eGroup);
	void Reset_RenderFunction();

	HRESULT Add_DebugComponent(class CComponent* pDebugCom);
	HRESULT Add_DebugObject(class CGameObject* pDebugObject);

	HRESULT Render_DummyPlayer(_float _fTimeDelta);
private:
	HRESULT _Create_RenderTargets(const D3D11_VIEWPORT& _viewport);
	HRESULT _Create_ComputeShader(const D3D11_VIEWPORT& _viewport);
	HRESULT _Create_Structured_Buffer(const D3D11_VIEWPORT& _viewport);
	HRESULT _Create_Other_DepthStencilView(const D3D11_VIEWPORT& _viewport);
	HRESULT _Create_ShadowDepthStencilView(const D3D11_VIEWPORT& _viewport);


	HRESULT _Render_Priority();

	HRESULT _Render_Terrain();
	HRESULT _Render_Map();
	HRESULT _Render_Grass();
	HRESULT _Render_SSAO();
	HRESULT _Render_Player();
	HRESULT _Render_Character();
	HRESULT _Render_NonBlend();
	HRESULT _Render_SSLR();

	HRESULT _Render_Shadow();

	HRESULT _Render_LightAcc(); // not render group
	HRESULT _Render_Deferred(); // not render group

	HRESULT _Render_NonLight();
	HRESULT _Render_Blend();
	HRESULT _Render_VFX();
	HRESULT _Render_Glow();

	HRESULT _Render_OutLine();

	HRESULT _Render_Lensflare();
	HRESULT _Render_Bloom(); // deferred
	HRESULT _Render_Blur(); // deferred
	HRESULT _Render_Distortion(); // deferred
	HRESULT _Render_PostProcessing(_float _fTimeDelta); // deferred

	HRESULT _Post_Processing_Distortion();
	HRESULT _Post_Processing_HDR(_float _fTimeDelta);
	HRESULT _Post_Processing_Bloom();
	HRESULT _Post_Processing_Blur();
	HRESULT _Post_Processing_DOF();
	HRESULT _Post_Processing_Fog(_float _fTimeDelta);

	HRESULT _Render_Static_UI();
	HRESULT _Render_UI();
	HRESULT _Render_Font();

	HRESULT _Render_Final_Combined(_float fTimeDelta);

	HRESULT _Render_To_BackBuffer(const wstring& _wstrRenderTargetTag, _bool _bUsePointSampler = true, _float _fFadeAlpha = 1.f);
	HRESULT _Render_To_BackBuffer_Alpha_Discard(const wstring& _wstrRenderTargetTag, _float _fAlpha);
	HRESULT _Render_To_BackBuffer_AlphaBlend(const wstring& _wstrRenderTargetTag, _bool _bUsePointSampler = true, _float _fFadeAlpha = 1.f);

	HRESULT _Render_Debug();

#ifdef _DEBUG
	_float _Get_AverageLuminance();
#endif

	HRESULT _Copy_SRV_To_UAV(const wstring& _strSRVtag, const wstring& _strUAVtag);
	HRESULT _Copy_SRV_To_SRV(const wstring& _strSourceTag, const wstring& _strDestTag);
	
	HRESULT _Blur_NPass(_uint _iPassCount = 1);
	HRESULT _RenderTarget_Blur_NPass(const wstring& _strTag, _uint _iPassCount = 1, _bool _bDownScaled = false);

	HRESULT _Bind_BaseInfo(CShader* _pShader);

public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

private:
	_bool m_bDebugRender = { false };

public:
	void Toggle_RenderMode() { m_bDebugRender = !m_bDebugRender; }

private:
	list<class CGameObject*>			m_RenderGroup[RENDER_END];
	list<class CComponent*>				m_DebugComponents;
	class CRenderTarget_Manager*		m_pRenderTarget_Manager = { nullptr };
	class CLight_Manager*				m_pLight_Manager = { nullptr };

	class CVIBuffer_Rect*				m_pVIBuffer = { nullptr };

	CShader*							m_pShader = { nullptr };
	CShader*							m_pPostProcessingShader = { nullptr };

	// Compute Shader
	CShader*							m_pHDRShader = { nullptr };
	CShader*							m_pBlurShader = { nullptr };
	CShader*							m_pBloomShader = { nullptr };
	CShader*							m_pSSAOShader = { nullptr };
	CShader*							m_pSSLRShader = { nullptr };

	unordered_map<string, CStructured_Buffer*> m_resources;

	CShader*							m_pShader_UI_Static = { nullptr };

	_float4x4							m_WorldMatrix_Deferred = SimpleMath::Matrix::Identity;
	_float4x4							m_ViewMatrix_Deferred = SimpleMath::Matrix::Identity;
	_float4x4							m_ProjMatrix_Deferred = SimpleMath::Matrix::Identity;

	static const _uint					HDR_GROUP_SIZE;

	RENDER_ATTRIBUTES					m_tRenderAttributes = {};

	_float								m_fAdaptedProgressTime = { 0.f };
	_uint								m_iBatchCount = { 0 };
	_float								m_fShadowBias = { 0.0002f };

	CustomRenderer						m_customRenderer[RENDER_END];

	ID3D11DepthStencilView*				m_pUIDepthStencilView = { nullptr };
	ID3D11DepthStencilView*				m_pSSLRDepthStencilView = { nullptr };

#ifdef _DEBUG
	bool								m_bEnableRenderMap = { false };
#endif

	ID3D11DepthStencilView*				m_pShadowDepthStencilView = { nullptr };
	D3D11_VIEWPORT						m_shadowViewport = {};

#pragma region Debug
	_bool								m_bEnableClearDepthStencilView_ForDebug = { false };
#pragma endregion
};
END