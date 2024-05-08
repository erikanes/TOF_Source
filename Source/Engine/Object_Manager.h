#pragma once

#include "Base.h"

BEGIN(Engine)

class CObject_Pool;

class CObject_Manager final : public CBase
{
	friend class CLevel; // _Swap_Prototype_Container의 유일한 접근자로 지정하기 위함

	DECLARE_SINGLETON(CObject_Manager)
private:
	CObject_Manager() = default;
	virtual ~CObject_Manager() = default;

public:
	class CComponent* Get_Component(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strComTag, _uint iIndex);

public:
	HRESULT Reserve_Manager(_uint iNumLevels);
	HRESULT Add_Prototype(const wstring& strPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_GameObject(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg);
	HRESULT Add_GameObject(_uint iLevelIndex, const wstring& strLayerTag, CGameObject* _pGameObject);
	HRESULT Set_Dead_Object_Specific_Layer(_uint iLevelIndex, const wstring& strLayerTag);
	HRESULT Create_GameObject(const wstring& strPrototypeTag, CGameObject** _ppOut, void* _pArg);
	HRESULT Create_ObjectPool(const wstring& _strPoolTag, _uint _iSize, CGameObject* _pPrototype, void* _pArg, const wstring& _wstrLayerTag);
	CGameObject* Pop_FromObjectPool(const wstring& _strPoolTag, any _args);
	CGameObject* Get_FromObjectPool(const wstring& _strPoolTag, any _args);
	class CGameObject* Find_Prototype(const wstring& strPrototypeTag);
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);

	void Clear_All();
	void Clear(_uint iLevelIndex);

	map<wstring, class CLayer*>* Get_SpecificLayer(_uint _uLevel);

	HRESULT Set_UpdateLevel(_uint _uLevel, _bool _bRegistered);

private:
	class CLayer* Find_Layer(_uint iLevelIndex, const wstring& strLayerTag);
	void _Swap_Prototype_Container();
	_bool _Clear_Prototype();
	_bool _Clear_Pool();

public:
	virtual void Free() override;

private:
	map<wstring, class CGameObject*>		m_Prototypes;
	map<wstring, class CGameObject*>		m_SwapPrototypes;

	map<wstring, CObject_Pool*>				m_mapPools;
	map<wstring, CObject_Pool*>				m_SwapPools;

	_uint									m_iNumLevels = { 0 };
	map<wstring, class CLayer*>* m_pLayers = { nullptr };
	typedef map<wstring, class CLayer*>		LAYERS;

	_bool* m_arrRegistredLevel = { nullptr };
};

END