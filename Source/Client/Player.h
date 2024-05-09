#pragma once

#include "Character.h"

BEGIN(Engine)
class CModel;
class CShader;
class CPhysics;
class CTexture;
class CRenderer;
class CCollider;
class CTransform;
class CNavigation;
class CVIBuffer_Rect;
class CVIBuffer_Trail_Rect;

class CLight;
END

BEGIN(Client)
class CPlayer final : public CCharacter
{
public:
	typedef struct tPlayerInitData
	{
		LEVEL		eLevel = { LEVEL_END };
		_float		fHP = { 100.f };
		_float4		vPos = { 0.f, 0.f, 0.f, 1.f };
		_float3		vRotation = { 0.f, 0.f, 0.f };
	}INITDATA;

	enum COLLIDER
	{
		COLLIDER_AABB,
		COLLIDER_OBB,
		COLLIDER_SPHERE,
		COLLIDER_END
	};

	enum ViewMode
	{
		ADS,
		TPS,
		NONE
	};

	typedef struct tConstraintDesc
	{
		Vector4		vPlayerStartPosition = { 0.f, 0.f, 0.f, 1.f };
		Vector4		vPlayerMoveConstraintCenterPosition = { 0.f, 0.f, 0.f, 1.f };
		Vector4		vPlayerMoveConstraintSize = { 1.f, 1.f, 1.f, 0.f };
		Vector4		vPlayerStartLook = { 0.f, 0.f, 1.f, 0.f };
		_bool		bEnableMounting = { false };
		_bool		bEnableGliding = { false };
		_bool		bEnableAttack = { true };
		_bool		bEnableJump = { true };
		_bool		bChangeJumpSpeed = { false };
		_bool		bChangeJumpAirSpeed = { false };
		_bool		bChangeTurnLimitTime = { false };
		_bool		bChangePlayerLook = { false };
		_float		fGravityOffset = { 1.f };
		_float		fYJumpSpeed = { 1.f };
		_float		fYJumpAriSpeed = { 1.f };
		_float		fYTurnLimitTime = { 0.1f };
	}CONSTRAINTDESC;

private:
	CPlayer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CPlayer(const CPlayer& _rhs);
	~CPlayer() = default;

	HRESULT			Bind_ShaderResources();
	HRESULT			Add_Components(void* _pArg);
	HRESULT			Add_PlayerEquipments();

	HRESULT			Update_AnimByPlayerState();
	HRESULT			Update_PlayerState(_float _fTimeDelta);
	HRESULT			Update_PlayerTurn(_float _fTimeDelta);
	HRESULT			Update_RootMotion(_float _fTimeDelta);
	HRESULT         Update_Motion_By_Gliding(_float _fTimeDelta);
	HRESULT			Update_Motion_By_Mounting(_float _fTimeDelta);

	HRESULT			Handling_Jump(_float _fTimeDelta);
	HRESULT			Handling_Aimming(_float _fTimeDelta);

	HRESULT			Handling_YRootMotion(_float _fTimeDelta);
	HRESULT			Handling_XZRootMotion(_float _fTimeDelta);

	void			Handling_Skill(_float _fTimeDelta);

	void			Check_Weapon(_float _fTimeDelta);
	_bool			Is_MoveInput();
	_bool			Is_ShiftTapTrigger();
	_bool			Is_ShiftHoldTrigger();

	HRESULT			Camera_To_ADS();
	HRESULT			Camera_To_Original();

	HRESULT			Move_Player_From_ADS_Mode(_float _fTimeDelta);
	void			Add_CapturedMonster(class CMonster* _pMonster);
	void			Initialize_Event_Trigger_Function();

	/* 플레이어 에너지 게이지 Enable, Disable 조절 */
	void			_Switch_Energy();
	/* 플레이어 에너지 리젠 */
	void			_Regen_EP(_float _fTimeDelta);
	/* 스왑 스킬 상황 따라 On */
	void			_On_SwapSkill();

