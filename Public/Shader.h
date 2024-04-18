#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CShader final : public CComponent
{
private:
	enum class DownScaleLevel : _uchar
	{
		NONE, X2, X4
	};

private:
	CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShader(const CShader& rhs);
	virtual ~CShader() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElememts, _uint iNumElements, _bool _bIsComputeShader);
	virtual HRESULT Initialize_Prototype(const _tchar* _pShaderFilePath);
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Bind_ShaderResourceView(const char* pConstantName, ID3D11ShaderResourceView* pSRV);
	HRESULT Bind_ShaderResourceViews(const char* pConstantName, ID3D11ShaderResourceView** ppSRVs, _uint iNumTexture);
	HRESULT Bind_Matrix(const char* _pConstantName, const _float4x4* _pMatrix);
	HRESULT Bind_Matricies(const char* _pConstantName, const _float4x4* _pArrMatricies, _uint _uNumData);
	HRESULT Bind_RawValue(const char* _pConstantName, void* _pData, _uint _uByteSize);
	HRESULT Begin(_uint iPassIndex);
	HRESULT Begin(_uint _iPassIndex, _uint _iSizeX, _uint _iSizeY, _uint _iSizeZ);

	template<typename _Ty, typename... _Args>
	HRESULT Bind(const char* _szConstantName, _Ty _arg, _Args... _args)
	{
		if (nullptr == m_pEffect)
		{
			MSG_BOX("Nullptr exception: CShader::Bind(m_effect)");
			return E_FAIL;
		}

		using compare_type = If_t<is_pointer_v<_Ty>, remove_const_t<remove_pointer_t<_Ty>>*, remove_const_t<remove_reference_t<_Ty>>>;

		ComPtr<ID3DX11EffectVariable> pVariable = m_pEffect->GetVariableByName(_szConstantName);

		if (nullptr == pVariable)
		{
			MSG_BOX("Nullptr exception: CShader::Bind(variable)");
			return E_FAIL;
		}

		// 컴퓨트 셰이드용 Unordered Access View
		if constexpr (is_same_v<compare_type, ID3D11UnorderedAccessView*>)
		{
			ComPtr<ID3DX11EffectUnorderedAccessViewVariable> pUnorderedAccessViewVariable = pVariable->AsUnorderedAccessView();

			if (nullptr == pUnorderedAccessViewVariable)
			{
				MSG_BOX("Nullptr exception: CShader::Bind(pUnorderedAccessViewVariable)");
				return E_FAIL;
			}

			return pUnorderedAccessViewVariable->SetUnorderedAccessView(_arg);
		}
		

		// 단일 텍스쳐
		if constexpr (is_same_v<compare_type, ID3D11ShaderResourceView*>)
		{
			ComPtr<ID3DX11EffectShaderResourceVariable> pShaderResourceViewVariable = pVariable->AsShaderResource();

			if (nullptr == pShaderResourceViewVariable)
			{
				MSG_BOX("Nullptr exception: CShader::Bind(pShaderResourceViewVariable");
				return E_FAIL;
			}

			return pShaderResourceViewVariable->SetResource(_arg);
		}

		// 다중 텍스쳐
		if constexpr (is_same_v<compare_type, ID3D11ShaderResourceView**> || is_same_v<compare_type, const ID3D11ShaderResourceView**>)
		{
			ComPtr<ID3DX11EffectShaderResourceVariable> pShaderResourceViewVariable = pVariable->AsShaderResource();

			if (nullptr == pShaderResourceViewVariable)
			{
				MSG_BOX("Nullptr exception: CShader::Bind(pShaderResourceViewVariable");
				return E_FAIL;
			}

			return pShaderResourceViewVariable->SetResourceArray(_arg, 0, _args...);
		}

		// 행렬
		if constexpr (is_same_v<compare_type, _float4x4> || is_same_v<compare_type, SimpleMath::Matrix>)
		{
			ComPtr<ID3DX11EffectMatrixVariable> pMatrixVariable = pVariable->AsMatrix();

			if (nullptr == pMatrixVariable)
			{
				MSG_BOX("Nullptr exception: CShader::Bind(pMatrixVaiable");
				return E_FAIL;
			}

			if constexpr (is_pointer_v<_Ty>)
				return pMatrixVariable->SetMatrix(reinterpret_cast<const _float*>(_arg));
			else
				return pMatrixVariable->SetMatrix(reinterpret_cast<const _float*>(&_arg));
		}

		// 행렬 배열 (포인터) 주로 본 연산할때 사용
		if constexpr (is_same_v<compare_type, _float4x4*> || is_same_v<compare_type, SimpleMath::Matrix*>)
		{
			ComPtr<ID3DX11EffectMatrixVariable> pMatrixVariable = pVariable->AsMatrix();

			if (nullptr == pMatrixVariable)
			{
				MSG_BOX("Nullptr exception: CShader::Bind(pMatrixVariable");
				return E_FAIL;
			}

			if constexpr (is_pointer_v<_Ty>)
				return pMatrixVariable->SetMatrixArray(reinterpret_cast<const _float*>(_arg), 0, _args...);
		}

		// vector (float4)
		if constexpr (is_same_v<compare_type, _float4> || is_same_v<compare_type, SimpleMath::Vector4>)
		{
			ComPtr<ID3DX11EffectVectorVariable> pVectorVariable = pVariable->AsVector();

			if (nullptr == pVectorVariable)
			{
				MSG_BOX("Nullptr exception: CShader::Bind(pVectorVariable");
				return E_FAIL;
			}

			return pVectorVariable->SetFloatVector(reinterpret_cast<const _float*>(&_arg));
		}

		// vector (float3)
		if constexpr (is_same_v<compare_type, _float3> || is_same_v<compare_type, SimpleMath::Vector3>)
		{
			ComPtr<ID3DX11EffectVectorVariable> pVectorVariable = pVariable->AsVector();

			if (nullptr == pVectorVariable)
			{
				MSG_BOX("Nullptr exception: CShader::Bind(pVectorVariable");
				return E_FAIL;
			}

			return pVariable->SetRawValue(&_arg, 0, sizeof(_float3));
		}

		// vector (float2)
		if constexpr (is_same_v<compare_type, _float2> || is_same_v<compare_type, SimpleMath::Vector2>)
		{
			ComPtr<ID3DX11EffectVectorVariable> pVectorVariable = pVariable->AsVector();

			if (nullptr == pVectorVariable)
			{
				MSG_BOX("Nullptr exception: CShader::Bind(pVectorVariable");
				return E_FAIL;
			}

			return pVariable->SetRawValue(&_arg, 0, sizeof(_float2));
		}

		// vector array (float4)
		if constexpr (is_same_v<compare_type, _float4*>	|| is_same_v<compare_type, SimpleMath::Vector4*>)
		{
			ComPtr<ID3DX11EffectVectorVariable> pVectorVariable = pVariable->AsVector();

			if (nullptr == pVectorVariable)
			{
				MSG_BOX("Nullptr exception: CShader::Bind(pVectorVariable");
				return E_FAIL;
			}

			return pVectorVariable->SetFloatVectorArray(reinterpret_cast<const _float*>(_arg), 0, _args...);
		}

		// vector array (float3)
		if constexpr (is_same_v<compare_type, _float3*> || is_same_v<compare_type, SimpleMath::Vector3*>)
		{
			ComPtr<ID3DX11EffectVectorVariable> pVectorVariable = pVariable->AsVector();

			if (nullptr == pVectorVariable)
			{
				MSG_BOX("Nullptr exception: CShader::Bind(pVectorVariable");
				return E_FAIL;
			}

			return pVariable->SetRawValue(_arg, 0, sizeof(_float3) * _args...);
		}

		// vector array (float2)
		if constexpr (is_same_v<compare_type, _float2*> || is_same_v<compare_type, SimpleMath::Vector2*>)
		{
			ComPtr<ID3DX11EffectVectorVariable> pVectorVariable = pVariable->AsVector();

			if (nullptr == pVectorVariable)
			{
				MSG_BOX("Nullptr exception: CShader::Bind(pVectorVariable");
				return E_FAIL;
			}

			return pVariable->SetRawValue(_arg, 0, sizeof(_float2) * _args...);
		}

		// float type
		if constexpr (is_same_v<compare_type, _float>)
		{
			ComPtr<ID3DX11EffectScalarVariable> pScalarVariable = pVariable->AsScalar();

			if (nullptr == pScalarVariable)
			{
				MSG_BOX("Nullptr exception: CShader::Bind(pScalarVariable)");
				return E_FAIL;
			}

			return pScalarVariable->SetFloat(_arg);
		}

		// float array type
		if constexpr (is_same_v<compare_type, _float*>)
		{
			ComPtr<ID3DX11EffectScalarVariable> pScalarVariable = pVariable->AsScalar();

			if (nullptr == pScalarVariable)
			{
				MSG_BOX("Nullptr exception: CShader::Bind(pScalarVariable)");
				return E_FAIL;
			}

			return pScalarVariable->SetFloatArray(_arg, 0, _args...);
		}

		// bool type
		if constexpr (is_same_v<compare_type, _bool>)
		{
			ComPtr<ID3DX11EffectScalarVariable> pScalarVariable = pVariable->AsScalar();

			if (nullptr == pScalarVariable)
			{
				MSG_BOX("Nullptr exception: CShader::Bind(pScalarVariable)");
				return E_FAIL;
			}

			return pScalarVariable->SetBool(_arg);
		}

		// int type
		if constexpr (is_same_v<compare_type, _int> || is_same_v<compare_type, _uint>)
		{
			ComPtr<ID3DX11EffectScalarVariable> pScalarVariable = pVariable->AsScalar();

			if (nullptr == pScalarVariable)
			{
				MSG_BOX("Nullptr exception: CShader::Bind(pScalarVariable)");
				return E_FAIL;
			}

			return pScalarVariable->SetInt(_arg);
		}

		// int array type
		if constexpr (is_same_v<compare_type, _int*> || is_same_v<compare_type, _uint*>)
		{
			ComPtr<ID3DX11EffectScalarVariable> pScalarVariable = pVariable->AsScalar();

			if (nullptr == pScalarVariable)
			{
				MSG_BOX("Nullptr exception: CShader::Bind(pScalarVariable)");
				return E_FAIL;
			}

			return pScalarVariable->SetIntArray(_arg, 0, _args...);
		}

		MSG_BOX("Type exception: CShader::Bind");

		return E_FAIL;
	}

	void Clear_PS();
	void Clear_CS();

private:
	ID3DX11Effect* m_pEffect = { nullptr };
	_uint						m_iNumPasses = { 0 };

	vector<ID3D11InputLayout*>	m_InputLayouts;

	_bool						m_bIsComputeShader = { false };

public:
	static CShader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElememts, _uint iNumElements, _bool _bIsComputeShader = false);
	static CShader* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _pShaderFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END