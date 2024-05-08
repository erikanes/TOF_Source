#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CGameModeImpl abstract : public CBase
{
protected:
	CGameModeImpl(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CGameModeImpl() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_float _fTimeDelta);
	virtual void Late_Tick(_float _fTimeDelta);
	virtual HRESULT Render();
	virtual void Reset();
	virtual void Begin();
	virtual void End();

public:
	virtual void Free() override;
	virtual void Execute_Event_Function(EventMsg _tEventMsg) {};

protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
};

END