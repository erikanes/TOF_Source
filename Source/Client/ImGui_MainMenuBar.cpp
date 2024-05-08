#include "Client_pch.h"
#include "ImGui_MainMenuBar.h"

CImGui_MainMenuBar::CImGui_MainMenuBar()
{
	m_strName = "Main Menu Bar";
}

CImGui_MainMenuBar::~CImGui_MainMenuBar()
{
}

HRESULT CImGui_MainMenuBar::Initialize()
{
	return S_OK;
}

void CImGui_MainMenuBar::Tick(_float _fTimeDelta)
{
	ImGui::BeginMainMenuBar();

	char szTitle[256] = {};
	GetWindowTextA(CGlobal::g_hWnd, szTitle, 256);

	ImGui::SetCursorPos(ImVec2(CGlobal::g_fWinSizeX * 0.8f, 0.f));
	ImGui::Text(szTitle);

	if (ImGui::CloseButton(m_ID, ImVec2(CGlobal::g_fWinSizeX - 19.f, 0.f)))
		PostQuitMessage(0);

	ImGui::EndMainMenuBar();
}

CImGui_MainMenuBar* CImGui_MainMenuBar::Create()
{
	auto pInstance = new CImGui_MainMenuBar();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Create: CImGui_MainMenuBar::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CImGui_MainMenuBar::Free()
{
}
