#include "Engine_pch.h"
#include "GameModeImpl.h"

CGameModeImpl::CGameModeImpl(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: m_pDevice(_pDevice), m_pContext(_pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CGameModeImpl::Initialize()
{
	return S_OK;
}

void CGameModeImpl::Tick(_float _fTimeDelta)
{
}

void CGameModeImpl::Late_Tick(_float _fTimeDelta)
{
}

void CGameModeImpl::Begin()
{

}

void CGameModeImpl::End()
{

}

HRESULT CGameModeImpl::Render()
{
	return S_OK;
}

void CGameModeImpl::Reset()
{
}

void CGameModeImpl::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}