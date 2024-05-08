#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CImGui_MainMenuBar;
class CImGui_Render;
class CImGui_ClientMonitoringManager;

class CImGui_Manager final : public CBase
{
	DECLARE_SINGLETON(CImGui_Manager)

private:
	CImGui_Manager() = default;
	virtual ~CImGui_Manager() = default;

public:
	HRESULT Initialize(HWND _hWnd, ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	void Tick(_float _fTimeDelta);
	void Render();

private:
	void _BeginFrame();

public:
	virtual void Free() override;

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	CImGui_MainMenuBar* m_pMainMenuBar = { nullptr };
	CImGui_Render* m_pRender = { nullptr };
	CImGui_ClientMonitoringManager* m_pClientMonitoring = { nullptr };
};

END