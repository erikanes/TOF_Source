#include "Engine_pch.h"
#include "Renderer.h"

// Singleton
#include "GameInstance.h"
#include "RenderTarget_Manager.h"
#include "Graphic_Device.h"
#include "Light_Manager.h"
#include "PipeLine.h"

// GameObject
#include "GameObject.h"
#include "BlendObject.h"

// Component
#include "Shader.h"
#include "VIBuffer_Rect.h"

#include "Structured_Buffer.h"
#include "RenderTarget.h"

#include "Engine_Math.h" 
#include "Engine_Functions.h"

#include "Light.h"

#define DEFERRED_PASS_DEFAULT					0
#define DEFERRED_PASS_DIRECTIONALLIGHT			1
#define DEFERRED_PASS_POINTLIGHT				2
#define DEFERRED_PASS_BLEND						3
#define DEFERRED_PASS_DISTORTION				4
#define DEFERRED_PASS_BLUR_VERTICAL				5
#define DEFERRED_PASS_BLUR_HORIZONTAL			6
#define DEFERRED_PASS_POSTPROCESSING			7
#define DEFERRED_PASS_HDR						8
#define DEFERRED_PASS_SSAO						9
#define DEFERRED_PASS_OUTLINE_SOBEL				10
#define DEFERRED_PASS_BLOOM						11
#define DEFERRED_PASS_MOTIONBLUR				12
#define DEFERRED_PASS_DEFAULT_ALPHA_DISCARD		13
#define DEFERRED_PASS_DEPTH_OF_FIELD			14
#define DEFERRED_PASS_FOG						15
#define DEFERRED_PASS_DEFAULT_LINEAR			16
#define DEFERRED_PASS_SSLR						17
#define DEFERRED_PASS_DEFAULT_ALPHABLEND		18


#define COMPUTE_PASS_BLUR_HORIZONTAL			0
#define COMPUTE_PASS_BLUR_VERTICAL				1
#define COMPUTE_PASS_BLUR_DOWNSCALE4X			2

#define COMPUTE_PASS_HDR_DOWNSCALE_FIRST		0
#define COMPUTE_PASS_HDR_DOWNSCALE_SECOND		1

#define COMPUTE_PASS_BLOOM						0
#define COMPUTE_PASS_GLOW						1
#define COMPUTE_PASS_BLOOM_DEPTH_DOWNSCALE_4X	2


#define COMPUTE_PASS_SSAO						0
#define COMPUTE_PASS_SSAO_BIND					1

#define COMPUTE_PASS_OUTLINE_SOBEL				0

#define COMPUTE_PASS_SSLR_DOWNSCALE2X			0
#define COMPUTE_PASS_SSLR_OCCLUSION				1

const _uint CRenderer::HDR_GROUP_SIZE = 16;

#ifdef _DEBUG
// 수치가 높을수록 그림자 품질이 좋아지는 대신 vram 사용량이 늘어남
//constexpr _float SHADOWDEPTH_SCALE = 8.f;
constexpr _float SHADOWDEPTH_SCALE = 1.f;
#else
constexpr _float SHADOWDEPTH_SCALE = 12.f;
#endif

constexpr _float SHADOWDEPTH_RESOLUTION = SHADOWDEPTH_SCALE * 1000.f;

_bool Compare(CGameObject* pSour, CGameObject* pDest)
{
	return ((CBlendObject*)pSour)->Get_CamDistance() > ((CBlendObject*)pDest)->Get_CamDistance();
}

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
	, m_pRenderTarget_Manager(CRenderTarget_Manager::Get_Instance())
	, m_pLight_Manager(CLight_Manager::Get_Instance())
{
	Safe_AddRef(m_pRenderTarget_Manager);
	Safe_AddRef(m_pLight_Manager);
}

HRESULT CRenderer::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	_uint					iNumViews = 1;
	D3D11_VIEWPORT			ViewPortDesc;

	m_pContext->RSGetViewports(&iNumViews, &ViewPortDesc);

	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	m_pShader_UI_Static = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_UI_Static.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	if (nullptr == m_pShader_UI_Static)
	{
		MSG_BOX("Failed to Create Shader_UI_Static");
		return E_FAIL;
	}

	XMStoreFloat4x4(&m_WorldMatrix_Deferred, XMMatrixScaling(ViewPortDesc.Width, ViewPortDesc.Height, 1.f));
	XMStoreFloat4x4(&m_ViewMatrix_Deferred, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix_Deferred, XMMatrixOrthographicLH(ViewPortDesc.Width, ViewPortDesc.Height, 0.f, 1.f));

	if (FAILED(_Create_RenderTargets(ViewPortDesc)))
		return E_FAIL;

	if (FAILED(_Create_ComputeShader(ViewPortDesc)))
		return E_FAIL;

	if (FAILED(_Create_Structured_Buffer(ViewPortDesc)))
		return E_FAIL;

	if (FAILED(_Create_Other_DepthStencilView(ViewPortDesc)))
		return E_FAIL;

#pragma region Render Attributes
	{
		m_tRenderAttributes.bEnableHDR = true;
		m_tRenderAttributes.bEnableBloom = true;
		m_tRenderAttributes.bEnableGlow = false;
		m_tRenderAttributes.bEnableBlur = false;
		m_tRenderAttributes.bEnableSSAO = false;
		m_tRenderAttributes.bEnableEdgeHighlight = false;
		m_tRenderAttributes.bEnableAdaptation = true;
		m_tRenderAttributes.bEnableDepthOfField = true;
		m_tRenderAttributes.bEnableFog = false;
		m_tRenderAttributes.bEnableSSLR = false;

		// HDR
		m_tRenderAttributes.fMiddleGrey = 1.f;
		m_tRenderAttributes.fLuminanceWhite = 5.f;

		// Bloom
		m_tRenderAttributes.fBloomThreshold = 0.8f;
		m_tRenderAttributes.fBloomScale = 1.f;

		// Blur
		m_tRenderAttributes.iBlurPass = 1;

		// SSAO
		m_tRenderAttributes.fSSAOLength = 1.f;
		m_tRenderAttributes.fSSAOThreshold = 0.8f;
		m_tRenderAttributes.iSSAOBlurPass = 1;

		// Edge Highlight
		m_tRenderAttributes.fEdgeHighlightThickness = 0.0005f;
		m_tRenderAttributes.vEdgeHighlightColor = Vector3::One;

		// Adaptation
		m_tRenderAttributes.fAdaptationCoefficient = 0.f;
		m_tRenderAttributes.fAdaptationElapsedTime = 1.f;

		// Glow
		m_tRenderAttributes.fGlowAverageLuminance = 0.7f;
		m_tRenderAttributes.fGlowThreshold = 0.8f;
		m_tRenderAttributes.fGlowScale = 0.7f;

		// DOF
		m_tRenderAttributes.fFocus = 0.1f;
		m_tRenderAttributes.fFocusRange = 0.5f;

		// Fog
		m_tRenderAttributes.iFogMode = 0;
		m_tRenderAttributes.fFogStart = -200.f;
		m_tRenderAttributes.fFogEnd = 1000.f;
		m_tRenderAttributes.vFogColor = _float4(0.77f, 0.56f, 0.3f, 1.f);
	}

	m_fShadowBias = 0.0002f;
#pragma endregion

	Reset_RenderFunction();

	if (FAILED(_Create_ShadowDepthStencilView(ViewPortDesc)))
		return E_FAIL;

#ifdef _DEBUG
	if (FAILED(m_pRenderTarget_Manager->Ready_Debug(TEXT("Target_Diffuse"), 100.0f, 100.0f, 200.0f, 200.0f)))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Ready_Debug(TEXT("Target_Normal"), 100.0f, 300.0f, 200.0f, 200.0f)))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Ready_Debug(TEXT("Target_Depth"), 100.0f, 500.0f, 200.0f, 200.0f)))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Ready_Debug(TEXT("Target_Shade"), 300.0f, 100.f, 200.0f, 200.0f)))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Ready_Debug(TEXT("Target_Specular"), 300.0f, 300.f, 200.0f, 200.0f)))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Ready_Debug(TEXT("Target_PlayerDepth"), 500.0f, 100.f, 200.0f, 200.0f)))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Ready_Debug(TEXT("Target_VFXDepth"), 500.0f, 300.f, 200.0f, 200.0f)))
		return E_FAIL;

	m_bEnableRenderMap = true;
#endif

	return S_OK;
}

HRESULT CRenderer::Add_RenderGroup(RENDERGROUP eRenderGroup, CGameObject* pGameObject)
{
	if (eRenderGroup >= RENDER_END)
		return E_FAIL;

	m_RenderGroup[static_cast<_uint>(eRenderGroup)].push_back(pGameObject);

	Safe_AddRef(pGameObject);

	return S_OK;
}

