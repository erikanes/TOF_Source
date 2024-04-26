#pragma once

#include "Client_Defines.h"
#include "UI_Base.h"

BEGIN(Engine)
class CShader;
class CMaterial;
class CRenderer;

END

BEGIN(Client)

class CVIBuffer_Point_Damage_Instance;

class CUI_DamagePrinter final : public CUI_Base
{
private:
	enum DamageCalculateType : _uint
	{
		DCT_MONSTER_NORMAL,
		DCT_MONSTER_CRITICAL,
		DCT_PLAYER,
		DCT_MARK_CRITICAL,
		DCT_MARK_RESIST,
		DCT_END
	};

private:
	CUI_DamagePrinter(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CUI_DamagePrinter(const CUI_DamagePrinter& _rhs);
	virtual ~CUI_DamagePrinter() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* _pArg);
	virtual void Tick(_float _fTimeDelta);
	virtual void Late_Tick(_float _fTimeDelta);
	virtual HRESULT Render();

public:
	void Add_Damage(const _float3& _vPosition, _float _fDamage, ElementType _eElement);

private:
	HRESULT _Add_Components();
	HRESULT _Bind_ShaderResourceView();
	_float4 _Calculate_Color(ElementType _iElement);
	_float4 _Calculate_UV(_uint _iDamage);
	_float4 _Calculate_Player_UV(_uint _iDamage);
	_float2 _Calculate_SizeAlpha(const DAMAGE_INSTANCE_DESC& _desc);
	_float2 _Calculate_AddPosition(const DAMAGE_INSTANCE_DESC& _desc);
	void _Calculate_Damage_Desc(_float _fTimeDelta);

	void _Register_Damage(_float3 _vPosition, _float _fAddX, _float _fWaitTime, _uint _iDevidedDamage, const _float4& _vColor);
	void _Register_Critical_Damage(_float3 _vPosition, _float _fAddX, _float _fWaitTime, _uint _iDevidedDamage, const _float4& _vColor);
	void _Register_Player_Damage(_float3 _vPosition, _float _fAddX, _uint _iDevidedDamage, const _float4& _vColor);
	void _Register_Critical_Mark(_float3 _vPosition);
	void _Register_Resist_Mark(_float3 _vPosition);

public:
	static CUI_DamagePrinter* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;

private:
	CShader*							m_pShader	= { nullptr };
	CMaterial*							m_pMaterial = { nullptr };
	CRenderer*							m_pRenderer = { nullptr };
	CVIBuffer_Point_Damage_Instance*	m_pVIBuffer = { nullptr };

	_float4x4 m_matWorld = {};
	_float4x4 m_matView = {};
	_float4x4 m_matProj = {};

	list<DAMAGE_INSTANCE_DESC>			m_listDamage;
	
	vector<VTXDAMAGEINSTANCE>			m_vecDamageDesc;
};

END