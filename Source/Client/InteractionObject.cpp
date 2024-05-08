#include "Client_pch.h"
#include "InteractionObject.h"

// Singleton
#include "GameInstance.h"
#include "Data_Manager.h"
#include "Item_Manager.h"
#include "Quest_Manager.h"

// Effect
#include "Effect_Coll.h"

#include "Client_Functions.h"
#include "Engine_Functions.h"

CInteractionObject::CInteractionObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject(_pDevice, _pContext)
{
	m_eType = OBJECT_TYPE::INTERACTION;
}

CInteractionObject::CInteractionObject(const CInteractionObject& _rhs)
	: CGameObject(_rhs)
{
}

HRESULT CInteractionObject::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CInteractionObject::Initialize(void* _pArg)
{
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	if (FAILED(_Add_Components(_pArg)))
		return E_FAIL;

	CQuest_Manager::Get_Instance()->Register_Observer(this);

	return S_OK;
}

void CInteractionObject::Tick(_float _fTimeDelta)
{
	__super::Tick(_fTimeDelta);

	if (nullptr != m_pColliderCom)
		m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
}

void CInteractionObject::Late_Tick(_float _fTimeDelta)
{
	__super::Late_Tick(_fTimeDelta);

#ifdef _DEBUG
	if (nullptr != m_pColliderCom)
		m_pRendererCom->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CInteractionObject::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(CInteractionObject::Bind_ShaderResource()))
		return E_FAIL;

	return S_OK;
}

void CInteractionObject::OnCollisionBegin(CCollider* _pSelf, CCollider* _pOther, _float _fTimeDelta)
{
}

void CInteractionObject::OnCollision(CCollider* _pSelf, CCollider* _pOther, _float _fTimeDelta)
{
}

void CInteractionObject::OnCollisionEnd(CCollider* _pSelf, CCollider* _pOther, _float _fTimeDelta)
{
	if (COLLISION_GROUP::INTERACTION == _pSelf->Get_CollisionGroup())
		Disable_Popup_UI();
}

void CInteractionObject::Interaction()
{
	CFunc::Play_Audio_UI("UI_Click_4");
}

void CInteractionObject::Take_Notify(OBSERVER_NOTIFY_TYPE _eType, any _arg)
{
}

HRESULT CInteractionObject::Bind_ShaderResource()
{
	auto pGameInstance = CGameInstance::Get_Instance();

	auto matView = pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
	auto matProj = pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ);

	auto fCameraFar = pGameInstance->Get_CameraFar();

	m_pShaderCom->Bind("g_ViewMatrix", matView);
	m_pShaderCom->Bind("g_ProjMatrix", matProj);
	m_pShaderCom->Bind("g_WorldMatrix", m_matWorld);
	m_pShaderCom->Bind("g_fCameraFar", fCameraFar);

	return S_OK;
}

void CInteractionObject::Popup_UI_Item(_uint _iID)
{
	auto pGameInstance = CGameInstance::Get_Instance();

	Engine::EventMsg msg;

	const auto& tInfo = CItem_Manager::Get_Instance()->Get_ItemInfo(_iID);

	if (0 == tInfo.iID)
		return;

	msg.eEventType = EVENT_TYPE::UI_INTERACTION_ENABLE;
	msg.wParam = m_eLevel;
	msg.lParam = INTER_GATHERING;
	msg.cParam = wstring(LAYER_UI_INTERACTIONINFO);
	msg.sParam = tInfo.wstrName;

	pGameInstance->Add_EventMsg(msg);
}

void CInteractionObject::Popup_UI_NPC(_uint _iID)
{
}

void CInteractionObject::Popup_UI_Interaction(wstring _wstrMessage)
{
	auto pGameInstance = CGameInstance::Get_Instance();

	Engine::EventMsg msg;

	msg.eEventType = EVENT_TYPE::UI_INTERACTION_ENABLE;
	msg.wParam = m_eLevel;
	msg.lParam = INTER_GATHERING;
	msg.cParam = wstring(LAYER_UI_INTERACTIONINFO);
	msg.sParam = _wstrMessage;

	pGameInstance->Add_EventMsg(msg);
}

void CInteractionObject::Disable_Popup_UI()
{
	auto pGameInstance = CGameInstance::Get_Instance();

	Engine::EventMsg msg;

	msg.eEventType = EVENT_TYPE::UI_DISABLE;
	msg.wParam = m_eLevel;
	msg.cParam = wstring(LAYER_UI_INTERACTIONINFO);

	pGameInstance->Add_EventMsg(msg);
}

