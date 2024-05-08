#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTexture;
class CTransform;
class CVIBuffer_Point;
END

BEGIN(Client)

class CFakeShadow final : public CGameObject
{
private:
	CFakeShadow(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CFakeShadow(const CFakeShadow& _rhs);
	virtual ~CFakeShadow() = default;

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
	static CFakeShadow* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CVIBuffer_Point* m_pVIBufferCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };

	CGameObject* m_pOwner = { nullptr };
	CTransform* m_pOwnerTransformCom = { nullptr };

	_float2 m_vShadowSize = { 1.f, 1.f };

	function<_bool()> m_funcCondition;
};

END