HRESULT CRenderer::Draw_RenderGroup(_float _fTimeDelta)
{
	m_iBatchCount = 0;

	// 임의의 백버퍼 바인딩
	if (FAILED(m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_BackBuffer"))))
		return E_FAIL;

	// Skybox, ...
	if (FAILED(_Render_Priority()))
		return E_FAIL;

	// Map
	if (FAILED(_Render_Terrain()))
		return E_FAIL;

	if (FAILED(_Render_Map()))
		return E_FAIL;

	if (FAILED(_Render_Grass()))
		return E_FAIL;

	if (FAILED(_Render_SSAO()))
		return E_FAIL;

	// Player
	if (FAILED(_Render_Player()))
		return E_FAIL;

	// Character : NPC, Monster...
	if (FAILED(_Render_Character()))
		return E_FAIL;

#pragma region currently disabled
	if (FAILED(_Render_NonBlend()))
		return E_FAIL;

	if (FAILED(_Render_NonLight()))
		return E_FAIL;
#pragma endregion

	//if (FAILED(_Render_SSLR()))
	//	return E_FAIL;

	if (FAILED(_Render_Shadow()))
		return E_FAIL;

	if (FAILED(_Render_LightAcc()))
		return E_FAIL;

	if (FAILED(_Render_Deferred()))
		return E_FAIL;

	if (FAILED(_Render_Blend()))
		return E_FAIL;

	if (FAILED(_Render_VFX()))
		return E_FAIL;

	if (FAILED(_Render_Glow()))
		return E_FAIL;

	if (FAILED(_Render_Lensflare()))
		return E_FAIL;

	/* Deferred */
	if (FAILED(_Render_OutLine()))
		return E_FAIL;

	if (FAILED(_Render_Blur()))
		return E_FAIL;

	if (FAILED(_Render_Distortion()))
		return E_FAIL;

	if (FAILED(_Render_Bloom()))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	if (FAILED(_Render_PostProcessing(_fTimeDelta)))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Reserve"))))
		return E_FAIL;

	_Render_To_BackBuffer(TEXT("Target_BackBuffer"));

	/* UI */
	if (FAILED(_Render_Static_UI()))
		return E_FAIL;

	if (FAILED(_Render_UI()))
		return E_FAIL;

	if (FAILED(_Render_Font()))
		return E_FAIL;

	m_pRenderTarget_Manager->End_MRT(m_pContext);

	if (FAILED(_Render_Final_Combined(_fTimeDelta)))
		return E_FAIL;

//#ifdef _DEBUG
	if (FAILED(_Render_Debug()))
		return E_FAIL;
//#endif

	return S_OK;
}

HRESULT CRenderer::Register_RenderFunction(CustomRenderer _renderFunc, RENDERGROUP _eGroup)
{
	m_customRenderer[(_uint)_eGroup] = _renderFunc;

	return S_OK;
}

void CRenderer::Reset_RenderFunction()
{
	auto iMax = static_cast<_uint>(RENDERGROUP::RENDER_END);

	for (_uint i = 0; i < iMax; ++i)
		m_customRenderer[i] = [](list<CGameObject*>&) { return S_OK; };

	// Map
	m_customRenderer[RENDER_MAP] = [this](list<CGameObject*>& _list)
	{
		for (auto& pObject : _list)
		{
			if (nullptr != pObject)
				pObject->Render();

			Safe_Release(pObject);
		}

		_list.clear();

		return S_OK;
	};

	// Grass
	m_customRenderer[RENDER_GRASS] = m_customRenderer[RENDER_MAP];

	// Shadow
	m_customRenderer[RENDER_SHADOW] = [this](list<CGameObject*>& _list)
	{
		m_pContext->ClearDepthStencilView(m_pShadowDepthStencilView, D3D11_CLEAR_DEPTH, 1.f, 0);

		if (FAILED(m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_ShadowMap"), m_pShadowDepthStencilView)))
			return E_FAIL;

		_uint iViewportCount = 1;
		D3D11_VIEWPORT viewport;

		m_pContext->RSGetViewports(&iViewportCount, &viewport);
		m_pContext->RSSetViewports(1, &m_shadowViewport);

		auto& lights = m_pLight_Manager->Get_Lights();

		for (auto& pObject : m_RenderGroup[RENDER_SHADOW])
		{
			pObject->Render_Shadow(lights);
			Safe_Release(pObject);
		}
		m_RenderGroup[RENDER_SHADOW].clear();

		m_pContext->RSSetViewports(1, &viewport);

		m_pRenderTarget_Manager->End_MRT(m_pContext);

		return S_OK;
	};

}

HRESULT CRenderer::_Create_RenderTargets(const D3D11_VIEWPORT& _viewport)
{
	/* Target_BackBuffer */
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_BackBuffer"), _viewport.Width, _viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	/* Target_PostProcessing (aka. second backbuffer) */
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_PostProcessing"), _viewport.Width, _viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	/* Target_Diffuse */
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Diffuse"), _viewport.Width, _viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(1.f, 1.f, 1.f, 0.f))))
		return E_FAIL;

	/* Target_Normal */
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Normal"), _viewport.Width, _viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	/* Target_Emissive */
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Emissive"), _viewport.Width, _viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* Target_Depth */
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Depth"), _viewport.Width, _viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.0f, 1.0f, 0.0f, 1.f))))
		return E_FAIL;

	/* Target_MapDepth */
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_MapDepth"), _viewport.Width, _viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	/* Target_PlayerDepth */
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_PlayerDepth"), _viewport.Width, _viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	/* Target_CharacterDepth */
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_CharacterDepth"), _viewport.Width, _viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	/* Target_RimLight */
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_RimLight"), _viewport.Width, _viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.0f, 1.0f, 0.0f, 1.f))))
		return E_FAIL;

	/* Target_Shade */
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Shade"), _viewport.Width, _viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.0f, 0.0f, 0.0f, 1.f))))
		return E_FAIL;

	/* Target_Specular */
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Specular"), _viewport.Width, _viewport.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
		return E_FAIL;

	/* Target_ShadowDepth */
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_ShadowDepth"), SHADOWDEPTH_RESOLUTION, SHADOWDEPTH_RESOLUTION, DXGI_FORMAT_R32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	/* Target Distortion*/
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Distortion"), _viewport.Width, _viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* Target UI_Static */
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_UI_Static"), _viewport.Width, _viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(1.f, 1.f, 1.f, 0.f))))
		return E_FAIL;

	/* Target UI_Font */
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_UI_Font"), _viewport.Width, _viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(1.f, 1.f, 1.f, 0.f))))
		return E_FAIL;

	/* Target_Sobel */
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Sobel"), _viewport.Width, _viewport.Height, DXGI_FORMAT_R32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* Target_MotionBlur */
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_MotionBlur"), _viewport.Width, _viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* Target_VFX */
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_VFX"), _viewport.Width, _viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* Target_VFXDepth */
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_VFXDepth"), _viewport.Width, _viewport.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 0.f, 1.f, 1.f))))
		return E_FAIL;

	/* Target_DOF */
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_DOF"), _viewport.Width, _viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	/* Target_Blur_Temp */
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Blur_Temp"), _viewport.Width, _viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* Target_Lensflare */
	if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Lensflare"), _viewport.Width, _viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

#pragma region Reserve
	{
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};

		DXGI_FORMAT eFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

		rtvDesc.Format = eFormat;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;

		srvDesc.Format = eFormat;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;

		uavDesc.Format = eFormat;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Texture2D.MipSlice = 0;
		uavDesc.Buffer.NumElements = _viewport.Width * _viewport.Height;

		if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Reserve"), _viewport.Width, _viewport.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f), &rtvDesc, &srvDesc, &uavDesc)))
			return E_FAIL;

		if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_Reserve"), TEXT("Target_Reserve"))))
			return E_FAIL;
	}
#pragma endregion

#pragma region HDR
	{
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};

		DXGI_FORMAT eFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

		rtvDesc.Format = eFormat;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;

		srvDesc.Format = eFormat;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;

		uavDesc.Format = eFormat;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Texture2D.MipSlice = 0;
		uavDesc.Buffer.NumElements = _viewport.Width * _viewport.Height;

		if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_HDR"), _viewport.Width, _viewport.Height, eFormat, _float4(0.f, 0.f, 0.f, 0.f), &rtvDesc, &srvDesc, &uavDesc)))
			return E_FAIL;

		uavDesc.Buffer.NumElements = _viewport.Width * _viewport.Height / 16.f;

		if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_HDR_DownScaled"), _viewport.Width * 0.25f, _viewport.Height * 0.25f, eFormat, _float4(0.f, 0.f, 0.f, 0.f), &rtvDesc, &srvDesc, &uavDesc)))
			return E_FAIL;

		if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_HDR"), TEXT("Target_HDR"))))
			return E_FAIL;
	}
#pragma endregion

#pragma region Blur Target
	{
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};

		DXGI_FORMAT eFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

		rtvDesc.Format = eFormat;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;

		srvDesc.Format = eFormat;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;

		uavDesc.Format = eFormat;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Texture2D.MipSlice = 0;

		uavDesc.Buffer.NumElements = _viewport.Width * _viewport.Height / 16.f;

		if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Blur_Horizontal"), _viewport.Width * 0.25f, _viewport.Height * 0.25f, eFormat, _float4(0.5f, 0.5f, 0.5f, 1.f), &rtvDesc, &srvDesc, &uavDesc)))
			return E_FAIL;

		if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Blur_Vertical"), _viewport.Width * 0.25f, _viewport.Height * 0.25f, eFormat, _float4(0.5f, 0.5f, 0.5f, 1.f), &rtvDesc, &srvDesc, &uavDesc)))
			return E_FAIL;
			
		if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_Blur"), TEXT("Target_Blur_Horizontal"))))
			return E_FAIL;
	}
#pragma endregion

#pragma region Bloom
	{
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};

		DXGI_FORMAT eFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

		rtvDesc.Format = eFormat;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;

		srvDesc.Format = eFormat;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;

		uavDesc.Format = eFormat;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Texture2D.MipSlice = 0;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.NumElements = _viewport.Width * _viewport.Height / 16.f;

		if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Bloom"), _viewport.Width * 0.25f, _viewport.Height * 0.25f, eFormat, _float4(0.f, 0.f, 0.f, 0.f), &rtvDesc, &srvDesc, &uavDesc)))
			return E_FAIL;
	}
#pragma endregion

#pragma region Glow
	{
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};

		DXGI_FORMAT eFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

		rtvDesc.Format = eFormat;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;

		srvDesc.Format = eFormat;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;

		uavDesc.Format = eFormat;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Texture2D.MipSlice = 0;

		if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Glow"), _viewport.Width, _viewport.Height, eFormat, _float4(0.f, 0.f, 0.f, 0.f), &rtvDesc, &srvDesc, &uavDesc)))
			return E_FAIL;

		if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_Glow"), TEXT("Target_Glow"))))
			return E_FAIL;
	}

#pragma region SSAO
	{
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};

		DXGI_FORMAT eFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

		rtvDesc.Format = eFormat;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;

		srvDesc.Format = eFormat;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;

		uavDesc.Format = eFormat;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Texture2D.MipSlice = 0;

		if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_SSAO"), _viewport.Width, _viewport.Height, eFormat, _float4(1.f, 1.f, 1.f, 0.f), &rtvDesc, &srvDesc, &uavDesc)))
			return E_FAIL;
	}
#pragma endregion

