#include "Client_pch.h"
#include "LensFlare.h"

// Singleton
#include "GameInstance.h"
#include "PipeLine.h"
#include "Render_Manager.h"

// Component
#include "Transform.h"
#include "Shader.h"
//#include "Material.h"
#include "Renderer.h"
#include "Collider.h"
#include "VIBuffer_LensFlare.h"
#include "Texture.h"

// Etc
#include "Engine_Math.h"

#define SHADER_PASS_LENSFLARE_CORONA	0
#define SHADER_PASS_LENSFLARE_FLARE		1

CLensFlare::CLensFlare(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject(_pDevice, _pContext)
{
}

CLensFlare::CLensFlare(const CLensFlare& _rhs)
	: CGameObject(_rhs)
{
}

CLensFlare::~CLensFlare()
{
}

HRESULT CLensFlare::Initialize(void* _pArg)
{
	m_funcTick = mem_fn(&CLensFlare::_FindLight);
	m_funcLateTick = [](CLensFlare*, _float) {};
	m_funcRender = [](CLensFlare*) { return S_OK; };

	if (FAILED(_Add_Components(_pArg)))
	{
		return E_FAIL;
	}

	D3D11_QUERY_DESC queryDesc;
	ZeroMemory(&queryDesc, sizeof(D3D11_QUERY_DESC));

	queryDesc.Query = D3D11_QUERY_OCCLUSION_PREDICATE;
	queryDesc.MiscFlags = 0;

	m_pDevice->CreatePredicate(&queryDesc, &m_pPredicate);

	ZeroMemory(&queryDesc, sizeof(D3D11_QUERY_DESC));

	queryDesc.Query = D3D11_QUERY_OCCLUSION;

	m_pDevice->CreateQuery(&queryDesc, &m_pOcclusionQuery);

	m_fSunVisibility = 0.f;
	m_bQuerySunVisibility = true;

	m_tFlares.resize(11);
	ZeroMemory(&m_tFlares.front(), sizeof(LENSFLARE_FLARE) * m_tFlares.size());

	_uint iTextureIndex[11] = { 0, 4, 2, 7, 3, 5, 7, 3, 5, 4, 8 };

	for (_uint i = 0; i < 11; ++i)
	{
		m_tFlares[i].iTextureIndex = iTextureIndex[i];
		m_tFlares[i].vColor = _float4(1.f, 1.f, 1.f, 1.f);
	}

	m_tCorona.vColor = _float4(1.f, 1.f, 1.f, 1.f);

	return S_OK;
}

HRESULT CLensFlare::Initialize_Prototype()
{
	m_funcTick = mem_fn(&CLensFlare::_FindLight);

	return S_OK;
}

void CLensFlare::Tick(_float _fTimeDelta)
{
	/*
	광원의 위치와 카메라 방향에 따라 직선 생성

	플레어를 구성하는 텍스쳐에 색상, 알파, 스케일 적당히 적용
	화면 중심에 가까워질수록 크기는 커지고 투명도가 줄어든다

	광원의 좌표와 화면의 중앙을 잇는 선분 생성

	차폐 처리
	- 태양의 2D 투영좌표가 해상도를 벗어나는 경우를 체크
	*/

	if (CGlobal::g_bUseLensFlare)
		m_funcTick(this, _fTimeDelta);
}

void CLensFlare::Late_Tick(_float _fTimeDelta)
{
	if (CGlobal::g_bUseLensFlare)
		m_funcLateTick(this, _fTimeDelta);
}

HRESULT CLensFlare::Render()
{
	return m_funcRender(this);
}



void CLensFlare::_FindLight(_float _fTimeDelta)
{
	auto pGameInstance = CGameInstance::Get_Instance();

	const auto pLight = pGameInstance->Get_Light(0);

	if (nullptr == pLight)
		return;

	m_pLight = pLight;

	m_funcTick = mem_fn(&CLensFlare::_Update);
	m_funcLateTick = mem_fn(&CLensFlare::_Late_Update);
}

void CLensFlare::_Update(_float _fTimeDelta)
{
}

void CLensFlare::_Late_Update(_float _fTimeDelta)
{
	_Update_Transform();

	if (_IsInViewport())
	{
		CRender_Manager::Get_Instance()->Enable_SSLR();
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_LENSFLARE, this);
		m_funcRender = mem_fn(&CLensFlare::_Render);

		m_fProgressTime = clamp(m_fProgressTime + _fTimeDelta * 2.f, 0.f, 1.f);

#ifdef _DEBUG
		m_pRendererCom->Add_DebugComponent(m_pColliderCom);
#endif
	}

	else
	{
		CRender_Manager::Get_Instance()->Disable_SSLR();
		m_fBeforeCdotL = 0.f;
		m_fProgressTime = 0.f;
	}

	// 디버깅 코드
	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
}

