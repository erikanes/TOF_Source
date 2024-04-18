#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Observer.h"

BEGIN(Engine)
class CTransform;
class CCollider;
class CModel;
class CShader;
class CRenderer;
END

BEGIN(Client)

class CInteractionObject abstract : public CGameObject, public IObserver
{
public:
	typedef struct tInteractionObjectDesc
	{
		Vector4 vBatchPosition = { 0.f, 0.f, 0.f, 1.f };
		Vector4 vBatchRotationDegree = { 0.f, 0.f, 0.f, 0.f };
	}INTERACTIONOBJECTDESC;


protected:
	CInteractionObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CInteractionObject(const CInteractionObject& _rhs);
	virtual ~CInteractionObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Tick(_float _fTimeDelta) override;
	virtual void Late_Tick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void OnCollisionBegin(CCollider* _pSelf, CCollider* _pOther, _float _fTimeDelta) override;
	virtual void OnCollision(CCollider* _pSelf, CCollider* _pOther, _float _fTimeDelta) override;
	virtual void OnCollisionEnd(CCollider* _pSelf, CCollider* _pOther, _float _fTimeDelta) override;

	virtual void Interaction() = 0;
	virtual void Take_Notify(OBSERVER_NOTIFY_TYPE _eType, any _arg) override;

protected:
	virtual HRESULT Bind_ShaderResource();

	void Popup_UI_Item(_uint _iID);
	void Popup_UI_NPC(_uint _iID);
	void Popup_UI_Interaction(wstring _wstrMessage);
	void Disable_Popup_UI();

	CGameObject* Create_FieldItem_Effect(wstring _wstrEffect, _float _fCameraDistance, _float3 _vFixPos, _bool* _pDead = nullptr);
	void Enable_FieldItem_Effect(CGameObject* _pObject, _float _fCameraDistance, _float3 _vFixPos, _bool* _pDead = nullptr);
	HRESULT		Create_Effect(wstring _wstrEffect, const _float4x4* _pWorldMatrix = nullptr, _float3 _vFixPos = { 0.f, 0.f, 0.f }, _bool _bMaintainPointerPos = false, _bool _bMaintainPivotPointerPos = false, const _float4x4* _pBoneMatrix = nullptr, _float4x4 _PivotMatrix = IDENTITY_MATRIX, _bool* _pDead = nullptr, _bool _bMaintainWorldMatrix = false);
	CGameObject* Create_Effect_Ptr(wstring _wstrEffect, const _float4x4* _pWorldMatrix = nullptr, _float3 _vFixPos = { 0.f, 0.f, 0.f }, _bool _bMaintainPointerPos = false, _bool _bMaintainPivotPointerPos = false, const _float4x4* _pBoneMatrix = nullptr, _float4x4 _PivotMatrix = IDENTITY_MATRIX, _bool* _pDead = nullptr, _bool _bMaintainWorldMatrix = false);

	_bool IsIn_Distance_To_Camera();

	HRESULT _Add_Components(void* _pArg);

public:
	virtual void Free() override;

protected:
	CTransform* m_pTransformCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

	//¿Ã∆Â∆Æ
	_float			m_fEffectTime = { 0.f };
	class CGameObject* m_pFieldEffect = { nullptr };
	_float			m_fEffectPosY = { 0.f };

	_float4x4 m_matWorld = IDENTITY_MATRIX;
	_float4x4 m_matPivot = IDENTITY_MATRIX;
};

END