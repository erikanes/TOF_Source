#include "Client_pch.h"
#include "Character_NPC.h"

// Singleton
#include "GameInstance.h"
#include "Render_Manager.h"
#include "Dialog_Manager.h"
#include "Quest_Manager.h"
#include "Data_Manager.h"

// GameObject
#include "Effect_Coll.h"

// Component
#include "Mesh.h"

#include "AnimationState.h"

#include "Engine_Functions.h"
#include "Client_Functions.h"
#include "Client_Math.h"

#include "Light.h"

CCharacter_NPC::CCharacter_NPC(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CCharacter(_pDevice, _pContext)
{
	m_eType = OBJECT_TYPE::NPC;
	m_iID = ID_NONE;
}


CCharacter_NPC::CCharacter_NPC(const CCharacter_NPC& _rhs)
	: CCharacter(_rhs)
{
}

HRESULT CCharacter_NPC::Initialize(void* _pArg)
{
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	if (FAILED(_Add_Components(_pArg)))
		return E_FAIL;

	CQuest_Manager::Get_Instance()->Register_Observer(this);

	if (LEVEL_DESERT != m_eLevel)
		Register_FakeShadow(_float2(2.f, 2.f));

	return S_OK;
}


HRESULT CCharacter_NPC::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}



HRESULT CCharacter_NPC::Bind_ShaderResources()
{
	auto pGameInstance = CGameInstance::Get_Instance();

	auto matView = pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
	auto matProj = pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ);

	if (nullptr != m_pDrawTransformCom)
	{
		m_pDrawTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix");
	}

	m_pShaderCom->Bind("g_ViewMatrix", matView);
	m_pShaderCom->Bind("g_ProjMatrix", matProj);

	_float3 vRight, vForward;
	XMStoreFloat3(&vRight, XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT)));
	XMStoreFloat3(&vForward, XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)));

	auto pLight = pGameInstance->Get_Light(0);

	if (nullptr != pLight)
	{
		Vector4 vLightDir = pLight->vDirection;
		vLightDir.Normalize();

		m_pShaderCom->Bind("g_vLightDirection", vLightDir);
	}

	m_pShaderCom->Bind("g_vCamPosition", pGameInstance->Get_CamPosition());
	m_pShaderCom->Bind("g_vCamDirection", pGameInstance->Get_CamForward());
	m_pShaderCom->Bind("g_fCameraFar", pGameInstance->Get_CameraFar());

	m_pShaderCom->Bind("g_vCharacterForward", vForward);
	m_pShaderCom->Bind("g_vCharacterRight", vRight);

	m_pShaderCom->Bind("g_vDiffuseColor", m_vHairColor);
	m_pShaderCom->Bind("g_fAnisoHairFresnelPow", CGlobal::g_fHairFresnelPower);
	m_pShaderCom->Bind("g_fAnisoHairFresnelIntensity", CGlobal::g_fHairFresnelIntensity);

	m_pShaderCom->Bind("g_bUseRimLight", CRender_Manager::Get_Instance()->Is_Enable_RimLight());
	m_pShaderCom->Bind("g_vRimColor", _float4(1.f, 1.f, 1.f, 0.f));
	m_pShaderCom->Bind("g_fRimPower", 8.f);

	return S_OK;
}

