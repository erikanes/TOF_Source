#include "Client_pch.h"
#include "Player.h"

// Singleton
#include "GameInstance.h"
#include "PipeLine.h"
#include "Render_Manager.h"
#include "Quest_Manager.h"
#include "Inventory.h"
#include "Item_Manager.h"
#include "Data_Manager.h"

#include "Layer.h"
#include "Camera_TPS.h"

#include "Animator.h"
#include "Level_Loading.h"

#include "Client_Math.h"
#include "Player_Setting_Module.h"
#include "Player_Control_Module.h"
#include "Player_Combat_Module.h"

#include "AnimationState.h"

#include "Player_Acc_ShortHair.h"
#include "Player_Acc_LongHair.h"
#include "Player_Weapon_Ice_Hammer.h"
#include "Player_Weapon_CobaltB_Gun.h"
#include "Player_Weapon_Sickle.h"
#include "Player_Weapon_Linghan.h"
#include "Player_Weapon_DualGun.h"

#include "Player_Acc_Hair.h"
#include "Player_Mount.h"
#include "Player_Glider.h"

#include "Mesh.h"

#include "Projectile.h"

#include "Light.h"

// GameObject
#include "Character_NPC.h"

#include "Client_Functions.h"
#include "Engine_Functions.h"
#include "Client_Anim_Enum.h"


#include "GameModeImpl_MiniGame_ElementalCube.h"
#include "GameModeImpl_MiniGame_BasketBall.h"
#include "GameModeImpl_MiniGame_BrainQuiz.h"
#include "MiniGame_Elemental_Cube.h"

// Test
#include "GameModeImpl_ApophisCutScene.h"


//Test, Must be manipulate by kjg
#include "MonsterNormal.h"
#include "Timeillusion.h"

#include "Effect_Coll.h"

#pragma region UI
#include "FoodList.h"
#pragma endregion

#pragma region Interaction
#include "FieldItem.h"
#pragma endregion

/*
CPlayer::CPlayer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CCharacter(_pDevice, _pContext)
{
}

CPlayer::CPlayer(const CPlayer& _rhs)
	: CCharacter(_rhs)
{
	m_bCloned = true;
}

HRESULT CPlayer::Initialize(void* _pArg)
{
	m_szClassName = "CPlayer";

	INITDATA* pInit = (INITDATA*)_pArg;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	m_eLevel = (_int)pGameInstance->Get_CurLevel();


	const CData_Manager::DB_PLAYER& tPlayerDB = CData_Manager::Get_Instance()->Get_Player_DB();
	m_eWeaponMode = tPlayerDB.eCenterWeapon;
	m_eRightWeaponMode = tPlayerDB.eRightWeapon;
	m_eLeftWeaponMode = tPlayerDB.eLeftWeapon;
	m_eExWeaponMode = tPlayerDB.eExWeapon;


	m_tStateInfo.wszName = L"건전한 닉네임";
	m_tStateInfo.uLevel = 0;
	m_tStateInfo.fMaxHP = 60000.f;
	//m_tStateInfo.fCurrentHP = 0.f;

	m_tPlayerStateDesc.fMaxSP = 100.f;		// SP : Stamina Point
	m_tPlayerStateDesc.fCurrentSP = 100.f;
	m_tPlayerStateDesc.fMaxEP = 100.f;		// EP : Energy Point
	m_tPlayerStateDesc.fCurrentEP = 100.f;
	m_tPlayerStateDesc.fEPRegenTime = 0.3f;	// EP 회복 쿨타임
	m_tPlayerStateDesc.fCritical = 50.f;
	m_tPlayerStateDesc.iCurrentSatiety = 10;
	m_tPlayerStateDesc.iMaxSatiety = 100;
	
	m_bSwapSkillOn = false;

	m_fCurrentYJumpSpeed = m_fYJumpSpeed;
	m_fCurrentYAirJumpSpeed = m_fYAirJumpSeed;
	m_fCurrentTurnLimitTime = m_fInitialTurnLimitTime;

	ZeroMemory(m_fWeaponSkillCurrentCoolTime, sizeof(_float) * (_int)(PLAYER_WEAPON_MODE::WEAPON_END));
	

	if (FAILED(__super::Initialize(_pArg)))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Components(_pArg)))
	{
		return E_FAIL;
	}

	if (FAILED(Add_PlayerEquipments()))
	{
		return E_FAIL;
	}


	m_tStateInfo.fCurrentHP = m_tStateInfo.fMaxHP * tPlayerDB.fCurrentPlayerHP;


	Initialize_Event_Trigger_Function();

	// 조명 테스트 코드. 추후 삭제 예정
	// CGameInstance::Get_Instance()->Bind_GameObject_To_Light(this, 0);
	*/
	// Fake Shadow
	auto funcCondition = [this]()
	{
		_bool bState = false;

		bState =
			m_eCurState == PLAYER_STATE::Dash_Front_Air ||
			m_eCurState == PLAYER_STATE::Dash_Back_Air ||
			m_eCurState == PLAYER_STATE::Jump ||
			m_eCurState == PLAYER_STATE::Jump_Air ||
			m_eCurState == PLAYER_STATE::Falling_Attack ||
			m_eCurState == PLAYER_STATE::Air_Attack ||
			m_eCurState == PLAYER_STATE::Gliding ||
			m_eCurState == PLAYER_STATE::Mounting_Up ||
			m_eCurState == PLAYER_STATE::Mounting_Idle ||
			m_eCurState == PLAYER_STATE::Mounting_Move ||
			m_eCurState == PLAYER_STATE::Mounting_Jump;

		return !bState;
	};

	if (LEVEL_DESERT != m_eLevel)
		Register_FakeShadow(_float2(2.f, 2.f), funcCondition);

	return S_OK;
}

/*
HRESULT CPlayer::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CPlayer::Initialize_Event_Trigger_Function()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	auto pAnimator = m_pModelBodyCom->Get_Animator();

	auto funcWeaponIn = [this]() { m_pCurWeapon->In_Weapon(); };

	pAnimator->Add_Notify("char_2gun_receive_show", 63, 0, funcWeaponIn); // 듀얼건 수납
	pAnimator->Add_Notify("char_gun_receive_show", 120, 0, funcWeaponIn); // 낫 수납
	pAnimator->Add_Notify("char_sickle_receive_show", 100, 0, funcWeaponIn); // 샷건 수납
	pAnimator->Add_Notify("char_linghan_receive_show", 242, 0, funcWeaponIn); // 칼 수납

}


void CPlayer::Create_WeaponEffect_RightHand(wstring _wstrWeaponEffect, _float3 _vFixRUL, _bool bMaintainPointerPos, _float3 _vFixPlayerRUL, _bool _bMaintainWorldMatrix, _bool _bMaintainPivotPointerPos)
{
	if (nullptr != m_pCurWeapon)
		m_pCurWeapon->Create_WeaponEffect_RightHand(_wstrWeaponEffect, _vFixRUL, bMaintainPointerPos, _vFixPlayerRUL, _bMaintainWorldMatrix, _bMaintainPivotPointerPos);
}

void CPlayer::Create_WeaponEffect_LeftHand(wstring _wstrWeaponEffect, _float3 _vFixRUL, _bool bMaintainPointerPos, _float3 _vFixPlayerRUL, _bool _bMaintainWorldMatrix )
{
	if (nullptr != m_pCurWeapon)
		m_pCurWeapon->Create_WeaponEffect_LeftHand(_wstrWeaponEffect, _vFixRUL, bMaintainPointerPos, _vFixPlayerRUL, _bMaintainWorldMatrix);

}

void CPlayer::Set_WeaponEx(class CPlayer_Weapon* pWeapon) 
{ 
	m_pWeaponEx = pWeapon; 
	m_eExWeaponMode = m_pWeaponEx->Get_Weapon_Mode();
}

void CPlayer::Set_Player_State()
{
	_float fCurrentHPRatio = m_tStateInfo.fCurrentHP / m_tStateInfo.fMaxHP;


	const auto& tEquipmentDesc = m_pCurWeapon->Get_Weapon_Desc();

	//m_tStateInfo.fMaxHP = tEquipmentDesc.fMaxHP;
	//m_tStateInfo.fCurrentHP = m_tStateInfo.fMaxHP * fCurrentHPRatio;
}

void CPlayer::Set_WeaponHold(class CPlayer_Weapon* pWeapon) 
{ 	
	m_pCurWeapon = pWeapon; 
	m_eWeaponMode = pWeapon->Get_Weapon_Mode();
	Set_Player_State();
}

void CPlayer::Set_Weapon(WEAPONSWAP eWeaponSwap, class CPlayer_Weapon* pWeapon) 
{ 
	m_pWeaponSwap[eWeaponSwap] = pWeapon; 

	if (WEAPONSWAP::WEAPONSWAP_LEFT == eWeaponSwap)
	{
		m_eLeftWeaponMode = pWeapon->Get_Weapon_Mode();
	}
	else if (WEAPONSWAP::WEAPONSWAP_RIGHT == eWeaponSwap)
	{
		m_eRightWeaponMode = pWeapon->Get_Weapon_Mode();
	}
}

HRESULT CPlayer::Add_PlayerEquipments()
{
	m_pSettingModule->Initialize_Player_Weapon(m_mapPlayerWeapons, this, m_pModelBodyCom, m_pTransformModelDrawCom);
	m_pSettingModule->Initialize_Player_Parts(m_mapPlayerParts, this, m_pModelBodyCom, m_pTransformModelDrawCom);
	m_pSettingModule->Initialize_Player_Mounts(m_mapPlayerParts, this, m_pModelBodyCom, m_pTransformModelDrawCom);
	m_pSettingModule->Initialize_Player_Custom_Notifications(this, m_pModelBodyCom);
	m_pSettingModule->Initialize_Player_Glid_Animation_State(m_pModelBodyCom);
	m_pSettingModule->Initialize_Player_Mount_AnimationState(m_pModelBodyCom);

	m_pGlider = m_mapPlayerParts.find(L"Equip_Glider")->second;
	m_pMount = m_mapPlayerParts.find(L"Equip_Mount")->second;

	// current weapon
	//m_eWeaponMode = PLAYER_WEAPON_MODE::SICKLE;
	//m_pCurWeapon = m_mapPlayerWeapons.find(L"Equip_Sickle")->second;
	if (PLAYER_WEAPON_MODE::SICKLE == m_eWeaponMode)
	{
		m_pCurWeapon = m_mapPlayerWeapons.find(L"Equip_Sickle")->second;
	}
	else if (PLAYER_WEAPON_MODE::GUN == m_eWeaponMode)
	{
		m_pCurWeapon = m_mapPlayerWeapons.find(L"Equip_CobaltBGun")->second;
	}
	else if (PLAYER_WEAPON_MODE::DUALGUN == m_eWeaponMode)
	{
		m_pCurWeapon = m_mapPlayerWeapons.find(L"Equip_DualGun")->second;
	}
	else if (PLAYER_WEAPON_MODE::LINGHAN == m_eWeaponMode)
	{
		m_pCurWeapon = m_mapPlayerWeapons.find(L"Equip_Linghan")->second;
	}

	/////////////////////////////// INHO ///////////////////////////////////////////////
	// swapable weapon
	//m_pWeaponSwap[WEAPONSWAP_LEFT] = m_mapPlayerWeapons.find(L"Equip_DualGun")->second;
	//m_pWeaponSwap[WEAPONSWAP_RIGHT] = m_mapPlayerWeapons.find(L"Equip_CobaltBGun")->second;

	if (PLAYER_WEAPON_MODE::SICKLE == m_eLeftWeaponMode)
	{
		m_pWeaponSwap[WEAPONSWAP_LEFT] = m_mapPlayerWeapons.find(L"Equip_Sickle")->second;
	}
	else if (PLAYER_WEAPON_MODE::GUN == m_eLeftWeaponMode)
	{
		m_pWeaponSwap[WEAPONSWAP_LEFT] = m_mapPlayerWeapons.find(L"Equip_CobaltBGun")->second;
	}
	else if (PLAYER_WEAPON_MODE::DUALGUN == m_eLeftWeaponMode)
	{
		m_pWeaponSwap[WEAPONSWAP_LEFT] = m_mapPlayerWeapons.find(L"Equip_DualGun")->second;
	}
	else if (PLAYER_WEAPON_MODE::LINGHAN == m_eLeftWeaponMode)
	{
		m_pWeaponSwap[WEAPONSWAP_LEFT] = m_mapPlayerWeapons.find(L"Equip_Linghan")->second;
	}

	if (PLAYER_WEAPON_MODE::SICKLE == m_eRightWeaponMode)
	{
		m_pWeaponSwap[WEAPONSWAP_RIGHT] = m_mapPlayerWeapons.find(L"Equip_Sickle")->second;
	}
	else if (PLAYER_WEAPON_MODE::GUN == m_eRightWeaponMode)
	{
		m_pWeaponSwap[WEAPONSWAP_RIGHT] = m_mapPlayerWeapons.find(L"Equip_CobaltBGun")->second;
	}
	else if (PLAYER_WEAPON_MODE::DUALGUN == m_eRightWeaponMode)
	{
		m_pWeaponSwap[WEAPONSWAP_RIGHT] = m_mapPlayerWeapons.find(L"Equip_DualGun")->second;
	}
	else if (PLAYER_WEAPON_MODE::LINGHAN == m_eRightWeaponMode)
	{
		m_pWeaponSwap[WEAPONSWAP_RIGHT] = m_mapPlayerWeapons.find(L"Equip_Linghan")->second;
	}

	// inventory weapon
	if (PLAYER_WEAPON_MODE::SICKLE == m_eExWeaponMode)
	{
		m_pWeaponEx = m_mapPlayerWeapons.find(L"Equip_Sickle")->second;
	}
	else if (PLAYER_WEAPON_MODE::GUN == m_eExWeaponMode)
	{
		m_pWeaponEx = m_mapPlayerWeapons.find(L"Equip_CobaltBGun")->second;
	}
	else if (PLAYER_WEAPON_MODE::DUALGUN == m_eExWeaponMode)
	{
		m_pWeaponEx = m_mapPlayerWeapons.find(L"Equip_DualGun")->second;
	}
	else if (PLAYER_WEAPON_MODE::LINGHAN == m_eExWeaponMode)
	{
		m_pWeaponEx = m_mapPlayerWeapons.find(L"Equip_Linghan")->second;
	}
	
	m_pCurWeapon->Disable_Weapon();
	Set_Player_State();

	return S_OK;
}

_bool CPlayer::Execute_Event_Function(_int _iIndex)
{
	if (_iIndex < 0 || m_vecEventFunctions.size() <= _iIndex)
	{
		return false;
	}
	else
	{
		m_vecEventFunctions[_iIndex]();
		return true;
	}
}



void CPlayer::Tick(_float _fTimeDelta)
{
	m_vBeforePosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (!m_bEnablePlayer)
	{
		return;
	}

	__super::Tick(_fTimeDelta);


	for (_int i = 0; i < (_int)PLAYER_WEAPON_MODE::WEAPON_END; ++i)
	{
		if (0.f < m_fWeaponSkillCurrentCoolTime[i])
		{
			m_fWeaponSkillCurrentCoolTime[i] -= _fTimeDelta;
		}
	}

	Apply_Debuff(_fTimeDelta);

	if (nullptr != m_pModelBodyCom)
	{
		if (0.f < m_fCurrent_Hit_WaitTime)
		{
			m_fCurrent_Hit_WaitTime -= _fTimeDelta;
		}
		else
		{
			m_pModelBodyCom->Play_Animation(_fTimeDelta);
		}
	}

	CAnimationState* pCurrentAnimationState = m_pModelBodyCom->Get_Animator()->Get_CurAnimState();
	AnimState eAnimState = pCurrentAnimationState->Get_eAnimStateTag();
	_float fTrackPosition = m_pModelBodyCom->Get_Animator()->Get_TrackPosition();
	if ((AnimState::Hand_RollForward == eAnimState || AnimState::Hand_RollBack == eAnimState ||
		 AnimState::Hand_RollForwardAir == eAnimState || AnimState::Hand_RollBackAir == eAnimState) &&
		 0.f >= m_fCurrentCoolTimeForTimeillusion)
	{
		m_bDashForTimeillusion = true;
	}
	else
	{
		m_bDashForTimeillusion = false;
	}

	if (0.f < m_fCurrentCoolTimeForTimeillusion)
	{
		m_fCurrentCoolTimeForTimeillusion += -_fTimeDelta;
	}


	if (PLAYER_WEAPON_MODE::DUALGUN == m_eWeaponMode)
	{
		Handling_Aimming(_fTimeDelta);
	}

	Handling_Jump(_fTimeDelta);
	if (PLAYER_STATE::Mounting_Idle == m_eCurState || PLAYER_STATE::Mounting_Jump == m_eCurState ||
		PLAYER_STATE::Mounting_Move == m_eCurState || PLAYER_STATE::Mounting_Up == m_eCurState)
	{
		Update_Motion_By_Mounting(_fTimeDelta);
	}
	else if (PLAYER_STATE::Gliding != m_eCurState && PLAYER_STATE::Gliding_Charge != m_eCurState)
	{
		m_pPhysics->Set_Max_Falling_Velocity(m_fYMaxPhysicsSpeed);
		Update_RootMotion(_fTimeDelta);
	}
	else if(PLAYER_STATE::Gliding == m_eCurState || PLAYER_STATE::Gliding_Charge == m_eCurState)
	{
		Update_Motion_By_Gliding(_fTimeDelta);
	}


	// Transfrom update by custom physics
	Vector4 vPlayerCurrentPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float fYGround;
	m_pNavigationCom->Get_YWorldCoordinate(vPlayerCurrentPosition, fYGround);
	m_pPhysics->Set_YGround(fYGround);

	Update_PlayerState(_fTimeDelta);
	Check_Weapon(_fTimeDelta);
	Update_AnimByPlayerState();
	Handling_CapturedMonster();
	Change_Collision_Transform_By_Attack_Animation();

	Matrix matWorldTransform = m_pTransformCom->Get_WorldMatrix();
	m_pPhysics->Tick(_fTimeDelta, matWorldTransform);
	m_pTransformCom->Set_WorldMatrix(matWorldTransform);


	//Vector4 vCurrentVelocity = m_pPhysics->Get_Current_Velocity();
	//_float fYTmpVelocity = vCurrentVelocity.y;
	//vCurrentVelocity.y = 0.f;

	//Vector4 vDir = vCurrentVelocity; vDir.w = 0.f; 
	//_float fLength = vDir.Length();
	//vDir.Normalize();
	//
	//if (0.5f >= vDir.Length())
	//{
	//	vCurrentVelocity.x = 0.f;
	//	vCurrentVelocity.z = 0.f;
	//}
	//else
	//{
	//	vCurrentVelocity += -vDir * 45.f *_fTimeDelta;
	//}
	//
	//
	//vCurrentVelocity.y = fYTmpVelocity;
	//m_pPhysics->Set_Velocity(vCurrentVelocity);

	//m_pTransformCom->Set_WorldMatrix(matWorldTransform);
	//Vector3 vNewPosition = matWorldTransform.Translation();
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, { vNewPosition.x, vNewPosition.y, vNewPosition.z, 1.f}, m_pNavigationCom);

	
	// To-Do : may be handled by State
	AnimState eCurAnimState = m_pModelBodyCom->Get_Animator()->Get_CurAnimState()->Get_eAnimStateTag();
	if (AnimState::Sickle_Melee_Air_Attack01 != eCurAnimState &&
		AnimState::Sickle_Melee_Air_Attack02 != eCurAnimState &&
		AnimState::Sickle_Melee_Air_Attack03 != eCurAnimState &&
		AnimState::Sickle_Melee_Air_Attack04 != eCurAnimState &&
		AnimState::Sickle_Melee_Air_Attack05Start != eCurAnimState &&
		AnimState::Sickle_Melee_Attack05 != eCurAnimState &&
		AnimState::Linghan_Melee_Air_Attack01 != eCurAnimState &&
		AnimState::Linghan_Melee_Air_Attack02 != eCurAnimState &&
		AnimState::Linghan_Melee_Air_Attack03 != eCurAnimState &&
		AnimState::Linghan_Melee_Air_Attack04 != eCurAnimState &&
		AnimState::Linghan_Melee_Air_Attack05 != eCurAnimState &&
		AnimState::Linghan_Melee_Air_Attack05_1 != eCurAnimState && 
		AnimState::Linghan_Air_Dash_Attack != eCurAnimState &&
		AnimState::Dual_Gun_Air_Skill_Begin != eCurAnimState &&
		AnimState::Dual_Gun_Air_Skill_Loop != eCurAnimState &&
		AnimState::Dual_Gun_Air_Skill_End != eCurAnimState &&
		AnimState::Dual_Gun_DropKick_Begin != eCurAnimState &&
		AnimState::Hand_RollForwardAir != eCurAnimState &&
		AnimState::Hand_RollBackAir != eCurAnimState &&
		AnimState::Gun_Air_Jump_Normal_Attack != eCurAnimState)
	{
		m_pPhysics->Set_Enable_Velocity(true);
		m_pPhysics->Set_Enable_Gravity(true);
	}
	else
	{
		m_pPhysics->Set_Enable_Gravity(false);
		m_pPhysics->Set_Enable_Velocity(false);
	}

	Update_PlayerTurn(_fTimeDelta);
	//Handling_Jump(_fTimeDelta);
	Handling_Skill(_fTimeDelta);
	Handling_HitThrow(_fTimeDelta);

	if (PLAYER_STATE::Idle != m_eCurState && 
		PLAYER_STATE::Mounting_Idle != m_eCurState &&
		PLAYER_STATE::Mounting_Jump != m_eCurState &&
		PLAYER_STATE::Mounting_Move != m_eCurState &&
		PLAYER_STATE::Mounting_Up != m_eCurState &&
		AnimState::Hand_Stand == eCurAnimState)
	{
		m_eCurState = PLAYER_STATE::Idle;
		m_ePrevState = PLAYER_STATE::Idle;
	}


	

	if (nullptr != m_pMount && 
		(PLAYER_STATE::Mounting_Idle == m_eCurState || PLAYER_STATE::Mounting_Jump == m_eCurState || 
		 PLAYER_STATE::Mounting_Move == m_eCurState || PLAYER_STATE::Mounting_Up == m_eCurState))
	{
		m_pMount->Tick(_fTimeDelta);

		Matrix matWorld = static_cast<CPlayer_Mount*>(m_pMount)->Get_Specific_World_Bone_Matrix("");

		Vector4 vNewPosition = { matWorld._41, matWorld._42, matWorld._43, 1.f };
		m_pTransformModelDrawCom->Set_State(CTransform::STATE_POSITION, vNewPosition);
		//
		Vector3 vTmp = matWorld.Forward();
		vTmp.Normalize();
		//
		m_pTransformModelDrawCom->Set_NewLook({ vTmp.x, vTmp.y, vTmp.z, 0.f });



		AnimState eCurrentAnimationState = m_pModelBodyCom->Get_Animator()->Get_CurAnimState()->Get_eAnimStateTag();

		if (AnimState::Hand_Mount_Fox_Idle == eCurrentAnimationState || AnimState::Hand_Mount_Fox_up == eCurrentAnimationState)
		{
			Matrix matRotatedWorld = m_pTransformModelDrawCom->Get_WorldMatrix();
			Vector4 vNormalizedLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			vNormalizedLook.Normalize();

			Matrix matTranslation = XMMatrixTranslation(-0.12f, 0.f, 0.f);
			Matrix matRotation = XMMatrixRotationAxis({ 0.f, 0.f, 1.f, 0.f }, XMConvertToRadians(5.f));
			matRotatedWorld = matRotation * matTranslation * matRotatedWorld;

			m_pTransformModelDrawCom->Set_WorldMatrix(matRotatedWorld);
		}
	}

	auto iter = m_mapPlayerParts.find(L"Equip_Hair");
	if (m_mapPlayerParts.end() != iter)
	{
		static_cast<CPlayer_Acc_Hair*>(iter->second)->Tick(_fTimeDelta);
	}

	if (nullptr != m_pCurWeapon)
	{
		m_pCurWeapon->Tick(_fTimeDelta);
	}

	if (nullptr != m_pGlider && (PLAYER_STATE::Gliding == m_eCurState || PLAYER_STATE::Gliding_Charge == m_eCurState))
	{
		m_pGlider->Tick(_fTimeDelta);
	}



	if (nullptr != m_pMeleeAttackColliderCom)
	{
		m_pMeleeAttackColliderCom->Tick(m_pTransformModelDrawCom->Get_WorldMatrix());
	}

	if (nullptr != m_pMeleeOBBAttackColliderCom)
	{
		m_pMeleeOBBAttackColliderCom->Tick(m_pTransformModelDrawCom->Get_WorldMatrix());
	}

	if (nullptr != m_pPlayerBodyColliderCom)
	{
		m_pPlayerBodyColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
	}

	if (nullptr != m_pPlayerBodyHitColliderCom)
	{
		m_pPlayerBodyHitColliderCom->Tick(m_pTransformModelDrawCom->Get_WorldMatrix());
	}

	if (nullptr != m_pPlayerHitTimeillusionColliderCom)
	{
		m_pPlayerHitTimeillusionColliderCom->Tick(m_pTransformModelDrawCom->Get_WorldMatrix());
	}

	if (nullptr != m_pPlayerInteractionColliderCom)
	{
		m_pPlayerInteractionColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
	}

	// INHO
	_Regen_EP(_fTimeDelta);
	Add_SP(0.1f);
	_Switch_Energy();
	_On_SwapSkill();
	Add_Hp_Using_Satiety(_fTimeDelta);

	// F2키 눌렀을 때 퀵슬롯 음식 섭취
	if (CFunc::Is_KeyDown(DIK_F2, false))
		Eat_Food();

	_Handling_Eat_Food(_fTimeDelta);

#ifdef _DEBUG
	//Add_SP(m_tPlayerStateDesc.fMaxSP);
#endif

	Vector3 vCurrentPlayerPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_vADS_Camera_At = { vCurrentPlayerPosition.x, vCurrentPlayerPosition.y, vCurrentPlayerPosition.z };
	m_ePrevState = m_eCurState;
	
}

void CPlayer::Late_Tick(_float _fTimeDelta)
{
	if (!m_bEnablePlayer)
	{
		return;
	}

	__super::Late_Tick(_fTimeDelta);

	if (nullptr != m_pMount && 
		(PLAYER_STATE::Mounting_Idle == m_eCurState || PLAYER_STATE::Mounting_Jump == m_eCurState ||
		 PLAYER_STATE::Mounting_Move == m_eCurState || PLAYER_STATE::Mounting_Up == m_eCurState))
	{
		m_pMount->Late_Tick(_fTimeDelta);
	}

	auto iter = m_mapPlayerParts.find(L"Equip_Hair");
	if (m_mapPlayerParts.end() != iter)
	{
		static_cast<CPlayer_Acc_Hair*>(iter->second)->Late_Tick(_fTimeDelta);
	}

	if (nullptr != m_pCurWeapon)
	{
		m_pCurWeapon->Late_Tick(_fTimeDelta);
	}

	if (nullptr != m_pGlider && (PLAYER_STATE::Gliding == m_eCurState || PLAYER_STATE::Gliding_Charge == m_eCurState))
	{
		m_pGlider->Late_Tick(_fTimeDelta);
	}

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PLAYER, this);

}

HRESULT CPlayer::Update_Motion_By_Mounting(_float _fTimeDelta)
{
	CAnimator* pAnimator = m_pModelBodyCom->Get_Animator();
	CAnimationState* pCurrentAnimationState = pAnimator->Get_CurAnimState();
	AnimState					eCurrentAnimationState = pCurrentAnimationState->Get_eAnimStateTag();
	CPlayer_Mount::MOUNT_STATE	eCurrentMountState = static_cast<CPlayer_Mount*>(m_pMount)->Get_Current_Mount_State();

	if (AnimState::Hand_Mount_Fox_up == eCurrentAnimationState)
	{
		return S_OK;
	}

	if (PLAYER_STATE::Mounting_Move != m_eCurState && PLAYER_STATE::Mounting_Jump != m_eCurState)
	{
		return S_OK;
	}

	Vector4 vCurrentPlayerPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	Vector4 vCurrentPlayerLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	vCurrentPlayerLook.y = 0.f; vCurrentPlayerLook.Normalize();

	_bool bSpaceTap = CFunc::Is_KeyDown(DIK_SPACE);
	_bool bMoveInput = Is_MoveInput();


	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Vector4 vTowardDirection = { 0.f, 0.f, 0.f, 0.f };

	if ((CFunc::Is_KeyDown(DIK_W) || CFunc::Is_KeyHold(DIK_W)) &&
		(CFunc::Is_KeyDown(DIK_D) || CFunc::Is_KeyHold(DIK_D)))
	{
		_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(45.f));
		_matrix matViewInv = XMMatrixInverse(nullptr, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
		vTowardDirection = XMVector3TransformNormal(XMVector3Normalize(XMVectorSetY(matViewInv.r[2], 0.f)), matRotY);
	}
	else if ((CFunc::Is_KeyDown(DIK_W) || CFunc::Is_KeyHold(DIK_W)) &&
		(CFunc::Is_KeyDown(DIK_A) || CFunc::Is_KeyHold(DIK_A)))
	{
		_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(-45.f));
		_matrix matViewInv = XMMatrixInverse(nullptr, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
		vTowardDirection = XMVector3TransformNormal(XMVector3Normalize(XMVectorSetY(matViewInv.r[2], 0.f)), matRotY);
	}
	else if ((CFunc::Is_KeyDown(DIK_S) || CFunc::Is_KeyHold(DIK_S)) &&
		(CFunc::Is_KeyDown(DIK_A) || CFunc::Is_KeyHold(DIK_A)))
	{
		_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(-45.f));
		_matrix matViewInv = XMMatrixInverse(nullptr, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
		vTowardDirection = XMVector3TransformNormal(XMVector3Normalize(XMVectorSetY(matViewInv.r[0], 0.f)), matRotY) * -1.f;
	}
	else if ((CFunc::Is_KeyDown(DIK_S) || CFunc::Is_KeyHold(DIK_S)) &&
		(CFunc::Is_KeyDown(DIK_D) || CFunc::Is_KeyHold(DIK_D)))
	{
		_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(45.f));
		_matrix matViewInv = XMMatrixInverse(nullptr, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
		vTowardDirection = XMVector3TransformNormal(XMVector3Normalize(XMVectorSetY(matViewInv.r[0], 0.f)), matRotY);
	}
	else if (CFunc::Is_KeyDown(DIK_W) || CFunc::Is_KeyHold(DIK_W))
	{
		_matrix matViewInv = XMMatrixInverse(nullptr, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
		vTowardDirection = XMVector3Normalize(XMVectorSetY(matViewInv.r[2], 0.f));
	}
	else if (CFunc::Is_KeyDown(DIK_A) || CFunc::Is_KeyHold(DIK_A))
	{
		_matrix matViewInv = XMMatrixInverse(nullptr, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
		vTowardDirection = XMVector3Normalize(XMVectorSetY(matViewInv.r[0], 0.f)) * -1.f;
	}
	else if (CFunc::Is_KeyDown(DIK_D) || CFunc::Is_KeyHold(DIK_D))
	{
		_matrix matViewInv = XMMatrixInverse(nullptr, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
		vTowardDirection = XMVector3Normalize(XMVectorSetY(matViewInv.r[0], 0.f));
	}
	else if (CFunc::Is_KeyDown(DIK_S) || CFunc::Is_KeyHold(DIK_S))
	{
		_matrix matViewInv = XMMatrixInverse(nullptr, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
		vTowardDirection = XMVector3Normalize(XMVectorSetY(matViewInv.r[2], 0.f)) * -1.f;
	}
	else
	{
		Clear_Damping();
	}

	//Vector4 vVelocity = m_pPhysics->Get_Current_Velocity();
	//_float fYTmpVelocity = vVelocity.y;
	//vVelocity.y = 0.f;
	//
	//vVelocity += (vTowardDirection * 100.f) * _fTimeDelta ;
	//
	//if (vVelocity.Length() < 50.f)
	//{
	//	vVelocity.y = fYTmpVelocity;
	//	m_pPhysics->Set_Velocity(vVelocity);
	//}
	//else
	//{
	//	vVelocity.Normalize();
	//	vVelocity = vVelocity * 50.f;
	//	vVelocity.y = fYTmpVelocity;
	//	m_pPhysics->Set_Velocity(vVelocity);
	//}


	vCurrentPlayerPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vCurrentPlayerPosition += vTowardDirection * m_fMountVelocity * _fTimeDelta;
	//here
	constexpr _int iStartIndex = 512 * 512 * 2;

	if (PLAYER_STATE::Mounting_Jump == m_eCurState)
	{
		_int iFindNewCell = m_pNavigationCom->Find_Cell_Specific_Range(vCurrentPlayerPosition, iStartIndex);
		_int iGroundCell = m_pNavigationCom->Find_Cell_By_Distribution_Position(vCurrentPlayerPosition);

		if (-1 == iGroundCell && 100 > m_pNavigationCom->Get_Cell_Count())
		{
			iGroundCell = m_pNavigationCom->Get_Index(vCurrentPlayerPosition);
		}

		if (-1 != iFindNewCell && -1 != iGroundCell)
		{
			_float fNewYDistance = 0.f;
			_float fGroundYDistance = 0.f;

			m_pNavigationCom->Get_YDistance(vCurrentPlayerPosition, iFindNewCell, fNewYDistance);
			m_pNavigationCom->Get_YDistance(vCurrentPlayerPosition, iGroundCell, fGroundYDistance);


			if (fGroundYDistance >= fNewYDistance)
			{
				m_pNavigationCom->Set_CurrentIndex(iFindNewCell);
			}
			else
			{
				m_pNavigationCom->Set_CurrentIndex(iGroundCell);
			}

			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCurrentPlayerPosition, m_pNavigationCom);

		}
		else if (-1 == iFindNewCell && -1 != iGroundCell)
		{
			m_pNavigationCom->Set_CurrentIndex(iGroundCell);
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCurrentPlayerPosition, m_pNavigationCom);
		}
		else if (-1 != iFindNewCell && -1 == iGroundCell)
		{
			m_pNavigationCom->Set_CurrentIndex(iFindNewCell);
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCurrentPlayerPosition, m_pNavigationCom);
		}
	}
	else
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCurrentPlayerPosition, m_pNavigationCom);
	}
			

	if (m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCurrentPlayerPosition, m_pNavigationCom))
	{
		if (PLAYER_STATE::Mounting_Idle == m_eCurState || PLAYER_STATE::Mounting_Move == m_eCurState)
		{
			_float fYWorldPosition = 0.f;
	
			m_pNavigationCom->Get_YWorldCoordinate(vCurrentPlayerPosition, fYWorldPosition);
			vCurrentPlayerPosition.y = fYWorldPosition;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCurrentPlayerPosition);
		}
	}
	

	vCurrentPlayerPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float fYRootMotion = m_pModelBodyCom->Get_SecondRootBone_AccumulatedRootMotion();
	_float fYMove = static_cast<CPlayer_Mount*>(m_pMount)->Get_Y_Root_Motion();

	vCurrentPlayerPosition.y += -fYRootMotion * 0.025f + fYMove * -0.0175 + m_fYMountingModelDrawOffset;

	m_pTransformModelDrawCom->Set_WorldMatrix(m_pTransformCom->Get_WorldFloat4x4());
	m_pTransformModelDrawCom->Set_State(CTransform::STATE_POSITION, vCurrentPlayerPosition);

	if (AnimState::Hand_Mount_Fox_Idle == eCurrentAnimationState || AnimState::Hand_Mount_Fox_up == eCurrentAnimationState)
	{
		Matrix matRotatedWorld = m_pTransformModelDrawCom->Get_WorldMatrix();
		Vector4 vNormalizedLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		vNormalizedLook.Normalize();

		Matrix matTranslation = XMMatrixTranslation(-0.12f, 0.f, 0.f);
		Matrix matRotation = XMMatrixRotationAxis({ 0.f, 0.f, 1.f, 0.f }, XMConvertToRadians(20.f));
		matRotatedWorld = matRotation * matRotatedWorld;

		m_pTransformModelDrawCom->Set_WorldMatrix(matRotatedWorld);
	}

	return S_OK;
}


HRESULT CPlayer::Update_Motion_By_Gliding(_float _fTimeDelta)
{
	// 동작 transform은 여기서 처리.
	// 애니메이션은 control module에서 처리.

	AnimState eCurrentAnimationState = m_pModelBodyCom->Get_Animator()->Get_CurAnimState()->Get_eAnimStateTag();

	Vector4 vCurrentPlayerPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (PLAYER_STATE::Gliding_Charge == m_eCurState)
	{
		// charging 관련 root motion 처리

		_float fYRootMotion = m_pModelBodyCom->Get_SecondRootBone_AccumulatedRootMotion();
		vCurrentPlayerPosition.y += -fYRootMotion * 0.025f;

		m_pTransformModelDrawCom->Set_WorldMatrix(m_pTransformCom->Get_WorldFloat4x4());
		m_pTransformModelDrawCom->Set_State(CTransform::STATE_POSITION, vCurrentPlayerPosition);

		return S_OK;
	}

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Matrix matViewInv = pGameInstance->Get_Transform_Float4x4_Inverse(CPipeLine::D3DTS_VIEW);
	Vector3 vLook = matViewInv.Backward();
	Vector4 vLookNormalized = { vLook.x, vLook.y, vLook.z, 0.f };

	Vector4 vPlayerLookNormalized = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	vPlayerLookNormalized.y = vLookNormalized.y;
	// For. Debug god mode, later need to erase.

	vPlayerLookNormalized.Normalize();
	vLookNormalized.Normalize();

	if (AnimState::Hand_Glid_Open != eCurrentAnimationState)
	{
		Vector4 vDir = { 0.f, 0.f, 0.f, 0.f };

		if ((CFunc::Is_KeyDown(DIK_W) || CFunc::Is_KeyHold(DIK_W)) &&
			(CFunc::Is_KeyDown(DIK_D) || CFunc::Is_KeyHold(DIK_D)))
		{
			_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(45.f));
			_matrix matViewInv = XMMatrixInverse(nullptr, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
			vDir = XMVector3TransformNormal(XMVector3Normalize(XMVectorSetY(matViewInv.r[2], 0.f)), matRotY);
		}
		else if ((CFunc::Is_KeyDown(DIK_W) || CFunc::Is_KeyHold(DIK_W)) &&
			(CFunc::Is_KeyDown(DIK_A) || CFunc::Is_KeyHold(DIK_A)))
		{
			_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(-45.f));
			_matrix matViewInv = XMMatrixInverse(nullptr, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
			vDir = XMVector3TransformNormal(XMVector3Normalize(XMVectorSetY(matViewInv.r[2], 0.f)), matRotY);
		}
		else if ((CFunc::Is_KeyDown(DIK_S) || CFunc::Is_KeyHold(DIK_S)) &&
			(CFunc::Is_KeyDown(DIK_A) || CFunc::Is_KeyHold(DIK_A)))
		{
			_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(-45.f));
			_matrix matViewInv = XMMatrixInverse(nullptr, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
			vDir = XMVector3TransformNormal(XMVector3Normalize(XMVectorSetY(matViewInv.r[0], 0.f)), matRotY) * -1.f;
		}
		else if ((CFunc::Is_KeyDown(DIK_S) || CFunc::Is_KeyHold(DIK_S)) &&
			(CFunc::Is_KeyDown(DIK_D) || CFunc::Is_KeyHold(DIK_D)))
		{
			_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(45.f));
			_matrix matViewInv = XMMatrixInverse(nullptr, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
			vDir = XMVector3TransformNormal(XMVector3Normalize(XMVectorSetY(matViewInv.r[0], 0.f)), matRotY);
		}
		else if (CFunc::Is_KeyDown(DIK_W) || CFunc::Is_KeyHold(DIK_W))
		{
			_matrix matViewInv = XMMatrixInverse(nullptr, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
			vDir = XMVector3Normalize(XMVectorSetY(matViewInv.r[2], 0.f));
		}
		else if (CFunc::Is_KeyDown(DIK_A) || CFunc::Is_KeyHold(DIK_A))
		{
			_matrix matViewInv = XMMatrixInverse(nullptr, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
			vDir = XMVector3Normalize(XMVectorSetY(matViewInv.r[0], 0.f)) * -1.f;
		}
		else if (CFunc::Is_KeyDown(DIK_D) || CFunc::Is_KeyHold(DIK_D))
		{
			_matrix matViewInv = XMMatrixInverse(nullptr, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
			vDir = XMVector3Normalize(XMVectorSetY(matViewInv.r[0], 0.f));
		}
		else if (CFunc::Is_KeyDown(DIK_S) || CFunc::Is_KeyHold(DIK_S))
		{
			_matrix matViewInv = XMMatrixInverse(nullptr, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
			vDir = XMVector3Normalize(XMVectorSetY(matViewInv.r[2], 0.f)) * -1.f;
		}
		else
		{
			Clear_Damping();
		}

		if (LEVEL_DESERT == m_eLevel)
		{
			vCurrentPlayerPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			vCurrentPlayerPosition += vDir * 50.f * _fTimeDelta;

			constexpr _int iStartIndex = 512 * 512 * 2;

			_int iFindNewCell = m_pNavigationCom->Find_Cell_Specific_Range(vCurrentPlayerPosition, iStartIndex);
			_int iGroundCell = m_pNavigationCom->Find_Cell_By_Distribution_Position(vCurrentPlayerPosition);

			if (-1 == iGroundCell && 100 > m_pNavigationCom->Get_Cell_Count())
			{
				iGroundCell = m_pNavigationCom->Get_Index(vCurrentPlayerPosition);
			}

			if (-1 != iFindNewCell && -1 != iGroundCell)
			{
				_float fNewYDistance = 0.f;
				_float fGroundYDistance = 0.f;

				m_pNavigationCom->Get_YDistance(vCurrentPlayerPosition, iFindNewCell, fNewYDistance);
				m_pNavigationCom->Get_YDistance(vCurrentPlayerPosition, iGroundCell, fGroundYDistance);

				if (fGroundYDistance >= fNewYDistance)
				{
					m_pNavigationCom->Set_CurrentIndex(iFindNewCell);
				}
				else
				{
					m_pNavigationCom->Set_CurrentIndex(iGroundCell);
				}

				m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCurrentPlayerPosition, m_pNavigationCom);

			}
			else if (-1 == iFindNewCell && -1 != iGroundCell)
			{
				m_pNavigationCom->Set_CurrentIndex(iGroundCell);
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCurrentPlayerPosition, m_pNavigationCom);
			}
			else if (-1 != iFindNewCell && -1 == iGroundCell)
			{
				m_pNavigationCom->Set_CurrentIndex(iFindNewCell);
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCurrentPlayerPosition, m_pNavigationCom);
			}

			vCurrentPlayerPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		}
		else
		{
			vCurrentPlayerPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			vCurrentPlayerPosition += vDir * 50.f * _fTimeDelta;

			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCurrentPlayerPosition, m_pNavigationCom);
			vCurrentPlayerPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		}		
	}

	_float fYRootMotion = m_pModelBodyCom->Get_SecondRootBone_AccumulatedRootMotion();
	vCurrentPlayerPosition.y += -fYRootMotion * 0.025f;

	m_pTransformModelDrawCom->Set_WorldMatrix(m_pTransformCom->Get_WorldFloat4x4());
	m_pTransformModelDrawCom->Set_State(CTransform::STATE_POSITION, vCurrentPlayerPosition);


	return S_OK;
}

HRESULT CPlayer::Update_RootMotion(_float _fTimeDelta)
{
	// 실질적인 플레이어 transform 제어 부분
	Handling_XZRootMotion(_fTimeDelta);
	Handling_YRootMotion(_fTimeDelta);

	return S_OK;
}


void CPlayer::Handling_CapturedMonster()
{
	// 한번 띄우고 마는 공격이 아니라, 띄우면서 같이 올라가는 공격의 경우 처리함.
	auto iter = m_setCapturedMonster.begin();

	while (m_setCapturedMonster.end() != iter)
	{

		if ((*iter)->Get_Dead())
		{
			(*iter)->Set_Captured(false);
			iter = m_setCapturedMonster.erase(iter);
		}
		else if (!(*iter)->Get_Captured())
		{
			// 공중으로 띄워 올렸지만,
			// 공중에서 히트가 안된경우 일정 시간이 지나면 fall로
			(*iter)->Set_Captured(false);
			iter = m_setCapturedMonster.erase(iter);
		}
		else
		{
			++iter;
		}
	}





	CAnimationState* pCurrentAnimationState = m_pModelBodyCom->Get_Animator()->Get_CurAnimState();
	AnimState			eCurrentAnimationState = pCurrentAnimationState->Get_eAnimStateTag();

	if (AnimState::Sickle_Melee_Air_Attack01 != eCurrentAnimationState &&
		AnimState::Sickle_Melee_Air_Attack02 != eCurrentAnimationState &&
		AnimState::Sickle_Melee_Air_Attack03 != eCurrentAnimationState &&
		AnimState::Sickle_Melee_Air_Attack04 != eCurrentAnimationState &&
		AnimState::Sickle_Melee_Attack05 != eCurrentAnimationState)
	{
		for (auto& pMonster : m_setCapturedMonster)
		{
			pMonster->Set_Captured(false);
		}

		m_setCapturedMonster.clear();
	}
}

HRESULT CPlayer::Handling_XZRootMotion(_float _fTimeDelta)
{
	CAnimator* pAnimator = m_pModelBodyCom->Get_Animator();

	Vector4 xmVecLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK); xmVecLook.y = 0.f; xmVecLook.Normalize();
	Vector4 xmVecCurrentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	Vector4 xmVecNewPos = xmVecCurrentPos;

	_bool bMoveInput = Is_MoveInput();
	_bool bShiftTap = CFunc::Is_KeyDown(DIK_LSHIFT);
	_bool bShiftHold = CFunc::Is_KeyHold(DIK_LSHIFT);

	// XZ Root motion handling first here.
	if (nullptr != pAnimator->Get_CurAnimState())
	{
		CAnimationState* pCurrentAnimationState = pAnimator->Get_CurAnimState();

		if (pCurrentAnimationState->Get_RootMotionEnable())
		{
			_float fXZDeltaRootMotion = m_pModelBodyCom->Get_FirstRootBone_XZDelta();
			_float fXZDeltaPower = 1.f;

			if (PLAYER_STATE::DASH_BACK == m_eCurState)
			{
				fXZDeltaPower = 0.025f;
			}
			else
			{
				fXZDeltaPower = 0.025f;//0.05f;
			}

			xmVecNewPos = xmVecCurrentPos + xmVecLook * fXZDeltaPower * fXZDeltaRootMotion;

			if (m_bEnableConstraint)
			{
				if (Is_In_Cube(m_vConstraintCenterPosition, m_vConstraintSize, xmVecNewPos))
				{
					m_pTransformCom->Set_State(CTransform::STATE_POSITION, xmVecNewPos, m_pNavigationCom);
				}
			}
			else
			{
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, xmVecNewPos, m_pNavigationCom);
			}
			
		}
		else if (PLAYER_STATE::Aimming_Move != m_eCurState && PLAYER_STATE::Aimming_Move_Attack != m_eCurState)
		{
			_float fSpeed = 0.f;

			if (PLAYER_STATE::Walk == m_eCurState) { fSpeed = m_fWalkSpeed; }
			else if (PLAYER_STATE::Run == m_eCurState) { fSpeed = m_fRunSpeed; }
			else if (PLAYER_STATE::Sprint == m_eCurState) { fSpeed = m_fSprintSpeed; }
			else if (PLAYER_STATE::DASH_FRONT == m_eCurState)
			{
				m_fYDashAccTime += _fTimeDelta * 2.f;
				fSpeed = m_fInitialDashSpeed + -10.f * m_fYDashAccTime;
			}
			else if (PLAYER_STATE::Jump == m_eCurState || PLAYER_STATE::Jump_Air == m_eCurState)
			{
				if (bMoveInput)
				{
					if (bShiftHold || bShiftTap)
					{
						fSpeed = m_fSprintSpeed;
					}
					else
					{
						fSpeed = m_fRunSpeed;
					}
				}
			}

			xmVecNewPos = xmVecCurrentPos + xmVecLook * fSpeed * _fTimeDelta;

			constexpr _int iStartIndex = 512 * 512 * 2;

			if (LEVEL_DESERT == m_eLevel)
			{
				//if ((PLAYER_STATE::Jump == m_eCurState || PLAYER_STATE::Jump_Air == m_eCurState ||
				//	 PLAYER_STATE::Gliding == m_eCurState || PLAYER_STATE::Gliding_Charge == m_eCurState) &&
				//	!m_pTransformCom->Set_Position_No_Sliding(xmVecNewPos, m_pNavigationCom))
				//{
				//	_int iFindNewCell = m_pNavigationCom->Find_Cell_Specific_Range(xmVecNewPos, iStartIndex);
				//	_int iGroundCell = m_pNavigationCom->Find_Cell_By_Distribution_Position(xmVecNewPos);
				//
				//	if (-1 != iFindNewCell && -1 != iGroundCell)
				//	{
				//		_float fNewYDistance = 0.f;
				//		_float fGroundYDistance = 0.f;
				//
				//		m_pNavigationCom->Get_YDistance(xmVecNewPos, iFindNewCell, fNewYDistance);
				//		m_pNavigationCom->Get_YDistance(xmVecNewPos, iGroundCell, fGroundYDistance);
				//
				//		m_pTransformCom->Set_State(CTransform::STATE_POSITION, xmVecNewPos, m_pNavigationCom);
				//
				//		if (fGroundYDistance >= fNewYDistance)
				//		{
				//			m_pNavigationCom->Set_CurrentIndex(iFindNewCell);
				//		}
				//		else
				//		{
				//			m_pNavigationCom->Set_CurrentIndex(iGroundCell);
				//		}
				//		
				//	}
				//	else if (-1 == iFindNewCell && -1 != iGroundCell)
				//	{
				//		m_pTransformCom->Set_State(CTransform::STATE_POSITION, xmVecNewPos, m_pNavigationCom);
				//		m_pNavigationCom->Set_CurrentIndex(iGroundCell);
				//	}
				//
				//}
				//else if ((PLAYER_STATE::Jump == m_eCurState || PLAYER_STATE::Jump_Air == m_eCurState ||
				if ((PLAYER_STATE::Jump == m_eCurState || PLAYER_STATE::Jump_Air == m_eCurState ||
					PLAYER_STATE::Gliding == m_eCurState || PLAYER_STATE::Gliding_Charge == m_eCurState || PLAYER_STATE::Mounting_Jump == m_eCurState ||
					PLAYER_STATE::Dash_Front_Air == m_eCurState || PLAYER_STATE::Dash_Back_Air == m_eCurState ))
				{
					_int iFindNewCell = m_pNavigationCom->Find_Cell_Specific_Range(xmVecNewPos, iStartIndex);
					_int iGroundCell = m_pNavigationCom->Find_Cell_By_Distribution_Position(xmVecNewPos);
					
					if (-1 == iGroundCell && 100 > m_pNavigationCom->Get_Cell_Count())
					{
						iGroundCell = m_pNavigationCom->Get_Index(xmVecNewPos);
					}

					if (-1 != iFindNewCell && -1 != iGroundCell)
					{
						_float fNewYDistance = 0.f;
						_float fGroundYDistance = 0.f;
			
						m_pNavigationCom->Get_YDistance(xmVecNewPos, iFindNewCell, fNewYDistance);
						m_pNavigationCom->Get_YDistance(xmVecNewPos, iGroundCell, fGroundYDistance);
			
								
						if (fGroundYDistance >= fNewYDistance)
						{
							m_pNavigationCom->Set_CurrentIndex(iFindNewCell);
						}
						else
						{
							m_pNavigationCom->Set_CurrentIndex(iGroundCell);
						}

						m_pTransformCom->Set_State(CTransform::STATE_POSITION, xmVecNewPos, m_pNavigationCom);
			
					}
					else if (-1 == iFindNewCell && -1 != iGroundCell)
					{
						m_pNavigationCom->Set_CurrentIndex(iGroundCell);
						m_pTransformCom->Set_State(CTransform::STATE_POSITION, xmVecNewPos, m_pNavigationCom);
					}
					else if(-1 != iFindNewCell && -1 == iGroundCell)
					{
						m_pNavigationCom->Set_CurrentIndex(iFindNewCell);
						m_pTransformCom->Set_State(CTransform::STATE_POSITION, xmVecNewPos, m_pNavigationCom);
					}
				}
				else
				{
					m_pTransformCom->Set_State(CTransform::STATE_POSITION, xmVecNewPos, m_pNavigationCom);
				}
			}
			else
			{
				if (m_bEnableConstraint)
				{
					if (Is_In_Cube(m_vConstraintCenterPosition, m_vConstraintSize, xmVecNewPos))
					{
						m_pTransformCom->Set_State(CTransform::STATE_POSITION, xmVecNewPos, m_pNavigationCom);
					}
				}
				else
				{
					m_pTransformCom->Set_State(CTransform::STATE_POSITION, xmVecNewPos, m_pNavigationCom);
				}
			}

		}
		else if (PLAYER_STATE::Aimming_Move == m_eCurState || PLAYER_STATE::Aimming_Move_Attack == m_eCurState)
		{
			Move_Player_From_ADS_Mode(_fTimeDelta);
		}
	}


	Vector4 vPlayerPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (PLAYER_STATE::Idle == m_eCurState || PLAYER_STATE::Run == m_eCurState || PLAYER_STATE::Sprint == m_eCurState ||
		PLAYER_STATE::DASH_FRONT == m_eCurState || PLAYER_STATE::DASH_BACK == m_eCurState)
	{
		_float fYPosition = 0.f;
		if (m_pNavigationCom->Get_YWorldCoordinate(vPlayerPosition, fYPosition))
		{
			vPlayerPosition.y = fYPosition;
		}
	}
	else
	{
		_float fYDist = 0.f;
		CNavigation::INTERSECTDIRECTION eIntersectType = m_pNavigationCom->Get_YIntersectDistanceFromPoint(vPlayerPosition, fYDist);

		if (CNavigation::INTERSECTDIRECTION::UP == eIntersectType)
		{
			vPlayerPosition.y += fabsf(fYDist);
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPlayerPosition, m_pNavigationCom);
		}
	}


	if (m_bEnableConstraint && Is_In_Cube(m_vConstraintCenterPosition, m_vConstraintSize, vPlayerPosition))
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPlayerPosition, m_pNavigationCom);
	}
	else
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPlayerPosition, m_pNavigationCom);
	}

	return S_OK;
}

HRESULT	CPlayer::Handling_YRootMotion(_float _fTimeDelta)
{
	Vector4 vNewPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	// Y Root motion handling here.
	_float4x4 matWorld = m_pTransformCom->Get_WorldFloat4x4();
	m_pTransformModelDrawCom->Set_WorldMatrix(matWorld); // Setting right, up, look first.

	CAnimator* pAnimator = m_pModelBodyCom->Get_Animator();
	CAnimationState* pCurAnimState = pAnimator->Get_CurAnimState();
	AnimState			eCurAnimState = (AnimState)pCurAnimState->Get_eAnimStateTag();

	// Set Y for specific animation in hand. Otherwise just use Y root motion.
	if (AnimState::Sickle_Melee_Attack05 == eCurAnimState || AnimState::Sickle_Melee_Air_Attack01 == eCurAnimState)
	{
		m_fYRootMotion_Offset = m_pModelBodyCom->Get_SecondRootBone_AccumulatedRootMotion();

		if (pCurAnimState->Is_Transition())
		{

			vNewPos.y += m_fYRootMotion_Offset * -0.025f;

			//Vector4 vCurPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		}
		else
		{
			Vector4 vCurPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			if (AnimState::Sickle_Melee_Air_Attack01 != eCurAnimState)
			{
				if (AnimState::Sickle_Melee_Attack05 == eCurAnimState)
				{
					_float fTrackPosition = m_pModelBodyCom->Get_Animator()->Get_TrackPosition();
					if (52.f >= fTrackPosition)
					{
						vCurPos.y += m_pModelBodyCom->Get_SecondRootBone_YDelta() * -m_fSickleMelee05AttackYOffset;//-0.05f;//-0.10f;
					}
					m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCurPos, m_pNavigationCom);

				}
				else
				{
					vCurPos.y += m_pModelBodyCom->Get_SecondRootBone_YDelta() * -0.07f + m_fSickleAirAtkStart * _fTimeDelta;
					m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCurPos, m_pNavigationCom);
				}

			}
			vNewPos = vCurPos;
			vNewPos.y += m_fYRootMotion_Offset * -0.025f;
		}
	}
	else
	{
		_float fYRootMotion = m_pModelBodyCom->Get_SecondRootBone_AccumulatedRootMotion();
		vNewPos.y += -fYRootMotion * 0.025f;
	}


	if (AnimState::Sickle_Melee_Air_Attack01 == eCurAnimState && !pCurAnimState->Is_Transition() && !pCurAnimState->Is_PlayDone())
	{
		_float fY = m_pModelBodyCom->Get_SecondRootBone_AccumulatedRootMotion();
	}

	m_pTransformModelDrawCom->Set_State(CTransform::STATE_POSITION, vNewPos);


	return S_OK;
}
*/

