#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CTransform;
class CRenderer;
class CCollider;
END

BEGIN(Client)

class CTrigger_Object : public CGameObject
{
private:
	CTrigger_Object(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CTrigger_Object(const CTrigger_Object& _rhs);
	virtual ~CTrigger_Object() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;
	virtual HRESULT Initialize_Prototype() override;
	virtual void Tick(_float _fTimeDelta) override;
	virtual void Late_Tick(_float _fTimeDelta) override;

	virtual void OnCollisionBegin(CCollider* _pSelf, CCollider* _pOther, _float _fTimeDelta);
	virtual void OnCollision(CCollider* _pSelf, CCollider* _pOther, _float _fTimeDelta);
	virtual void OnCollisionEnd(CCollider* _pSelf, CCollider* _pOther, _float _fTimeDelta);

private:
	HRESULT _Add_Components(void* _pArg);
	void Bind_Function(const TRIGGER_DESC* _pDesc);

public:
	static CTrigger_Object* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg);
	virtual void Free() override;

private:
	CTransform* m_pTransformCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };

	function<void(CCollider*, CCollider*, _float)> m_onCollisionBegin;
	function<void(CCollider*, CCollider*, _float)> m_onCollision;
	function<void(CCollider*, CCollider*, _float)> m_onCollisionEnd;
};

END