/*
HRESULT CCharacter_NPC::Handling_RootMotion(_float _fTimeDelta)
{
	Matrix matWorld = m_pTransformCom->Get_WorldMatrix();
	Vector4	vCurrentNPCPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	Vector4 vLookNormalized = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));

	CAnimator* pAnimator = m_pModelCom->Get_Animator();
	CAnimationState* pCurrentAnimationState = pAnimator->Get_CurAnimState();

	if (nullptr != pCurrentAnimationState && pCurrentAnimationState->Get_RootMotionEnable())
	{
		_float fXZDelta = m_pModelCom->Get_FirstRootBone_XZDelta();
		vCurrentNPCPosition += (fXZDelta * m_vModelScaleSize.x) * vLookNormalized;
	}
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCurrentNPCPosition, m_pNavigationCom);

	vCurrentNPCPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	
	_float fYDist = 0.f;
	CNavigation::INTERSECTDIRECTION eIntersectType = m_pNavigationCom->Get_YIntersectDistanceFromPoint(vCurrentNPCPosition, fYDist);
	if (CNavigation::INTERSECTDIRECTION::UP == eIntersectType)
	{
		vCurrentNPCPosition.y += fabsf(fYDist);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCurrentNPCPosition, m_pNavigationCom);
	}

	m_pDrawTransformCom->Set_WorldMatrix(m_pTransformCom->Get_WorldFloat4x4());

	return S_OK;
}

HRESULT CCharacter_NPC::_Add_Components(void* _pArg)
{
	// 참고 : NPC의 대화용 콜라이더 타입은 COLLISION_GROUP::INTERACTION으로 설정한다

	auto pGameInstance = CGameInstance::Get_Instance();
	auto pDesc = static_cast<CHARACTERDESC*>(_pArg);

	m_eLevel = (_int)CGameInstance::Get_Instance()->Get_CurLevel();

	if (FAILED(__super::Add_Component(LEVEL_STATIC, L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;
	
	if (FAILED(__super::Add_Component(m_eLevel, PC_SHADER_CHARACTER, TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;
	
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;
	else
	{
		if (nullptr != _pArg)
		{
			CHARACTERDESC* pDesc = (CHARACTERDESC*)_pArg;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vBatchPosition);

			m_pTransformCom->RotationByCurTransform(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(pDesc->vBatchRotationDegree.x));
			m_pTransformCom->RotationByCurTransform(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(pDesc->vBatchRotationDegree.y));
			m_pTransformCom->RotationByCurTransform(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(pDesc->vBatchRotationDegree.z));
		}
	}
	
	
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform_Model"), (CComponent**)&m_pDrawTransformCom)))
	{
		return E_FAIL;
	}
	
	CCollider::COLLIDERDESC tColliderDesc;
	ZeroMemory(&tColliderDesc, sizeof tColliderDesc);
	tColliderDesc.vPosition = m_vSolidBodyColliderPosition;
	tColliderDesc.vSize = m_vSolidBodyColliderSize;
	tColliderDesc.fRadius = 2.f;
	CComponent* pCollider = pGameInstance->Add_ColliderComponent(m_eLevel,
		TEXT("Prototype_Component_Collider_OBB"), this, COLLISION_GROUP::NPC_BODY, &tColliderDesc);
	Register_ColliderCom(L"Collider_Solid_Body", pCollider);

	m_pColliderSolidBodyCom = static_cast<CCollider*>(pCollider);
	m_pColliderSolidBodyCom->Set_Enable(true);
	m_pColliderSolidBodyCom->Tick(m_pTransformCom->Get_WorldMatrix());

	CPhysics::PHYSICSDESC tPhysicsDesc;
	ZeroMemory(&tPhysicsDesc, sizeof tPhysicsDesc);
	tPhysicsDesc.fMaxSpeed = +30.f;
	tPhysicsDesc.fMinSpeed = -70.f;
	tPhysicsDesc.fGravityOffset = 1.f;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, L"Prototype_Component_Physics", L"Com_Physics", (CComponent**)&m_pPhysicsCom, &tPhysicsDesc)))
		return E_FAIL;

	return S_OK;
}
*/

void CCharacter_NPC::Change_Animation_For_Dialog(const string& _szAnimName)
{
	if (nullptr != m_pModelCom)
	{
		m_pModelCom->Change_Animation(_szAnimName.c_str());
		m_bExternAnimationControl = true;
	}
}

void CCharacter_NPC::OnCollisionBegin(CCollider* _pSelf, CCollider* _pOther, _float _fTimeDelta)
{
	if (COLLISION_GROUP::INTERACTION == _pSelf->Get_CollisionGroup())
		Popup_UI();
}

