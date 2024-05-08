#pragma once

#include "Character.h"
#include "Observer.h"
#include "SkillObject_Monster_Shield.h"

BEGIN(Engine)
class CShader;
class CPhysics;
class CRenderer;
class CTransform;
class CNavigation;
class CFSM_Controller;
END

BEGIN(Client)
class CMonster abstract : public CCharacter, public IObserver
{
public:
	typedef struct tMonsterDesc
	{
		wstring			wszModelPrototype = { L"" };
		AIPATROLDESC	tAiPatrolDesc;
		CHARACTERDESC	tCharacterDesc;
	}MONSTERNDESC;

private:
	using DropItemFunc = void(CMonster::*)();

protected:
	CMonster(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CMonster(const CMonster& _rhs);
	virtual ~CMonster() = default;

public:
	virtual HRESULT			Initialize_Prototype();
	virtual HRESULT			Initialize(void* _pArg);
	virtual void			Tick(_float _fTimeDelta);
	virtual void			Late_Tick(_float _fTimeDelta);
	virtual HRESULT			Render();
	virtual HRESULT			Render_Shadow(list<CLight*>& _lights) override;

	_bool					Get_Captured() { return m_bAirCpaturedByPlayer; }
	void					Set_Captured(_bool _bCaptured) { m_bAirCpaturedByPlayer = _bCaptured; }

	void					Set_CapturedCoolTime(_float _fCapturedCoolTime) { m_fCapturedCoolTime = _fCapturedCoolTime; }
	_float					Get_CapturedCoolTime() { return m_fCapturedCoolTime; }

	virtual _bool			Trigger_Floating();
	virtual _bool			Trigger_KnockOver();
	virtual _bool			Trigger_Falling();

	virtual void			Create_Reward_Box();

	virtual _bool Execute_Event_Function(_int _iIndex) override { return true; }

	virtual HRESULT Begin(any _pArg) override { return S_OK; };	


	const MONSTERSTATEDESC& Get_Monster_State_Information() { return m_tMonsterStateDesc; }
	void					Set_Monster_State_Information(MONSTERSTATEDESC _tNewMonsterDesc) { memcpy(&m_tMonsterStateDesc, &_tNewMonsterDesc, sizeof(MONSTERSTATEDESC)); }

	void					Set_Monster_State_Max_Defense_Gauge(_float _fGaguge) { m_tMonsterStateDesc.fMaxDefenseGauge = _fGaguge; }
	void					Set_Monster_State_Current_Defense_Gauge(_float _fGaguge) { m_tMonsterStateDesc.fCurrentDefenseGauge = _fGaguge; }

	void					Set_DefenseGaugeOn(_bool _bDefenseGaugeOn) { m_bDefenseGaugeOn = _bDefenseGaugeOn; }
	_bool					Is_DefenseGaugeOn() { return m_bDefenseGaugeOn; }

	_uint					Get_HpLine() { return m_iHPLine; }

	void					Set_Engage(_bool _bEngage) { m_bCurrentEngae = _bEngage; }
	_bool					Is_Engage() { return m_bCurrentEngae; }

	_float					Get_Weak_Time() { return m_fWeak_Time; }

	ELEMENTAL_TYPE			Get_ElementalType() { return m_eElementalType; }
	ELEMENTAL_TYPE			Get_ElementalType_Weak() { return m_eElementalType_Weak; }

	void					Set_Prepare_Attack(_bool _bPrepareAttack) { m_bPrepareAttack = _bPrepareAttack; }
	_bool					Get_Prepare_Attack() { return m_bPrepareAttack; }

	CCollider*				Get_Monster_Hit_Body_Collider() { return m_pMonsterHitBodyCollider; }

	void					Set_Position(Vector4 _vNewPosition, _bool _bApplyNavigation = true);
	void					Set_WorldMatrix(Matrix _matWorld, _bool _bApplyNavigation = true);

	void					Set_EnableMonster(_bool _bEnable) { m_bEnableMonster = _bEnable; }
	_bool					Get_EnableMonster() { return m_bEnableMonster; }

	Vector4					Get_Basic_Stand_Look() { return m_vBasicStartLook; }

	// Monster���� ���������� ������ �ϴ� �ʼ����� ������Ʈ��
protected:
	CPhysics*			m_pPhysics = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	CTransform*			m_pDrawTransformCom = { nullptr };
	CModel*				m_pModelCom = { nullptr }; // ������ ����ϴ� ����. TOF���� ���ҽ����� ���� ���͵��� ���Ⱑ �پ� �־ �״�� ����ϸ��.
	CFSM_Controller*	m_pFSMControllerCom = { nullptr };
	CNavigation*		m_pNavigationCom = { nullptr };

	CCollider*			m_pMonsterTimeillusionCheckCollider = { nullptr };
	CCollider*			m_pMonsterHitBodyCollider = { nullptr };
	CCollider*			m_pPrepareAttackColliderSphere = { nullptr }; // For. Time illusion monster attack check.
	CCollider*			m_pPrepareAttackColliderOBB = { nullptr };
	const _float		m_fTimeSlowForTimeillusion = { 0.01f };
	_bool				m_bApplyTimeSlowForTimeillusion = { false };

	_bool				m_bEnableMonster = { true }; // if false, then tick, and late tick disable

	class CSkillObject* m_pSkillObject_Shield = { nullptr };

	MONSTERSTATEDESC	m_tMonsterStateDesc;
	ELEMENTAL_TYPE		m_eElementalType = { ELEMENTAL_END }; 			// ���� �Ӽ�
	ELEMENTAL_TYPE		m_eElementalType_Weak = { ELEMENTAL_END };		// ���� �Ӽ�
	_bool				m_bDefenseGaugeOn = { false };					// ���潺 �������� Ȱ��ȭ �ƴ���
	_uint				m_iHPLine = { 0 };								// ���� ��

	_float				m_fCurrent_Detection_Range = { 1.f };
	_float				m_fPeace_Detection_Range = { 1.f };
	_float				m_fEngae_Detection_Range = { 1.f };

	_float				m_fMelee_Attack_Distance = { 1.f };
	_float				m_fRange_Attack_Distance = { 1.f };

	_float				m_fWeak_Time = { 0.f };
	_float				m_fWalk_Velocity = { 1.f };
	_float				m_fRun_Velocity = { 1.f };

	_bool				m_bCurrentEngae = { false };


	Vector4				m_vBasicStartLook = { 0.f, 0.f, 1.f, 0.f }; //���Ͱ� ó�� add_components�� �߰��Ǿ��� �� ������ Look

	// For. Air combo combat
	_bool				m_bAirCpaturedByPlayer = { false };

	_float				m_fCapturedCoolTime = { 0.f };
	// Floating �Ǿ��� ��, ���� ������ ������ ������ floating �Ǿ� ���� �� �ִ� �ð�

	_float				m_fInitialFallingSpeed = { 0.f };

	_float				m_fHitSlowAccumulatedTime = { 0.f };

	_bool				m_bMonsterDead = { false }; // instance�� �Ҹ��� �ƴ϶�, ������������ ������ dead�� ��Ÿ��.

	_ulong				m_ulAttackType = { 0ul };

	// Rimlight
	_bool				m_bOnHitRimLight = { false };
	Vector4				m_vHitRimLightColor = { 0.f, 0.f, 0.f, 0.f };
	_bool				m_bOnRageRimLight = { false };
	Vector4				m_vRageRimLightColor = { 0.f, 0.f, 0.f, 0.f };
	_float				m_fRageRimLightProgressTime = { 0.f };


	// used for playing done for dead animation.
	_float				m_fMonster_Dead_Wait_Time = { 0.f };

	// Dissolve
	_float				m_fAfterDeadTime = { 0.f };

	_bool				m_bPrepareAttack = { false };

	//RimLight			
	_float				m_fRimPower = { 0.f };
public:
	//Monster���� �������� ��Ҹ� �����ϴ� ��ü�� ���� ���� �������� �ʵ��� ����.
	virtual CGameObject*			Clone(void* _pArg) = 0;
	virtual void					Free() override;

	virtual void					OnCollisionBegin(class CCollider* _pSelf, class CCollider* _pOther, _float _fTimeDelta) override;
	virtual void					OnCollision(class CCollider* _pSelf, class  CCollider* _pOther, _float _fTimeDelta) override;
	virtual void					OnCollisionEnd(class CCollider* _pSelf, class CCollider* _pOther, _float _fTimeDelta) override;

	virtual void					Take_Notify(OBSERVER_NOTIFY_TYPE _eType, any _arg) override;

	CNavigation*					Get_NavigationCom() { return m_pNavigationCom; }
	CTransform*						Get_TransformCom() { return m_pTransformCom; }
	CTransform*						Get_DrawTransformCom() { return m_pDrawTransformCom; }



	CModel*							Get_ModelCom() { return m_pModelCom; }

	_bool							Get_MonsterBodyDead() { return m_bMonsterDead; }

	virtual AttackDamageReation_	Take_Damage(const DAMAGEDESC& _tDamageDesc) override;

	void							Set_CurrentAttackType(_ulong _ulAttackType) { m_ulAttackType = _ulAttackType; }
	_ulong							Get_CurrentAttackType() { return m_ulAttackType; }

	void							Pause_Physics();
	void							Resume_Physics();

	_float							Get_Monster_Dead_Wait_Time() { return m_fMonster_Dead_Wait_Time; }

public:
	_bool							Is_MonsterBodyDead() { return m_bMonsterDead; }


	_float							Get_Current_Detection_Range() { return m_fCurrent_Detection_Range; }
	_float							Get_Peace_Detection_Range() { return m_fPeace_Detection_Range; }
	_float							Get_Engage_Detection_Range() { return m_fEngae_Detection_Range; }

	_float							Get_Walk_Velocity() { return m_fWalk_Velocity; }
	_float							Get_Run_velocity() { return m_fRun_Velocity; }

	_float							Get_Melee_Attack_Distance() { return m_fMelee_Attack_Distance; }
	_float							Get_Range_Attack_Distance() { return m_fRange_Attack_Distance; }


	void							Set_Current_Detection_Range(_float _fRange) { m_fCurrent_Detection_Range = _fRange; }

protected:
	/* ������ */
	void					Apply_Damage(_float _fDamage);
	virtual HRESULT			Create_Skill_Object_Shield(CSkillObject_Monster_Shield::MONSTERSHIELDDESC _tShieldDesc);
	void					Destroy_Skill_Object_Shield();
	
	HRESULT					Camera_Shaking(CAMERA_SHAKING_TYPE eShakingType, _float _fShakingStrengthen, _float fShakingTime, _float _fPlayerLength);

	// ��Ʈ ����Ʈ
	void			_Create_Hit_WeapoenType(const DAMAGEDESC& _tDamageDesc);

	void			Enable_Hit_RimLight();
	void			Calculate_Hit_RimLight(_float _fTimeDelta);
	void			Enable_Rage_RimLight(_float _fTime = 2.f);
	void			Calculate_Rage_RimLight(_float _fTimeDelta);
	virtual HRESULT Bind_ShaderResources();
	_bool			IsIn_Distance_To_Camera();

	// ������ ���
	virtual void	Drop_Item();

private:
	DropItemFunc m_funcDropItem = { nullptr };
};
END
