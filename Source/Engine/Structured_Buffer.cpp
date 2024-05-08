#include "Engine_pch.h"
#include "Structured_Buffer.h"

// Component
#include "Shader.h"

CStructured_Buffer::CStructured_Buffer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: m_pDevice(_pDevice)
	, m_pContext(_pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CStructured_Buffer::Initialize(_uint _iWidth)
{
	if (FAILED(_Create_Buffer(_iWidth)))
		return E_FAIL;

	if (FAILED(_Create_View(_iWidth)))
		return E_FAIL;

	return S_OK;
}

HRESULT CStructured_Buffer::Bind_SRV(CShader* _pShader, const char* _pConstantName)
{
	if (nullptr == _pShader || nullptr == m_pSRV)
		return E_FAIL;

	return _pShader->Bind(_pConstantName, m_pSRV.Get());
}

HRESULT CStructured_Buffer::Bind_UAV(CShader* _pShader, const char* _pConstantName)
{
	if (nullptr == _pShader || nullptr == m_pUAV)
		return E_FAIL;

	return _pShader->Bind(_pConstantName, m_pUAV.Get());
}

HRESULT CStructured_Buffer::Swap(CStructured_Buffer* _pOther)
{
	if (nullptr == _pOther)
		return E_FAIL;

	m_pBuffer.Swap(_pOther->m_pBuffer);
	m_pSRV.Swap(_pOther->m_pSRV);
	m_pUAV.Swap(_pOther->m_pUAV);

	return S_OK;
}

ID3D11ShaderResourceView* CStructured_Buffer::Get_SRV()
{
	return m_pSRV.Get();
}

ID3D11UnorderedAccessView* CStructured_Buffer::Get_UAV()
{
	return m_pUAV.Get();
}

ID3D11Buffer* CStructured_Buffer::Get_Buffer()
{
	return m_pBuffer.Get();
}

HRESULT CStructured_Buffer::_Create_Buffer(_uint _iWidth)
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	desc.StructureByteStride = sizeof(_float);
	desc.ByteWidth = _iWidth * desc.StructureByteStride;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	if (FAILED(m_pDevice->CreateBuffer(&desc, nullptr, m_pBuffer.GetAddressOf())))
		return E_FAIL;

	return S_OK;
}

HRESULT CStructured_Buffer::_Create_View(_uint _iWidth)
{
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.NumElements = _iWidth;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pBuffer.Get(), &srvDesc, m_pSRV.GetAddressOf())))
		return E_FAIL;

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));

	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.NumElements = _iWidth;

	if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pBuffer.Get(), &uavDesc, m_pUAV.GetAddressOf())))
		return E_FAIL;

	return S_OK;
}

CStructured_Buffer* CStructured_Buffer::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _uint _iWidth)
{
	auto pInstance = new CStructured_Buffer(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_iWidth)))
	{
		MSG_BOX("Failed to Create: CStructured_Buffer::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStructured_Buffer::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}