void CCharacter_NPC::OnCollision(CCollider* _pSelf, CCollider* _pOther, _float _fTimeDelta)
{
}

void CCharacter_NPC::OnCollisionEnd(CCollider* _pSelf, CCollider* _pOther, _float _fTimeDelta)
{
	if (COLLISION_GROUP::INTERACTION == _pSelf->Get_CollisionGroup())
		Disable_Popup_UI();
}


void CCharacter_NPC::Interaction()
{
	if (0 == m_iDialogID)
		return;

	if (FAILED(CDialog_Manager::Get_Instance()->Begin_Dialog(m_iDialogID, this)))
		return;

	CFunc::Play_Audio_UI("UI_Click_4");

	if (nullptr != m_pColliderInteraction)
		m_pColliderInteraction->Set_Enable(false);

	auto funcOnCollider = [this]()
	{
		if (nullptr != m_pColliderInteraction)
			m_pColliderInteraction->Set_Enable(true);
	};

	CDialog_Manager::Get_Instance()->Bind_Callback_EndOfDialog(funcOnCollider);
}

void CCharacter_NPC::Take_Notify(OBSERVER_NOTIFY_TYPE _eType, any _arg)
{
}

/*
void CCharacter_NPC::Set_Turn(Vector4 _vTowardTarget)
{
	m_bTurn = true;
	m_fTurnTime = 0.4f;
	m_fTurnVelocity = 0.f;
	m_vTurnTowardTarget = _vTowardTarget;
}

void CCharacter_NPC::Handling_Turn(_float _fTimeDelta)
{
	Vector4 vCurrentLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	vCurrentLook.y = 0.f; vCurrentLook.Normalize();

	Vector4 vNewLook = SmoothDampDirectionalVector({ 0.f, 1.f, 0.f, 0.f }, vCurrentLook, m_vTurnTowardTarget, m_fTurnVelocity, m_fTurnTime, 100.f, _fTimeDelta);

	_float fAngle = Get_AngleDeg_BetweenVectors(vCurrentLook, vNewLook);

	if (2.f >= fabsf(fAngle))
	{
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, m_vTurnTowardTarget);
		m_bTurn = false;
	}
	else
	{
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, vNewLook);
	}
	
}
*/

void CCharacter_NPC::Popup_UI()
{
	if (ID_NONE == m_iID)
		return;

	auto pGameInstance = CGameInstance::Get_Instance();

	Engine::EventMsg msg;

	const auto& wstrName = CData_Manager::Get_Instance()->Get_ObjectName(m_iID);

	msg.eEventType = EVENT_TYPE::UI_INTERACTION_ENABLE;
	msg.wParam = m_eLevel;
	msg.lParam = INTER_TALK;
	msg.cParam = wstring(LAYER_UI_INTERACTIONINFO);
	msg.sParam = wstrName;

	pGameInstance->Add_EventMsg(msg);
}

void CCharacter_NPC::Disable_Popup_UI()
{
	auto pGameInstance = CGameInstance::Get_Instance();

	Engine::EventMsg msg;

	msg.eEventType = EVENT_TYPE::UI_DISABLE;
	msg.wParam = m_eLevel;
	msg.cParam = wstring(LAYER_UI_INTERACTIONINFO);

	pGameInstance->Add_EventMsg(msg);
}

void CCharacter_NPC::Tick(_float _fTimeDelta)
{
	__super::Tick(_fTimeDelta);

	if (nullptr != m_pColliderInteraction)
		m_pColliderInteraction->Tick(m_pDrawTransformCom->Get_WorldMatrix());
}

void CCharacter_NPC::Late_Tick(_float _fTimeDelta)
{
	__super::Late_Tick(_fTimeDelta);
}

