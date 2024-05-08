#include "Client_pch.h"
#include "UI_DamagePrinter.h"

// Singleton
#include "GameInstance.h"

// Component
#include "VIBuffer_Point_Damage_Instance.h"

#include "Engine_Math.h"

#include "Engine_Functions.h"

static constexpr _float INIT_LIFETIME = 1.f;
static constexpr _float FONT_NORMAL_SIZE = 60.f;
static constexpr _float FONT_NORMAL_MARGIN = FONT_NORMAL_SIZE / 2.5f;
static constexpr _float FONT_CRITICAL_SIZE = 80.f;
static constexpr _float FONT_CRITICAL_MARGIN = FONT_CRITICAL_SIZE / 2.5f;
static constexpr _float FONT_PLAYER_SIZE = 40.f;
static constexpr _float FONT_PLAYER_MARGIN = FONT_PLAYER_SIZE / 2.f;
static constexpr _float MARK_CRITICAL_SIZE = 40.f;
static constexpr _float ADD_SPEED_Y = -20.f;
static constexpr _float SPAWN_INTERVAL = 0.05f;
static constexpr _float START_RATIO = 0.1f;
static constexpr _float END_RATIO = 0.8f;

CUI_DamagePrinter::CUI_DamagePrinter(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CUI_Base(_pDevice, _pContext)
{
}

CUI_DamagePrinter::CUI_DamagePrinter(const CUI_DamagePrinter& _rhs)
	: CUI_Base(_rhs)
{
}

HRESULT CUI_DamagePrinter::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_DamagePrinter::Initialize(void* _pArg)
{
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	if (FAILED(_Add_Components()))
		return E_FAIL;

	XMStoreFloat4x4(&m_matWorld, XMMatrixIdentity());
	XMStoreFloat4x4(&m_matView, XMMatrixIdentity());
	XMStoreFloat4x4(&m_matProj, XMMatrixOrthographicLH(CGlobal::g_fWinSizeX, CGlobal::g_fWinSizeY, 0.f, 1.f));

	return S_OK;
}

void CUI_DamagePrinter::Tick(_float _fTimeDelta)
{
}

void CUI_DamagePrinter::Late_Tick(_float _fTimeDelta)
{
	__super::Late_Tick(_fTimeDelta);

	if (m_listDamage.empty())
		return;

	_Calculate_Damage_Desc(_fTimeDelta);

	m_pVIBuffer->Late_Tick(_fTimeDelta, m_vecDamageDesc);

	m_pRenderer->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_DamagePrinter::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(_Bind_ShaderResourceView()))
		return E_FAIL;

	m_pShader->Begin(0);
	m_pVIBuffer->Render();

	m_vecDamageDesc.clear();

	return S_OK;
}

void CUI_DamagePrinter::Add_Damage(const _float3& _vPosition, _float _fDamage, ElementType _eElement)
{
	auto iDamage = static_cast<_uint>(_fDamage);
	auto fNumOfDigit = floor(log10(_fDamage));

	_float4 vColor = _Calculate_Color(_eElement);

	// Critical
	if (_eElement & ELEMENT_HIT_WEAK)
	{
		_Register_Critical_Mark(_vPosition);
		_Register_Critical_Damage(_vPosition, fNumOfDigit * FONT_CRITICAL_MARGIN, fNumOfDigit * SPAWN_INTERVAL, iDamage, vColor);
	}
	// Player
	else if (_eElement & ELEMENT_HIT_PLAYER)
		_Register_Player_Damage(_vPosition, fNumOfDigit * FONT_NORMAL_MARGIN, iDamage, vColor);
	else
		_Register_Damage(_vPosition, fNumOfDigit * FONT_NORMAL_MARGIN, fNumOfDigit * SPAWN_INTERVAL, iDamage, vColor);
}