#pragma region SSLR
	{
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};

		DXGI_FORMAT eFormat = DXGI_FORMAT_R32_FLOAT;

		rtvDesc.Format = eFormat;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;

		srvDesc.Format = eFormat;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;

		uavDesc.Format = eFormat;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Texture2D.MipSlice = 0;

		if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Depth_Downscaled"), _viewport.Width * 0.5f, _viewport.Height * 0.5f, eFormat, _float4(1.f, 1.f, 1.f, 0.f), &rtvDesc, &srvDesc, &uavDesc)))
			return E_FAIL;

		if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Depth_Downscaled_4X"), _viewport.Width * 0.25f, _viewport.Height * 0.25f, eFormat, _float4(1.f, 1.f, 1.f, 0.f), &rtvDesc, &srvDesc, &uavDesc)))
			return E_FAIL;

		eFormat = DXGI_FORMAT_R8_UNORM;
		rtvDesc.Format = eFormat;
		srvDesc.Format = eFormat;
		uavDesc.Format = eFormat;

		if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_SSLR_Occlusion"), _viewport.Width * 0.5f, _viewport.Height * 0.5f, eFormat, _float4(1.f, 1.f, 1.f, 1.f), &rtvDesc, &srvDesc, &uavDesc)))
			return E_FAIL;

		if (FAILED(m_pRenderTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_SSLR_Ray"), _viewport.Width * 0.5f, _viewport.Height * 0.5f, eFormat, _float4(0.f, 0.f, 0.f, 0.f), &rtvDesc, &srvDesc, &uavDesc)))
			return E_FAIL;

		if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_SSLR"), TEXT("Target_SSLR_Ray"))))
			return E_FAIL;
	}
#pragma endregion

	/* MRT_BackBuffer */
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_BackBuffer"), TEXT("Target_BackBuffer"))))
		return E_FAIL;

	/* MRT_PostProcessing */
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_PostProcessing"), TEXT("Target_PostProcessing"))))
		return E_FAIL;

	/* MRT_GameObjects */
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth"))))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_MapDepth"))))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Emissive"))))
		return E_FAIL;

	/* MRT_Player */
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_Player"), TEXT("Target_Diffuse"))))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_Player"), TEXT("Target_Normal"))))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_Player"), TEXT("Target_Depth"))))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_Player"), TEXT("Target_PlayerDepth"))))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_Player"), TEXT("Target_Emissive"))))
		return E_FAIL;

	/* MRT_Characters */
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_Characters"), TEXT("Target_Diffuse"))))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_Characters"), TEXT("Target_Normal"))))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_Characters"), TEXT("Target_Depth"))))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_Characters"), TEXT("Target_CharacterDepth"))))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_Characters"), TEXT("Target_Emissive"))))
		return E_FAIL;

	/* MRT_LightAcc */
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular"))))
		return E_FAIL;

	/* MRT_Shadow */
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_ShadowMap"), TEXT("Target_ShadowDepth"))))
		return E_FAIL;

	/* MRT_Distortion */
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_Distortion"), TEXT("Target_Distortion"))))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_Distortion"), TEXT("Target_Reserve")))) // 짬통
		return E_FAIL;

	/* MRT_Bloom */
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_Bloom"), TEXT("Target_Bloom"))))
		return E_FAIL;

	/* MRT_UI_Static */
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_UI_Static"), TEXT("Target_UI_Static"))))
		return E_FAIL;

	/* MRT_UI_Font */
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_UI_Font"), TEXT("Target_UI_Font"))))
		return E_FAIL;

	/* MRT_Sobel */
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_Sobel"), TEXT("Target_Sobel"))))
		return E_FAIL;

	/* MRT_VFX */
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_VFX"), TEXT("Target_VFX"))))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_VFX"), TEXT("Target_VFXDepth"))))
		return E_FAIL;

	/* MRT_DOF */
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_DOF"), TEXT("Target_DOF"))))
		return E_FAIL;

	/* MRT_Blur_Temp */
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_Blur_Temp"), TEXT("Target_Blur_Temp"))))
		return E_FAIL;

	/* MRT_Lensflare */
	if (FAILED(m_pRenderTarget_Manager->Add_MRT(TEXT("MRT_Lensflare"), TEXT("Target_Lensflare"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::_Create_ComputeShader(const D3D11_VIEWPORT& _viewport)
{
	m_pHDRShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Compute_HDR.hlsl"));
	m_pBlurShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Compute_Blur.hlsl"));
	m_pBloomShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Compute_Bloom.hlsl"));
	m_pSSAOShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Compute_SSAO.hlsl"));
	m_pSSLRShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Compute_SSLR.hlsl"));

	return S_OK;
}

HRESULT CRenderer::_Create_Structured_Buffer(const D3D11_VIEWPORT& _viewport)
{
#pragma region HDR
	// 1920x1080 => 480x270
	_uint iWidth = static_cast<_uint>(ceil(_viewport.Width * _viewport.Height / 16.f) / 1024.f);

	// first pass
	auto pFirstPassBuffer = CStructured_Buffer::Create(m_pDevice, m_pContext, iWidth);
	
	if (nullptr == pFirstPassBuffer)
		return E_FAIL;

	// average luminance
	auto pSecondPassBuffer = CStructured_Buffer::Create(m_pDevice, m_pContext, 1);

	if (nullptr == pSecondPassBuffer)
		return E_FAIL;

	// previous average luminance
	auto pPrevAverageLumen = CStructured_Buffer::Create(m_pDevice, m_pContext, 1);

	if (nullptr == pPrevAverageLumen)
		return E_FAIL;
#pragma endregion

	m_resources.emplace("HDR_FirstPass", pFirstPassBuffer);
	m_resources.emplace("HDR_SecondPass", pSecondPassBuffer);
	m_resources.emplace("HDR_PrevAverageLumen", pPrevAverageLumen);

	return S_OK;
}

HRESULT CRenderer::_Create_ShadowDepthStencilView(const D3D11_VIEWPORT& _viewport)
{
	ComPtr<ID3D11Texture2D> texture;
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	textureDesc.Width = SHADOWDEPTH_RESOLUTION;
	textureDesc.Height = SHADOWDEPTH_RESOLUTION;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	textureDesc.SampleDesc.Quality = 0;
	textureDesc.SampleDesc.Count = 1;

	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&textureDesc, nullptr, texture.GetAddressOf())))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateDepthStencilView(texture.Get(), nullptr, &m_pShadowDepthStencilView)))
		return E_FAIL;

	m_shadowViewport.TopLeftX = 0.f;
	m_shadowViewport.TopLeftY = 0.f;
	m_shadowViewport.Width = SHADOWDEPTH_RESOLUTION;
	m_shadowViewport.Height = SHADOWDEPTH_RESOLUTION;
	m_shadowViewport.MinDepth = 0.f;
	m_shadowViewport.MaxDepth = 1.f;

	return S_OK;
}

HRESULT CRenderer::_Create_Other_DepthStencilView(const D3D11_VIEWPORT& _viewport)
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	D3D11_TEXTURE2D_DESC	TextureDesc;

#pragma region UI DepthStencilView
	ComPtr<ID3D11Texture2D> pUIDepthStencilTexture = nullptr;
	
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = _viewport.Width;
	TextureDesc.Height = _viewport.Height;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, pUIDepthStencilTexture.GetAddressOf())))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateDepthStencilView(pUIDepthStencilTexture.Get(), nullptr, &m_pUIDepthStencilView)))
		return E_FAIL;
#pragma endregion

#pragma region SSLR DepthStencilView
	ComPtr<ID3D11Texture2D> pSSLRDepthStencilTexture = nullptr;

	TextureDesc.Width = _viewport.Width * 0.5f;
	TextureDesc.Height = _viewport.Height * 0.5f;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, pSSLRDepthStencilTexture.GetAddressOf())))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateDepthStencilView(pSSLRDepthStencilTexture.Get(), nullptr, &m_pSSLRDepthStencilView)))
		return E_FAIL;
#pragma endregion

	return S_OK;
}

#pragma region Render Functions
HRESULT CRenderer::_Render_Priority()
{
	for (auto& pGameObject : m_RenderGroup[RENDER_PRIORITY])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderGroup[RENDER_PRIORITY].clear();

	return S_OK;
}

HRESULT CRenderer::_Render_Terrain()
{
	m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_GameObjects"));

	for (auto& pGameObject : m_RenderGroup[RENDER_TERRAIN])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderGroup[RENDER_TERRAIN].clear();

	return S_OK;
}

HRESULT CRenderer::_Render_Map()
{
#ifdef _DEBUG
	if (m_bEnableRenderMap)
		m_customRenderer[RENDER_MAP](m_RenderGroup[RENDER_MAP]);
	else
	{
		for (auto& pObject : m_RenderGroup[RENDER_MAP])
			Safe_Release(pObject);

		m_RenderGroup[RENDER_MAP].clear();
	}

#else
	m_customRenderer[RENDER_MAP](m_RenderGroup[RENDER_MAP]);
#endif

	return S_OK;
}

HRESULT CRenderer::_Render_Grass()
{
#ifdef _DEBUG
	if (m_bEnableRenderMap)
		m_customRenderer[RENDER_GRASS](m_RenderGroup[RENDER_GRASS]);
	else
	{
		for (auto& pObject : m_RenderGroup[RENDER_GRASS])
			Safe_Release(pObject);

		m_RenderGroup[RENDER_GRASS].clear();
	}

#else
	m_customRenderer[RENDER_GRASS](m_RenderGroup[RENDER_GRASS]);
#endif
	m_pRenderTarget_Manager->End_MRT(m_pContext);

	return S_OK;
}

HRESULT CRenderer::_Render_Player()
{
	auto pPlayerDepth = m_pRenderTarget_Manager->Find_RenderTarget(TEXT("Target_PlayerDepth"));
	pPlayerDepth->Clear();

	m_pRenderTarget_Manager->Begin_MRT_NoClear(m_pContext, TEXT("MRT_Player"));

	for (auto& pGameObject : m_RenderGroup[RENDER_PLAYER])
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Render();
			Add_RenderGroup(RENDERGROUP::RENDER_SHADOW, pGameObject);
		}

		Safe_Release(pGameObject);
	}

	m_RenderGroup[RENDER_PLAYER].clear();

	m_pRenderTarget_Manager->End_MRT(m_pContext);

	return S_OK;
}

