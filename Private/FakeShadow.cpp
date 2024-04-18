#include "Client_pch.h"
#include "Client_Defines.h"
#include "FakeShadow.h"

// Singleton
#include "GameInstance.h"

// Component
#include "VIBuffer_Point.h"

#include "Engine_Functions.h"

CFakeShadow::CFakeShadow(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject(_pDevice, _pContext)
{
}

CFakeShadow::CFakeShadow(const CFakeShadow& _rhs)
	: CGameObject(_rhs)
{
}

HRESULT CFakeShadow::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFakeShadow::Initialize(void* _pArg)
{
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	if (FAILED(_Add_Components(_pArg)))
		return E_FAIL;

	return S_OK;
}

void CFakeShadow::Tick(_float _fTimeDelta)
{
	__super::Tick(_fTimeDelta);
}

void CFakeShadow::Late_Tick(_float _fTimeDelta)
{
	__super::Late_Tick(_fTimeDelta);

	if (m_pOwner != nullptr && m_pOwner->Get_Dead() && !m_bDead)
	{
		m_pOwner = nullptr;
		m_pOwnerTransformCom = nullptr;

		Set_Dead(true);
	}

	if (m_funcCondition())
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
}

HRESULT CFakeShadow::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(_Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;
	
	m_pShaderCom->Begin(0);
	
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CFakeShadow::_Add_Components(void* _pArg)
{
	m_eLevel = CFunc::Get_CurLevel();

	if (FAILED(__super::Add_Component(LEVEL_STATIC, PC_RENDERER, TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_eLevel, PC_SHADER_FAKESHADOW, TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, PC_VIBUFFER_POINT, TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_eLevel, PC_TEXTURE_FAKESHADOW, TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	auto pDesc = static_cast<FAKESHADOW_DESC*>(_pArg);

	m_pOwner = pDesc->pOwner;
	m_vShadowSize = pDesc->vSize;
	m_funcCondition = pDesc->funcCondition;

	if (nullptr != m_pOwner)
		m_pOwnerTransformCom = static_cast<CTransform*>(m_pOwner->Find_Component(TEXT("Com_Transform")));

	return S_OK;
}

HRESULT CFakeShadow::_Bind_ShaderResources()
{
	if (nullptr == m_pOwnerTransformCom)
		return E_FAIL;

	auto pGameInstance = CGameInstance::Get_Instance();

	auto matView = pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
	auto matProj = pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ);

	m_pShaderCom->Bind("g_WorldMatrix", m_pOwnerTransformCom->Get_WorldFloat4x4());
	m_pShaderCom->Bind("g_ViewMatrix", matView);
	m_pShaderCom->Bind("g_ProjMatrix", matProj);
	m_pShaderCom->Bind("g_vSize", m_vShadowSize);

	return S_OK;
}

CFakeShadow* CFakeShadow::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	auto pInstance = new CFakeShadow(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created: CFakeShadow::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFakeShadow::Clone(void* _pArg)
{
	auto pInstance = new CFakeShadow(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned: CFakeShadow::Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFakeShadow::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);

	m_pOwner = nullptr;
	m_pOwnerTransformCom = nullptr;
}
