#include "Client_pch.h"
#include "ImGui_Window_RenderTarget.h"

// Singleton
#include "GameInstance.h"

#include "RenderTarget.h"


CImGui_Window_RenderTarget::CImGui_Window_RenderTarget()
	: CImGui_BaseWindow("Render Targets")
{
}

HRESULT CImGui_Window_RenderTarget::Initialize()
{
	auto pGameInstance = CGameInstance::Get_Instance();

	_Add_RenderTarget("BackBuffer", "BackBuffer", pGameInstance->Find_RenderTarget(TEXT("Target_BackBuffer")));
	_Add_RenderTarget("BackBuffer", "Reserve", pGameInstance->Find_RenderTarget(TEXT("Target_Reserve")));

	_Add_RenderTarget("Object", "Diffuse", pGameInstance->Find_RenderTarget(TEXT("Target_Diffuse")));
	_Add_RenderTarget("Object", "Normal", pGameInstance->Find_RenderTarget(TEXT("Target_Normal")), false);
	_Add_RenderTarget("Object", "Emissive", pGameInstance->Find_RenderTarget(TEXT("Target_Emissive")), false);
	_Add_RenderTarget("Object", "Depth", pGameInstance->Find_RenderTarget(TEXT("Target_Depth")), false);
	_Add_RenderTarget("Object", "PlayerDepth", pGameInstance->Find_RenderTarget(TEXT("Target_PlayerDepth")), false);
	_Add_RenderTarget("Object", "CharacterDepth", pGameInstance->Find_RenderTarget(TEXT("Target_CharacterDepth")), false);
	_Add_RenderTarget("Object", "MapDepth", pGameInstance->Find_RenderTarget(TEXT("Target_MapDepth")), false);
	
	_Add_RenderTarget("Light", "Shade", pGameInstance->Find_RenderTarget(TEXT("Target_Shade")), false);
	_Add_RenderTarget("Light", "Specular", pGameInstance->Find_RenderTarget(TEXT("Target_Specular")), false);
	_Add_RenderTarget("Light", "SSLR Occlusion", pGameInstance->Find_RenderTarget(TEXT("Target_SSLR_Occlusion")), false);
	_Add_RenderTarget("Light", "SSLR Ray", pGameInstance->Find_RenderTarget(TEXT("Target_SSLR_Ray")), false);

	_Add_RenderTarget("Shadow", "ShadowDepth", pGameInstance->Find_RenderTarget(TEXT("Target_ShadowDepth")), false);
	_Add_RenderTarget("Shadow", "SSAO", pGameInstance->Find_RenderTarget(TEXT("Target_SSAO")));

	_Add_RenderTarget("Effect", "VFX", pGameInstance->Find_RenderTarget(TEXT("Target_VFX")));
	_Add_RenderTarget("Effect", "Distortion", pGameInstance->Find_RenderTarget(TEXT("Target_Distortion")), false);
	_Add_RenderTarget("Effect", "Glow", pGameInstance->Find_RenderTarget(TEXT("Target_Glow")));
	_Add_RenderTarget("Effect", "Depth", pGameInstance->Find_RenderTarget(TEXT("Target_VFXDepth")), false);

	_Add_RenderTarget("OutLine", "OutLine", pGameInstance->Find_RenderTarget(TEXT("Target_Sobel")));

	_Add_RenderTarget("PostProcessing", "HDR", pGameInstance->Find_RenderTarget(TEXT("Target_HDR")));
	_Add_RenderTarget("PostProcessing", "Bloom", pGameInstance->Find_RenderTarget(TEXT("Target_Bloom")), false);

	return S_OK;
}

void CImGui_Window_RenderTarget::Tick(_float _fTimeDelta)
{
	if (!m_bIsEnable)
		return;

	ImGui::Begin(m_strName.c_str(), &m_bIsEnable);

	_Draw_RenderTargets();

	ImGui::End();
}

void CImGui_Window_RenderTarget::_Add_RenderTarget(const string& _strTag, const char* _szName, CRenderTarget* _pRenderTarget, _bool _bKeepAlpha)
{
	if (nullptr == _pRenderTarget)
		return;

	const auto& vClearColor = _pRenderTarget->Get_ClearColor();
	ImVec4 vColor;
	
	memcpy(&vColor, &vClearColor, sizeof(_float4));

	if (!_bKeepAlpha)
		vColor.w = 1.f;

	m_renderTargets[_strTag].push_back({ _szName, _pRenderTarget->Get_SRV(), vColor });
}

void CImGui_Window_RenderTarget::_Draw_RenderTargets()
{
	const _float fBaseSize = 128.f;
	const _float fAspect = static_cast<_float>(CGlobal::g_iWinSizeX) / static_cast<_float>(CGlobal::g_iWinSizeY);

	const ImVec2 vImageSize(fBaseSize * fAspect, fBaseSize);
	const ImVec2 vUV0(0.f, 0.f);
	const ImVec2 vUV1(1.f, 1.f);

	for (auto& pairGroup : m_renderTargets)
	{
		auto& renderTargets = pairGroup.second;

		ImGui::SeparatorText(pairGroup.first.c_str());

		for (size_t i = 0; i < renderTargets.size(); ++i)
		{
			ImGui::BeginGroup();

			ImGui::Text(renderTargets[i].szName);
			ImGui::ImageButton(renderTargets[i].pTexture, vImageSize, vUV0, vUV1, -1, renderTargets[i].vColor);
			ImGui::EndGroup();

			if (0 == ((i + 1) % 3))
				ImGui::NewLine();
			else
			{
				if (renderTargets.size()-1 != i)
					ImGui::SameLine();
			}
		}
	}
}

CImGui_Window_RenderTarget* CImGui_Window_RenderTarget::Create()
{
	auto pInstance = new CImGui_Window_RenderTarget();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Create: CImGui_Window_RenderTarget::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CImGui_Window_RenderTarget::Free()
{
	for (auto& pairGroup : m_renderTargets)
		pairGroup.second.clear();

	m_renderTargets.clear();
}
