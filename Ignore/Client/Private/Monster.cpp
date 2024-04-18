#include "Client_pch.h"
#include "Client_Defines.h"
#include "Monster.h"

// Singleton
#include "GameInstance.h"
#include "Render_Manager.h"
#include "Quest_Manager.h"

// Component
#include "Mesh.h"

#include "Light.h"

#include "SkillObject.h"

#include "Client_Math.h"
#include "Client_Functions.h"
#include "Engine_Functions.h"

#include "BossInfo.h"
#include "MonsterInfo.h"
#include "DpInfo.h"

#include "InteractionObject_Battle_Reward.h"

#include "Camera_TPS.h"
#include "Layer.h"

CMonster::CMonster(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CCharacter(_pDevice, _pContext)
{
}

CMonster::CMonster(const CMonster& _rhs)
	: CCharacter(_rhs)
{
}

HRESULT CMonster::Initialize(void* _pArg)
{
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	m_funcDropItem = &CMonster::Drop_Item;
	CQuest_Manager::Get_Instance()->Register_Observer(this);

	return S_OK;
}

HRESULT CMonster::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

/*
_bool CMonster::Trigger_Floating()
{
	return false;
}

_bool CMonster::Trigger_KnockOver()
{
	return false;
}

_bool CMonster::Trigger_Falling()
{
	return false;
}

void CMonster::Set_WorldMatrix(Matrix _matWorld, _bool _bApplyNavigation)
{
	if (_bApplyNavigation)
	{
		if (nullptr != m_pTransformCom)
		{
			Vector3 vTmpPosition = _matWorld.Translation();
			Vector4 vNewPosition = { vTmpPosition.x, vTmpPosition.y, vTmpPosition.z, 1.f };

			m_pTransformCom->Set_WorldMatrix(_matWorld);
			_int iNewCell = m_pNavigationCom->Get_Index(vNewPosition);
			m_pNavigationCom->Set_CurrentIndex(iNewCell);
		}
	}
	else
	{
		if (nullptr != m_pTransformCom)
		{
			m_pTransformCom->Set_WorldMatrix(_matWorld);		
		}
	}
}

void CMonster::Set_Position(Vector4 _vNewPosition, _bool _bApplyNavigation)
{

	if (_bApplyNavigation)
	{
		if (nullptr != m_pTransformCom)
		{
			_int iNewCell = m_pNavigationCom->Get_Index(_vNewPosition);
			m_pNavigationCom->Set_CurrentIndex(iNewCell);
			m_pTransformCom->Set_Position_No_Sliding(_vNewPosition, m_pNavigationCom);
		}
	}
	else
	{
		if (nullptr != m_pTransformCom)
		{
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vNewPosition);
		}
	}

}

void CMonster::Create_Reward_Box()
{
	CInteractionObject_Battle_Reward::BATTLEREWARDDESC tBattleRewardDesc;

	if (nullptr != m_pTransformCom)
	{
		Vector4 vBatchPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		tBattleRewardDesc.vBatchPosition = vBatchPosition;
		
		if (nullptr != m_pTransformCom)
		{
			Vector4 vZBasicLook = { 0.f, 0.f, 1.f, 0.f };

			Vector4 vCurrentLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			vCurrentLook.y = 0.f; vCurrentLook.Normalize();
			
			_float fAngle = Get_AngleDeg_BetweenVectors(vZBasicLook, vCurrentLook);
			tBattleRewardDesc.vBatchRotationDegree = { 0.f, fAngle + 45.f, 0.f, 0.f };
		}


		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		pGameInstance->Add_GameObject((_uint)m_eLevel, L"Layer_Interaction_Object", L"Prototype_GameObject_InteractionObject_BattleReward", &tBattleRewardDesc);
	}
}
*/

AttackDamageReation_ CMonster::Take_Damage(const DAMAGEDESC& _tDamageDesc)
{
	return AttackDamageReaction_None;
}

/*
void CMonster::Pause_Physics()
{
	if (nullptr != m_pPhysics)
	{
		m_pPhysics->Set_Enable_Gravity(false);
		m_pPhysics->Set_Enable_Velocity(false);
	}
}
*/

void CMonster::Take_Notify(OBSERVER_NOTIFY_TYPE _eType, any _arg)
{
	if (OBSERVER_NOTIFY_TYPE::QUEST_BEGIN == _eType)
	{
		auto iBeginQuestID = any_cast<_uint>(_arg);

		const auto& quest = CQuest_Manager::Get_Instance()->Get_QuestDesc(iBeginQuestID);

		if (0 == quest.iQuestType)
		{
			if (m_iID == quest.iTargetID)
				CFunc::Add_QuestDisplay(this, 3.f);
		}
	}

	else if (OBSERVER_NOTIFY_TYPE::QUEST_ACHIEVEMENT == _eType)
	{
		auto iAchievementQuestID = any_cast<_uint>(_arg);

		const auto& quest = CQuest_Manager::Get_Instance()->Get_QuestDesc(iAchievementQuestID);

		if (0 == quest.iQuestType)
		{
			if (m_iID == quest.iTargetID)
				CFunc::Remove_QuestDisplay(this);
		}
	}
}

/*
void CMonster::Resume_Physics()
{
	if (nullptr != m_pPhysics)
	{
		m_pPhysics->Set_Enable_Gravity(true);
		m_pPhysics->Set_Enable_Velocity(true);
	}
}

void CMonster::OnCollisionBegin(CCollider* _pSelf, CCollider* _pOther, _float _fTimeDelta)
{	
	if (COLLISION_GROUP::MONSTER_HIT_BODY == _pSelf->Get_CollisionGroup() &&
		COLLISION_GROUP::PLAYER_SKILL_AREA == _pOther->Get_CollisionGroup())
	{

		if (SKILLTYPE::TIME_ILLUSION == static_cast<CSkillObject*>(_pOther->Get_Owner())->Get_Skill_Type())
		{
			m_bApplyTimeSlowForTimeillusion = true;
		}
	}
}

void CMonster::OnCollision(CCollider* _pSelf, CCollider* _pOther, _float _fTimeDelta)
{
	if (COLLISION_GROUP::MONSTER_HIT_BODY == _pSelf->Get_CollisionGroup() &&
		COLLISION_GROUP::PLAYER_SKILL_AREA == _pOther->Get_CollisionGroup())
	{

		if (SKILLTYPE::TIME_ILLUSION == static_cast<CSkillObject*>(_pOther->Get_Owner())->Get_Skill_Type())
		{
			m_bApplyTimeSlowForTimeillusion = true;
		}
	}
}

void CMonster::OnCollisionEnd(CCollider* _pSelf, CCollider* _pOther, _float _fTimeDelta)
{
	if (COLLISION_GROUP::MONSTER_HIT_BODY == _pSelf->Get_CollisionGroup() &&
		COLLISION_GROUP::PLAYER_SKILL_AREA == _pOther->Get_CollisionGroup())
	{

		if (SKILLTYPE::TIME_ILLUSION == static_cast<CSkillObject*>(_pOther->Get_Owner())->Get_Skill_Type())
		{
			m_bApplyTimeSlowForTimeillusion = false;
		}
	}
}
*/

void CMonster::Enable_Hit_RimLight()
{
	m_bOnHitRimLight = true;

	m_vHitRimLightColor = _float4(0.8f, 0.8f, 0.8f, 0.8f);
}

void CMonster::Calculate_Hit_RimLight(_float _fTimeDelta)
{
	if (!m_bOnHitRimLight)
		return;

	m_vHitRimLightColor.x -= _fTimeDelta * 3.f;
	m_vHitRimLightColor.y -= _fTimeDelta * 3.f;
	m_vHitRimLightColor.z -= _fTimeDelta * 3.f;
	m_vHitRimLightColor.w -= _fTimeDelta * 3.f;

	if (0.f >= m_vHitRimLightColor.w)
	{
		m_bOnHitRimLight = false;

		m_vHitRimLightColor = Vector4(0.f, 0.f, 0.f, 0.f);
	}
}

void CMonster::Enable_Rage_RimLight(_float _fTime)
{
	m_bOnRageRimLight = true;
	m_fRageRimLightProgressTime = _fTime;

	m_vRageRimLightColor = Vector4(0.8f, 0.f, 0.f, 0.9f);
}

void CMonster::Calculate_Rage_RimLight(_float _fTimeDelta)
{
	if (!m_bOnRageRimLight)
		return;

	m_vRageRimLightColor.x = clamp(m_fRageRimLightProgressTime, 0.f, 0.5f);
	m_vRageRimLightColor.w = clamp(m_fRageRimLightProgressTime, 0.f, 0.9f);

	m_fRageRimLightProgressTime -= _fTimeDelta;

	if (0.f >= m_fRageRimLightProgressTime)
	{
		m_bOnRageRimLight = false;
		m_fRageRimLightProgressTime = 0.f;

		m_vRageRimLightColor = Vector4(0.f, 0.f, 0.f, 0.f);
	}
}

HRESULT CMonster::Bind_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	_float4x4 matView = pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
	_float4x4 matProj = pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ);
	
	m_pShaderCom->Bind_Matrix("g_ViewMatrix", &matView);
	m_pShaderCom->Bind_Matrix("g_ProjMatrix", &matProj);

	auto pLight = pGameInstance->Get_Light(0);

	if (nullptr != pLight)
	{
		Vector4 vLightDir = pLight->vDirection;
		vLightDir.Normalize();

		m_pShaderCom->Bind("g_vLightDirection", vLightDir);
	}

	m_pShaderCom->Bind("g_vCamPosition", pGameInstance->Get_CamPosition());

	auto vCamForward = pGameInstance->Get_CamForward();

	m_pShaderCom->Bind("g_vCamDirection", vCamForward);
	m_pShaderCom->Bind("g_bUseRimLight", m_bOnHitRimLight || m_bOnRageRimLight);

	auto vRimColor = Vector4::Max(m_vHitRimLightColor, m_vRageRimLightColor);

	m_pShaderCom->Bind("g_vRimColor", vRimColor);
	m_pShaderCom->Bind("g_fRimPower", m_fRimPower);
	m_pShaderCom->Bind("g_fCameraFar", pGameInstance->Get_CameraFar());

	m_pDrawTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix");

	return S_OK;
}