HRESULT CRenderer::_Render_Character()
{
	// Character Depth Clear
	auto pCharacterDepth = m_pRenderTarget_Manager->Find_RenderTarget(TEXT("Target_CharacterDepth"));
	pCharacterDepth->Clear();

	m_pRenderTarget_Manager->Begin_MRT_NoClear(m_pContext, TEXT("MRT_Characters"));

	for (auto& pGameObject : m_RenderGroup[RENDER_CHARACTER])
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Render();
			Add_RenderGroup(RENDERGROUP::RENDER_SHADOW, pGameObject);
		}

		Safe_Release(pGameObject);
	}

	m_RenderGroup[RENDER_CHARACTER].clear();

	m_pRenderTarget_Manager->End_MRT(m_pContext);

	return S_OK;
}

HRESULT CRenderer::_Render_NonBlend()
{
	m_pRenderTarget_Manager->Begin_MRT_NoClear(m_pContext, TEXT("MRT_GameObjects"));

	for (auto& pGameObject : m_RenderGroup[RENDER_NONBLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderGroup[RENDER_NONBLEND].clear();

	m_pRenderTarget_Manager->End_MRT(m_pContext);

	return S_OK;
}

HRESULT CRenderer::_Render_Lensflare()
{
	if (m_RenderGroup[RENDER_LENSFLARE].empty())
	{
		m_pRenderTarget_Manager->Clear_RenderTarget(TEXT("Target_Lensflare"));
		return S_OK;
	}

	m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Lensflare"));

	for (auto& pGameObject : m_RenderGroup[RENDER_LENSFLARE])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderGroup[RENDER_LENSFLARE].clear();

	m_pRenderTarget_Manager->End_MRT(m_pContext);

	_RenderTarget_Blur_NPass(TEXT("Target_Lensflare"));

	return S_OK;
}

HRESULT CRenderer::_Render_SSAO()
{
	if (m_tRenderAttributes.bEnableSSAO)
	{
		if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResource(m_pSSAOShader, TEXT("Target_Depth"), "g_DepthTexture")))
			return E_FAIL;

		if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResource(m_pSSAOShader, TEXT("Target_Normal"), "g_NormalTexture")))
			return E_FAIL;

		if (FAILED(m_pRenderTarget_Manager->Bind_UnorderedAccess(m_pSSAOShader, TEXT("Target_SSAO"), "g_OutputTexture")))
			return E_FAIL;

		if (FAILED(m_pSSAOShader->Bind("g_fResX", CEngine_Global::fWinSizeX)))
			return E_FAIL;

		if (FAILED(m_pSSAOShader->Bind("g_fResY", CEngine_Global::fWinSizeY)))
			return E_FAIL;

		if (FAILED(m_pSSAOShader->Bind("g_fLength", m_tRenderAttributes.fSSAOLength)))//m_tRenderAttributes.fSSAOLength)))
			return E_FAIL;

		if (FAILED(m_pSSAOShader->Bind("g_fThreshold", m_tRenderAttributes.fSSAOThreshold)))//m_tRenderAttributes.fSSAOLength)))
			return E_FAIL;

		if (FAILED(m_pSSAOShader->Bind("g_fCameraFar", CPipeLine::Get_Instance()->Get_CameraFar())))
			return E_FAIL;

		_uint x = static_cast<_uint>(ceil(CEngine_Global::fWinSizeX / 32.f));
		_uint y = static_cast<_uint>(ceil(CEngine_Global::fWinSizeY / 32.f));

		m_pSSAOShader->Begin(COMPUTE_PASS_SSAO, x, y, 1);

		if (FAILED(_RenderTarget_Blur_NPass(TEXT("Target_SSAO"), m_tRenderAttributes.iSSAOBlurPass)))
			return E_FAIL;


		if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResource(m_pSSAOShader, TEXT("Target_Diffuse"), "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResource(m_pSSAOShader, TEXT("Target_SSAO"), "g_SSAOTexture")))
			return E_FAIL;

		if (FAILED(m_pRenderTarget_Manager->Bind_UnorderedAccess(m_pSSAOShader, TEXT("Target_Reserve"), "g_OutputTexture")))
			return E_FAIL;

		m_pSSAOShader->Begin(COMPUTE_PASS_SSAO_BIND, x, y, 1);

		if (FAILED(_Copy_SRV_To_SRV(TEXT("Target_Reserve"), TEXT("Target_Diffuse"))))
			return E_FAIL;
	}

	else
	{
		auto pSSAOTarget = m_pRenderTarget_Manager->Find_RenderTarget(TEXT("Target_SSAO"));

		if (nullptr == pSSAOTarget)
			return E_FAIL;

		else
			pSSAOTarget->Clear();
	}

	return S_OK;
}

// disabled
HRESULT CRenderer::_Render_SSLR()
{
	m_pRenderTarget_Manager->Clear_RenderTarget(TEXT("Target_SSLR_Occlusion"));
	m_pRenderTarget_Manager->Clear_RenderTarget(TEXT("Target_SSLR_Ray"));

	auto& lights = m_pLight_Manager->Get_Lights();

	if (lights.empty())
		return S_OK;

	if (m_tRenderAttributes.bEnableSSLR)
	{
		_uint iWidth = CEngine_Global::iWinSizeX / 2;
		_uint iHeight = CEngine_Global::iWinSizeY / 2;

		m_pSSLRShader->Bind("g_iWidth", iWidth);
		m_pSSLRShader->Bind("g_iHeight", iHeight);
		
		if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResource(m_pSSLRShader, TEXT("Target_Depth"), "g_InputDepthTexture")))
			return E_FAIL;

		if (FAILED(m_pRenderTarget_Manager->Bind_UnorderedAccess(m_pSSLRShader, TEXT("Target_Depth_Downscaled"), "g_OutputDepthTexture")))
			return E_FAIL;

		_uint iX = static_cast<_uint>(ceil((CEngine_Global::fWinSizeX * CEngine_Global::fWinSizeY / 4.f) / 1024.f));

		m_pSSLRShader->Begin(COMPUTE_PASS_SSLR_DOWNSCALE2X, iX, 1, 1);


		m_pSSLRShader->Bind("g_iWidth", iWidth);
		m_pSSLRShader->Bind("g_iHeight", iHeight);

		if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResource(m_pSSLRShader, TEXT("Target_Depth_Downscaled"), "g_InputTexture")))
			return E_FAIL;

		if (FAILED(m_pRenderTarget_Manager->Bind_UnorderedAccess(m_pSSLRShader, TEXT("Target_SSLR_Occlusion"), "g_OutputOcclusion")))
			return E_FAIL;

		m_pSSLRShader->Begin(COMPUTE_PASS_SSLR_OCCLUSION, iX, 1, 1);

		_uint iNumViewport = 1;
		D3D11_VIEWPORT tOriginViewport;
		D3D11_VIEWPORT tHalfViewport;
		m_pContext->RSGetViewports(&iNumViewport, &tOriginViewport);
		
		tHalfViewport = tOriginViewport;
		tHalfViewport.Width /= 2.f;
		tHalfViewport.Height /= 2.f;

		m_pContext->RSSetViewports(iNumViewport, &tHalfViewport);

		// 광선 세기 계산
		if (FAILED(m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_SSLR"), m_pSSLRDepthStencilView)))
			return E_FAIL;

		if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResource(m_pShader, TEXT("Target_SSLR_Occlusion"), "g_SSLRTexture")))
			return E_FAIL;

		auto matWorld = m_WorldMatrix_Deferred;
		matWorld._43 = 1.f;

		m_pShader->Bind("g_WorldMatrix", matWorld);
		m_pShader->Bind("g_ViewMatrix", m_ViewMatrix_Deferred);
		m_pShader->Bind("g_ProjMatrix", m_ProjMatrix_Deferred);

		// 태양의 texcoord에서의 위치가 필요함
		// init decay, dist decay 필요
		auto pLight = lights.front();
		_float4 vSunPosition = pLight->Get_LightDesc()->vPosition;
		auto vPos = XMLoadFloat4(&vSunPosition);

		auto matView = CPipeLine::Get_Instance()->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW);
		auto matProj = CPipeLine::Get_Instance()->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ);

		vPos = XMVector3TransformCoord(vPos, matView * matProj);
		vPos /= XMVectorGetW(vPos);

		XMStoreFloat4(&vSunPosition, vPos);
		vSunPosition.x = (vSunPosition.x + 1.f) * 0.5f;
		vSunPosition.y = (1.f - vSunPosition.y) * 0.5f;

		m_pShader->Bind("g_vLightPosition", vSunPosition);
		m_pShader->Bind("g_fInitDecay", 0.2f);
		m_pShader->Bind("g_fDistDecay", 0.8f);
		m_pShader->Bind("g_fMaxDeltaLength", 0.005f);

		m_pShader->Begin(DEFERRED_PASS_SSLR);
		m_pVIBuffer->Render();

		m_pRenderTarget_Manager->End_MRT(m_pContext);

		m_pContext->RSSetViewports(iNumViewport, &tOriginViewport);
	}

	return S_OK;
}

HRESULT CRenderer::_Render_Shadow()
{
	m_customRenderer[RENDER_SHADOW](m_RenderGroup[RENDER_SHADOW]);

	return S_OK;
}

HRESULT CRenderer::_Render_LightAcc()
{
	m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_LightAcc"));

	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResource(m_pShader, TEXT("Target_Normal"), "g_NormalTexture")))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResource(m_pShader, TEXT("Target_Depth"), "g_DepthTexture")))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix_Deferred)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix_Deferred)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix_Deferred)))
		return E_FAIL;

	CPipeLine* pPipeLine = CPipeLine::Get_Instance();

	_float4x4 matView = pPipeLine->Get_Transform_Float4x4_Inverse(CPipeLine::D3DTS_VIEW);
	_float4x4 matProj = pPipeLine->Get_Transform_Float4x4_Inverse(CPipeLine::D3DTS_PROJ);

	_float4 vCamPos = pPipeLine->Get_CamPosition();

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", &matView)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", &matProj)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", &vCamPos, sizeof(_float4))))
		return E_FAIL;

	m_pLight_Manager->Render_Lights(m_pShader, m_pVIBuffer);

	m_pRenderTarget_Manager->End_MRT(m_pContext);

	return S_OK;
}

