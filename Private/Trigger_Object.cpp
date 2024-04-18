#include "Client_pch.h"
#include "Client_Defines.h"
#include "Trigger_Object.h"

// Singleton
#include "GameInstance.h"
#include "Render_Manager.h"

// Component
#include "Transform.h"
#include "Collider.h"
#include "Renderer.h"

#include "Client_Functions.h"
#include "Engine_Functions.h"

CTrigger_Object::CTrigger_Object(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject(_pDevice, _pContext)
{
}

CTrigger_Object::CTrigger_Object(const CTrigger_Object& _rhs)
	: CGameObject(_rhs)
{
}

HRESULT CTrigger_Object::Initialize(void* _pArg)
{
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	if (FAILED(_Add_Components(_pArg)))
		return E_FAIL;

	auto pDesc = static_cast<TRIGGER_DESC*>(_pArg);

	Bind_Function(pDesc);

	return S_OK;
}

HRESULT CTrigger_Object::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_onCollisionBegin = [](CCollider*, CCollider*, _float) {};
	m_onCollision = [](CCollider*, CCollider*, _float) {};
	m_onCollisionEnd = [](CCollider*, CCollider*, _float) {};

	return S_OK;
}

void CTrigger_Object::Tick(_float _fTimeDelta)
{
	if (nullptr != m_pColliderCom)
		m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
}

void CTrigger_Object::Late_Tick(_float _fTimeDelta)
{
#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pColliderCom);
#endif
}

void CTrigger_Object::OnCollisionBegin(CCollider* _pSelf, CCollider* _pOther, _float _fTimeDelta)
{
	m_onCollisionBegin(_pSelf, _pOther, _fTimeDelta);
}

void CTrigger_Object::OnCollision(CCollider* _pSelf, CCollider* _pOther, _float _fTimeDelta)
{
	m_onCollision(_pSelf, _pOther, _fTimeDelta);
}

void CTrigger_Object::OnCollisionEnd(CCollider* _pSelf, CCollider* _pOther, _float _fTimeDelta)
{
	m_onCollisionEnd(_pSelf, _pOther, _fTimeDelta);
}

HRESULT CTrigger_Object::_Add_Components(void* _pArg)
{
	m_eLevel = CFunc::Get_CurLevel();

	TRIGGER_DESC* pDesc = static_cast<TRIGGER_DESC*>(_pArg);
	auto pGameInstance = CGameInstance::Get_Instance();

	if (FAILED(__super::Add_Component(LEVEL_STATIC, PC_TRANSFORM, TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	m_pTransformCom->Set_WorldMatrix(pDesc->matWorld);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, PC_RENDERER, TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC colliderDesc;
	ZeroMemory(&colliderDesc, sizeof(CCollider::COLLIDERDESC));

	colliderDesc.vSize = pDesc->vSize;
	colliderDesc.fRadius = pDesc->vSize.x;
	colliderDesc.iPriorityLevel = m_eLevel;

	CComponent* pCollider = nullptr;

	if (pDesc->bIsAABB)
		pCollider = pGameInstance->Add_ColliderComponent(m_eLevel, PC_COLLIDER_AABB, this, COLLISION_GROUP::TRIGGER, &colliderDesc);
	else
		pCollider = pGameInstance->Add_ColliderComponent(m_eLevel, PC_COLLIDER_SPHERE, this, COLLISION_GROUP::TRIGGER, &colliderDesc);
	
	if (nullptr == pCollider)
		return E_FAIL;

	m_pColliderCom = static_cast<CCollider*>(pCollider);

	m_pColliderCom->Set_Enable(true);
	Register_ColliderCom(TEXT("Com_Collider"), m_pColliderCom);

	return S_OK;
}

void CTrigger_Object::Bind_Function(const TRIGGER_DESC* _pDesc)
{
	m_onCollisionBegin = [](CCollider*, CCollider*, _float) {};
	m_onCollision = [](CCollider*, CCollider*, _float) {};
	m_onCollisionEnd = [](CCollider*, CCollider*, _float) {};

	const auto& wParam = _pDesc->wParam;
	const auto& lParam = _pDesc->lParam;

	auto pGameInstance = CGameInstance::Get_Instance();
	auto iCurrentLevel = static_cast<_uint>(pGameInstance->Get_CurLevel());

	// enable change location UI
	if (0 == wParam)
	{
		// lParam = Texture index

		m_onCollisionBegin = [this, iTextureIndex = lParam](CCollider* _pSelf, CCollider* _pOther, _float _fTimeDelta)
		{
			auto eCurrentLevel = CFunc::Get_CurLevel();
			CGameInstance::Get_Instance()->Enable_UI(eCurrentLevel, LAYER_UI_REGIONNAME, iTextureIndex);

			if (LEVEL_ABYSS == m_eLevel)
				m_pColliderCom->Set_Enable(false);
		};
	}

	// level change
	else if (1 == wParam)
	{
		m_onCollisionBegin = [this, eNextLevel = lParam](CCollider* _pSelf, CCollider* _pOther, _float _fTimeDelta)
		{
			auto eCurrentLevel = CFunc::Get_CurLevel();

			m_pColliderCom->Set_Enable(false);
			CRender_Manager::Get_Instance()->Begin_Fade(CRender_Manager::FADE_OUT, 0.3f);

			auto func = [this, eCurrentLevel, eNextLevel](_float)
			{
				CFunc::Add_Event_ChangeLevel(eCurrentLevel, eNextLevel);
			};

			CFunc::Add_Event_Inteval_Function(0.5f, 0.f, 1, func, this, false);
		};
	}

	// ... more binding
}

CTrigger_Object* CTrigger_Object::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	auto pInstance = new CTrigger_Object(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created::CTrigger_Object::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTrigger_Object::Clone(void* _pArg)
{
	auto pInstance = new CTrigger_Object(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned::CTrigger_Object::Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTrigger_Object::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pColliderCom);
}