/*
CGameObject* CInteractionObject::Create_FieldItem_Effect(wstring _wstrEffect, _float _fCameraDistance, _float3 _vFixPos, _bool* _pDead)
{
	CE_Effect_Coll::CLONEEFFECTDESC desc;

	desc.pWorldMatrix = nullptr;
	desc.bMaintainPointerPos = false;
	desc.bMaintainPivotPointerPos = false;
	desc.pBoneMatrix = nullptr;
	desc.PivotMatrix = IDENTITY_MATRIX;
	desc.vPointerFixPos = _float3(0.f, 0.f, 0.f);

	auto matWorld = m_pTransformCom->Get_WorldFloat4x4();
	
	_float3 vRight, vUp, vLook;
	_float4 vPos;

	memcpy_s(&vRight, sizeof(_float3), &matWorld.m[0], sizeof(_float3));
	memcpy_s(&vUp, sizeof(_float3), &matWorld.m[1], sizeof(_float3));
	memcpy_s(&vLook, sizeof(_float3), &matWorld.m[2], sizeof(_float3));
	memcpy_s(&vPos, sizeof(_float4), &matWorld.m[3], sizeof(_float4));

	auto vEffectPos = XMLoadFloat4(&vPos)
		+ XMVector3Normalize(XMLoadFloat3(&vRight)) * _vFixPos.x
		+ XMVector3Normalize(XMLoadFloat3(&vUp)) * _vFixPos.y
		+ XMVector3Normalize(XMLoadFloat3(&vLook)) * _vFixPos.z;

	auto vCameraPos = CGameInstance::Get_Instance()->Get_CamPosition();

	vEffectPos = vEffectPos + _fCameraDistance * XMVector3Normalize(XMLoadFloat4(&vCameraPos) - vEffectPos);

	XMStoreFloat4(&desc.vPos, vEffectPos);

	desc.vLook = _float3(0.f, 0.f, 1.f);
	desc.vRight = _float3(1.f, 0.f, 0.f);
	desc.pDead = _pDead;

	auto pEffect = CGameInstance::Get_Instance()->Get_FromObjectPool(_wstrEffect, &desc);

	return pEffect;
}

void CInteractionObject::Enable_FieldItem_Effect(CGameObject* _pObject, _float _fCameraDistance, _float3 _vFixPos, _bool* _pDead)
{
	if (nullptr == _pObject)
		return;

	CE_Effect_Coll::CLONEEFFECTDESC desc;

	desc.pWorldMatrix = nullptr;
	desc.bMaintainPointerPos = false;
	desc.bMaintainPivotPointerPos = false;
	desc.pBoneMatrix = nullptr;
	desc.PivotMatrix = IDENTITY_MATRIX;
	desc.vPointerFixPos = _float3(0.f, 0.f, 0.f);

	auto matWorld = m_pTransformCom->Get_WorldFloat4x4();

	_float3 vRight, vUp, vLook;
	_float4 vPos;

	memcpy_s(&vRight, sizeof(_float3), &matWorld.m[0], sizeof(_float3));
	memcpy_s(&vUp, sizeof(_float3), &matWorld.m[1], sizeof(_float3));
	memcpy_s(&vLook, sizeof(_float3), &matWorld.m[2], sizeof(_float3));
	memcpy_s(&vPos, sizeof(_float4), &matWorld.m[3], sizeof(_float4));

	auto vEffectPos = XMLoadFloat4(&vPos)
		+ XMVector3Normalize(XMLoadFloat3(&vRight)) * _vFixPos.x
		+ XMVector3Normalize(XMLoadFloat3(&vUp)) * _vFixPos.y
		+ XMVector3Normalize(XMLoadFloat3(&vLook)) * _vFixPos.z;

	auto vCameraPos = CGameInstance::Get_Instance()->Get_CamPosition();

	vEffectPos = vEffectPos + _fCameraDistance * XMVector3Normalize(XMLoadFloat4(&vCameraPos) - vEffectPos);

	XMStoreFloat4(&desc.vPos, vEffectPos);

	desc.vLook = _float3(0.f, 0.f, 1.f);
	desc.vRight = _float3(1.f, 0.f, 0.f);
	desc.pDead = _pDead;

	_pObject->Enable();
	_pObject->Set_Dead(false);
	_pObject->Begin(&desc);
}

HRESULT CInteractionObject::Create_Effect(wstring _wstrEffect, const _float4x4* _pWorldMatrix, _float3 _vFixPos, _bool _bMaintainPointerPos, _bool _bMaintainPivotPointerPos, const _float4x4* _pBoneMatrix, _float4x4 _PivotMatrix, _bool* _pDead, _bool _bMaintainWorldMatrix)
{
	CE_Effect_Coll::CLONEEFFECTDESC desc;
	desc.pWorldMatrix = _pWorldMatrix;
	desc.bMaintainPointerPos = _bMaintainPointerPos;
	desc.bMaintainPivotPointerPos = _bMaintainPivotPointerPos;
	desc.bMaintainWorldMatrix = _bMaintainWorldMatrix;

	desc.vPointerFixPos = _vFixPos;
	desc.pBoneMatrix = _pBoneMatrix;
	desc.PivotMatrix = _PivotMatrix;

	auto matWorld = m_pTransformCom->Get_WorldFloat4x4();
	_float3 vRight, vUp, vLook;
	_float4 vPos;

	memcpy_s(&vRight, sizeof(_float3), &matWorld.m[0], sizeof(_float3));
	memcpy_s(&vUp, sizeof(_float3), &matWorld.m[1], sizeof(_float3));
	memcpy_s(&vLook, sizeof(_float3), &matWorld.m[2], sizeof(_float3));
	memcpy_s(&vPos, sizeof(_float4), &matWorld.m[3], sizeof(_float4));

	XMStoreFloat4(&desc.vPos, XMLoadFloat4(&vPos)
		+ XMVector3Normalize(XMLoadFloat3(&vRight)) * _vFixPos.x
		+ XMVector3Normalize(XMLoadFloat3(&vUp)) * _vFixPos.y
		+ XMVector3Normalize(XMLoadFloat3(&vLook)) * _vFixPos.z);

	desc.vLook = vLook;
	desc.vRight = vRight;
	desc.pDead = _pDead;

	if (nullptr == CGameInstance::Get_Instance()->Pop_FromObjectPool(_wstrEffect, &desc))
		return E_FAIL;

	return S_OK;
}

CGameObject* CInteractionObject::Create_Effect_Ptr(wstring _wstrEffect, const _float4x4* _pWorldMatrix, _float3 _vFixPos, _bool _bMaintainPointerPos, _bool _bMaintainPivotPointerPos, const _float4x4* _pBoneMatrix, _float4x4 _PivotMatrix, _bool* _pDead, _bool _bMaintainWorldMatrix)
{
	CE_Effect_Coll::CLONEEFFECTDESC desc;
	desc.pWorldMatrix = _pWorldMatrix;
	desc.bMaintainPointerPos = _bMaintainPointerPos;
	desc.bMaintainPivotPointerPos = _bMaintainPivotPointerPos;
	desc.bMaintainWorldMatrix = _bMaintainWorldMatrix;

	desc.vPointerFixPos = _vFixPos;
	desc.pBoneMatrix = _pBoneMatrix;
	desc.PivotMatrix = _PivotMatrix;

	auto matWorld = m_pTransformCom->Get_WorldFloat4x4();
	_float3 vRight, vUp, vLook;
	_float4 vPos;

	memcpy_s(&vRight, sizeof(_float3), &matWorld.m[0], sizeof(_float3));
	memcpy_s(&vUp, sizeof(_float3), &matWorld.m[1], sizeof(_float3));
	memcpy_s(&vLook, sizeof(_float3), &matWorld.m[2], sizeof(_float3));
	memcpy_s(&vPos, sizeof(_float4), &matWorld.m[3], sizeof(_float4));

	XMStoreFloat4(&desc.vPos, XMLoadFloat4(&vPos)
		+ XMVector3Normalize(XMLoadFloat3(&vRight)) * _vFixPos.x
		+ XMVector3Normalize(XMLoadFloat3(&vUp)) * _vFixPos.y
		+ XMVector3Normalize(XMLoadFloat3(&vLook)) * _vFixPos.z);

	desc.vLook = vLook;
	desc.vRight = vRight;
	desc.pDead = _pDead;

	CGameObject* pGameObejct = CGameInstance::Get_Instance()->Pop_FromObjectPool(_wstrEffect, &desc);

	return pGameObejct;
}
*/

_bool CInteractionObject::IsIn_Distance_To_Camera()
{
	auto pGameInstance = CGameInstance::Get_Instance();

	Vector4 vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	Vector4 vCamPosition = pGameInstance->Get_CamPosition();

	auto fDistanceSquared = (vPosition - vCamPosition).LengthSquared();

	if (300.f * 300.f >= fDistanceSquared)
		return true;

	else
		return false;
}

HRESULT CInteractionObject::_Add_Components(void* _pArg)
{
	m_eLevel = CFunc::Get_CurLevel();

	if (FAILED(__super::Add_Component(LEVEL_STATIC, PC_TRANSFORM, TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, PC_RENDERER, TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// 셰이더, 콜라이더, 모델은 상속받은 쪽에서 초기화 하기

	return S_OK;
}

void CInteractionObject::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);

	Safe_Release(m_pFieldEffect);

	CQuest_Manager::Get_Instance()->Remove_Observer(this);
}