HRESULT CRenderer::_Render_Deferred()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix_Deferred)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix_Deferred)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix_Deferred)))
		return E_FAIL;

	auto pPipeLine = CPipeLine::Get_Instance();

	auto matViewInv = pPipeLine->Get_Transform_Float4x4_Inverse(CPipeLine::D3DTS_VIEW);
	auto matProjInv = pPipeLine->Get_Transform_Float4x4_Inverse(CPipeLine::D3DTS_PROJ);
	auto matLightView = pPipeLine->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
	auto matLightProj = pPipeLine->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ);
	auto& lights = m_pLight_Manager->Get_Lights();
	auto iter = find_if(lights.begin(), lights.end(), [](CLight* _pLight) { return LIGHTDESC::TYPE_DIRECTION == _pLight->Get_LightDesc()->eType; });
	auto fNear = pPipeLine->Get_CameraNear();
	auto fFar = pPipeLine->Get_CameraFar();

	if (iter != lights.end())
	{
		matLightView = (*iter)->Get_ViewMatrix();
		matLightProj = (*iter)->Get_ProjMatrix();

		m_pShader->Bind("g_vLightPosition", (*iter)->Get_LightDesc()->vPosition);
		m_pShader->Bind("g_vLightDirection", (*iter)->Get_LightDesc()->vDirection);
	}

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", &matViewInv)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", &matProjInv)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_LightViewMatrix", &matLightView)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_LightProjMatrix", &matLightProj)))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResource(m_pShader, TEXT("Target_Diffuse"), "g_DiffuseTexture")))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResource(m_pShader, TEXT("Target_Depth"), "g_DepthTexture")))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResource(m_pShader, TEXT("Target_Shade"), "g_ShadeTexture")))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResource(m_pShader, TEXT("Target_Specular"), "g_SpecularTexture")))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResource(m_pShader, TEXT("Target_Emissive"), "g_EmissiveTexture")))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResource(m_pShader, TEXT("Target_ShadowDepth"), "g_ShadowDepthTexture")))
		return E_FAIL;

	if (FAILED(m_pShader->Bind("g_fCameraNear", fNear)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind("g_fCameraFar", fFar)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind("g_fShadowBias", m_fShadowBias)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind("g_fWidth", SHADOWDEPTH_RESOLUTION)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind("g_fHeight", SHADOWDEPTH_RESOLUTION)))
		return E_FAIL; 

	m_pShader->Begin(DEFERRED_PASS_BLEND);
	m_pVIBuffer->Render();

	return S_OK;
}


HRESULT CRenderer::_Render_NonLight()
{
	for (auto& pGameObject : m_RenderGroup[RENDER_NONLIGHT])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderGroup[RENDER_NONLIGHT].clear();

	return S_OK;
}

HRESULT CRenderer::_Render_Blend()
{
	m_RenderGroup[RENDER_BLEND].sort([](CGameObject* pSour, CGameObject* pDest)->_bool
		{
			return ((CBlendObject*)pSour)->Get_CamDistance() > ((CBlendObject*)pDest)->Get_CamDistance();
		});

	for (auto& pGameObject : m_RenderGroup[RENDER_BLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderGroup[RENDER_BLEND].clear();

	return S_OK;
}

HRESULT CRenderer::_Render_VFX()
{
	if (FAILED(m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_VFX"))))
		return E_FAIL;

	for (auto& pGameObject : m_RenderGroup[RENDER_VFX])
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Render();
		}

		Safe_Release(pGameObject);
	}

	m_RenderGroup[RENDER_VFX].clear();

	m_pRenderTarget_Manager->End_MRT(m_pContext);

	return S_OK;
}

HRESULT CRenderer::_Render_Glow()
{
	// 블룸 셰이더를 이용하는데, 대신에 평균 휘도값을 사용하지 않고 무조건 올린다
	static _bool bIsFirstTime = true;

	if (m_tRenderAttributes.bEnableGlow)
	{
		if (bIsFirstTime)
		{
			bIsFirstTime = false;
			return S_OK;
		}

		auto& objectList = m_RenderGroup[RENDERGROUP::RENDER_GLOW];

		if (FAILED(m_pRenderTarget_Manager->Clear_RenderTarget(TEXT("Target_Glow"))))
			return E_FAIL;

		if (FAILED(m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Reserve"))))
			return E_FAIL;

		for (auto& pGameObject : objectList)
		{
			if (nullptr != pGameObject)
				pGameObject->Render();

			Safe_Release(pGameObject);
		}

		objectList.clear();

		m_pRenderTarget_Manager->End_MRT(m_pContext);

		// 블룸 수행. 단, 평균 휘도값을 사용하지 않음
		if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResource(m_pBloomShader, TEXT("Target_Reserve"), "g_InputTexture")))
			return E_FAIL;

		if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResource(m_pBloomShader, TEXT("Target_Emissive"), "g_EmissiveTexture")))
			return E_FAIL;

		if (FAILED(m_pBloomShader->Bind("g_fGlowAverageLuminance", m_tRenderAttributes.fGlowAverageLuminance)))
			return E_FAIL;

		if (FAILED(m_pRenderTarget_Manager->Bind_UnorderedAccess(m_pBloomShader, TEXT("Target_Glow"), "g_OutputTexture")))
			return E_FAIL;

		if (FAILED(m_pBloomShader->Bind("g_iResWidth", CEngine_Global::iWinSizeX)))
			return E_FAIL;

		if (FAILED(m_pBloomShader->Bind("g_iResHeight", CEngine_Global::iWinSizeY)))
			return E_FAIL;

		if (FAILED(m_pBloomShader->Bind("g_fGlowThreshold", m_tRenderAttributes.fGlowThreshold)))
			return E_FAIL;

		_uint x = _uint(ceil(CEngine_Global::fWinSizeX / 1024.f));
		_uint y = CEngine_Global::iWinSizeY;

		m_pBloomShader->Begin(COMPUTE_PASS_GLOW, x, y, 1);

		if (FAILED(_RenderTarget_Blur_NPass(TEXT("Target_Glow"))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CRenderer::_Render_OutLine()
{
	// Depth, CharacterDepth => Input SRV
	// Sobel target => Output UAV

	// 두 개의 뎁스 타겟을 비교해서 Depth.r >= CharacterDepth.x이면 소벨 필터링 결과를 적용한다

	if (m_tRenderAttributes.bEnableEdgeHighlight)
	{
		if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResource(m_pShader, TEXT("Target_Depth"), "g_DepthTexture")))
			return E_FAIL;

		if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResource(m_pShader, TEXT("Target_PlayerDepth"), "g_CharacterDepthTexture")))
			return E_FAIL;

		if (FAILED(m_pShader->Bind("g_fEdgeHighlightThickness", m_tRenderAttributes.fEdgeHighlightThickness)))
			return E_FAIL;

		if (FAILED(m_pShader->Bind("g_vEdgeHighlightColor", m_tRenderAttributes.vEdgeHighlightColor)))
			return E_FAIL;

		if (FAILED(_Bind_BaseInfo(m_pShader)))
			return E_FAIL;

		m_pShader->Begin(DEFERRED_PASS_OUTLINE_SOBEL);
		m_pVIBuffer->Render();
	}

	return S_OK;
}

HRESULT CRenderer::_Render_Bloom()
{
	static _bool bIsFirstTime = true;

	if (bIsFirstTime)
	{
		bIsFirstTime = false;
		return S_OK;
	}

	auto& objectList = m_RenderGroup[RENDERGROUP::RENDER_BLOOM];

	if (0 >= objectList.size())
		return S_OK;

	_float fBloomScale = m_tRenderAttributes.fBloomScale;

	for (auto& pGameObject : objectList)
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Render();
		}

		Safe_Release(pGameObject);
	}

	objectList.clear();

	m_pRenderTarget_Manager->End_MRT(m_pContext);

	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResource(m_pBloomShader, TEXT("Target_BackBuffer"), "g_InputTexture")))
		return E_FAIL;

	if (FAILED(m_pBloomShader->Bind("g_AverageLuminance", m_resources["HDR_SecondPass"]->Get_SRV())))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Bind_UnorderedAccess(m_pBloomShader, TEXT("Target_Bloom"), "g_OutputTexture")))
		return E_FAIL;

	if (FAILED(m_pBloomShader->Bind("g_iResWidth", CEngine_Global::iWinSizeX)))
		return E_FAIL;

	if (FAILED(m_pBloomShader->Bind("g_iResHeight", CEngine_Global::iWinSizeY)))
		return E_FAIL;

	if (FAILED(m_pBloomShader->Bind("g_fBloomThreshold", m_tRenderAttributes.fBloomThreshold)))
		return E_FAIL;

	_uint x = _uint(ceil(CEngine_Global::fWinSizeX / 1024.f));
	_uint y = CEngine_Global::iWinSizeY;

	m_pBloomShader->Begin(COMPUTE_PASS_BLOOM, x, y, 1);

	if (FAILED(_RenderTarget_Blur_NPass(TEXT("Target_Bloom"), 1, true)))
		return E_FAIL;

	_Copy_SRV_To_SRV(TEXT("Target_BackBuffer"), TEXT("Target_Reserve"));

	m_pRenderTarget_Manager->Begin_MRT_NoClear(m_pContext, TEXT("MRT_BackBuffer"));

	m_pRenderTarget_Manager->Bind_ShaderResource(m_pShader, TEXT("Target_Reserve"), "g_Texture");
	m_pRenderTarget_Manager->Bind_ShaderResource(m_pShader, TEXT("Target_Bloom"), "g_BloomTexture");

	_Bind_BaseInfo(m_pShader);

	m_pShader->Begin(DEFERRED_PASS_BLOOM);
	m_pVIBuffer->Render();

	m_tRenderAttributes.fBloomScale = fBloomScale;

	return S_OK;
}

HRESULT CRenderer::_Render_Blur()
{
	return S_OK;
}