HRESULT CPlayer::Bind_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	_float4x4 matView = pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
	_float4x4 matProj = pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ);

	m_pShaderCom->Bind_Matrix("g_ViewMatrix", &matView);
	m_pShaderCom->Bind_Matrix("g_ProjMatrix", &matProj);

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

	auto vCamForward = pGameInstance->Get_CamForward();
	m_pShaderCom->Bind("g_vCamDirection", vCamForward);

	m_pShaderCom->Bind("g_bUseRimLight", CRender_Manager::Get_Instance()->Is_Enable_RimLight());
	m_pShaderCom->Bind("g_vCharacterForward", vForward);
	m_pShaderCom->Bind("g_vCharacterRight", vRight);

	m_pShaderCom->Bind("g_vRimColor", _float4(1.f, 1.f, 1.f, 0.f));
	m_pShaderCom->Bind("g_fRimPower", 8.f);
	m_pShaderCom->Bind("g_fCameraFar", pGameInstance->Get_CameraFar());

	m_pTransformModelDrawCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix");

	return S_OK;
}

HRESULT CPlayer::Render()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint uMeshNums = m_pModelBodyCom->Get_NumMesh();
	auto& meshes = m_pModelBodyCom->Get_vMeshes();

	m_pShaderCom->Bind("g_CameraNoiseTexture", CGameInstance::Get_Instance()->Get_Texture_SRV(LEVEL_STATIC, PC_TEXTURE_NOISE, 2));

	// Outline
	for (_uint i = 0; i < uMeshNums; ++i)
	{
		m_pShaderCom->Bind("g_fOutlineWidth", CGlobal::g_fOutlineWidth);

		m_pModelBodyCom->Bind_ShaderBoneMatricies(m_pShaderCom, i, "g_BoneMatricies");
		m_pModelBodyCom->Bind_ShaderResourceView(m_pShaderCom, i);

		m_pShaderCom->Begin(SHADER_PASS_CHARACTER_OUTLINE);

		m_pModelBodyCom->Render(i);
	}

	// Shading
	for (_uint i = 0; i < uMeshNums; ++i)
	{
		// 얼굴인 경우
		if ("f_mimicry_face_06_Morpher" == meshes[i]->GetMeshName())
		{
			m_pModelBodyCom->Bind_ShaderBoneMatricies(m_pShaderCom, i, "g_BoneMatricies");
			m_pModelBodyCom->Bind_ShaderResourceView(m_pShaderCom, i);

			m_pShaderCom->Begin(SHADER_PASS_CHARACTER_FACE);
		}

		// 그 외의 경우
		else
		{
			m_pModelBodyCom->Bind_ShaderBoneMatricies(m_pShaderCom, i, "g_BoneMatricies");
			m_pModelBodyCom->Bind_ShaderResourceView(m_pShaderCom, i);

			m_pShaderCom->Begin(SHADER_PASS_CHARACTER_OTHER);
		}
		m_pModelBodyCom->Render(i);
	}

#ifdef _DEBUG
	if (nullptr != m_pMeleeOBBAttackColliderCom)
		m_pRendererCom->Add_DebugComponent(m_pMeleeOBBAttackColliderCom);

	if (nullptr != m_pPlayerBodyHitColliderCom)
		m_pRendererCom->Add_DebugComponent(m_pPlayerBodyHitColliderCom);
#endif

	return S_OK;
}

HRESULT CPlayer::Render_Shadow(list<CLight*>& _lights)
{
	m_pTransformModelDrawCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix");

	_uint uMeshNums = m_pModelBodyCom->Get_NumMesh();
	auto& meshes = m_pModelBodyCom->Get_vMeshes();

	for (_uint i = 0; i < uMeshNums; ++i)
	{
		m_pModelBodyCom->Bind_ShaderBoneMatricies(m_pShaderCom, i, "g_BoneMatricies");

		for (auto& pLight : _lights)
			pLight->Render_Shadow(m_pShaderCom, meshes[i], SHADER_PASS_CHARACTER_SHADOWMAP);
	}

	return S_OK;
}

