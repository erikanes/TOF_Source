#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CTransform;
class CRenderer;
class CShader;
class CTexture;
END

BEGIN(Client)
class CVIBuffer_Cloud;

class CCloud final : public CGameObject
{
private:
	CCloud(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCloud(const CCloud& _rhs);
	virtual ~CCloud() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Tick(_float _fTimeDelta) override;
	virtual void Late_Tick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT _Add_Components(void* _pArg);
	HRESULT _Bind_ShaderResources();

public:
	static CCloud* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;

private:
	CTransform* m_pTransformCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };

	CVIBuffer_Cloud* m_pVIBufferCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };

	_float4x4 m_matWorld = {};
	_float m_fAccumulatedTime = { 0.f };
};

END