HRESULT CRenderer::_Render_Distortion()
{
	auto iDrawCount = m_RenderGroup[RENDER_DISTORTION].size();

	if (0 >= iDrawCount)
		return S_OK;

	if (FAILED(m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Distortion"))))
		return E_FAIL;

	for (auto& pGameObject : m_RenderGroup[RENDER_DISTORTION])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderGroup[RENDER_DISTORTION].clear();

	if (FAILED(m_pRenderTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	m_pRenderTarget_Manager->Clear_RenderTarget(TEXT("Target_Reserve"));

	return S_OK;
}

HRESULT CRenderer::_Render_PostProcessing(_float _fTimeDelta)
{
	_Post_Processing_Distortion();
	_Post_Processing_Fog(_fTimeDelta); // 안개 적용
	_Post_Processing_HDR(_fTimeDelta); // HDR 선적용. Target_HDR로 출력
	_Post_Processing_Bloom(); // 블룸 적용

	if (FAILED(m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_HDR"))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix_Deferred)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix_Deferred)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix_Deferred)))
		return E_FAIL;

	if (FAILED(m_resources["HDR_SecondPass"]->Bind_SRV(m_pShader, "g_AverageLumen")))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResource(m_pShader, TEXT("Target_BackBuffer"), "g_Texture")))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResource(m_pShader, TEXT("Target_Bloom"), "g_BloomTexture")))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResource(m_pShader, TEXT("Target_Glow"), "g_GlowTexture")))
		return E_FAIL;

	if (FAILED(m_pShader->Bind("g_fBloomScale", m_tRenderAttributes.fBloomScale)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind("g_fGlowScale", m_tRenderAttributes.fGlowScale)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind("g_fMiddleGrey", m_tRenderAttributes.fMiddleGrey)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind("g_fLumWhiteSqr", m_tRenderAttributes.fLuminanceWhite)))
		return E_FAIL;

	m_pShader->Begin(DEFERRED_PASS_HDR);
	m_pVIBuffer->Render();

	if (FAILED(m_pRenderTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	if (m_tRenderAttributes.bEnableSSAO)
	{
		if (FAILED(_RenderTarget_Blur_NPass(TEXT("Target_SSAO"))))
			return E_FAIL;
	}
	
	if (FAILED(m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_BackBuffer"))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix_Deferred)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix_Deferred)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix_Deferred)))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResource(m_pShader, TEXT("Target_HDR"), "g_HDRTexture")))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResource(m_pShader, TEXT("Target_VFX"), "g_VFXTexture")))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResource(m_pShader, TEXT("Target_Lensflare"), "g_LensflareTexture")))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResource(m_pShader, TEXT("Target_SSLR_Ray"), "g_SSLRTexture")))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResource(m_pShader, TEXT("Target_VFXDepth"), "g_VFXDepthTexture")))
		return E_FAIL;

	m_pShader->Begin(DEFERRED_PASS_POSTPROCESSING);
	m_pVIBuffer->Render();

	if (FAILED(m_pRenderTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	_Post_Processing_DOF();

	if (FAILED(_Render_To_BackBuffer(TEXT("Target_BackBuffer"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::_Post_Processing_Distortion()
{
	if (FAILED(_Copy_SRV_To_SRV(TEXT("Target_BackBuffer"), TEXT("Target_Reserve"))))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_BackBuffer"))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix_Deferred)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix_Deferred)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix_Deferred)))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResource(m_pShader, TEXT("Target_Distortion"), "g_DistortionTexture")))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResource(m_pShader, TEXT("Target_Reserve"), "g_BackBufferTexture")))
		return E_FAIL;

	m_pShader->Begin(DEFERRED_PASS_DISTORTION);
	m_pVIBuffer->Render();

	if (FAILED(m_pRenderTarget_Manager->End_MRT(m_pContext)))
		return E_FAIL;

	auto pDistortion = m_pRenderTarget_Manager->Find_RenderTarget(TEXT("Target_Distortion"));

	if (nullptr == pDistortion)
		return E_FAIL;

	pDistortion->Clear();

	return S_OK;
}

HRESULT CRenderer::_Post_Processing_HDR(_float _fTimeDelta)
{
	/*
	Target_HDR_First
	Target_HDR_Second
	Target_HDR_ToneMapping
	*/

	/*
	First Pass
	- 16픽셀 그룹을 하나의 픽셀로 줄임
	- 1024에서 4로 다운 스케일
	- 4에서 1로 다운 스케일
	*/

	if (m_tRenderAttributes.bEnableHDR)
	{
		auto pFirstPass = m_resources["HDR_FirstPass"];
		auto pSecondPass = m_resources["HDR_SecondPass"];
		auto pPrevAverageLumen = m_resources["HDR_PrevAverageLumen"];

		static _bool bFirstTime = true;

		if (bFirstTime)
		{
			m_tRenderAttributes.fAdaptationCoefficient = 0.f;
			bFirstTime = false;
		}

		else
		{
			const auto& T = m_tRenderAttributes.fAdaptationElapsedTime;

			if (m_fAdaptedProgressTime >= T)
				m_fAdaptedProgressTime = 0.f;

			m_tRenderAttributes.fAdaptationCoefficient =
				min(T < 0.0001f ? T : m_fAdaptedProgressTime / T, T - 0.0001f);
		}

		m_fAdaptedProgressTime += _fTimeDelta;


		if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResource(m_pHDRShader, TEXT("Target_BackBuffer"), "g_InputTexture")))
			return E_FAIL;

		// 백버퍼 너비, 높이를 4로 나눈 값
		_int iWidth = CEngine_Global::iWinSizeX / 4;
		_int iHeight = CEngine_Global::iWinSizeY / 4;

		if (FAILED(m_pHDRShader->Bind("g_iWidth", iWidth)))
			return E_FAIL;

		if (FAILED(m_pHDRShader->Bind("g_iHeight", iHeight)))
			return E_FAIL;

		_int iDomain = CEngine_Global::iWinSizeX * CEngine_Global::iWinSizeY / 16;
		_int iGroupSize = iDomain * 1024;

		if (FAILED(m_pHDRShader->Bind("g_iDomain", iDomain)))
			return E_FAIL;

		if (FAILED(pFirstPass->Bind_UAV(m_pHDRShader, "g_AverageLumenFinal")))
			return E_FAIL;

		if (FAILED(m_pRenderTarget_Manager->Bind_UnorderedAccess(m_pHDRShader, TEXT("Target_HDR_DownScaled"), "g_OutputTexture")))
			return E_FAIL;

		_uint iX = static_cast<_uint>(ceil(CEngine_Global::fWinSizeX * CEngine_Global::fWinSizeY / 16.f) / 1024.f);

		m_pHDRShader->Begin(COMPUTE_PASS_HDR_DOWNSCALE_FIRST, iX, 1, 1);

		if (FAILED(pFirstPass->Bind_SRV(m_pHDRShader, "g_AverageValues1D")))
			return E_FAIL;

		if (FAILED(pPrevAverageLumen->Bind_UAV(m_pHDRShader, "g_PrevAverageLumen")))
			return E_FAIL;

		if (FAILED(pSecondPass->Bind_UAV(m_pHDRShader, "g_AverageLumenFinal")))
			return E_FAIL;

		if (FAILED(m_pHDRShader->Bind("g_bEnableAdaptation", m_tRenderAttributes.bEnableAdaptation)))
			return E_FAIL;

		if (FAILED(m_pHDRShader->Bind("g_fAdaptation", m_tRenderAttributes.fAdaptationCoefficient)))
			return E_FAIL;

		if (FAILED(m_pHDRShader->Bind("g_iGroupSize", iGroupSize)))
			return E_FAIL;

		if (FAILED(m_pHDRShader->Bind("g_vAverageLuminanceRange", m_tRenderAttributes.vAverageLuminanceRange)))
			return E_FAIL;

		m_pHDRShader->Begin(COMPUTE_PASS_HDR_DOWNSCALE_SECOND, 1, 1, 1);

		pSecondPass->Swap(pPrevAverageLumen);
	}

	return S_OK;
}

HRESULT CRenderer::_Post_Processing_Bloom()
{
	if (m_tRenderAttributes.bEnableBloom)
	{
		_uint iWidth = CEngine_Global::iWinSizeX / 4;
		_uint iHeight = CEngine_Global::iWinSizeY / 4;

		m_pBloomShader->Bind("g_iResWidth", iWidth);
		m_pBloomShader->Bind("g_iResHeight", iHeight);

		_uint iX = static_cast<_uint>(ceil((CEngine_Global::fWinSizeX * CEngine_Global::fWinSizeY / 16.f) / 1024.f));

		if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResource(m_pBloomShader, TEXT("Target_Depth"), "g_InputTexture")))
			return E_FAIL;

		if (FAILED(m_pRenderTarget_Manager->Bind_UnorderedAccess(m_pBloomShader, TEXT("Target_Depth_Downscaled_4X"), "g_OutputDepthTexture")))
			return E_FAIL;

		m_pBloomShader->Begin(COMPUTE_PASS_BLOOM_DEPTH_DOWNSCALE_4X, iX, 1, 1);
		m_pBloomShader->Clear_CS();


		if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResource(m_pBloomShader, TEXT("Target_HDR_DownScaled"), "g_InputTexture")))
			return E_FAIL;

		if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResource(m_pBloomShader, TEXT("Target_Depth_Downscaled_4X"), "g_InputDepthTexture")))
			return E_FAIL;

		if (FAILED(m_pRenderTarget_Manager->Bind_UnorderedAccess(m_pBloomShader, TEXT("Target_Bloom"), "g_OutputTexture")))
			return E_FAIL;

		if (FAILED(m_pBloomShader->Bind("g_AverageLuminance", m_resources["HDR_SecondPass"]->Get_SRV())))
			return E_FAIL;

		if (FAILED(m_pBloomShader->Bind("g_iResWidth", CEngine_Global::iWinSizeX / 4.f)))
			return E_FAIL;

		if (FAILED(m_pBloomShader->Bind("g_iResHeight", CEngine_Global::iWinSizeY / 4.f)))
			return E_FAIL;

		if (FAILED(m_pBloomShader->Bind("g_fBloomThreshold", m_tRenderAttributes.fBloomThreshold)))
			return E_FAIL;

		auto fCameraFar = CGameInstance::Get_Instance()->Get_CameraFar();
		auto fDepthThreshold = m_tRenderAttributes.bEnableFog ? m_tRenderAttributes.fFogEnd / fCameraFar : 0.99f;

		if (FAILED(m_pBloomShader->Bind("g_fDepthThreshold", fDepthThreshold)))
			return E_FAIL;

		_uint x = _uint(ceil(CEngine_Global::fWinSizeX * CEngine_Global::fWinSizeY / 16.f / 1024.f));

		m_pBloomShader->Begin(COMPUTE_PASS_BLOOM, x, 1, 1);

		if (FAILED(_RenderTarget_Blur_NPass(TEXT("Target_Bloom"), 3, true)))
			return E_FAIL;
	}

	else
	{
		auto pBloomTarget = m_pRenderTarget_Manager->Find_RenderTarget(TEXT("Target_Bloom"));
		
		if (nullptr == pBloomTarget)
			return E_FAIL;

		else
			pBloomTarget->Clear();
	}

	return S_OK;
}

HRESULT CRenderer::_Post_Processing_Blur()
{
	return S_OK;
}

HRESULT CRenderer::_Post_Processing_DOF()
{
	if (m_tRenderAttributes.bEnableDepthOfField)
	{
		if (FAILED(_Copy_SRV_To_SRV(TEXT("Target_BackBuffer"), TEXT("Target_DOF"))))
			return E_FAIL;

		if (FAILED(_Copy_SRV_To_SRV(TEXT("Target_BackBuffer"), TEXT("Target_Reserve"))))
			return E_FAIL;

		if (FAILED(m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_BackBuffer"))))
			return E_FAIL;

		if (FAILED(_RenderTarget_Blur_NPass(TEXT("Target_Reserve"), 3)))
			return E_FAIL;

		if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResource(m_pShader, TEXT("Target_Depth"), "g_DepthTexture")))
			return E_FAIL;

		if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResource(m_pShader, TEXT("Target_DOF"), "g_Texture"))) // Sharp
			return E_FAIL;

		if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResource(m_pShader, TEXT("Target_Reserve"), "g_BlurTexture"))) // Blur
			return E_FAIL;

		if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResource(m_pShader, TEXT("Target_VFXDepth"), "g_VFXDepthTexture")))
			return E_FAIL;

		auto fCameraFar = CGameInstance::Get_Instance()->Get_CameraFar();
		auto fDepthThreshold = m_tRenderAttributes.bEnableFog ? m_tRenderAttributes.fFogEnd / fCameraFar : 0.99f;

		m_pShader->Bind("g_fFocus", m_tRenderAttributes.fFocus);
		m_pShader->Bind("g_fFocusRange", m_tRenderAttributes.fFocusRange);
		m_pShader->Bind("g_fCameraFar", fCameraFar);
		m_pShader->Bind("g_fDepthThreshold", fDepthThreshold);

		m_pShader->Begin(DEFERRED_PASS_DEPTH_OF_FIELD);
		m_pVIBuffer->Render();

		m_pRenderTarget_Manager->End_MRT(m_pContext);
	}

	return S_OK;
}