/*
void CPlayer::Clear_Damping()
{
	// Clear and off damping when still damping operation is proceed.
	m_bSmoothRotation = false;
	m_fCurrentRoationVelocity = 0.f;
	m_vTargetLook = Vector4(0.f, 0.f, 0.f, 0.f);
}

CGameObject* CPlayer::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CPlayer* pInstance = new CPlayer(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to create : CPlayer");
	}

	return pInstance;
}


void CPlayer::Create_WeaponEffects()
{
	const _float4x4* pWorldmatrix = m_pTransformCom->Get_WorldMatrixFloat4x4Ptr();
	const _float4x4* pModelBoneMatrix = nullptr;
	_float4x4 ModelPivotMatrix = m_pModelBodyCom->Get_PivotMatrix();
	//Move
#pragma region Move
	function<void()> functionEffect = [_pOwner = this, _pWorldmatrix = pWorldmatrix]() {
		CModel* pModel = static_cast<CModel*>(_pOwner->Find_Component(L"Com_Model_Body"));
		pModel->Get_SecondRootBone_AccumulatedRootMotion();
		_pOwner->Create_Effect(TEXT("Char_h_Jump_Air_4_Client.Effect"), nullptr, _float3(0.f, 2.f, 0.f)); };
	m_pModelBodyCom->Add_EffectFunc(4, "", "char_h_jump_air", functionEffect);
	functionEffect = [_pOwner = this, _pWorldmatrix = pWorldmatrix]() { _pOwner->Create_Effect(TEXT("Char_h_Jump_Air_9_Client.Effect"), _pWorldmatrix, _float3(0.f, 2.f, 0.f), false, true); };
	m_pModelBodyCom->Add_EffectFunc(9, "", "char_h_jump_air", functionEffect);

	functionEffect = [_pOwner = this, _pWorldmatrix = pWorldmatrix]() { _pOwner->Create_Effect(TEXT("Char_h_Sprint_Client.Effect"), nullptr, _float3(0.f, 2.f, 1.f)); };
	m_pModelBodyCom->Add_EffectFunc(0, "", "char_h_sprint_47", functionEffect);
	functionEffect = [_pOwner = this, _pWorldmatrix = pWorldmatrix]() { _pOwner->Create_Effect(TEXT("Char_h_Sprint_Client.Effect"), nullptr, _float3(0.f, 2.f, 1.f)); };
	m_pModelBodyCom->Add_EffectFunc(5, "", "char_h_sprint_47", functionEffect);
	functionEffect = [_pOwner = this, _pWorldmatrix = pWorldmatrix]() { _pOwner->Create_Effect(TEXT("Char_h_Sprint_Client.Effect"), nullptr, _float3(0.f, 2.f, 1.f)); };
	m_pModelBodyCom->Add_EffectFunc(10, "", "char_h_sprint_47", functionEffect);
	functionEffect = [_pOwner = this, _pWorldmatrix = pWorldmatrix]() { _pOwner->Create_Effect(TEXT("Char_h_Sprint_Client.Effect"), nullptr, _float3(0.f, 2.f, 1.f)); };
	m_pModelBodyCom->Add_EffectFunc(15, "", "char_h_sprint_47", functionEffect);
	functionEffect = [_pOwner = this, _pWorldmatrix = pWorldmatrix]() { _pOwner->Create_Effect(TEXT("Char_h_Sprint_Client.Effect"), nullptr, _float3(0.f, 2.f, 1.f)); };
	m_pModelBodyCom->Add_EffectFunc(20, "", "char_h_sprint_47", functionEffect);
	functionEffect = [_pOwner = this, _pWorldmatrix = pWorldmatrix]() { _pOwner->Create_Effect(TEXT("Char_h_Sprint_Client.Effect"), nullptr, _float3(0.f, 2.f, 1.f)); };
	m_pModelBodyCom->Add_EffectFunc(25, "", "char_h_sprint_47", functionEffect);

	functionEffect = [_pOwner = this, _pWorldmatrix = pWorldmatrix]() { _pOwner->Create_Effect(TEXT("Char_h_Roll_Forward_2_Particle_Client.Effect"), _pWorldmatrix, _float3(0.f, 2.f, 0.f), false, true); };
	m_pModelBodyCom->Add_EffectFunc(0, "", "char_h_roll_forward", functionEffect);
	functionEffect = [_pOwner = this, _pWorldmatrix = pWorldmatrix]() { _pOwner->Create_Effect(TEXT("Char_h_Roll_Forward_6_Client_3.Effect"), nullptr, _float3(0.f, 2.f, 0.f)); };
	m_pModelBodyCom->Add_EffectFunc(6, "", "char_h_roll_forward", functionEffect);
	functionEffect = [_pOwner = this, _pWorldmatrix = pWorldmatrix]() { _pOwner->Create_Effect(TEXT("Char_h_Roll_Forward_6_Dust5_Client.Effect"), _pWorldmatrix, _float3(0.f, 2.f, 5.f), true); };
	m_pModelBodyCom->Add_EffectFunc(0, "", "char_h_roll_forward", functionEffect);

	functionEffect = [_pOwner = this, _pWorldmatrix = pWorldmatrix]() { _pOwner->Create_Effect(TEXT("Char_h_Roll_Forward_2_Particle_Client.Effect"), _pWorldmatrix, _float3(0.f, 2.f, 0.f), false, true); };
	m_pModelBodyCom->Add_EffectFunc(5, "", "char_h_roll_forward_air", functionEffect);
	functionEffect = [_pOwner = this, _pWorldmatrix = pWorldmatrix]() { _pOwner->Create_Effect(TEXT("Char_h_Roll_Forward_6_Client_3.Effect"), nullptr, _float3(0.f, 2.f, 0.f)); };
	m_pModelBodyCom->Add_EffectFunc(10, "", "char_h_roll_forward_air", functionEffect);
	functionEffect = [_pOwner = this, _pWorldmatrix = pWorldmatrix]() { _pOwner->Create_Effect(TEXT("Char_h_Roll_Forward_6_Dust5_Client.Effect"), _pWorldmatrix, _float3(0.f, 2.f, 5.f), true); };
	m_pModelBodyCom->Add_EffectFunc(5, "", "char_h_roll_forward_air", functionEffect);

	//Glider
	for (size_t i = 0; i < 4; i++)
	{
		_float fAnimFrame = 20.f * i + 0.5f;
		for (size_t j = 0; j < 6; j++)
		{
			string strAnim = "";
			if (j == 0)
				strAnim = "char_h_glid_loop";
			else if (j == 1)
				strAnim = "char_h_glid_loop_forward";
			else if (j == 2)
				strAnim = "char_h_glid_loop_left";
			else if (j == 3)
				strAnim = "char_h_glid_loop_right";
			else if (j == 4)
				strAnim = "char_h_glid_idleground";
			else if (j == 5)
				strAnim = "char_h_glid_sprintU_Start";

			functionEffect = [_pOwner = this, fAnimFrame, _pWorldmatrix = pWorldmatrix]() { _pOwner->Create_Effect(TEXT("Glider_Dust_Left.Effect"), _pWorldmatrix, _float3(-5.f, 2.f, -4.f), false, true); };
			m_pModelBodyCom->Add_EffectFunc(fAnimFrame, "", strAnim, functionEffect);
			functionEffect = [_pOwner = this, fAnimFrame, _pWorldmatrix = pWorldmatrix]() { _pOwner->Create_Effect(TEXT("Glider_Dust_Right.Effect"), _pWorldmatrix, _float3(5.f, 2.f, -4.f), false, true); };
			m_pModelBodyCom->Add_EffectFunc(fAnimFrame, "", strAnim, functionEffect);
			functionEffect = [_pOwner = this, fAnimFrame, _pWorldmatrix = pWorldmatrix]() { _pOwner->Create_Effect(TEXT("Glider_Dust_Top.Effect"), _pWorldmatrix, _float3(0.f, 6.f, -4.f), false, true); };
			m_pModelBodyCom->Add_EffectFunc(fAnimFrame, "", strAnim, functionEffect);
			functionEffect = [_pOwner = this, fAnimFrame, _pWorldmatrix = pWorldmatrix]() { _pOwner->Create_Effect(TEXT("Glider_Dust_Bottom.Effect"), _pWorldmatrix, _float3(0.f, -2.f, -4.f), false, true); };
			m_pModelBodyCom->Add_EffectFunc(fAnimFrame, "", strAnim, functionEffect);
		}
	}
	for (size_t i = 0; i < 2; i++)
	{
		_float fAnimFrame = 20.f * i + 5.5f;
		string strAnim = "char_h_glid_sprintF_Start";
		functionEffect = [_pOwner = this, fAnimFrame, _pWorldmatrix = pWorldmatrix]() { _pOwner->Create_Effect(TEXT("Glider_Dust_Left.Effect"), _pWorldmatrix, _float3(-5.f, 2.f, -4.f), false, true); };
		m_pModelBodyCom->Add_EffectFunc(fAnimFrame, "", strAnim, functionEffect);
		functionEffect = [_pOwner = this, fAnimFrame, _pWorldmatrix = pWorldmatrix]() { _pOwner->Create_Effect(TEXT("Glider_Dust_Right.Effect"), _pWorldmatrix, _float3(5.f, 2.f, -4.f), false, true); };
		m_pModelBodyCom->Add_EffectFunc(fAnimFrame, "", strAnim, functionEffect);
		functionEffect = [_pOwner = this, fAnimFrame, _pWorldmatrix = pWorldmatrix]() { _pOwner->Create_Effect(TEXT("Glider_Dust_Top.Effect"), _pWorldmatrix, _float3(0.f, 6.f, -4.f), false, true); };
		m_pModelBodyCom->Add_EffectFunc(fAnimFrame, "", strAnim, functionEffect);
		functionEffect = [_pOwner = this, fAnimFrame, _pWorldmatrix = pWorldmatrix]() { _pOwner->Create_Effect(TEXT("Glider_Dust_Bottom.Effect"), _pWorldmatrix, _float3(0.f, -2.f, -4.f), false, true); };
		m_pModelBodyCom->Add_EffectFunc(fAnimFrame, "", strAnim, functionEffect);
	}
	for (size_t i = 0; i < 3; i++)
	{
		_float fAnimFrame = 20.f * i + 0.5f;
		string strAnim = "char_h_glid_groundStartU";
		functionEffect = [_pOwner = this, fAnimFrame, _pWorldmatrix = pWorldmatrix]() { _pOwner->Create_Effect(TEXT("Glider_Dust_Left.Effect"), _pWorldmatrix, _float3(-5.f, 2.f, -4.f), false, true); };
		m_pModelBodyCom->Add_EffectFunc(fAnimFrame, "", strAnim, functionEffect);
		functionEffect = [_pOwner = this, fAnimFrame, _pWorldmatrix = pWorldmatrix]() { _pOwner->Create_Effect(TEXT("Glider_Dust_Right.Effect"), _pWorldmatrix, _float3(5.f, 2.f, -4.f), false, true); };
		m_pModelBodyCom->Add_EffectFunc(fAnimFrame, "", strAnim, functionEffect);
		functionEffect = [_pOwner = this, fAnimFrame, _pWorldmatrix = pWorldmatrix]() { _pOwner->Create_Effect(TEXT("Glider_Dust_Top.Effect"), _pWorldmatrix, _float3(0.f, 6.f, -4.f), false, true); };
		m_pModelBodyCom->Add_EffectFunc(fAnimFrame, "", strAnim, functionEffect);
		functionEffect = [_pOwner = this, fAnimFrame, _pWorldmatrix = pWorldmatrix]() { _pOwner->Create_Effect(TEXT("Glider_Dust_Bottom.Effect"), _pWorldmatrix, _float3(0.f, -2.f, -4.f), false, true); };
		m_pModelBodyCom->Add_EffectFunc(fAnimFrame, "", strAnim, functionEffect);
	}
	//Landing
	//char_h_jump_downlowrun
	//Left
	functionEffect = [_pOwner = this, _pWorldmatrix = pWorldmatrix]() {_pOwner->Create_Effect(TEXT("Char_Landing_Toe_Dust.Effect"), _pWorldmatrix, _float3(-0.05f, 0.35f, 0.15f), false, true); };
	m_pModelBodyCom->Add_EffectFunc(15.f, "", "char_h_jump_downlowrun", functionEffect);
	functionEffect = [_pOwner = this, _pWorldmatrix = pWorldmatrix]() {_pOwner->Create_Effect(TEXT("Char_Landing_Toe_Dust.Effect"), _pWorldmatrix, _float3(0.15f, 0.35f, 0.15f), false, true); };
	m_pModelBodyCom->Add_EffectFunc(5.f, "", "char_h_jump_downlowrun", functionEffect);
	//char_h_jump_fallsprint
	functionEffect = [_pOwner = this, _pWorldmatrix = pWorldmatrix]() {_pOwner->Create_Effect(TEXT("Char_Landing_Toe_Dust.Effect"), _pWorldmatrix, _float3(-0.15f, 0.35f, 0.15f), false, true); };
	m_pModelBodyCom->Add_EffectFunc(21.f, "", "char_h_jump_fallsprint", functionEffect);
	functionEffect = [_pOwner = this, _pWorldmatrix = pWorldmatrix]() {_pOwner->Create_Effect(TEXT("Char_Landing_Toe_Dust.Effect"), _pWorldmatrix, _float3(0.45f, 0.35f, 0.15f), false, true); };
	m_pModelBodyCom->Add_EffectFunc(11.f, "", "char_h_jump_fallsprint", functionEffect);
	//char_h_jump_forward_l
	functionEffect = [_pOwner = this]() {_pOwner->Create_Effect(TEXT("Char_Jump_Toe_Dust.Effect"), nullptr, _float3(0.f, 0.35f, -0.75f)); };
	m_pModelBodyCom->Add_EffectFunc(0.5f, "", "char_h_jump_forward_l", functionEffect);
	//char_h_jump_situ
	functionEffect = [_pOwner = this]() {_pOwner->Create_Effect(TEXT("Char_Jump_Toe_Dust.Effect"), nullptr, _float3(0.f, 0.35f, 0.f)); };
	m_pModelBodyCom->Add_EffectFunc(0.5f, "", "char_h_jump_situ", functionEffect);
	//char_h_jump_downcommon
	functionEffect = [_pOwner = this]() {_pOwner->Create_Effect(TEXT("Char_Landing_Dust.Effect"), nullptr, _float3(0.f, 0.2f, 0.f)); };
	m_pModelBodyCom->Add_EffectFunc(0.f, "", "char_h_jump_downcommon", functionEffect);
	//char_h_glid_roll
	functionEffect = [_pOwner = this]() {_pOwner->Create_Effect(TEXT("Char_Landing_Glider_Dust.Effect"), nullptr, _float3(0.f, 0.35f, 1.f)); };
	m_pModelBodyCom->Add_EffectFunc(18.f, "", "char_h_glid_roll", functionEffect);
	//char_h_glid_sprint_roll
	functionEffect = [_pOwner = this]() {_pOwner->Create_Effect(TEXT("Char_Landing_Glider_Dust.Effect"), nullptr, _float3(0.f, 0.35f, 1.f)); };
	m_pModelBodyCom->Add_EffectFunc(18.f, "", "char_h_glid_sprint_roll", functionEffect);
#pragma endregion
	// Sickle
#pragma region Sickle
	functionEffect = [_pOwner = this]() { _pOwner->Create_WeaponEffect_RightHand(TEXT("Sickle_Skill001_18_Client.Effect"), _float3(0.659f, 0, 0)); };
	m_pModelBodyCom->Add_EffectFunc(18, "", "char_sickle_skill_001", functionEffect);

	functionEffect = [_pOwner = this]() { _pOwner->Create_WeaponEffect_RightHand(TEXT("Sickle_Skill002_7_Client.Effect"), _float3(-0.5f, -0.5f, -0.5f)); };
	m_pModelBodyCom->Add_EffectFunc(7, "", "char_sickle_skill_002", functionEffect);

	functionEffect = [_pOwner = this]() { _pOwner->Create_WeaponEffect_RightHand(TEXT("Sickle_Skill003_8_Client.Effect"), _float3(-1.5f, -1.5f, -0.5f), false, _float3(0.f, 0.f, 2.f)); };
	m_pModelBodyCom->Add_EffectFunc(8, "", "char_sickle_skill_003", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_WeaponEffect_RightHand(TEXT("Sickle_Skill003_33_Client.Effect"), _float3(-0.5f, -0.5f, -0.5f), false, _float3(0.f, 0.f, 1.f)); };
	m_pModelBodyCom->Add_EffectFunc(33, "", "char_sickle_skill_003", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_WeaponEffect_RightHand(TEXT("Sickle_Skill004_38_2_Client.Effect"), _float3(-3.f, -0.5f, -0.5f)); };
	m_pModelBodyCom->Add_EffectFunc(38, "", "char_sickle_skill_004", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_WeaponEffect_RightHand(TEXT("Sickle_Skill005_8_Client.Effect"), _float3(2.f, -0.5f, -0.5f)); };
	m_pModelBodyCom->Add_EffectFunc(8, "", "char_sickle_skill_005", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_WeaponEffect_RightHand(TEXT("Sickle_Skill005_10_Client.Effect"), _float3(-2.f, -2.f, 0.f)); };
	m_pModelBodyCom->Add_EffectFunc(10, "", "char_sickle_skill_005", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_WeaponEffect_RightHand(TEXT("Sickle_Skill005_14_Client.Effect"), _float3(0.f, 0.f, 0.f)); };
	m_pModelBodyCom->Add_EffectFunc(14, "", "char_sickle_skill_005", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_WeaponEffect_RightHand(TEXT("Sickle_Skill004_38_2_Client.Effect"), _float3(0.f, 0.f, 0.f)); };
	m_pModelBodyCom->Add_EffectFunc(19.f, "", "char_sickle_skill_006", functionEffect);

	functionEffect = [_pOwner = this]() { _pOwner->Create_WeaponEffect_RightHand(TEXT("Sickle_Skill011a_7_Client.Effect"), _float3(-0.5f, -0.5f, -0.5f)); };
	m_pModelBodyCom->Add_EffectFunc(7, "", "char_sickle_skill_011a", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_WeaponEffect_RightHand(TEXT("Sickle_Skill011a_7_Client.Effect"), _float3(-0.5f, -1.5f, -0.5f)); };
	m_pModelBodyCom->Add_EffectFunc(25, "", "char_sickle_skill_011a", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_WeaponEffect_RightHand(TEXT("Sickle_Skill011b_11_Client.Effect"), _float3(-0.5f, -1.5f, 0.f)); };
	m_pModelBodyCom->Add_EffectFunc(11, "", "char_sickle_skill_011b", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_WeaponEffect_RightHand(TEXT("Sickle_Skill011b_36_Client.Effect"), _float3(-0.5f, -1.5f, 0.f)); };
	m_pModelBodyCom->Add_EffectFunc(36, "", "char_sickle_skill_011b", functionEffect);

	functionEffect = [_pOwner = this]() { _pOwner->Create_WeaponEffect_RightHand(TEXT("Sickle_Skill011c_27_BodyTrail_Client.Effect"), _float3(0.f, -1.f, 0.f)); };
	m_pModelBodyCom->Add_EffectFunc(27, "", "char_sickle_skill_011c", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_WeaponEffect_RightHand(TEXT("Sickle_Skill011c_44_BodyTrail_Client.Effect"), _float3(0.f, -1.f, 0.f)); };
	m_pModelBodyCom->Add_EffectFunc(43, "", "char_sickle_skill_011c", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_WeaponEffect_RightHand(TEXT("Sickle_Skill011c_53_BodyTrail_Client.Effect"), _float3(0.f, -1.f, 0.f)); };
	m_pModelBodyCom->Add_EffectFunc(53, "", "char_sickle_skill_011c", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_WeaponEffect_RightHand(TEXT("Sickle_Skill011c_76_BodyTrail_Client.Effect"), _float3(0.f, -1.f, 0.f)); };
	m_pModelBodyCom->Add_EffectFunc(76, "", "char_sickle_skill_011c", functionEffect);

	functionEffect = [_pOwner = this]() { _pOwner->Create_WeaponEffect_RightHand(TEXT("Sickle_Skill011c_10_Client.Effect"), _float3(-0.5f, -1.5f, 0.f)); };
	m_pModelBodyCom->Add_EffectFunc(10, "", "char_sickle_skill_011c", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_WeaponEffect_RightHand(TEXT("Sickle_Skill011c_24_Client.Effect"), _float3(-0.5f, -1.5f, 0.f)); };
	m_pModelBodyCom->Add_EffectFunc(24, "", "char_sickle_skill_011c", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_WeaponEffect_RightHand(TEXT("Sickle_Skill011c_36_Client.Effect"), _float3(-0.5f, -1.5f, 0.f)); };
	m_pModelBodyCom->Add_EffectFunc(36, "", "char_sickle_skill_011c", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_WeaponEffect_RightHand(TEXT("Sickle_Skill011c_49_Client.Effect"), _float3(-0.5f, -1.5f, 0.f)); };
	m_pModelBodyCom->Add_EffectFunc(49, "", "char_sickle_skill_011c", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_WeaponEffect_RightHand(TEXT("Sickle_Skill011c_66_Client.Effect"), _float3(-0.5f, -1.5f, 0.f)); };
	m_pModelBodyCom->Add_EffectFunc(66, "", "char_sickle_skill_011c", functionEffect);

	//functionEffect = [_pOwner = this]() { _pOwner->Create_WeaponEffect_RightHand(TEXT("Sickle_Skill011d_10_Client.Effect"), _float3(0.f,0.f,0.f), true); };
	//m_pModelBodyCom->Add_EffectFunc(10, "", "char_sickle_skill_011d", functionEffect);
	//functionEffect = [_pOwner = this]() { _pOwner->Create_WeaponEffect_RightHand(TEXT("Sickle_Skill011c_10_Particle_Client.Effect"), _float3(0.f, 0.f, 0.f)); };
	//m_pModelBodyCom->Add_EffectFunc(10, "", "char_sickle_skill_011d", functionEffect);
	functionEffect = [_pOwner = this, _pWorldmatrix = pWorldmatrix]() { _pOwner->Create_Effect(TEXT("Sickle_Skill011d_10_World_Client.Effect"), _pWorldmatrix, _float3(0.f, 3.5f, 0.f), true); };
	m_pModelBodyCom->Add_EffectFunc(10, "", "char_sickle_skill_011d", functionEffect);
	functionEffect = [_pOwner = this, _pWorldmatrix = pWorldmatrix]() { _pOwner->Create_Effect(TEXT("Sickle_Skill011d_10_Particle_World_Client.Effect"), _pWorldmatrix, _float3(0.f, 3.5f, 0.f), false, true); };
	m_pModelBodyCom->Add_EffectFunc(10, "", "char_sickle_skill_011d", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_WeaponEffect_RightHand(TEXT("Sickle_Skill011d_86_Client.Effect"), _float3(-0.5f, -1.5f, 0.f)); };
	m_pModelBodyCom->Add_EffectFunc(86, "", "char_sickle_skill_011d", functionEffect);

	functionEffect = [_pOwner = this]() { _pOwner->Create_WeaponEffect_RightHand(TEXT("Sickle_Skill011e_1_14_Client.Effect"), _float3(0.0f, 0.f, 0.f), true); };
	m_pModelBodyCom->Add_EffectFunc(14, "", "char_sickle_skill_011e_1", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_WeaponEffect_RightHand(TEXT("Sickle_Skill011e_1_Particle_Client.Effect"), _float3(0.f, -1.f, 0.f)); };
	m_pModelBodyCom->Add_EffectFunc(15, "", "char_sickle_skill_011e_1", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_WeaponEffect_RightHand(TEXT("Sickle_Skill011e_1_Particle_Client.Effect"), _float3(0.f, -1.f, 0.f)); };
	m_pModelBodyCom->Add_EffectFunc(20, "", "char_sickle_skill_011e_1", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_WeaponEffect_RightHand(TEXT("Sickle_Skill011e_1_Particle_Client.Effect"), _float3(0.f, -1.f, 0.f)); };
	m_pModelBodyCom->Add_EffectFunc(25, "", "char_sickle_skill_011e_1", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_WeaponEffect_RightHand(TEXT("Sickle_Skill011e_1_16_Client.Effect"), _float3(0.5f, -1.75f, 0.f), true); };
	m_pModelBodyCom->Add_EffectFunc(16, "", "char_sickle_skill_011e_1", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_WeaponEffect_RightHand(TEXT("Sickle_Skill011e_1_20_Client.Effect"), _float3(0.0f, 0.f, 0.f), true); };
	m_pModelBodyCom->Add_EffectFunc(20, "", "char_sickle_skill_011e_1", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_WeaponEffect_RightHand(TEXT("Sickle_Skill011e_1_28_Client.Effect"), _float3(0.f, -1.f, 0.f), true); };
	m_pModelBodyCom->Add_EffectFunc(28, "", "char_sickle_skill_011e_1", functionEffect);

	functionEffect = [_pOwner = this, _pWorldmatrix = pWorldmatrix]() { _pOwner->Create_Effect(TEXT("Sickle_Skill011e_2_0_Client.Effect"), _pWorldmatrix, _float3(0.0f, 0.f, 0.f), true); };
	m_pModelBodyCom->Add_EffectFunc(0, "", "char_sickle_skill_011e_2", functionEffect);
	functionEffect = [_pOwner = this, _pWorldmatrix = pWorldmatrix]() { _pOwner->Create_Effect(TEXT("Sickle_Skill011e_2_0_Client.Effect"), _pWorldmatrix, _float3(0.0f, 0.f, 0.f), true); };
	m_pModelBodyCom->Add_EffectFunc(20, "", "char_sickle_skill_011e_2", functionEffect);
	functionEffect = [_pOwner = this, _pWorldmatrix = pWorldmatrix]() { _pOwner->Create_Effect(TEXT("Sickle_Skill011e_2_0_Client.Effect"), _pWorldmatrix, _float3(0.0f, 0.f, 0.f), true); };
	m_pModelBodyCom->Add_EffectFunc(40, "", "char_sickle_skill_011e_2", functionEffect);

	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Sickle_Skill011e_3_3_Client.Effect"), nullptr, _float3(0.0f, 0.f, 1.f)); };
	m_pModelBodyCom->Add_EffectFunc(3, "", "char_sickle_skill_011e_3", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Sickle_Skill011e_3_5_Client.Effect"), nullptr, _float3(-1.f, 0.f, 4.f)); };
	m_pModelBodyCom->Add_EffectFunc(5, "", "char_sickle_skill_011e_3", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Sickle_Skill011e_3_5_Particle_Client.Effect"), nullptr, _float3(-1.f, 0.f, 4.f)); };
	m_pModelBodyCom->Add_EffectFunc(5, "", "char_sickle_skill_011e_3", functionEffect);
	//char_sickle_skill_015_New
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Sickle_Skill015_10_Client.Effect"), nullptr, _float3(0.f, 2.5f, 0.f)); };
	m_pModelBodyCom->Add_EffectFunc(10.f, "", "char_sickle_skill_015_New", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_WeaponEffect_RightHand(TEXT("Sickle_Skill015_40_Client.Effect"), _float3(0.0f, 0.f, 0.f), false, { 0.f,0.f,0.f }, false, true); };
	m_pModelBodyCom->Add_EffectFunc(20.f, "", "char_sickle_skill_015_New", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_WeaponEffect_RightHand(TEXT("Sickle_Skill015_37_Client.Effect"), _float3(0.0f, 0.f, 0.f)); };
	m_pModelBodyCom->Add_EffectFunc(32.f, "", "char_sickle_skill_015_New", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Sickle_Skill015_83_Client.Effect"), nullptr, _float3(0.f, 0.5f, 3.f)); };
	m_pModelBodyCom->Add_EffectFunc(83.f, "", "char_sickle_skill_015_New", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Sickle_Skill015_83_Line_Client.Effect"), nullptr, _float3(0.0f, 7.f, 2.5f)); };
	m_pModelBodyCom->Add_EffectFunc(83.f, "", "char_sickle_skill_015_New", functionEffect);
#pragma endregion
	//2Gun
#pragma region 2Gun
	functionEffect = [_pOwner = this]() { _pOwner->Create_WeaponEffect_RightHand(TEXT("2Gun_Aim_5_World_Client.Effect"), _float3(2.f, 0.f, 0.f)); };
	m_pModelBodyCom->Add_EffectFunc(12.f, "", "char_2gun_aim_begin", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_WeaponEffect_RightHand(TEXT("2Gun_Aim_5_World_Client.Effect"), _float3(2.f, 0.f, 0.25f)); };
	m_pModelBodyCom->Add_EffectFunc(8.25f, "", "char_2gun_aim_attack", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_WeaponEffect_LeftHand(TEXT("2Gun_Aim_5_World_Client.Effect"), _float3(2.f, 0.f, -0.25f)); };
	m_pModelBodyCom->Add_EffectFunc(1.25f, "", "char_2gun_aim_attack", functionEffect);

	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("2Gun_Skill001_8_World_Client.Effect"), nullptr, _float3(-0.4f, 3.25f, 2.5f)); };
	m_pModelBodyCom->Add_EffectFunc(8.f, "", "char_2gun_skill_001", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("2Gun_Skill001_8_SmallElectric_World_Client.Effect"), nullptr, _float3(-0.4f, 3.25f, 3.5f)); };
	m_pModelBodyCom->Add_EffectFunc(8.f, "", "char_2gun_skill_001", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("2Gun_Skill001_8_World_Client.Effect"), nullptr, _float3(0.15f, 3.1f, 2.7f)); };
	m_pModelBodyCom->Add_EffectFunc(18.f, "", "char_2gun_skill_001", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("2Gun_Skill001_8_SmallElectric_World_Client.Effect"), nullptr, _float3(0.15f, 3.1f, 3.7f)); };
	m_pModelBodyCom->Add_EffectFunc(18.f, "", "char_2gun_skill_001", functionEffect);
	//002
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("2Gun_Skill002_16_World3_Angle330_Client.Effect"), nullptr, _float3(-1.25f, 2.f, 2.5f)); };
	m_pModelBodyCom->Add_EffectFunc(15.f, "", "char_2gun_skill_002", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("2Gun_Skill001_8_World_Client.Effect"), nullptr, _float3(0.f, 2.f, 3.5f)); };
	m_pModelBodyCom->Add_EffectFunc(17.f, "", "char_2gun_skill_002", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("2Gun_Skill001_8_SmallElectric_World_Client.Effect"), nullptr, _float3(1.5f, 2.f, 5.5f)); };
	m_pModelBodyCom->Add_EffectFunc(17.f, "", "char_2gun_skill_002", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("2Gun_Skill001_8_SmallElectric_World_Client.Effect"), nullptr, _float3(-1.5f, 2.f, 5.5f)); };
	m_pModelBodyCom->Add_EffectFunc(17.f, "", "char_2gun_skill_002", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("2Gun_Skill002_16_World3_Angle30_Client.Effect"), nullptr, _float3(1.25f, 2.f, 2.5f)); };
	m_pModelBodyCom->Add_EffectFunc(17.f, "", "char_2gun_skill_002", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("2Gun_Skill002_16_Particle_World_Client.Effect"), nullptr, _float3(0.f, 2.f, 3.5f)); };
	m_pModelBodyCom->Add_EffectFunc(17.f, "", "char_2gun_skill_002", functionEffect);

	//003
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("2Gun_Skill003_SmallElectric_World_Client.Effect"), nullptr, _float3(0.3f, 2.5f, 3.25f)); };
	m_pModelBodyCom->Add_EffectFunc(4.f, "", "char_2gun_skill_003", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("2Gun_Skill003_4_World_Client.Effect"), nullptr, _float3(0.3f, 2.5f, 3.25f)); };
	m_pModelBodyCom->Add_EffectFunc(4.f, "", "char_2gun_skill_003", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("2Gun_Skill003_4_Electric1_World_Client.Effect"), nullptr, _float3(0.6f, 1.5f, 3.f)); };
	m_pModelBodyCom->Add_EffectFunc(14.f, "", "char_2gun_skill_003", functionEffect);

	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("2Gun_Skill001_8_World_Client.Effect"), nullptr, _float3(-0.5f, 2.6f, 3.5f)); };
	m_pModelBodyCom->Add_EffectFunc(20.f, "", "char_2gun_skill_003", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("2Gun_Skill003_4_Electric1_World_Client.Effect"), nullptr, _float3(-1.4f, 3.25f, 3.25f)); };
	m_pModelBodyCom->Add_EffectFunc(24.f, "", "char_2gun_skill_003", functionEffect);

	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("2Gun_Skill001_8_World_Client.Effect"), nullptr, _float3(0.3f, 2.6f, 3.25f)); };
	m_pModelBodyCom->Add_EffectFunc(30.f, "", "char_2gun_skill_003", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("2Gun_Skill003_4_Electric1_World_Client.Effect"), nullptr, _float3(0.7f, 3.2f, 2.75f)); };
	m_pModelBodyCom->Add_EffectFunc(34.f, "", "char_2gun_skill_003", functionEffect);

	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("2Gun_Skill003_4_World_Client.Effect"), nullptr, _float3(0.f, 2.6f, 3.25f)); };
	m_pModelBodyCom->Add_EffectFunc(38.f, "", "char_2gun_skill_003", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("2Gun_Skill003_4_Electric1_World_Client.Effect"), nullptr, _float3(-0.6f, 1.5f, 2.7f)); };
	m_pModelBodyCom->Add_EffectFunc(42.f, "", "char_2gun_skill_003", functionEffect);

	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("2Gun_Skill003_4_World_Client.Effect"), nullptr, _float3(0.3f, 2.5f, 3.25f)); };
	m_pModelBodyCom->Add_EffectFunc(47.f, "", "char_2gun_skill_003", functionEffect);

	//004
#pragma region 2Gun_Skill_004
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("2Gun_Skill004_Cube_World_Client.Effect"), nullptr, _float3(0.f, 1.5f, 0.f)); };
	m_pModelBodyCom->Add_EffectFunc(9.f, "", "char_2gun_skill_004", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("2Gun_Skill004_SmallElectric_World_Client.Effect"), nullptr, _float3(0.f, 1.5f, 0.f)); };
	m_pModelBodyCom->Add_EffectFunc(12.f, "", "char_2gun_skill_004", functionEffect);
	for (size_t i = 0; i < 4; i++)
	{
		_float _fChangeRotate = 0.f;
		_float fAnimFlame = 1.f;

		if (0 == i)
		{
			_fChangeRotate = 0.f;
			fAnimFlame = 1.f;
		}
		else if (1 == i)
		{
			_fChangeRotate = 20.f;
			fAnimFlame = 9.f;
		}
		else if (2 == i)
		{
			_fChangeRotate = 50.f;
			fAnimFlame = 17.f;
		}
		else
		{
			_fChangeRotate = 0.f;
			fAnimFlame = 25.f;
		}
		functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("2Gun_Skill004_World_Angle0_Client.Effect"), 0.f, 330.f - _fChangeRotate, 0.f, 345.f, 3.f, 1.5f); };
		m_pModelBodyCom->Add_EffectFunc(fAnimFlame, "", "char_2gun_skill_004", functionEffect);
		functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("2Gun_Skill004_World_Angle0_Client.Effect"), 0.f, 330.f - _fChangeRotate - 180.f, 0.f, 345.f - 180.f, 3.f, 1.5f); };
		m_pModelBodyCom->Add_EffectFunc(fAnimFlame, "", "char_2gun_skill_004", functionEffect);

		fAnimFlame += 1.f;
		functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("2Gun_Skill004_World_Angle0_Client.Effect"), 0.f, 315.f - _fChangeRotate, 0.f, 315.f, 3.f, 1.75f); };
		m_pModelBodyCom->Add_EffectFunc(fAnimFlame, "", "char_2gun_skill_004", functionEffect);
		functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("2Gun_Skill004_Electric_World_Client.Effect"), 0.f, 315.f - _fChangeRotate, 0.f, 315.f, 2.f, 1.75f); };
		m_pModelBodyCom->Add_EffectFunc(fAnimFlame, "", "char_2gun_skill_004", functionEffect);
		functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("2Gun_Skill004_World_Angle0_Client.Effect"), 0.f, 315.f - _fChangeRotate - 180.f, 0.f, 315.f - 180.f, 3.f, 1.75f); };
		m_pModelBodyCom->Add_EffectFunc(fAnimFlame, "", "char_2gun_skill_004", functionEffect);
		functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("2Gun_Skill004_Electric_World_Client.Effect"), 0.f, 315.f - _fChangeRotate - 180.f, 0.f, 315.f - 180.f, 2.f, 1.75f); };
		m_pModelBodyCom->Add_EffectFunc(fAnimFlame, "", "char_2gun_skill_004", functionEffect);

		fAnimFlame += 2.f;
		functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("2Gun_Skill004_World_Angle0_Client.Effect"), 0.f, 225.f - _fChangeRotate, 0.f, 285.f, 3.f, 1.65f); };
		m_pModelBodyCom->Add_EffectFunc(fAnimFlame, "", "char_2gun_skill_004", functionEffect);
		functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("2Gun_Skill004_World_Angle0_Client.Effect"), 0.f, 190.f - _fChangeRotate, 0.f, 195.f, 2.5f, 0.5f); };
		m_pModelBodyCom->Add_EffectFunc(fAnimFlame, "", "char_2gun_skill_004", functionEffect);
		functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("2Gun_Skill004_World_Angle0_Client.Effect"), 0.f, 225.f - _fChangeRotate - 180.f, 0.f, 285.f - 180.f, 3.f, 1.65f); };
		m_pModelBodyCom->Add_EffectFunc(fAnimFlame, "", "char_2gun_skill_004", functionEffect);
		functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("2Gun_Skill004_World_Angle0_Client.Effect"), 0.f, 190.f - _fChangeRotate - 180.f, 0.f, 195.f - 180.f, 2.5f, 0.5f); };
		m_pModelBodyCom->Add_EffectFunc(fAnimFlame, "", "char_2gun_skill_004", functionEffect);

		fAnimFlame += 5.f;
		functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("2Gun_Skill004_World_Angle0_Client.Effect"), 0.f, 215.f - _fChangeRotate, 0.f, 215.f, 3.f, 1.25f); };
		m_pModelBodyCom->Add_EffectFunc(fAnimFlame, "", "char_2gun_skill_004", functionEffect);
		functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("2Gun_Skill004_World_Angle0_Client.Effect"), 0.f, 215.f - _fChangeRotate - 180.f, 0.f, 215.f - 180.f, 3.f, 1.25f); };
		m_pModelBodyCom->Add_EffectFunc(fAnimFlame, "", "char_2gun_skill_004", functionEffect);
	}

	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("2Gun_Skill002_16_Particle_World_Client.Effect"), nullptr, _float3(0.f, 2.f, 3.5f)); };
	m_pModelBodyCom->Add_EffectFunc(47.f, "", "char_2gun_skill_004", functionEffect);
#pragma endregion
	//005
	//functionEffect = [_pOwner = this, _pWorldmatrix = pWorldmatrix]() { _pOwner->Create_Effect(TEXT("2Gun_Skill005_9_World_Client.Effect"), _pWorldmatrix, _float3(0.f, 2.f, 3.5f), false, true); };
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("2Gun_Skill005_9_World_Client.Effect"), nullptr, _float3(0.f, 2.f, 3.5f)); };
	m_pModelBodyCom->Add_EffectFunc(9.f, "", "char_2gun_skill_005", functionEffect);

	functionEffect = [_pOwner = this]() { _pOwner->Create_WeaponEffect_RightHand(TEXT("2Gun_Skill005_21_Client.Effect"), _float3(2.f, 0.f, 0.f)); };
	m_pModelBodyCom->Add_EffectFunc(21, "", "char_2gun_skill_005", functionEffect);

	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("2Gun_Skill005_SmallElectric_World_Client.Effect"), nullptr, _float3(-0.5f, 2.5f, 2.f)); };
	m_pModelBodyCom->Add_EffectFunc(49.f, "", "char_2gun_skill_005", functionEffect);

	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("2Gun_Skill005_51_World_Client.Effect"), nullptr, _float3(0.f, 2.5f, 2.f)); };
	m_pModelBodyCom->Add_EffectFunc(51.f, "", "char_2gun_skill_005", functionEffect);

	//007
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("2Gun_Skill007b_0_Client.Effect"), nullptr, _float3(0.f, 0.f, 2.f)); };
	m_pModelBodyCom->Add_EffectFunc(0.f, "", "char_2gun_skill_007b", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("2Gun_Skill007b_0_Client.Effect"), nullptr, _float3(0.f, 0.f, 2.f)); };
	m_pModelBodyCom->Add_EffectFunc(9.f, "", "char_2gun_skill_007b", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("2Gun_Skill007b_0_Client.Effect"), nullptr, _float3(0.f, 0.f, 2.f)); };
	m_pModelBodyCom->Add_EffectFunc(13.f, "", "char_2gun_skill_007b", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("2Gun_Skill007b_0_Client.Effect"), nullptr, _float3(0.f, 0.f, 2.f)); };
	m_pModelBodyCom->Add_EffectFunc(25.f, "", "char_2gun_skill_007b", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("2Gun_Skill007b_0_Client.Effect"), nullptr, _float3(0.f, 0.f, 2.f)); };
	m_pModelBodyCom->Add_EffectFunc(29.f, "", "char_2gun_skill_007b", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("2Gun_Skill007b_0_Client.Effect"), nullptr, _float3(0.f, 0.f, 2.f)); };
	m_pModelBodyCom->Add_EffectFunc(35.f, "", "char_2gun_skill_007b", functionEffect);

	// functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("2Gun_Skill007b_20_Client2.Effect"), nullptr, _float3(0.f, -12.5f, 2.f)); };
	// m_pModelBodyCom->Add_EffectFunc(19.f, "", "char_2gun_skill_007b", functionEffect);

	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("2Gun_Skill007b_20_Client2.Effect"), nullptr, _float3(0.f, -12.5f, 2.f)); };
	m_pModelBodyCom->Add_EffectFunc(0.5f, "", "char_2gun_skill_007c", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("2Gun_Skill007c_8_Client.Effect"), nullptr, _float3(0.f, 0.f, 2.5f)); };
	m_pModelBodyCom->Add_EffectFunc(8.f, "", "char_2gun_skill_007c", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("2Gun_Skill007c_9_Client.Effect"), nullptr, _float3(0.f, 0.f, 2.5f)); };
	m_pModelBodyCom->Add_EffectFunc(9.f, "", "char_2gun_skill_007c", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("2Gun_Skill007c_10_Client.Effect"), nullptr, _float3(0.f, 0.5f, 2.5f)); };
	m_pModelBodyCom->Add_EffectFunc(10.f, "", "char_2gun_skill_007c", functionEffect);

	//011
	functionEffect = [_pOwner = this, _pWorldmatrix = pWorldmatrix]() { _pOwner->Create_Effect(TEXT("2Gun_Skill011_0_World_Client.Effect"), _pWorldmatrix, _float3(0.f, 2.f, 0.f), true); };
	m_pModelBodyCom->Add_EffectFunc(0.f, "", "char_2gun_skill_011", functionEffect);
	functionEffect = [_pOwner = this, _pWorldmatrix = pWorldmatrix]() { _pOwner->Create_Effect(TEXT("2Gun_Skill011_0_SmallElectric_World_Client.Effect"), _pWorldmatrix, _float3(0.f, 2.f, 0.f), false, true); };
	m_pModelBodyCom->Add_EffectFunc(0.f, "", "char_2gun_skill_011", functionEffect);
	functionEffect = [_pOwner = this, _pWorldmatrix = pWorldmatrix]() { _pOwner->Create_Effect(TEXT("2Gun_Skill011_0_Cube_World_Client.Effect"), _pWorldmatrix, _float3(0.f, 2.f, 0.f), false, true); };
	m_pModelBodyCom->Add_EffectFunc(0.f, "", "char_2gun_skill_011", functionEffect);

	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("2Gun_Skill011_22_World_Client.Effect"), nullptr, _float3(0.f, 6.f, 0.f)); };
	m_pModelBodyCom->Add_EffectFunc(22.f, "", "char_2gun_skill_011", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("2Gun_Skill011_25_World_Client.Effect"), nullptr, _float3(0.f, 4.f, 2.f)); };
	m_pModelBodyCom->Add_EffectFunc(23.f, "", "char_2gun_skill_011", functionEffect);
	functionEffect = [_pOwner = this, _pWorldmatrix = pWorldmatrix]() { _pOwner->Create_Effect(TEXT("2Gun_Skill011_22_Cube_World_Client.Effect"), _pWorldmatrix, _float3(0.f, 2.f, 2.f), false, true); };
	m_pModelBodyCom->Add_EffectFunc(22.f, "", "char_2gun_skill_011", functionEffect);

	//009
#pragma region 2Gun_Skill_009b
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("2Gun_Skill009_Cube_World_Client.Effect"), nullptr, _float3(0.f, -1.f, 0.f)); };
	m_pModelBodyCom->Add_EffectFunc(1.f, "", "char_2gun_skill_009b", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("2Gun_Skill004_SmallElectric_World_Client.Effect"), nullptr, _float3(0.f, -1.5f, 0.f)); };
	m_pModelBodyCom->Add_EffectFunc(3.f, "", "char_2gun_skill_009b", functionEffect);
	for (size_t i = 0; i < 4; i++)
	{
		_float _fChangeRotate = 0.f;
		_float fAnimFrame = 1.f;

		if (0 == i)
		{
			_fChangeRotate = 0.f;
			fAnimFrame = 1.f;
		}
		else if (1 == i)
		{
			_fChangeRotate = 20.f;
			fAnimFrame = 9.f;
		}
		else if (2 == i)
		{
			_fChangeRotate = 50.f;
			fAnimFrame = 17.f;
		}
		else
		{
			_fChangeRotate = 0.f;
			fAnimFrame = 25.f;
		}
		functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("2Gun_Skill004_World_Angle0_Client.Effect"), 20.f, 330.f - _fChangeRotate, 0.f, 345.f, 2.f, -0.5f); };
		m_pModelBodyCom->Add_EffectFunc(fAnimFrame, "", "char_2gun_skill_009b", functionEffect);
		//180.f 
		functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("2Gun_Skill004_World_Angle0_Client.Effect"), 20.f, 330.f - _fChangeRotate - 180.f, 0.f, 345.f - 180.f, 2.f, -0.5f); };
		m_pModelBodyCom->Add_EffectFunc(fAnimFrame, "", "char_2gun_skill_009b", functionEffect);

		fAnimFrame += 1.f;
		functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("2Gun_Skill004_World_Angle0_Client.Effect"), 25.f, 315.f - _fChangeRotate, 0.f, 315.f, 2.5f, -0.75f); };
		m_pModelBodyCom->Add_EffectFunc(fAnimFrame, "", "char_2gun_skill_009b", functionEffect);
		functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("2Gun_Skill004_Electric_World_Client.Effect"), 25.f, 315.f - _fChangeRotate, 0.f, 315.f, 2.f, -0.75f); };
		m_pModelBodyCom->Add_EffectFunc(fAnimFrame, "", "char_2gun_skill_009b", functionEffect);
		functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("2Gun_Skill004_World_Angle0_Client.Effect"), 25.f, 315.f - _fChangeRotate - 180.f, 0.f, 315.f - 180.f, 2.5f, -0.75f); };
		m_pModelBodyCom->Add_EffectFunc(fAnimFrame, "", "char_2gun_skill_009b", functionEffect);
		functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("2Gun_Skill004_Electric_World_Client.Effect"), 25.f, 315.f - _fChangeRotate - 180.f, 0.f, 315.f - 180.f, 2.f, -0.75f); };
		m_pModelBodyCom->Add_EffectFunc(fAnimFrame, "", "char_2gun_skill_009b", functionEffect);

		fAnimFrame += 2.f;
		functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("2Gun_Skill004_World_Angle0_Client.Effect"), 15.f, 225.f - _fChangeRotate, 0.f, 285.f, 3.f, -0.5f); };
		m_pModelBodyCom->Add_EffectFunc(fAnimFrame, "", "char_2gun_skill_009b", functionEffect);
		functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("2Gun_Skill004_World_Angle0_Client.Effect"), 15.f, 190.f - _fChangeRotate, 0.f, 195.f, 2.5f, 0.5f); };
		m_pModelBodyCom->Add_EffectFunc(fAnimFrame, "", "char_2gun_skill_009b", functionEffect);
		functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("2Gun_Skill004_World_Angle0_Client.Effect"), 15.f, 225.f - _fChangeRotate - 180.f, 0.f, 285.f - 180.f, 3.f, -0.5f); };
		m_pModelBodyCom->Add_EffectFunc(fAnimFrame, "", "char_2gun_skill_009b", functionEffect);
		functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("2Gun_Skill004_World_Angle0_Client.Effect"), 15.f, 190.f - _fChangeRotate - 180.f, 0.f, 195.f - 180.f, 2.5f, 0.5f); };
		m_pModelBodyCom->Add_EffectFunc(fAnimFrame, "", "char_2gun_skill_009b", functionEffect);

		fAnimFrame += 5.f;
		functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("2Gun_Skill004_World_Angle0_Client.Effect"), 20.f, 215.f - _fChangeRotate, 0.f, 215.f, 3.f, -0.25f); };
		m_pModelBodyCom->Add_EffectFunc(fAnimFrame, "", "char_2gun_skill_009b", functionEffect);
		functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("2Gun_Skill004_World_Angle0_Client.Effect"), 20.f, 215.f - _fChangeRotate - 180.f, 0.f, 215.f - 180.f, 3.f, -0.25f); };
		m_pModelBodyCom->Add_EffectFunc(fAnimFrame, "", "char_2gun_skill_009b", functionEffect);

	}
#pragma endregion
#pragma region 2Gun_Skill_009c
	_float _fChangeRotate = 0.f;
	_float fAnimFlame = 0.f;
	functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("2Gun_Skill004_World_Angle0_Client.Effect"), 35.f, 330.f - _fChangeRotate, 0.f, 345.f, 2.f, 0.5f); };
	m_pModelBodyCom->Add_EffectFunc(fAnimFlame, "", "char_2gun_skill_009c", functionEffect);
	functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("2Gun_Skill004_World_Angle0_Client.Effect"), 35.f, 330.f - _fChangeRotate - 180.f, 0.f, 345.f - 180.f, 2.f, 0.5f); };
	m_pModelBodyCom->Add_EffectFunc(fAnimFlame, "", "char_2gun_skill_009c", functionEffect);

	fAnimFlame = 5.f;
	functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("2Gun_Skill004_World_Angle0_Client.Effect"), 45.f, 315.f - _fChangeRotate, 0.f, 315.f, 2.5f, -0.0f); };
	m_pModelBodyCom->Add_EffectFunc(fAnimFlame, "", "char_2gun_skill_009c", functionEffect);
	functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("2Gun_Skill004_Electric_World_Client.Effect"), 45.f, 315.f - _fChangeRotate, 0.f, 315.f, 1.5f, 0.0f); };
	m_pModelBodyCom->Add_EffectFunc(fAnimFlame, "", "char_2gun_skill_009c", functionEffect);
	fAnimFlame = 9.f;
	functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("2Gun_Skill004_World_Angle0_Client.Effect"), 55.f, 315.f - _fChangeRotate - 180.f, 0.f, 315.f - 180.f, 2.5f, -0.5f); };
	m_pModelBodyCom->Add_EffectFunc(fAnimFlame, "", "char_2gun_skill_009c", functionEffect);
	functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("2Gun_Skill004_Electric_World_Client.Effect"), 55.f, 315.f - _fChangeRotate - 180.f, 0.f, 315.f - 180.f, 1.5f, -0.f); };
	m_pModelBodyCom->Add_EffectFunc(fAnimFlame, "", "char_2gun_skill_009c", functionEffect);

	fAnimFlame = 11.f;
	functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("2Gun_Skill004_World_Angle0_Client.Effect"), 66.f, 225.f - _fChangeRotate - 180.f, 0.f, 285.f - 180.f, 2.5f, -0.75f); };
	m_pModelBodyCom->Add_EffectFunc(fAnimFlame, "", "char_2gun_skill_009c", functionEffect);
	functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("2Gun_Skill004_World_Angle0_Client.Effect"), 66.f, 190.f - _fChangeRotate - 180.f, 0.f, 195.f - 180.f, 2.5f, -0.f); };
	m_pModelBodyCom->Add_EffectFunc(fAnimFlame, "", "char_2gun_skill_009c", functionEffect);

	fAnimFlame = 13.f;
	functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("2Gun_Skill004_World_Angle0_Client.Effect"), 75.f, 215.f - _fChangeRotate, 0.f, 215.f, 2.5f, -1.f); };
	m_pModelBodyCom->Add_EffectFunc(fAnimFlame, "", "char_2gun_skill_009c", functionEffect);
	functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("2Gun_Skill004_World_Angle0_Client.Effect"), 75.f, 215.f - _fChangeRotate - 180.f, 0.f, 215.f - 180.f, 2.5f, -1.f); };
	m_pModelBodyCom->Add_EffectFunc(fAnimFlame, "", "char_2gun_skill_009c", functionEffect);
	fAnimFlame = 15.f;
	functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("2Gun_Skill004_World_Angle0_Client.Effect"), 85.f, 215.f - _fChangeRotate, 0.f, 215.f, 0.5f, -1.25f); };
	m_pModelBodyCom->Add_EffectFunc(fAnimFlame, "", "char_2gun_skill_009c", functionEffect);
	functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("2Gun_Skill004_World_Angle0_Client.Effect"), 85.f, 215.f - _fChangeRotate - 180.f, 0.f, 215.f - 180.f, 0.5f, -1.25f); };
	m_pModelBodyCom->Add_EffectFunc(fAnimFlame, "", "char_2gun_skill_009c", functionEffect);
#pragma endregion
	//char_2gun_skill_010
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("2Gun_Skill010_1_Client.Effect"), nullptr, _float3(0.f, 1.f, 0.f)); };
	m_pModelBodyCom->Add_EffectFunc(1.f, "", "char_2gun_skill_010", functionEffect);

	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("2Gun_Skill010_20_Client.Effect"), nullptr, _float3(0.f, 2.f, 2.5f)); };
	m_pModelBodyCom->Add_EffectFunc(20.f, "", "char_2gun_skill_010", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("2Gun_Skill010_29_Client.Effect"), nullptr, _float3(0.f, 2.f, 2.5f)); };
	m_pModelBodyCom->Add_EffectFunc(29.f, "", "char_2gun_skill_010", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("2Gun_Skill005_SmallElectric_World_Client.Effect"), nullptr, _float3(0.f, 2.f, 7.f)); };
	m_pModelBodyCom->Add_EffectFunc(42.f, "", "char_2gun_skill_010", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("2Gun_Skill005_51_World_Client.Effect"), nullptr, _float3(0.f, 2.f, 3.5f)); };
	m_pModelBodyCom->Add_EffectFunc(42.f, "", "char_2gun_skill_010", functionEffect);

#pragma endregion
	//Gun
#pragma region Gun
	//char_gun_skill_melee1
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Gun_Skill_Melee1_12_Client.Effect"), nullptr, _float3(0.25f, 2.75f, 2.5f)); };
	m_pModelBodyCom->Add_EffectFunc(12.f, "", "char_gun_skill_melee1", functionEffect);
	//char_gun_skill_melee2
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Gun_Skill_Melee1_12_Client.Effect"), nullptr, _float3(0.225f, 2.4f, 2.5f)); };
	m_pModelBodyCom->Add_EffectFunc(13.f, "", "char_gun_skill_melee2", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Gun_Skill_Melee1_12_Client.Effect"), nullptr, _float3(0.225f, 2.6f, 2.75f)); };
	m_pModelBodyCom->Add_EffectFunc(22.f, "", "char_gun_skill_melee2", functionEffect);
	//char_gun_skill_melee3
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Gun_Skill_Melee3_35_Client.Effect"), nullptr, _float3(0.25f, 2.2f, 2.75f)); };
	m_pModelBodyCom->Add_EffectFunc(35.f, "", "char_gun_skill_melee3", functionEffect);
	//char_gun_skill_melee4
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Gun_Skill_Melee1_12_Client.Effect"), nullptr, _float3(0.25f, 2.4f, 3.f)); };
	m_pModelBodyCom->Add_EffectFunc(24.f, "", "char_gun_skill_melee4", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Gun_Skill_Melee1_12_Client.Effect"), nullptr, _float3(0.25f, 2.7f, 3.f)); };
	m_pModelBodyCom->Add_EffectFunc(40.f, "", "char_gun_skill_melee4", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Gun_Skill_Melee1_12_Client.Effect"), nullptr, _float3(0.25f, 2.95f, 3.f)); };
	m_pModelBodyCom->Add_EffectFunc(58.f, "", "char_gun_skill_melee4", functionEffect);
	//char_gun_skill_melee5
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Gun_Skill_Melee3_35_Client.Effect"), nullptr, _float3(0.25f, 2.7f, 2.5f)); };
	m_pModelBodyCom->Add_EffectFunc(22.f, "", "char_gun_skill_melee5", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Gun_Skill_Melee3_35_Circle_Client.Effect"), nullptr, _float3(0.25f, 2.2f, 4.5f)); };
	m_pModelBodyCom->Add_EffectFunc(28.f, "", "char_gun_skill_melee5", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Gun_Skill_Melee3_35_Circle_Client.Effect"), nullptr, _float3(0.25f, 3.2f, 5.f)); };
	m_pModelBodyCom->Add_EffectFunc(28.f, "", "char_gun_skill_melee5", functionEffect);
	//char_gun_skill_005
	functionEffect = [_pOwner = this]() { _pOwner->Create_WeaponEffect_RightHand(TEXT("Gun_Skill_5_6_Client.Effect"), _float3(3.f, 0.25f, 0.f)); };
	m_pModelBodyCom->Add_EffectFunc(6.f, "", "char_gun_skill_005", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Gun_Skill_5_8_Client.Effect"), nullptr, _float3(0.25f, -6.f, 15.f)); };
	m_pModelBodyCom->Add_EffectFunc(8.f, "", "char_gun_skill_005", functionEffect);
	//char_gun_skill_006
	for (size_t i = 0; i < 11; i++)
	{
		_float fAnimFrame = 16.f + 3.f * i;
		functionEffect = [_pOwner = this, fAnimFrame]() { _pOwner->Create_WeaponEffect_RightHand(TEXT("Gun_Skill_6_Shoot_Client.Effect"), _float3(3.f, 0.25f, 0.f)); };
		m_pModelBodyCom->Add_EffectFunc(fAnimFrame, "", "char_gun_skill_006", functionEffect);
	}
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Gun_Skill_6_24_Circle_Client.Effect"), nullptr, _float3(0.f, 2.f, 3.f)); };
	m_pModelBodyCom->Add_EffectFunc(20.f, "", "char_gun_skill_006", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Gun_Skill_6_24_Client.Effect"), nullptr, _float3(0.f, 2.f, 3.5f)); };
	m_pModelBodyCom->Add_EffectFunc(26.f, "", "char_gun_skill_006", functionEffect);
	//char_gun_skill_002
	functionEffect = [_pOwner = this]() { _pOwner->Create_WeaponEffect_RightHand(TEXT("Gun_Skill_5_6_Client.Effect"), _float3(3.f, 0.25f, 0.f)); };
	m_pModelBodyCom->Add_EffectFunc(11.f, "", "char_gun_skill_002", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_WeaponEffect_RightHand(TEXT("Gun_Skill_2_13_Client.Effect"), _float3(3.f, 0.25f, 0.f), false, _float3(0.f, 0.f, 0.f), true); };
	m_pModelBodyCom->Add_EffectFunc(13.f, "", "char_gun_skill_002", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_WeaponEffect_RightHand(TEXT("Gun_Skill_2_16_Client.Effect"), _float3(5.f, 0.25f, 0.f), false, _float3(0.f, 0.f, 0.f), true); };
	m_pModelBodyCom->Add_EffectFunc(16.f, "", "char_gun_skill_002", functionEffect);

	//char_gun_skill_skill
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Gun_Skill_Skill_Dust_Client.Effect"), nullptr, _float3(0.f, 1.f, 0.f)); };
	m_pModelBodyCom->Add_EffectFunc(11.f, "", "char_gun_skill_skill", functionEffect);
	m_pModelBodyCom->Add_EffectFunc(21.f, "", "char_gun_skill_skill", functionEffect);
	m_pModelBodyCom->Add_EffectFunc(28.f, "", "char_gun_skill_skill", functionEffect);
	m_pModelBodyCom->Add_EffectFunc(36.f, "", "char_gun_skill_skill", functionEffect);
	m_pModelBodyCom->Add_EffectFunc(44.f, "", "char_gun_skill_skill", functionEffect);
	m_pModelBodyCom->Add_EffectFunc(51.f, "", "char_gun_skill_skill", functionEffect);
	m_pModelBodyCom->Add_EffectFunc(60.f, "", "char_gun_skill_skill", functionEffect);
	m_pModelBodyCom->Add_EffectFunc(68.f, "", "char_gun_skill_skill", functionEffect);

	_fChangeRotate = -10.f;
	_float fAnimFrame = 11.f;
	functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("Gun_Skill_Melee3_35_Client.Effect"), 0.f, _fChangeRotate, 0.f, _fChangeRotate, 2.5f, 2.7f); };
	m_pModelBodyCom->Add_EffectFunc(fAnimFrame, "", "char_gun_skill_skill", functionEffect);
	functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("Gun_Skill_Melee3_35_Circle_Client.Effect"), 0.f, _fChangeRotate, 0.f, _fChangeRotate, 4.75f, 2.2f); };
	m_pModelBodyCom->Add_EffectFunc(fAnimFrame, "", "char_gun_skill_skill", functionEffect);
	functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("Gun_Skill_Melee3_35_Circle_Client.Effect"), 0.f, _fChangeRotate, 0.f, _fChangeRotate, 5.25f, 3.2f); };
	m_pModelBodyCom->Add_EffectFunc(fAnimFrame, "", "char_gun_skill_skill", functionEffect);
	_fChangeRotate = 10.f;
	fAnimFrame = 21.f;
	functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("Gun_Skill_Melee3_35_Client.Effect"), 0.f, _fChangeRotate, 0.f, _fChangeRotate, 2.5f, 2.7f); };
	m_pModelBodyCom->Add_EffectFunc(fAnimFrame, "", "char_gun_skill_skill", functionEffect);
	functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("Gun_Skill_Melee3_35_Circle_Client.Effect"), 0.f, _fChangeRotate, 0.f, _fChangeRotate, 4.75f, 2.2f); };
	m_pModelBodyCom->Add_EffectFunc(fAnimFrame, "", "char_gun_skill_skill", functionEffect);
	functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("Gun_Skill_Melee3_35_Circle_Client.Effect"), 0.f, _fChangeRotate, 0.f, _fChangeRotate, 5.25f, 3.2f); };
	m_pModelBodyCom->Add_EffectFunc(fAnimFrame, "", "char_gun_skill_skill", functionEffect);
	_fChangeRotate = 0.f;
	fAnimFrame = 28.f;
	functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("Gun_Skill_Melee3_35_Client.Effect"), 0.f, _fChangeRotate, 0.f, _fChangeRotate, 2.5f, 2.7f); };
	m_pModelBodyCom->Add_EffectFunc(fAnimFrame, "", "char_gun_skill_skill", functionEffect);
	functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("Gun_Skill_Melee3_35_Circle_Client.Effect"), 0.f, _fChangeRotate, 0.f, _fChangeRotate, 4.75f, 2.2f); };
	m_pModelBodyCom->Add_EffectFunc(fAnimFrame, "", "char_gun_skill_skill", functionEffect);
	functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("Gun_Skill_Melee3_35_Circle_Client.Effect"), 0.f, _fChangeRotate, 0.f, _fChangeRotate, 5.25f, 3.2f); };
	m_pModelBodyCom->Add_EffectFunc(fAnimFrame, "", "char_gun_skill_skill", functionEffect);
	_fChangeRotate = -20.f;
	fAnimFrame = 36.f;
	functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("Gun_Skill_Melee3_35_Client.Effect"), 0.f, _fChangeRotate, 0.f, _fChangeRotate, 2.5f, 2.7f); };
	m_pModelBodyCom->Add_EffectFunc(fAnimFrame, "", "char_gun_skill_skill", functionEffect);
	functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("Gun_Skill_Melee3_35_Circle_Client.Effect"), 0.f, _fChangeRotate, 0.f, _fChangeRotate, 4.75f, 2.2f); };
	m_pModelBodyCom->Add_EffectFunc(fAnimFrame, "", "char_gun_skill_skill", functionEffect);
	functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("Gun_Skill_Melee3_35_Circle_Client.Effect"), 0.f, _fChangeRotate, 0.f, _fChangeRotate, 5.25f, 3.2f); };
	m_pModelBodyCom->Add_EffectFunc(fAnimFrame, "", "char_gun_skill_skill", functionEffect);
	_fChangeRotate = -40.f;
	fAnimFrame = 44.f;
	functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("Gun_Skill_Melee3_35_Client.Effect"), 0.f, _fChangeRotate, 0.f, _fChangeRotate, 2.5f, 2.7f); };
	m_pModelBodyCom->Add_EffectFunc(fAnimFrame, "", "char_gun_skill_skill", functionEffect);
	functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("Gun_Skill_Melee3_35_Circle_Client.Effect"), 0.f, _fChangeRotate, 0.f, _fChangeRotate, 4.75f, 2.2f); };
	m_pModelBodyCom->Add_EffectFunc(fAnimFrame, "", "char_gun_skill_skill", functionEffect);
	functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("Gun_Skill_Melee3_35_Circle_Client.Effect"), 0.f, _fChangeRotate, 0.f, _fChangeRotate, 5.25f, 3.2f); };
	m_pModelBodyCom->Add_EffectFunc(fAnimFrame, "", "char_gun_skill_skill", functionEffect);
	_fChangeRotate = -20.f;
	fAnimFrame = 51.f;
	functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("Gun_Skill_Melee3_35_Client.Effect"), 0.f, _fChangeRotate, 0.f, _fChangeRotate, 2.5f, 2.7f); };
	m_pModelBodyCom->Add_EffectFunc(fAnimFrame, "", "char_gun_skill_skill", functionEffect);
	functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("Gun_Skill_Melee3_35_Circle_Client.Effect"), 0.f, _fChangeRotate, 0.f, _fChangeRotate, 4.75f, 2.2f); };
	m_pModelBodyCom->Add_EffectFunc(fAnimFrame, "", "char_gun_skill_skill", functionEffect);
	functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("Gun_Skill_Melee3_35_Circle_Client.Effect"), 0.f, _fChangeRotate, 0.f, _fChangeRotate, 5.25f, 3.2f); };
	m_pModelBodyCom->Add_EffectFunc(fAnimFrame, "", "char_gun_skill_skill", functionEffect);
	_fChangeRotate = 0.f;
	fAnimFrame = 60.f;
	functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("Gun_Skill_Melee3_35_Client.Effect"), 0.f, _fChangeRotate, 0.f, _fChangeRotate, 2.5f, 2.7f); };
	m_pModelBodyCom->Add_EffectFunc(fAnimFrame, "", "char_gun_skill_skill", functionEffect);
	functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("Gun_Skill_Melee3_35_Circle_Client.Effect"), 0.f, _fChangeRotate, 0.f, _fChangeRotate, 4.75f, 2.2f); };
	m_pModelBodyCom->Add_EffectFunc(fAnimFrame, "", "char_gun_skill_skill", functionEffect);
	functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("Gun_Skill_Melee3_35_Circle_Client.Effect"), 0.f, _fChangeRotate, 0.f, _fChangeRotate, 5.25f, 3.2f); };
	m_pModelBodyCom->Add_EffectFunc(fAnimFrame, "", "char_gun_skill_skill", functionEffect);
	_fChangeRotate = -10.f;
	fAnimFrame = 68.f;
	functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("Gun_Skill_Melee3_35_Client.Effect"), 0.f, _fChangeRotate, 0.f, _fChangeRotate, 2.5f, 2.7f); };
	m_pModelBodyCom->Add_EffectFunc(fAnimFrame, "", "char_gun_skill_skill", functionEffect);
	functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("Gun_Skill_Melee3_35_Circle_Client.Effect"), 0.f, _fChangeRotate, 0.f, _fChangeRotate, 4.75f, 2.2f); };
	m_pModelBodyCom->Add_EffectFunc(fAnimFrame, "", "char_gun_skill_skill", functionEffect);
	functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("Gun_Skill_Melee3_35_Circle_Client.Effect"), 0.f, _fChangeRotate, 0.f, _fChangeRotate, 5.25f, 3.2f); };
	m_pModelBodyCom->Add_EffectFunc(fAnimFrame, "", "char_gun_skill_skill", functionEffect);

#pragma endregion
	//Linghan
#pragma region Linghan
	//char_linghan_skill_001
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Linghan_Skill_01_10_Ground_Client.Effect"), nullptr, _float3(2.f, 0.1f, 2.f)); };
	m_pModelBodyCom->Add_EffectFunc(9.f, "", "char_linghan_skill_001", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_WeaponEffect_RightHand(TEXT("Linghan_Skill_01_10_Client.Effect"), _float3(0.f, 0.f, 0.f)); };
	m_pModelBodyCom->Add_EffectFunc(10.f, "", "char_linghan_skill_001", functionEffect);
	//char_linghan_skill_002
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Linghan_Skill_02_12_Ground_Client.Effect"), nullptr, _float3(0.f, 0.1f, 1.f)); };
	m_pModelBodyCom->Add_EffectFunc(11.f, "", "char_linghan_skill_002", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_WeaponEffect_RightHand(TEXT("Linghan_Skill_02_12_Client.Effect"), _float3(0.f, 0.f, 0.f)); };
	m_pModelBodyCom->Add_EffectFunc(12.f, "", "char_linghan_skill_002", functionEffect);
	//char_linghan_skill_003
	functionEffect = [_pOwner = this, _pWorldmatrix = pWorldmatrix]() { _pOwner->Create_Effect(TEXT("Linghan_Skill_03_9_WithSword_Client.Effect"), _pWorldmatrix, _float3(0.f, 3.f, 0.f), false, true); };
	m_pModelBodyCom->Add_EffectFunc(9.f, "", "char_linghan_skill_003", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Linghan_Skill_03_9_Ground_Client.Effect"), nullptr, _float3(0.f, 0.1f, 1.f)); };
	m_pModelBodyCom->Add_EffectFunc(8.f, "", "char_linghan_skill_003", functionEffect);
	//char_linghan_skill_004
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Linghan_Skill_04_0_Sowrd_Client.Effect"), nullptr, _float3(3.5f, 3.f, -5.f)); };
	m_pModelBodyCom->Add_EffectFunc(0.5f, "", "char_linghan_skill_004", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Linghan_Skill_04_0_Sowrd_Client.Effect"), nullptr, _float3(-3.5f, 3.f, -5.f)); };
	m_pModelBodyCom->Add_EffectFunc(0.5f, "", "char_linghan_skill_004", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Linghan_Skill_04_3_Particle_Client.Effect"), nullptr, _float3(0.f, 2.5f, 7.5f)); };
	m_pModelBodyCom->Add_EffectFunc(0.5f, "", "char_linghan_skill_004", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Linghan_Skill_05_19_Ground_Client.Effect"), nullptr, _float3(0.f, 0.1f, 0.f)); };
	m_pModelBodyCom->Add_EffectFunc(0.5f, "", "char_linghan_skill_004", functionEffect);
	functionEffect = [_pOwner = this, _pWorldmatrix = pWorldmatrix]() { _pOwner->Create_Effect(TEXT("Linghan_Skill_04_8_Client.Effect"), _pWorldmatrix, _float3(0.f, 2.f, -4.5f), true); };
	m_pModelBodyCom->Add_EffectFunc(8.f, "", "char_linghan_skill_004", functionEffect);
	functionEffect = [_pOwner = this, _pWorldmatrix = pWorldmatrix]() { _pOwner->Create_Effect(TEXT("Linghan_Skill_04_8_2_Client.Effect"), _pWorldmatrix, _float3(-0.5f, 1.5f, -17.5f), true); };
	m_pModelBodyCom->Add_EffectFunc(8.f, "", "char_linghan_skill_004", functionEffect);
	functionEffect = [_pOwner = this, _pWorldmatrix = pWorldmatrix]() { _pOwner->Create_Effect(TEXT("Linghan_Skill_04_8_Dust_Client.Effect"), _pWorldmatrix, _float3(0.f, 2.f, 12.5f), true); };
	m_pModelBodyCom->Add_EffectFunc(7.f, "", "char_linghan_skill_004", functionEffect);
	//char_linghan_skill_005
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Linghan_Skill_05_15_Sword_Client.Effect"), nullptr, _float3(0.f, 3.f, -5.f)); };
	m_pModelBodyCom->Add_EffectFunc(15.f, "", "char_linghan_skill_005", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Linghan_Skill_05_15_Sword2_Client.Effect"), nullptr, _float3(0.f, 3.f, -10.f)); };
	m_pModelBodyCom->Add_EffectFunc(15.f, "", "char_linghan_skill_005", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Linghan_Skill_05_15_Sword_Particle_Client.Effect"), nullptr, _float3(0.f, 3.f, 0.f)); };
	m_pModelBodyCom->Add_EffectFunc(17.f, "", "char_linghan_skill_005", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Linghan_Skill_05_19_Ground_Client.Effect"), nullptr, _float3(0.f, 0.1f, 15.f)); };
	m_pModelBodyCom->Add_EffectFunc(19.f, "", "char_linghan_skill_005", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Linghan_Skill_05_19_Client.Effect"), nullptr, _float3(0.f, 3.f, 15.f)); };
	m_pModelBodyCom->Add_EffectFunc(19.f, "", "char_linghan_skill_005", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Linghan_Skill_05_21_Client.Effect"), nullptr, _float3(0.f, 3.f, 15.f)); };
	m_pModelBodyCom->Add_EffectFunc(21.f, "", "char_linghan_skill_005", functionEffect);
	//char_linghan_skill_005_1
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Linghan_Skill_05_1_21_Client.Effect"), nullptr, _float3(0.f, 2.5f, 6.f)); };
	m_pModelBodyCom->Add_EffectFunc(21.f, "", "char_linghan_skill_005_1", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Linghan_Skill_05_1_21_Ground_Plane_Client.Effect"), nullptr, _float3(0.f, 0.1f, 1.f)); };
	m_pModelBodyCom->Add_EffectFunc(21.f, "", "char_linghan_skill_005_1", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Linghan_Skill_05_1_21_Ground_Client.Effect"), nullptr, _float3(0.f, 0.1f, 7.5f)); };
	m_pModelBodyCom->Add_EffectFunc(23.f, "", "char_linghan_skill_005_1", functionEffect);
	//char_linghan_skill_009
	//functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Linghan_Skill_01_10_Ground_Client.Effect"), nullptr, _float3(2.f, 0.1f, 2.f)); };
	//m_pModelBodyCom->Add_EffectFunc(9.f, "", "char_linghan_skill_009", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_WeaponEffect_RightHand(TEXT("Linghan_Skill_01_10_Client.Effect"), _float3(0.f, 0.f, 0.f)); };
	m_pModelBodyCom->Add_EffectFunc(10.f, "", "char_linghan_skill_009", functionEffect);
	//char_linghan_skill_010
	//functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Linghan_Skill_02_12_Ground_Client.Effect"), nullptr, _float3(0.f, 0.1f, 1.f)); };
	//m_pModelBodyCom->Add_EffectFunc(11.f, "", "char_linghan_skill_010", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_WeaponEffect_RightHand(TEXT("Linghan_Skill_02_12_Client.Effect"), _float3(0.f, 0.f, 0.f)); };
	m_pModelBodyCom->Add_EffectFunc(12.f, "", "char_linghan_skill_010", functionEffect);
	//char_linghan_skill_011
	functionEffect = [_pOwner = this, _pWorldmatrix = pWorldmatrix]() { _pOwner->Create_Effect(TEXT("Linghan_Skill_03_9_WithSword_Client.Effect"), _pWorldmatrix, _float3(0.f, 3.f, 0.f), false, true); };
	m_pModelBodyCom->Add_EffectFunc(9.f, "", "char_linghan_skill_011", functionEffect);
	//functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Linghan_Skill_03_9_Ground_Client.Effect"), nullptr, _float3(0.f, 0.1f, 1.f)); };
	//m_pModelBodyCom->Add_EffectFunc(8.f, "", "char_linghan_skill_011", functionEffect);
	//char_linghan_skill_012
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Linghan_Skill_04_0_Sowrd_Client.Effect"), nullptr, _float3(3.5f, 3.f, -5.f)); };
	m_pModelBodyCom->Add_EffectFunc(0.5f, "", "char_linghan_skill_012", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Linghan_Skill_04_0_Sowrd_Client.Effect"), nullptr, _float3(-3.5f, 3.f, -5.f)); };
	m_pModelBodyCom->Add_EffectFunc(0.5f, "", "char_linghan_skill_012", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Linghan_Skill_04_3_Particle_Client.Effect"), nullptr, _float3(0.f, 2.5f, 7.5f)); };
	m_pModelBodyCom->Add_EffectFunc(0.5f, "", "char_linghan_skill_012", functionEffect);
	functionEffect = [_pOwner = this, _pWorldmatrix = pWorldmatrix]() { _pOwner->Create_Effect(TEXT("Linghan_Skill_04_8_Client.Effect"), _pWorldmatrix, _float3(0.f, 2.f, -4.5f), true); };
	m_pModelBodyCom->Add_EffectFunc(8.f, "", "char_linghan_skill_012", functionEffect);
	functionEffect = [_pOwner = this, _pWorldmatrix = pWorldmatrix]() { _pOwner->Create_Effect(TEXT("Linghan_Skill_04_8_2_Client.Effect"), _pWorldmatrix, _float3(-0.5f, 1.5f, -17.5f), true); };
	m_pModelBodyCom->Add_EffectFunc(8.f, "", "char_linghan_skill_012", functionEffect);
	functionEffect = [_pOwner = this, _pWorldmatrix = pWorldmatrix]() { _pOwner->Create_Effect(TEXT("Linghan_Skill_04_8_Dust_Client.Effect"), _pWorldmatrix, _float3(0.f, 2.f, 12.5f), true); };
	m_pModelBodyCom->Add_EffectFunc(7.f, "", "char_linghan_skill_012", functionEffect);
	//char_linghan_skill_013
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Linghan_Skill_05_15_Sword_Client.Effect"), nullptr, _float3(0.f, 3.f, -5.f)); };
	m_pModelBodyCom->Add_EffectFunc(15.f, "", "char_linghan_skill_013", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Linghan_Skill_05_15_Sword2_Client.Effect"), nullptr, _float3(0.f, 3.f, -10.f)); };
	m_pModelBodyCom->Add_EffectFunc(15.f, "", "char_linghan_skill_013", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Linghan_Skill_05_15_Sword_Particle_Client.Effect"), nullptr, _float3(0.f, 3.f, 0.f)); };
	m_pModelBodyCom->Add_EffectFunc(17.f, "", "char_linghan_skill_013", functionEffect);
	//functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Linghan_Skill_05_19_Ground_Client.Effect"), nullptr, _float3(0.f, 0.1f, 15.f)); };
	//m_pModelBodyCom->Add_EffectFunc(19.f, "", "char_linghan_skill_013", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Linghan_Skill_05_19_Client.Effect"), nullptr, _float3(0.f, 3.f, 15.f)); };
	m_pModelBodyCom->Add_EffectFunc(19.f, "", "char_linghan_skill_013", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Linghan_Skill_05_21_Client.Effect"), nullptr, _float3(0.f, 3.f, 15.f)); };
	m_pModelBodyCom->Add_EffectFunc(21.f, "", "char_linghan_skill_013", functionEffect);
	//char_linghan_skill_013_1
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Linghan_Skill_05_1_21_Client.Effect"), nullptr, _float3(0.f, 2.5f, 6.f)); };
	m_pModelBodyCom->Add_EffectFunc(21.f, "", "char_linghan_skill_013_1", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Linghan_Skill_05_1_21_Ground_Plane_Client.Effect"), nullptr, _float3(0.f, 0.1f, 1.f)); };
	m_pModelBodyCom->Add_EffectFunc(21.f, "", "char_linghan_skill_013_1", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Linghan_Skill_05_1_21_Ground_Client.Effect"), nullptr, _float3(0.f, 0.1f, 7.5f)); };
	m_pModelBodyCom->Add_EffectFunc(23.f, "", "char_linghan_skill_013_1", functionEffect);
	//char_linghan_skill_007
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Linghan_Skill_01_10_Ground_Client.Effect"), nullptr, _float3(-2.f, 0.1f, 2.f)); };
	m_pModelBodyCom->Add_EffectFunc(14.f, "", "char_linghan_skill_007", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_WeaponEffect_RightHand(TEXT("Linghan_Skill_01_10_Client.Effect"), _float3(0.f, 0.f, 0.f)); };
	m_pModelBodyCom->Add_EffectFunc(14.f, "", "char_linghan_skill_007", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Linghan_Skill_01_10_Ground_Client.Effect"), nullptr, _float3(-2.f, 0.1f, 2.f)); };
	m_pModelBodyCom->Add_EffectFunc(30.f, "", "char_linghan_skill_007", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_WeaponEffect_RightHand(TEXT("Linghan_Skill_07_30_Client.Effect"), _float3(0.f, 0.f, 0.f)); };
	m_pModelBodyCom->Add_EffectFunc(30.f, "", "char_linghan_skill_007", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_WeaponEffect_RightHand(TEXT("Linghan_Skill_07_30_Forward_Client.Effect"), _float3(0.f, 2.f, 0.f)); };
	m_pModelBodyCom->Add_EffectFunc(30.f, "", "char_linghan_skill_007", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_WeaponEffect_RightHand(TEXT("Linghan_Skill_07_30_Forward_Paricle_Client.Effect"), _float3(0.f, 10.f, 0.f)); };
	m_pModelBodyCom->Add_EffectFunc(30.f, "", "char_linghan_skill_007", functionEffect);
	//char_linghan_skill_007_sea
	//functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Linghan_Skill_01_10_Ground_Client.Effect"), nullptr, _float3(-2.f, 0.1f, 2.f)); };
	//m_pModelBodyCom->Add_EffectFunc(14.f, "", "char_linghan_skill_007_sea", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_WeaponEffect_RightHand(TEXT("Linghan_Skill_01_10_Client.Effect"), _float3(0.f, 0.f, 0.f)); };
	m_pModelBodyCom->Add_EffectFunc(14.f, "", "char_linghan_skill_007_sea", functionEffect);
	//functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Linghan_Skill_01_10_Ground_Client.Effect"), nullptr, _float3(-2.f, 0.1f, 2.f)); };
	//m_pModelBodyCom->Add_EffectFunc(30.f, "", "char_linghan_skill_007_sea", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_WeaponEffect_RightHand(TEXT("Linghan_Skill_07_30_Client.Effect"), _float3(0.f, 0.f, 0.f)); };
	m_pModelBodyCom->Add_EffectFunc(30.f, "", "char_linghan_skill_007_sea", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_WeaponEffect_RightHand(TEXT("Linghan_Skill_07_30_Forward_Client.Effect"), _float3(0.f, 2.f, 0.f)); };
	m_pModelBodyCom->Add_EffectFunc(30.f, "", "char_linghan_skill_007_sea", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_WeaponEffect_RightHand(TEXT("Linghan_Skill_07_30_Forward_Paricle_Client.Effect"), _float3(0.f, 10.f, 0.f)); };
	m_pModelBodyCom->Add_EffectFunc(30.f, "", "char_linghan_skill_007_sea", functionEffect);

	//char_linghan_skill_015
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Linghan_Skill_015_45_Ground_Client.Effect"), nullptr, _float3(0.f, 0.25f, -5.f)); };
	m_pModelBodyCom->Add_EffectFunc(45.f, "", "char_linghan_skill_015", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Linghan_Skill_015_75_Ground_Client.Effect"), nullptr, _float3(0.f, 0.25f, -5.f)); };
	m_pModelBodyCom->Add_EffectFunc(75.f, "", "char_linghan_skill_015", functionEffect);
	functionEffect = [_pOwner = this]() { _pOwner->Create_Effect(TEXT("Linghan_Skill_015_75_Client.Effect"), nullptr, _float3(0.f, 1.f, -5.f)); };
	m_pModelBodyCom->Add_EffectFunc(75.f, "", "char_linghan_skill_015", functionEffect);
	for (size_t i = 0; i < 10; i++)
	{
		_float fAnimFrame = 75.f + 3.5f * i;
		_float	_fChangeRotate = 360.f / 10.f * i;
		functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("Linghan_Skill_015_75_Sword_Client.Effect"), 0.f, 0.f, 0.f, _fChangeRotate, 8.f, 1.f, _float3(0.f, 1.f, -5.f)); };
		m_pModelBodyCom->Add_EffectFunc(fAnimFrame, "", "char_linghan_skill_015", functionEffect);

		functionEffect = [_pOwner = this, _fChangeRotate]() { _pOwner->Create_Effect_Rotate(TEXT("Linghan_Skill_015_75_SwordUp_Client.Effect"), 0.f, 0.f, 0.f, _fChangeRotate, 8.f, 1.f, _float3(0.f, 1.f, -5.f)); };
		m_pModelBodyCom->Add_EffectFunc(fAnimFrame, "", "char_linghan_skill_015", functionEffect);
	}



}

CGameObject* CPlayer::Clone(void* _pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to clone : CPlayer");
	}

	return pInstance;
}

HRESULT CPlayer::Add_Components(void* _pArg)
{
	// Add renderer component
	if (FAILED(__super::Add_Component(LEVEL_STATIC, L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
	{
		return E_FAIL;
	}

	// Add shader component
	if (FAILED(__super::Add_Component(m_eLevel, PC_SHADER_CHARACTER, L"Com_Shader", (CComponent**)&m_pShaderCom)))
	{
		return E_FAIL;
	}

	// Add and initialize transform component
	CTransform::TRANSFORMDESC tTransformDesc;
	ZeroMemory(&tTransformDesc, sizeof(tTransformDesc));
	tTransformDesc.fRotationPerSec = 10.f;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, L"Prototype_Component_Transform", L"Com_Transform", (CComponent**)&m_pTransformCom, &tTransformDesc)))
	{
		return E_FAIL;
	}
	INITDATA* pData = (INITDATA*)_pArg;
	m_pTransformCom->RotationByCurTransform({ 1.f, 0.f, 0.f }, XMConvertToRadians(pData->vRotation.x));
	m_pTransformCom->RotationByCurTransform({ 0.f, 1.f, 0.f }, XMConvertToRadians(pData->vRotation.y));
	m_pTransformCom->RotationByCurTransform({ 0.f, 0.f, 1.f }, XMConvertToRadians(pData->vRotation.z));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(pData->vPos.x, pData->vPos.y, pData->vPos.z, 1.f));


	// Add (only for model draw)transform component
	if (FAILED(__super::Add_Component(LEVEL_STATIC, L"Prototype_Component_Transform", L"Com_Transform_ModelDraw", (CComponent**)&m_pTransformModelDrawCom, &tTransformDesc)))
	{
		return E_FAIL;
	}


	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

#pragma region Collider Settings
	// 1.) Add and initialize sphere melee area collider
	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof ColliderDesc);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 3.f);
	ColliderDesc.fRadius = 4.f;
	ColliderDesc.vRotation = _float3(0.0f, 0.f, 0.f);
	CComponent* pCollider = pGameInstance->Add_ColliderComponent(m_eLevel,
		TEXT("Prototype_Component_Collider_Sphere"), this, COLLISION_GROUP::PLAYER_MELEE_WEAPON, &ColliderDesc);
	Register_ColliderCom(L"Melee_Area", pCollider);
	if (nullptr == pCollider)
	{
		return E_FAIL;
	}
	m_pMeleeAttackColliderCom = static_cast<CCollider*>(pCollider);
	m_pMeleeAttackColliderCom->Set_Enable(false);

	// 2.) Add and initialize OBB melee area collider
	ZeroMemory(&ColliderDesc, sizeof ColliderDesc);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 3.f);
	ColliderDesc.vSize = _float3(5.f, 5.f, 5.f);
	ColliderDesc.vRotation = _float3(0.0f, 0.f, 0.f);
	pCollider = pGameInstance->Add_ColliderComponent(m_eLevel,
		TEXT("Prototype_Component_Collider_OBB"), this, COLLISION_GROUP::PLAYER_MELEE_WEAPON, &ColliderDesc);
	Register_ColliderCom(L"Melee_Area_OBB", pCollider);
	if (nullptr == pCollider)
	{
		return E_FAIL;
	}
	m_pMeleeOBBAttackColliderCom = static_cast<CCollider*>(pCollider);
	m_pMeleeOBBAttackColliderCom->Set_Enable(false);

	// 3.) Add and initialize sphere collider for solid body
	ZeroMemory(&ColliderDesc, sizeof ColliderDesc);
	ColliderDesc.fRadius = 2.f;
	pCollider = pGameInstance->Add_ColliderComponent(m_eLevel,
		TEXT("Prototype_Component_Collider_Sphere"), this, COLLISION_GROUP::PLAYER_BODY, &ColliderDesc);
	Register_ColliderCom(L"Player_SolidBody", pCollider);
	if (nullptr == pCollider)
	{
		return E_FAIL;
	}
	m_pPlayerBodyColliderCom = static_cast<CCollider*>(pCollider);
	m_pPlayerBodyColliderCom->Set_Enable(true);

	// 4.) Add and initialize OBB collider for hit body
	ZeroMemory(&ColliderDesc, sizeof ColliderDesc);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vSize = _float3(2.f, 3.f, 2.f);
	pCollider = pGameInstance->Add_ColliderComponent(m_eLevel,
		TEXT("Prototype_Component_Collider_OBB"), this, COLLISION_GROUP::PLAYER_HIT_BODY, &ColliderDesc);
	Register_ColliderCom(L"Player_HitBody", pCollider);
	m_pPlayerBodyHitColliderCom = static_cast<CCollider*>(pCollider);
	m_pPlayerBodyHitColliderCom->Set_Enable(true);


	// 5.) Add and initialize OBB collider for hit timeillusion check
	ZeroMemory(&ColliderDesc, sizeof ColliderDesc);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	//ColliderDesc.vSize = _float3(50.f, 50.f, 50.f);
	ColliderDesc.fRadius = 20.f;//100.f;
	pCollider = pGameInstance->Add_ColliderComponent(m_eLevel,
		TEXT("Prototype_Component_Collider_Sphere"), this, COLLISION_GROUP::PLAYER_HIT_AREA_MARGIN, &ColliderDesc);
	Register_ColliderCom(L"Player_Hit_Margin_Area", pCollider);
	m_pPlayerHitTimeillusionColliderCom = static_cast<CCollider*>(pCollider);
	m_pPlayerHitTimeillusionColliderCom->Set_Enable(true);

	// 6. ) Add and initialize OBB collider for player interaction
	//m_pPlayerInteractionCom
	ZeroMemory(&ColliderDesc, sizeof ColliderDesc);
	ColliderDesc.vPosition = _float3(0.f, 2.5f, 1.5f);
	ColliderDesc.vSize = _float3(5.f, 5.f, 5.f);
	ColliderDesc.fRadius = 20.f;//100.f;
	pCollider = pGameInstance->Add_ColliderComponent(m_eLevel,
		TEXT("Prototype_Component_Collider_OBB"), this, COLLISION_GROUP::PLAYER_INTERACTION, &ColliderDesc);
	Register_ColliderCom(L"Player_Intraction", pCollider);
	m_pPlayerInteractionColliderCom = static_cast<CCollider*>(pCollider);
	m_pPlayerInteractionColliderCom->Set_Enable(true);
	m_pPlayerInteractionColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());


#pragma endregion
	// Add and initialize model component
	CModel::MODELDESC tModelDesc;
	tModelDesc.jsonTag = "Player";
	tModelDesc.pOwner = this;
	if (FAILED(__super::Add_Component(m_eLevel, L"Prototype_Component_Model_Player_Body", L"Com_Model_Body", (CComponent**)&m_pModelBodyCom, (void*)&tModelDesc)))
	{
		return E_FAIL;
	}

	Matrix matRotation = XMMatrixRotationZ(XMConvertToRadians(90.f)) * XMMatrixRotationY(XMConvertToRadians(90.f));
	Matrix matScaling = XMMatrixScaling(0.025f, 0.025f, 0.025f);
	Matrix matPivot = matScaling * matRotation;
	m_pModelBodyCom->Set_PivotMatrix(matPivot);

	CAnimator* pAnimator = m_pModelBodyCom->Get_Animator();
	pAnimator->Set_XZRootBone("Root_Bone");
	pAnimator->Set_YRootBone("Bip001");

	//Effects
	Create_WeaponEffects();

	CPlayer_Combat_Module::PLAYERCOMBATDESC tCombatDesc;
	tCombatDesc.pOwner = this;
	tCombatDesc.pAnimator = m_pModelBodyCom->Get_Animator();
	m_pCombatModule = CPlayer_Combat_Module::Create((void*)&tCombatDesc);

	CPlayer_Control_Module::PLAYERCONTROLDESC tControlDesc;
	tControlDesc.pOwner = this;
	tControlDesc.pAnimator = m_pModelBodyCom->Get_Animator();

	m_pControlModule = CPlayer_Control_Module::Create((void*)&tControlDesc);
	m_pSettingModule = CPlayer_Setting_Module::Create();
	m_pSettingModule->Initialize_Player_Animation_State(this, m_pModelBodyCom);

	CPhysics::PHYSICSDESC tPhysicsDesc;
	tPhysicsDesc.fMaxSpeed = m_fYMaxPhysicsSpeed;//30.f;
	tPhysicsDesc.fMinSpeed = m_fYMinPhysicsSpeed;//-70.f;
	tPhysicsDesc.fGravityOffset = 1.f;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, L"Prototype_Component_Physics", L"Com_Physics",
		(CComponent**)&m_pPhysics, &tPhysicsDesc)))
	{
		return E_FAIL;
	}

	CTransform* pTerrainTransform = (CTransform*)pGameInstance->Get_Component(m_eLevel, TEXT("Layer_Map"), TEXT("Com_Transform"));
	CNavigation::NAVIDESC tNaviDesc;
	tNaviDesc.pWorldMatrix = pTerrainTransform->Get_WorldMatrixFloat4x4Ptr();
	if (FAILED(__super::Add_Component(m_eLevel, L"Prototype_Component_Navigation",
		L"Com_Navigation", (CComponent**)&m_pNavigationCom, (void*)&tNaviDesc)))
	{
		return E_FAIL;
	}
	m_pNavigationCom->Set_CurrentIndex(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pNavigationCom->Register_Not_Go_Cell(6u);

	return S_OK;
}

HRESULT CPlayer::Move_Player_From_ADS_Mode(_float _fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	Vector4 vDir = { 0.f, 0.f, 0.f, 0.f };

	if ((CFunc::Is_KeyDown(DIK_W) || CFunc::Is_KeyHold(DIK_W)) &&
		(CFunc::Is_KeyDown(DIK_D) || CFunc::Is_KeyHold(DIK_D)))
	{
		_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(45.f));
		_matrix matViewInv = XMMatrixInverse(nullptr, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
		vDir = XMVector3TransformNormal(XMVector3Normalize(XMVectorSetY(matViewInv.r[2], 0.f)), matRotY);
	}
	else if ((CFunc::Is_KeyDown(DIK_W) || CFunc::Is_KeyHold(DIK_W)) &&
			 (CFunc::Is_KeyDown(DIK_A) || CFunc::Is_KeyHold(DIK_A)))
	{
		_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(-45.f));
		_matrix matViewInv = XMMatrixInverse(nullptr, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
		vDir = XMVector3TransformNormal(XMVector3Normalize(XMVectorSetY(matViewInv.r[2], 0.f)), matRotY);
	}
	else if ((CFunc::Is_KeyDown(DIK_S) || CFunc::Is_KeyHold(DIK_S)) &&
			 (CFunc::Is_KeyDown(DIK_A) || CFunc::Is_KeyHold(DIK_A)))
	{
		_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(-45.f));
		_matrix matViewInv = XMMatrixInverse(nullptr, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
		vDir = XMVector3TransformNormal(XMVector3Normalize(XMVectorSetY(matViewInv.r[0], 0.f)), matRotY) * -1.f;
	}
	else if ((CFunc::Is_KeyDown(DIK_S) || CFunc::Is_KeyHold(DIK_S)) &&
			 (CFunc::Is_KeyDown(DIK_D) || CFunc::Is_KeyHold(DIK_D)))
	{
		_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(45.f));
		_matrix matViewInv = XMMatrixInverse(nullptr, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
		vDir = XMVector3TransformNormal(XMVector3Normalize(XMVectorSetY(matViewInv.r[0], 0.f)), matRotY);
	}
	else if (CFunc::Is_KeyDown(DIK_W) || CFunc::Is_KeyHold(DIK_W))
	{
		_matrix matViewInv = XMMatrixInverse(nullptr, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
		vDir = XMVector3Normalize(XMVectorSetY(matViewInv.r[2], 0.f));
	}
	else if (CFunc::Is_KeyDown(DIK_A) || CFunc::Is_KeyHold(DIK_A))
	{
		_matrix matViewInv = XMMatrixInverse(nullptr, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
		vDir = XMVector3Normalize(XMVectorSetY(matViewInv.r[0], 0.f)) * -1.f;
	}
	else if (CFunc::Is_KeyDown(DIK_D) || CFunc::Is_KeyHold(DIK_D))
	{
		_matrix matViewInv = XMMatrixInverse(nullptr, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
		vDir = XMVector3Normalize(XMVectorSetY(matViewInv.r[0], 0.f));
	}
	else if (CFunc::Is_KeyDown(DIK_S) || CFunc::Is_KeyHold(DIK_S))
	{
		_matrix matViewInv = XMMatrixInverse(nullptr, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
		vDir = XMVector3Normalize(XMVectorSetY(matViewInv.r[2], 0.f)) * -1.f;
	}

	Vector4 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos += vDir * 7.f * _fTimeDelta;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos, m_pNavigationCom);

	return S_OK;
}

HRESULT CPlayer::Handling_Aimming(_float _fTimeDelta)
{
	if (PLAYER_STATE::Aimming == m_eCurState || PLAYER_STATE::Aimming_Move == m_eCurState ||
		PLAYER_STATE::Aimming_Stand_Attack == m_eCurState || PLAYER_STATE::Aimming_Move_Attack == m_eCurState)
	{
		CAnimator* pAnimator = m_pModelBodyCom->Get_Animator();
		CAnimationState* pCurrentAnimationState = pAnimator->Get_CurAnimState();
		AnimState eCurrentAnimationState = pCurrentAnimationState->Get_eAnimStateTag();

		//if (PLAYER_STATE::Aimming_Stand_Attack == m_eCurState || PLAYER_STATE::Aimming_Move_Attack == m_eCurState)
		//{
			//Sub_EP(m_tPlayerStateDesc.fMaxEP * 0.1f * _fTimeDelta);
		//}

		if (ViewMode::ADS == m_eViewMode)
		{
			// 크로스헤어 활성화
			CFunc::Enable_Crosshair();

			m_bSmoothRotation = false;
			//Vector3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			//m_vADS_Camera_Eye = vPos;
			//m_vADS_Camera_At = vPos;


			Matrix matRot;
			matRot = matRot.CreateRotationZ(XMConvertToRadians(Get_ADSAngle()));
			m_pModelBodyCom->Set_BoneAdjustMatrix(matRot, "Bip001-Spine1");

			m_fADSRotationVelocity = 40.f;

			Matrix matViewInv = CGameInstance::Get_Instance()->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_VIEW);
			Vector3 vCamLook = matViewInv.Backward(); vCamLook.y = 0.f; vCamLook.Normalize();
			Vector4 vCamNormalizedLook = { vCamLook.x, vCamLook.y, vCamLook.z, 0.f };

			Vector4 vPlayerLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			vPlayerLook.y = 0.f; vPlayerLook.Normalize();

			Vector4 vNewLook = SmoothDampDirectionalVector({ 0.f, 1.f, 0.f, 0.f }, vPlayerLook, vCamNormalizedLook, m_fADSRotationVelocity, 0.025f, 360.f, _fTimeDelta);

			m_pTransformCom->Set_NewLook(vNewLook);

		}
	}
	else
	{
		// 크로스헤어 비활성화
		CFunc::Disable_Crosshair();
	}
	





	return S_OK;
}

void CPlayer::Set_ADS()
{
	m_eCurState = PLAYER_STATE::Aimming;
	m_eViewMode = ViewMode::ADS;
	Camera_To_ADS();

	if (CPlayer_Weapon::WEAPON_STATE::WEAPON_OUT != m_pCurWeapon->Get_Weapon_State())
	{
		m_pCurWeapon->Out_Weapon();
	}

}

void CPlayer::Set_TPS()
{
	m_eCurState = PLAYER_STATE::Idle;
	m_eViewMode = ViewMode::TPS;
	Camera_To_Original();

	Matrix matRot;
	matRot = matRot.CreateRotationZ(XMConvertToRadians(0.f));
	m_pModelBodyCom->Set_BoneAdjustMatrix(matRot, "Bip001-Spine1");

	//if (CPlayer_Weapon::WEAPON_STATE::WEAPON_IN != m_pCurWeapon->Get_Weapon_State())
	//{
	//	m_pCurWeapon->In_Weapon();
	//}
}

_float	CPlayer::Get_ADSAngle()
{
	_float fResult = 0.f;
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();


	Matrix matViewInv = pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_VIEW);
	Vector3 vCamLook = matViewInv.Backward();
	Vector3 vUp = matViewInv.Up();


	Vector3 vDirBase = vCamLook;
	Vector3 vDirTarget = vCamLook;


	Vector3 vXZLook = vCamLook;
	vXZLook.y = 0.f;

	vXZLook.Normalize();
	vCamLook.Normalize();


	_float fDot = vCamLook.Dot(vXZLook);
	_float fRad = acosf(fDot);
	_float fDeg = XMConvertToDegrees(fRad);


	vDirBase.y = 0.f;
	vDirBase.x = 0.f;
	vDirBase.z = fabsf(vDirBase.z);
	vDirBase.Normalize();

	vDirTarget.x = 0.f;
	vDirTarget.z = fabsf(vDirTarget.z);
	vDirTarget.Normalize();

	_float fX = XMVectorGetX(XMVector3Cross(vDirBase, vDirTarget));

	if (fX < 0.f)
	{
		fDeg *= -1.f;
	}

	fResult = fDeg;

	return fResult;
}

HRESULT CPlayer::Handling_Jump(_float _fTimeDelta)
{
	_bool bShiftTrigger = Is_ShiftHoldTrigger();
	_bool bMoveInput = Is_MoveInput();

	Vector4 vCurrentPlayerPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float fYGround = 0.f;
	m_pNavigationCom->Get_YWorldCoordinate(vCurrentPlayerPosition, fYGround);

	CAnimationState* pCurrentAnimationState = m_pModelBodyCom->Get_Animator()->Get_CurAnimState();
	AnimState			eCurrentAnimationState = m_pModelBodyCom->Get_Animator()->Get_CurAnimState()->Get_eAnimStateTag();
	_float				fTrackPosition = m_pModelBodyCom->Get_Animator()->Get_TrackPosition();

	// Irregular case when, upper sickle attack and land height not same with start.
	if (AnimState::Sickle_Melee_Attack05 == eCurrentAnimationState &&
		!pCurrentAnimationState->Is_Transition() && 50.f < fTrackPosition)
	{
		_float fYOutDist = Get_Y_Ground_Distance();

		if (0 > fYOutDist && 0.5f <= fabsf(fYOutDist))
		{
			Set_Fall_State(0.f);
		}
	}


	if (PLAYER_STATE::Falling_Attack != m_eCurState &&
		PLAYER_STATE::Air_Attack != m_eCurState &&
		PLAYER_STATE::Jump != m_eCurState &&
		PLAYER_STATE::Jump_Air != m_eCurState &&
		PLAYER_STATE::Gliding != m_eCurState &&
		PLAYER_STATE::Hit_Throw_Air != m_eCurState &&
		PLAYER_STATE::Mounting_Jump != m_eCurState)
	{
		// To-Do : Temp. figure out!
		if (AnimState::Hand_Jump_Loop == eCurrentAnimationState)
		{
			m_pModelBodyCom->Change_Animation(AnimState::Hand_Jump_Down);
		}

		return S_OK;
	}
	else if (PLAYER_STATE::Jump == m_eCurState || PLAYER_STATE::Jump_Air == m_eCurState)
	{

		//if (vCurrentPos.y <= 0.f)
		if (vCurrentPlayerPosition.y <= fYGround)
		{
			if (!bShiftTrigger && bMoveInput)
			{
				m_eCurState = PLAYER_STATE::Run;
				m_ePrevState = m_eCurState;
				m_pModelBodyCom->Change_Animation("char_h_jump_downlowrun");
			}
			else if (bShiftTrigger && bMoveInput)
			{
				m_eCurState = PLAYER_STATE::Sprint;
				m_ePrevState = m_eCurState;
				m_pModelBodyCom->Change_Animation("char_h_jump_fallsprint");
			}
			else
			{
				m_eCurState = PLAYER_STATE::Idle;
				m_ePrevState = m_eCurState;
				m_pModelBodyCom->Change_Animation("char_h_jump_downcommon");
			}
		}


	}
	else if (PLAYER_STATE::Falling_Attack == m_eCurState)
	{
		CAnimator* pAnimator = m_pModelBodyCom->Get_Animator();
		AnimState eCurAnimState = pAnimator->Get_CurAnimState()->Get_eAnimStateTag();

		if (AnimState::Sickle_Melee_Air_Attack05Loop == eCurAnimState || AnimState::Dual_Gun_DropKick_Loop == eCurAnimState)
		{
			if (!m_pPhysics->Is_Enable_Gravity() || !m_pPhysics->Is_Enable_Velocity())
			{
				m_pPhysics->Set_Enable_Gravity(true);
				m_pPhysics->Set_Enable_Velocity(true);
			}


			//Vector4 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			//if (vPos.y <= 0.f)
			if (vCurrentPlayerPosition.y <= fYGround)
			{

				if (AnimState::Sickle_Melee_Air_Attack05Loop == eCurAnimState)
				{
					pAnimator->Change_Animation(AnimState::Sickle_Melee_Air_Attack05End);
				}
				else
				{
					pAnimator->Change_Animation(AnimState::Dual_Gun_DropKick_End);
				}
			}

		}
		else if ((AnimState::Sickle_Melee_Air_Attack05End == eCurAnimState || AnimState::Dual_Gun_DropKick_End == eCurAnimState) &&
			pCurrentAnimationState->Is_PlayDone())
		{
			m_eCurState = PLAYER_STATE::Idle;
		}
	}
	else if (PLAYER_STATE::Falling_Attack == m_eCurState)
	{


	}
	else if (PLAYER_STATE::Gliding == m_eCurState)
	{
		Vector4 vPlayerPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_float fYDist;
		CNavigation::INTERSECTDIRECTION eDir = m_pNavigationCom->Get_YIntersectDistanceFromPoint(vPlayerPosition, fYDist);
		
		if (AnimState::Hand_Glid_Start_Up != eCurrentAnimationState && 
			(CNavigation::INTERSECTDIRECTION::UP == eDir || (CNavigation::INTERSECTDIRECTION::DOWN == eDir && 0.25f >= fabsf(fYDist))))
		{					
			if (Is_MoveInput() && Is_ShiftHoldTrigger())
			{
				m_eCurState = Sprint;
				m_ePrevState = Sprint;
				m_pModelBodyCom->Change_Animation(AnimState::Hand_Glid_Sprint_Roll);

				static_cast<CPlayer_Glider*>(m_pGlider)->Disable_Glider();
			}
			else if (Is_MoveInput() && !Is_ShiftHoldTrigger())
			{
				m_eCurState = Run;
				m_ePrevState = Run;
				m_pModelBodyCom->Change_Animation(AnimState::Hand_Glid_Roll);

				static_cast<CPlayer_Glider*>(m_pGlider)->Disable_Glider();
			}
			else if (AnimState::Hand_Glid_Start_Up != eCurrentAnimationState)
			{
				m_eCurState = Idle;
				m_ePrevState = Idle;
				m_pModelBodyCom->Change_Animation(AnimState::Hand_Jump_Down);;

				static_cast<CPlayer_Glider*>(m_pGlider)->Disable_Glider();
			}
			//else
			//{
			//	m_eCurState = Idle;
			//	m_ePrevState = Idle;
			//	m_pModelBodyCom->Change_Animation(AnimState::Hand_Jump_Down);;
			//}
		}

	}
	else if (PLAYER_STATE::Mounting_Jump == m_eCurState)
	{
		Vector4 vCurrentPlayerPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_float	fYDist;
		CNavigation::INTERSECTDIRECTION eDir = m_pNavigationCom->Get_YIntersectDistanceFromPoint(vCurrentPlayerPosition, fYDist);

		if (CNavigation::INTERSECTDIRECTION::UP == eDir)
		{
		
			if (Is_MoveInput())
			{
				m_pModelBodyCom->Change_Animation(AnimState::Hand_Mount_Fox_Run);
				static_cast<CPlayer_Mount*>(m_pMount)->Move();

				m_eCurState = PLAYER_STATE::Mounting_Move;
				m_ePrevState = PLAYER_STATE::Mounting_Move;
			}
			else
			{
				m_pModelBodyCom->Change_Animation(AnimState::Hand_Mount_Fox_Idle);
				static_cast<CPlayer_Mount*>(m_pMount)->Stand();

				m_eCurState = PLAYER_STATE::Mounting_Idle;
				m_ePrevState = PLAYER_STATE::Mounting_Idle;
			}

		}
		else if (CNavigation::INTERSECTDIRECTION::DOWN == eDir && 0.1f >= fYDist)
		{
			//m_eCurState = PLAYER_STATE::Mounting;
			//m_ePrevState = PLAYER_STATE::Mounting;
		
			if (Is_MoveInput())
			{
				m_pModelBodyCom->Change_Animation(AnimState::Hand_Mount_Fox_Run);
				static_cast<CPlayer_Mount*>(m_pMount)->Move();

				m_eCurState = PLAYER_STATE::Mounting_Move;
				m_ePrevState = PLAYER_STATE::Mounting_Move;
			}
			else
			{
				m_pModelBodyCom->Change_Animation(AnimState::Hand_Mount_Fox_Idle);
				static_cast<CPlayer_Mount*>(m_pMount)->Stand();

				m_eCurState = PLAYER_STATE::Mounting_Idle;
				m_ePrevState = PLAYER_STATE::Mounting_Idle;
		
			}
		}
	

	}
	else if (PLAYER_STATE::Hit_Throw_Air == m_eCurState)
	{
		CAnimationState* pCurrentAnimationState = m_pModelBodyCom->Get_Animator()->Get_CurAnimState();
		AnimState eCurrentAnimationState = pCurrentAnimationState->Get_eAnimStateTag();

		if (AnimState::Hand_FloatLoop == eCurrentAnimationState)
		{
			Vector4 vPlayerPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_float fYDist;
			CNavigation::INTERSECTDIRECTION eDir = m_pNavigationCom->Get_YIntersectDistanceFromPoint(vPlayerPosition, fYDist);

			if (CNavigation::INTERSECTDIRECTION::UP == eDir || 0.1f >= fYDist)
			{
				m_pModelBodyCom->Change_Animation(AnimState::Hand_FloatUp);
			}
		}
	}
	else if (PLAYER_STATE::Air_Attack == m_eCurState)
	{
		CAnimationState* pCurrentAnimationState = m_pModelBodyCom->Get_Animator()->Get_CurAnimState();
		AnimState eCurrentAnimationState = pCurrentAnimationState->Get_eAnimStateTag();

		if (AnimState::Dual_Gun_Upper_Kick_Air == eCurrentAnimationState && pCurrentAnimationState->Is_PlayDone())
		{
			m_eCurState = Jump;
			m_ePrevState = Jump;
			m_pPhysics->Set_Enable_Gravity(true);
			m_pPhysics->Set_Enable_Velocity(true);

			//Vector4 vCurrentVelocity = m_pPhysics->Get_Current_Velocity();
			//vCurrentVelocity.y = -1.f;
			//m_pPhysics->Set_Velocity(vCurrentVelocity);

			//m_pModelBodyCom->Get_Animator()->Change_Animation("char_h_jump_loop");
			m_pModelBodyCom->Change_Animation(AnimState::Hand_Jump_Loop);
		}
		else if (AnimState::Gun_Jump_Normal_Attack == eCurrentAnimationState && pCurrentAnimationState->Is_PlayDone())
		{
			Vector4 vPlayerPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_float fYDist;
			CNavigation::INTERSECTDIRECTION eDir = m_pNavigationCom->Get_YIntersectDistanceFromPoint(vPlayerPosition, fYDist);

			if (CNavigation::INTERSECTDIRECTION::DOWN == eDir && 0.5f <= fYDist)
			{
				m_eCurState = Jump;
				m_ePrevState = Jump;
				m_pModelBodyCom->Change_Animation(AnimState::Hand_Jump_Loop);
			}
			else
			{
				m_eCurState = Idle;
			}
		}
		else if (AnimState::Gun_Air_Jump_Normal_Attack == eCurrentAnimationState && pCurrentAnimationState->Is_PlayDone())
		{
			m_eCurState = Jump;
			m_ePrevState = Jump;
			m_pPhysics->Set_Enable_Gravity(true);
			m_pPhysics->Set_Enable_Velocity(true);

			m_pModelBodyCom->Change_Animation(AnimState::Hand_Jump_Loop);
		}

	}

	return S_OK;
}

_float CPlayer::Calc_PlayerLookAndCameraAngle()
{
	// To-Do : Need to immigrate to Game_Mode class
	_float fResult = 0.f;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	auto iter = pGameInstance->Get_SpecificLayer(m_eLevel)->find(L"Layer_Camera");
	auto end = pGameInstance->Get_SpecificLayer(m_eLevel)->end();

	if (end == iter)
		return fResult;

	CLayer* pLayer = iter->second;
	auto list = pLayer->GetObjectList();
	CCamera_TPS* pCamObject = dynamic_cast<CCamera_TPS*>(list.front());

	_vector vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_vector vCamLookAt = pCamObject->Get_LookAt();
	vCamLookAt = XMVectorSetY(vCamLookAt, 0.f) * -1;
	vCamLookAt = XMVector3Normalize(vCamLookAt);

	_vector vPlayerLookAt = XMVector3Normalize(XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_LOOK), 0.f));
	_vector vDot = XMVector3Dot(vCamLookAt, vPlayerLookAt);

	_vector vCross = XMVector3Cross(vPlayerLookAt, vCamLookAt);
	_float fY = XMVectorGetY(vCross);

	_float fDotResult = XMVectorGetX(vDot);

	if (fDotResult > 1.0f)
		fDotResult = 1.f;

	fResult = acosf(fDotResult);

	if (fY > 0)
		fResult *= +1;
	else
		fResult *= -1;

	return fResult;
}
*/
AttackDamageReation_ CPlayer::Take_Damage(const DAMAGEDESC& _tDamageDesc)
{
	if (m_bRollforwardDodgeWhenHit)
		return AttackDamageReation_::AttackDamageReaction_Block;	

	Sub_Hp(_tDamageDesc.fDamage);

	m_bHitWhenEating = true;

	Vector3 vPos = m_pTransformModelDrawCom->Get_State(CTransform::STATE_POSITION);
	vPos.x += (rand() % 101 - 50) / 2.f * 0.1;
	vPos.y += (rand() % 101 - 50) / 2.f * 0.1;
	vPos.z += (rand() % 101 - 50) / 2.f * 0.1;

	ElementType eElementType = _tDamageDesc.eElementType | ELEMENT_HIT_PLAYER;

	CFunc::Print_Damage(vPos, _tDamageDesc.fDamage, eElementType);

	/*
	if (BIT_ATTACK_NORMAL & _tDamageDesc.uDamageType)
	{
		if (PLAYER_STATE::Idle == m_eCurState)
		{
			m_eCurState = PLAYER_STATE::Hit_Light;
			m_ePrevState = PLAYER_STATE::Hit_Light;
			m_pModelBodyCom->Change_Animation(AnimState::Hand_LightHit);
		}
		_float fPlayerCenterY = 3.f;
		_float3 vRandEffectFixPos = { (-50 + rand() % 100) * 0.01f , fPlayerCenterY + (-50 + rand() % 100) * 0.02f , (-50 + rand() % 100) * 0.01f };
		Vector4 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		if (FAILED(Create_Hit_Effect(TEXT("Player_Hit_Client.Effect"), 2.f, vPos, vRandEffectFixPos)))
			MSG_BOX("Fail to Create Player Hit Effect");

		auto iIndex = CFunc::Generate_Random_Int(1, 4);
		string strAudioTag = "Player_Hit_" + to_string(iIndex);
		CGameInstance::Get_Instance()->Play_Audio(strAudioTag);
	}
	else if (BIT_ATTACK_HEAVY & _tDamageDesc.uDamageType)
	{
		Vector4 vDstPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		Vector4	vSrcPos = _tDamageDesc.vFromPosition;

		Vector4 vDir = vDstPos - vSrcPos;
		vDir.y = 0.f; vDir.w = 0.f; vDir.Normalize();

		m_vHitThrow = vDir;
		m_fHitThrowCurrentSpeed = m_fHitThrowStartSpeed;

		m_pTransformCom->Set_NewLook(-m_vHitThrow);

		Set_Hit_Throw_Ground_State();
		//Set_Hit_Throw_Air_State();

		_float fPlayerCenterY = 2.f;
		_float3 vRandEffectFixPos = { (-50 + rand() % 100) * 0.01f , fPlayerCenterY + (-50 + rand() % 100) * 0.01f , (-50 + rand() % 100) * 0.01f };
		Vector4 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		if (FAILED(Create_Hit_Effect(TEXT("Player_Hit_Big_Client.Effect"), 2.f, vPos, vRandEffectFixPos)))
			MSG_BOX("Fail to Create Player Hit Effect");

		auto iIndex = CFunc::Generate_Random_Int(1, 4);
		string strAudioTag = "Player_Hit_" + to_string(iIndex);
		CGameInstance::Get_Instance()->Play_Audio(strAudioTag);

	}
	return AttackDamageReaction_Hit;
}

void CPlayer::OnCollisionBegin(CCollider* _pSelf, CCollider* _pOther, _float _fTimeDelta)
{
	auto pOtherOwner = _pOther->Get_Owner();

	// Handling solid body collision with monster rigid body
	if (COLLISION_GROUP::PLAYER_BODY == _pSelf->Get_CollisionGroup() &&
		COLLISION_GROUP::MONSTER_BODY == _pOther->Get_CollisionGroup())
	{
		CTransform* pMonsterTransformCom = static_cast<CTransform*>(pOtherOwner->Find_Component(L"Com_Transform"));
		Vector4 vMonsterCurrentPosition = pMonsterTransformCom->Get_State(CTransform::STATE_POSITION);
		Vector4 vPlayerCurrentPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		vMonsterCurrentPosition.y = vPlayerCurrentPosition.y;
		

		_float fPlayerColliderRadius = _pSelf->Get_Radius();
		_float fMonsterColliderRadius = _pOther->Get_Radius();

		Vector4 vTowardPlayer = vPlayerCurrentPosition - vMonsterCurrentPosition;
		_float fDistance = vTowardPlayer.Length();
		vTowardPlayer.Normalize();

		_float fDifferenceDistance = fDistance - (fPlayerColliderRadius + fMonsterColliderRadius);



		if (0.f > fDifferenceDistance)
		{
			Vector4 vPlayerNewPosition = vPlayerCurrentPosition;

			CAnimator*			pAnimator = m_pModelBodyCom->Get_Animator();
			CAnimationState*	pCurrentAnimationState = pAnimator->Get_CurAnimState();
			AnimState			eCurrentAnimationState = pCurrentAnimationState->Get_eAnimStateTag();
			_float				fTrackPosition = pAnimator->Get_TrackPosition();

			if ((PLAYER_WEAPON_MODE::LINGHAN == m_eWeaponMode && Normal_Atk == m_eCurState) ||
				(PLAYER_WEAPON_MODE::DUALGUN == m_eWeaponMode && AnimState::Dual_Gun_Upper_Kick_Air == eCurrentAnimationState && !pCurrentAnimationState->Is_Transition() && 20.f >= fTrackPosition))
			{
				vPlayerNewPosition = vPlayerCurrentPosition + vTowardPlayer * fabsf(fDifferenceDistance);

				if (PLAYER_WEAPON_MODE::DUALGUN == m_eWeaponMode && AnimState::Dual_Gun_Upper_Kick_Air == eCurrentAnimationState)
				{
					m_bUpperKickMoveStop = true;
				}
			}
			else
			{
				vPlayerNewPosition = vPlayerCurrentPosition + vTowardPlayer * min(fabsf(fDifferenceDistance) * _fTimeDelta * 9.f, fabsf(fDifferenceDistance));
			}

			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPlayerNewPosition, m_pNavigationCom);
		}
	}
	// Handling time illusion check
	else if (COLLISION_GROUP::PLAYER_HIT_AREA_MARGIN == _pSelf->Get_CollisionGroup() &&
			 COLLISION_GROUP::MONSTER_PREPARE_ATTACK_CHECK == _pOther->Get_CollisionGroup() &&
			 m_bDashForTimeillusion && 
			 0.f >= m_fCurrentCoolTimeForTimeillusion)
	{
		CTimeillusion::SKILLOBJECTDESC tSkillObjectDesc;
		tSkillObjectDesc.pOwner = this;
		tSkillObjectDesc.vBatchPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		CGameInstance::Get_Instance()->Add_GameObject(m_eLevel, L"Layer_Player_SkillObject", L"Prototype_GameObject_SkillObject_Timeillusion", &tSkillObjectDesc);
		m_fCurrentCoolTimeForTimeillusion = m_fCoolTimeForTimeillusion;
	}
	// Handling additional when hit by monster
	else if (COLLISION_GROUP::PLAYER_HIT_BODY == _pSelf->Get_CollisionGroup() &&
			 COLLISION_GROUP::MONSTER_MELEE_WEAPON == _pOther->Get_CollisionGroup())
	{
		CGameInstance::Get_Instance()->Get_Usual_Function_In_Level(MARKED_FUNCTION_TAG::CAMERA_SHAKING_PLAYER_NORMAL_HIT)(nullptr);
	}
	// Handling player hit to monster body
	else if (COLLISION_GROUP::PLAYER_MELEE_WEAPON == _pSelf->Get_CollisionGroup() &&
			 COLLISION_GROUP::MONSTER_HIT_BODY == _pOther->Get_CollisionGroup())
	{
		AnimState eCurrentAnimationState = m_pModelBodyCom->Get_Animator()->Get_CurAnimState()->Get_eAnimStateTag();
		// 특정 공격에서는 애니메이션을 멈추지 않음.

		if (m_fCurrent_Hit_WaitTime <= 0.f && 
			AnimState::Sickle_Melee_Attack03 != eCurrentAnimationState &&
			AnimState::Sickle_Melee_Air_Attack04 != eCurrentAnimationState)
		{
			m_fCurrent_Hit_WaitTime = 0.048f;
		}

		auto resultDamage = m_pCurWeapon->Calculate_Damage(m_fBasic_Damage);

		DAMAGEDESC tDamageDesc;
		tDamageDesc.fDamage = resultDamage.first;
		tDamageDesc.uDamageType = m_ulAttackType;
		tDamageDesc.uWeaponType = (_uint)m_eWeaponMode;
		tDamageDesc.vWeaponRight = m_pCurWeapon->Get_RightWeapon_TransformState(CTransform::STATE_RIGHT);
		tDamageDesc.vWeaponUp = m_pCurWeapon->Get_RightWeapon_TransformState(CTransform::STATE_UP);
		tDamageDesc.eElementType = m_pCurWeapon->Get_Element();

		tDamageDesc.eElementType |= resultDamage.second ? ELEMENT_HIT_WEAK : ELEMENT_NONE;

		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_float3 vMonsterCollPos = _pOther->Get_Position();
		_float3 vMonsterCollSize = _pOther->Get_Size();
		_vector vDir = XMVector3Normalize(vPos - XMLoadFloat3(&vMonsterCollPos));
				
		tDamageDesc.vEffectPos = XMLoadFloat3(&vMonsterCollPos) + vDir * vMonsterCollSize.x * 1.f;
		tDamageDesc.vEffectPos.y = XMVectorGetY(m_pCurWeapon->Get_RightWeapon_TransformState(CTransform::STATE_POSITION));
		tDamageDesc.vEffectPos.w = 1.f;
		
		AttackDamageReation_ iReaction = static_cast<CCharacter*>(pOtherOwner)->Take_Damage(tDamageDesc);

		// 무기별 타격 사운드 재생
		m_pCurWeapon->Play_HitSound(pOtherOwner->Get_ObjectID());

		if (AttackDamageReaction_Dead == iReaction)
			CQuest_Manager::Get_Instance()->Notify(pOtherOwner->Get_ObjectID(), 1);

		CGameInstance* pGameInstance = CGameInstance::Get_Instance();

		if (PLAYER_WEAPON_MODE::SICKLE == m_eWeaponMode)
		{
			pGameInstance->Get_Usual_Function_In_Level(MARKED_FUNCTION_TAG::CAMERA_SHAKING_PLAYER_SICKLE_ATTACK)(nullptr);
		}
		else if (PLAYER_WEAPON_MODE::GUN == m_eWeaponMode && 0 != (BIT_ATTACK_MELEE & m_ulAttackType))
		{
			pGameInstance->Get_Usual_Function_In_Level(MARKED_FUNCTION_TAG::CAMERA_SHAKING_PLAYER_DUALGUN_MELEE)(nullptr);
		}
		else if (PLAYER_WEAPON_MODE::LINGHAN == m_eWeaponMode)
		{

		}

		CAnimationState* pCurAnimState = m_pModelBodyCom->Get_Animator()->Get_CurAnimState();
		AnimState			eCurAnimState = m_pModelBodyCom->Get_Animator()->Get_CurAnimState()->Get_eAnimStateTag();
		if (AnimState::Sickle_Melee_Attack05 == eCurAnimState ||
			AnimState::Sickle_Melee_Air_Attack01 == eCurAnimState ||
			AnimState::Sickle_Melee_Air_Attack02 == eCurAnimState ||
			AnimState::Sickle_Melee_Air_Attack03 == eCurAnimState ||
			AnimState::Sickle_Melee_Air_Attack04 == eCurAnimState)
		{
			Add_CapturedMonster(static_cast<CMonster*>(pOtherOwner));
		}

		// test
		Add_WeaponSwapGauge(m_fWeaponSwapMaxGauge * 0.01f);
	}
	else if (COLLISION_GROUP::PLAYER_BODY == _pSelf->Get_CollisionGroup() &&
		COLLISION_GROUP::PLAYER_SKILL_AREA == _pOther->Get_CollisionGroup())
	{
		if (SKILLTYPE::TIME_ILLUSION == static_cast<CSkillObject*>(_pOther->Get_Owner())->Get_Skill_Type())
		{
			m_pPlayerBodyHitColliderCom->Set_Enable(false);
		}
	}
	else if (COLLISION_GROUP::PLAYER_MELEE_WEAPON == _pSelf->Get_CollisionGroup()&&
			 COLLISION_GROUP::MINI_GAME_INTERACTION == _pOther->Get_CollisionGroup())
		{
			CMiniGame_Elemental_Cube::CUBEINTERACTIONDESC tInteractionDesc;
			tInteractionDesc.eElementalType = (ELEMENTAL_TYPE)m_pCurWeapon->Get_Element();
			tInteractionDesc.pPlayer = this;
			static_cast<CMiniGame_Elemental_Cube*>(pOtherOwner)->Take_Interaction(&tInteractionDesc);
		}
		*/
	else if (COLLISION_GROUP::PLAYER_INTERACTION == _pSelf->Get_CollisionGroup()&&
			 COLLISION_GROUP::NPC_INTERACTION == _pOther->Get_CollisionGroup())
	{
		if (CFunc::Is_KeyDown(DIK_L))
			static_cast<CCharacter_NPC*>(pOtherOwner)->Interaction();
	}

}