_bool CMonster::IsIn_Distance_To_Camera()
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

/*
void CMonster::Apply_Damage(_float _fDamage)
{
	// 디펜스 게이지가 있을 때는 들어온 데미지의 50퍼센트를 디펜스 게이지가 받고,
	// 들어온 데미지의 50퍼센트를 체력이 받는다.
	if (m_tMonsterStateDesc.fCurrentDefenseGauge > 0.f)
	{
		m_tMonsterStateDesc.fCurrentDefenseGauge -= _fDamage * 0.5f;

		// 디펜스 게이지가 0 밑으로 내려가면
		if (m_tMonsterStateDesc.fCurrentDefenseGauge <= 0.f)
		{
			m_tMonsterStateDesc.fCurrentDefenseGauge = 0.f;
			m_bDefenseGaugeOn = false;
		}

		m_tStateInfo.fCurrentHP -= _fDamage * 0.5f;

		if (m_tStateInfo.fCurrentHP < 0.f)
		{
			m_tStateInfo.fCurrentHP = 0.f;
		}
	}
	// 디펜스 게이지가 없을 때는 체력이 달음
	else
	{
		m_tStateInfo.fCurrentHP -= _fDamage;

		if (m_tStateInfo.fCurrentHP < 0.f)
		{
			m_tStateInfo.fCurrentHP = 0.f;
		}
	}
}

void CMonster::Destroy_Skill_Object_Shield()
{
	if (nullptr != m_pSkillObject_Shield)
	{
		// Shield Destroy effect add here
		m_pSkillObject_Shield->Set_Dead(true);
		m_pSkillObject_Shield = nullptr;

		auto pGameInstance = CGameInstance::Get_Instance();
		pGameInstance->Play_Audio("Shield_Broken");

		auto func1 = [](_float)
		{
			CGameInstance::Get_Instance()->Set_TimeScale(TEXT("Timer_60"), 0.2f);
		};

		auto func2 = [](_float)
		{
			CGameInstance::Get_Instance()->Set_TimeScale(TEXT("Timer_60"), 1.f);
		};

		CFunc::Add_Event_Inteval_Function(0.15f, 0.f, 1, func1, this, false);
		CFunc::Add_Event_Inteval_Function(0.47f, 0.f, 1, func2, this, false);
	}
}
HRESULT CMonster::Create_Skill_Object_Shield(CSkillObject_Monster_Shield::MONSTERSHIELDDESC _tShieldDesc)
{
	if (nullptr != m_pSkillObject_Shield)
	{
		return S_OK;
		//m_pSkillObject_Shield->Set_Dead(true);
		//m_pSkillObject_Shield = nullptr;
	}

	m_tMonsterStateDesc.fCurrentDefenseGauge = m_tMonsterStateDesc.fMaxDefenseGauge;

	// 디펜스 게이지 활성화 코드
	Set_Monster_State_Current_Defense_Gauge(m_tMonsterStateDesc.fMaxDefenseGauge);
	Set_DefenseGaugeOn(true);
	CGameInstance::Get_Instance()->Enable_UI(CFunc::Get_CurLevel(), TEXT("Layer_UI_DpInfo"), 0);

	// 참고 : 디펜스 게이지 비활성화 코드 -> 많이 맞아서 디펜스 게이지 0되면 알아서 Disable됨, 디펜스 게이지 차있을 때 Disable 시키고 싶으면 쓰면 될듯
	//Set_DefenseGaugeOn(false);
	//CGameInstance::Get_Instance()->Disable_UI(CFunc::Get_CurLevel(), TEXT("Layer_UI_DpInfo"), 0);

	//Prototype_GameObject_SkillObject_MonsterShield
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	pGameInstance->Create_GameObject(L"Prototype_GameObject_SkillObject_MonsterShield", (CGameObject**)&m_pSkillObject_Shield, &_tShieldDesc);
	pGameInstance->Add_GameObject(m_eLevel, L"Layer_Skill_Object", m_pSkillObject_Shield);

	return S_OK;
}

HRESULT CMonster::Camera_Shaking(CAMERA_SHAKING_TYPE eShakingType, _float _fShakingStrengthen, _float fShakingTime, _float _fPlayerLength)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CTransform* pPlayerTransform = static_cast<CTransform*>(pGameInstance->Get_Component(m_eLevel,
		L"Layer_Player", L"Com_Transform"));
	if (nullptr == pPlayerTransform)
		return E_FAIL;

	Vector4 vPlayerPosition = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	Vector4 vLightningPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	Vector4 vDirection = vPlayerPosition - vLightningPosition;

	if (_fPlayerLength < vDirection.Length())
		return S_OK;

	CCamera_TPS* pCamTPS = nullptr;

	auto pMap = CGameInstance::Get_Instance()->Get_SpecificLayer(CFunc::Get_CurLevel());
	if (nullptr == pMap || pMap->end() == pMap->find(L"Layer_Camera"))
	{
		return E_FAIL;
	}

	auto pLayer = (pMap->find(L"Layer_Camera"))->second;
	auto& pCameraObjectList = pLayer->GetObjectList();

	for (auto& pCamObj : pCameraObjectList)
	{
		if ("Camera_TPS" == pCamObj->Get_ClassName())
		{
			pCamTPS = static_cast<CCamera_TPS*>(pCamObj);
			break;
		}
	}
	CAMERAEVENTMESSAGE tCamEventMsg;
	CAMERAEVENTSHAKINGDESC tShakingDesc;
	tCamEventMsg.eCameraEventType = (_int)CAMERA_EVENT_TYPE::SHAKING;
	tShakingDesc.eCameraShakingType = (_int)eShakingType;
	tShakingDesc.fShakingStrengthen = _fShakingStrengthen;
	tShakingDesc.fShakingTime = fShakingTime;
	tCamEventMsg.tCameraShakingDesc = tShakingDesc;
	pCamTPS->Add_Event_Message(&tCamEventMsg);

	return S_OK;
}
*/

