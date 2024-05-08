#include "Engine_pch.h"
#include "Object_Manager.h"

// GameObject
#include "GameObject.h"

// Etc
#include "Layer.h"
#include "Object_Pool.h"

IMPLEMENT_SINGLETON(CObject_Manager)

CComponent* CObject_Manager::Get_Component(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strComTag, _uint iIndex)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);

	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Get_Component(strComTag, iIndex);
}

/*
HRESULT CObject_Manager::Reserve_Manager(_uint iNumLevels)
{
	if (nullptr != m_pLayers)
		return E_FAIL;

	m_iNumLevels = iNumLevels;

	m_pLayers = new LAYERS[m_iNumLevels];

	m_arrRegistredLevel = new _bool[m_iNumLevels];

	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		m_arrRegistredLevel[i] = true;
	}

	return S_OK;
}


HRESULT CObject_Manager::Add_Prototype(const wstring& strPrototypeTag, CGameObject* pPrototype)
{
	if (nullptr != Find_Prototype(strPrototypeTag))
		return E_FAIL;

	m_Prototypes.emplace(strPrototypeTag, pPrototype);

	return S_OK;
}
*/

HRESULT CObject_Manager::Add_GameObject(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg)
{
	CGameObject* pPrototype = Find_Prototype(strPrototypeTag);
	if (nullptr == pPrototype)
		return E_FAIL;

	CGameObject* pGameObject = pPrototype->Clone(pArg);
	if (nullptr == pGameObject)
		return E_FAIL;

	pGameObject->Set_Level(iLevelIndex);

	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		if (nullptr == pLayer)
			return E_FAIL;

		pLayer->Add_GameObject(pGameObject);

		m_pLayers[iLevelIndex].emplace(strLayerTag, pLayer);
	}
	else
		pLayer->Add_GameObject(pGameObject);

	return S_OK;
}

/*
HRESULT CObject_Manager::Set_Dead_Object_Specific_Layer(_uint iLevelIndex, const wstring& strLayerTag)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);

	if (nullptr != pLayer)
	{
		pLayer->Set_Dead_Objects();
	}

	return S_OK;
}

HRESULT CObject_Manager::Add_GameObject(_uint iLevelIndex, const wstring& strLayerTag, CGameObject* _pGameObject)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		if (nullptr == pLayer)
			return E_FAIL;

		pLayer->Add_GameObject(_pGameObject);

		m_pLayers[iLevelIndex].emplace(strLayerTag, pLayer);
	}
	else
		pLayer->Add_GameObject(_pGameObject);

	return S_OK;
}


HRESULT CObject_Manager::Create_GameObject(const wstring& strPrototypeTag, CGameObject** _ppOut, void* _pArg)
{
	CGameObject* pPrototype = Find_Prototype(strPrototypeTag);
	if (nullptr == pPrototype)
		return E_FAIL;

	CGameObject* pGameObject = pPrototype->Clone(_pArg);
	if (nullptr == pGameObject)
		return E_FAIL;

	*_ppOut = pGameObject;

	return S_OK;
}
*/

HRESULT CObject_Manager::Create_ObjectPool(const wstring& _strPoolTag, _uint _iSize, CGameObject* _pPrototype, void* _pArg, const wstring& _wstrLayerTag)
{
	auto iter = m_mapPools.find(_strPoolTag);

	if (iter != m_mapPools.end())
	{
		if (nullptr != _pPrototype)
			Safe_Release(_pPrototype);

		return E_FAIL;
	}

	auto pObjectPool = CObject_Pool::Create(_iSize, _pPrototype, _pArg, _wstrLayerTag);

	if (nullptr == pObjectPool)
		return E_FAIL;

	m_mapPools.emplace(_strPoolTag, pObjectPool);
		
	return S_OK;
}

CGameObject* CObject_Manager::Pop_FromObjectPool(const wstring& _strPoolTag, any _args)
{
	auto iter = m_mapPools.find(_strPoolTag);

	if (iter == m_mapPools.end())
		return nullptr;

	return iter->second->Pop(_args);
}

