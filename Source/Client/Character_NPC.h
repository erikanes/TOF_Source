#pragma once

#include "Character.h"
#include "Observer.h"

BEGIN(Engine)
class CModel;
class CShader;
class CPhysics;
class CRenderer;
class CCollider;
class CTransform;
class CNavigation;
class CFSM_Controller;
END

BEGIN(Client)
class CCharacter_NPC abstract : public CCharacter, public IObserver
{
protected:
	CCharacter_NPC(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCharacter_NPC(const CCharacter_NPC& _rhs);
	virtual ~CCharacter_NPC() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* _pArg) override;
	virtual void		Tick(_float _fTimeDelta) override;
	virtual void		Late_Tick(_float _fTimeDelta) override;
	virtual HRESULT		Render() override;
	virtual HRESULT		Render_Shadow(list<CLight*>& _lights) override;

	virtual void		OnCollisionBegin(class CCollider* _pSelf, class CCollider* _pOther, _float _fTimeDelta) override;
	virtual void		OnCollision(class CCollider* _pSelf, class  CCollider* _pOther, _float _fTimeDelta) override;
	virtual void		OnCollisionEnd(class CCollider* _pSelf, class CCollider* _pOther, _float _fTimeDelta) override;


	virtual void		Change_Animation_For_Dialog(const string& _szAnimName);
	virtual void		Interaction();
	virtual void		Take_Notify(OBSERVER_NOTIFY_TYPE _eType, any _arg) override;

public:
#pragma region Getter
	CNavigation*	Get_NavigationCom() { return m_pNavigationCom; }
	CTransform*		Get_TransformCom() { return m_pTransformCom; }
	CTransform*		Get_DrawTransformCom() { return m_pDrawTransformCom; }
	CModel*			Get_ModelCom() { return m_pModelCom; }
#pragma endregion

#pragma region Setter
	void Set_DialogID(_uint _iID) { m_iDialogID = _iID; }
#pragma endregion

#pragma region EventCall_By_Dialog_Manager
	virtual void Change_FSM_State(string _szTagFSM, _int _eMsgType) {}
#pragma endregion

protected:
	virtual HRESULT Bind_ShaderResources();
	virtual HRESULT Handling_RootMotion(_float _fTimeDelta);

	void Set_Turn(Vector4 _vTowardTarget);
	void Handling_Turn(_float _fTimeDelta);

	void Popup_UI();
	void Disable_Popup_UI();

private:
	HRESULT _Add_Components(void* _pArg);

public:
	virtual CGameObject* Clone(void* _pArg) = 0;
	// NPC들의 공통적인 요소를 관리하는 객체이므로 따로 생성 X
	virtual void			Free() override;

	_float Get_Walk_Velocity() { return m_fWalkVelocity; }
	_float Get_Run_Velocity() { return m_fRunVelocity; }

	Vector4	Get_Idle_Look() { return m_vIdleLook; }

protected:
	CGameObject* Create_Npc_Effect(wstring _wstrEffect, const _float4x4* _pWorldMatrix = nullptr, _float3 _vFixPos = { 0.f, 0.f, 0.f }, _bool _bMaintainPointerPos = false, _bool _bMaintainPivotPointerPos = false, const _float4x4* _pBoneMatrix = nullptr, _float4x4 _PivotMatrix = IDENTITY_MATRIX, _bool* _pDead = nullptr, _bool _bMaintainWorldMatrix = false);

protected:
	CGameObject* m_pNpcEffect = { nullptr };

	CShader* m_pShaderCom = { nullptr };
	CPhysics* m_pPhysicsCom = { nullptr };
	CTransform* m_pDrawTransformCom = { nullptr };

	CFSM_Controller* m_pFSMControllerCom = { nullptr };
	CNavigation* m_pNavigationCom = { nullptr };

	CModel*		m_pModelCom = { nullptr };
	CCollider* m_pColliderSolidBodyCom = { nullptr }; // 플레이어와 충돌이 가능한 콜라이더
	CCollider* m_pColliderInteraction = { nullptr }; // 상호작용을 위한 콜라이더

	_uint m_iDialogID = { 0 }; // 대화 시도시 출력할 DialogID

protected:
	_float		m_fWalkVelocity = { 0.f };
	_float		m_fRunVelocity = { 0.f };
	Vector3		m_vSolidBodyColliderSize = { 1.f, 1.f, 1.f };
	Vector3		m_vSolidBodyColliderPosition = { 0.f, 0.f, 0.f };

	_bool		m_bExternAnimationControl = { false };

	_bool		m_bTurn = { false };
	_float		m_fTurnTime = { 0.f };
	_float		m_fTurnVelocity = { 0.f };
	Vector4		m_vTurnTowardTarget = { 0.f, 0.f, 0.f, 0.f };

	Vector4		m_vIdleLook = { 0.f, 0.f, 0.f, 0.f };

	_float4		m_vHairColor = { 1.f, 1.f, 1.f, 1.f };
};
END