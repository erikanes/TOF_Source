#include "Engine_pch.h"
#include "Level.h"

// Singleton
#include "GameInstance.h"
#include "Event_Manager.h"
#include "Object_Manager.h"
#include "Component_Manager.h"
#include "UI_Manager.h"
#include "Collision_Manager.h"
#include "Light_Manager.h"
#include "GameMode.h"

#include "Engine_Functions.h"

CLevel::CLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CLevel::Initialize(void* _pArg)
{
	m_future = m_promise.get_future();

	auto func = [this](_uint _eCurrentLevel)
	{
		m_promise.set_value(_ClearPrevLevelResources(_eCurrentLevel));

		future_status status = m_future.wait_for(chrono::seconds(1));

		if (future_status::ready == status)
		{
			auto hResult = m_future.get();

			if (FAILED(hResult))
			{
				SetWindowText(CEngine_Global::hWnd, TEXT("Failed to loading"));
				MSG_BOX("Failed to clear");
			}
			else if (SUCCEEDED(hResult))
				m_bPrevLevelCleared = true;
		}
	};

	// 원본 객체 컨테이너 스왑
	if (!m_bKeepPrototypeObject)
	{
		CObject_Manager::Get_Instance()->_Swap_Prototype_Container();
		CUI_Manager::Get_Instance()->_Swap_Prototype_Container();
	}

	CEvent_Manager::Get_Instance()->Clear_IntervalFunction();

	CCollision_Manager::Get_Instance()->ClearCollisionGroup();
	CCollision_Manager::Get_Instance()->ResetCollisionGroup();

	CLight_Manager::Get_Instance()->Clear_Lights();

	CGameMode::Get_Instance()->Clear();

	auto eCurrentLevel = CFunc::Get_CurLevel(true);

	m_thread = thread(func, eCurrentLevel);

	Initialize_RenderSetting();

	return S_OK;
}

void CLevel::Tick(_float fTimeDelta)
{
}

void CLevel::Late_Tick(_float fTimeDelta)
{
}

HRESULT CLevel::Render()
{
	return S_OK;
}

/*
HRESULT	CLevel::Add_Usual_Marked_Function(MARKED_FUNCTION_TAG _eTag, function<void(any)> _func)
{
	auto iter = m_MapUsualMarkedFunctions.find(_eTag);

	if (m_MapUsualMarkedFunctions.end() != iter)
	{
		m_MapUsualMarkedFunctions.erase(iter);
	}
	
	m_MapUsualMarkedFunctions.insert({ _eTag, _func });

	return S_OK;
}

function<void(any)>	CLevel::Get_Usual_Marked_Function(MARKED_FUNCTION_TAG _eTag)
{
	auto iter = m_MapUsualMarkedFunctions.find(_eTag);

	if (m_MapUsualMarkedFunctions.end() == iter)
	{
		return [](any _arg) {};
	}
	else
	{
		return iter->second;
	}
}
*/

void CLevel::Initialize_RenderSetting()
{
}

HRESULT CLevel::_ClearPrevLevelResources(_uint _eCurrentLevel)
{
	lock_guard lg(m_mutex);

	if (0 != _eCurrentLevel)
		Sleep(1000);

	// 현재 레벨 레이어 오브젝트 정리
	CObject_Manager::Get_Instance()->Clear(_eCurrentLevel);
	CUI_Manager::Get_Instance()->Clear_UI(_eCurrentLevel);
	CUI_Manager::Get_Instance()->Clear_SystemInfo();

	if (0 != _eCurrentLevel)
	{
		// 현재 레벨 원본 컴포넌트 정리
		if (FAILED(CComponent_Manager::Get_Instance()->Clear_SpecificLevel_Prototype(_eCurrentLevel)))
			return E_FAIL;
	}

	// 원본 객체 정리 (진입 전에 컨테이너 교환된 상태)
	if (!m_bKeepPrototypeObject)
	{
		CObject_Manager::Get_Instance()->_Clear_Prototype();
		CObject_Manager::Get_Instance()->_Clear_Pool();
		CUI_Manager::Get_Instance()->_Clear_Prototype();
	}

	return S_OK;
}

void CLevel::Free()
{
	m_thread.join();

	m_MapUsualMarkedFunctions.clear();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