HRESULT CLensFlare::_Render()
{
	// Corona
	if (nullptr != m_pShaderCom)
	{
		m_pShaderCom->Bind("g_vPosition", &m_tCorona.vPosition, 1);
		m_pShaderCom->Bind("g_vScaleRotate", &m_tCorona.vScaleRotate, 1);
		m_pShaderCom->Bind("g_vColor", &m_tCorona.vColor, 1);
		m_pShaderCom->Bind("g_fBrightness", m_tCorona.fBrightness);

		m_pShaderCom->Bind("g_fWidth", CGlobal::g_fWinSizeX);
		m_pShaderCom->Bind("g_fHeight", CGlobal::g_fWinSizeY);

		m_pTextureCom->Bind_ShaderResourceViews(m_pShaderCom, "g_Texture");

		m_pShaderCom->Begin(SHADER_PASS_LENSFLARE_CORONA);
	}

	if (nullptr != m_pVIBufferCom)
		m_pVIBufferCom->Render();

	// Flares
	if (nullptr != m_pShaderCom)
	{
		_float4 vPosition[11] = {};
		_float4 vScaleRotate[11] = {};
		_float4 vColor[11] = {};
		_int iTextureIndex[11] = {};

		for (_uint i = 0; i < 11; ++i)
		{
			vPosition[i] = m_tFlares[i].vOffset;
			vScaleRotate[i] = m_tFlares[i].vScaleRotate;
			vColor[i] = m_tFlares[i].vColor;
			iTextureIndex[i] = m_tFlares[i].iTextureIndex;
		}

		m_pShaderCom->Bind("g_vPosition", &vPosition[0], 11);
		m_pShaderCom->Bind("g_vScaleRotate", &vScaleRotate[0], 11);
		m_pShaderCom->Bind("g_vColor", &vColor[0], 11);
		m_pShaderCom->Bind("g_iTextureIndex", &iTextureIndex[0], 11);

		m_pShaderCom->Begin(SHADER_PASS_LENSFLARE_FLARE);
	}

	if (nullptr != m_pVIBufferCom)
		m_pVIBufferCom->Render();

	return S_OK;
}

void CLensFlare::_Update_Transform()
{
	const auto& vPos = m_pLight->vPosition;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vPos));
	m_pTransformCom->Scaled(_float3(1.f, 1.f, 1.f));
	m_pTransformCom->LookAt(XMVectorSet(0.f, 0.f, 0.f, 0.f));
}

_bool CLensFlare::_IsInViewport()
{
	_float fBorderX = 1.5f;
	_float fBorderY = 1.5f;

	auto vLightScreenPos = _Transform_Light();

	if (-fBorderX > vLightScreenPos.x || fBorderX < vLightScreenPos.x)
		return false;

	else if (-fBorderY > vLightScreenPos.y || fBorderY < vLightScreenPos.y)
		return false;
	
	_Set_Corona(vLightScreenPos);
	_Set_Flares(vLightScreenPos);

	return true;
}


Vector2 CLensFlare::_Transform_Light()
{
	auto pGameInstance = CGameInstance::Get_Instance();

	Vector4 vLightPos = m_pLight->vPosition;
	Vector4 vLightDir = m_pLight->vDirection;
	auto vCameraDir = pGameInstance->Get_CamForward();
	
	auto vPos = vLightPos.operator DirectX::XMVECTOR();
	auto vDir = XMVector3Normalize(vLightDir.operator DirectX::XMVECTOR());
	auto vCamDir = XMVector3Normalize(XMLoadFloat3(&vCameraDir));

	auto fDot = XMVectorGetX(XMVector3Dot(vDir, vCamDir));

	if (fDot >= 0.3f)
		return Vector2(-10.f, -10.f);

	auto matView = pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW);
	auto matProj = pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ);

	vPos = XMVector3TransformCoord(vPos, matView * matProj);
	vPos /= XMVectorGetW(vPos);

	return Vector2(XMVectorGetX(vPos), XMVectorGetY(vPos));
}


