#pragma once
#include "Base.h"

#include "Collider.h"

BEGIN(Engine)

class CGameModeImpl;

class ENGINE_DLL CGameMode final : public CBase
{
	DECLARE_SINGLETON(CGameMode);

private:
	CGameMode();
	virtual ~CGameMode() = default;

public:
	void Tick(_float _fTimeDelta);
	void Late_Tick(_float _fTimeDelta);
	HRESULT Render(); // TestCode
	virtual void Free() override;

	void Set_GameMode(CGameModeImpl* _pGameModeImpl);
	void Change_GameMode(CGameModeImpl* _pGameModeImpl);
	void Reset_GameMode();

	void Clear();
	
	void Handling_Event(EventMsg _tEventMsg);

private:
	CGameModeImpl* m_pImpl = { nullptr };
};

END