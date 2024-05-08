#pragma once

#include "InteractionObject.h"

BEGIN(Client)

class CFieldItem final : public CInteractionObject
{
private:
	CFieldItem(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CFieldItem(const CFieldItem& _rhs);
	virtual ~CFieldItem() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;
	virtual HRESULT Initialize_Prototype() override;
	virtual void Tick(_float _fTimeDelta) override;
	virtual void Late_Tick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void OnCollisionBegin(CCollider* _pSelf, CCollider* _pOther, _float _fTimeDelta) override;

	virtual void Interaction() override;
	virtual void Take_Notify(OBSERVER_NOTIFY_TYPE _eType, any _arg) override;
public:
	HRESULT Rooting_Item();

	virtual HRESULT Bind_ShaderResource() override;

private:
	HRESULT _Add_Components(void* _pArg);

public:
	static CFieldItem* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;

private:
	_uint			m_iItemID = { 0 };
	_float			m_fSpawnTime = { 0.f };
	_float			m_fProgressTime = { 0.f };

	_float			m_fAlphaDiscard = { 0.f };

};

END