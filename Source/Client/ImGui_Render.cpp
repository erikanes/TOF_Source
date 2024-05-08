#include "Client_pch.h"
#include "ImGui_Render.h"

// Singleton
#include "GameInstance.h"
#include "Render_Manager.h"
#include "Dialog_Manager.h"
#include "Quest_Manager.h"

// GameObject
#include "Camera_Free.h"

// Component
#include "Renderer.h"
#include "Transform.h"

// ImGui
#include "ImGui_Window_RenderTarget.h"

// Etc
#include "Layer.h"

#include "Engine_Functions.h"
#include "Client_Functions.h"

CImGui_Render::CImGui_Render()
	: CImGui_BaseWindow("Render", ImVec2(100.f, 100.f), ImVec2(400.f, 400.f))
{
}

HRESULT CImGui_Render::Initialize(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	m_pDevice = _pDevice;
	m_pContext = _pContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	m_pRender_Manager = CRender_Manager::Get_Instance();
	Safe_AddRef(m_pRender_Manager);

	m_pRenderTargetWindow = CImGui_Window_RenderTarget::Create();

	if (nullptr == m_pRenderTargetWindow)
		return E_FAIL;

	m_mapLevels.emplace(LEVEL_DESERT, "Level_Desert");
	m_mapLevels.emplace(LEVEL_MIRRORCITY, "Level_MirrorCity");
	m_mapLevels.emplace(LEVEL_ABYSS, "Level_Abyss");
	m_mapLevels.emplace(LEVEL_TEST, "Level_Test");

	return S_OK;
}

void CImGui_Render::Tick(_float _fTimeDelta)
{
	ImGui::Begin(m_strName.c_str(), &m_bIsEnable);
	{
		_Draw_DebugInfo();

		if (ImGui::CollapsingHeader("Post Processing", ImGuiTreeNodeFlags_Framed))
		{
			_Control_HDR();
			_Control_Adaptation();
			_Control_Bloom();
			_Control_Glow();
			_Control_SSAO();
			_Control_RimLight();
			_Control_DepthOfField();
			_Control_Fog();
		}

#ifdef _DEBUG
		_Control_Shader();
		_Control_Other();
		_Control_Level();
		_Control_Dialog();
#endif

		if (ImGui::Button(m_pRenderTargetWindow->Get_Name().c_str()))
			m_pRenderTargetWindow->Enable();

		m_pRenderTargetWindow->Tick(_fTimeDelta);
	}
	ImGui::End();
}

void CImGui_Render::_Draw_DebugInfo()
{
	auto& io = ImGui::GetIO();

	ImGui::SeparatorText("Info");

	ImGui::Text("FPS: %.2f", io.Framerate);
	ImGui::Text("DT: %.4f", io.DeltaTime);
	ImGui::Text("Batch : %d", m_pRender_Manager->Get_BatchCount());
}

void CImGui_Render::_Control_HDR()
{
	auto tAttributes = any_cast<CRenderer::RENDER_ATTRIBUTES>(m_pRender_Manager->Get_RenderAttributes());

	_bool& bIsEnableHDR = tAttributes.bEnableHDR;

	ImGui::SeparatorText("HDR");

	if (ImGui::Checkbox("##HDR", &bIsEnableHDR))
	{
		if (bIsEnableHDR)
			m_pRender_Manager->Enable_HDR();
		else
			m_pRender_Manager->Disable_HDR();
	}

	ImGui::SameLine();
	ImGui::Text("Enable");

	if (bIsEnableHDR)
	{
		_float& fMiddleGrey = tAttributes.fMiddleGrey;
		_float& fLuminanceWhite = tAttributes.fLuminanceWhite;

		const auto& vMiddleGreyRange = m_pRender_Manager->Get_MiddleGreyRange();
		const auto& vLuminanceWhiteRange = m_pRender_Manager->Get_LuminanceWhiteRange();

#ifdef _DEBUG
		ImGui::Text("Avg Lum: %.4f", m_pRender_Manager->Get_AverageLuminance());
#endif

		if (ImGui::SliderFloat("Middle Grey", &fMiddleGrey, vMiddleGreyRange.x, vMiddleGreyRange.y, "%.3f", ImGuiSliderFlags_AlwaysClamp))
			m_pRender_Manager->Set_MiddleGrey(fMiddleGrey);

		if (ImGui::SliderFloat("Luminance White", &fLuminanceWhite, vLuminanceWhiteRange.x, vLuminanceWhiteRange.y, "%.3f", ImGuiSliderFlags_AlwaysClamp))
			m_pRender_Manager->Set_LuminanceWhite(fLuminanceWhite);
	}

	ImGui::NewLine();
}

