#include "Client_pch.h"
#include "Client_Defines.h"
#include "FieldItem.h"

// Singleton
#include "GameInstance.h"
#include "Item_Manager.h"
#include "UI_Manager.h"
#include "Quest_Manager.h"

// Component
#include "Transform.h"
#include "Renderer.h"
#include "Collider.h"
#include "Shader.h"
#include "Model.h"
#include "Mesh.h"

#include "Inventory.h"

#include "Engine_Functions.h"
#include "Client_Functions.h"

#include "Effect_Coll.h"

CFieldItem::CFieldItem(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CInteractionObject(_pDevice, _pContext)
{
	m_eType = OBJECT_TYPE::INTERACTION;
}

CFieldItem::CFieldItem(const CFieldItem& _rhs)
	: CInteractionObject(_rhs)
{
}

HRESULT CFieldItem::Initialize(void* _pArg)
{
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	if (FAILED(_Add_Components(_pArg)))
		return E_FAIL;

	// 툴에 의해 맵에 배치된다.
	// 툴에서 전달받을 값은 충돌체의 크기, 아이템 ID, 획득 개수(기본 1), 모델 원본 태그or이름
	// 특별한 일이 없으면 위치가 변동될일은 없다.

	m_fSpawnTime = 10.f;

	//이펙트 추가
	m_pFieldEffect = Create_FieldItem_Effect(TEXT("Filed_Item.Effect"), 0.1f, _float3(0.f, 1.f, 0.f));
	m_fEffectTime = 2.f + (rand() % 10) * 0.15f;
	return S_OK;
}

HRESULT CFieldItem::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

void CFieldItem::Tick(_float _fTimeDelta)
{
	__super::Tick(_fTimeDelta);

	if (!IsEnable())
	{
		m_fProgressTime += _fTimeDelta;

		if (m_fSpawnTime <= m_fProgressTime)
		{
			m_bEnable = true;
			m_fProgressTime = 0.f;
		}
	}

	if (nullptr == m_pFieldEffect)
		return;
	
	if(true == m_pFieldEffect->IsEnable())
		m_pFieldEffect->Tick(_fTimeDelta);
	else
	{
		m_fEffectTime -= _fTimeDelta;

		if (0.f > m_fEffectTime)
		{
			m_fEffectTime = 2.f + (rand() % 10) * 0.15f;
			Enable_FieldItem_Effect(m_pFieldEffect, 0.1f, _float3(0.f, m_fEffectPosY, 0.f));
		}
	}

	if (true == m_pFieldEffect->Get_Dead())
		m_pFieldEffect->Disable();
}

void CFieldItem::Late_Tick(_float _fTimeDelta)
{
	__super::Late_Tick(_fTimeDelta);

	if (!IsEnable())
		return;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

	if (nullptr == m_pFieldEffect)
		return;

	if (true == m_pFieldEffect->IsEnable())
		m_pFieldEffect->Late_Tick(_fTimeDelta);
}

HRESULT CFieldItem::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(Bind_ShaderResource()))
		return E_FAIL;

	m_pShaderCom->Bind("g_bIsInstanced", false);

	auto iNumMeshes = m_pModelCom->Get_NumMesh();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_ShaderResourceView(m_pShaderCom, i);
		m_pShaderCom->Begin(0);
		m_pModelCom->Render(i);
	}

	return S_OK;
}

void CFieldItem::OnCollisionBegin(CCollider* _pSelf, CCollider* _pOther, _float _fTimeDelta)
{
	Popup_UI_Item(m_iItemID);
}

void CFieldItem::Interaction()
{
	__super::Interaction();

	if (SUCCEEDED(Rooting_Item()))
		CFunc::Remove_QuestDisplay(this);
}

void CFieldItem::Take_Notify(OBSERVER_NOTIFY_TYPE _eType, any _arg)
{
	if (OBSERVER_NOTIFY_TYPE::QUEST_BEGIN == _eType)
	{
		auto iBeginQuestID = any_cast<_uint>(_arg);

		const auto& quest = CQuest_Manager::Get_Instance()->Get_QuestDesc(iBeginQuestID);

		if (quest.iTargetID == m_iItemID)
			CFunc::Add_QuestDisplay(this, 1.f);
	}

	else if (OBSERVER_NOTIFY_TYPE::QUEST_ACHIEVEMENT == _eType)
	{
		auto iAchievementQuestID = any_cast<_uint>(_arg);

		const auto& quest = CQuest_Manager::Get_Instance()->Get_QuestDesc(iAchievementQuestID);

		if (quest.iTargetID == m_iItemID)
			CFunc::Remove_QuestDisplay(this);
	}
}

