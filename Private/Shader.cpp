#include "Engine_pch.h"
#include "Shader.h"

CShader::CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CShader::CShader(const CShader& rhs)
	: CComponent(rhs)
	, m_pEffect(rhs.m_pEffect)
	, m_iNumPasses(rhs.m_iNumPasses)
	, m_InputLayouts(rhs.m_InputLayouts)
	, m_bIsComputeShader(rhs.m_bIsComputeShader)
{
	for (auto& pInputLayout : m_InputLayouts)
		Safe_AddRef(pInputLayout);

	Safe_AddRef(m_pEffect);
}

HRESULT CShader::Initialize_Prototype(const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElememts, _uint iNumElements, _bool _bIsComputeShader)
{
	_uint			iHlslFlag = { 0 };

#ifdef _DEBUG
	iHlslFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	iHlslFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif
	LPD3DBLOB p = nullptr;

	if (FAILED(D3DX11CompileEffectFromFile(pShaderFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, iHlslFlag, 0, m_pDevice, &m_pEffect, &p)))
	{
		OutputDebugStringA(reinterpret_cast<const char*>(p->GetBufferPointer()));
		p->Release();
		return E_FAIL;
	}

	m_bIsComputeShader = _bIsComputeShader;

	ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(0);
	if (nullptr == pTechnique)
		return E_FAIL;

	D3DX11_TECHNIQUE_DESC		TechniqueDesc;
	ZeroMemory(&TechniqueDesc, sizeof TechniqueDesc);

	if (FAILED(pTechnique->GetDesc(&TechniqueDesc)))
		return E_FAIL;

	m_iNumPasses = TechniqueDesc.Passes;

	if (_bIsComputeShader)
		return S_OK;

	m_InputLayouts.reserve(TechniqueDesc.Passes);

	for (_uint i = 0; i < TechniqueDesc.Passes; i++)
	{
		ID3DX11EffectPass* pPass = pTechnique->GetPassByIndex(i);


		D3DX11_PASS_DESC		PassDesc;
		ZeroMemory(&PassDesc, sizeof PassDesc);

		if (FAILED(pPass->GetDesc(&PassDesc)))
			return E_FAIL;

		ID3D11InputLayout* pInputLayout = { nullptr };

		if (FAILED(m_pDevice->CreateInputLayout(pElememts, iNumElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &pInputLayout)))
			return E_FAIL;

		m_InputLayouts.push_back(pInputLayout);
	}

	return S_OK;
}

HRESULT CShader::Initialize_Prototype(const _tchar* _pShaderFilePath)
{
	_uint			iHlslFlag = { 0 };

#ifdef _DEBUG
	iHlslFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	iHlslFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif
	LPD3DBLOB p = nullptr;

	if (FAILED(D3DX11CompileEffectFromFile(_pShaderFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, iHlslFlag, 0, m_pDevice, &m_pEffect, &p)))
	{
		OutputDebugStringA(reinterpret_cast<const char*>(p->GetBufferPointer()));
		p->Release();
		return E_FAIL;
	}

	m_bIsComputeShader = true;

	ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(0);
	if (nullptr == pTechnique)
		return E_FAIL;

	D3DX11_TECHNIQUE_DESC		TechniqueDesc;
	ZeroMemory(&TechniqueDesc, sizeof TechniqueDesc);

	if (FAILED(pTechnique->GetDesc(&TechniqueDesc)))
		return E_FAIL;

	m_iNumPasses = TechniqueDesc.Passes;

	return S_OK;
}

HRESULT CShader::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CShader::Bind_ShaderResourceView(const char* pConstantName, ID3D11ShaderResourceView* pSRV)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectShaderResourceVariable* pSRVVariable = pVariable->AsShaderResource();
	if (nullptr == pSRVVariable)
		return E_FAIL;

	return pSRVVariable->SetResource(pSRV);
}

