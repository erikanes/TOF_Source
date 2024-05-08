#pragma once

#include "UI_Base.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CVIBuffer_Point;
class CTexture;
END

BEGIN(Client)

class CUI_Minimap final : public CUI_Base
{
private:
	CUI_Minimap(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CUI_Minimap(const CUI_Minimap& _rhs);
	virtual ~CUI_Minimap() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Tick(_float _fTimeDelta) override;
	virtual void Late_Tick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void Enable_UI(any _msgDesc) { m_bEnable = true; }
	virtual void Disable_UI(any _msgDesc) { m_bEnable = false; }

private:
	HRESULT _Add_Components(void* _pArg);
	HRESULT _Bind_ShaderResources();

public:
	static CUI_Minimap* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;

private:
	CRenderer* m_pRendererCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Point* m_pVIBufferCom = { nullptr };

	CTransform* m_pPlayerTransformCom = { nullptr };

	_uint m_iTextureIndex = { 0 };

	_float2 m_vPosition = { 0.f, 0.f };
	_float4 m_vRange = { 0.f, 0.f, 0.f, 0.f };
	_float m_fUVSize = { 0.f };
};

END