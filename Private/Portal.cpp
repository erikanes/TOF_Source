#include "Client_pch.h"
#include "Portal.h"

// Singleton
#include "GameInstance.h"
#include "Render_Manager.h"
#include "Quest_Manager.h"

// GameObject
#include "Map.h"
#include "PortalEffect.h"

// Component
#include "Mesh.h"

#include "Light.h"

#include "Engine_Functions.h"
#include "Client_Functions.h"

CPortal::CPortal(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CInteractionObject(_pDevice, _pContext)
{
}

CPortal::CPortal(const CPortal& _rhs)
	: CInteractionObject(_rhs)
{
}

HRESULT CPortal::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPortal::Initialize(void* _pArg)
{
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	if (FAILED(_Add_Components(_pArg)))
		return E_FAIL;

	m_pPortalEffect = CPortalEffect::Create(m_pDevice, m_pContext);
	
	if (nullptr == m_pPortalEffect)
		return E_FAIL;

	auto matWorld = m_pTransformCom->Get_WorldFloat4x4();

	if (FAILED(m_pPortalEffect->Initialize(&matWorld)))
		return E_FAIL;

	if (LEVEL_ABYSS == m_eLevel)
	{
		m_bEnable = false;
		m_pColliderCom->Set_Enable(false);

		m_iShaderPass = 4;
		m_fDissolveAlpha = 0.f;
	}
	else
	{
		m_iShaderPass = 0;
		m_fDissolveAlpha = 1.f;
	}

	return S_OK;
}

void CPortal::Tick(_float _fTimeDelta)
{
	if (!m_bEnable)
		return;

	__super::Tick(_fTimeDelta);

	if (m_eLevel == LEVEL_ABYSS)
		m_fDissolveAlpha = clamp(m_fDissolveAlpha + _fTimeDelta , 0.f, 1.f);

	m_pPortalEffect->Tick(_fTimeDelta);
}

void CPortal::Late_Tick(_float _fTimeDelta)
{
	if (!m_bEnable)
		return;

	__super::Late_Tick(_fTimeDelta);

	m_pPortalEffect->Late_Tick(_fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CPortal::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(Bind_ShaderResource()))
		return E_FAIL;

	m_pShaderCom->Bind("g_bIsInstanced", false);

	auto iNumMesh = m_pModelCom->Get_NumMesh();

	for (_uint i = 0; i < iNumMesh; ++i)
	{
		if (FAILED(m_pModelCom->Bind_ShaderResourceView(m_pShaderCom, i)))
			return E_FAIL;

		m_pShaderCom->Begin(m_iShaderPass);

		m_pModelCom->Render(i);
	}

	if (LEVEL_DESERT == m_eLevel)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);

	return S_OK;
}

HRESULT CPortal::Render_Shadow(list<CLight*>& _lights)
{
	m_pShaderCom->Bind("g_bIsInstanced", false);
	m_pShaderCom->Bind("g_fAlpha_Discard", 0.f);

	_uint iNumMeshes = m_pModelCom->Get_NumMesh();
	auto& vecMeshes = m_pModelCom->Get_vMeshes();

	m_pShaderCom->Bind("g_WorldMatrix", m_matWorld);

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_ShaderResourceView(m_pShaderCom, i);

		for (auto& pLight : _lights)
			pLight->Render_Shadow(m_pShaderCom, vecMeshes[i], 1);
	}

	return S_OK;
}

void CPortal::OnCollisionBegin(CCollider* _pSelf, CCollider* _pOther, _float _fTimeDelta)
{
	Popup_UI_Interaction(TEXT("포탈 작동"));
}

void CPortal::Take_Notify(OBSERVER_NOTIFY_TYPE _eType, any _arg)
{
	if (OBSERVER_NOTIFY_TYPE::QUEST_BEGIN == _eType)
	{
		auto iBeginQuestID = any_cast<_uint>(_arg);

		if (LEVEL_MIRRORCITY == m_eDestLevel)
		{
			switch (iBeginQuestID)
			{
			case 12: // 시리와 대화하기
			case 16: // 린에게 보고하기
				CFunc::Add_QuestDisplay(this, 3.f);
				break;
			}

			if (16 == iBeginQuestID)
			{
				m_bEnable = true;
				m_pColliderCom->Set_Enable(true);
			}
		}

		else if (LEVEL_ABYSS == m_eDestLevel)
		{
		}

		else if (LEVEL_DESERT == m_eDestLevel)
		{
			switch (iBeginQuestID)
			{
			case 5:
			case 14:
				CFunc::Add_QuestDisplay(this, 3.f);
				break;
			}
		}
	}
}