void CMonster::Drop_Item()
{
	CFunc::Add_Item(100001, 1); // 토큰 1개
	CFunc::Add_Item(CFunc::Generate_Random_Int(300001, 300031), 1); // 요리재료 랜덤 1개
}


void CMonster::Tick(_float _fTimeDelta)
{
	__super::Tick(_fTimeDelta);

	Calculate_Hit_RimLight(_fTimeDelta);
	Calculate_Rage_RimLight(_fTimeDelta);
}

void CMonster::Late_Tick(_float _fTimeDelta)
{
	__super::Late_Tick(_fTimeDelta);

	if (m_bMonsterDead)
	{
		m_fAfterDeadTime += _fTimeDelta;
		
		if (nullptr != m_funcDropItem)
			(this->*m_funcDropItem)();

		m_funcDropItem = nullptr;
	}

	if (!IsIn_Distance_To_Camera())
		return;

	if (IsIn_Frustum_InWorld(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_CHARACTER, this);
}

HRESULT CMonster::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (m_bMonsterDead)
	{
		_float fAlpha = clamp(m_fAfterDeadTime - 1.f, 0.f, 1.f);
		auto pDissolveTexture = CGameInstance::Get_Instance()->Get_Texture_SRV(LEVEL_STATIC, PC_TEXTURE_NOISE, 0);

		m_pShaderCom->Bind("g_fDissolveAlpha", fAlpha);
		m_pShaderCom->Bind("g_DissolveTexture", pDissolveTexture);
	}

	m_pShaderCom->Bind("g_bDead", m_bMonsterDead);

	return S_OK;
}