HRESULT CLensFlare::_Add_Components(void* _pArg)
{
	auto pGameInstance = CGameInstance::Get_Instance();
	auto eCurrentLevel = static_cast<_uint>(pGameInstance->Get_CurLevel());

	if (FAILED(__super::Add_Component(LEVEL_STATIC, PC_RENDERER, TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, PC_TRANSFORM, TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(eCurrentLevel, PC_VIBUFFER_LENSFLARE, TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(eCurrentLevel, PC_SHADER_LENSFLARE, TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(eCurrentLevel, PC_TEXTURE_LENSFLARE, TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC colliderDesc;
	ZeroMemory(&colliderDesc, sizeof(CCollider::COLLIDERDESC));

	colliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	colliderDesc.fRadius = 5.f;
	colliderDesc.vRotation = _float3(0.f, 0.f, 0.f);

	m_pColliderCom = static_cast<CCollider*>(pGameInstance->Add_ColliderComponent(eCurrentLevel, PC_COLLIDER_SPHERE, this, COLLISION_GROUP::PLAYER_BODY, &colliderDesc));

	m_mapColliders.emplace(TEXT("Com_Collider"), m_pColliderCom);

	return S_OK;
}

_float CLensFlare::_Calculate_CdotL()
{
	auto pGameInstnace = CGameInstance::Get_Instance();

	auto vCameraDirection = pGameInstnace->Get_CamForward();
	auto vLightPosition = m_pLight->vPosition;

	vLightPosition.w = 0.f;

	auto vCamDir = XMVector3Normalize(XMLoadFloat3(&vCameraDirection));
	auto vLightDir = XMVector3Normalize(XMLoadFloat4(&vLightPosition));

	_float fCdotL = XMVectorGetX(XMVector3Dot(vCamDir, vLightDir));

	fCdotL = clamp(fCdotL, 0.f, 1.f);
	fCdotL = CMath::Lerp(m_fBeforeCdotL, fCdotL, m_fProgressTime);

	m_fBeforeCdotL = fCdotL;

	return fCdotL;
}

void CLensFlare::_Set_Corona(const Vector2& _vPos)
{
	Vector4 vOrigin = Vector4(0.f, 0.f, 1.f, 1.f);
	Vector4 vPosition = Vector4(_vPos.x, _vPos.y, 1.f, 1.f);
	Vector4 vDirection = vOrigin - vPosition;
	_float fAspect = CGlobal::g_fWinSizeX / CGlobal::g_fWinSizeY;
	constexpr _float fBaseRadians = XMConvertToRadians(45.f);

	vDirection.Normalize();

	const auto fRadians = fmodf(atan2f(-vDirection.y, vDirection.x) * 2.f + PI_TWO, PI_TWO) + fBaseRadians;

	m_tCorona.vPosition = vPosition;
	m_tCorona.vScaleRotate = _float4(0.7f, fRadians, fAspect, 0.f);
	m_tCorona.vColor = _float4(0.8f, 0.8f, 0.6f, 1.f);
	m_tCorona.fBrightness = clamp(1.1f - _vPos.Length() * 0.6f, 0.f, 1.f);
}

void CLensFlare::_Set_Flares(const Vector2& _vPos)
{
	Vector4 vOrigin = Vector4(0.f, 0.f, 0.f, 1.f);
	Vector4 vPosition = Vector4(_vPos.x, _vPos.y, 0.f, 1.f);
	Vector4 vDirection = vOrigin - vPosition;
	_float fLength = vDirection.Length();
	_float fOffset[11] = { 0.f, 0.2f, 0.4f, 0.6f, 0.8f, 1.f, 1.2f, 1.4f, 1.6f, 1.8f, 2.f };
	_float fSize[11] = { 0.f, 0.23f, 0.1f, 0.05f, 0.06f, 0.07f, 0.02f, 0.07f, 0.3f, 0.4f, 0.6f };
	_float fAspect = CGlobal::g_fWinSizeX / CGlobal::g_fWinSizeY;

	constexpr _float fBaseRadians = XMConvertToRadians(45.f);

	vDirection.Normalize();

	const auto fRadians = fmodf(atan2f(-vDirection.y, vDirection.x) + PI_TWO, PI_TWO) + fBaseRadians;

	for (_uint i = 0; i < 11; ++i)
	{
		m_tFlares[i].vOffset = vPosition + (vDirection * fLength * fOffset[i]);
		m_tFlares[i].vScaleRotate = _float4(fSize[i], fRadians, fAspect, 0.f);
	}
}

CLensFlare* CLensFlare::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	auto pInstance = new CLensFlare(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created: CLensFlare::Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLensFlare::Clone(void* _pArg)
{
	auto pInstance = new CLensFlare(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned: CLensFlare::Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLensFlare::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);

	Safe_Release(m_pPredicate);
	Safe_Release(m_pOcclusionQuery);

	m_tFlares.clear();
}