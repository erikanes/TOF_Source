#include "Client_pch.h"
#include "Client_Defines.h"
#include "VIBuffer_LensFlare.h"

CVIBuffer_LensFlare::CVIBuffer_LensFlare(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CVIBuffer(_pDevice, _pContext)
{
}

CVIBuffer_LensFlare::CVIBuffer_LensFlare(const CVIBuffer_LensFlare& _rhs)
	: CVIBuffer(_rhs)
{
}

HRESULT CVIBuffer_LensFlare::Initialize(void* _pArg)
{
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_LensFlare::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_iNumVertices = 1;
	m_iVertexStride = sizeof(VTXPOS);
	m_iNumVertexBuffers = 1;

	m_iNumIndices = 1;
	m_iIndexStride = sizeof(_ushort);

	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;

#pragma region Vertex Buffer
	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	VTXPOS vertices[1] = {_float3(0.f, 0.f, 0.f)};

	ZeroMemory(&m_InitializeData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_InitializeData.pSysMem = &vertices;

	if (FAILED(Create_Buffer(&m_pVB)))
		return E_FAIL;
#pragma endregion

#pragma region Index Buffer
	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));

	m_BufferDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	_ushort indices[1] = { 0 };

	ZeroMemory(&m_InitializeData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_InitializeData.pSysMem = &indices;

	if (FAILED(Create_Buffer(&m_pIB)))
		return E_FAIL;
#pragma endregion

	return S_OK;
}

CVIBuffer_LensFlare* CVIBuffer_LensFlare::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	auto pInstance = new CVIBuffer_LensFlare(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created: CVIBuffer_LensFlare::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_LensFlare::Clone(void* _pArg)
{
	auto pInstance = new CVIBuffer_LensFlare(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned: CVIBuffer_LensFlare::Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_LensFlare::Free()
{
	__super::Free();
}