HRESULT CRenderer::_Post_Processing_Fog(_float _fTimeDelta)
{
	if (m_tRenderAttributes.bEnableFog)
	{
		if (FAILED(_Copy_SRV_To_SRV(TEXT("Target_BackBuffer"), TEXT("Target_Reserve"))))
			return E_FAIL;

		if (FAILED(m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_BackBuffer"))))
			return E_FAIL;

		if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResource(m_pShader, TEXT("Target_Depth"), "g_DepthTexture")))
			return E_FAIL;

		if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResource(m_pShader, TEXT("Target_Reserve"), "g_Texture")))
			return E_FAIL;

		auto matViewInv = CPipeLine::Get_Instance()->Get_Transform_Float4x4_Inverse(CPipeLine::D3DTS_VIEW);
		auto matProjInv = CPipeLine::Get_Instance()->Get_Transform_Float4x4_Inverse(CPipeLine::D3DTS_PROJ);

		m_pShader->Bind("g_fFogStart", m_tRenderAttributes.fFogStart);
		m_pShader->Bind("g_fFogEnd", m_tRenderAttributes.fFogEnd);
		m_pShader->Bind("g_vFogColor", m_tRenderAttributes.vFogColor);
		m_pShader->Bind("g_fCameraFar", CPipeLine::Get_Instance()->Get_CameraFar());

		m_pShader->Begin(DEFERRED_PASS_FOG);
		m_pVIBuffer->Render();

		m_pRenderTarget_Manager->End_MRT(m_pContext);
	}

	return S_OK;
}

HRESULT CRenderer::_Render_Static_UI()
{
	m_pContext->ClearDepthStencilView(m_pUIDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	if (FAILED(m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_UI_Static"), m_pUIDepthStencilView)))
	{
		MSG_BOX("Failed to Begin MRT");
		return E_FAIL;
	}

	m_RenderGroup[RENDER_STATIC_UI].sort([](CGameObject* _pFirst, CGameObject* _pSecond)->_bool {
		return _pFirst->Get_RenderPrioritySameLevel() < _pSecond->Get_RenderPrioritySameLevel(); });

	for (auto& pGameObject : m_RenderGroup[RENDER_STATIC_UI])
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Render();
		}

		Safe_Release(pGameObject);
	}

	m_RenderGroup[RENDER_STATIC_UI].clear();

	if (FAILED(m_pRenderTarget_Manager->End_MRT(m_pContext)))
	{
		MSG_BOX("Failed to End MRT");
		return E_FAIL;
	}

	if (FAILED(m_pShader_UI_Static->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix_Deferred)))
		return E_FAIL;

	if (FAILED(m_pShader_UI_Static->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix_Deferred)))
		return E_FAIL;

	if (FAILED(m_pShader_UI_Static->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix_Deferred)))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResource(m_pShader_UI_Static, TEXT("Target_UI_Static"), "g_Texture")))
		return E_FAIL;

	m_pShader_UI_Static->Begin(0);
	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CRenderer::_Render_UI()
{
	m_RenderGroup[RENDER_UI].sort([](CGameObject* _pFirst, CGameObject* _pSecond)->_bool {
		return _pFirst->Get_RenderPrioritySameLevel() < _pSecond->Get_RenderPrioritySameLevel(); });

	for (auto& pGameObject : m_RenderGroup[RENDER_UI])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderGroup[RENDER_UI].clear();

	return S_OK;
}

HRESULT CRenderer::_Render_Font()
{
	m_pContext->ClearDepthStencilView(m_pUIDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	if (FAILED(m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_UI_Font"), m_pUIDepthStencilView)))
	{
		MSG_BOX("Failed to Begin MRT");
		return E_FAIL;
	}

	m_RenderGroup[RENDER_FONT].sort([](CGameObject* _pFirst, CGameObject* _pSecond)->_bool {
		return _pFirst->Get_RenderPrioritySameLevel() < _pSecond->Get_RenderPrioritySameLevel(); });

	for (auto& pGameObject : m_RenderGroup[RENDER_FONT])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderGroup[RENDER_FONT].clear();

	if (FAILED(m_pRenderTarget_Manager->End_MRT(m_pContext)))
	{
		MSG_BOX("Failed to End MRT");
		return E_FAIL;
	}

	if (FAILED(m_pShader_UI_Static->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix_Deferred)))
		return E_FAIL;

	if (FAILED(m_pShader_UI_Static->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix_Deferred)))
		return E_FAIL;

	if (FAILED(m_pShader_UI_Static->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix_Deferred)))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResource(m_pShader_UI_Static, TEXT("Target_UI_Font"), "g_Texture")))
		return E_FAIL;

	m_pShader_UI_Static->Begin(0);
	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CRenderer::_Render_Final_Combined(_float fTimeDelta)
{
	auto& fProgressTime = m_tRenderAttributes.fFadeProgressTime;
	auto& fDestTime = m_tRenderAttributes.fFadeTime;
	auto& fValue = m_tRenderAttributes.fFadeValue;
	auto& fAlpha = m_tRenderAttributes.fFadeAlpha;

	fProgressTime = clamp(fProgressTime + fTimeDelta, 0.f, fDestTime);

	auto fRate = fProgressTime / fDestTime;
	fAlpha = abs(fValue - fRate);

	_Render_To_BackBuffer(TEXT("Target_Reserve"), true, m_tRenderAttributes.fFadeAlpha);

	return S_OK;
}

HRESULT CRenderer::_Render_To_BackBuffer(const wstring& _wstrRenderTargetTag, _bool _bUsePointSampler, _float _fFadeAlpha)
{
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix_Deferred)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix_Deferred)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix_Deferred)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind("g_fFadeAlpha", _fFadeAlpha)))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResource(m_pShader, _wstrRenderTargetTag, "g_Texture")))
		return E_FAIL;

	if (_bUsePointSampler)
		m_pShader->Begin(DEFERRED_PASS_DEFAULT);

	else
		m_pShader->Begin(DEFERRED_PASS_DEFAULT_LINEAR);

	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CRenderer::_Render_To_BackBuffer_Alpha_Discard(const wstring& _wstrRenderTargetTag, _float _fAlpha)
{
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix_Deferred)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix_Deferred)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix_Deferred)))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResource(m_pShader, _wstrRenderTargetTag, "g_Texture")))
		return E_FAIL;

	if (FAILED(m_pShader->Bind("g_fAlphaDiscard", _fAlpha)))
		return E_FAIL;

	m_pShader->Begin(DEFERRED_PASS_DEFAULT_ALPHA_DISCARD);
	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CRenderer::_Render_To_BackBuffer_AlphaBlend(const wstring& _wstrRenderTargetTag, _bool _bUsePointSampler, _float _fFadeAlpha)
{
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix_Deferred)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix_Deferred)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix_Deferred)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind("g_fFadeAlpha", _fFadeAlpha)))
		return E_FAIL;

	if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResource(m_pShader, _wstrRenderTargetTag, "g_Texture")))
		return E_FAIL;

	m_pShader->Begin(DEFERRED_PASS_DEFAULT_ALPHABLEND);
	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CRenderer::_Render_Debug()
{
	if (m_bEnableClearDepthStencilView_ForDebug)
		CGraphic_Device::Get_Instance()->Clear_DepthStencil_View();

	for (auto& pDebugCom : m_DebugComponents)
	{
		if (nullptr == pDebugCom)
			continue;

		pDebugCom->Render();

		Safe_Release(pDebugCom);
	}
	m_DebugComponents.clear();

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix_Deferred)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix_Deferred)))
		return E_FAIL;

	if (m_bDebugRender)
	{
		m_pRenderTarget_Manager->Debug_Render(m_pShader, m_pVIBuffer, TEXT("MRT_Player"));
		m_pRenderTarget_Manager->Debug_Render(m_pShader, m_pVIBuffer, TEXT("MRT_LightAcc"));
		m_pRenderTarget_Manager->Debug_Render(m_pShader, m_pVIBuffer, TEXT("MRT_VFX"));
	}

	return S_OK;
}

