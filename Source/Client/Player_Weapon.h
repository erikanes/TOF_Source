#pragma once

#include "Player_Equipment.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CRenderer;
class CTransform;
END


BEGIN(Client)
class CPlayer_Weapon abstract : public CPlayer_Equipment
{
public:
	enum class WEAPON_STATE { WEAPON_IN, WEAPON_OUT, WEAPON_DISABLE, WEAPON_NONE };

	typedef struct tagWeaponStatus
	{
		_float fBaseAttack = { 0.f };
		_float fHealth = { 0.f };
		_float fCritical = { 0.f };
		_float fSkillMagnification = { 0.f };
	} WEAPON_STATUS;

protected:
	CPlayer_Weapon(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CPlayer_Weapon(const CPlayer_Weapon& _rhs);
	virtual ~CPlayer_Weapon() = default;

public:
	_vector Get_RightWeapon_TransformState(_uint _eState);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Tick(_float _fTimeDelta) override;
	virtual void Late_Tick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual CGameObject* Clone(void* _pArg)	= 0;
	virtual void Free() override;

	virtual void Out_Weapon() = 0;
	virtual void In_Weapon() = 0;
	virtual void Play_HitSound(GameObjectID _iID) = 0;

	virtual void Disable_Weapon();
	
	void Set_Dummy_Render_Model(_bool _bRenderModel) { m_bOnlyRenderDummyModel = _bRenderModel; }

	WEAPON_STATE Get_Weapon_State() { return m_eWeaponState; }
	ElementType Get_Element() const { return m_eElementType; }

	void	Create_WeaponEffect_RightHand(wstring _wstrWeaponEffect, _float3 _vFixRUL ={0.f,0.f,0.f}, _bool bMaintainPointerPos = false, _float3 _vFixPlayerRUL = { 0.f,0.f,0.f }, _bool _bMaintainWorldMatrix = false, _bool _bMaintainPivotPointer = false, _bool _bMaintainPivotPointerPos =false);
	void	Create_WeaponEffect_LeftHand(wstring _wstrWeaponEffect, _float3 _vFixRUL = { 0.f,0.f,0.f }, _bool bMaintainPointerPos = false, _float3 _vFixPlayerRUL = { 0.f,0.f,0.f }, _bool _bMaintainWorldMatrix = false, _bool _bMaintainPivotPointer = false);

	const WEAPON_STATUS&	Get_WeaponStatus() const { return m_tWeaponStatus; }
	PLAYER_WEAPON_MODE		Get_Weapon_Mode() { return m_eWeaponMode; }

	const EQUIPMENT_INFO_DESC& Get_Weapon_Desc() { return m_tWeaponInfoDesc; }

	pair<_float, _bool> Calculate_Damage(_float _fBasiceDamage);

protected:
	virtual HRESULT Bind_ShaderResources();
	virtual void Calculate_Dissolve(_float _fTimeDelta);
	virtual void Spawn_DisableEffect() {}

protected:
	EQUIPMENT_INFO_DESC			m_tWeaponInfoDesc;
	PLAYER_WEAPON_MODE			m_eWeaponMode = { PLAYER_WEAPON_MODE::WEAPON_END };

	const _float4x4*			m_pParentMatrix = { nullptr };

	_float4x4					m_matLeftHandWorldMatrix = { IDENTITY_MATRIX };
	_float4x4					m_matRightHandWorldMatrix = { IDENTITY_MATRIX };

	static inline WEAPON_STATE	m_eWeaponState = { WEAPON_STATE::WEAPON_NONE };

	ElementType					m_eElementType = { ELEMENT_NONE };

	_float						m_fWeaponOnEffectTime = { 0.f };

	static inline _float		m_fDissolveAlpha = { 1.f };
	static inline _float		m_fDissolveProgressTime = { 0.f };
	static inline _bool			m_bSpawnableOutWeaponEffect = { false };
	_float4						m_vDissolveColor = _float4(1.f, 1.f, 1.f, 1.f);

	_bool						m_bOnlyRenderDummyModel = { false };
	// Dissolve를 피하면서, dummy render 모델만 사용하기 위한 변수

	WEAPON_STATUS				m_tWeaponStatus;
};
END