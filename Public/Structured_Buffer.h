#pragma once

#include "Base.h"

BEGIN(Engine)

class CShader;

class ENGINE_DLL CStructured_Buffer final : public CBase
{
private:
	CStructured_Buffer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CStructured_Buffer(const CStructured_Buffer& _rhs) = default;
	virtual ~CStructured_Buffer() = default;

public:
	HRESULT Initialize(_uint _iWidth);

	HRESULT Bind_SRV(CShader* _pShader, const char* _pConstantName);
	HRESULT Bind_UAV(CShader* _pShader, const char* _pConstantName);

	HRESULT Swap(CStructured_Buffer* _pOther);

	ID3D11ShaderResourceView* Get_SRV();
	ID3D11UnorderedAccessView* Get_UAV();
	ID3D11Buffer* Get_Buffer();

private:
	HRESULT _Create_Buffer(_uint _iViewNumElements);
	HRESULT _Create_View(_uint _iBufferByteWidth);

public:
	static CStructured_Buffer* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _uint _iWidth);
	virtual void Free() override;

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	ComPtr<ID3D11ShaderResourceView> m_pSRV = { nullptr };
	ComPtr<ID3D11UnorderedAccessView> m_pUAV = { nullptr };
	ComPtr<ID3D11Buffer> m_pBuffer = { nullptr };
};

END