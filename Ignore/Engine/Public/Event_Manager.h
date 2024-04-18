#pragma once

#include "Base.h"

BEGIN(Engine)
class CEvent_Manager final
	: public CBase
{
	DECLARE_SINGLETON(CEvent_Manager);
private:
	CEvent_Manager();
	~CEvent_Manager() = default;

public:
	HRESULT Initialize(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	HRESULT Tick(_float _fTimeDelta);
	HRESULT Late_Tick(_float _fTimeDelta);
	HRESULT Add_EventMsg(EventMsg _tMsg);

	void Clear_IntervalFunction() { m_listIntervalFunctions.clear(); }

public:
	virtual void Free() override;

private:
	HRESULT Handling_EventMsg(EventMsg _tMsg);

private:
	deque<EventMsg> m_qEventMsg;

	deque<class CGameObject*> m_qDestroyInstances;

	list<INTERVAL_FUNCTION_DESC> m_listIntervalFunctions;

	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
};
END