/*
void CPlayer::OnCollision(CCollider* _pSelf, CCollider* _pOther, _float _fTimeDelta)
{
	auto pOtherOwner = _pOther->Get_Owner();
	auto eOtherOwnerType = pOtherOwner->Get_ObjectType();

	if (COLLISION_GROUP::PLAYER_BODY == _pSelf->Get_CollisionGroup() &&
		COLLISION_GROUP::MONSTER_BODY == _pOther->Get_CollisionGroup())
	{
		Vector4 vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		//vPlayerPos.y = 0.f;

		CTransform* pMonsterTransform = static_cast<CTransform*>(pOtherOwner->Find_Component(L"Com_Transform"));
		Vector4 vMonsterPos = pMonsterTransform->Get_State(CTransform::STATE_POSITION);
		//vMonsterPos.y = 0.f;

		_float fPlayerRadius = _pSelf->Get_Radius();
		_float fMonsterRadius = _pOther->Get_Radius();

		Vector4 vDir = vPlayerPos - vMonsterPos;

		Vector4 vNormalized = vDir; vNormalized.Normalize();
		_float fDist = vDir.Length();
		_float fDiff = fDist - (fPlayerRadius + fMonsterRadius);

		if (fDiff < 0.f)
		{
			CAnimator* pAnimator = m_pModelBodyCom->Get_Animator();
			CAnimationState* pCurrentAnimationState = pAnimator->Get_CurAnimState();
			AnimState			eCurrentAnimationState = pCurrentAnimationState->Get_eAnimStateTag();
			_float				fTrackPosition = pAnimator->Get_TrackPosition();


			Vector4 vNewPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

			if ( (PLAYER_WEAPON_MODE::LINGHAN == m_eWeaponMode && Normal_Atk == m_eCurState)||
				(PLAYER_WEAPON_MODE::DUALGUN == m_eWeaponMode && AnimState::Dual_Gun_Upper_Kick_Air == eCurrentAnimationState && !pCurrentAnimationState->Is_Transition() && 20.f >= fTrackPosition))
			{
				vNewPos = vNewPos + vNormalized * fabsf(fDiff);
			}
			else
			{
				vNewPos += vNormalized * min(fabsf(fDiff) * _fTimeDelta * 9.f, fabsf(fDiff));		
			}

			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vNewPos, m_pNavigationCom);
		}

	}
	*/
	else if (COLLISION_GROUP::INTERACTION == _pOther->Get_CollisionGroup())
	{
		if (CFunc::Is_KeyDown(DIK_F))
		{
			if (OBJECT_TYPE::NPC == eOtherOwnerType)
				static_cast<CCharacter_NPC*>(pOtherOwner)->Interaction();

			else if (OBJECT_TYPE::INTERACTION == eOtherOwnerType)
				static_cast<CInteractionObject*>(pOtherOwner)->Interaction();
		}
	}
	/*
	else if (COLLISION_GROUP::PLAYER_BODY == _pSelf->Get_CollisionGroup() &&
		COLLISION_GROUP::PLAYER_SKILL_AREA == _pOther->Get_CollisionGroup())
	{
		if (SKILLTYPE::TIME_ILLUSION == static_cast<CSkillObject*>(_pOther->Get_Owner())->Get_Skill_Type())
		{
			m_pPlayerBodyHitColliderCom->Set_Enable(false);
		}
	}
	else if (COLLISION_GROUP::PLAYER_HIT_AREA_MARGIN == _pSelf->Get_CollisionGroup() &&
		COLLISION_GROUP::MONSTER_PREPARE_ATTACK_CHECK == _pOther->Get_CollisionGroup() &&
		m_bDashForTimeillusion &&
		0.f >= m_fCurrentCoolTimeForTimeillusion)
	{
			CTimeillusion::SKILLOBJECTDESC tSkillObjectDesc;
			tSkillObjectDesc.pOwner = this;
			tSkillObjectDesc.vBatchPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			CGameInstance::Get_Instance()->Add_GameObject(m_eLevel, L"Layer_Player_SkillObject", L"Prototype_GameObject_SkillObject_Timeillusion", &tSkillObjectDesc);
			m_fCurrentCoolTimeForTimeillusion = m_fCoolTimeForTimeillusion;
	}
	//else if (COLLISION_GROUP::PLAYER_INTERACTION == _pSelf->Get_CollisionGroup() &&
	//	COLLISION_GROUP::NPC_INTERACTION == _pOther->Get_CollisionGroup())
	//{
	//	if (CFunc::Is_KeyDown(DIK_L))
	//	{
	//		static_cast<CCharacter_NPC*>(pOtherOwner)->Interaction();
	//	}
	//}
}

void CPlayer::OnCollisionEnd(CCollider* _pSelf, CCollider* _pOther, _float _fTimeDelta)
{
	if (COLLISION_GROUP::PLAYER_BODY == _pSelf->Get_CollisionGroup() &&
		COLLISION_GROUP::PLAYER_SKILL_AREA == _pOther->Get_CollisionGroup())
		{
			if (SKILLTYPE::TIME_ILLUSION == static_cast<CSkillObject*>(_pOther->Get_Owner())->Get_Skill_Type())
			{
				m_pPlayerBodyHitColliderCom->Set_Enable(true);
			}
	}
}

void CPlayer::Add_CapturedMonster(CMonster* _pMonster)
{
	for (auto& pMonster : m_setCapturedMonster)
	{
		if (_pMonster == pMonster)
		{
			_pMonster->Set_CapturedCoolTime(2.f);
			_pMonster->Set_Captured(true);
			return;
		}
	}

	_pMonster->Set_CapturedCoolTime(2.f);
	_pMonster->Set_Captured(true);
	m_setCapturedMonster.insert(_pMonster);

}

void CPlayer::Auto_Targeting(Vector4 _vTargetPos)
{
	Vector4 vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vTargetPos.y = vPlayerPos.y;
	m_pTransformCom->LookAt(_vTargetPos);
}

void CPlayer::Auto_Targeting()
{
	_float4 vPos = { 0.f, 0.f, 0.f, 1.f };
	Vector4 vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (Find_Nearest_Monster(&vPos))
	{
		Vector4 vTargetPos = vPos;
		vTargetPos.y = vPlayerPos.y;
		m_pTransformCom->LookAt(vTargetPos);
	}
}

_bool CPlayer::Find_Nearest_Monster(_float4* _pOutPos)
{
	// To-Do : Need to immigrate to Game_Mode class

	_float fResultLength = FLT_MAX;
	Vector4 vNearestMonsterPos = Vector4(0.f, 0.f, 0.f, 1.f);


	CGameInstance* pGameInstance = CGameInstance::Get_Instance();


	auto pMapLayer = pGameInstance->Get_SpecificLayer(m_eLevel);
	auto iter = pMapLayer->find(L"Layer_Monster");

	if (pMapLayer->end() == iter)
	{
		return false;
	}

	list<CGameObject*> pObjList = iter->second->GetObjectList();

	Vector4 vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	for (auto& pObj : pObjList)
	{
		CMonsterNormal* pMonster = static_cast<CMonsterNormal*>(pObj);
		CTransform* pMonsterTransformCom = static_cast<CTransform*>(pMonster->Find_Component(L"Com_Transform"));
		Vector4 vMonsterPos = pMonsterTransformCom->Get_State(CTransform::STATE_POSITION);
		vMonsterPos.y = vPlayerPos.y;


		_vector vDir = vMonsterPos - vPlayerPos;
		_float fCalcLength = XMVectorGetX(XMVector3Length(vDir));

		if (fResultLength > fCalcLength && !pMonster->Is_MonsterBodyDead() && m_fAutoTargetingLimitDistance >= fCalcLength) //&& !pMonster->Get_Dead() && pMonster->Get_HP() > 0.f)
		{
			fResultLength = fCalcLength;
			vNearestMonsterPos = vMonsterPos;
		}

	}



	if (FLT_MAX != fResultLength) //&& m_fMaxEnenmyRange >= fLength)
	{
		*_pOutPos = vNearestMonsterPos;
		return true;
	}
	else
	{
		return false;
	}
}

HRESULT			CPlayer::Camera_To_ADS()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	auto iter = pGameInstance->Get_SpecificLayer(m_eLevel)->find(L"Layer_Camera");
	auto end = pGameInstance->Get_SpecificLayer(m_eLevel)->end();

	CAnimator* pAnimator = m_pModelBodyCom->Get_Animator();


	if (end != iter)
	{
		// 1. 카메라가 바라보는 방향으로 캐릭터 회전 시키기
		Matrix matView = pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_VIEW);
		Vector3 vLook = matView.Backward(); vLook.y = 0.f; vLook.Normalize();
		m_fCurrentRoationVelocity = 40.f;
		m_bSmoothRotation = true;
		m_vTargetLook = vLook;


		// 2. 카메라가 바라봐야 할 위치(at)와 eye위치 구하기.
		//	  Eye의 위치는 transform 기준으로
		//Vector3 vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		Vector3 vCurrentPlayerPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		m_vADS_Camera_At = vCurrentPlayerPosition;
		m_vADS_Camera_Eye = vCurrentPlayerPosition * vLook;


		// 3. Send camera event message
		CAMERAEVENTMESSAGE	tCamMsg;
		CAMERAEVENTMOVEDESC	tCameraMoveData	;
		tCamMsg.eCameraEventType = (_int)CAMERA_EVENT_TYPE::TRANSITION;
		tCameraMoveData.eMoveType = (_int)TRANSLATION_TRANSITION_TYPE::ADS;
		tCameraMoveData.bStickPosition = true;
		tCameraMoveData.pStickPosition = &m_vADS_Camera_At;
		tCameraMoveData.vEyePosition = m_vADS_Camera_Eye;
		tCameraMoveData.vAtPosition = m_vADS_Camera_At;
		tCameraMoveData.fMoveTime = 0.25f;
		tCameraMoveData.fMoveVelocity = 7.f;
		tCameraMoveData.fDestinationFov = XMConvertToRadians(CAM_FOV_ADS);
		tCamMsg.tCameraMoveDesc = tCameraMoveData;
	
		CLayer* pLayer = iter->second;
		auto list = pLayer->GetObjectList();
		CCamera_TPS* pCamObject = static_cast<CCamera_TPS*>(list.front());

		pCamObject->Add_Event_Message(&tCamMsg);
		pCamObject->Change_Camera_Arm_Length(7.5f, 0.1f);

		m_eViewMode = ViewMode::ADS;
	}

	return S_OK;
}

HRESULT			CPlayer::Set_Camera_To_Stick_Player(_float _fTransitionTime)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	auto iter = pGameInstance->Get_SpecificLayer(m_eLevel)->find(L"Layer_Camera");
	auto end = pGameInstance->Get_SpecificLayer(m_eLevel)->end();

	CAnimator* pAnimator = m_pModelBodyCom->Get_Animator();

	Vector3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_vADS_Camera_At = vPos;

	if (end != iter)
	{
		CAMERAEVENTMESSAGE	tCamMsg;
		CAMERAEVENTMOVEDESC	tCameraMoveData;
		tCamMsg.eCameraEventType = (_int)CAMERA_EVENT_TYPE::TRANSITION;
		tCameraMoveData.eMoveType = (_int)TRANSLATION_TRANSITION_TYPE::TPS;
		tCameraMoveData.bStickPosition = false;
		tCameraMoveData.pStickPosition = &m_vADS_Camera_At;
		tCameraMoveData.vEyePosition = m_vADS_Camera_Eye;
		tCameraMoveData.vAtPosition = m_vADS_Camera_At;
		tCameraMoveData.fArmLength = 10.f;
		tCameraMoveData.fMoveTime = _fTransitionTime;
		tCameraMoveData.fMoveVelocity = 100.f;
		tCameraMoveData.fDestinationFov = XMConvertToRadians(CAM_FOV_NORMAL);
		tCamMsg.tCameraMoveDesc = tCameraMoveData;

		CLayer* pLayer = iter->second;
		auto list = pLayer->GetObjectList();
		CCamera_TPS* pCamObject = dynamic_cast<CCamera_TPS*>(list.front());

		pCamObject->Add_Event_Message(&tCamMsg);
		pCamObject->Change_Camera_Arm_Length(12.f, 0.1f);

		m_eViewMode = ViewMode::TPS;
	}

	return S_OK;
}

HRESULT			CPlayer::Camera_To_Original()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	auto iter = pGameInstance->Get_SpecificLayer(m_eLevel)->find(L"Layer_Camera");
	auto end = pGameInstance->Get_SpecificLayer(m_eLevel)->end();

	CAnimator* pAnimator = m_pModelBodyCom->Get_Animator();


	if (end != iter)
	{
		CAMERAEVENTMESSAGE	tCamMsg;
		CAMERAEVENTMOVEDESC	tCameraMoveData;
		tCamMsg.eCameraEventType = (_int)CAMERA_EVENT_TYPE::TRANSITION;
		tCameraMoveData.eMoveType = (_int)TRANSLATION_TRANSITION_TYPE::TPS;
		tCameraMoveData.bStickPosition = false;
		tCameraMoveData.pStickPosition = &m_vADS_Camera_At;
		tCameraMoveData.vEyePosition = m_vADS_Camera_Eye;
		tCameraMoveData.vAtPosition = m_vADS_Camera_At;
		tCameraMoveData.fArmLength = 10.f;
		tCameraMoveData.fMoveTime = 0.25f;
		tCameraMoveData.fMoveVelocity = 7.f;
		tCameraMoveData.fDestinationFov = XMConvertToRadians(CAM_FOV_NORMAL);
		tCamMsg.tCameraMoveDesc = tCameraMoveData;

		CLayer* pLayer = iter->second;
		auto list = pLayer->GetObjectList();
		CCamera_TPS* pCamObject = dynamic_cast<CCamera_TPS*>(list.front());

		pCamObject->Add_Event_Message(&tCamMsg);
		pCamObject->Change_Camera_Arm_Length(12.f, 0.1f);

		m_eViewMode = ViewMode::TPS;
	}

	return S_OK;
}

HRESULT CPlayer::Update_AnimByPlayerState()
{
	CAnimator* pAnimator = m_pModelBodyCom->Get_Animator();

	if (m_eCurState != m_ePrevState)
	{
		if (PLAYER_STATE::Idle == m_eCurState)
		{
			if (pAnimator->Find_AnimIdx("char_h_stand_pe_f03") != pAnimator->Get_DstAnimIdx())
			{
				m_pModelBodyCom->Change_Animation(AnimState::Hand_Stand);
			}
		}
		else if (PLAYER_STATE::Run == m_eCurState)
		{
			m_pModelBodyCom->Change_Animation(AnimState::Hand_Run);
		}
		else if (PLAYER_STATE::Sprint == m_eCurState)
		{
			m_pModelBodyCom->Change_Animation(AnimState::Hand_Sprint);
		}
		else if (PLAYER_STATE::DASH_BACK == m_eCurState)
		{
			m_pModelBodyCom->Change_Animation(AnimState::Hand_RollBack);
		}
		else if (PLAYER_STATE::DASH_FRONT == m_eCurState)
		{
			m_pModelBodyCom->Change_Animation(AnimState::Hand_RollForward);
		}
		else if (PLAYER_STATE::Jump == m_eCurState)
		{
			if (PLAYER_STATE::Run == m_ePrevState ||
				PLAYER_STATE::Sprint == m_ePrevState ||
				PLAYER_STATE::DASH_FRONT == m_ePrevState)
			{
				m_pModelBodyCom->Change_Animation(AnimState::Hand_Jump_Fall_Forward);
			}
			else
			{
				m_pModelBodyCom->Change_Animation(AnimState::Hand_Jump);
			}

		}
		else if (PLAYER_STATE::Jump_Air == m_eCurState)
		{
			m_pModelBodyCom->Change_Animation(AnimState::Hand_Jump_Air);
		}
		else if (PLAYER_STATE::Dash_Front_Air == m_eCurState)
		{
			m_pModelBodyCom->Change_Animation(AnimState::Hand_RollForwardAir);
		}
		else if (PLAYER_STATE::Dash_Back_Air == m_eCurState)
		{
			m_pModelBodyCom->Change_Animation(AnimState::Hand_RollBackAir);
		}


		m_ePrevState = m_eCurState;

	}

	return S_OK;
}

HRESULT CPlayer::Update_PlayerTurn(_float _fTimeDelta)
{
	if (//PLAYER_STATE::Normal_Atk == m_eCurState ||
		PLAYER_STATE::Skill_Atk == m_eCurState ||
		PLAYER_STATE::Hit_Throw_Ground == m_eCurState ||
		PLAYER_STATE::Hit_Throw_Air == m_eCurState ||
		PLAYER_STATE::Hit_Light == m_eCurState ||
		PLAYER_STATE::Aimming == m_eCurState ||
		PLAYER_STATE::Aimming_Move == m_eCurState ||
		PLAYER_STATE::Aimming_Move_Attack == m_eCurState ||
		PLAYER_STATE::Aimming_Stand_Attack == m_eCurState ||
		PLAYER_STATE::DASH_BACK == m_eCurState ||
		PLAYER_STATE::Dash_Back_Air == m_eCurState ||
		PLAYER_STATE::Falling_Attack == m_eCurState ||
		PLAYER_STATE::Gliding_Charge == m_eCurState)
	{
		return S_OK;
	}



	// 일부 공중 공격 또는 특정 모션은 turn적용 X
	AnimState eCurrentAnimationState = m_pModelBodyCom->Get_Animator()->Get_CurAnimState()->Get_eAnimStateTag();
	
	if (AnimState::Gun_Air_Jump_Normal_Attack == eCurrentAnimationState ||
		AnimState::Gun_Jump_Normal_Attack == eCurrentAnimationState ||
		AnimState::Hand_Mount_Fox_up == eCurrentAnimationState)
	{
		return S_OK;
	}

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (PLAYER_STATE::Skill_Atk != m_eCurState)
	{
		if ((CFunc::Is_KeyDown(DIK_W) || CFunc::Is_KeyHold(DIK_W)) &&
			(CFunc::Is_KeyDown(DIK_D) || CFunc::Is_KeyHold(DIK_D)))
		{
			_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(-45.f));
			_matrix matViewInv = XMMatrixInverse(nullptr, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
			_vector vNewLook = XMVector3Transform(XMVector3Normalize(XMVectorSetY(matViewInv.r[0], 0.f)), matRotY) * 1.f;

			m_fCurrentRoationVelocity = 10.f;
			m_bSmoothRotation = true;
			m_vTargetLook = vNewLook;

		}
		else if ((CFunc::Is_KeyDown(DIK_W) || CFunc::Is_KeyHold(DIK_W)) &&
				 (CFunc::Is_KeyDown(DIK_A) || CFunc::Is_KeyHold(DIK_A)))
		{
			_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(45.f));
			_matrix matViewInv = XMMatrixInverse(nullptr, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
			_vector vNewLook = XMVector3Transform(XMVector3Normalize(XMVectorSetY(matViewInv.r[0], 0.f)), matRotY) * -1.f;

			m_fCurrentRoationVelocity = 10.f;
			m_bSmoothRotation = true;
			m_vTargetLook = vNewLook;
		}
		else if ((CFunc::Is_KeyDown(DIK_S) || CFunc::Is_KeyHold(DIK_S)) &&
			(CFunc::Is_KeyDown(DIK_A) || CFunc::Is_KeyHold(DIK_A)))
		{
			_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(-45.f));
			_matrix matViewInv = XMMatrixInverse(nullptr, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
			_vector vNewLook = XMVector3Transform(XMVector3Normalize(XMVectorSetY(matViewInv.r[0], 0.f)), matRotY) * -1.f;

			m_fCurrentRoationVelocity = 10.f;
			m_bSmoothRotation = true;
			m_vTargetLook = vNewLook;
		}
		else if ((CFunc::Is_KeyDown(DIK_S) || CFunc::Is_KeyHold(DIK_S)) &&
			(CFunc::Is_KeyDown(DIK_D) || CFunc::Is_KeyHold(DIK_D)))
		{
			_matrix matRotY = XMMatrixRotationY(XMConvertToRadians(45.f));
			_matrix matViewInv = XMMatrixInverse(nullptr, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
			_vector vNewLook = XMVector3Transform(XMVector3Normalize(XMVectorSetY(matViewInv.r[0], 0.f)), matRotY);

			m_fCurrentRoationVelocity = 10.f;
			m_bSmoothRotation = true;
			m_vTargetLook = vNewLook;

		}
		else if (CFunc::Is_KeyDown(DIK_W) || CFunc::Is_KeyHold(DIK_W))
		{
			_matrix matViewInv = XMMatrixInverse(nullptr, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
			_vector vNewLook = XMVector3Normalize(XMVectorSetY(matViewInv.r[2], 0.f));

			m_fCurrentRoationVelocity = 10.f;
			m_bSmoothRotation = true;
			m_vTargetLook = vNewLook;
		}
		else if (CFunc::Is_KeyDown(DIK_A) || CFunc::Is_KeyHold(DIK_A))
		{
			_matrix matViewInv = XMMatrixInverse(nullptr, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
			_vector vNewLook = XMVector3Normalize(XMVectorSetY(matViewInv.r[0], 0.f)) * -1.f;

			m_fCurrentRoationVelocity = 10.f;
			m_bSmoothRotation = true;
			m_vTargetLook = vNewLook;
		}
		else if (CFunc::Is_KeyDown(DIK_D) || CFunc::Is_KeyHold(DIK_D))
		{
			_matrix matViewInv = XMMatrixInverse(nullptr, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
			_vector vNewLook = XMVector3Normalize(XMVectorSetY(matViewInv.r[0], 0.f));

			m_fCurrentRoationVelocity = 10.f;
			m_bSmoothRotation = true;
			m_vTargetLook = vNewLook;

		}
		else if (CFunc::Is_KeyDown(DIK_S) || CFunc::Is_KeyHold(DIK_S))
		{
			_matrix matViewInv = XMMatrixInverse(nullptr, pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
			_vector vNewLook = XMVector3Normalize(XMVectorSetY(matViewInv.r[2], 0.f)) * -1.f;

			m_fCurrentRoationVelocity = 10.f;
			m_bSmoothRotation = true;
			m_vTargetLook = vNewLook;
		}

		else if (KEY_STATE::TAP == pGameInstance->Get_KeyState(DIK_LEFTARROW) || KEY_STATE::HOLD == pGameInstance->Get_KeyState(DIK_LEFTARROW))
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), -_fTimeDelta);
		}

		else if (KEY_STATE::TAP == pGameInstance->Get_KeyState(DIK_RIGHTARROW) || KEY_STATE::HOLD == pGameInstance->Get_KeyState(DIK_RIGHTARROW))
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), _fTimeDelta);
		}
	}


	if (m_bSmoothRotation)
	{
		if (XMVectorGetW(m_vTargetLook) != 0.f)
		{
			m_vTargetLook = XMVectorSetW(m_vTargetLook, 0.f);
		}

		Vector4 vNormalizedLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		vNormalizedLook.y = 0.f;
		vNormalizedLook.Normalize();

		_float fMaxTurnSpeed = 50.f;
		if (PLAYER_STATE::Gliding == m_eCurState)
		{
			fMaxTurnSpeed = 0.25f;
		}

		Vector4 vTargetLook = SmoothDampDirectionalVector({ 0.f, 1.f, 0.f, 0.f },
			vNormalizedLook, m_vTargetLook, m_fCurrentRoationVelocity, m_fCurrentTurnLimitTime, fMaxTurnSpeed, _fTimeDelta);

		vTargetLook.y = 0.f;
		vTargetLook.Normalize();

		m_pTransformCom->Set_NewLook(vTargetLook);

		Vector4 vNewLookNormalized = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		vNewLookNormalized.y = 0.f;
		vNewLookNormalized.Normalize();

		_float fDegAfterRot = Get_AngleDeg_BetweenVectors(vNewLookNormalized, m_vTargetLook);
	
		_float fThreadshold = 0.01f;
		if (PLAYER_STATE::Gliding == m_eCurState)
		{
			fThreadshold = 0.08f;
		}


		//if (fabsf(fDegAfterRot) < fThreadshold)
		//{
		//	m_bSmoothRotation = false;
		//}
		// 이게 문제 일으킬 수 있으려나
		// <- w is negative one in some occasion.

		if (fabsf(fDegAfterRot) < fThreadshold)
		{
			m_bSmoothRotation = false;
			//Vector4 vSetNewLook = m_vTargetLook;
			//vSetNewLook.w = 0.f;
			//vSetNewLook.Normalize();
			//m_pTransformCom->Set_NewLook(vSetNewLook);
			m_pTransformCom->Set_NewLook(m_vTargetLook);
		}
	}

	return S_OK;
}

_bool CPlayer::Is_MoveInput()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if (((CFunc::Is_KeyDown(DIK_W) || CFunc::Is_KeyHold(DIK_W))) ||
		((CFunc::Is_KeyDown(DIK_A) || CFunc::Is_KeyHold(DIK_A))) ||
		((CFunc::Is_KeyDown(DIK_D) || CFunc::Is_KeyHold(DIK_D))) ||
		((CFunc::Is_KeyDown(DIK_S) || CFunc::Is_KeyHold(DIK_S))))
	{
		return true;
	}

	return false;
}

_bool CPlayer::Is_ShiftHoldTrigger()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if (((CFunc::Is_KeyHold(DIK_LSHIFT))))
	{
		return true;
	}

	return false;
}

_bool CPlayer::Is_ShiftTapTrigger()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if (((CFunc::Is_KeyDown(DIK_LSHIFT))))
	{
		return true;
	}

	return false;
}

void CPlayer::Reset_Hit_Throw_State()
{
	m_fHitThrowAccTime = 0.f;
	m_fHitThrowCurrentSpeed = 0.f;
}

void CPlayer::Handling_HitThrow(_float _fTimeDelta)
{
	CAnimator* pAnimator = m_pModelBodyCom->Get_Animator();

	CAnimationState* pCurrentAnimationState = pAnimator->Get_CurAnimState();
	AnimState		 eCurrentAnimationState = pCurrentAnimationState->Get_eAnimStateTag();

	
	if (AnimState::Hand_Float == eCurrentAnimationState &&
		!pCurrentAnimationState->Is_PlayDone())
	{
		m_pModelBodyCom->Change_Animation(AnimState::Hand_FloatLoop);
	}
	else if (AnimState::Hand_Floating == eCurrentAnimationState &&
			 pCurrentAnimationState->Is_PlayDone())
	{
		m_pModelBodyCom->Change_Animation(AnimState::Hand_FloatLoop);
	}
	else if (AnimState::Hand_FloatUp == eCurrentAnimationState &&
			 pCurrentAnimationState->Is_PlayDone())
	{
		m_eCurState = PLAYER_STATE::Idle;
	}
	else if (AnimState::Hand_FloatUpAir == eCurrentAnimationState &&
			 pCurrentAnimationState->Is_PlayDone())
	{

	}
	else if (AnimState::Hand_HeavyHitStart == eCurrentAnimationState)
	{
		Vector4 vPlayerCurrentPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		vPlayerCurrentPosition += m_vHitThrow * m_fHitThrowCurrentSpeed * _fTimeDelta;

		m_fHitThrowCurrentSpeed -= 4.f * _fTimeDelta;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPlayerCurrentPosition, m_pNavigationCom);

		if (pCurrentAnimationState->Is_PlayDone())
		{
			m_fHitThrowAccTime = 0.f;
			m_pModelBodyCom->Change_Animation(AnimState::Hand_DiaupEnd);
		}
	}
	else if (AnimState::Hand_HeavyHitLoop == eCurrentAnimationState)
	{
		m_fHitThrowAccTime += _fTimeDelta;

		Vector4 vPlayerCurrentPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		vPlayerCurrentPosition += m_vHitThrow * m_fHitThrowCurrentSpeed * _fTimeDelta;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPlayerCurrentPosition, m_pNavigationCom);

		if (m_fHitThrowThreasholdTime <= m_fHitThrowAccTime)
		{
			m_fHitThrowAccTime = 0.f;
			m_pModelBodyCom->Change_Animation(AnimState::Hand_DiaupEnd);
		}
	}
	else if (AnimState::Hand_HeavyHitEnd == eCurrentAnimationState &&
		!pCurrentAnimationState->Is_Transition() && 
		!pCurrentAnimationState->Is_PlayDone())
	{
		_float fXZDelta = m_pModelBodyCom->Get_FirstRootBone_XZDelta();
		Vector4 vPlayerCurrentPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		vPlayerCurrentPosition += m_vHitThrow * fabsf(fXZDelta) * 0.03f;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPlayerCurrentPosition, m_pNavigationCom);
	}
	else if ((AnimState::Hand_HeavyHitEnd == eCurrentAnimationState ||
			  AnimState::Hand_DiaupEnd == eCurrentAnimationState ||
			  AnimState::Hand_LightHit == eCurrentAnimationState) &&
			  pCurrentAnimationState->Is_PlayDone())
	{
		m_eCurState = PLAYER_STATE::Idle;
	}
}
*/
void CPlayer::Set_Out_Weapon()
{	
	m_pCurWeapon->Out_Weapon();
}