CGameObject* CObject_Manager::Get_FromObjectPool(const wstring& _strPoolTag, any _args)
{
	auto iter = m_mapPools.find(_strPoolTag);

	if (iter == m_mapPools.end())
		return nullptr;

	return iter->second->Get(_args);
}

/*
HRESULT CObject_Manager::Set_UpdateLevel(_uint _uLevel, _bool _bRegistered)
{
	if (m_iNumLevels <= _uLevel)
		return E_FAIL;

	m_arrRegistredLevel[_uLevel] = _bRegistered;

	return S_OK;
}

void CObject_Manager::Tick(_float fTimeDelta)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		if (!m_arrRegistredLevel[i])
			continue;

		for (auto& Pair : m_pLayers[i])
		{
			if (nullptr != Pair.second)
				Pair.second->Tick(fTimeDelta);
		}
	}
}

void CObject_Manager::Late_Tick(_float fTimeDelta)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		if (!m_arrRegistredLevel[i])
			continue;

		for (auto& Pair : m_pLayers[i])
		{
			if (nullptr != Pair.second)
				Pair.second->Late_Tick(fTimeDelta);
		}
	}
}

void CObject_Manager::Clear_All()
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			Safe_Release(Pair.second);
		}

		m_pLayers[i].clear();
	}
}

void CObject_Manager::Clear(_uint iLevelIndex)
{
	if (iLevelIndex >= m_iNumLevels)
		return;

	for (auto& Pair : m_pLayers[iLevelIndex])
		Safe_Release(Pair.second);

	m_pLayers[iLevelIndex].clear();
}

_bool CObject_Manager::_Clear_Prototype()
{
	for (auto& Pair : m_SwapPrototypes)
		Safe_Release(Pair.second);

	m_SwapPrototypes.clear();
	
	return true;
}
*/

_bool CObject_Manager::_Clear_Pool()
{
	for (auto& Pair : m_SwapPools)
		Safe_Release(Pair.second);

	m_SwapPools.clear();

	return true;
}

/*
map<wstring, CLayer*>* CObject_Manager::Get_SpecificLayer(_uint _uLevel)
{
	if (m_iNumLevels <= _uLevel)
		return nullptr;
	else
		return &m_pLayers[_uLevel];
}

CGameObject* CObject_Manager::Find_Prototype(const wstring& strPrototypeTag)
{
	auto	iter = m_Prototypes.find(strPrototypeTag);

	if (iter == m_Prototypes.end())
		return nullptr;

	return iter->second;
}

CLayer* CObject_Manager::Find_Layer(_uint iLevelIndex, const wstring& strLayerTag)
{
	if (iLevelIndex >= m_iNumLevels)
		return nullptr;

	auto	iter = m_pLayers[iLevelIndex].find(strLayerTag);

	if (iter == m_pLayers[iLevelIndex].end())
		return nullptr;

	return iter->second;
}
*/

void CObject_Manager::_Swap_Prototype_Container()
{
	m_Prototypes.swap(m_SwapPrototypes);

	if (!m_Prototypes.empty())
		MSG_BOX("Not empty swap prototypes");

	m_Prototypes.clear();

	m_mapPools.swap(m_SwapPools);

	if (!m_mapPools.empty())
		MSG_BOX("Not empty swap pools");

	m_mapPools.clear();
}

void CObject_Manager::Free()
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			Safe_Release(Pair.second);
		}
		m_pLayers[i].clear();
	}
	Safe_Delete_Array(m_pLayers);

	Safe_Delete_Array(m_arrRegistredLevel);

	for (auto& Pair : m_Prototypes)
	{
		Safe_Release(Pair.second);
	}
	m_Prototypes.clear();

	for (auto& Pair : m_SwapPrototypes)
	{
		Safe_Release(Pair.second);
	}
	m_SwapPrototypes.clear();

	for (auto& pairs : m_mapPools)
	{
		Safe_Release(pairs.second);
	}
	m_mapPools.clear();

	for (auto& pairs : m_SwapPools)
	{
		Safe_Release(pairs.second);
	}
	m_SwapPools.clear();
}
