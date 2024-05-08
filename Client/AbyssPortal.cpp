#include "Client_pch.h"
#include "Client_Defines.h"
#include "AbyssPortal.h"

// Singleton
#include "GameInstance.h"
#include "Render_Manager.h"
#include "Quest_Manager.h"

// GameObject
#include "Player.h"

#include "Engine_Functions.h"
#include "Client_Functions.h"

CAbyssPortal::CAbyssPortal(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject(_pDevice, _pContext)
{
}

CAbyssPortal::CAbyssPortal(const CAbyssPortal& _rhs)
	: CGameObject(_rhs)
{
}

HRESULT CAbyssPortal::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAbyssPortal::Initialize(void* _pArg)
{
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	if (FAILED(_Add_Components(_pArg)))
		return E_FAIL;

	m_vColor = _float4(1.f, 0.6f, 0.6f, 1.f);

	m_funcCollisionBegin = [this]()
	{
		CRender_Manager::Get_Instance()->Begin_Fade(CRender_Manager::FADE_OUT, 0.5f);

		auto func = [](_float)
		{
			auto pPlayer = CFunc::Get_Player();
			auto pTransformCom = static_cast<CTransform*>(pPlayer->Find_Component(TEXT("Com_Transform")));
			auto pNavCom = static_cast<CNavigation*>(pPlayer->Find_Component(TEXT("Com_Navigation")));
			auto vPosition = XMVectorSet(313.154f, 0.f, 422.753f, 1.f);

			constexpr _int iStartIndex = 512 * 512 * 2;

			_int iFindNewCell = pNavCom->Find_Cell_Specific_Range(vPosition, iStartIndex);
			_int iGroundCell = pNavCom->Find_Cell_By_Distribution_Position(vPosition);

			if (-1 == iGroundCell && 100 > pNavCom->Get_Cell_Count())
				iGroundCell = pNavCom->Get_Index(vPosition);

			if (-1 != iFindNewCell && -1 != iGroundCell)
			{
				_float fNewYDistance = 0.f;
				_float fGroundYDistance = 0.f;

				pNavCom->Get_YDistance(vPosition, iFindNewCell, fNewYDistance);
				pNavCom->Get_YDistance(vPosition, iGroundCell, fGroundYDistance);

				if (fGroundYDistance >= fNewYDistance)
					pNavCom->Set_CurrentIndex(iFindNewCell);
				else
					pNavCom->Set_CurrentIndex(iGroundCell);

				pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition, pNavCom);

			}
			else if (-1 == iFindNewCell && -1 != iGroundCell)
			{
				pNavCom->Set_CurrentIndex(iGroundCell);
				pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition, pNavCom);
			}
			else if (-1 != iFindNewCell && -1 == iGroundCell)
			{
				pNavCom->Set_CurrentIndex(iFindNewCell);
				pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition, pNavCom);
			}

			CRender_Manager::Get_Instance()->Begin_Fade(CRender_Manager::FADE_IN, 0.5f);
		};

		CFunc::Add_Event_Inteval_Function(1.f, 0.f, 1, func, this, false);
	};

	CQuest_Manager::Get_Instance()->Register_Observer(this);

	return S_OK;
}

void CAbyssPortal::Tick(_float _fTimeDelta)
{
	__super::Tick(_fTimeDelta);

	m_fAccumulatedTime += _fTimeDelta * 0.01f;

	if (m_fAccumulatedTime > 1.f)
		m_fAccumulatedTime -= 1.f;

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
}

void CAbyssPortal::Late_Tick(_float _fTimeDelta)
{
	__super::Late_Tick(_fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_VFX, this);
}

HRESULT CAbyssPortal::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(_Bind_ShaderResources()))
		return E_FAIL;

	m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_DiffuseTexture", 0);
	m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_MaskMapTexture", 1);
	m_pShaderCom->Bind("g_fTimeDelta", m_fAccumulatedTime);
	m_pShaderCom->Bind("g_fScale", 0.3f);
	m_pShaderCom->Bind("g_fBrightness", 0.5f);
	m_pShaderCom->Bind("g_vColor", m_vColor);

	m_pShaderCom->Begin(0);
	m_pVIBufferCom->Render();

