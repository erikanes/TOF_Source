#include "Engine_pch.h"
#include "Object_Pool.h"

// Singleton
#include "GameInstance.h"

// GameObject
#include "GameObject.h"

HRESULT CObject_Pool::Initialize(_uint _iSize, CGameObject* _pPrototype, void* _pArg, const wstring& _wstrLayerTag)
{
	if ((0 == _iSize) || (nullptr == _pPrototype))
		return E_FAIL;

	for (_uint i = 0; i < _iSize; ++i)
	{
		auto pObject = _pPrototype->Clone(_pArg);
		pObject->Disable();
		pObject->Set_OwnerPool(this);

		m_objects.push(pObject);
	}

	Safe_Release(_pPrototype);

	m_wstrLayerTag = _wstrLayerTag;

	return S_OK;
}

void CObject_Pool::Push(CGameObject* _pObject)
{
	if (nullptr == _pObject)
		return;

	_pObject->Disable();
	m_objects.push(_pObject);
}

CGameObject* CObject_Pool::Pop(any _args)
{
	// can't resizing

	if (0 >= m_objects.size())
	{
		MSG_BOX("Empty object pool");
		return nullptr;
	}

	auto pObject = m_objects.top();
	m_objects.pop();

	pObject->Enable();
	pObject->Set_Dead(false);
	pObject->Begin(_args);

	auto pGameInstance = CGameInstance::Get_Instance();
	auto iCurrentLevel = static_cast<_uint>(pGameInstance->Get_CurLevel());

	pGameInstance->Add_GameObject(iCurrentLevel, m_wstrLayerTag, pObject);

	return pObject;
}


CGameObject* CObject_Pool::Get(any _args)
{
	// can't resizing

	if (0 >= m_objects.size())
	{
		MSG_BOX("Empty object pool");
		return nullptr;
	}

	auto pObject = m_objects.top();
	m_objects.pop();

	pObject->Enable();
	pObject->Set_Dead(false);
	pObject->Begin(_args);

	auto pGameInstance = CGameInstance::Get_Instance();
	auto iCurrentLevel = static_cast<_uint>(pGameInstance->Get_CurLevel());

	return pObject;
}


CObject_Pool* CObject_Pool::Create(_uint _iSize, CGameObject* _pPrototype, void* _pArg, const wstring& _wstrLayerTag)
{
	auto pInstance = new CObject_Pool();

	if (FAILED(pInstance->Initialize(_iSize, _pPrototype, _pArg, _wstrLayerTag)))
	{
		MSG_BOX("Failed to Create: CObject_Pool::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CObject_Pool::Free()
{
	while (!m_objects.empty())
	{
		Safe_Release(m_objects.top());
		m_objects.pop();
	}
}