HRESULT CShader::Bind_ShaderResourceViews(const char* pConstantName, ID3D11ShaderResourceView** ppSRVs, _uint iNumTexture)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectShaderResourceVariable* pSRVVariable = pVariable->AsShaderResource();
	if (nullptr == pSRVVariable)
		return E_FAIL;

	return pSRVVariable->SetResourceArray(ppSRVs, 0, iNumTexture);
}

HRESULT CShader::Bind_Matrix(const char* pConstantName, const _float4x4* pMatrix)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectMatrixVariable* pMatrixVariable = pVariable->AsMatrix();
	if (nullptr == pMatrixVariable)
		return E_FAIL;

	return pMatrixVariable->SetMatrix((_float*)pMatrix);
}

HRESULT CShader::Bind_Matricies(const char* _pConstantName, const _float4x4* _pArrMatricies, _uint _uNumData)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	ID3DX11EffectVariable* pEffectVariable = m_pEffect->GetVariableByName(_pConstantName);
	if (nullptr == pEffectVariable)
		return E_FAIL;

	ID3DX11EffectMatrixVariable* pMatrixVariable = pEffectVariable->AsMatrix();
	if (nullptr == pMatrixVariable)
		return E_FAIL;

	return pMatrixVariable->SetMatrixArray((_float*)_pArrMatricies, 0, _uNumData);
}

HRESULT CShader::Bind_RawValue(const char* _pConstantName, void* _pData, _uint _uByteSize)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(_pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	return pVariable->SetRawValue(_pData, 0, _uByteSize);
}

HRESULT CShader::Begin(_uint iPassIndex)
{
	if (iPassIndex >= m_iNumPasses)
		return E_FAIL;

	ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(0);
	if (nullptr == pTechnique)
		return E_FAIL;

	if (FAILED(pTechnique->GetPassByIndex(iPassIndex)->Apply(0, m_pContext)))
		return E_FAIL;

	m_pContext->IASetInputLayout(m_InputLayouts[iPassIndex]);

	return S_OK;
}

// for compute shader
HRESULT CShader::Begin(_uint _iPassIndex, _uint _iSizeX, _uint _iSizeY, _uint _iSizeZ)
{
	if (_iPassIndex >= m_iNumPasses)
		return E_FAIL;

	ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(0);
	if (nullptr == pTechnique)
		return E_FAIL;

	// Apply 함수 호출 전에 모든 자원들을 resource에 bind 시켜줘야 함
	if (FAILED(pTechnique->GetPassByIndex(_iPassIndex)->Apply(0, m_pContext)))
		return E_FAIL;

	m_pContext->Dispatch(_iSizeX, _iSizeY, _iSizeZ);

	Clear_CS();

	return S_OK;
}

void CShader::Clear_PS()
{
	static ID3D11ShaderResourceView* const pSRVClear[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = { nullptr };
	m_pContext->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, pSRVClear);
}

void CShader::Clear_CS()
{
	static ID3D11ShaderResourceView* const pSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = { nullptr };
	m_pContext->CSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, pSRV);

	static ID3D11UnorderedAccessView* const pUAV[8] = { nullptr };
	m_pContext->CSSetUnorderedAccessViews(0, 8, pUAV, nullptr);

	m_pContext->CSSetShader(nullptr, nullptr, 0);
}

CShader* CShader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElememts, _uint iNumElements, _bool _bIsComputeShader)
{
	CShader* pInstance = new CShader(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pShaderFilePath, pElememts, iNumElements, _bIsComputeShader)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CShader");
	}

	return pInstance;
}

CShader* CShader::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _pShaderFilePath)
{
	CShader* pInstance = new CShader(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype(_pShaderFilePath)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CShader");
	}

	return pInstance;
}

CComponent* CShader::Clone(void* pArg)
{
	CShader* pInstance = new CShader(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CShader");
	}

	return pInstance;
}

void CShader::Free()
{
	__super::Free();

	for (auto& pInputLayout : m_InputLayouts)
		Safe_Release(pInputLayout);

	m_InputLayouts.clear();

	Safe_Release(m_pEffect);



}