void CImGui_Render::_Control_Adaptation()
{
	auto tAttributes = any_cast<CRenderer::RENDER_ATTRIBUTES>(m_pRender_Manager->Get_RenderAttributes());

	_bool& bIsEnableAdaptation = tAttributes.bEnableAdaptation;

	ImGui::SeparatorText("Adaptation");

	if (ImGui::Checkbox("##Adaptation", &bIsEnableAdaptation))
	{
		if (bIsEnableAdaptation)
			m_pRender_Manager->Enable_Adaptation();

		else
			m_pRender_Manager->Disable_Adaptation();
	}

	ImGui::SameLine();
	ImGui::Text("Enable");

	if (bIsEnableAdaptation)
	{
		_float& fElapsedTime = tAttributes.fAdaptationElapsedTime;

		if (ImGui::SliderFloat("Adapt ElapsedTime", &fElapsedTime, 0.f, 10.f, "%.3f", ImGuiSliderFlags_AlwaysClamp))
			m_pRender_Manager->Set_AdaptationElapsedTime(fElapsedTime);
	}

	ImGui::NewLine();
}

void CImGui_Render::_Control_Bloom()
{
	auto tAttributes = any_cast<CRenderer::RENDER_ATTRIBUTES>(m_pRender_Manager->Get_RenderAttributes());

	_bool& bIsEnableBloom = tAttributes.bEnableBloom;

	ImGui::SeparatorText("Bloom");

	if (ImGui::Checkbox("##Bloom", &bIsEnableBloom))
	{
		if (bIsEnableBloom)
			m_pRender_Manager->Enable_Bloom();
		else
			m_pRender_Manager->Disable_Bloom();
	}

	ImGui::SameLine();
	ImGui::Text("Enable");

	if (bIsEnableBloom)
	{
		_float& fBloomThreshold = tAttributes.fBloomThreshold;
		_float& fBloomScale = tAttributes.fBloomScale;

		if (ImGui::SliderFloat("Bloom Threshold", &fBloomThreshold, 0.f, 5.f, "%.3f", ImGuiSliderFlags_AlwaysClamp))
			m_pRender_Manager->Set_Bloom_Threshold(fBloomThreshold);

		if (ImGui::SliderFloat("Bloom Scale", &fBloomScale, 0.f, 5.f, "%.3f", ImGuiSliderFlags_AlwaysClamp))
			m_pRender_Manager->Set_Bloom_Scale(fBloomScale);
	}

	ImGui::NewLine();
}

void CImGui_Render::_Control_Glow()
{
	auto tAttributes = any_cast<CRenderer::RENDER_ATTRIBUTES>(m_pRender_Manager->Get_RenderAttributes());

	_bool& bIsEnableGlow = tAttributes.bEnableGlow;

	ImGui::SeparatorText("Glow");

	if (ImGui::Checkbox("##Glow", &bIsEnableGlow))
	{
		if (bIsEnableGlow)
			m_pRender_Manager->Enable_Glow();
		else
			m_pRender_Manager->Disable_Glow();
	}

	ImGui::SameLine();
	ImGui::Text("Enable");

	if (bIsEnableGlow)
	{
		_float& fGlowAverageLuminance = tAttributes.fGlowAverageLuminance;
		_float& fGlowThreshold = tAttributes.fGlowThreshold;

		if (ImGui::SliderFloat("Glow Avg Lum", &fGlowAverageLuminance, 0.f, 1.f, "%.3f", ImGuiSliderFlags_AlwaysClamp))
			m_pRender_Manager->Set_Glow_AverageLuminance(fGlowAverageLuminance);

		if (ImGui::SliderFloat("Glow Threshold", &fGlowThreshold, 0.f, 5.f, "%.3f", ImGuiSliderFlags_AlwaysClamp))
			m_pRender_Manager->Set_Glow_Threshold(fGlowThreshold);
	}

	ImGui::NewLine();
}

