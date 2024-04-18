#include "Engine_pch.h"
#include "Event_Manager.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "Level.h"

#include "GameModeImpl.h"
#include "Object_Pool.h"

#include <optional>

IMPLEMENT_SINGLETON(CEvent_Manager);

CEvent_Manager::CEvent_Manager()
{
}

HRESULT CEvent_Manager::Tick(_float _fTimeDelta)
{
	auto& funcs = m_listIntervalFunctions;

	for (auto iter = funcs.begin(); iter != funcs.end();)
	{
		auto pCaller = iter->pCaller;

		if (nullptr != pCaller && pCaller->Get_Dead())
			iter = funcs.erase(iter);

		else
		{
			iter->fWaitTime -= _fTimeDelta;

			if (iter->fWaitTime <= 0.f)
			{
				iter->func(_fTimeDelta);

				iter->fWaitTime += iter->fInterval;

				if (iter->bRepeat)
				{
					++iter;
					continue;
				}
				else
				{
					if (--iter->iCount <= 0)
						iter = funcs.erase(iter);
					else
						++iter;
				}
			}
			else
				++iter;
		}
	}

	return S_OK;
}

HRESULT CEvent_Manager::Late_Tick(_float _fTimeDelta)
{
	while (!m_qDestroyInstances.empty())
	{
		CGameObject* pGameObject = m_qDestroyInstances.front();
		m_qDestroyInstances.pop_front();

		auto pOwnerPool = pGameObject->Get_OwnerPool();

		if (nullptr != pOwnerPool)
			pOwnerPool->Push(pGameObject);
		else
			Safe_Release(pGameObject);
	}

	optional<EventMsg> evt;

	while (!m_qEventMsg.empty())
	{
		EventMsg tMsg = m_qEventMsg.front();
		m_qEventMsg.pop_front();

		if (EVENT_TYPE::CHANGE_LEVEL == tMsg.eEventType)
		{
			evt = tMsg;
			continue;
		}

		Handling_EventMsg(tMsg);
	}

	// 레벨 전환은 마지막에 처리되어야 한다
	if (evt)
		Handling_EventMsg(*evt);

	return S_OK;
}

HRESULT CEvent_Manager::Initialize(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	m_pDevice = _pDevice;
	m_pContext = _pContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	return S_OK;
}

HRESULT CEvent_Manager::Add_EventMsg(EventMsg _tMsg)
{
	m_qEventMsg.push_back(_tMsg);

	return S_OK;
}

HRESULT CEvent_Manager::Handling_EventMsg(EventMsg _tMsg)
{
	switch (_tMsg.eEventType)
	{
	case EVENT_TYPE::DESTROY_INSTANCE:
	{
		CGameObject* pInstance = (CGameObject*)_tMsg.wParam;
		pInstance->Set_Dead(true);
		m_qDestroyInstances.push_back(pInstance);
	}
	break;

	case EVENT_TYPE::GAME_MODE_CAMERA:
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		pGameInstance->Handling_Event_GameMode(_tMsg);
	}
	break;

	case EVENT_TYPE::GAME_MODE_MINIGAME:
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		pGameInstance->Change_GameMode((CGameModeImpl*)(_tMsg.wParam));
	}
	break;

	case EVENT_TYPE::CHANGE_LEVEL:
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();

		typedef CLevel* (*pFuncCreate)(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _uint _eNextLevel);
		pFuncCreate pFunc = (pFuncCreate)(_tMsg.lParam);

		//상위 2바이트는 dst level index, 하위 2바이트는 src level index;
		_uint iDstLevel = (_tMsg.wParam >> 16) & 0x0000FFFF;
		_uint iSrcLevel = (_tMsg.wParam & 0x0000FFFF);

		if (FAILED(pGameInstance->Open_Level(iSrcLevel, pFunc(m_pDevice, m_pContext, iDstLevel))))
		{
			MSG_BOX("Error occure when event manager in change level.");
		}
	}
	break;

	case EVENT_TYPE::POP_OBJECTPOOL:
	{
		// wParam : wstring pointer (object pool tag)
		// cParam : any pointer (begin arguments)

		auto pGameInstance = CGameInstance::Get_Instance();

		auto strPoolTag = reinterpret_cast<wstring*>(_tMsg.wParam);
		auto args = _tMsg.cParam;

		pGameInstance->Pop_FromObjectPool(*strPoolTag, args);
	}
	break;

	case EVENT_TYPE::ADD_INTERVAL_FUNCTION:
	{
		// cParam : INTERVAL_FUNC_DESC

		auto desc = any_cast<INTERVAL_FUNCTION_DESC>(_tMsg.cParam);

		m_listIntervalFunctions.emplace_back(desc);
	}
	break;

	case EVENT_TYPE::UI_ENABLE:
	{
		// wParam : Level
		// cParam : wstring (UI Object Layer tag)
		CGameInstance::Get_Instance()->Enable_UI(_tMsg.wParam, any_cast<std::wstring>(_tMsg.cParam));
	}
	break;
	case EVENT_TYPE::UI_DISABLE:
	{
		// wParam : Level
		// cParam : wstring (UI Object Layer tag)
		CGameInstance::Get_Instance()->Disable_UI(_tMsg.wParam, any_cast<wstring>(_tMsg.cParam));
	}
	break;
	case EVENT_TYPE::UI_INTERACTION_ENABLE:
	{
		// wParam : Level
		// lParam : enum (INTERACTION_TYPE)
		// cParam : wstring (UI Object Layer Tag)
		// sParam : wstring (InteractionObjectName)
		CGameInstance::Get_Instance()->Enable_UI_InteractionInfo(_tMsg.wParam, any_cast<wstring>(_tMsg.cParam),
			any_cast<wstring>(_tMsg.sParam), _tMsg.lParam);
	}
	break;
	default:
		break;
	}

	return S_OK;
}


void CEvent_Manager::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}


