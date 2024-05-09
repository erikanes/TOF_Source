#include "Client_pch.h"
#include "Player_Weapon.h"
#include "GameInstance.h"

#include "Effect_Coll.h"

#include "Engine_Functions.h"
/*
CPlayer_Weapon::CPlayer_Weapon(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CPlayer_Equipment(_pDevice, _pContext)
{
}

CPlayer_Weapon::CPlayer_Weapon(const CPlayer_Weapon& _rhs)
	: CPlayer_Equipment(_rhs)
	, m_eElementType(_rhs.m_eElementType)
{
}


_vector CPlayer_Weapon::Get_RightWeapon_TransformState(_uint _eState)
{
	return XMLoadFloat4x4(&m_matRightHandWorldMatrix).r[_eState];
}

HRESULT CPlayer_Weapon::Initialize_Prototype()
{

	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Weapon::Initialize(void* _pArg)
{
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	m_tWeaponStatus.fBaseAttack = 1000.f; // 기본 공격력
	m_tWeaponStatus.fHealth = 1000.f; // 추가 체력
	m_tWeaponStatus.fCritical = 1000.f; // 크리티컬 시 추가 데미지?
	m_tWeaponStatus.fSkillMagnification = 1.f; // 스킬 배율. 필요 없을수도 있음


	return S_OK;
}
*/
pair<_float, _bool> CPlayer_Weapon::Calculate_Damage(_float _fBasiceDamage)
{
	// (캐릭터 기본 공격력 + 무기 공격력 + @) * 1.25 => 최종데미지
	// 최종 데미지 * 1.5 + (크리티컬 데미지 추가 / 2)

	// 크리티컬 확률 20%
	_bool bIsCritical = 20.f >= CFunc::Generate_Random_Float(0.1f, 100.f);
	_float fResultDamage = (_fBasiceDamage + m_tWeaponInfoDesc.fAttackPower) * 1.25f * CFunc::Generate_Random_Float(0.95f, 1.05f);
	fResultDamage *= bIsCritical ? 1.5f : 1.f;
	fResultDamage += bIsCritical ? m_tWeaponInfoDesc.fCritical : 0.f;

	return { fResultDamage, bIsCritical };
}

void CPlayer_Weapon::Play_HitSound(GameObjectID _iID)
{
	if (ID_MONSTER_BOSS_RUDOLPH == _iID || ID_MONSTER_BOSS_APOPHIS == _iID)
	{
		auto iIndex = CFunc::Generate_Random_Int(1, 2);
		string strAudioTag = "Weapon_Impact_Steel_" + to_string(iIndex);

		CGameInstance::Get_Instance()->Play_Audio(strAudioTag);
	}
}

void CPlayer_Weapon::Disable_Weapon()
{
	In_Weapon();

	m_fDissolveProgressTime = 5.f;
	m_fDissolveAlpha = 1.f;
}

void CPlayer_Weapon::Tick(_float _fTimeDelta)
{
	__super::Tick(_fTimeDelta);

	Calculate_Dissolve(_fTimeDelta);
}

void CPlayer_Weapon::Late_Tick(_float _fTimeDelta)
{
	if (5.f <= m_fDissolveProgressTime)
		return;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PLAYER, this);

	__super::Late_Tick(_fTimeDelta);
}