#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pColliderCom);
#endif

	return S_OK;
}

void CAbyssPortal::OnCollisionBegin(CCollider* _pSelf, CCollider* _pOther, _float _fTimeDelta)
{
	m_funcCollisionBegin();
}

void CAbyssPortal::Take_Notify(OBSERVER_NOTIFY_TYPE _eType, any _arg)
{
	if (OBSERVER_NOTIFY_TYPE::QUEST_BEGIN == _eType)
	{
		auto iBeginQuestID = any_cast<_uint>(_arg);

		if (14 == iBeginQuestID) // 심연 조사하기
		{
			m_funcCollisionBegin = [this]()
			{
				CRender_Manager::Get_Instance()->Begin_Fade(CRender_Manager::FADE_OUT, 0.5f);

				auto func = [this](_float)
				{
					CFunc::Add_Event_ChangeLevel(LEVEL_DESERT, LEVEL_ABYSS);
				};

				CFunc::Add_Event_Inteval_Function(1.f, 0.f, 1, func, this, false);
			};

			CFunc::Add_QuestDisplay(this, 0.f);
		}
	}
}

HRESULT CAbyssPortal::_Add_Components(void* _pArg)
{
	m_eLevel = CFunc::Get_CurLevel();

	if (FAILED(__super::Add_Component(LEVEL_STATIC, PC_TRANSFORM, TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, PC_RENDERER, TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_eLevel, PC_SHADER_ABYSSPORTAL, TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, PC_VIBUFFER_RECT, TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(m_eLevel, PC_TEXTURE_ABYSSPORTAL, TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	auto S = XMMatrixScaling(200.f, 200.f, 1.f);
	auto R = XMMatrixRotationX(XMConvertToRadians(90.f));
	auto T = XMMatrixTranslation(0.f, -30.f, 0.f);

	_float4x4 matWorld;
	XMStoreFloat4x4(&matWorld, S * R * T);
	
	m_pTransformCom->Set_WorldMatrix(matWorld);

	CCollider::COLLIDERDESC tColliderDesc;
	ZeroMemory(&tColliderDesc, sizeof(CCollider::COLLIDERDESC));

	tColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	tColliderDesc.vSize = _float3(1.f, 1.f, 15.f);

	m_pColliderCom = static_cast<CCollider*>(CGameInstance::Get_Instance()->Add_ColliderComponent(m_eLevel, PC_COLLIDER_OBB, this, COLLISION_GROUP::TRIGGER, &tColliderDesc));
	Register_ColliderCom(TEXT("Collider"), m_pColliderCom);

	m_pColliderCom->Set_Enable(true);

	return S_OK;
}

HRESULT CAbyssPortal::_Bind_ShaderResources()
{
	auto pGameInstance = CGameInstance::Get_Instance();

	auto matView = pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
	auto matProj = pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ);

	m_pShaderCom->Bind("g_ViewMatrix", matView);
	m_pShaderCom->Bind("g_ProjMatrix", matProj);
	m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix");

	auto fCameraFar = pGameInstance->Get_CameraFar();
	auto vCameraPosition = pGameInstance->Get_CamPosition();

	m_pShaderCom->Bind("g_fCameraFar", fCameraFar);
	m_pShaderCom->Bind("g_vCamPosition", vCameraPosition);

	return S_OK;
}

CAbyssPortal* CAbyssPortal::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	auto pInstance = new CAbyssPortal(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created: CAbyssPortal::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CAbyssPortal::Clone(void* _pArg)
{
	auto pInstance = new CAbyssPortal(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned: CAbyssPortal::Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAbyssPortal::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);

	CQuest_Manager::Get_Instance()->Remove_Observer(this);
}
