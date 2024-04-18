#include "Engine_pch.h"
#include "VIBuffer_Instance_Combined.h"

CVIBuffer_Instance_Combined::CVIBuffer_Instance_Combined(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CVIBuffer_Instance(_pDevice, _pContext)
{
}

CVIBuffer_Instance_Combined::CVIBuffer_Instance_Combined(const CVIBuffer_Instance_Combined& _rhs)
	: CVIBuffer_Instance(_rhs)
{
}

HRESULT CVIBuffer_Instance_Combined::Initialize(void* _pArg)
{
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Instance_Combined::Initialize_Prototype(ID3D11Buffer* _pVB, ID3D11Buffer* _pIB, const VIBUFFERINFO& _tInfo, const vector<MESH_INSTANCE_DATA>& _vecData)
{
	m_pVB = _pVB;
	Safe_AddRef(m_pVB);

	m_pIB = _pIB;
	Safe_AddRef(m_pIB);

	m_iNumVertexBuffers = 2;
	m_iNumVertices = _tInfo.iNumVertices;
	m_iVertexStride = _tInfo.iVertexStride;
	m_iNumIndices = m_iNumIndicesPerInstance = _tInfo.iNumIndices;
	m_iIndexStride = _tInfo.iIndexStride;
	m_eTopology = _tInfo.eTopology;
	m_eIndexFormat = _tInfo.eIndexFormat;

	m_iNumInstance = static_cast<_uint>(_vecData.size());


	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	// Non-Anim
	if (true)
	{
		m_iInstanceStride = sizeof(VTXMESHTOONINSTANCE);

		vector<VTXMESHTOONINSTANCE> instances(m_iNumInstance);

		for (size_t i = 0; i < m_iNumInstance; ++i)
			memcpy(&instances.front(), &_vecData.front(), sizeof(_float4x4) * 2);

		m_BufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstance;

		ZeroMemory(&m_InitializeData, sizeof(D3D11_SUBRESOURCE_DATA));
		m_InitializeData.pSysMem = &instances.front();

		if (FAILED(__super::Create_Buffer(&m_pInstanceVB)))
		{
			Safe_Release(m_pInstanceVB);
			return E_FAIL;
		}
	}

	// Anim
	else
	{
		m_iInstanceStride = sizeof(VTXMESHTOONANIMINSTANCE);

		vector<VTXMESHTOONINSTANCE> instances(m_iNumInstance);

		for (size_t i = 0; i < m_iNumInstance; ++i)
			memcpy(&instances.front(), &_vecData.front(), sizeof(_float4x4) * 2);

		m_BufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstance;

		ZeroMemory(&m_InitializeData, sizeof(D3D11_SUBRESOURCE_DATA));
		m_InitializeData.pSysMem = &instances.front();

		if (FAILED(__super::Create_Buffer(&m_pInstanceVB)))
		{
			Safe_Release(m_pInstanceVB);
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CVIBuffer_Instance_Combined::Initialize_Prototype(ID3D11Buffer* _pVB, ID3D11Buffer* _pIB, ID3D11Buffer* _pInstanceVB, const VIBUFFERINFO& _tInfo, _uint _iNumInstance, _uint _iInstanceStride)
{
	m_pVB = _pVB;
	Safe_AddRef(m_pVB);

	m_pIB = _pIB;
	Safe_AddRef(m_pIB);

	m_pInstanceVB = _pInstanceVB;
	Safe_AddRef(m_pInstanceVB);

	m_iNumVertexBuffers = 2;
	m_iNumVertices = _tInfo.iNumVertices;
	m_iVertexStride = _tInfo.iVertexStride;
	m_iNumIndices = m_iNumIndicesPerInstance = _tInfo.iNumIndices;
	m_iIndexStride = _tInfo.iIndexStride;
	m_eTopology = _tInfo.eTopology;
	m_eIndexFormat = _tInfo.eIndexFormat;

	m_iNumInstance = _iNumInstance;
	m_iInstanceStride = _iInstanceStride;

	return S_OK;
}

CVIBuffer_Instance_Combined* CVIBuffer_Instance_Combined::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext,
	ID3D11Buffer* _pVB, ID3D11Buffer* _pIB, const VIBUFFERINFO& _tInfo, const vector<MESH_INSTANCE_DATA>& _vecData)
{
	auto pInstance = new CVIBuffer_Instance_Combined(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype(_pVB, _pIB, _tInfo, _vecData)))
	{
		MSG_BOX("Failed to Create: CVIBuffer_Instance_Combined::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CVIBuffer_Instance_Combined* CVIBuffer_Instance_Combined::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, ID3D11Buffer* _pVB, ID3D11Buffer* _pIB, ID3D11Buffer* _pInstanceVB,
	const VIBUFFERINFO& _tInfo, _uint _iNumInstance, _uint _iInstanceStride)
{
	auto pInstance = new CVIBuffer_Instance_Combined(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype(_pVB, _pIB, _pInstanceVB, _tInfo, _iNumInstance, _iInstanceStride)))
	{
		MSG_BOX("Failed to Create: CVIBuffer_Instance_Combined::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Instance_Combined::Clone(void* _pArg)
{
	return this;
}

void CVIBuffer_Instance_Combined::Free()
{
	__super::Free();
}
