#pragma once

#include "Base.h"

BEGIN(Engine)

class CGameObject;

class CObject_Pool final : public CBase
{
private:
	CObject_Pool() = default;
	virtual ~CObject_Pool() = default;

public:
	HRESULT Initialize(_uint _iSize, CGameObject* _pPrototype, void* _pArg, const wstring& _wstrLayerTag);

	void Push(CGameObject* _pObject);
	CGameObject* Pop(any _args = 0);
	CGameObject* Get(any _args = 0);

public:
	static CObject_Pool* Create(_uint _iSize, CGameObject* _pPrototype, void* _pArg = nullptr, const wstring& _wstrLayerTag = TEXT("Layer_ObjectPool"));
	virtual void Free() override;

private:
	stack<CGameObject*> m_objects;
	wstring m_wstrLayerTag;
};

END