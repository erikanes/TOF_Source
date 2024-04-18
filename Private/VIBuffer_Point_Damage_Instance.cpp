#include "Client_pch.h"
#include "VIBuffer_Point_Damage_Instance.h"

CVIBuffer_Point_Damage_Instance::CVIBuffer_Point_Damage_Instance(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CVIBuffer_Instance(_pDevice, _pContext)
{
}

CVIBuffer_Point_Damage_Instance::CVIBuffer_Point_Damage_Instance(const CVIBuffer_Point_Damage_Instance& _rhs)
	: CVIBuffer_Instance(_rhs)
{
}

HRESULT CVIBuffer_Point_Damage_Instance::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_iNumInstance = 100;
	m_iInstanceStride = sizeof(VTXDAMAGEINSTANCE);
	m_iNumIndicesPerInstance = 1;

	m_iNumVertices = 1;
	m_iVertexStride = sizeof(VTXPOSPSIZE);
	m_iNumVertexBuffers = 2;

	m_iNumIndices = m_iNumInstance;
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

	vector<VTXPOSPSIZE> vertices(m_iNumVertices);
	ZeroMemory(&vertices.front(), sizeof(VTXPOSPSIZE) * m_iNumVertices);

	vertices[0].vPosition = _float3(0.f, 0.f, 0.f);
	vertices[0].vPSize = _float2(0.f, 0.f);

	ZeroMemory(&m_InitializeData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_InitializeData.pSysMem = &vertices.front();

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

	vector<_ushort> indices(m_iNumIndices, 0);

	ZeroMemory(&m_InitializeData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_InitializeData.pSysMem = &indices.front();

	if (FAILED(Create_Buffer(&m_pIB)))
		return E_FAIL;
#pragma endregion

#pragma region Instance Buffer
	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));

	m_BufferDesc.ByteWidth = m_iNumInstance * m_iInstanceStride;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	vector<VTXDAMAGEINSTANCE> instanceVertices(m_iNumInstance);
	ZeroMemory(&instanceVertices.front(), sizeof(VTXDAMAGEINSTANCE) * m_iNumInstance);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		auto& iv = instanceVertices[i];

		iv.vPosition = _float3(0.f, 0.f, 0.f);
		iv.vAddPosition = _float2(0.f, 0.f);
		iv.vSizeAlpha = _float2(1.f, 1.f);
		iv.vUV = _float4(0.f, 0.f, 1.f, 1.f);
		iv.vColor = _float4(1.f, 1.f, 1.f, 1.f);
		iv.iType = 0;
	}

	ZeroMemory(&m_InitializeData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_InitializeData.pSysMem = &instanceVertices.front();

	if (FAILED(Create_Buffer(&m_pInstanceVB)))
		return E_FAIL;
#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Point_Damage_Instance::Initialize(void* _pArg)
{
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	return S_OK;
}

void CVIBuffer_Point_Damage_Instance::Late_Tick(_float _fTimeDelta, const vector<VTXDAMAGEINSTANCE>& _vecDesc)
{
	m_iNumInstance = static_cast<_uint>(_vecDesc.size());

	if (0 >= m_iNumInstance)
		return;

	D3D11_MAPPED_SUBRESOURCE subResource;
	ZeroMemory(&subResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pContext->Map(m_pInstanceVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);

	auto pData = static_cast<VTXDAMAGEINSTANCE*>(subResource.pData);

	memcpy(pData, &_vecDesc.front(), sizeof(VTXDAMAGEINSTANCE) * m_iNumInstance);

	m_pContext->Unmap(m_pInstanceVB, 0);
}

CVIBuffer_Point_Damage_Instance* CVIBuffer_Point_Damage_Instance::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	auto pInstance = new CVIBuffer_Point_Damage_Instance(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create: CVIBuffer_Point_Damage_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Point_Damage_Instance::Clone(void* _pArg)
{
	auto pInstance = new CVIBuffer_Point_Damage_Instance(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned: CVIBuffer_Point_Damage_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Point_Damage_Instance::Free()
{
	__super::Free();
}