void CImGui_Render::_Control_SSAO()
{
	auto tAttributes = any_cast<CRenderer::RENDER_ATTRIBUTES>(m_pRender_Manager->Get_RenderAttributes());

	_bool& bIsEnableSSAO = tAttributes.bEnableSSAO;

	ImGui::SeparatorText("SSAO");

	if (ImGui::Checkbox("##SSAO", &bIsEnableSSAO))
	{
		if (bIsEnableSSAO)
			m_pRender_Manager->Enable_SSAO();
		else
			m_pRender_Manager->Disable_SSAO();
	}

	ImGui::SameLine();
	ImGui::Text("Enable");

	if (bIsEnableSSAO)
	{
		_float& fSSAOLength = tAttributes.fSSAOLength;
		_float& fSSAOThreshold = tAttributes.fSSAOThreshold;
		_int& iSSAOBlurPass = tAttributes.iSSAOBlurPass;

		if (ImGui::SliderFloat("SSAO Length", &fSSAOLength, 0.f, 10.f, "%.3f", ImGuiSliderFlags_AlwaysClamp))
			m_pRender_Manager->Set_SSAO_Length(fSSAOLength);

		if (ImGui::SliderFloat("SSAO Threshold", &fSSAOThreshold, 0.f, 1.f, "%.3f", ImGuiSliderFlags_AlwaysClamp))
			m_pRender_Manager->Set_SSAO_Threshold(fSSAOThreshold);

		if (ImGui::SliderInt("SSAO Blur Pass", &iSSAOBlurPass, 0, 10, "%d", ImGuiSliderFlags_AlwaysClamp))
			m_pRender_Manager->Set_SSAO_BlurPass(iSSAOBlurPass);
	}

	ImGui::NewLine();
}

void CImGui_Render::_Control_RimLight()
{
	_bool bEnableRimLight = m_pRender_Manager->Is_Enable_RimLight();

	ImGui::SeparatorText("RimLight");

	if (ImGui::Checkbox("##RimLight", &bEnableRimLight))
	{
		if (bEnableRimLight)
			m_pRender_Manager->Enable_RimLight();
		else
			m_pRender_Manager->Disable_RimLight();
	}

	ImGui::SameLine();
	ImGui::Text("Enable");

	ImGui::NewLine();
}

void CImGui_Render::_Control_DepthOfField()
{
	auto tAttributes = any_cast<CRenderer::RENDER_ATTRIBUTES>(m_pRender_Manager->Get_RenderAttributes());

	_bool& bIsEnableDepthOfField = tAttributes.bEnableDepthOfField;

	ImGui::SeparatorText("Depth Of Field");

	if (ImGui::Checkbox("##Depth Of Field", &bIsEnableDepthOfField))
	{
		if (bIsEnableDepthOfField)
			m_pRender_Manager->Enable_DepthOfField();
		else
			m_pRender_Manager->Disable_DepthOfField();
	}

	ImGui::SameLine();
	ImGui::Text("Enable");

	if (bIsEnableDepthOfField)
	{
		_float& fFocus = tAttributes.fFocus;
		_float& fRange = tAttributes.fFocusRange;

		if (ImGui::SliderFloat("Focus", &fFocus, 0.f, 1.f, "%.3f", ImGuiSliderFlags_AlwaysClamp))
			m_pRender_Manager->Set_DepthOfField_Focus(fFocus);

		if (ImGui::SliderFloat("Focus Range", &fRange, 0.f, 1.f, "%.3f", ImGuiSliderFlags_AlwaysClamp))
			m_pRender_Manager->Set_DepthOfField_FocusRange(fRange);
	}

	ImGui::NewLine();
}