HRESULT CPlayer_Weapon::Render()
{
	if (5.f <= m_fDissolveProgressTime && !m_bOnlyRenderDummyModel)
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(CPlayer_Weapon::Bind_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

/*
void CPlayer_Weapon::Create_WeaponEffect_RightHand(wstring _wstrWeaponEffect, _float3 _vFixRUL, _bool _bMaintainPointerPos, _float3 _vFixPlayerRUL, _bool _bMaintainWorldMatrix, _bool _bMaintainPivotPointer, _bool _bMaintainPivotPointerPos)
{
	CE_Effect_Coll::CLONEEFFECTDESC CloneEffectDesc;
	CloneEffectDesc.bMaintainPointerPos = _bMaintainPointerPos;
	CloneEffectDesc.bMaintainWorldMatrix = _bMaintainWorldMatrix;
	CloneEffectDesc.bMaintainPivotPointerPos = _bMaintainPivotPointerPos;
	if (true == _bMaintainPointerPos || true == _bMaintainWorldMatrix|| true == _bMaintainPivotPointerPos)
	{
		CloneEffectDesc.pWorldMatrix = &m_matRightHandWorldMatrix;
	}
	else
		CloneEffectDesc.pWorldMatrix = nullptr;
	CloneEffectDesc.bMaintainPivotPointerPos = _bMaintainPivotPointer;

	//무기 이펙트는 뼈 안씀
	// 보정
	CloneEffectDesc.vPointerFixPos = { 0.f, 0.f, 0.f };
	CloneEffectDesc.pBoneMatrix = nullptr;
	CloneEffectDesc.PivotMatrix = IDENTITY_MATRIX;
	// 생성 위치
	_float3 vRight;
	memcpy_s(&vRight, sizeof _float3, &m_matRightHandWorldMatrix.m[0], sizeof _float3);
	_float3 vUp;
	memcpy_s(&vUp, sizeof _float3, &m_matRightHandWorldMatrix.m[1], sizeof _float3);
	_float3 vLook;
	memcpy_s(&vLook, sizeof _float3, &m_matRightHandWorldMatrix.m[2], sizeof _float3);

	_float3 vPlayerRight;
	memcpy_s(&vPlayerRight, sizeof _float3, &m_pParentMatrix->m[0], sizeof _float3);
	_float3 vPlayerUp;
	memcpy_s(&vPlayerUp, sizeof _float3, &m_pParentMatrix->m[1], sizeof _float3);
	_float3 vPlayerLook;
	memcpy_s(&vPlayerLook, sizeof _float3, &m_pParentMatrix->m[2], sizeof _float3);

	_float4 vPos;
	memcpy_s(&vPos, sizeof _float4, &m_matRightHandWorldMatrix.m[3], sizeof _float4);
	XMStoreFloat4(&CloneEffectDesc.vPos, XMLoadFloat4(&vPos)
		+ XMVector3Normalize(XMLoadFloat3(&vRight)) * _vFixRUL.x
		+ XMVector3Normalize(XMLoadFloat3(&vUp)) * _vFixRUL.y
		+ XMVector3Normalize(XMLoadFloat3(&vLook)) * _vFixRUL.z
		+ XMVector3Normalize(XMLoadFloat3(&vPlayerRight)) * _vFixPlayerRUL.x
		+ XMVector3Normalize(XMLoadFloat3(&vPlayerUp)) * _vFixPlayerRUL.y
		+ XMVector3Normalize(XMLoadFloat3(&vPlayerLook)) * _vFixPlayerRUL.z
	);

	// 생성 룩
	CloneEffectDesc.vLook = vUp;
	CloneEffectDesc.vRight = vRight;
	CloneEffectDesc.pDead = nullptr;

	CloneEffectDesc.vPointerFixPos = _vFixRUL;

	if (nullptr == CGameInstance::Get_Instance()->Pop_FromObjectPool(_wstrWeaponEffect, &CloneEffectDesc))
		MSG_BOX("Filed to Create Right Hand Effect");
}

void CPlayer_Weapon::Create_WeaponEffect_LeftHand(wstring _wstrWeaponEffect, _float3 _vFixRUL, _bool _bMaintainPointerPos, _float3 _vFixPlayerRUL, _bool _bMaintainWorldMatrix, _bool _bMaintainPivotPointer)
{
	CE_Effect_Coll::CLONEEFFECTDESC CloneEffectDesc;
	CloneEffectDesc.bMaintainPointerPos = _bMaintainPointerPos;
	CloneEffectDesc.bMaintainWorldMatrix = _bMaintainWorldMatrix;
	if (true == _bMaintainPointerPos || true == _bMaintainWorldMatrix)
	{
		CloneEffectDesc.pWorldMatrix = &m_matLeftHandWorldMatrix;
	}
	else
		CloneEffectDesc.pWorldMatrix = nullptr;
	CloneEffectDesc.bMaintainPivotPointerPos = _bMaintainPivotPointer;

	//무기 이펙트는 뼈 안씀
	// 보정
	CloneEffectDesc.vPointerFixPos = { 0.f, 0.f, 0.f };
	CloneEffectDesc.pBoneMatrix = nullptr;
	CloneEffectDesc.PivotMatrix = IDENTITY_MATRIX;
	// 생성 위치
	_float3 vRight;
	memcpy_s(&vRight, sizeof _float3, &m_matLeftHandWorldMatrix.m[0], sizeof _float3);
	_float3 vUp;
	memcpy_s(&vUp, sizeof _float3, &m_matLeftHandWorldMatrix.m[1], sizeof _float3);
	_float3 vLook;
	memcpy_s(&vLook, sizeof _float3, &m_matLeftHandWorldMatrix.m[2], sizeof _float3);

	_float3 vPlayerRight;
	memcpy_s(&vPlayerRight, sizeof _float3, &m_pParentMatrix->m[0], sizeof _float3);
	_float3 vPlayerUp;
	memcpy_s(&vPlayerUp, sizeof _float3, &m_pParentMatrix->m[1], sizeof _float3);
	_float3 vPlayerLook;
	memcpy_s(&vPlayerLook, sizeof _float3, &m_pParentMatrix->m[2], sizeof _float3);

	_float4 vPos;
	memcpy_s(&vPos, sizeof _float4, &m_matLeftHandWorldMatrix.m[3], sizeof _float4);
	XMStoreFloat4(&CloneEffectDesc.vPos, XMLoadFloat4(&vPos)
		+ XMVector3Normalize(XMLoadFloat3(&vRight)) * _vFixRUL.x
		+ XMVector3Normalize(XMLoadFloat3(&vUp)) * _vFixRUL.y
		+ XMVector3Normalize(XMLoadFloat3(&vLook)) * _vFixRUL.z
		+ XMVector3Normalize(XMLoadFloat3(&vPlayerRight)) * _vFixPlayerRUL.x
		+ XMVector3Normalize(XMLoadFloat3(&vPlayerUp)) * _vFixPlayerRUL.y
		+ XMVector3Normalize(XMLoadFloat3(&vPlayerLook)) * _vFixPlayerRUL.z
	);

	// 생성 룩
	CloneEffectDesc.vLook = vUp;
	CloneEffectDesc.vRight = vRight;
	CloneEffectDesc.pDead = nullptr;

	CloneEffectDesc.vPointerFixPos = _vFixRUL;

	if (nullptr == CGameInstance::Get_Instance()->Pop_FromObjectPool(_wstrWeaponEffect, &CloneEffectDesc))
		MSG_BOX("Filed to Create Left Hand Effect");
}
*/

HRESULT CPlayer_Weapon::Bind_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	_float4x4 matView = pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
	_float4x4 matProj = pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ);

	_float4 vCamPosition = pGameInstance->Get_CamPosition();
	_float fCameraFar = pGameInstance->Get_CameraFar();

	auto pDissolveTexture = pGameInstance->Get_Texture_SRV(LEVEL_STATIC, PC_TEXTURE_NOISE, 0);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &matView)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &matProj)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind("g_fCameraFar", fCameraFar)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind("g_fDissolveAlpha", m_fDissolveAlpha)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind("g_vDissolveColor", m_vDissolveColor)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_ShaderResourceView("g_DissolveTexture", pDissolveTexture)))
		return E_FAIL;

	return S_OK;
}