HRESULT CCharacter_NPC::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(CCharacter_NPC::Bind_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCharacter_NPC::Render_Shadow(list<CLight*>& _lights)
{
	m_pDrawTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix");

	_uint iMeshNum = m_pModelCom->Get_NumMesh();
	auto& meshes = m_pModelCom->Get_vMeshes();

	for (_uint i = 0; i < iMeshNum; ++i)
	{
		m_pModelCom->Bind_ShaderBoneMatricies(m_pShaderCom, i, "g_BoneMatricies");

		for (auto& pLight : _lights)
		{
			pLight->Render_Shadow(m_pShaderCom, meshes[i], SHADER_PASS_CHARACTER_SHADOWMAP);
		}
	}

	return S_OK;
}


void CCharacter_NPC::Free()
{
	__super::Free();

	Safe_Release(m_pColliderSolidBodyCom);
	Safe_Release(m_pColliderInteraction);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pPhysicsCom);
	Safe_Release(m_pDrawTransformCom);
	Safe_Release(m_pFSMControllerCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pModelCom);

	Safe_Release(m_pNpcEffect);

	CQuest_Manager::Get_Instance()->Remove_Observer(this);
}

/*
CGameObject* CCharacter_NPC::Create_Npc_Effect(wstring _wstrEffect, const _float4x4* _pWorldMatrix, _float3 _vFixPos, _bool _bMaintainPointerPos, _bool _bMaintainPivotPointerPos, const _float4x4* _pBoneMatrix, _float4x4 _PivotMatrix, _bool* _pDead, _bool _bMaintainWorldMatrix)
{
	CE_Effect_Coll::CLONEEFFECTDESC CloneEffectDesc;

	CloneEffectDesc.pWorldMatrix = _pWorldMatrix;
	// 월드 위치로 인스턴스 위치 갱신
	CloneEffectDesc.bMaintainPointerPos = _bMaintainPointerPos;
	// 월드 위치 피벗 갱신
	CloneEffectDesc.bMaintainPivotPointerPos = _bMaintainPivotPointerPos;
	//월드와 동일
	CloneEffectDesc.bMaintainWorldMatrix = _bMaintainWorldMatrix;

	// 보정
	CloneEffectDesc.vPointerFixPos = _vFixPos;
	CloneEffectDesc.pBoneMatrix = _pBoneMatrix;
	CloneEffectDesc.PivotMatrix = _PivotMatrix;
	// 생성 위치
	_float4x4 vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
	_float3 vRight;
	memcpy_s(&vRight, sizeof _float3, &vWorldMatrix.m[0], sizeof _float3);
	_float3 vUp;
	memcpy_s(&vUp, sizeof _float3, &vWorldMatrix.m[1], sizeof _float3);
	_float3 vLook;
	memcpy_s(&vLook, sizeof _float3, &vWorldMatrix.m[2], sizeof _float3);
	_float4 vPos;
	memcpy_s(&vPos, sizeof _float4, &vWorldMatrix.m[3], sizeof _float4);
	XMStoreFloat4(&CloneEffectDesc.vPos, XMLoadFloat4(&vPos)
		+ XMVector3Normalize(XMLoadFloat3(&vRight)) * _vFixPos.x
		+ XMVector3Normalize(XMLoadFloat3(&vUp)) * _vFixPos.y
		+ XMVector3Normalize(XMLoadFloat3(&vLook)) * _vFixPos.z
	);
	//cout << vLook.x << "\t"<< vLook.y << "\t" << vLook.z << "\t" << endl;

	// 생성 룩
	CloneEffectDesc.vLook = vLook;
	CloneEffectDesc.vRight = vRight;
	CloneEffectDesc.pDead = _pDead;

	CGameObject* pEffect = CGameInstance::Get_Instance()->Get_FromObjectPool(_wstrEffect, &CloneEffectDesc);
	if (nullptr == pEffect)
		MSG_BOX("Failed to Create FiledItem Effect");

	return pEffect;
}
*/