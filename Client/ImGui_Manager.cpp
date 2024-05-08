#include "Client_pch.h"
#include "ImGui_Manager.h"

// ImGui Windows
#include "ImGui_Render.h"
#include "ImGui_MainMenuBar.h"
#include "ImGui_ClientMonitoringManager.h"

IMPLEMENT_SINGLETON(CImGui_Manager)

HRESULT CImGui_Manager::Initialize(HWND _hWnd, ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	m_pDevice = _pDevice;
	m_pContext = _pContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGuiIO& io = ImGui::GetIO();

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	ImGui_ImplWin32_Init(_hWnd);
	ImGui_ImplDX11_Init(_pDevice, _pContext);

	m_pMainMenuBar = CImGui_MainMenuBar::Create();
	m_pRender = CImGui_Render::Create(m_pDevice, m_pContext);
	m_pClientMonitoring = CImGui_ClientMonitoringManager::Create();

	return S_OK;
}

void CImGui_Manager::Tick(_float _fTimeDelta)
{
	_BeginFrame();

	m_pMainMenuBar->Tick(_fTimeDelta);
	m_pRender->Tick(_fTimeDelta);
	m_pClientMonitoring->Tick(_fTimeDelta);
}

void CImGui_Manager::Render()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void CImGui_Manager::_BeginFrame()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void CImGui_Manager::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pRender);
	Safe_Release(m_pMainMenuBar);

	Safe_Release(m_pClientMonitoring);

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

