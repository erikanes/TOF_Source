#pragma once

#include "GameObject.h"
#include "Observer.h"

BEGIN(Engine)
class CTransform;
class CRenderer;
class CVIBuffer_Rect;
class CTexture;
class CShader;
class CCollider;
END

BEGIN(Client)

class CAbyssPortal final : public CGameObject, public IObserver
{
private:
	CAbyssPortal(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CAbyssPortal(const CAbyssPortal& _rhs);
	virtual ~CAbyssPortal() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Tick(_float _fTimeDelta) override;
	virtual void Late_Tick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void OnCollisionBegin(CCollider* _pSelf, CCollider* _pOther, _float _fTimeDelta) override;

	virtual void Take_Notify(OBSERVER_NOTIFY_TYPE _eType, any _arg) override;

private:
	HRESULT _Add_Components(void* _pArg);
	HRESULT _Bind_ShaderResources();

public:
	static CAbyssPortal* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;

private:
	CTransform* m_pTransformCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };

	_float m_fAccumulatedTime = { 0.f };
	_float4 m_vColor = { 1.f, 1.f, 1.f, 1.f };

	function<void()> m_funcCollisionBegin = []() {};
};

END