void CPortal::Interaction()
{
	//m_pColliderCom->Set_Enable(false);

	/*
	F키로 상호작용 시도 시 이동여부를 묻는 UI를 먼저 띄우기
	1. 확인시 페이드 아웃 이후 다음 레벨로 이동


	우선은 인터렉션 시도시 바로 레벨로 이동시켜보기	
	*/

	__super::Interaction();

	auto func = [srcLevel = m_eLevel, destLevel = m_eDestLevel](_float)
	{
		CFunc::Add_Event_ChangeLevel(srcLevel, destLevel);
	};

	CRender_Manager::Get_Instance()->Begin_Fade(CRender_Manager::FADE_OUT, 0.5f);
	CFunc::Add_Event_Inteval_Function(0.7f, 0.f, 1, func, this, false);
}

HRESULT CPortal::Bind_ShaderResource()
{
	auto pGameInstance = CGameInstance::Get_Instance();

	_float4 vCameraPosition = pGameInstance->Get_CamPosition();

	auto pLight = pGameInstance->Get_Light(0);

	if (nullptr != pLight)
	{
		if (FAILED(m_pShaderCom->Bind("g_vLightPosition", pLight->vPosition)))
			return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind("g_vCamPosition", vCameraPosition)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind("g_fAlpha", 1.f)))
		return E_FAIL;
	
	if (FAILED(m_pShaderCom->Bind("g_fAlpha_Discard", 0.f)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind("g_CameraNoiseTexture", pGameInstance->Get_Texture_SRV(LEVEL_STATIC, PC_TEXTURE_NOISE, 2))))
		return E_FAIL;

	if (LEVEL_ABYSS == m_eLevel)
	{
		auto pDissolve = pGameInstance->Get_Texture_SRV(LEVEL_STATIC, PC_TEXTURE_NOISE, 0);

		m_pShaderCom->Bind("g_fDissolveAlpha", m_fDissolveAlpha);
		m_pShaderCom->Bind("g_vDissolveColor", _float4(1.f, 1.f, 1.f, 1.f));
		m_pShaderCom->Bind("g_DissolveTexture", pDissolve);
	}

	return S_OK;
}

HRESULT CPortal::_Add_Components(void* _pArg)
{
	auto pGameInstance = CGameInstance::Get_Instance();

	auto pDesc = static_cast<CMap::MAPDESC*>(_pArg);

	// model, shader, collider 초기화

	// 포탈 모델과 더불어서 rect 버퍼, 텍스쳐도 추가해야 할 수도 있음
	// 셰이더도 하나 더 추가하던가.. 해야할지도


	// Shader
	if (FAILED(__super::Add_Component(m_eLevel, PC_SHADER_STATICOBJECT, TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// Model
	wstring wstrTag = TEXT("Prototype_Component_Model_");

	char szFileName[MAX_PATH] = "";

	_splitpath_s(pDesc->strModel_Absoulute_Path.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);

	wstring wstrFileName(szFileName, szFileName + strlen(szFileName));

	if (FAILED(__super::Add_Component(m_eLevel, wstrTag + wstrFileName, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	// Collider
	CCollider::COLLIDERDESC colliderDesc;
	ZeroMemory(&colliderDesc, sizeof(CCollider::COLLIDERDESC));

	colliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	colliderDesc.fRadius = 4.f;
	colliderDesc.vRotation = _float3(0.f, 0.f, 0.f);

	m_pColliderCom = static_cast<CCollider*>(pGameInstance->Add_ColliderComponent(m_eLevel, PC_COLLIDER_SPHERE, this, COLLISION_GROUP::INTERACTION, &colliderDesc));
	Register_ColliderCom(TEXT("Detection_Range"), m_pColliderCom);
	m_pColliderCom->Set_Enable(true);

	m_matWorld = pDesc->matWorldMatrix;
	m_matPivot = pDesc->matPivotMatrix;

	m_pTransformCom->Set_WorldMatrix(m_matWorld);
	XMStoreFloat4x4(&m_matWorld, XMLoadFloat4x4(&m_matPivot) * XMLoadFloat4x4(&m_matWorld));

	m_iHash = CFunc::Generate_Hash(pDesc->strModel_Absoulute_Path);

	m_fOriginVolumeRadius = pDesc->fSize * 5.f;
	m_fVolumeRadius = XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_matPivot).r[0]));
	Calculate_VolumeRadius();
	
	//pDesc->fAlpha;


	if (1101 == pDesc->iMaptype)
		m_eDestLevel = LEVEL_MIRRORCITY;

	else if (1102 == pDesc->iMaptype)
		m_eDestLevel = LEVEL_DESERT;

	else if (1103 == pDesc->iMaptype)
		m_eDestLevel = LEVEL_ABYSS;

	return S_OK;
}

CPortal* CPortal::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	auto pInstance = new CPortal(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created: CPortal::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPortal::Clone(void* _pArg)
{
	auto pInstance = new CPortal(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned: CPortal::Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPortal::Free()
{
	__super::Free();

	Safe_Release(m_pPortalEffect);
}