HRESULT CMonster::Render_Shadow(list<CLight*>& _lights)
{
	m_pDrawTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix");

	_uint iMeshNum = m_pModelCom->Get_NumMesh();
	auto& meshes = m_pModelCom->Get_vMeshes();

	for (_uint i = 0; i < iMeshNum; ++i)
	{
		m_pModelCom->Bind_ShaderBoneMatricies(m_pShaderCom, i, "g_BoneMatricies");

		for (auto& pLight : _lights)
		{
			pLight->Render_Shadow(m_pShaderCom, meshes[i], SHADER_PASS_MONSTER_SHADOWMAP);
		}
	}

	return S_OK;
}

/*
void CMonster::_Create_Hit_WeapoenType(const DAMAGEDESC& _tDamageDesc)
{
	_float3 vRandEffectFixPos = { 0.f, 0.f, 0.f };
	//-0.5f ~ 0.5f
	vRandEffectFixPos = { (-50 + rand() % 100) * 0.01f , (-50 + rand() % 100) * 0.01f , (-50 + rand() % 100) * 0.01f };
	//Right 30.f -30.f Up 30.f -30.f
	_float fRandRightAngle = -30.f + _float(rand() % 61);
	_float fRandUpAngle = -30.f + _float(rand() % 61);
	_matrix		RotationMatrix = XMMatrixRotationAxis(XMLoadFloat3(&_tDamageDesc.vWeaponRight), XMConvertToRadians(fRandRightAngle))
		* XMMatrixRotationAxis(XMLoadFloat3(&_tDamageDesc.vWeaponUp), XMConvertToRadians(fRandUpAngle));
	
	Vector3 vWeaponRight = _tDamageDesc.vWeaponRight;
	Vector3 vWeaponUp = _tDamageDesc.vWeaponUp;

	vWeaponRight = XMVector3TransformNormal(vWeaponRight, RotationMatrix);
	vWeaponUp = XMVector3TransformNormal(vWeaponUp, RotationMatrix);

	switch ((PLAYER_WEAPON_MODE)_tDamageDesc.uWeaponType)
	{
	case Client::PLAYER_WEAPON_MODE::GUN:
		if(FAILED(Create_Hit_Effect(TEXT("Gun_Hit_Scale105_Client.Effect"), 0.5f, _tDamageDesc.vEffectPos, vRandEffectFixPos)))
			MSG_BOX("Fail to Create Gun Hit Effect");
		break;
	case Client::PLAYER_WEAPON_MODE::DUALGUN:
		if (FAILED(Create_Hit_Effect(TEXT("2Gun_Hit_Client.Effect"), 0.5f, _tDamageDesc.vEffectPos, vRandEffectFixPos)))
			MSG_BOX("Fail to Create DualGun Hit Effect");
		break;
	case Client::PLAYER_WEAPON_MODE::LINGHAN:
		if (FAILED(Create_Hit_Effect(TEXT("Linghan_Hit_Client.Effect"), 0.f, _tDamageDesc.vEffectPos, vRandEffectFixPos, vWeaponUp, vWeaponRight)))
			MSG_BOX("Fail to Create Linghan Hit Effect");
		break;
	case Client::PLAYER_WEAPON_MODE::SICKLE:
		if (FAILED(Create_Hit_Effect(TEXT("Sickle_Hit_Client.Effect"), 0.f, _tDamageDesc.vEffectPos, vRandEffectFixPos, vWeaponUp, vWeaponRight)))
			MSG_BOX("Fail to Create Sickle Hit Effect");
		break;
	case Client::PLAYER_WEAPON_MODE::HAMMER:
		break;
	case Client::PLAYER_WEAPON_MODE::WEAPON_END:
		break;
	default:
		break;
	}
}
*/

void CMonster::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pPhysics);
	Safe_Release(m_pDrawTransformCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pFSMControllerCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pMonsterHitBodyCollider);
	Safe_Release(m_pPrepareAttackColliderSphere);
	Safe_Release(m_pPrepareAttackColliderOBB);
	Safe_Release(m_pMonsterTimeillusionCheckCollider);

	CQuest_Manager::Get_Instance()->Remove_Observer(this);
}