#include "Client_pch.h"
#include "Client_Defines.h"
#include "Cloud.h"

// Singleton
#include "GameInstance.h"

// Component
#include "VIBuffer_Cloud.h"
#include "Texture.h"

#include "Engine_Functions.h"

CCloud::CCloud(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject(_pDevice, _pContext)
{
}

CCloud::CCloud(const CCloud& _rhs)
	: CGameObject(_rhs)
{
}

HRESULT CCloud::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCloud::Initialize(void* _pArg)
{
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	if (FAILED(_Add_Components(_pArg)))
		return E_FAIL;

	return S_OK;
}

void CCloud::Tick(_float _fTimeDelta)
{
	__super::Tick(_fTimeDelta);

	m_fAccumulatedTime += _fTimeDelta * 0.002f;

	if (m_fAccumulatedTime > 1.f)
		m_fAccumulatedTime -= 1.f;
}

void CCloud::Late_Tick(_float _fTimeDelta)
{
	__super::Late_Tick(_fTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	_float4 camPosition = pGameInstance->Get_CamPosition();
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&camPosition));

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);
}

HRESULT CCloud::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(_Bind_ShaderResources()))
		return E_FAIL;

	m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_DiffuseTexture", 0);
	m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_MaskMapTexture", 1);
	m_pShaderCom->Bind("g_fTimeDelta", m_fAccumulatedTime);
	m_pShaderCom->Bind("g_fScale", 0.3f);
	m_pShaderCom->Bind("g_fBrightness", 0.5f);

	m_pShaderCom->Begin(0);
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CCloud::_Add_Components(void* _pArg)
{
	m_eLevel = CFunc::Get_CurLevel();

	if (FAILED(__super::Add_Component(LEVEL_STATIC, PC_TRANSFORM, TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, PC_RENDERER, TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_eLevel, PC_SHADER_CLOUD, TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_eLevel, PC_VIBUFFER_CLOUD, TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_eLevel, PC_TEXTURE_CLOUD, TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	XMStoreFloat4x4(&m_matWorld, XMMatrixIdentity());
	m_pTransformCom->Set_WorldMatrix(m_matWorld);

	return S_OK;
}

HRESULT CCloud::_Bind_ShaderResources()
{
	auto pGameInstance = CGameInstance::Get_Instance();

	auto matView = pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
	auto matProj = pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ);

	m_pShaderCom->Bind("g_ViewMatrix", matView);
	m_pShaderCom->Bind("g_ProjMatrix", matProj);
	m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix");

	auto fCameraFar = pGameInstance->Get_CameraFar();
	auto vCameraPosition = pGameInstance->Get_CamPosition();

	m_pShaderCom->Bind("g_fCameraFar", fCameraFar);
	m_pShaderCom->Bind("g_vCamPosition", vCameraPosition);

	return S_OK;
}

CCloud* CCloud::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	auto pInstance = new CCloud(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created: CCloud::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCloud::Clone(void* _pArg)
{
	auto pInstance = new CCloud(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned: CCloud::Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCloud::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
}