HRESULT CUI_DamagePrinter::_Add_Components()
{
	auto eCurrentLevel = CFunc::Get_CurLevel();
	auto pGameInstance = CGameInstance::Get_Instance();

	if (FAILED(__super::Add_Component(LEVEL::LEVEL_STATIC, PC_TRANSFORM, TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(eCurrentLevel, PC_SHADER_DAMAGE, TEXT("Com_Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL::LEVEL_STATIC, PC_MATERIAL, TEXT("Com_Material"), (CComponent**)&m_pMaterial)))
		return E_FAIL;

	auto pFont = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Damage/T_Digits.dds"));
	auto pCritical = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Damage/icon_baoji_fx.dds"));

	m_pMaterial->Add_Texture(aiTextureType_DIFFUSE, "Font", "../Bin/Resources/UI/Damage/T_Digits.dds", pFont);
	m_pMaterial->Add_Texture(aiTextureType_NORMALS, "Critical", "../Bin/Resources/UI/Damage/icon_baoji_fx.dds", pCritical);

	if (FAILED(__super::Add_Component(LEVEL::LEVEL_STATIC, PC_RENDERER, TEXT("Com_Renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(eCurrentLevel, PC_VIBUFFER_POINT_DAMAGE_INSTANCE, TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_DamagePrinter::_Bind_ShaderResourceView()
{
	auto pGameInstance = CGameInstance::Get_Instance();

	auto matView = pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
	auto matProj = pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ);

	m_pShader->Bind("g_ViewMatrix", matView);
	m_pShader->Bind("g_ProjMatrix", matProj);
	m_pShader->Bind("g_OrthoProjMatrix", m_matProj);

	m_pShader->Bind("g_fWidth", CGlobal::g_fWinSizeX);
	m_pShader->Bind("g_fHeight", CGlobal::g_fWinSizeY);

	m_pMaterial->Bind_ShaderResources(m_pShader);

	return S_OK;
}

_float4 CUI_DamagePrinter::_Calculate_Color(ElementType _iElement)
{
	Vector4 vLightning(0.7f, 0.615f, 1.f, 1.f);
	Vector4 vFire(1.f, 0.69f, 0.42f, 1.f);
	Vector4 vIce(0.478f, 0.69f, 0.96f, 1.f);

	if (_iElement & ELEMENT_HIT_WEAK)
	{
		if (_iElement & ELEMENT_LIGHTNING)
			return vLightning;

		else if (_iElement & ELEMENT_FIRE)
			return vFire;

		else if (_iElement & ELEMENT_ICE)
			return vIce;// *0.7f;

		else
			return _float4(1.f, 1.f, 1.f, 1.f);
	}
	else if (_iElement & ELEMENT_HIT_PLAYER)
		return _float4(0.93f, 0.3f, 0.34f, 1.f);
	else
	{
		if (_iElement & ELEMENT_LIGHTNING)
			return vLightning;
		else if (_iElement & ELEMENT_FIRE)
			return vFire;
		else if (_iElement & ELEMENT_ICE)
			return vIce;
		else
			return _float4(1.f, 1.f, 1.f, 1.f);
	}
}

_float4 CUI_DamagePrinter::_Calculate_UV(_uint _iDamage)
{
	_float4 vUV;
	ZeroMemory(&vUV, sizeof(_float4));

	if (1 <= _iDamage && 9 >= _iDamage)
	{
		vUV.x = fmodf(static_cast<_float>(_iDamage - 1) / 3.f, 1.f);
		vUV.y = floor(static_cast<_float>(_iDamage - 1) / 3.f) / 4.f;
		vUV.z = fmodf(static_cast<_float>(_iDamage) / 3.f, 1.f);
		vUV.w = ceil(static_cast<_float>(_iDamage) / 3.f) / 4.f;

		vUV.z = 0.f >= vUV.z ? 1.f : vUV.z;
	}
	else
		vUV = _float4(0.f, 0.75f, 0.33f, 1.f);

	return vUV;
}

_float4 CUI_DamagePrinter::_Calculate_Player_UV(_uint _iDamage)
{
	_float4 vUV;
	ZeroMemory(&vUV, sizeof(_float4));

	if (1 <= _iDamage && 9 >= _iDamage)
	{
		vUV.x = fmodf(static_cast<_float>(_iDamage - 1) / 3.f, 1.f);
		vUV.y = floor(static_cast<_float>(_iDamage - 1) / 3.f) / 4.f;
		vUV.z = fmodf(static_cast<_float>(_iDamage) / 3.f, 1.f);
		vUV.w = ceil(static_cast<_float>(_iDamage) / 3.f) / 4.f;

		vUV.z = 0.f >= vUV.z ? 1.f : vUV.z;
	}
	else
		vUV = _float4(0.f, 0.75f, 0.33f, 1.f);

	return vUV;
}

_float2 CUI_DamagePrinter::_Calculate_SizeAlpha(const DAMAGE_INSTANCE_DESC& _desc)
{
	_float fSize = 0.f;
	_float fAlpha = 1.f;

	if (DCT_MONSTER_NORMAL == _desc.iType)
		fSize = clamp(8.f - (_desc.fProgressTime - _desc.fWaitTime) * 80.f, 1.f, 8.f) * FONT_NORMAL_SIZE;
	else if (DCT_MONSTER_CRITICAL == _desc.iType)
		fSize = clamp(8.f - (_desc.fProgressTime - _desc.fWaitTime) * 80.f, 1.f, 8.f) * FONT_CRITICAL_SIZE;
	else if (DCT_PLAYER == _desc.iType)
		fSize = FONT_PLAYER_SIZE;
	else if (DCT_MARK_CRITICAL == _desc.iType)
	{
		fSize = MARK_CRITICAL_SIZE;
		fAlpha = 0.8f;
	}

	if (_desc.fProgressTime - _desc.fWaitTime <= _desc.fLifeTime * START_RATIO)
	{
		_float fNewWholeTime = _desc.fLifeTime * START_RATIO;
		_float fNewProgressTime = clamp(_desc.fProgressTime - _desc.fWaitTime, 0.f, fNewWholeTime);

		fAlpha = (fNewProgressTime / fNewWholeTime) * fAlpha;
	}
	else if (_desc.fProgressTime >= _desc.fLifeTime * END_RATIO)
	{
		_float fNewWholeTime = _desc.fLifeTime - _desc.fLifeTime * END_RATIO;
		_float fNewProgressTime = _desc.fProgressTime - _desc.fLifeTime * END_RATIO;

		fAlpha = fAlpha - fNewProgressTime / fNewWholeTime;
	}

	return _float2(fSize, fAlpha);
}

_float2 CUI_DamagePrinter::_Calculate_AddPosition(const DAMAGE_INSTANCE_DESC& _desc)
{
	if (DCT_PLAYER != _desc.iType)
	{
		if (_desc.fProgressTime >= _desc.fLifeTime * END_RATIO)
		{
			auto fWholeTime = _desc.fLifeTime - _desc.fLifeTime * END_RATIO;
			auto fNewProgressTime = _desc.fProgressTime - _desc.fLifeTime * END_RATIO;

			auto fAddY = fNewProgressTime / fWholeTime * ADD_SPEED_Y;
			auto vPosition = _desc.vAddPosition;
			vPosition.y += fAddY;

			return vPosition;
		}
		else
			return _desc.vAddPosition;
	}

	return _desc.vAddPosition;
}

void CUI_DamagePrinter::_Calculate_Damage_Desc(_float _fTimeDelta)
{
	m_vecDamageDesc.reserve(m_listDamage.size());

	for (auto iter = m_listDamage.begin(); iter != m_listDamage.end();)
	{
		auto& damage = *iter;

		if (damage.fProgressTime >= damage.fLifeTime)
		{
			iter = m_listDamage.erase(iter);
			continue;
		}

		else if (damage.fProgressTime >= damage.fWaitTime)
		{
			VTXDAMAGEINSTANCE desc;
			desc.vColor = damage.vColor;
			desc.vUV = damage.vUV;
			desc.vPosition = damage.vPosition;
			desc.vAddPosition = _Calculate_AddPosition(damage);
			desc.vSizeAlpha = _Calculate_SizeAlpha(damage);
			desc.iType = damage.iType;

			m_vecDamageDesc.push_back(desc);
		}

		damage.fProgressTime += _fTimeDelta;

		++iter;
	}
}

void CUI_DamagePrinter::_Register_Damage(_float3 _vPosition, _float _fAddX, _float _fWaitTime, _uint _iDevidedDamage, const _float4& _vColor)
{
	if (0 >= _iDevidedDamage)
		return;

	_Register_Damage(_vPosition, _fAddX - FONT_NORMAL_MARGIN, _fWaitTime - SPAWN_INTERVAL, _iDevidedDamage / 10, _vColor);

	DAMAGE_INSTANCE_DESC desc;
	desc.fLifeTime = INIT_LIFETIME;
	desc.fProgressTime = 0.f;
	desc.fWaitTime = _fWaitTime;
	desc.vColor = _vColor;
	desc.vPosition = _vPosition;
	desc.vAddPosition = _float2(_fAddX, 0.f);
	desc.vUV = _Calculate_UV(_iDevidedDamage % 10);
	desc.iType = DCT_MONSTER_NORMAL;

	m_listDamage.emplace_back(desc);
}

void CUI_DamagePrinter::_Register_Critical_Damage(_float3 _vPosition, _float _fAddX, _float _fWaitTime, _uint _iDevidedDamage, const _float4& _vColor)
{
	if (0 >= _iDevidedDamage)
		return;

	_Register_Critical_Damage(_vPosition, _fAddX - FONT_CRITICAL_MARGIN, _fWaitTime - SPAWN_INTERVAL, _iDevidedDamage / 10, _vColor);

	DAMAGE_INSTANCE_DESC desc;
	desc.fLifeTime = INIT_LIFETIME;
	desc.fProgressTime = 0.f;
	desc.fWaitTime = _fWaitTime;
	desc.vColor = _vColor;
	desc.vPosition = _vPosition;
	desc.vAddPosition = _float2(_fAddX, 0.f);
	desc.vUV = _Calculate_UV(_iDevidedDamage % 10);
	desc.iType = DCT_MONSTER_CRITICAL;

	m_listDamage.emplace_back(desc);
}

void CUI_DamagePrinter::_Register_Player_Damage(_float3 _vPosition, _float _fAddX, _uint _iDevidedDamage, const _float4& _vColor)
{
	if (0 >= _iDevidedDamage)
		return;

	_Register_Player_Damage(_vPosition, _fAddX - FONT_PLAYER_MARGIN, _iDevidedDamage / 10, _vColor);

	DAMAGE_INSTANCE_DESC desc;
	desc.fLifeTime = INIT_LIFETIME;
	desc.fProgressTime = 0.f;
	desc.fWaitTime = 0.f;
	desc.vColor = _vColor;
	desc.vPosition = _vPosition;
	desc.vAddPosition = _float2(_fAddX, 0.f);
	desc.vUV = _Calculate_Player_UV(_iDevidedDamage % 10);
	desc.iType = DCT_PLAYER;

	m_listDamage.emplace_back(desc);
}

void CUI_DamagePrinter::_Register_Critical_Mark(_float3 _vPosition)
{
	DAMAGE_INSTANCE_DESC desc;
	desc.fLifeTime = INIT_LIFETIME;
	desc.fProgressTime = 0.f;
	desc.fWaitTime = 0.f;
	desc.vColor = _float4(1.f, 1.f, 1.f, 1.f);
	desc.vPosition = _vPosition;
	desc.vAddPosition = _float2(-FONT_CRITICAL_MARGIN * 0.6f, -20.f);
	desc.vUV = _float4(0.f, 0.f, 1.f, 1.f);
	desc.iType = DCT_MARK_CRITICAL;

	m_listDamage.emplace_back(desc);
}

void CUI_DamagePrinter::_Register_Resist_Mark(_float3 _vPosition)
{
	DAMAGE_INSTANCE_DESC desc;
	desc.fLifeTime = INIT_LIFETIME;
	desc.fProgressTime = 0.f;
	desc.fWaitTime = 0.f;
	desc.vColor = _float4(1.f, 1.f, 1.f, 1.f);
	desc.vPosition = _vPosition;
	desc.vAddPosition = _float2(-FONT_CRITICAL_MARGIN * 0.5f, 0.f);
	desc.vUV = _float4(0.f, 0.f, 1.f, 1.f);
	desc.iType = DCT_MARK_RESIST;

	m_listDamage.emplace_back(desc);
}

CUI_DamagePrinter* CUI_DamagePrinter::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	auto pInstance = new CUI_DamagePrinter(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create: CUI_DamagePrinter");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_DamagePrinter::Clone(void* _pArg)
{
	auto pInstance = new CUI_DamagePrinter(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned: CUI_DamagePrinter");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_DamagePrinter::Free()
{
	__super::Free();

	Safe_Release(m_pShader);
	Safe_Release(m_pMaterial);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pVIBuffer);

	m_listDamage.clear();
}
