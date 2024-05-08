#include "Engine_pch.h"
#include "GameMode.h"
#include "GameObject.h"
#include "GameInstance.h"

#include "GameModeImpl.h"

IMPLEMENT_SINGLETON(CGameMode);

CGameMode::CGameMode()
{

}

void CGameMode::Tick(_float _fTimeDelta)
{
	if (nullptr != m_pImpl)
		m_pImpl->Tick(_fTimeDelta);
}

void CGameMode::Late_Tick(_float _fTimeDelta)
{
	if (nullptr != m_pImpl)
		m_pImpl->Late_Tick(_fTimeDelta);
}

HRESULT CGameMode::Render()
{
	if (nullptr != m_pImpl)
		return m_pImpl->Render();

	return S_OK;
}

void CGameMode::Handling_Event(EventMsg _tEventMsg)
{
	if (nullptr != m_pImpl)
		m_pImpl->Execute_Event_Function(_tEventMsg);
}

void CGameMode::Set_GameMode(CGameModeImpl* _pGameModeImpl)
{
	if (nullptr != m_pImpl)
		m_pImpl->End();

	m_pImpl = _pGameModeImpl;

	m_pImpl->Begin();
}

void CGameMode::Change_GameMode(CGameModeImpl* _pGameModeImpl)
{
	if (nullptr != m_pImpl)
		m_pImpl->End();

	Safe_Release(m_pImpl);

	if (nullptr != _pGameModeImpl)
	{
		m_pImpl = _pGameModeImpl;

		m_pImpl->Begin();
	}
}

void CGameMode::Reset_GameMode()
{
	m_pImpl->Reset();
}

void CGameMode::Clear()
{
	Safe_Release(m_pImpl);
}

void CGameMode::Free()
{
	Safe_Release(m_pImpl);
}
