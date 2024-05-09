#include "Client_pch.h"
#include "Client_Defines.h"
#include "Timeillusion.h"
#include "GameInstance.h"
#include "Render_Manager.h"

#include "Effect_Coll.h"
#include "Layer.h"

/*
CTimeillusion::CTimeillusion(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CSkillObject(_pDevice, _pContext)
{
}

CTimeillusion::CTimeillusion(const CTimeillusion& _rhs)
	: CSkillObject(_rhs)
{
}


CGameObject* CTimeillusion::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CTimeillusion* pInstance = new CTimeillusion(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to create : CTimeillusion");
	}

	return pInstance;
}

HRESULT CTimeillusion::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTimeillusion::Initialize(void* _pArg)
{
	if (FAILED(__super::Initialize(_pArg)))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Components(_pArg)))
	{
		return E_FAIL;
	}

	m_eSkillType = SKILLTYPE::TIME_ILLUSION;
	m_fCurrentLifeTime = 5.f;

	CGameInstance::Get_Instance()->Play_Audio("Player_Skill_TimeIllusion_Begin");

	Create_Effect(L"Char_Time_Illusion_Scale25.Effect", nullptr, { 0.f, 0.f, 0.f }, false, false, nullptr, IDENTITY_MATRIX, &m_bDead);

	return S_OK;
}
*/

void CTimeillusion::OnCollisionBegin(CCollider* _pSelf, CCollider* _pOther, _float _fTimeDelta)
{
	auto pRender_Manager = CRender_Manager::Get_Instance();

	if (COLLISION_GROUP::PLAYER_BODY == _pOther->Get_CollisionGroup())
	{
		m_fBeforeMiddleGrey = pRender_Manager->Get_MiddleGrey();
		m_fBeforeLuminanceWhite = pRender_Manager->Get_LuminanceWhite();
		pRender_Manager->Set_MiddleGrey(0.65f);
	}
}

void CTimeillusion::OnCollisionEnd(CCollider* _pSelf, CCollider* _pOther, _float _fTimeDelta)
{
	auto pRender_Manager = CRender_Manager::Get_Instance();

	if (COLLISION_GROUP::PLAYER_BODY == _pOther->Get_CollisionGroup())
	{
		pRender_Manager->Set_MiddleGrey(m_fBeforeMiddleGrey);
		pRender_Manager->Set_LuminanceWhite(m_fBeforeLuminanceWhite);
	}
}

/*
HRESULT CTimeillusion::Add_Components(void* _pArg)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof ColliderDesc);

	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	ColliderDesc.fRadius = m_fCollisionRadius;
	ColliderDesc.vRotation = _float3(0.0f, 0.f, 0.f);

	CComponent* pCollider = pGameInstance->Add_ColliderComponent(m_eLevel,
		TEXT("Prototype_Component_Collider_Sphere"), this, COLLISION_GROUP::PLAYER_SKILL_AREA, &ColliderDesc);

	Register_ColliderCom(L"Skill_Area", pCollider);

	if (nullptr == pCollider)
		return E_FAIL;

	m_pColliderCom = static_cast<CCollider*>(pCollider);
	m_pColliderCom->Set_Enable(true);

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}
*/

void CTimeillusion::_Set_MonsterLayer_TimeScale(_float _fTimeScale)
{
	CGameInstance* pGameInstnace = CGameInstance::Get_Instance();

	auto mapLayer = pGameInstnace->Get_SpecificLayer(m_eLevel);
	auto iter = mapLayer->find(LAYER_MONSTER_OBJECTPOOL);

	Vector4 vTimeillusionCurrentPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (mapLayer->end() != iter)
	{
		auto& pListEffect = iter->second->GetObjectList();

		for (auto& pObj : pListEffect)
		{
			CE_Effect_Coll* pEffect = static_cast<CE_Effect_Coll*>(pObj);

			const CE_Effect_Coll::CLONEEFFECTDESC& tEffectDesc = pEffect->Get_Cloned_Effect_Desc();

			Vector4 vEffectPosition = tEffectDesc.vPos;
			Vector4 vDirection = vEffectPosition - vTimeillusionCurrentPosition;

			if (vDirection.Length() <= m_fCollisionRadius)
				pEffect->Set_Time_Scale(_fTimeScale);
		}
	}
}

void CTimeillusion::Tick(_float _fTimeDelta)
{
	__super::Tick(_fTimeDelta);

	// For. effect handling
	_Set_MonsterLayer_TimeScale(0.1f);

	m_fCurrentLifeTime -= _fTimeDelta;

	if (0.f >= m_fCurrentLifeTime)
	{
		Set_Dead(true);

		_Set_MonsterLayer_TimeScale(1.f);

		CGameInstance::Get_Instance()->Play_Audio("Player_Skill_TimeIllusion_End");
	}
}

/*
void CTimeillusion::Late_Tick(_float _fTimeDelta)
{
	__super::Late_Tick(_fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CTimeillusion::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

CGameObject* CTimeillusion::Clone(void* _pArg)
{
	CTimeillusion* pInstance = new CTimeillusion(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CTimeillusion");
	}

	return pInstance;
}

void CTimeillusion::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
}
*/