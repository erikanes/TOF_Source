#include "Client_pch.h"
#include "Client_Defines.h"
#include "UI_Minimap.h"

// Singleton
#include "GameInstance.h"

// GameObject
#include "Player.h"

// Component
#include "VIBuffer_Point.h"

#include "Client_Functions.h"
#include "Engine_Functions.h"

#define SHADER_PASS_MINIMAP_BACKGROUND	0
#define SHADER_PASS_MINIMAP_PLAYERARROW	1

CUI_Minimap::CUI_Minimap(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CUI_Base(_pDevice, _pContext)
{
}

CUI_Minimap::CUI_Minimap(const CUI_Minimap& _rhs)
	: CUI_Base(_rhs)
{
}

HRESULT CUI_Minimap::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Minimap::Initialize(void* _pArg)
{
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	if (FAILED(_Add_Components(_pArg)))
		return E_FAIL;

	/*
	���� �ִ� ������ ���� �̴ϸʿ����� �÷��̾� �̵� �ݰ� ����
	�ش� �̵� �ݰ濡 ���� uv ����
	*/

	// Player Transform
	auto pPlayer = CFunc::Get_Player();

	if (nullptr == pPlayer)
		return E_FAIL;

	m_pPlayerTransformCom = static_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));

	m_pPlayerTransformCom->Get_RollPitchYawFromWorldMatrix();
	/*
	m_vRange : min_x, max_x, min_z, max_z
	*/

	if (LEVEL_DESERT == m_eLevel)
	{
		m_vRange = _float4(-1024.f, 1024.f, -1024.f, 1024.f);
		m_fUVSize = 0.2f;
		m_iTextureIndex = 0;
	}

	else if (LEVEL_MIRRORCITY == m_eLevel)
	{
		m_vRange = _float4(5.f, 480.f, -225.f, 230.f);
		m_fUVSize = 0.2f;
		m_iTextureIndex = 1;
	}

	else if (LEVEL_ABYSS == m_eLevel)
	{
		m_vRange = _float4(-192.f, 192.f, -192.f, 192.f);
		m_fUVSize = 0.2f;
		m_iTextureIndex = 2;
	}

	// �׽�Ʈ ���� ���� ����ó���� ���� ���Ƿ� ����
	else
	{
		m_vRange = _float4(-1024.f, 1024.f, -1024.f, 1024.f);
		m_fUVSize = 0.2f;
		m_iTextureIndex = 0;
	}

	auto fX = CGlobal::g_fWinSizeX;
	auto fY = CGlobal::g_fWinSizeY;

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(fX, fY, 0.f, 1.f));

	m_vPosition = _float2(fX * 0.087f, fY * 0.2f);

	return S_OK;
}

void CUI_Minimap::Tick(_float _fTimeDelta)
{
	if (m_bEnable)
		__super::Tick(_fTimeDelta);
}

void CUI_Minimap::Late_Tick(_float _fTimeDelta)
{
	if (!m_bEnable)
		return;

	// ���� �Լ� ȣ������ �� ��

	if (m_bEnable)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Minimap::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(_Bind_ShaderResources()))
		return E_FAIL;

	/*
	�ؽ��� �ε���
	0~2 : ���
	3 : ȭ��ǥ
	4 : �̴ϸ� ����ũ
	*/

	/*
	���� �ܰ�
	1 : �̴ϸ� ��� ��ü ����
	2 : �̴ϸ� Ư�� ��ġ ����
	3 : ��� ����ŷ
	4 : ȭ��ǥ ���
	5 : �÷��̾� ����� ����ȭ
	6 : �þ߰� ����
	7 : �þ߰� ����ŷ
	*/

	_float fX = CGlobal::g_fWinSizeX;
	_float fY = CGlobal::g_fWinSizeY;

	// Background
	m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_DiffuseTexture", m_iTextureIndex);
	m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_MaskMapTexture", 4);
	m_pShaderCom->Bind("g_vSize", _float2(fY * 0.2f, fY * 0.2f));

	m_pShaderCom->Begin(SHADER_PASS_MINIMAP_BACKGROUND);
	m_pVIBufferCom->Render();

	// Player Arrow
	m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_DiffuseTexture", 3); // arrow
	m_pShaderCom->Bind("g_vSize", _float2(fY * 0.04f, fY * 0.04f));

	m_pShaderCom->Begin(SHADER_PASS_MINIMAP_PLAYERARROW);
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_Minimap::_Add_Components(void* _pArg)
{
	m_eLevel = CFunc::Get_CurLevel();

	if (FAILED(__super::Add_Component(LEVEL_STATIC, PC_TRANSFORM, TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, PC_RENDERER, TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_eLevel, PC_SHADER_MINIMAP, TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, PC_VIBUFFER_POINT, TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_eLevel, PC_TEXTURE_MINIMAP, TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Minimap::_Bind_ShaderResources()
{
	auto pGameInstance = CGameInstance::Get_Instance();

	m_pShaderCom->Bind("g_ViewMatrix", m_ViewMatrix);
	m_pShaderCom->Bind("g_ProjMatrix", m_ProjMatrix);

	m_pShaderCom->Bind("g_fWidth", CGlobal::g_fWinSizeX);
	m_pShaderCom->Bind("g_fHeight", CGlobal::g_fWinSizeY);

	m_pShaderCom->Bind("g_vRange", m_vRange);
	m_pShaderCom->Bind("g_vPosition", m_vPosition);

	_float fCameraFov = pGameInstance->Get_CameraFov();

	Vector3 vCamDir = pGameInstance->Get_CamForward();
	Vector4 vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION);
	Vector4 vPlayerDir = m_pPlayerTransformCom->Get_State(CTransform::STATE_LOOK);

	Vector2 vCamDirXZ(vCamDir.x, vCamDir.z);
	Vector2 vPlayerDirXZ(vPlayerDir.x, vPlayerDir.z);

	vCamDirXZ.Normalize();
	vPlayerDirXZ.Normalize();

	/*
	ī�޶� ���� ���ε� (�ȼ� ���̴����� -x,z => xy�� ġȯ)
	�÷��̾� ��ġ ���ε� (�ȼ� ���̴����� x,z => xy�� ġȯ)
	�÷��̾� ���� ���ε� (�ȼ� ���̴����� -x,z => xy�� ġȯ)
	������ζ�� x,-z�� ġȯ�ؾ� �ϴµ� �̴ϸ� ĸ�ĸ� �ݴ�� �߱� ������ ��ȣ�� �ݴ�� ����
	*/

	m_pShaderCom->Bind("g_vCamDirection", vCamDirXZ);
	m_pShaderCom->Bind("g_vPlayerPos", _float2(vPlayerPos.x, vPlayerPos.z));
	m_pShaderCom->Bind("g_vPlayerDir", vPlayerDirXZ);
	m_pShaderCom->Bind("g_fCameraFov", fCameraFov);
	m_pShaderCom->Bind("g_fUVSize", m_fUVSize);

	return S_OK;
}

CUI_Minimap* CUI_Minimap::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	auto pInstance = new CUI_Minimap(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created: CUI_Minimap::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Minimap::Clone(void* _pArg)
{
	auto pInstance = new CUI_Minimap(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned: CUI_Minimap::Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Minimap::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);

	m_pPlayerTransformCom = nullptr;
}