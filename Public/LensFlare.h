#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTransform;
class CShader;
class CRenderer;
class CCollider;
class CTexture;
END

BEGIN(Client)
class CVIBuffer_LensFlare;

class CLensFlare : public CGameObject
{
private:
	typedef struct tagCorona
	{
		_float4 vPosition = _float4(0.f, 0.f, 0.f, 1.f);
		_float4 vScaleRotate = _float4(1.f, 1.f, 0.f, 0.f);
		_float4 vColor = _float4(1.f, 1.f, 1.f, 1.f);
		_float fBrightness = 0.f;
	} LENSFLARE_CORONA;

	typedef struct tagFlare
	{
		_float4 vOffset = _float4(0.f, 0.f, 0.f, 1.f);
		_float4 vScaleRotate = _float4(1.f, 1.f, 0.f, 0.f);
		_float4 vColor = _float4(1.f, 1.f, 1.f, 1.f);
		_uint iTextureIndex = { 0 };
	} LENSFLARE_FLARE;

private:
	CLensFlare(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CLensFlare(const CLensFlare& _rhs);
	virtual ~CLensFlare() override;

public:
	virtual HRESULT Initialize(void* _pArg) override;
	virtual HRESULT Initialize_Prototype();
	virtual void Tick(_float _fTimeDelta) override;
	virtual void Late_Tick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	void _FindLight(_float _fTimeDelta);
	void _Update(_float _fTimeDelta);
	void _Late_Update(_float _fTimeDelta);

	HRESULT _Render();

	void _Update_Transform();
	_bool _IsInViewport();

	Vector2 _Transform_Light();
	
	HRESULT _Add_Components(void* _pArg);
	_float _Calculate_CdotL();

	void _Set_Corona(const Vector2& _vPos);
	void _Set_Flares(const Vector2& _vPos);

public:
	static CLensFlare* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;

private:
	CTransform*				m_pTransformCom	= { nullptr };
	CShader*				m_pShaderCom	= { nullptr };
	CRenderer*				m_pRendererCom	= { nullptr };
	CCollider*				m_pColliderCom	= { nullptr }; // for debugging
	CVIBuffer_LensFlare*	m_pVIBufferCom	= { nullptr };
	CTexture*				m_pTextureCom	= { nullptr };

	const LIGHTDESC* m_pLight = { nullptr };

	function<void(CLensFlare*, _float)> m_funcTick;
	function<void(CLensFlare*, _float)> m_funcLateTick;
	function<HRESULT(CLensFlare*)> m_funcRender;

	_float m_fBeforeCdotL = { 0.f };
	_float m_fProgressTime = { 0.f };

	ID3D11Predicate*	m_pPredicate = { nullptr };
	ID3D11Query*		m_pOcclusionQuery = { nullptr };

	_float m_fSunVisibility = { 0.f };
	_bool m_bQuerySunVisibility = { true };

	vector<LENSFLARE_FLARE> m_tFlares;
	LENSFLARE_CORONA m_tCorona = {};
};

END