HRESULT CFieldItem::Rooting_Item()
{
	/*
	- 들어갈 자리가 있는지 인벤토리 탐색
	- 들어갈 자리가 있으면
		- 아이템 획득 UI 요청
		- 인벤토리에 아이템 추가
		- 오브젝트 비활성화

	- 들어갈 자리가 없으면
		- 아이템 획득 불가 UI 요청
	*/

	auto pInventory = CInventory::Get_Instance();

	auto iCount = CFunc::Generate_Random_Int(1, 5);

	if (FAILED(pInventory->Add_Item(m_iItemID, iCount)))
		return E_FAIL;

	auto pGameInstance = CGameInstance::Get_Instance();
	auto iCurrentLevel = static_cast<_uint>(pGameInstance->Get_CurLevel());

	SYSTEMINFO_GAIN_ITEM(m_iItemID, iCount)

	Engine::EventMsg msg;

	msg.eEventType = EVENT_TYPE::UI_DISABLE;
	msg.wParam = iCurrentLevel;
	msg.cParam = wstring(LAYER_UI_INTERACTIONINFO);

	pGameInstance->Add_EventMsg(msg);

	Disable();

	Create_Effect(TEXT("Filed_Item_Get_Scale105.Effect"), nullptr, {0.f, 0.f, 0.f});

	return S_OK;
}

HRESULT CFieldItem::Bind_ShaderResource()
{
	auto pGameInstance = CGameInstance::Get_Instance();

	auto vCamPosition = pGameInstance->Get_CamPosition();
	auto vCamForward = pGameInstance->Get_CamForward();

	auto pLight = pGameInstance->Get_Light(0);

	if (nullptr != pLight)
	{
		Vector4 vLightDir = pLight->vDirection;
		vLightDir.Normalize();

		m_pShaderCom->Bind("g_vLightDirection", vLightDir);
	}

	m_pShaderCom->Bind("g_vCamPosition", vCamPosition);
	m_pShaderCom->Bind("g_vCamDirection", vCamForward);

	m_pShaderCom->Bind("g_fAlpha", 1.f);
	m_pShaderCom->Bind("g_fAlpha_Discard", m_fAlphaDiscard);

	return S_OK;
}

HRESULT CFieldItem::_Add_Components(void* _pArg)
{
	auto pGameInstance = CGameInstance::Get_Instance();
	FIELD_ITEM_DESC* pDesc = static_cast<FIELD_ITEM_DESC*>(_pArg);

	_float fRadius = pDesc->fRadius * 2.f;
	_float fPivotSize = XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&pDesc->matPivot).r[0]));
	
	CCollider::COLLIDERDESC tColliderDesc;
	ZeroMemory(&tColliderDesc, sizeof(CCollider::COLLIDERDESC));

	tColliderDesc.fRadius = fRadius + (2.f / fPivotSize);
	tColliderDesc.vPosition = pDesc->vPosition;

	auto pCollider = pGameInstance->Add_ColliderComponent(m_eLevel, PC_COLLIDER_SPHERE, this, COLLISION_GROUP::INTERACTION, &tColliderDesc);

	if (nullptr == pCollider)
		return E_FAIL;

	m_pColliderCom = static_cast<CCollider*>(pCollider);

	m_pColliderCom->Set_Enable(true);
	Register_ColliderCom(TEXT("Com_Collider"), m_pColliderCom);

	wstring strModelTag = TEXT("Prototype_Component_Model_");

	char szFileName[MAX_PATH] = "";

	_splitpath_s(pDesc->strFullPath.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);

	strModelTag += wstring(szFileName, szFileName + strlen(szFileName));

	if (FAILED(Add_Component(m_eLevel, strModelTag, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (FAILED(Add_Component(m_eLevel, PC_SHADER_STATICOBJECT, TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;
	
	m_iItemID = pDesc->iID;

	m_matWorld = pDesc->matWorld;
	m_matPivot = pDesc->matPivot;

	XMStoreFloat4x4(&m_matWorld, XMLoadFloat4x4(&pDesc->matPivot) * XMLoadFloat4x4(&pDesc->matWorld));
	m_pTransformCom->Set_WorldMatrix(m_matWorld);

	m_iHash = CFunc::Generate_Hash(pDesc->strFullPath);

	m_fOriginVolumeRadius = fRadius;
	m_fVolumeSize = XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&pDesc->matPivot).r[0]));
	Calculate_VolumeRadius();

	_float3 vPivotRight;
	_float3 vWorldRight;
	memcpy(&vPivotRight, &pDesc->matPivot.m[0], sizeof _float3);
	memcpy(&vWorldRight, &pDesc->matWorld.m[0], sizeof _float3);
	m_fEffectPosY = pDesc->fRadius *XMVectorGetX( XMVector3Length(XMLoadFloat3(&vPivotRight))) * XMVectorGetX(XMVector3Length(XMLoadFloat3(&vWorldRight))) * 1.25f;

	return S_OK;
}

CFieldItem* CFieldItem::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	auto pInstance = new CFieldItem(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created: CFieldItem::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFieldItem::Clone(void* _pArg)
{
	auto pInstance = new CFieldItem(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned: CFieldItem::Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFieldItem::Free()
{
	__super::Free();
}
