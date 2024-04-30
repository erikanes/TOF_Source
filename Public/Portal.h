#pragma once

#include "InteractionObject.h"

BEGIN(Client)

class CPortalEffect;

class CPortal final : public CInteractionObject
{
private:
	CPortal(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CPortal(const CPortal& _rhs);
	virtual ~CPortal() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Tick(_float _fTimeDelta) override;
	virtual void Late_Tick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow(list<CLight*>& _lights) override;

	virtual void OnCollisionBegin(CCollider* _pSelf, CCollider* _pOther, _float _fTimeDelta) override;

	virtual void Take_Notify(OBSERVER_NOTIFY_TYPE _eType, any _arg) override;

public:
	virtual void Interaction() override;

protected:
	virtual HRESULT Bind_ShaderResource() override;

private:
	HRESULT _Add_Components(void* _pArg);

public:
	static CPortal* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;

private:
	LEVEL m_eDestLevel = { LEVEL_END };
	wstring m_wstrInteractionText;

	CPortalEffect* m_pPortalEffect = { nullptr };

	_float m_fDissolveAlpha = { 0.f };
	_uint m_iShaderPass = { 0 };
};

END