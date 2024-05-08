#pragma once

#include "Component.h"

BEGIN(Engine)
class ENGINE_DLL CMaterial final: public CComponent
{
public:
	typedef struct tagTextureAttribute
	{
		tagTextureAttribute() = default;
		tagTextureAttribute(const tagTextureAttribute&) = default;
		tagTextureAttribute& operator=(const tagTextureAttribute&) = default;

		string strName;
		string strPath;
		class CTexture* pTexture = { nullptr };
		_float4 vBaseColor = SimpleMath::Vector4::Zero;
		aiTextureType	eTextureType;
	} TEXTURE_ATTRIBUTE;

private:
	CMaterial(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CMaterial(const CMaterial& _rhs);
	virtual ~CMaterial() = default;

public:
	HRESULT Initialize(void* _pArg) override;
	HRESULT Initialize_Prototype() override;

public:
	HRESULT Add_Texture(aiTextureType _eTexType, const char* _pTexName, const char* _pTexFullPath, CComponent* _pTextureCom);
	HRESULT Add_Textures(MATERIAL_FILE_DATA* _pArrMtrlTextures);
	HRESULT Bind_ShaderResources(class CShader* _pShaderCom);
	static CComponent* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CComponent* Clone(void* _pArg) override;

public:
	virtual void Free() override;

public:
	const vector<TEXTUREMTRLINFO>& Get_MtrlTextures() { return m_vTextures; }
	class CTexture* Get_Texture(aiTextureType _eTexType);
	MATERIAL_FILE_DATA				Get_MaterialFileData(aiTextureType _eTexType);
	HRESULT							Get_MaterialAllFileDatas(MATERIAL_FILE_DATA* _pArrMtrlTextures);

private:
	vector<TEXTUREMTRLINFO> m_vTextures;
	vector<TEXTURE_ATTRIBUTE> m_vecTextures;
};
END

