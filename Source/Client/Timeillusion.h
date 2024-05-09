#pragma once

#include "SkillObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CRenderer;
class CTransform;
class CVIBuffer_Trail_Rect;
END

BEGIN(Client)
class CTimeillusion final : 
	public CSkillObject
{
private:
	CTimeillusion(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CTimeillusion(const CTimeillusion& _rhs);
	virtual ~CTimeillusion() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Tick(_float _fTimeDelta) override;
	virtual void Late_Tick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;

	static CGameObject* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);

	CGameObject* Get_Owner() { return m_pOwner; }

	void OnCollisionBegin(CCollider* _pSelf, CCollider* _pOther, _float _fTimeDelta) override;
	void OnCollisionEnd(CCollider* _pSelf, CCollider* _pOther, _float _fTimeDelta) override;

private:
	HRESULT Add_Components(void* _pArg);
	void _Set_MonsterLayer_TimeScale(_float _fTimeScale);

private:
	CCollider*			m_pColliderCom = { nullptr };

	_float4x4			m_WorldMatrix = { IDENTITY_MATRIX };
	_float4x4			m_PivotMatrix = { IDENTITY_MATRIX };

	_float				m_fWaitTime = { 0.1f };

	const _float		m_fCollisionRadius = {25.f};

	_float				m_fBeforeMiddleGrey = { 0.f };
	_float				m_fBeforeLuminanceWhite = { 0.f };
};
END