void CPlayer_Weapon::Calculate_Dissolve(_float _fTimeDelta)
{
	//무기상태
	//WEAPON_IN : 무기가 플레이어 손에 있지 않은 상태. IN 상태에서 일정시간 지나면 디졸브로 없애기
	//WEAPON_OUT : 무기가 플레이어 손에 있는 상태
	//                                                                          
	//추가로 필요해 보이는 것
	//WEAPON_USE : 공격 버튼 눌렀을 때.
	//WEAPON_DISABLE : 아예 렌더링이 안되는 상태

	//플레이어 상태에 따른 것
	//공격으로 인해 WEAPON_USE가 계속 갱신되지 않는 이상 WEAPON_OUT

	m_fDissolveProgressTime = clamp(m_fDissolveProgressTime + _fTimeDelta, 0.f, 5.f);

	// 무기 수납
	if (WEAPON_STATE::WEAPON_IN == m_eWeaponState)
	{
		if (5.f <= m_fDissolveProgressTime)
		{
			if (m_bSpawnableOutWeaponEffect)
				Spawn_DisableEffect();

			m_bSpawnableOutWeaponEffect = false;
		}

		m_fDissolveAlpha = clamp(m_fDissolveProgressTime * 10.f - 10.f, 0.f, 1.f);
	}

	// 무기 꺼냄
	else if (WEAPON_STATE::WEAPON_OUT == m_eWeaponState)
	{
		m_fDissolveProgressTime = 0.f;
		m_fDissolveAlpha = 1.f;
	}
}

void CPlayer_Weapon::Free()
{
	__super::Free();

}