void CImGui_Render::_Control_Fog()
{
	auto tAttributes = any_cast<CRenderer::RENDER_ATTRIBUTES>(m_pRender_Manager->Get_RenderAttributes());

	_bool& bIsEnableFog = tAttributes.bEnableFog;

	ImGui::SeparatorText("Fog");

	if (ImGui::Checkbox("##Fog", &bIsEnableFog))
	{
		if (bIsEnableFog)
			m_pRender_Manager->Enable_Fog();
		else
			m_pRender_Manager->Disable_Fog();
	}

	ImGui::SameLine();
	ImGui::Text("Enable");

	if (bIsEnableFog)
	{
		_float& fStart = tAttributes.fFogStart;
		_float& fEnd = tAttributes.fFogEnd;
		_float4& vFogColor = tAttributes.vFogColor;
		_bool& bUseHeightFog = tAttributes.bUseHeightFog;
		_float& fHeightStart = tAttributes.fFogHeightStart;
		_float& fHeightEnd = tAttributes.fFogHeightEnd;

		_float fFar = CGameInstance::Get_Instance()->Get_CameraFar();

		if (ImGui::SliderFloat("Fog Start", &fStart, -fFar, fFar, "%.3f", ImGuiSliderFlags_AlwaysClamp))
			m_pRender_Manager->Set_FogStart(fStart);

		if (ImGui::SliderFloat("Fog End", &fEnd, 0.f, fFar, "%.3f", ImGuiSliderFlags_AlwaysClamp))
			m_pRender_Manager->Set_FogEnd(fEnd);

		if (ImGui::ColorEdit4("##Fog Color", &vFogColor.x, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_AlphaBar))
			m_pRender_Manager->Set_FogColor(vFogColor);

		if (ImGui::Checkbox("##Height Fog", &bUseHeightFog))
			m_pRender_Manager->Set_UseHeightFog(bUseHeightFog);

		ImGui::SameLine();
		ImGui::Text("Height Fog");

		if (bUseHeightFog)
		{
			if (ImGui::SliderFloat("Height Fog Start", &fHeightStart, -fFar, fFar, "%.3f", ImGuiSliderFlags_AlwaysClamp))
				m_pRender_Manager->Set_HeightFogStart(fHeightStart);

			if (ImGui::SliderFloat("Height Fog End", &fHeightEnd, -fFar, fFar, "%.3f", ImGuiSliderFlags_AlwaysClamp))
				m_pRender_Manager->Set_HeightFogEnd(fHeightEnd);
		}
	}

	ImGui::NewLine();
}

