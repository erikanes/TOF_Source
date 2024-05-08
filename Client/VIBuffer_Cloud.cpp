#include "Client_pch.h"
#include "Client_Defines.h"
#include "VIBuffer_Cloud.h"

CVIBuffer_Cloud::CVIBuffer_Cloud(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CVIBuffer(_pDevice, _pContext)
{
}

CVIBuffer_Cloud::CVIBuffer_Cloud(const CVIBuffer_Cloud& _rhs)
	: CVIBuffer(_rhs)
{
}

HRESULT CVIBuffer_Cloud::Initialize_Prototype(_uint _iResolution, _float _fWidth)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_iNumVertexBuffers = 1;
	m_iNumVertices = (_iResolution + 1) * (_iResolution + 1) * 6;
	m_iVertexStride = sizeof(VTXPOSTEX);
	m_iNumIndices = m_iNumVertices;
	m_iIndexStride = sizeof(_ushort);
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region Initialize
	vector<VTXPOSTEX> datas(m_iNumVertices);
	_float fQuadSize, fRadius, fConstant, fTextureDelta, fTop, fBottom;
	_uint idx = 0;

	fTop = 0.5f;
	fBottom = 0.f;

	fQuadSize = _fWidth / static_cast<_float>(_iResolution);
	fRadius = _fWidth / 2.f;
	fConstant = (fTop - fBottom) / (fRadius * fRadius);
	fTextureDelta = 2.f / static_cast<_float>(_iResolution);

	for (_uint j = 0; j <= _iResolution; ++j)
	{
		for (_uint i = 0; i <= _iResolution; ++i)
		{
			_float x = (-0.5f * _fWidth) + (static_cast<_float>(i) * fQuadSize);
			_float z = (-0.5f * _fWidth) + (static_cast<_float>(j) * fQuadSize);
			_float y = fTop - (fConstant * ((x * x) + (z * z)));

			idx = j * (_iResolution + 1) + i;

			datas[idx].vPosition = _float3(x, y, z);
			datas[idx].vTexcoord = _float2(static_cast<_float>(i) * fTextureDelta, static_cast<_float>(j) * fTextureDelta);
		}
	}
#pragma endregion

#pragma region Vertex Buffer
	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));

	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	_uint idx1, idx2, idx3, idx4;
	idx = idx1 = idx2 = idx3 = idx4 = 0;

	vector<VTXPOSTEX> vertices(m_iNumVertices);
	vector<_ushort> indices(m_iNumIndices, 0);

	for (_uint j = 0; j < _iResolution; ++j)
	{
		for (_uint i = 0; i < _iResolution; ++i)
		{
			idx1 = j * (_iResolution + 1) + i;
			idx2 = j * (_iResolution + 1) + (i + 1);
			idx3 = (j + 1) * (_iResolution + 1) + i;
			idx4 = (j + 1) * (_iResolution + 1) + (i + 1);

			vertices[idx].vPosition = datas[idx1].vPosition;
			vertices[idx].vTexcoord = datas[idx1].vTexcoord;
			indices[idx] = idx;
			++idx;

			vertices[idx].vPosition = datas[idx2].vPosition;
			vertices[idx].vTexcoord = datas[idx2].vTexcoord;
			indices[idx] = idx;
			++idx;

			vertices[idx].vPosition = datas[idx3].vPosition;
			vertices[idx].vTexcoord = datas[idx3].vTexcoord;
			indices[idx] = idx;
			++idx;

			vertices[idx].vPosition = datas[idx3].vPosition;
			vertices[idx].vTexcoord = datas[idx3].vTexcoord;
			indices[idx] = idx;
			++idx;

			vertices[idx].vPosition = datas[idx2].vPosition;
			vertices[idx].vTexcoord = datas[idx2].vTexcoord;
			indices[idx] = idx;
			++idx;

			vertices[idx].vPosition = datas[idx4].vPosition;
			vertices[idx].vTexcoord = datas[idx4].vTexcoord;
			indices[idx] = idx;
			++idx;
		}
	}

	ZeroMemory(&m_InitializeData, sizeof(D3D11_SUBRESOURCE_DATA));

	m_InitializeData.pSysMem = &vertices.front();

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;
#pragma endregion

#pragma region Index Buffer
	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));

	m_BufferDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0.;
	m_BufferDesc.StructureByteStride = 0;

	ZeroMemory(&m_InitializeData, sizeof(D3D11_SUBRESOURCE_DATA));

	m_InitializeData.pSysMem = &indices.front();

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;
#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Cloud::Initialize(void* _pArg)
{
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	return S_OK;
}

CVIBuffer_Cloud* CVIBuffer_Cloud::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _uint _iResolution, _float _fWidth)
{
	auto pInstance = new CVIBuffer_Cloud(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype(_iResolution, _fWidth)))
	{
		MSG_BOX("Failed to Created: CVIBuffer_Cloud::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Cloud::Clone(void* _pArg)
{
	auto pInstance = new CVIBuffer_Cloud(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned: CVIBuffer_Cloud::Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Cloud::Free()
{
	__super::Free();
}