	void			_Handling_Eat_Food(_float _fTimeDelta);
	/* 음식 효과 적용 */
	HRESULT			_Adjust_Food_Effect();

public:
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual HRESULT			Initialize_Prototype() override;
	virtual void			Tick(_float _fTimeDelta) override;
	virtual void			Late_Tick(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual HRESULT			Render_Shadow(list<CLight*>& _lights) override;
	static CGameObject* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;

	virtual AttackDamageReation_ Take_Damage(const DAMAGEDESC& _tDamageDesc) override;

	virtual _bool			Is_Intersect(_float3& _vOutPos, _float& _fOutDist) override;


	const PLAYERSTATEDESC&	Get_Player_State_Information() { return m_tPlayerStateDesc; }
	void					Set_Player_State_Information(PLAYERSTATEDESC _tNewStateDesc) { memcpy(&m_tPlayerStateDesc, &_tNewStateDesc, sizeof(PLAYERSTATEDESC)); }

	void					Set_Current_EP(_float _fEP) { m_tPlayerStateDesc.fCurrentEP = _fEP; }
	void					Set_Max_EP(_float _fEP) { m_tPlayerStateDesc.fMaxEP = _fEP; }

	_bool					Get_SwapSkillOn() const { return m_bSwapSkillOn; }

	_float					Calc_PlayerLookAndCameraAngle();

	void					Set_CurPlayerState(PLAYER_STATE _ePlayerCurState) { m_eCurState = _ePlayerCurState; }
	void					Set_PrevPlayerState(PLAYER_STATE _ePlayerPrevState) { m_ePrevState = _ePlayerPrevState; }
	PLAYER_STATE			Get_PlayerCurState() { return m_eCurState; }
	PLAYER_STATE			Get_PlayerPrevState() { return m_ePrevState; }
	PLAYER_WEAPON_MODE		Get_CurWeaponMode() { return m_eWeaponMode; }

	class CPlayer_Equipment* Get_Mount() { return m_pMount; }
	class CPlayer_Equipment* Get_Glider() { return m_pGlider; }

	void					Set_CurWeaponMode(PLAYER_WEAPON_MODE _eMode) { m_eWeaponMode = _eMode; }

	_ulong					Get_CurrentAttackType() { return m_ulAttackType; }
	void					Set_CurrentAttackType(_ulong _ulAttackType) { m_ulAttackType = _ulAttackType; }

	void					Auto_Targeting();
	void					Auto_Targeting(Vector4 _vecTargetPos);

	void					Set_Gliding_Ground_Charging_State();
	void					Set_Gliding_Run_Sprint_State();
	void					Set_Gliding_Open_Air_State();
	void					Set_Gliding_Jump_Air_State();

	void					Set_Jump_State();
	void					Set_Air_Jump_State();
	void					Set_Mount_Jump_State();
	void					Set_Hand_Idle_State();
	void					Set_Dash_Front_State();
	void					Set_Dash_Air_State();
	void					Set_Dash_Back_Air_State();
	void					Set_Fall_State(_float _fStartYVelocity);
	void					Set_Falling_Attack_State(_float _fStartYVelocity);
	void					Reset_Hit_Throw_State();
	void					Set_Hit_Throw_Air_State();
	void					Set_Hit_Throw_Ground_State();

	void					Stop_Reset_Physics();
	void					Pause_Physics();
	void					Resume_Physics();
	void					Set_MaxYVelocity(_float _fYVelocity);
	void					Set_MinYVelocity(_float _fYVelocity);
	_float					Get_Y_Ground_Distance();
	Vector4					Get_Player_Position();

	void					Set_Disable_Move();

	_bool					Get_DodgeRollForwardWhenHit() { return m_bRollforwardDodgeWhenHit; }
	void					Set_DodgeRollForwardWhenHit(_bool _bRollforwardDodgeWhenHit) { m_bRollforwardDodgeWhenHit = _bRollforwardDodgeWhenHit; }

	void					Clear_Damping();

	void					Set_ADS();
	void					Set_TPS();
	HRESULT					Set_Camera_To_Stick_Player(_float _fTransitionTime);

	void					Set_DualGunUpperKick();

	void					Set_CutScene();
	void					Set_Constraint_For_Mini_Game(CONSTRAINTDESC _tDesc);
	void					Reset_Constraint_For_Mini_Game(_bool _bGoBackBeforePosition = true);
	_bool					Get_Upper_Kick_Move_Stop() { return m_bUpperKickMoveStop; }
	void					Set_Upper_Kick_Move_Stop(_bool _bStop) { m_bUpperKickMoveStop = _bStop; }

	_bool					Get_Enable_Gliding() { return m_bEnableGliding; }
	_bool					Get_Enable_Mounting() { return m_bEnableMounting; }
	_bool					Get_Enable_Attack() { return m_bEnableAttack; }
	_bool					Get_Enable_Jump() { return m_bEnableJump; }


	void					Set_DualGunAirSkillTime(_float _fTime) { m_fDualGunAirSkillUseAccTime = _fTime; }
	_float					Get_DualGunAirSkillTime() { return m_fDualGunAirSkillUseAccTime; }
	_float					Get_DualGunAirSkillLimitTime() { return m_fDualGunAirSkillLimitTime; }

	void					Set_InteractionObject(class CGameObject* _pInteractionObject) { m_pInteractionObject = _pInteractionObject; }
	class CGameObject*		Get_InteractionObject() { return m_pInteractionObject; }

	_float					Get_Basic_Damage() { return m_fBasic_Damage; }
	_float					Get_Sickle_Active_Skill_Range() { return m_fSickleActiveSkillRange; }

	virtual _bool			Execute_Event_Function(_int _iIndex) override;

	void					Set_Enable_Player(_bool _bEnablePlayer) { m_bEnablePlayer = _bEnablePlayer; }
	_bool					Get_Enalbe_Player() { return m_bEnablePlayer; }

	class CPlayer_Weapon*	Get_Current_Weapon() { return m_pCurWeapon; }
	void					Set_Out_Weapon();
	void					Set_In_Weapon();
	void					Add_WeaponSwapGauge(_float _fWeaponSwapGauge)
	{
		m_fWeaponSwapGauge += _fWeaponSwapGauge;
		if (m_fWeaponSwapGauge > m_fWeaponSwapMaxGauge)
		{
			m_fWeaponSwapGauge = m_fWeaponSwapMaxGauge;
		}
	}

	/* 플레이어 HP 증가 */
	void					Add_Hp(_float _fAmount);
	/* 플레이어 HP 감소 */
	void					Sub_Hp(_float _fAmount);

	/* 플레이어 대쉬 게이지 증가 */
	void					Add_SP(_float _fTickAmount);
	/* 플레이어 대쉬 게이지 감소 */
	void					Sub_SP(_float _fAmount);

	/* 플레이어 에너지 감소 */
	void					Sub_EP(_float _fAmount);
	/* 플레이어 에너지 증가 */
	void					Add_EP(_float _fTickAmount);
	/* 요리로 플레이어 에너지 증가 */
	void					Increase_EP(_float _fAmount);

	/* 플레이어 포만감 증가 */
	void					Add_Satiety(_int _iAmount);
	/* 플레이어 포만감 감소 */
	void					Sub_Satiety(_int _iAmount);

	/* 플레이어가 손에 들고 있는 무기 반환 */
	class CPlayer_Weapon* Get_Weapon(WEAPONSWAP eWeaponSwap) { return m_pWeaponSwap[eWeaponSwap]; }
	class CPlayer_Weapon* Get_WeaponEx() { return m_pWeaponEx; }
	class CPlayer_Weapon* Get_WeaponHold() { return m_pCurWeapon; }

	void Reset_Weapon_Current_Cool_Time(PLAYER_WEAPON_MODE _eWeaponMode) { m_fWeaponSkillCurrentCoolTime[(_int)_eWeaponMode] = m_fWeaponSkillCoolTime[(_int)_eWeaponMode];}
	_float Get_Weapon_Cool_Time(PLAYER_WEAPON_MODE _eWeaponMode) { return m_fWeaponSkillCoolTime[(_int)_eWeaponMode]; }
	_float Get_Weapon_Current_Cool_Time(PLAYER_WEAPON_MODE _eWeaponMode) { return m_fWeaponSkillCurrentCoolTime[(_int)_eWeaponMode]; }

	void Set_Weapon(WEAPONSWAP eWeaponSwap, class CPlayer_Weapon* pWeapon);
	void Set_WeaponEx(class CPlayer_Weapon* pWeapon);
	void Set_WeaponHold(class CPlayer_Weapon* pWeapon);
	void Set_Player_State();

	/* 플레이어 스왑 게이지 반환 */
	_float		Get_CurrentWeaponSwapGauge() const { return m_fWeaponSwapGauge; }
	_float		Get_MaxWeaponSwapGauge() const { return m_fWeaponSwapMaxGauge; }

	/* 플레이어의 스왑 스킬이 On 상태인지 */
	_bool		Is_SwapSkillOn() const { return m_bSwapSkillOn; }

	void		Add_Hp_Using_Satiety(_float _fTimeDelta);

	void		Eat_Food();
	void		Apply_Debuff(_float _fTimeDelta);
	void		Add_Debuff(DEBUFF_TYPE _eType, DEBUFFDESC _tDebuffDesc);

	void		Off_Agressive_Collider();

private:
	CModel* m_pModelBodyCom = { nullptr };
	CPhysics* m_pPhysics = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CTransform* m_pTransformModelDrawCom = { nullptr };
	CNavigation* m_pNavigationCom = { nullptr };

	PLAYERSTATEDESC									m_tPlayerStateDesc;

	PLAYER_STATE									m_ePrevState = PLAYER_STATE::Idle;
	PLAYER_STATE									m_eCurState = PLAYER_STATE::Idle;

	map<DEBUFF_TYPE, DEBUFFDESC>					m_mapDebuffs;

	class CPlayer_Weapon* m_pCurWeapon = { nullptr };		// 플레이어가 현재 장비한 무기
	class CPlayer_Equipment* m_pGlider = { nullptr };
	class CPlayer_Equipment* m_pMount = { nullptr };

	map<const wstring, class CPlayer_Equipment*>	m_mapPlayerParts;
	map<const wstring, class CPlayer_Weapon*>		m_mapPlayerWeapons;

	class CPlayer_Combat_Module* m_pCombatModule = { nullptr };
	class CPlayer_Control_Module* m_pControlModule = { nullptr };
	class CPlayer_Setting_Module* m_pSettingModule = { nullptr };

	CCollider* m_pMeleeAttackColliderCom = { nullptr };
	CCollider* m_pMeleeOBBAttackColliderCom = { nullptr };
	CCollider* m_pPlayerBodyColliderCom = { nullptr };
	CCollider* m_pPlayerBodyHitColliderCom = { nullptr };
	CCollider* m_pPlayerHitTimeillusionColliderCom = { nullptr };
	CCollider* m_pPlayerInteractionColliderCom = { nullptr };

	set<class CMonster*>							m_setCapturedMonster;

	const _float									m_fSickleAirAtkStart = { 0.f };
	_float											m_fYRootMotion_Offset = { 0.f };

	_bool											m_bOnRenderWeapon = { true };

	const _float									m_fInitialTurnLimitTime = { 0.04f };
	_float											m_fCurrentTurnLimitTime = { 0.f };

	const _float									m_fInitialDashSpeed = { 50.f };
	const _float									m_fWalkSpeed = { 3.f };
	const _float									m_fRunSpeed = { 15.f };
	const _float									m_fSprintSpeed = { 24.f };

	// To-Do : GameObject의 field값을 파일로...
	const _float									m_fYMaxPhysicsSpeed = { +50.f };
	const _float									m_fYMinPhysicsSpeed = { -70.f };
	const _float									m_fYJumpSpeed = { +40.f };
	const _float									m_fYAirJumpSeed = { +47.5f };
	const _float									m_fYInitializeSpeed = { 50.f };
	_float											m_fCurrentYJumpSpeed = { 0.f };
	_float											m_fCurrentYAirJumpSpeed = { 0.f };

	// Gliding control variable, and properties
	const _float									m_fGliding_Y_Velocity_Ground = { 100.f };
	const _float									m_fGliding_Y_Velocity_Open_Air = { 40.f };
	const _float									m_fGliding_Y_Velocity_Jump_Air = { 50.f };
	const _float									m_fGliding_Y_Velocity_Run_Sprint = { 50.f };

	// Mount control variable, and properties
	const _float									m_fYMountingModelDrawOffset = { -0.2f };
	const _float									m_fMountVelocity = { 60.f };

	// Dodge control variable, and properties
	_bool											m_bRollforwardDodgeWhenHit = { false };

	// Control Player class if false, then tick, late tick is disable
	_bool											m_bEnablePlayer = { true };

	// Hit control variable, and properties
	const _float									m_fHitThrowAirYVelocity = { 20.f };
	//Vector4											m_vHitThrow
	_float											m_fHitThrowThreasholdTime = { 0.1f };
	_float											m_fHitThrowAccTime = { 0.f };
	Vector4											m_vHitThrow = { Vector4(0.f, 0.f, 0.f, 0.f) };

	const _float									m_fHitThrowStartSpeed = { 15.f };
	_float											m_fHitThrowCurrentSpeed = { 0.f };

	_float											m_fCurrent_Hit_WaitTime = { 0.f };

	// Player constraint for mini game
	_int											m_iSavedCell = { -1 };
	_float											m_fSaveGravityOffset = { 1.f };
	_float											m_fSaveTurnLimitTime = { 0.f };
	Vector4											m_vSavedPosition = { 0.f, 0.f, 0.f, 1.f };
	Vector4											m_vConstraintCenterPosition = { 0.f, 0.f, 0.f, 1.f };
	Vector4											m_vConstraintSize = { 0.f, 0.f, 0.f, 0.f };
	Vector4											m_vPlayerSaveLook = { 0.f, 0.f, 1.f, 0.f };
	_bool											m_bEnableConstraint = { false };
	_bool											m_bEnableMounting = { true };
	_bool											m_bEnableGliding = { true };
	_bool											m_bEnableAttack = { true };
	_bool											m_bEnableJump = { true };

	// Time illusion control variable, and properties
	const _float									m_fCoolTimeForTimeillusion = { 20.f };
	_float											m_fCurrentCoolTimeForTimeillusion = { 0.f };
	_bool											m_bDashForTimeillusion = { false };
	_bool											m_bUseTimeillusion = { false };
	class CSkillObject* m_pTimeillusionSkillObject = { nullptr };

	// ADS aimming control variable, and properties
	_float											m_fADSRotationVelocity = { 0.f };
	Vector4											m_vADSTargetLook = { 0.f, 0.f, 0.f, 0.f };

	// Player turn control variable, and properties
	const _float				m_fStartDeg = { 0.f };
	_bool						m_bSmoothRotation = { false };
	_float						m_fCurrentRoationVelocity = { 0.f };
	_vector						m_vTargetLook;

	// Player current weapon state variable
	PLAYER_WEAPON_MODE			m_eRightWeaponMode = { PLAYER_WEAPON_MODE::WEAPON_END };
	PLAYER_WEAPON_MODE			m_eLeftWeaponMode = { PLAYER_WEAPON_MODE::WEAPON_END };
	PLAYER_WEAPON_MODE			m_eExWeaponMode = { PLAYER_WEAPON_MODE::WEAPON_END };
	PLAYER_WEAPON_MODE			m_eWeaponMode = { PLAYER_WEAPON_MODE::GUN };
	MSG_ATTACK_TYPE				m_eAttackType = { MSG_ATTACK_TYPE::NONE }; // need to erase
	_ulong						m_ulAttackType = { 0ul };

	const _float				m_fAutoTargetingLimitDistance = { 10.f };

	// Player skill control, and properties
	const _float				m_fDualGunAirSkillMoveSpeed = { 12.f };
	const _float				m_fDualGunAirSkillLimitTime = { 3.f };
	_float						m_fDualGunAirSkillUseAccTime = { 0.f };

	// Determin Y jump speed when sick normal attack05(floating, upper attack)
	const _float				m_fSickleMelee05AttackYOffset = { 0.05f };

	ViewMode					m_eViewMode = { ViewMode::TPS };

	_float3						m_vADS_Camera_Eye;
	_float3						m_vADS_Camera_At;
	
	const _float				m_fBasic_Damage = { 2000.f };//{ 20000.f };//{ 1000.f };

	const _float				m_fSickleActiveSkillRange = { 80.f };

	const _float				m_fUseEnergyCount = { 10.f };

	_float						m_fYDashAccTime = { 0.f };

	vector<function<void()>>	m_vecEventFunctions;

	CPlayer_Weapon* m_pWeaponSwap[WEAPONSWAP_END] = { nullptr, };	// 스왑할 수 있는 무기 2개
	CPlayer_Weapon* m_pWeaponEx = { nullptr };						// 플레이어가 인벤토리에 가지고 있는 여분의 무기 1개
	_float											m_fWeaponSwapGauge = { 0.f };					// 무기 스왑 게이지 -> 다 찬 상태에서 스왑하면 스왑한 무기 교체 스킬 발동
	_float											m_fPrevWeaponSwapGauge = { 0.f };				// 무기 스왑 게이지가 다 찼는지 판단
	_float											m_fWeaponSwapMaxGauge = { 100.f };
	_bool											m_bSwapSkillOn = { false };

	_float											m_fWeaponSkillCurrentCoolTime[(_int)PLAYER_WEAPON_MODE::WEAPON_END];
	const _float									m_fWeaponSkillCoolTime[(_int)PLAYER_WEAPON_MODE::WEAPON_END] = { 10.f, 10.f, 10.f, 10.f, 10.f };

	// Test
	_bool											m_bTestOn = { false };
	
private:
	virtual void OnCollisionBegin(class CCollider* _pSelf, class CCollider* _pOther, _float _fTimeDelta) override;
	virtual void OnCollision(class CCollider* _pSelf, class  CCollider* _pOther, _float _fTimeDelta) override;
	virtual void OnCollisionEnd(class CCollider* _pSelf, class CCollider* _pOther, _float _fTimeDelta) override;

	void		Handling_HitThrow(_float _fTimeDelta);
	void		Change_Collision_Transform_By_Attack_Animation();
	_bool		Find_Nearest_Monster(_float4* _pOutPos);
	_float		Get_ADSAngle();

	void		Handling_CapturedMonster();

	void		Create_WeaponEffects();
	void		Create_WeaponEffect_RightHand(wstring _wstrWeaponEffect, _float3 _vFixRUL, _bool bMaintainPointerPos = false, _float3 _vFixPlayerRUL = { 0.f, 0.f, 0.f }, _bool _bMaintainWorldMatrix = false, _bool _bMaintainPivotPointerPos = false);
	void		Create_WeaponEffect_LeftHand(wstring _wstrWeaponEffect, _float3 _vFixRUL, _bool bMaintainPointerPos = false, _float3 _vFixPlayerRUL = { 0.f, 0.f, 0.f }, _bool _bMaintainWorldMatrix = false);

private:
	_float				m_fEPRegenTimeAcc = 0.f;

	_bool				m_bSatietyHPRegen = false;
	_bool				m_bStateNoFight = false;
	_float				m_fSatietyWaitTimeAcc = { 0.f };
	_float				m_fSatietyWaitTime = { 5.f };
	_float				m_fSatietyHpRegenCoolTimeAcc = { 0.f };
	_float				m_fSatietyHpRegenCoolTime = { 2.f };
	_float				m_fSatietyRegenHP = { 0.f };	// 포만감 수치를 통해서 회복한 HP

	_bool				m_bCanEat = { true };			// 먹을 수 있는 상태인지
	_bool				m_bEating = { false };			// 지금 음식을 먹고 있는 중인지
	_bool				m_bHitWhenEating = { false };	// 먹다가 맞았을 때
	_float				m_fEatingTime = { 1.5f };		// 먹는 지속 시간
	_float				m_fEatingTimeAcc = { 0.f };

	_bool				m_bUpperKickMoveStop = { false };

	Vector4				m_vBeforePosition = { 0.f, 0.f, 0.f, 1.f };

	/* 다른 객체와 상호작용 중인가? */
	class CGameObject* m_pInteractionObject = { nullptr };

public:
	_float	Get_Player_Use_Energy_Count() { return m_fUseEnergyCount; }

	void	Set_CanEat(_bool _bCanEat) { m_bCanEat = _bCanEat; }

	_float	Get_EatingTimeAcc() { return m_fEatingTimeAcc; }
	_float	Get_EatingTime() { return m_fEatingTime; }

#ifdef _DEBUG
	_int	Get_Navigation_Mesh_Index();
	Vector4 Get_Player_Physics_Velocity();
#endif // _DEBUG
};
END