#ifdef _DEBUG
_float CRenderer::_Get_AverageLuminance()
{
	D3D11_MAPPED_SUBRESOURCE subResource;
	ZeroMemory(&subResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	auto pBuffer = m_resources["HDR_SecondPass"]->Get_Buffer();

	m_pContext->Map(pBuffer, 0, D3D11_MAP_READ, 0, &subResource);
	
	auto fAverageLuminance = static_cast<_float*>(subResource.pData)[0];

	m_pContext->Unmap(pBuffer, 0);

	return fAverageLuminance;
}
#endif

HRESULT CRenderer::_Copy_SRV_To_UAV(const wstring& _strSRVtag, const wstring& _strUAVtag)
{
	auto pSRVTarget = m_pRenderTarget_Manager->Find_RenderTarget(_strSRVtag);
	auto pUAVTarget = m_pRenderTarget_Manager->Find_RenderTarget(_strUAVtag);

	if (nullptr == pSRVTarget)
		return E_FAIL;

	if (nullptr == pUAVTarget)
		return E_FAIL;

	ID3D11Resource* pSRVResource = nullptr;
	ID3D11Resource* pUAVResource = nullptr;

	pSRVTarget->Get_RTV()->GetResource(&pSRVResource);
	pUAVTarget->Get_UAV()->GetResource(&pUAVResource);

	Safe_Release(pSRVResource);
	Safe_Release(pUAVResource);

	m_pContext->CopyResource(pUAVResource, pSRVResource);

	return S_OK;
}

HRESULT CRenderer::_Copy_SRV_To_SRV(const wstring& _strSourceTag, const wstring& _strDestTag)
{
	auto pSourceTarget = m_pRenderTarget_Manager->Find_RenderTarget(_strSourceTag);
	auto pDestTarget = m_pRenderTarget_Manager->Find_RenderTarget(_strDestTag);

	if (nullptr == pSourceTarget)
		return E_FAIL;

	if (nullptr == pDestTarget)
		return E_FAIL;

	ID3D11Resource* pSourceResource = nullptr;
	ID3D11Resource* pDestResource = nullptr;

	pSourceTarget->Get_RTV()->GetResource(&pSourceResource);
	pDestTarget->Get_RTV()->GetResource(&pDestResource);

	Safe_Release(pSourceResource);
	Safe_Release(pDestResource);

	m_pContext->CopyResource(pDestResource, pSourceResource);

	return S_OK;
}

HRESULT CRenderer::_Blur_NPass(_uint _iPassCount)
{
	return S_OK;
}

HRESULT CRenderer::_RenderTarget_Blur_NPass(const wstring& _strTag, _uint _iPassCount, _bool _bDownScaled)
{
	/*
	원본 텍스쳐인 경우에는 다운 스케일링을 한번 해야함
	*/

	if (!_bDownScaled)
	{
		m_pBlurShader->Bind("g_iWidth", CEngine_Global::iWinSizeX / 4);
		m_pBlurShader->Bind("g_iHeight", CEngine_Global::iWinSizeY / 4);

		if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResource(m_pBlurShader, _strTag, "g_InputTexture")))
			return E_FAIL;

		if (FAILED(m_pRenderTarget_Manager->Bind_UnorderedAccess(m_pBlurShader, TEXT("Target_Blur_Vertical"), "g_OutputTexture")))
			return E_FAIL;

		_uint iX = static_cast<_uint>(ceil(CEngine_Global::fWinSizeX * CEngine_Global::fWinSizeY / 16.f) / 1024.f);

		m_pBlurShader->Begin(COMPUTE_PASS_BLUR_DOWNSCALE4X, iX, 1, 1);

		m_pBlurShader->Clear_CS();
	}

	for (_uint i = 0; i < _iPassCount; ++i)
	{
		m_pBlurShader->Bind("g_iWidth", CEngine_Global::iWinSizeX / 4);
		m_pBlurShader->Bind("g_iHeight", CEngine_Global::iWinSizeY / 4);

		if (_bDownScaled)
		{
			if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResource(m_pBlurShader, _strTag, "g_InputTexture")))
				return E_FAIL;
		}

		else
		{
			if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResource(m_pBlurShader, TEXT("Target_Blur_Vertical"), "g_InputTexture")))
				return E_FAIL;
		}

		if (FAILED(m_pRenderTarget_Manager->Bind_UnorderedAccess(m_pBlurShader, TEXT("Target_Blur_Horizontal"), "g_OutputTexture")))
			return E_FAIL;

		_float fX = CEngine_Global::fWinSizeX / 4.f;
		_float fY = CEngine_Global::fWinSizeY / 4.f;

		_uint x = static_cast<_uint>(ceil(fX / (128.f - 12.f)));
		_uint y = static_cast<_uint>(ceil(fY));

		m_pBlurShader->Begin(COMPUTE_PASS_BLUR_HORIZONTAL, x, y, 1);


		m_pBlurShader->Bind("g_iWidth", CEngine_Global::iWinSizeX / 4);
		m_pBlurShader->Bind("g_iHeight", CEngine_Global::iWinSizeY / 4);

		fX = CEngine_Global::fWinSizeX / 4.f;
		fY = CEngine_Global::fWinSizeY / 4.f;

		x = static_cast<_uint>(ceil(fX));
		y = static_cast<_uint>(ceil(fY / (128.f - 12.f)));

		if (FAILED(m_pRenderTarget_Manager->Bind_ShaderResource(m_pBlurShader, TEXT("Target_Blur_Horizontal"), "g_InputTexture")))
			return E_FAIL;

		if (FAILED(m_pRenderTarget_Manager->Bind_UnorderedAccess(m_pBlurShader, TEXT("Target_Blur_Vertical"), "g_OutputTexture")))
			return E_FAIL;

		m_pBlurShader->Begin(COMPUTE_PASS_BLUR_VERTICAL, x, y, 1);

		// 다운 스케일 된 텍스쳐 또는 원본 텍스쳐 두개의 선택지를 제공
		if (_bDownScaled)
		{
			if (FAILED(_Copy_SRV_To_SRV(TEXT("Target_Blur_Vertical"), _strTag)))
				return E_FAIL;
		}
		else
		{
			if (FAILED(m_pRenderTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Blur_Temp"))))
				return E_FAIL;

			_Render_To_BackBuffer(TEXT("Target_Blur_Vertical"), false);

			m_pRenderTarget_Manager->End_MRT(m_pContext);

			if (FAILED(_Copy_SRV_To_SRV(TEXT("Target_Blur_Temp"), _strTag)))
				return E_FAIL;
		}

		m_pShader->Clear_PS();
		m_pBlurShader->Clear_CS();

		m_pRenderTarget_Manager->Clear_RenderTarget(TEXT("Target_Blur_Vertical"));
	}

	return S_OK;
}

HRESULT CRenderer::_Bind_BaseInfo(CShader* _pShader)
{
	if (FAILED(_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix_Deferred)))
		return E_FAIL;

	if (FAILED(_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix_Deferred)))
		return E_FAIL;

	if (FAILED(_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix_Deferred)))
		return E_FAIL;

	return S_OK;
}
#pragma endregion

HRESULT CRenderer::Add_DebugComponent(CComponent* pDebugCom)
{
	m_DebugComponents.push_back(pDebugCom);

	Safe_AddRef(pDebugCom);

	return S_OK;
}

HRESULT CRenderer::Add_DebugObject(CGameObject* pDebugObject)
{
	return S_OK;
}

HRESULT CRenderer::Render_DummyPlayer(_float _fTimeDelta)
{
	_Render_Deferred();

	return S_OK;
	
}

CRenderer* CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRenderer* pInstance = new CRenderer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CRenderer");
	}

	return pInstance;
}

CComponent* CRenderer::Clone(void* pArg)
{
	AddRef();

	return this;
}

void CRenderer::Free()
{
	__super::Free();

	Safe_Release(m_pShadowDepthStencilView);

	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShader);
	Safe_Release(m_pShader_UI_Static);
	Safe_Release(m_pUIDepthStencilView);
	Safe_Release(m_pSSLRDepthStencilView);

	// Compute Shader
	Safe_Release(m_pHDRShader);
	Safe_Release(m_pBlurShader);
	Safe_Release(m_pBloomShader);
	Safe_Release(m_pSSAOShader);
	Safe_Release(m_pSSLRShader);

	for (auto& pComponent : m_DebugComponents)
		Safe_Release(pComponent);

	m_DebugComponents.clear();


	Safe_Release(m_pRenderTarget_Manager);
	Safe_Release(m_pLight_Manager);

	for (size_t i = 0; i < RENDER_END; i++)
	{
		for (auto& pGameObject : m_RenderGroup[i])
			Safe_Release(pGameObject);

		m_RenderGroup[i].clear();
	}

	for (auto pComponent : m_DebugComponents)
		Safe_Release(pComponent);

	m_DebugComponents.clear();

	for (auto& pairResource : m_resources)
		Safe_Release(pairResource.second);

	m_resources.clear();
}