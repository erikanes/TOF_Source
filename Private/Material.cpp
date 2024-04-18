#include "Engine_pch.h"
#include "Material.h"

// Component
#include "Texture.h"
#include "Shader.h"

CMaterial::CMaterial(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CComponent(_pDevice, _pContext)
{
	TEXTUREMTRLINFO tInfo;
	ZeroMemory(&tInfo, sizeof(TEXTUREMTRLINFO));
	m_vTextures = vector<TEXTUREMTRLINFO>(AI_TEXTURE_TYPE_MAX, tInfo);

	m_vecTextures.resize(AI_TEXTURE_TYPE_MAX);
}

CMaterial::CMaterial(const CMaterial& _rhs)
	: CComponent(_rhs)
	, m_vecTextures(_rhs.m_vecTextures)
{
	TEXTUREMTRLINFO tInfo;
	ZeroMemory(&tInfo, sizeof(TEXTUREMTRLINFO));
	m_vTextures = vector<TEXTUREMTRLINFO>(AI_TEXTURE_TYPE_MAX, tInfo);

	for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
	{
		m_vTextures[i] = _rhs.m_vTextures[i];
		Safe_AddRef(m_vTextures[i].pComponent);
	}

	for (auto& texture : m_vecTextures)
		Safe_AddRef(texture.pTexture);
}

HRESULT CMaterial::Initialize(void* _pArg)
{
	m_vecTextures[aiTextureType_DIFFUSE].vBaseColor = SimpleMath::Vector4::One;

	return S_OK;
}

HRESULT CMaterial::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMaterial::Add_Texture(aiTextureType _eTexType, const char* _pTexName, const char* _pTexFullPath, CComponent* _pTextureCom)
{
	if (nullptr == _pTextureCom)
	{
		MSG_BOX("Failed add to texture: CMaterial::Add_Texture");
		return E_FAIL;
	}

	_uint iTextureIdx = static_cast<_uint>(_eTexType);

	m_vecTextures[iTextureIdx].eTextureType = _eTexType;
	m_vecTextures[iTextureIdx].strName = _pTexName;
	m_vecTextures[iTextureIdx].strPath = _pTexFullPath;
	m_vecTextures[iTextureIdx].pTexture = static_cast<CTexture*>(_pTextureCom);

	return S_OK;
}

HRESULT CMaterial::Add_Textures(MATERIAL_FILE_DATA* _pArrMtrlTextures)
{
	if (nullptr == _pArrMtrlTextures)
		return E_FAIL;

	for (_uint i = 0; i < static_cast<_uint>(AI_TEXTURE_TYPE_MAX); ++i)
	{
		if (!strcmp("", _pArrMtrlTextures[i].szTexName) || !strcmp("", _pArrMtrlTextures[i].szTexFullPath))
			continue;
		else
		{
			m_vecTextures[i].strName = _pArrMtrlTextures[i].szTexName;
			m_vecTextures[i].strPath = _pArrMtrlTextures[i].szTexFullPath;

			wstring wstrPath(m_vecTextures[i].strPath.begin(), m_vecTextures[i].strPath.end());

			auto pTexture = CTexture::Create(m_pDevice, m_pContext, wstrPath.c_str());

			m_vecTextures[i].pTexture = pTexture;
			m_vecTextures[i].eTextureType = static_cast<aiTextureType>(i);
		}
	}

	return S_OK;
}

HRESULT CMaterial::Bind_ShaderResources(CShader* _pShaderCom)
{
	if (nullptr == _pShaderCom)
		return E_FAIL;

	_pShaderCom->Bind("g_bUseDiffuseTexture", false);
	_pShaderCom->Bind("g_bUseNormalTexture", false);
	_pShaderCom->Bind("g_bUseEmissiveTexture", false);
	_pShaderCom->Bind("g_bUseNoiseTexture", false);
	_pShaderCom->Bind("g_bUseMaskMapTexture", false);
	_pShaderCom->Bind("g_bUseSecondDiffuseTexture", false);
	_pShaderCom->Bind("g_bUseSecondNormalTexture", false);
	_pShaderCom->Bind("g_bUseLightMapTexture", false);

	for (auto& textureAttribute : m_vecTextures)
	{
		if (nullptr == textureAttribute.pTexture)
			continue;

		auto pShaderResourceView = textureAttribute.pTexture->Get_SRV(0);

		// diffuse, normal, emissive, lightmap

		// _d : Diffuse
		if (aiTextureType_DIFFUSE == textureAttribute.eTextureType)
		{
			_pShaderCom->Bind("g_DiffuseTexture", pShaderResourceView);
			_pShaderCom->Bind("g_bUseDiffuseTexture", true);
		}

		// _n : Normals
		// But, 얼굴의 라이트맵은 노말에 배정됨
		else if (aiTextureType_NORMALS == textureAttribute.eTextureType)
		{
			_pShaderCom->Bind("g_NormalTexture", pShaderResourceView);
			_pShaderCom->Bind("g_bUseNormalTexture", true);
		}

		// Noise : Opacity
		else if (aiTextureType_OPACITY == textureAttribute.eTextureType)
		{
			_pShaderCom->Bind("g_NoiseTexture", pShaderResourceView);
			_pShaderCom->Bind("g_bUseNoiseTexture", true);
		}

		// : Emissive
		else if (aiTextureType_EMISSIVE == textureAttribute.eTextureType)
		{
			_pShaderCom->Bind("g_EmissiveTexture", pShaderResourceView);
			_pShaderCom->Bind("g_bUseEmissiveTexture", true);
		}

		// _m, _mre : Reflection
		else if (aiTextureType_REFLECTION == textureAttribute.eTextureType)
		{
			_pShaderCom->Bind("g_MaskMapTexture", pShaderResourceView);
			_pShaderCom->Bind("g_bUseMaskMapTexture", true);
		}

		// 두번째 _d : Ambient
		else if (aiTextureType_AMBIENT == textureAttribute.eTextureType)
		{
			_pShaderCom->Bind("g_SecondDiffuseTexture", pShaderResourceView);
			_pShaderCom->Bind("g_bUseSecondDiffuseTexture", true);
		}

		// 두번째 _n : Displacement
		else if (aiTextureType_DISPLACEMENT == textureAttribute.eTextureType)
		{
			_pShaderCom->Bind("g_SecondNormalTexture", pShaderResourceView);
			_pShaderCom->Bind("g_bUseSecondNormalTexture", true);
		}

		// _l, _light : Specular
		else if (aiTextureType_SPECULAR == textureAttribute.eTextureType)
		{
			_pShaderCom->Bind("g_LightMapTexture", pShaderResourceView);
			_pShaderCom->Bind("g_bUseLightMapTexture", true);
		}
	}

	return S_OK;
}

CComponent* CMaterial::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CMaterial* pInstance = new CMaterial(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to create CMaterial");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CMaterial::Clone(void* _pArg)
{
	CMaterial* pInstance = new CMaterial(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to clone CMaterial");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CTexture* CMaterial::Get_Texture(aiTextureType _eTexType)
{
	if (m_vTextures.size() <= _eTexType || nullptr == m_vTextures[_eTexType].pComponent)
		return nullptr;
	else
		return static_cast<CTexture*>(m_vTextures[_eTexType].pComponent);
}

MATERIAL_FILE_DATA CMaterial::Get_MaterialFileData(aiTextureType _eTexType)
{
	MATERIAL_FILE_DATA tData;

	strcpy_s(tData.szTexName, m_vTextures[(_uint)_eTexType].szTexName);
	strcpy_s(tData.szTexFullPath, m_vTextures[(_uint)_eTexType].szTexFullPath);

	return tData;
}

HRESULT CMaterial::Get_MaterialAllFileDatas(MATERIAL_FILE_DATA* _pArrMtrlTextures)
{
	if (nullptr == _pArrMtrlTextures)
		return E_FAIL;

	for (_uint i = 0; i < (_uint)AI_TEXTURE_TYPE_MAX; ++i)
	{
		strcpy_s(_pArrMtrlTextures[i].szTexName, m_vTextures[i].szTexName);
		strcpy_s(_pArrMtrlTextures[i].szTexFullPath, m_vTextures[i].szTexFullPath);
	}

	return S_OK;
}


void CMaterial::Free()
{
	__super::Free();

	for (auto& tTextureMtrl : m_vTextures)

		Safe_Release(tTextureMtrl.pComponent);

	for (auto& texture : m_vecTextures)
		Safe_Release(texture.pTexture);

	m_vTextures.clear();
	m_vecTextures.clear();
}