#ifdef _DEBUG
void CImGui_Render::_Control_Shader()
{
	if (ImGui::CollapsingHeader("Model Options", ImGuiTreeNodeFlags_Framed))
	{
		auto tAttributes = any_cast<CRenderer::RENDER_ATTRIBUTES>(m_pRender_Manager->Get_RenderAttributes());

		{
			ImGui::SeparatorText("Play Hair");

			ImGui::SliderFloat("Hair Presnel Power", &CGlobal::g_fHairFresnelPower, 0.f, 5.f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
			ImGui::SliderFloat("Hair Presnel Intensity", &CGlobal::g_fHairFresnelIntensity, 0.f, 5.f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
		}

		{
			_bool& bIsEnableEdgeHighlight = tAttributes.bEnableEdgeHighlight;
			auto& vEdgeColor = tAttributes.vEdgeHighlightColor;

			ImGui::SeparatorText("Edge Highlight (Sobel)");

			if (ImGui::Checkbox("##Edge Highlight", &bIsEnableEdgeHighlight))
			{
				if (bIsEnableEdgeHighlight)
					m_pRender_Manager->Enable_EdgeHighlight();
				else
					m_pRender_Manager->Disable_EdgeHighlight();
			}

			ImGui::SameLine(); ImGui::Text("Enable");

			if (ImGui::SliderFloat("##Edge Sobel Thickness", &tAttributes.fEdgeHighlightThickness, 0.f, 0.001f, "%.4f", ImGuiSliderFlags_AlwaysClamp))
				m_pRender_Manager->Set_EdgeHighlight_Thickness(tAttributes.fEdgeHighlightThickness);

			ImGui::SameLine(); ImGui::Text("Thickness");

			if (ImGui::ColorEdit3("##Edge Sobel Color", &vEdgeColor.x, ImGuiColorEditFlags_DisplayRGB))
				m_pRender_Manager->Set_EdgeHighlight_Color(vEdgeColor);
			
		}
	}
}
void CImGui_Render::_Control_Other()
{
	if (ImGui::CollapsingHeader("Others", ImGuiTreeNodeFlags_Framed))
	{
#pragma region Lens Flare
		auto& bUseLensFlare = CGlobal::g_bUseLensFlare;

		ImGui::SeparatorText("Lens Flare");

		ImGui::Checkbox("##Use LensFlare", &bUseLensFlare);
		ImGui::SameLine(); ImGui::Text("Enable");
#pragma endregion

#pragma region Auto Targetting
		ImGui::SeparatorText("Auto Target");

		auto& bUseAutoTarget = CGlobal::g_bUseAutoTarget;

		ImGui::Checkbox("##Use Auto Target", &bUseAutoTarget);
		ImGui::SameLine(); ImGui::Text("Enable");
#pragma endregion

#pragma region Free Camera
		ImGui::SeparatorText("Free Camera");

		if (ImGui::Checkbox("##Use Free Camera", &m_bUseFreeCamera))
		{
			if (!m_bCameraInitialized)
			{
				m_bCameraInitialized = true;
				_Create_Camera();
			}

			_Change_Camera();
		}

		ImGui::SameLine(); ImGui::Text("Use");

		if (m_bUseFreeCamera)
		{
			auto pTransform = static_cast<CTransform*>(m_pCamera->Find_Component(TEXT("Com_Transform")));
			auto tDesc = pTransform->Get_TransformDesc();
			auto fRotateToRadians = XMConvertToDegrees(tDesc.fRotationPerSec);

			if (ImGui::SliderFloat("Speed", &tDesc.fSpeedPerSec, 0.f, 300.f, "%.1f", ImGuiSliderFlags_AlwaysClamp))
				pTransform->Set_TransformDesc(tDesc);

			if (ImGui::SliderFloat("Rotate", &fRotateToRadians, 0.f, 360.f, "%.1f", ImGuiSliderFlags_AlwaysClamp))
			{
				tDesc.fRotationPerSec = XMConvertToRadians(fRotateToRadians);
				pTransform->Set_TransformDesc(tDesc);
			}

			if (ImGui::SmallButton("##Reset"))
			{
				tDesc.fRotationPerSec = XMConvertToRadians(90.f);
				tDesc.fSpeedPerSec = 50.f;
				pTransform->Set_TransformDesc(tDesc);
			}
			ImGui::SameLine(); ImGui::Text("Reset");
		}
#pragma endregion

#pragma region Enable/Disable Render
		ImGui::SeparatorText("Enable/Disable Render");

		_bool bRenderMap = m_pRender_Manager->IsEnableRenderMap();

		if (ImGui::Checkbox("##Set Render Map", &bRenderMap))
		{
			m_pRender_Manager->Set_EnableRenderMap(bRenderMap);
		}
		ImGui::SameLine(); ImGui::Text("Map");
#pragma endregion

#pragma region Shadow
		ImGui::SeparatorText("Shadow");

		_float fShadowBias = m_pRender_Manager->Get_ShadowBias();

		if (ImGui::InputFloat("##Shadow Bias", &fShadowBias, 0.00001f, 0.00001f, "%.5f"))
			m_pRender_Manager->Set_ShadowBias(fShadowBias);

		ImGui::SameLine(); ImGui::Text("Bias");
#pragma endregion

#pragma region PBR
		ImGui::SeparatorText("PBR");

		_bool bEnablePBR = m_pRender_Manager->IsEnable_PBR();

		if (ImGui::Checkbox("##Enable PBR", &bEnablePBR))
		{
			if (bEnablePBR)
				m_pRender_Manager->Enable_PBR();
			else
				m_pRender_Manager->Disable_PBR();
		}
		ImGui::SameLine(); ImGui::Text("Enable");
#pragma endregion
	}
}

#ifdef  _DEBUG
_bool CImGui_Render::Is_Render_Map()
{
	return m_pRender_Manager->IsEnableRenderMap();
}
#endif

void CImGui_Render::_Control_Level()
{
	static LEVEL eSelectLevel = LEVEL_MIRRORCITY;
	static _bool bFirstInit = false;	

	if (ImGui::CollapsingHeader("Level", ImGuiTreeNodeFlags_Framed))
	{
		if (ImGui::BeginCombo("##Level Select", m_mapLevels[eSelectLevel].c_str()))
		{
			for (auto& pairLevel : m_mapLevels)
			{
				if (ImGui::Selectable(pairLevel.second.c_str()))
					eSelectLevel = pairLevel.first;
			}

			ImGui::EndCombo();
		}

		ImGui::SameLine();

		if (ImGui::Button("Change Level"))
		{
			auto eCurrentLevel = CFunc::Get_CurLevel();
			auto bIsDiffLevel = eCurrentLevel != eSelectLevel;
			auto bIsLoadingLevel = CFunc::Get_CurLevel(true) == LEVEL_LOADING;

			if (bIsDiffLevel && !bIsLoadingLevel)
				CFunc::Add_Event_ChangeLevel(eCurrentLevel, eSelectLevel);
		}
	}
}

void CImGui_Render::_Control_Dialog()
{
	if (ImGui::CollapsingHeader("Dialog", ImGuiTreeNodeFlags_Framed))
	{
		static _int iDialogID = 0;
		ImGui::InputInt("DialogID", &iDialogID, 1, 100000);

		ImGui::SameLine();
		
		if (ImGui::Button("Run Dialog"))
			CDialog_Manager::Get_Instance()->Begin_Dialog(iDialogID);
	}
}

CCamera* CImGui_Render::_Create_Camera()
{
	auto pGameInstance = CGameInstance::Get_Instance();
	auto iCurrentLevel = static_cast<LEVEL>(pGameInstance->Get_CurLevel());

	CCamera_Free::CAMERAFREEDESC desc;
	desc.eLevel = iCurrentLevel;

	desc.CameraDesc.fAspect = CGlobal::g_fWinSizeX / CGlobal::g_fWinSizeY;
	desc.CameraDesc.fFar = 3000.f;
	desc.CameraDesc.fFovy = XMConvertToRadians(60.f);
	desc.CameraDesc.fNear = 0.1f;
	desc.CameraDesc.iLevelIndex = LEVEL_STATIC;
	desc.CameraDesc.strTranformTag = PC_TRANSFORM;
	desc.CameraDesc.TransformDesc = { 50.f, XMConvertToRadians(90.f) };
	desc.CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	desc.CameraDesc.vEye = _float4(0.f, -5.f, 0.f, 1.f);

	auto pPrototype = CCamera_Free::Create(m_pDevice, m_pContext);
	auto pCamera = static_cast<CCamera*>(pPrototype->Clone(&desc));

	Safe_Release(pPrototype);

	m_pCamera = pCamera;

	return pCamera;
}

void CImGui_Render::_Change_Camera()
{
	if (m_bUseFreeCamera)
	{
		auto pGameInstance = CGameInstance::Get_Instance();
		auto iCurrentLevel = static_cast<_uint>(pGameInstance->Get_CurLevel());

		auto pMapLayers = pGameInstance->Get_SpecificLayer(iCurrentLevel);

		if (nullptr == pMapLayers)
			return;

		auto iter = pMapLayers->find(TEXT("Layer_Camera"));

		if (iter == pMapLayers->end())
			return;

		auto pLayer = iter->second;

		if (nullptr == pLayer)
			return;

		auto& objectList = pLayer->GetObjectList();

		if (objectList.empty())
			return;

		auto pMainCamera = static_cast<CCamera*>(objectList.front());
		auto pMainCameraTransform = static_cast<CTransform*>(pMainCamera->Find_Component(TEXT("Com_Transform")));

		if (nullptr == pMainCameraTransform)
			return;

		auto matWorld = pMainCameraTransform->Get_WorldFloat4x4();
		auto pTransform = static_cast<CTransform*>(m_pCamera->Find_Component(TEXT("Com_Transform")));

		pTransform->Set_WorldMatrix(matWorld);

		objectList.push_back(m_pCamera);
		Safe_AddRef(m_pCamera);
	}

	else
	{
		auto pGameInstance = CGameInstance::Get_Instance();
		auto iCurrentLevel = static_cast<_uint>(pGameInstance->Get_CurLevel());
		auto pMapLayers = pGameInstance->Get_SpecificLayer(iCurrentLevel);

		if (nullptr == pMapLayers)
			return;

		auto iter = pMapLayers->find(TEXT("Layer_Camera"));

		if (iter == pMapLayers->end())
			return;

		auto pLayer = iter->second;

		if (nullptr == pLayer)
			return;

		auto& objectList = pLayer->GetObjectList();

		if (objectList.empty())
			return;

		auto findIter = find_if(objectList.begin(), objectList.end(), [](CGameObject* _pObject) { return nullptr != dynamic_cast<CCamera_Free*>(_pObject); });

		if (findIter == objectList.end())
			return;

		objectList.erase(findIter);
		Safe_Release(m_pCamera);
	}
}
#endif

CImGui_Render* CImGui_Render::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	auto pInstance = new CImGui_Render();

	if (FAILED(pInstance->Initialize(_pDevice, _pContext)))
	{
		MSG_BOX("Failed to Create: CImGui_Render::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CImGui_Render::Free()
{
	Safe_Release(m_pRender_Manager);
	Safe_Release(m_pRenderTargetWindow);

	Safe_Release(m_pCamera);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	m_mapLevels.clear();
}