void CPlayer::Set_In_Weapon()
{
	m_pCurWeapon->In_Weapon();
}
/*
void CPlayer::Set_Gliding_Ground_Charging_State()
{
	m_eCurState = PLAYER_STATE::Gliding;
	m_ePrevState = PLAYER_STATE::Gliding;

	Vector4 vVelocity = m_pPhysics->Get_Current_Velocity();
	vVelocity.y = m_fGliding_Y_Velocity_Ground + m_pControlModule->Get_Charging_Gauge_For_Glid();

	m_pPhysics->Set_Max_Falling_Velocity(m_fGliding_Y_Velocity_Ground);
	m_pPhysics->Set_Velocity(vVelocity);
}

void CPlayer::Set_Gliding_Run_Sprint_State()
{
	m_eCurState = PLAYER_STATE::Gliding;
	m_ePrevState = PLAYER_STATE::Gliding;

	Vector4 vVelocity = m_pPhysics->Get_Current_Velocity();
	vVelocity.y = m_fGliding_Y_Velocity_Run_Sprint;

	m_pPhysics->Set_Max_Falling_Velocity(m_fGliding_Y_Velocity_Ground);
	m_pPhysics->Set_Velocity(vVelocity);
}

void CPlayer::Set_Gliding_Open_Air_State()
{
	m_eCurState = PLAYER_STATE::Gliding;
	m_ePrevState = PLAYER_STATE::Gliding;

	Vector4 vVelocity = m_pPhysics->Get_Current_Velocity();
	vVelocity.y = m_fGliding_Y_Velocity_Open_Air;

	m_pPhysics->Set_Max_Falling_Velocity(m_fGliding_Y_Velocity_Ground);
	m_pPhysics->Set_Velocity(vVelocity);
}

void CPlayer::Set_Gliding_Jump_Air_State()
{
	m_eCurState = PLAYER_STATE::Gliding;
	m_ePrevState = PLAYER_STATE::Gliding;

	Vector4 vVelocity = m_pPhysics->Get_Current_Velocity();
	vVelocity.y = m_fGliding_Y_Velocity_Jump_Air;

	m_pPhysics->Set_Max_Falling_Velocity(m_fGliding_Y_Velocity_Ground);
	m_pPhysics->Set_Velocity(vVelocity);
}

void CPlayer::Set_Jump_State()
{
	m_pPhysics->Set_Enable_Gravity(true);
	m_pPhysics->Set_Enable_Velocity(true);

	Vector4 vCurrentVelocity = m_pPhysics->Get_Current_Velocity();
	vCurrentVelocity.y = m_fCurrentYJumpSpeed;
	m_pPhysics->Set_Velocity(vCurrentVelocity);

	m_eCurState = Jump;
}

void CPlayer::Set_Air_Jump_State()
{
	m_pPhysics->Set_Enable_Gravity(true);
	m_pPhysics->Set_Enable_Velocity(true);

	Vector4 vCurrentVelocity = m_pPhysics->Get_Current_Velocity();
	vCurrentVelocity.y = m_fCurrentYAirJumpSpeed;
	m_pPhysics->Set_Velocity(vCurrentVelocity);

	m_eCurState = Jump_Air;
}

void CPlayer::Set_Mount_Jump_State()
{
	m_eCurState = PLAYER_STATE::Mounting_Jump;
	m_ePrevState = PLAYER_STATE::Mounting_Jump;	

	m_pPhysics->Set_Enable_Gravity(true);
	m_pPhysics->Set_Enable_Velocity(true);

	Vector4 vCurrentVelocity = m_pPhysics->Get_Current_Velocity();
	vCurrentVelocity.y = m_fCurrentYJumpSpeed - 10.f;//- 10.f;
	m_pPhysics->Set_Velocity(vCurrentVelocity);
}

void CPlayer::Set_Dash_Front_State()
{
	if (m_tPlayerStateDesc.fCurrentSP < m_tPlayerStateDesc.fMaxSP * 0.3f)
	{
		SYSTEMINFO_MESSAGE(MESSAGE_COOLTIME)
		return;
	}


	m_eCurState = DASH_FRONT;
	m_fYDashAccTime = 0.f;
	Sub_SP(m_tPlayerStateDesc.fMaxSP * 0.3f);
}

void CPlayer::Set_Dash_Air_State()
{
	if (m_tPlayerStateDesc.fCurrentSP < m_tPlayerStateDesc.fMaxSP * 0.3f)
	{
		SYSTEMINFO_MESSAGE(MESSAGE_COOLTIME)
		return;
	}


	Pause_Physics();
	m_eCurState = Dash_Front_Air;
	Sub_SP(m_tPlayerStateDesc.fMaxSP * 0.3f);
}

void CPlayer::Set_Dash_Back_Air_State()
{
	if (m_tPlayerStateDesc.fCurrentSP < m_tPlayerStateDesc.fMaxSP * 0.3f)
	{
		SYSTEMINFO_MESSAGE(MESSAGE_COOLTIME)
		return;
	}


	Pause_Physics();
	m_eCurState = Dash_Back_Air;
	Sub_SP(m_tPlayerStateDesc.fMaxSP * 0.3f);
}

void CPlayer::Set_Hit_Throw_Air_State()
{
	m_eCurState = PLAYER_STATE::Hit_Throw_Air;
	m_ePrevState = PLAYER_STATE::Hit_Throw_Air;

	CAnimationState* pCurrentAnimationState = m_pModelBodyCom->Get_Animator()->Get_CurAnimState();
	AnimState		 eCurrentAnimationState = pCurrentAnimationState->Get_eAnimStateTag();

	if (AnimState::Hand_FloatLoop == eCurrentAnimationState)
	{
		m_pModelBodyCom->Change_Animation(AnimState::Hand_Floating);
	}
	else
	{
		m_pModelBodyCom->Change_Animation(AnimState::Hand_Float);
	}

	Vector4 vVelocity = m_pPhysics->Get_Current_Velocity();
	vVelocity.y = m_fHitThrowAirYVelocity;

	m_pPhysics->Set_Velocity(vVelocity);
}

void CPlayer::Set_Hit_Throw_Ground_State()
{
	m_eCurState = PLAYER_STATE::Hit_Throw_Ground;
	m_ePrevState = PLAYER_STATE::Hit_Throw_Ground;
	m_pModelBodyCom->Change_Animation(AnimState::Hand_HeavyHitEnd);


	if (ViewMode::ADS == m_eViewMode)
	{
		Camera_To_Original();
	}
}

void CPlayer::Set_Hand_Idle_State()
{
	m_eCurState = PLAYER_STATE::Idle;
	m_ePrevState = PLAYER_STATE::Idle;
	m_pPhysics->Set_Enable_Gravity(true);
	m_pPhysics->Set_Enable_Velocity(true);

	m_pModelBodyCom->Change_Animation(AnimState::Hand_Stand);
}

void CPlayer::Set_Fall_State(_float _fStartYVelocity)
{
	m_eCurState = Jump;
	m_ePrevState = Jump;
	m_pPhysics->Set_Enable_Gravity(true);
	m_pPhysics->Set_Enable_Velocity(true);

	Vector4 vCurrentVelocity = m_pPhysics->Get_Current_Velocity();
	vCurrentVelocity.y = _fStartYVelocity;
	m_pPhysics->Set_Velocity(vCurrentVelocity);

	m_pModelBodyCom->Change_Animation(AnimState::Hand_Jump_Loop);
}

void CPlayer::Set_Falling_Attack_State(_float _fStartYVelocity)
{
	m_eCurState = Falling_Attack;
	m_ePrevState = Falling_Attack;

	m_pPhysics->Set_Enable_Gravity(true);
	m_pPhysics->Set_Enable_Velocity(true);

	Vector4 vCurrentVelocity = m_pPhysics->Get_Current_Velocity();
	vCurrentVelocity.y = _fStartYVelocity;
	m_pPhysics->Set_Velocity(vCurrentVelocity);
}

void	CPlayer::Set_DualGunUpperKick()
{
	m_pPhysics->Set_Enable_Gravity(true);
	m_pPhysics->Set_Enable_Velocity(true);

	Vector4 vCurrentVelocity = m_pPhysics->Get_Current_Velocity();
	vCurrentVelocity.y = m_fCurrentYAirJumpSpeed;
	m_pPhysics->Set_Velocity(vCurrentVelocity);
	//m_pPhysics->Set_Gravity_Offset((30.f) / (98.f * (2.f / 60.f)));

	m_eCurState = Air_Attack;
	m_ePrevState = Air_Attack;
}

void CPlayer::Stop_Reset_Physics()
{
	m_pPhysics->Set_Enable_Gravity(false);
	m_pPhysics->Set_Enable_Velocity(false);
	m_pPhysics->Set_Velocity({ 0.f, 0.f, 0.f, 0.f });
}

void	CPlayer::Pause_Physics()
{
	m_pPhysics->Set_Enable_Gravity(false);
	m_pPhysics->Set_Enable_Velocity(false);
}

void	CPlayer::Resume_Physics()
{
	m_pPhysics->Set_Enable_Gravity(true);
	m_pPhysics->Set_Enable_Velocity(true);
}


void	CPlayer::Set_MaxYVelocity(_float _fYVelocity)
{
	m_pPhysics->Set_Max_Falling_Velocity(_fYVelocity);
}

void	CPlayer::Set_MinYVelocity(_float _fYVelocity)
{
	m_pPhysics->Set_Min_Falling_Velocity(_fYVelocity);
}

void CPlayer::Set_Disable_Move()
{
	m_eCurState = PLAYER_STATE::Idle;
}

Vector4 CPlayer::Get_Player_Position()
{
	if (nullptr == m_pTransformCom)
	{
		return { 0.f, 0.f, 0.f, 1.f };
	}
	else
	{
		return m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	}
}


HRESULT CPlayer::Update_PlayerState(_float _fTimeDelta)
{
	m_pCombatModule->Tick(_fTimeDelta);
	m_pControlModule->Tick(_fTimeDelta);
	return S_OK;
}

void CPlayer::Set_CutScene()
{
	Clear_Damping();

	if (PLAYER_STATE::Mounting_Idle == m_eCurState || PLAYER_STATE::Mounting_Jump == m_eCurState ||
		PLAYER_STATE::Mounting_Move == m_eCurState || PLAYER_STATE::Mounting_Up == m_eCurState)
	{
		m_eCurState = PLAYER_STATE::Idle;
	}
	else if (PLAYER_STATE::Gliding == m_eCurState)
	{
		Set_Fall_State(0.f);
	}

	m_pPlayerBodyHitColliderCom->Set_Enable(false);
}

void CPlayer::Set_Constraint_For_Mini_Game(CONSTRAINTDESC _tDesc)
{
	m_bEnableConstraint = true;

	m_iSavedCell = m_pNavigationCom->Get_CurrentIndex();
	m_vSavedPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION); m_vSavedPosition.w = 1.f;
	
	_int iNewCell = m_pNavigationCom->Get_Index(_tDesc.vPlayerStartPosition);

	m_pNavigationCom->Set_CurrentIndex(iNewCell);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _tDesc.vPlayerStartPosition, m_pNavigationCom);

	m_vConstraintCenterPosition = _tDesc.vPlayerMoveConstraintCenterPosition;
	m_vConstraintSize = _tDesc.vPlayerMoveConstraintSize;

	m_bEnableMounting = _tDesc.bEnableMounting;
	m_bEnableGliding = _tDesc.bEnableGliding;
	m_bEnableAttack = _tDesc.bEnableAttack;
	m_bEnableJump = _tDesc.bEnableJump;

	if (_tDesc.bChangeTurnLimitTime)
	{
		m_fSaveTurnLimitTime = m_fCurrentTurnLimitTime;
		m_fCurrentTurnLimitTime = _tDesc.fYTurnLimitTime;
	}

	m_fSaveGravityOffset = m_pPhysics->Get_Gravity_Offset();
	m_pPhysics->Set_Gravity_Offset(m_fSaveGravityOffset);

	if (_tDesc.bChangeJumpSpeed)
	{
		m_fCurrentYJumpSpeed = _tDesc.fYJumpSpeed;
	}

	if (_tDesc.bChangeJumpAirSpeed)
	{
		m_fCurrentYAirJumpSpeed = _tDesc.fYJumpAriSpeed;
	}

	m_vPlayerSaveLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	m_vPlayerSaveLook.Normalize();

	if (_tDesc.bChangePlayerLook)
	{
		m_pTransformCom->Set_NewLook(_tDesc.vPlayerStartLook);
	}
}


void CPlayer::Reset_Constraint_For_Mini_Game(_bool _bGoBackBeforePosition)
{
	if (_bGoBackBeforePosition)
	{
		m_pNavigationCom->Set_CurrentIndex(m_iSavedCell);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vSavedPosition, m_pNavigationCom);
	}

	ZeroMemory(&m_vSavedPosition, sizeof m_vSavedPosition); m_vSavedPosition.w = 1.f;
	ZeroMemory(&m_vConstraintSize, sizeof m_vConstraintSize);
	ZeroMemory(&m_vConstraintCenterPosition, sizeof m_vConstraintCenterPosition); m_vConstraintCenterPosition.w = 1.f;

	m_bEnableConstraint = false;
	m_bEnableMounting = true;
	m_bEnableGliding = true;
	m_bEnableAttack = true;
	m_bEnableJump = true;

	m_pPhysics->Set_Gravity_Offset(m_fSaveGravityOffset);
	m_fCurrentYJumpSpeed = m_fYJumpSpeed;
	m_fCurrentYAirJumpSpeed = m_fYAirJumpSeed;
	m_fCurrentTurnLimitTime = m_fInitialTurnLimitTime;

	m_pTransformCom->Set_NewLook(m_vPlayerSaveLook);
}

void CPlayer::Check_Weapon(_float _fTimeDelta)
{
	// Player weapon swap, and weapon in and out state check.

	_bool bCanSwap = { false };
	if (PLAYER_STATE::Idle == m_eCurState || PLAYER_STATE::Walk == m_eCurState || PLAYER_STATE::Run == m_eCurState ||
		PLAYER_STATE::Sprint == m_eCurState ||
		PLAYER_STATE::DASH_FRONT == m_eCurState ||
		PLAYER_STATE::DASH_BACK == m_eCurState ||
		PLAYER_STATE::Jump == m_eCurState ||
		PLAYER_STATE::Jump_Air == m_eCurState)
	{
		bCanSwap = true;
	}

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	//////////////////////////////// INHO ////////////////////////////////////////////////////
	// Q로 스왑하면 WEAPONSWAP_LEFT랑 스왑 -> 근데 다른 스킬 키 설정 되어있는것 같아서 일단 1로 해놓음
	if (CFunc::Is_KeyDown(DIK_Q) && bCanSwap)
	{
		string strWeaponSwapLeft = m_pWeaponSwap[WEAPONSWAP_LEFT]->Get_ClassName();
		string strCurWeapon = m_pCurWeapon->Get_ClassName();

		if (strWeaponSwapLeft != strCurWeapon)
		{
			if (Idle == m_eCurState)
			{
				m_pModelBodyCom->Change_Animation(AnimState::Hand_Stand);
				m_pCurWeapon->In_Weapon();
			}

			// swap
			CPlayer_Weapon* pWeaponTemp = m_pCurWeapon;
			m_pCurWeapon = m_pWeaponSwap[WEAPONSWAP_LEFT];
			Set_Player_State();

			m_pWeaponSwap[WEAPONSWAP_LEFT] = pWeaponTemp;
			m_eLeftWeaponMode = m_pWeaponSwap[WEAPONSWAP_LEFT]->Get_Weapon_Mode();

			// use swap skill
			if (m_bSwapSkillOn)
			{
				m_bSwapSkillOn = false;
			}

			// 준기형꺼 swap이후 처리
			if ("CPlayer_Weapon_CobaltB_Gun" == m_pCurWeapon->Get_ClassName())
			{
				m_eWeaponMode = PLAYER_WEAPON_MODE::GUN;
			}
			else if ("CPlayer_Weapon_Linghan" == m_pCurWeapon->Get_ClassName())
			{
				m_eWeaponMode = PLAYER_WEAPON_MODE::LINGHAN;
			}
			else if ("CPlayer_Weapon_DualGun" == m_pCurWeapon->Get_ClassName())
			{
				m_eWeaponMode = PLAYER_WEAPON_MODE::DUALGUN;
			}
			else if ("CPlayer_Weapon_Sickle" == m_pCurWeapon->Get_ClassName())
			{
				m_eWeaponMode = PLAYER_WEAPON_MODE::SICKLE;
			}

			CFunc::Play_Audio_UI("UI_Weapon_Change");
		}


	}
	// E로 스왑하면 WEAPONSWAP_RIGHT랑 스왑 -> 일단 2로
	else if (CFunc::Is_KeyDown(DIK_E) && bCanSwap)
	{
		string strWeaponSwapRight = m_pWeaponSwap[WEAPONSWAP_RIGHT]->Get_ClassName();
		string strCurWeapon = m_pCurWeapon->Get_ClassName();

		if (strWeaponSwapRight != strCurWeapon)
		{
			if (Idle == m_eCurState)
			{
				m_pModelBodyCom->Change_Animation(AnimState::Hand_Stand);
				m_pCurWeapon->In_Weapon();
			}

			// swap
			CPlayer_Weapon* pWeaponTemp = m_pCurWeapon;
			m_pCurWeapon = m_pWeaponSwap[WEAPONSWAP_RIGHT];
			Set_Player_State();

			m_pWeaponSwap[WEAPONSWAP_RIGHT] = pWeaponTemp;
			m_eRightWeaponMode = m_pWeaponSwap[WEAPONSWAP_RIGHT]->Get_Weapon_Mode();

			// use swap skill
			if (m_bSwapSkillOn)
			{
				m_bSwapSkillOn = false;
			}

			// 준기형꺼 swap이후 처리
			if ("CPlayer_Weapon_CobaltB_Gun" == m_pCurWeapon->Get_ClassName())
			{
				m_eWeaponMode = PLAYER_WEAPON_MODE::GUN;
			}
			else if ("CPlayer_Weapon_Linghan" == m_pCurWeapon->Get_ClassName())
			{
				m_eWeaponMode = PLAYER_WEAPON_MODE::LINGHAN;
			}
			else if ("CPlayer_Weapon_DualGun" == m_pCurWeapon->Get_ClassName())
			{
				m_eWeaponMode = PLAYER_WEAPON_MODE::DUALGUN;
			}
			else if ("CPlayer_Weapon_Sickle" == m_pCurWeapon->Get_ClassName())
			{
				m_eWeaponMode = PLAYER_WEAPON_MODE::SICKLE;
			}

			CFunc::Play_Audio_UI("UI_Weapon_Change");
		}
	}


	CAnimationState* pAnimState = m_pModelBodyCom->Get_Animator()->Get_CurAnimState();


	if (PLAYER_STATE::Normal_Atk == m_eCurState || PLAYER_STATE::Air_Attack == m_eCurState || PLAYER_STATE::Falling_Attack == m_eCurState ||
		PLAYER_STATE::Skill_Atk == m_eCurState)
	{
		m_pCurWeapon->Out_Weapon();
	}
	else if (PLAYER_STATE::Walk == m_eCurState || PLAYER_STATE::Run == m_eCurState)
	{
		m_pCurWeapon->In_Weapon();
	}
	else if (PLAYER_STATE::Sprint == m_eCurState || PLAYER_STATE::DASH_FRONT == m_eCurState || PLAYER_STATE::DASH_BACK == m_eCurState ||
		PLAYER_STATE::Dash_Front_Air == m_eCurState || PLAYER_STATE::Dash_Back_Air == m_eCurState || PLAYER_STATE::Jump == m_eCurState ||
		PLAYER_STATE::Jump_Air == m_eCurState || PLAYER_STATE::Hit_Throw_Ground == m_eCurState)
	{
		m_pCurWeapon->Disable_Weapon();
	}
}

void CPlayer::Handling_Skill(_float _fTimeDelta)
{
	CAnimationState* pCurAnimState = m_pModelBodyCom->Get_Animator()->Get_CurAnimState();

	_bool bMoveInput = Is_MoveInput();

	if (AnimState::Dual_Gun_Air_Skill_Loop == (AnimState)pCurAnimState->Get_eAnimStateTag())
	{
		m_fDualGunAirSkillUseAccTime += _fTimeDelta;

		if (bMoveInput)
		{
			CGameInstance* pGameInstance = CGameInstance::Get_Instance();


			Matrix matView = pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_VIEW);
			Vector3 vCurPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

			Vector3 vDir;
			Vector3 vLook = matView.Backward();
			vLook.y = 0.f;


			if (CFunc::Is_KeyHold(DIK_W))
			{
				vDir = matView.Backward();
			}
			else if (CFunc::Is_KeyHold(DIK_S))
			{
				vDir = matView.Forward();
			}
			else if (CFunc::Is_KeyHold(DIK_A))
			{
				vDir = matView.Left();
			}
			else if (CFunc::Is_KeyHold(DIK_D))
			{
				vDir = matView.Right();
			}

			vDir.y = 0.f;
			vDir.Normalize();

			vCurPos += vDir * _fTimeDelta * m_fDualGunAirSkillMoveSpeed;

			m_pTransformCom->Set_State(CTransform::STATE_POSITION, { vCurPos.x, vCurPos.y, vCurPos.z, 1.f }, m_pNavigationCom);
		}

	}
}

_bool CPlayer::Is_Intersect(_float3& _vOutPos, _float& _fOutDist)
{
	return false;
}


_float	CPlayer::Get_Y_Ground_Distance()
{
	Vector4 vPlayerPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float fYOutDist;

	CNavigation::INTERSECTDIRECTION eIntersect = m_pNavigationCom->Get_YIntersectDistanceFromPoint(vPlayerPosition, fYOutDist);

	if (CNavigation::INTERSECTDIRECTION::DOWN == eIntersect)
	{
		return -fYOutDist;
	}
	else if (CNavigation::INTERSECTDIRECTION::UP == eIntersect)
	{
		return +fYOutDist;
	}
	else
	{
		return +0.f;
	}

}

void CPlayer::Change_Collision_Transform_By_Attack_Animation()
{
	AnimState eCurrentAnimationState = m_pModelBodyCom->Get_Animator()->Get_CurAnimState()->Get_eAnimStateTag();

	switch (m_eWeaponMode)
	{
	case Client::PLAYER_WEAPON_MODE::GUN:
	{
		if (AnimState::Gun_Melee_Attack01 == eCurrentAnimationState)
		{
			m_pMeleeOBBAttackColliderCom->Set_Position({ 0.f, 0.f, 10.f });
			m_pMeleeOBBAttackColliderCom->Set_Size({ 5.f, 4.f, 7.5f });
		}
		else if (AnimState::Gun_Melee_Attack02 == eCurrentAnimationState)
		{
			m_pMeleeOBBAttackColliderCom->Set_Position({ 0.f, 0.f, 10.f });
			m_pMeleeOBBAttackColliderCom->Set_Size({ 5.f, 4.f, 7.5f });
		}
		else if (AnimState::Gun_Melee_Attack03 == eCurrentAnimationState)
		{
			m_pMeleeOBBAttackColliderCom->Set_Position({ 0.f, 0.f, 14.f });
			m_pMeleeOBBAttackColliderCom->Set_Size({ 6.5f, 4.f, 11.5f });
		}
		else if (AnimState::Gun_Melee_Attack04 == eCurrentAnimationState)
		{
			m_pMeleeOBBAttackColliderCom->Set_Position({ 0.f, 0.f, 10.f });
			m_pMeleeOBBAttackColliderCom->Set_Size({ 5.f, 4.f, 7.5f });
		}
		else if (AnimState::Gun_Melee_Attack05 == eCurrentAnimationState)
		{
			m_pMeleeOBBAttackColliderCom->Set_Position({ 0.f, 0.f, 10.f });
			m_pMeleeOBBAttackColliderCom->Set_Size({ 5.f, 4.f, 7.5f });
		}
		else if (AnimState::Gun_Holding_Normal_Combo_Attack == eCurrentAnimationState)
		{
			m_pMeleeOBBAttackColliderCom->Set_Position({ 0.f, 0.f, 10.f });
			m_pMeleeOBBAttackColliderCom->Set_Size({ 5.f, 4.f, 7.5f });
		}
		else if (AnimState::Gun_Burst_Skill == eCurrentAnimationState)
		{
			m_pMeleeOBBAttackColliderCom->Set_Position({ 0.f, 0.f, 11.f });
			m_pMeleeOBBAttackColliderCom->Set_Size({ 7.5f, 4.f, 10.f });
		}
		else if (AnimState::Gun_Jump_Normal_Attack == eCurrentAnimationState)
		{
			m_pMeleeOBBAttackColliderCom->Set_Position({ 0.f, -7.f, 17.f });
			m_pMeleeOBBAttackColliderCom->Set_Size({ 5.f, 4.f, 7.5f });
		}
		else
		{
			m_pMeleeOBBAttackColliderCom->Set_Position({ 0.f, 0.f, 10.f });
			m_pMeleeOBBAttackColliderCom->Set_Size({ 5.f, 4.f, 7.5f });
		}
		break;
	}
	case Client::PLAYER_WEAPON_MODE::DUALGUN:
	{
		if (AnimState::Dual_Gun_Melee_Attack01 == eCurrentAnimationState)
		{
			m_pMeleeOBBAttackColliderCom->Set_Position({ 0.f, 0.f, 10.f });
			m_pMeleeOBBAttackColliderCom->Set_Size({ 5.f, 4.f, 7.5f });
		}
		else if (AnimState::Dual_Gun_Melee_Attack02 == eCurrentAnimationState)
		{
			m_pMeleeOBBAttackColliderCom->Set_Position({ 0.f, 0.f, 10.f });
			m_pMeleeOBBAttackColliderCom->Set_Size({ 5.f, 4.f, 7.5f });
		}
		else if (AnimState::Dual_Gun_Melee_Attack03 == eCurrentAnimationState)
		{
			m_pMeleeOBBAttackColliderCom->Set_Position({ 0.f, 0.f, 10.f });
			m_pMeleeOBBAttackColliderCom->Set_Size({ 5.f, 4.f, 7.5f });
		}
		else if (AnimState::Dual_Gun_Melee_Attack04 == eCurrentAnimationState)
		{
			m_pMeleeAttackColliderCom->Set_Position({ 0.f, 0.f, 0.f });
			m_pMeleeAttackColliderCom->Set_Radius(9.f);
		}
		else if (AnimState::Dual_Gun_Melee_Attack05 == eCurrentAnimationState)
		{
			m_pMeleeOBBAttackColliderCom->Set_Position({ 0.f, 0.f, 10.f });
			m_pMeleeOBBAttackColliderCom->Set_Size({ 5.f, 4.f, 7.5f });
		}
		else if (AnimState::Dual_Gun_DropKick_End == eCurrentAnimationState)
		{
			m_pMeleeAttackColliderCom->Set_Position({ 0.f, 0.f, 0.f });
			m_pMeleeAttackColliderCom->Set_Radius(7.f);
		}
		else if (AnimState::Dual_Gun_Air_Skill_Loop == eCurrentAnimationState)
		{
			m_pMeleeAttackColliderCom->Set_Position({ 0.f, 0.f, 0.f });
			m_pMeleeAttackColliderCom->Set_Radius(9.f);
		}
		else
		{
			m_pMeleeAttackColliderCom->Set_Position({ 0.f, 0.f, 3.f });
			m_pMeleeAttackColliderCom->Set_Radius(4.f);
		}
		break;
	}
	case Client::PLAYER_WEAPON_MODE::LINGHAN:
	{
		if (AnimState::Linghan_Melee_Attack01 == eCurrentAnimationState)
		{
			m_pMeleeOBBAttackColliderCom->Set_Position({ 0.f, 0.f, 5.f });
			m_pMeleeOBBAttackColliderCom->Set_Size({ 5.f, 4.f, 5.f });
		}
		else if (AnimState::Linghan_Melee_Attack02 == eCurrentAnimationState)
		{
			m_pMeleeOBBAttackColliderCom->Set_Position({ 0.f, 0.f, 5.f });
			m_pMeleeOBBAttackColliderCom->Set_Size({ 10.f, 4.f, 5.f });
		}
		else if (AnimState::Linghan_Melee_Attack03 == eCurrentAnimationState)
		{
			m_pMeleeAttackColliderCom->Set_Position({ 0.f, 0.f, 0.f });
			m_pMeleeAttackColliderCom->Set_Radius(7.5f);
		}
		else if (AnimState::Linghan_Melee_Attack04 == eCurrentAnimationState)
		{
			m_pMeleeOBBAttackColliderCom->Set_Position({ 0.f, 0.f, 7.5f });
			m_pMeleeOBBAttackColliderCom->Set_Size({ 10.f, 4.f, 7.5f });
		}
		else if (AnimState::Linghan_Melee_Attack05 == eCurrentAnimationState)
		{
			m_pMeleeOBBAttackColliderCom->Set_Position({ 0.f, 0.f, 7.5f });
			m_pMeleeOBBAttackColliderCom->Set_Size({ 10.f, 4.f, 7.5f });
		}
		else if (AnimState::Linghan_Melee_Attack05_1 == eCurrentAnimationState)
		{
			m_pMeleeOBBAttackColliderCom->Set_Position({ 0.f, 0.f, 15.f });
			m_pMeleeOBBAttackColliderCom->Set_Size({ 12.f, 4.f, 15.f });
		}
		else if (AnimState::Linghan_Melee_Air_Attack01 == eCurrentAnimationState)
		{
			m_pMeleeOBBAttackColliderCom->Set_Position({ 0.f, 0.f, 5.f });
			m_pMeleeOBBAttackColliderCom->Set_Size({ 5.f, 4.f, 5.f });
		}
		else if (AnimState::Linghan_Melee_Air_Attack02 == eCurrentAnimationState)
		{
			m_pMeleeOBBAttackColliderCom->Set_Position({ 0.f, 0.f, 5.f });
			m_pMeleeOBBAttackColliderCom->Set_Size({ 10.f, 4.f, 5.f });
		}
		else if (AnimState::Linghan_Melee_Air_Attack03 == eCurrentAnimationState)
		{
			m_pMeleeAttackColliderCom->Set_Position({ 0.f, 0.f, 0.f });
			m_pMeleeAttackColliderCom->Set_Radius(7.5f);
		}
		else if (AnimState::Linghan_Melee_Air_Attack04 == eCurrentAnimationState)
		{
			m_pMeleeOBBAttackColliderCom->Set_Position({ 0.f, 0.f, 7.5f });
			m_pMeleeOBBAttackColliderCom->Set_Size({ 10.f, 4.f, 7.5f });
		}
		else if (AnimState::Linghan_Melee_Air_Attack05 == eCurrentAnimationState)
		{
			m_pMeleeOBBAttackColliderCom->Set_Position({ 0.f, 0.f, 7.5f });
			m_pMeleeOBBAttackColliderCom->Set_Size({ 10.f, 4.f, 7.5f });
		}
		else if (AnimState::Linghan_Melee_Air_Attack05_1 == eCurrentAnimationState)
		{
			m_pMeleeOBBAttackColliderCom->Set_Position({ 0.f, 0.f, 15.f });
			m_pMeleeOBBAttackColliderCom->Set_Size({ 12.f, 4.f, 15.f });
		}

		break;
	}
	case Client::PLAYER_WEAPON_MODE::SICKLE:
	{
		// Player solid collider off for specific case.
		if (AnimState::Sickle_Melee_Air_Attack05Loop == eCurrentAnimationState)
		{
			m_pPlayerBodyColliderCom->Set_Enable(false);
		}
		else
		{
			m_pPlayerBodyColliderCom->Set_Enable(true);
		}


		if (AnimState::Sickle_Melee_Air_Attack03 == eCurrentAnimationState)
		{
			m_pMeleeAttackColliderCom->Set_Position({ 0.f, 0.f, 0.f });
			m_pMeleeAttackColliderCom->Set_Radius(8.f);
		}
		else if (AnimState::Sickle_Melee_Air_Attack05End == eCurrentAnimationState)
		{
			m_pMeleeAttackColliderCom->Set_Position({ 0.f, 0.f, 0.f });
			m_pMeleeAttackColliderCom->Set_Radius(6.5f);
		}
		else
		{
			m_pMeleeAttackColliderCom->Set_Position({ 0.f, 0.f, 3.f });
			m_pMeleeAttackColliderCom->Set_Radius(4.f);
		}
		break;
	}
	case Client::PLAYER_WEAPON_MODE::HAMMER:
	{

		break;
	}
	case Client::PLAYER_WEAPON_MODE::WEAPON_END:
		break;
	default:
		break;
	}
}

void CPlayer::Add_Hp(_float _fAmount)
{
	_fAmount = abs(_fAmount);

	m_tStateInfo.fCurrentHP += _fAmount;

	if (m_tStateInfo.fCurrentHP > m_tStateInfo.fMaxHP)
	{
		m_tStateInfo.fCurrentHP = m_tStateInfo.fMaxHP;
	}
}

void CPlayer::Sub_Hp(_float _fAmount)
{
	_fAmount = abs(_fAmount);

	m_tStateInfo.fCurrentHP -= _fAmount;

	if (m_tStateInfo.fCurrentHP < 0.f)
	{
		m_tStateInfo.fCurrentHP = 0.f;
	}
}

void CPlayer::Sub_SP(_float _fAmount)
{
	_fAmount = abs(_fAmount);

	// 임의의 값 설정
	m_tPlayerStateDesc.fCurrentSP -= _fAmount;

	if (m_tPlayerStateDesc.fCurrentSP < 0)
	{
		m_tPlayerStateDesc.fCurrentSP = 0;
	}
}

void CPlayer::Sub_EP(_float _fAmount)
{
	_fAmount = abs(_fAmount);

	m_tPlayerStateDesc.fCurrentEP -= _fAmount;

	if (m_tPlayerStateDesc.fCurrentEP < 0.f)
	{
		m_tPlayerStateDesc.fCurrentEP = 0.f;
	}
}

void CPlayer::Add_EP(_float _fTickAmount)
{
	_fTickAmount = abs(_fTickAmount);

	m_tPlayerStateDesc.fCurrentEP += _fTickAmount;

	if (m_tPlayerStateDesc.fCurrentEP > m_tPlayerStateDesc.fMaxEP)
	{
		m_tPlayerStateDesc.fCurrentEP = m_tPlayerStateDesc.fMaxEP;
	}
}

void CPlayer::Increase_EP(_float _fAmount)
{
	_fAmount = abs(_fAmount);

	m_tPlayerStateDesc.fCurrentEP += _fAmount;

	if (m_tPlayerStateDesc.fCurrentEP > m_tPlayerStateDesc.fMaxEP)
	{
		m_tPlayerStateDesc.fCurrentEP = m_tPlayerStateDesc.fMaxEP;
	}
}

void CPlayer::Add_Satiety(_int _iAmount)
{
	_iAmount = abs(_iAmount);

	m_tPlayerStateDesc.iCurrentSatiety += _iAmount;

	if (m_tPlayerStateDesc.iCurrentSatiety > m_tPlayerStateDesc.iMaxSatiety)
	{
		m_tPlayerStateDesc.iCurrentSatiety = m_tPlayerStateDesc.iMaxSatiety;
	}
}

void CPlayer::Sub_Satiety(_int _iAmount)
{
	_iAmount = abs(_iAmount);

	m_tPlayerStateDesc.iCurrentSatiety -= _iAmount;

	if (m_tPlayerStateDesc.iCurrentSatiety < 0)
	{
		m_tPlayerStateDesc.iCurrentSatiety = 0;
	}
}

void CPlayer::Add_Hp_Using_Satiety(_float _fTimeDelta)
{
	// 전투 x : Idle, Weapon_Idle, Walk, Run, Sprint, DASH_FRONT, DASH_BACK, Dash_Front_Air, Dash_Back_Air, Jump, Jump_Air, Gliding_Charge, Gliding, Mounting, Mounting_Jump, Interaction_No_Move, Interaction_Move, Aimming, Aimming_Move
	// 전투 o : Aimming_Stand_Attack, Aimming_Move_Attack, Falling_Attack, Air_Attack, Normal_Atk, Skill_Atk, Hit_Throw_Ground, Hit_Throw_Air, Hit_Llight,


	// 현재 전투상태가 아닐 때
	if (Idle == m_eCurState || Weapon_Idle == m_eCurState || Walk == m_eCurState ||
		Run == m_eCurState || Sprint == m_eCurState || DASH_FRONT == m_eCurState ||
		DASH_BACK == m_eCurState || Dash_Front_Air == m_eCurState || Dash_Back_Air == m_eCurState ||
		Jump == m_eCurState || Jump_Air == m_eCurState || Gliding_Charge == m_eCurState ||
		Gliding == m_eCurState || Mounting_Idle == m_eCurState || Mounting_Jump == m_eCurState ||
		Mounting_Move == m_eCurState || Mounting_Up == m_eCurState ||
		Interaction_No_Move == m_eCurState || Interaction_Move == m_eCurState || Aimming == m_eCurState || Aimming_Move == m_eCurState)
	{
		// 이전에 전투 상태였으면
		if (Aimming_Stand_Attack == m_ePrevState || Aimming_Move_Attack == m_ePrevState || Falling_Attack == m_ePrevState ||
			Air_Attack == m_ePrevState || Normal_Atk == m_ePrevState || Skill_Atk == m_ePrevState ||
			Hit_Throw_Ground == m_ePrevState || Hit_Throw_Air == m_ePrevState || Hit_Light == m_ePrevState)
		{
			m_bStateNoFight = true;
		}
		// 이전에도 전투상태가 아니었을 때
		else
		{
			m_bStateNoFight = true;
		}
	}
	// 현재 전투 상태일 때
	else
	{
		m_bStateNoFight = false;
		m_bSatietyHPRegen = false;
		m_fSatietyWaitTimeAcc = 0.f;
		m_fSatietyHpRegenCoolTimeAcc = 0.f;
	}

	// m_bStateNoFight이 true가 된지 5초 후부터 포만감 수치에 따라 hp 리젠
	if (m_bStateNoFight)
	{
		m_fSatietyWaitTimeAcc += _fTimeDelta;

		if (m_fSatietyWaitTimeAcc >= m_fSatietyWaitTime)
		{
			m_bSatietyHPRegen = true;
		}
	}

	// 포만감 수치에 따라 플레이어 hp 증가
	if (m_bSatietyHPRegen)
	{
		// 플레이어 HP가 가득 차 있지 않을 때만
		if (m_tStateInfo.fCurrentHP < m_tStateInfo.fMaxHP)
		{
			// 2초 쿨타임 지날 때마다
			m_fSatietyHpRegenCoolTimeAcc += _fTimeDelta;

			if (m_fSatietyHpRegenCoolTimeAcc >= m_fSatietyHpRegenCoolTime)
			{
				m_fSatietyHpRegenCoolTimeAcc = 0.f;

				// 포만감 수치에 따라서
				// 100 ~ 61
				if (100 >= m_tPlayerStateDesc.iCurrentSatiety && 61 <= m_tPlayerStateDesc.iCurrentSatiety)
				{
					// hp 총량의 5% 회복
					_float fAddHp = m_tStateInfo.fMaxHP * 0.05f;
					m_fSatietyRegenHP += fAddHp;
					Add_Hp(fAddHp);
				}
				// 60 ~ 31
				else if (60 >= m_tPlayerStateDesc.iCurrentSatiety && 31 <= m_tPlayerStateDesc.iCurrentSatiety)
				{
					// hp 총량의 3.5% 회복
					_float fAddHp = m_tStateInfo.fMaxHP * 0.035f;
					m_fSatietyRegenHP += fAddHp;
					Add_Hp(fAddHp);
				}
				// 30 ~ 1
				else if (30 >= m_tPlayerStateDesc.iCurrentSatiety && 1 <= m_tPlayerStateDesc.iCurrentSatiety)
				{
					// hp 총량의 2% 회복
					_float fAddHp = m_tStateInfo.fMaxHP * 0.02f;
					m_fSatietyRegenHP += fAddHp;
					Add_Hp(fAddHp);
				}
				// 0
				else
				{
					// 자동 회복 불가
				}

			}
		}
		// 플레이어 hp 가득 차면
		else
		{
			m_fSatietyHpRegenCoolTimeAcc = 0.f;
		}
	}

	// hp 총량에서 10%를 회복할 때마다 포만감 포인트가 1씩 감소
	if (m_fSatietyRegenHP >= m_tStateInfo.fMaxHP * 0.1f)
	{
		m_fSatietyRegenHP = 0.f;			// 10% 회복한거 체크했으니까 다시 초기화
		m_tPlayerStateDesc.iCurrentSatiety -= 1;
		if (m_tPlayerStateDesc.iCurrentSatiety < 0)
		{
			m_tPlayerStateDesc.iCurrentSatiety = 0;
		}
	}
}

void CPlayer::Off_Agressive_Collider()
{

	if (nullptr != m_pMeleeAttackColliderCom)
	{
		m_pMeleeAttackColliderCom->Set_Enable(false);
	}

	if (nullptr != m_pMeleeOBBAttackColliderCom)
	{
		m_pMeleeOBBAttackColliderCom->Set_Enable(false);
	}


}

void CPlayer::Add_Debuff(DEBUFF_TYPE _eType, DEBUFFDESC _tDebuffDesc)
{
	auto iter = m_mapDebuffs.find(_eType);

	if (m_mapDebuffs.end() != iter)
	{
		_float fCurrentTime = iter->second.fCurrentTime;

		iter->second = _tDebuffDesc;
		iter->second.fCurrentTime = fCurrentTime;
	}
	else
	{
		m_mapDebuffs.insert({ _eType, _tDebuffDesc });
	}
}

void CPlayer::Apply_Debuff(_float _fTimeDelta)
{
	auto iter = m_mapDebuffs.begin();

	while (m_mapDebuffs.end() != iter)
	{
		DEBUFFDESC& tDesc = iter->second;

		tDesc.fCurrentTime -= _fTimeDelta;
		
		if (0.f >= tDesc.fCurrentTime)
		{
			tDesc.fCurrentTime = tDesc.fApplyTickDeltaTime;
			tDesc.fRemainTime += -tDesc.fApplyTickDeltaTime;

			Take_Damage(tDesc.tDamageDescData);
		}
		
		if (0.f >= tDesc.fRemainTime)
		{
			iter = m_mapDebuffs.erase(iter);
		}
		else
		{
			++iter;
		}
		
	}
	
}

void CPlayer::Eat_Food()
{
	// 퀵슬롯에 음식이 없을 때
	if (0 == CFoodList::Get_Instance()->Get_FoodAvailableID())
		return;

	// 먹을 수 없는 상태면
	if (!m_bCanEat)
	{
		SYSTEMINFO_MESSAGE(MESSAGE_COOLTIME)
		return;
	}
		

	m_bEating = true;
	m_bCanEat = false;
	m_bHitWhenEating = false;
	// Layer_UI_Eating_Progress Enable
	CGameInstance::Get_Instance()->Enable_UI(CFunc::Get_CurLevel(), TEXT("Layer_UI_Eating"), 0);

	m_tStateInfo.fCurrentHP += 500.f + 500.f * CFunc::Generate_Random_Float(0.1f, 0.25f);
	if (m_tStateInfo.fMaxHP < m_tStateInfo.fCurrentHP)
	{
		m_tStateInfo.fCurrentHP = m_tStateInfo.fMaxHP;
	}

	m_pModelBodyCom->Change_Animation(AnimState::Hand_Eat);

	auto iIndex = CFunc::Generate_Random_Int(1, 2);
	string strAudioTag = "UI_Food_Eat_" + to_string(iIndex);
	CFunc::Play_Audio_UI(strAudioTag);
}

void CPlayer::_Handling_Eat_Food(_float _fTimeDelta)
{
	if (m_bEating)
	{
		m_fEatingTimeAcc += _fTimeDelta;

		// 먹다가 맞으면
		if (m_bHitWhenEating)
		{
			m_bEating = false;
			m_bCanEat = true;
			m_fEatingTimeAcc = 0.f;
			m_bHitWhenEating = false;
			// Layer_UI_Eating_Progress Disable
			CGameInstance::Get_Instance()->Disable_UI(CFunc::Get_CurLevel(), TEXT("Layer_UI_Eating"), 0);
		}

		// 음식 먹는 거 끝났을 때
		if (m_fEatingTimeAcc >= m_fEatingTime)
		{
			// Layer_UI_Eating_Progress Disable
			CGameInstance::Get_Instance()->Disable_UI(CFunc::Get_CurLevel(), TEXT("Layer_UI_Eating"), 0);
			m_fEatingTimeAcc = 0.f;
			m_bEating = false;
			// Food_Available_Background Enable
			if (FAILED(CGameInstance::Get_Instance()->Enable_UI_Specific(CFunc::Get_CurLevel(), TEXT("Layer_UI_Food_Available"),
				TEXT("Food_Available_Background"), 0)))
				return;
			// 음식 효과 플레이어한테 적용
			if (FAILED(_Adjust_Food_Effect()))
				return;
			// 인벤토리에서 먹은 음식 Remove_Item
			_uint iFoodID = CFoodList::Get_Instance()->Get_FoodAvailableID();
			if (FAILED(CInventory::Get_Instance()->Remove_Item(iFoodID)))
				return;
			// SYSTEMINFO 사용 완료 띄우기

			CFunc::Play_Audio_UI("UI_Item_Get");

			SYSTEMINFO_MESSAGE(MESSAGE_USE_SUCCESS)
		}
	}

}

HRESULT CPlayer::_Adjust_Food_Effect()
{
	const _float4x4* pModelWorld = m_pTransformCom->Get_WorldMatrixFloat4x4Ptr();
	Create_Effect(TEXT("Player_Eat_Cook.Effect"), pModelWorld, _float3(0.f, 0.475f, 0.f), false, true);
	Create_Effect(TEXT("Player_Eat_Mouth_Cook.Effect"), nullptr, _float3(0.f, 3.4f, 0.25f));

	_uint iFoodID = CFoodList::Get_Instance()->Get_FoodAvailableID();

	auto& tItemInfo = CItem_Manager::Get_Instance()->Get_ItemInfo(iFoodID);

	// 아이템 정보 찾기 실패
	if (0 == tItemInfo.iID)
		return E_FAIL;

	if (PC_TEXTURE_UI_ITEM_FOOD == tItemInfo.wstrTextureTag)
	{
		auto& tFoodInfo = CItem_Manager::Get_Instance()->Get_FoodInfo(iFoodID);

		// 포만감 수치 적용
		Add_Satiety(tFoodInfo.iSatiety);

		// 타입에 따라서 각각의 수치 적용
		if (static_cast<_uint>(FOOD_TYPE::HEALING) == tFoodInfo.iType)
		{
			_float fPercent = static_cast<_float>(tFoodInfo.iPercent);
			_float fPercentValue = m_tStateInfo.fMaxHP * 0.01f * fPercent;
			_float fValue = tFoodInfo.iValue;
			_float fFinalValue = fPercentValue + fValue;
			Add_Hp(fFinalValue);
		}
		else if(static_cast<_uint>(FOOD_TYPE::ENDURANCE) == tFoodInfo.iType)
		{
			_float fValue = tFoodInfo.iValue;
			Add_EP(fValue);
		}
	}
	else if (PC_TEXTURE_UI_ITEM_COOKED == tItemInfo.wstrTextureTag)
	{
		auto& tCookedInfo = CItem_Manager::Get_Instance()->Get_CookedInfo(iFoodID);

		// 포만감 수치 적용
		Add_Satiety(tCookedInfo.iSatiety);
	
		// 타입에 따라서 각각의 수치 적용
		if (static_cast<_uint>(COOKED_TYPE::HEALING) == tCookedInfo.iType)
		{
			_float fPercent = static_cast<_float>(tCookedInfo.iPercent);
			_float fPercentValue = m_tStateInfo.fMaxHP * 0.01f * fPercent;
			_float fValue = tCookedInfo.iValue;
			_float fFinalValue = fPercentValue + fValue;
			Add_Hp(fFinalValue);

			// 에너지 수치 적용
			if (tCookedInfo.iEnergy > 0.f)
			{
				Increase_EP(tCookedInfo.iEnergy);
			}
			else
			{

			}
		}
		else if (static_cast<_uint>(COOKED_TYPE::FLAME_DMG) == tCookedInfo.iType)
		{

		}
		else if (static_cast<_uint>(COOKED_TYPE::FROST_DMG) == tCookedInfo.iType)
		{

		}
		else if (static_cast<_uint>(COOKED_TYPE::VOLT_DMG) == tCookedInfo.iType)
		{

		}
		else if (static_cast<_uint>(COOKED_TYPE::VOLT_RES) == tCookedInfo.iType)
		{

		}
		else if (static_cast<_uint>(COOKED_TYPE::FLAME_RES) == tCookedInfo.iType)
		{

		}
		else if (static_cast<_uint>(COOKED_TYPE::FROST_RES) == tCookedInfo.iType)
		{

		}
		else if (static_cast<_uint>(COOKED_TYPE::PHYSICAL_RES) == tCookedInfo.iType)
		{

		}
	}

	return S_OK;
}

void CPlayer::Add_SP(_float _fTickAmount)
{
	_fTickAmount = abs(_fTickAmount);

	m_tPlayerStateDesc.fCurrentSP += _fTickAmount;

	if (m_tPlayerStateDesc.fCurrentSP > m_tPlayerStateDesc.fMaxSP)
	{
		m_tPlayerStateDesc.fCurrentSP = m_tPlayerStateDesc.fMaxSP;
	}
}

void CPlayer::_Switch_Energy()
{
	if (m_tPlayerStateDesc.fCurrentEP < m_tPlayerStateDesc.fMaxEP)
		CGameInstance::Get_Instance()->Enable_UI(CFunc::Get_CurLevel(), L"Layer_UI_EnergyBar", 0);
	else
		CGameInstance::Get_Instance()->Disable_UI(CFunc::Get_CurLevel(), L"Layer_UI_EnergyBar", 0);
}

void CPlayer::_Regen_EP(_float _fTimeDelta)
{
	// 플레이어가 공중에 있지 않을 때만 EP 회복
	if (Dash_Front_Air != m_eCurState && Dash_Back_Air != m_eCurState &&
		Jump != m_eCurState && Jump_Air != m_eCurState &&
		Falling_Attack != m_eCurState && Air_Attack != m_eCurState &&
		Aimming != m_eCurState && Aimming_Move != m_eCurState &&
		Aimming_Stand_Attack != m_eCurState && Aimming_Move_Attack != m_eCurState)
	{
		m_fEPRegenTimeAcc += _fTimeDelta;

		if (m_fEPRegenTimeAcc >= m_tPlayerStateDesc.fEPRegenTime)
		{
			Add_EP(m_tPlayerStateDesc.fMaxEP * 0.025f);
			m_fEPRegenTimeAcc = 0.f;
		}
	}
}

void CPlayer::_On_SwapSkill()
{
	if (m_fWeaponSwapGauge >= m_fWeaponSwapMaxGauge)
	{
		if (m_fPrevWeaponSwapGauge < m_fWeaponSwapGauge)
		{
			CGameInstance::Get_Instance()->Enable_UI_Specific(CFunc::Get_CurLevel(), TEXT("Layer_UI_Player_Weapon_Skill"), TEXT("PlayerWeaponSwapGaugeBack_Left"), 0);
			CGameInstance::Get_Instance()->Enable_UI_Specific(CFunc::Get_CurLevel(), TEXT("Layer_UI_Player_Weapon_Skill"), TEXT("PlayerWeaponSwapGaugeBack_Right"), 0);
			m_bSwapSkillOn = true;
		}
		m_fWeaponSwapGauge = 0.f;
	}
	m_fPrevWeaponSwapGauge = m_fWeaponSwapGauge;
	//ex) m_fWeaponSwapGauge = 90이라 m_fPrevWeaponSwapGauge = 90들어간 상태에서
	//한바퀴 돌아서 m_fWeaponSwapGauge가 100 채워진 상태로 들어왔을 때
	//90 < 100이니까 -> 100보다 작은 수에서 100됐으니까 m_bSwapSkillOn = true;
}

#ifdef _DEBUG
_int CPlayer::Get_Navigation_Mesh_Index()
{
	if (nullptr == m_pNavigationCom)
	{
		return -1;
	}
	else
	{
		return m_pNavigationCom->Get_Navigation_Mesh_Index();
	}
}

Vector4 CPlayer::Get_Player_Physics_Velocity()
{
	if (nullptr == m_pPhysics)
	{
		return { 0.f, 0.f, 0.f, 1.f };
	}
	else
	{
		return m_pPhysics->Get_Current_Velocity();
	}
}
#endif // DEBUG


void CPlayer::Free()
{	
	if (m_bCloned)
	{
		CData_Manager::DB_PLAYER tPlayerDB = CData_Manager::Get_Instance()->Get_Player_DB();
		tPlayerDB.fCurrentPlayerHP = m_tStateInfo.fCurrentHP / m_tStateInfo.fMaxHP;
		tPlayerDB.eCenterWeapon = m_eWeaponMode;
		tPlayerDB.eLeftWeapon = m_eLeftWeaponMode;
		tPlayerDB.eRightWeapon = m_eRightWeaponMode;
		tPlayerDB.eExWeapon = m_eExWeaponMode;

		CData_Manager::Get_Instance()->Save_Player_Information(tPlayerDB);
	}

	__super::Free();

	Safe_Release(m_pSettingModule);
	Safe_Release(m_pCombatModule);
	Safe_Release(m_pControlModule);

	Safe_Release(m_pPhysics);

	Safe_Release(m_pModelBodyCom);
	//Safe_Release(m_pModelAnimCom);
	//Safe_Release(m_pModelFaceCom);
	//Safe_Release(m_pModelEyeSlashCom);
	//Safe_Release(m_pModelEyeBrowCom);
	//Safe_Release(m_pModelEyeCom);

	Safe_Release(m_pPlayerBodyColliderCom);
	Safe_Release(m_pMeleeAttackColliderCom);
	Safe_Release(m_pPlayerBodyHitColliderCom);
	Safe_Release(m_pMeleeOBBAttackColliderCom);
	Safe_Release(m_pPlayerInteractionColliderCom);
	Safe_Release(m_pPlayerHitTimeillusionColliderCom);


	Safe_Release(m_pShaderCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pTransformModelDrawCom);

	for (auto& pair : m_mapPlayerParts)
	{
		Safe_Release(pair.second);
	}

	for (auto& pair : m_mapPlayerWeapons)
	{
		Safe_Release(pair.second);
	}

	m_mapPlayerWeapons.clear();
	m_mapPlayerParts.clear();
}
*/