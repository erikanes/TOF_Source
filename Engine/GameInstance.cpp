#include "Engine_pch.h"
#include "GameInstance.h"

#include "Picking_Manager.h"
#include "Object_Manager.h"
#include "Graphic_Device.h"
#include "Event_Manager.h"
#include "Level_Manager.h"
#include "Timer_Manager.h"
#include "Sound_Manager.h"
#include "Light_Manager.h"
#include "Key_Manager.h"
#include "Collision_Manager.h"
#include "Frustum.h"
#include "RenderTarget_Manager.h"
#include "Effect_Manager.h"
#include "Font_Manager.h"
#include "Render_Manager.h"
#include "Json_Manager.h"
#include "UI_Manager.h"
#include "GameMode.h"
#include "Level.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
	: m_pGraphic_Device{ CGraphic_Device::Get_Instance() }
	, m_pLevel_Manager{ CLevel_Manager::Get_Instance() }
	, m_pObject_Manager{ CObject_Manager::Get_Instance() }
	, m_pComponent_Manager{ CComponent_Manager::Get_Instance() }
	, m_pCollision_Manager{ CCollision_Manager::Get_Instance() }
	, m_pPicking_Manager{ CPicking_Manager::Get_Instance() }
	, m_pTimer_Manager{ CTimer_Manager::Get_Instance() }
	, m_pKey_Manager{ CKey_Manager::Get_Instance() }
	, m_pSound_Manager{ CSound_Manager::Get_Instance() }
	, m_pEvent_Manager{ CEvent_Manager::Get_Instance() }
	, m_pLight_Manager{ CLight_Manager::Get_Instance() }
	, m_pPipeLine{ CPipeLine::Get_Instance() }
	, m_pFrustum{ CFrustum::Get_Instance() }
	, m_pRenderTarget_Manager{ CRenderTarget_Manager::Get_Instance() }
	, m_pEffect_Manager{ CEffect_Manager::Get_Instance() }
	, m_pFont_Manager{ CFont_Manager::Get_Instance() }
	, m_pUI_Manager{ CUI_Manager::Get_Instance() }
	, m_pGameMode { CGameMode::Get_Instance() }
{
	Safe_AddRef(m_pGraphic_Device);
	Safe_AddRef(m_pSound_Manager);
	Safe_AddRef(m_pEvent_Manager);
	Safe_AddRef(m_pTimer_Manager);
	Safe_AddRef(m_pComponent_Manager);
	Safe_AddRef(m_pPicking_Manager);
	Safe_AddRef(m_pObject_Manager);
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pLevel_Manager);
	Safe_AddRef(m_pPipeLine);
	Safe_AddRef(m_pKey_Manager);
	Safe_AddRef(m_pFrustum);
	Safe_AddRef(m_pCollision_Manager);
	Safe_AddRef(m_pRenderTarget_Manager);
	Safe_AddRef(m_pEffect_Manager);
	Safe_AddRef(m_pFont_Manager);
	Safe_AddRef(m_pUI_Manager);
	Safe_AddRef(m_pGameMode);
}

HRESULT CGameInstance::Initialize_Engine(HINSTANCE _hInstance, _uint iNumCollisionGroup,
	_uint iNumLevels, const GRAPHICDESC& GraphicDesc,
	_Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext)
{
	if (FAILED(m_pGraphic_Device->Ready_Graphic_Device(GraphicDesc.hWnd, GraphicDesc.eWinMode, GraphicDesc.iSizeX, GraphicDesc.iSizeY, ppDevice, ppContext)))
		return E_FAIL;

	if (FAILED(m_pFrustum->Initialize()))
		return E_FAIL;

	if (FAILED(m_pSound_Manager->Initialize()))
		return E_FAIL;

	if (FAILED(m_pPicking_Manager->Initialize(GraphicDesc.hWnd)))
		return E_FAIL;

	if (FAILED(m_pKey_Manager->Ready_DInput(_hInstance, GraphicDesc.hWnd)))
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Reserve_Manager(iNumLevels)))
		return E_FAIL;
	
	if (FAILED(m_pUI_Manager->Reserve_Manager_UI(iNumLevels)))
		return E_FAIL;

	if (FAILED(m_pComponent_Manager->Reserve_Manager(iNumLevels)))
		return E_FAIL;

	if (FAILED(m_pCollision_Manager->Reserve_Collider_Group(iNumCollisionGroup)))
		return E_FAIL;

	if (FAILED(m_pEvent_Manager->Initialize(*ppDevice, *ppContext)))
		return E_FAIL;

	return S_OK;
}

void CGameInstance::Tick_Engine(_float fTimeDelta)
{
	if (nullptr == m_pLevel_Manager ||
		nullptr == m_pObject_Manager)
		return;

	m_pKey_Manager->Tick();

	// Level의 삭제가 이루어지는 동안에는 ObjectManager, UIManager, CollisionManager의 틱이 돌면 안된다
	if (!m_pLevel_Manager->Is_CleaningUp())
	{
		m_pObject_Manager->Tick(fTimeDelta);
		m_pUI_Manager->Tick(fTimeDelta);
	}

	m_pLevel_Manager->Tick(fTimeDelta);

	if (!m_pLevel_Manager->Is_CleaningUp())
	{
		m_pCollision_Manager->Tick(fTimeDelta);
		m_pGameMode->Tick(fTimeDelta);
	}

	m_pPipeLine->Tick();
	m_pFrustum->Tick();
	m_pPicking_Manager->Tick();
	
	m_pEvent_Manager->Tick(fTimeDelta);

	if (!m_pLevel_Manager->Is_CleaningUp())
	{
		m_pObject_Manager->Late_Tick(fTimeDelta);
		m_pUI_Manager->Late_Tick(fTimeDelta);
		
		m_pGameMode->Late_Tick(fTimeDelta);

		m_pLight_Manager->Late_Tick(fTimeDelta);
	}

	m_pSound_Manager->Update();

	m_pEvent_Manager->Late_Tick(fTimeDelta);
	m_pLevel_Manager->Late_Tick(fTimeDelta);
}

void CGameInstance::Clear_All()
{
	if (nullptr == m_pObject_Manager)
		return;

	m_pObject_Manager->Clear_All();
}


void CGameInstance::Clear(_uint iLevelIndex)
{
	if (nullptr == m_pObject_Manager)
		return;

	m_pObject_Manager->Clear(iLevelIndex);
}

void CGameInstance::Clear_All_UI()
{
	if (nullptr == m_pUI_Manager)
		return;

	m_pUI_Manager->Clear_All_UI();
}

void CGameInstance::Clear_UI(_uint iLevelIndex)
{
	if (nullptr == m_pUI_Manager)
		return;

	m_pUI_Manager->Clear_UI(iLevelIndex);
}

void CGameInstance::Clear_Level(_uint _iLevelIndex)
{
	m_pObject_Manager->Clear(_iLevelIndex);
	m_pComponent_Manager->Clear_SpecificLevel_Prototype(_iLevelIndex); 
}

/*
HRESULT	CGameInstance::RegisterNoDeleteLevel(_int _eLevel)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	m_pComponent_Manager->Register_Reserve_Level(_eLevel);

	return S_OK;
}
*/

ID3D11ShaderResourceView* CGameInstance::Get_Texture_SRV(_uint iLevelIndex, const wstring& strPrototypeTag, _uint _uIdx)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;

	return m_pComponent_Manager->Get_Texture_SRV(iLevelIndex, strPrototypeTag, _uIdx);
}

HRESULT CGameInstance::Clear_PrototpyeComponent(_int _eLevel)
{
	return m_pComponent_Manager->Clear_SpecificLevel_Prototype(_eLevel);
}

HRESULT CGameInstance::Clear_BackBuffer_View(_float4 vClearColor)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);
}

HRESULT CGameInstance::Clear_DepthStencil_View()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Clear_DepthStencil_View();
}

HRESULT CGameInstance::Present()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Present();
}

/*
HRESULT CGameInstance::Set_NormalRenderState()
{
	D3D11_RASTERIZER_DESC tRSDesc;
	ZeroMemory(&tRSDesc, sizeof(tRSDesc));
	tRSDesc.FillMode = D3D11_FILL_SOLID;
	tRSDesc.CullMode = D3D11_CULL_BACK;

	m_pGraphic_Device->Change_RSState(tRSDesc);
	return S_OK;
}

HRESULT CGameInstance::Set_WireframeRenderState()
{
	D3D11_RASTERIZER_DESC tRSDesc;
	ZeroMemory(&tRSDesc, sizeof(tRSDesc));
	tRSDesc.FillMode = D3D11_FILL_WIREFRAME;
	tRSDesc.CullMode = D3D11_CULL_BACK;

	m_pGraphic_Device->Change_RSState(tRSDesc);
	return S_OK;
}
*/

CRenderTarget* CGameInstance::Find_RenderTarget(const wstring& _wstrTag)
{
	return m_pRenderTarget_Manager->Find_RenderTarget(_wstrTag);
}

HRESULT CGameInstance::Bind_ShaderResource_RTV(CShader* pShader, const wstring& strTargetTag, const char* pConstantName)
{
	if (nullptr == m_pRenderTarget_Manager)
		return E_FAIL;

	return m_pRenderTarget_Manager->Bind_ShaderResource(pShader, strTargetTag, pConstantName);
}

Level   CGameInstance::Get_CurLevel(_bool _bIsLoading)
{
	return m_pLevel_Manager->Get_CurLevel(_bIsLoading);
}

HRESULT CGameInstance::Open_Level(_uint iLevelIndex, CLevel* pLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Open_Level(iLevelIndex, pLevel);
}

/*
void* CGameInstance::Get_SerializedData()
{
	if (nullptr == m_pLevel_Manager)
		return nullptr;

	return m_pLevel_Manager->Get_SerializedData();
}

function<void(any)> CGameInstance::Get_Usual_Function_In_Level(MARKED_FUNCTION_TAG _eTag)
{
	return m_pLevel_Manager->Get_Usual_Function(_eTag);
}

CComponent* CGameInstance::Get_Component(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strComTag, _uint iIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_Component(iLevelIndex, strLayerTag, strComTag, iIndex);
}

HRESULT CGameInstance::Add_Prototype(const wstring& strPrototypeTag, CGameObject* pPrototype)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Prototype(strPrototypeTag, pPrototype);
}

HRESULT CGameInstance::Add_GameObject(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_GameObject(iLevelIndex, strLayerTag, strPrototypeTag, pArg);
}

HRESULT CGameInstance::Add_GameObject(_uint _iLevelIndex, const wstring& _strLayerTag, CGameObject* _pGameObject)
{
	return m_pObject_Manager->Add_GameObject(_iLevelIndex, _strLayerTag, _pGameObject);
}

HRESULT	CGameInstance::Set_Dead_Specific_Game_Objects(_uint _iLevelIndex, const wstring& _strLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Set_Dead_Object_Specific_Layer(_iLevelIndex, _strLayerTag);
}

HRESULT CGameInstance::Create_GameObject(const wstring& _strPrototypeTag, CGameObject** _ppOut, void* _pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Create_GameObject(_strPrototypeTag, _ppOut, _pArg);
}
*/

HRESULT CGameInstance::Create_ObjectPool(const wstring& _strPoolTag, _uint _iSize, CGameObject* _pPrototype, void* _pArg, const wstring& _wstrLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Create_ObjectPool(_strPoolTag, _iSize, _pPrototype, _pArg, _wstrLayerTag);
}

CGameObject* CGameInstance::Pop_FromObjectPool(const wstring& _strPoolTag, any _args)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Pop_FromObjectPool(_strPoolTag, _args);
}

CGameObject* CGameInstance::Get_FromObjectPool(const wstring& _strPoolTag, any _args)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_FromObjectPool(_strPoolTag, _args);
}

/*
CGameObject* CGameInstance::Get_Prototype(const wstring& strPrototypeTag)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Find_Prototype(strPrototypeTag);
}

HRESULT CGameInstance::Set_RegisterUpdateLevel(_uint _uLevel, _bool _bRegistered)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Set_UpdateLevel(_uLevel, _bRegistered);
}

map<wstring, CLayer*>* CGameInstance::Get_SpecificLayer(_uint _uLevel)
{
	return m_pObject_Manager->Get_SpecificLayer(_uLevel);
}

CComponent* CGameInstance::Get_Component_UI(_uint _iLevelIndex, const wstring& _strLayerTag, const wstring& _strComTag, _uint _iIndex)
{
	if (nullptr == m_pUI_Manager)
		return nullptr;

	return m_pUI_Manager->Get_Component_UI(_iLevelIndex, _strLayerTag, _strComTag, _iIndex);
}

HRESULT CGameInstance::Add_Prototype_UI(const wstring& _strPrototypeTag, CGameObject* _pPrototype)
{
	if (nullptr == m_pUI_Manager)
		return E_FAIL;

	return m_pUI_Manager->Add_Prototype_UI(_strPrototypeTag, _pPrototype);
}

HRESULT CGameInstance::Add_GameObject_UI(_uint _iLevelIndex, const wstring& _strLayerTag, const wstring& _strPrototypeTag, void* _pArg)
{
	if (nullptr == m_pUI_Manager)
		return E_FAIL;

	return m_pUI_Manager->Add_GameObject_UI(_iLevelIndex, _strLayerTag, _strPrototypeTag, _pArg);
}

HRESULT CGameInstance::Add_GameObject_UI(_uint _iLevelIndex, const wstring& _strLayerTag, CGameObject* _pGameObject)
{
	if (nullptr == m_pUI_Manager)
		return E_FAIL;

	return m_pUI_Manager->Add_GameObject_UI(_iLevelIndex, _strLayerTag, _pGameObject);
}

HRESULT CGameInstance::Create_GameObject_UI(const wstring& _strPrototypeTag, CGameObject** _ppOut, void* _pArg)
{
	if (nullptr == m_pUI_Manager)
		return E_FAIL;

	return m_pUI_Manager->Create_GameObject_UI(_strPrototypeTag, _ppOut, _pArg);
}

HRESULT CGameInstance::Set_UpdateLevel_UI(_uint _uLevel, _bool _bRegistered)
{
	if (nullptr == m_pUI_Manager)
		return E_FAIL;

	return m_pUI_Manager->Set_UpdateLevel_UI(_uLevel, _bRegistered);
}

map<wstring, class CLayer*>* CGameInstance::Get_SpecificLayer_UI(_uint _uLevel)
{
	if (nullptr == m_pUI_Manager)
		return nullptr;

	return m_pUI_Manager->Get_SpecificLayer_UI(_uLevel);
}

HRESULT CGameInstance::Enable_UI(_uint _iNumLevels, const wstring& _strLayerTag)
{
	if (nullptr == m_pUI_Manager)
		return E_FAIL;

	return m_pUI_Manager->Enable_UI(_iNumLevels, _strLayerTag);
}
*/

HRESULT CGameInstance::Enable_UI(_uint _iNumLevels, const wstring& _wstrLayerTag, any _msgDesc)
{
	if (nullptr == m_pUI_Manager)
		return E_FAIL;

	return m_pUI_Manager->Enable_UI(_iNumLevels, _wstrLayerTag, _msgDesc);
}

/*
HRESULT CGameInstance::Enable_UI_Specific(_uint _iNumLevels, const wstring& _wstrLayerTag, const wstring& _wstrName, any msgDesc)
{
	if (nullptr == m_pUI_Manager)
		return E_FAIL;

	return m_pUI_Manager->Enable_UI_Specific(_iNumLevels, _wstrLayerTag, _wstrName, msgDesc);
}
*/

HRESULT CGameInstance::Disable_UI(_uint _iNumLevels, const wstring& _wstrLayerTag, any _msgDesc)
{
	if (nullptr == m_pUI_Manager)
		return E_FAIL;

	return m_pUI_Manager->Disable_UI(_iNumLevels, _wstrLayerTag, _msgDesc);
}

/*
HRESULT CGameInstance::Disable_UI_Specific(_uint _iNumLevels, const wstring& _wstrLayerTag, const wstring& _wstrName, any _msgDesc)
{
	if (nullptr == m_pUI_Manager)
		return E_FAIL;

	return m_pUI_Manager->Disable_UI_Specific(_iNumLevels, _wstrLayerTag, _wstrName, _msgDesc);
}

HRESULT CGameInstance::MouseEnable_UI(_uint _iNumLevels, const wstring& _wstrLayerTag)
{
	if (nullptr == m_pUI_Manager)
		return E_FAIL;

	return m_pUI_Manager->MouseEnable_UI(_iNumLevels, _wstrLayerTag);
}

HRESULT CGameInstance::MouseEnable_UI_Specific(_uint _iNumLevels, const wstring& _wstrLayerTag, const wstring& _wstrName)
{
	if (nullptr == m_pUI_Manager)
		return E_FAIL;

	return m_pUI_Manager->MouseDisable_UI_Specific(_iNumLevels, _wstrLayerTag, _wstrName);
}

HRESULT CGameInstance::MouseDisable_UI(_uint _iNumLevels, const wstring& _wstrLayerTag)
{
	if (nullptr == m_pUI_Manager)
		return E_FAIL;

	return m_pUI_Manager->MouseDisable_UI(_iNumLevels, _wstrLayerTag);
}

HRESULT CGameInstance::MouseDisable_UI_Specific(_uint _iNumLevels, const wstring& _wstrLayerTag, const wstring& _wstrName)
{
	if (nullptr == m_pUI_Manager)
		return E_FAIL;

	return m_pUI_Manager->MouseDisable_UI_Specific(_iNumLevels, _wstrLayerTag, _wstrName);
}

HRESULT CGameInstance::Disable_UI(_uint _iNumLevels, const wstring& _strLayerTag)
{
	if (nullptr == m_pUI_Manager)
		return E_FAIL;

	return m_pUI_Manager->Disable_UI(_iNumLevels, _strLayerTag);
}

HRESULT CGameInstance::Enable_UI_InteractionInfo(_uint _iNumLevels, const wstring& _wstrLayerTag, const wstring& _wstrInteractionObjectName, _uint _eInteractionType)
{
	if (nullptr == m_pUI_Manager)
		return E_FAIL;

	return m_pUI_Manager->Enable_UI_InteractionInfo(_iNumLevels, _wstrLayerTag, _wstrInteractionObjectName, _eInteractionType);
}

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag, CComponent* pPrototype)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Add_Prototype(iLevelIndex, strPrototypeTag, pPrototype);
}

CComponent* CGameInstance::Add_ColliderComponent(_uint iLevelIndex, const wstring& strPrototypeTag,
	class CGameObject* _pOwner, _int eCollisionGroup, void* _pArg)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;

	CCollider* pCollider = static_cast<CCollider*>(m_pComponent_Manager->Clone_Component(iLevelIndex, strPrototypeTag, _pArg));

	pCollider->Set_CollisionGroup(eCollisionGroup);
	pCollider->Set_Owner(_pOwner);
	m_pCollision_Manager->Add_Collider(eCollisionGroup, pCollider);

	pCollider->Set_Render(true);

	return pCollider;
}

HRESULT CGameInstance::Register_Collider(_int _eCollisionGroup, CCollider* _pCollider)
{
	m_pCollision_Manager->Add_Collider(_eCollisionGroup, _pCollider);

	return S_OK;
}

HRESULT CGameInstance::Register_Collision_Group(_uint _eLeftGroupType, _uint _eRightGroupType)
{
	m_pCollision_Manager->Register_Collision_Group(_eLeftGroupType, _eRightGroupType);


	return S_OK;
}

HRESULT CGameInstance::ClearCollisionGroup()
{
	if (nullptr == m_pCollision_Manager)
		return E_FAIL;

	m_pCollision_Manager->ClearCollisionGroup();

	return S_OK;
}

HRESULT CGameInstance::ResetCollisionGroup()
{
	m_pCollision_Manager->ResetCollisionGroup();

	return S_OK;
}

#ifdef _DEBUG
void	CGameInstance::Set_Collider_Render(_bool _bRender)
{
	m_pCollision_Manager->Set_Render_Collider(_bRender);
}

_int CGameInstance::Get_Collider_Count()
{
	return m_pCollision_Manager->Get_Collider_Count();
}

_int CGameInstance::Get_Collider_Count_For_Specific_Collision_Group(_int _eGroupType)
{
	return m_pCollision_Manager->Get_Collider_Count_For_Specificy_Collision_Group(_eGroupType);
}
#endif 

CComponent* CGameInstance::Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, void* pArg)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;

	return m_pComponent_Manager->Clone_Component(iLevelIndex, strPrototypeTag, pArg);
}

HRESULT CGameInstance::Add_Timer(const wstring& strTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Add_Timer(strTimerTag);
}

_float CGameInstance::Compute_TimeDelta(const wstring& strTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return 0.f;

	return m_pTimer_Manager->Compute_TimeDelta(strTimerTag);
}

void CGameInstance::Set_TimeScale(const wstring& _strTimerTag, _float _fTimeScale)
{
	m_pTimer_Manager->Set_TimeScale(_strTimerTag, _fTimeScale);
}

_matrix CGameInstance::Get_Transform_Matrix(CPipeLine::TRANSFORMSTATE _eState)
{
	return m_pPipeLine->Get_Transform_Matrix(_eState);
}

_float4x4 CGameInstance::Get_Transform_Float4x4(CPipeLine::TRANSFORMSTATE _eState)
{
	return m_pPipeLine->Get_Transform_Float4x4(_eState);
}

_matrix CGameInstance::Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORMSTATE _eState)
{
	return m_pPipeLine->Get_Transform_Matrix_Inverse(_eState);
}

_float4x4 CGameInstance::Get_Transform_Float4x4_Inverse(CPipeLine::TRANSFORMSTATE _eState)
{
	return m_pPipeLine->Get_Transform_Float4x4_Inverse(_eState);
}

_float4 CGameInstance::Get_CamPosition()
{
	return m_pPipeLine->Get_CamPosition();
}

_float3 CGameInstance::Get_CamForward()
{
	return m_pPipeLine->Get_CamForward();
}

_float CGameInstance::Get_CameraNear() const
{
	return m_pPipeLine->Get_CameraNear();
}

_float CGameInstance::Get_CameraFar() const
{
	return m_pPipeLine->Get_CameraFar();
}

_float CGameInstance::Get_CameraFov() const
{
	return m_pPipeLine->Get_CameraFov();
}

KEY_STATE CGameInstance::Get_KeyState(_ubyte _ubyKeyID)
{
	return m_pKey_Manager->Get_KeyState(_ubyKeyID);
}

KEY_STATE CGameInstance::Get_MouseState(MOUSEKEYSTATE _eMouseID)
{
	return m_pKey_Manager->Get_MouseState(_eMouseID);
}

_long CGameInstance::Get_MouseMoveState(MOUSEMOVESTATE _eMouseMove)
{
	return m_pKey_Manager->Get_DIMouseMove(_eMouseMove);
}

_float2	CGameInstance::Get_MouseCurPos()
{
	return m_pKey_Manager->Get_MousePos();
}

_bool CGameInstance::Is_MouseInRestrictedArea()
{
	return m_pKey_Manager->Is_InRestrictedArea();
}

void CGameInstance::Set_MouseRestrictedArea(RECT _rt)
{
	m_pKey_Manager->Set_RestrictedRect(_rt);
}
*/

#pragma region Sound_Manager
HRESULT CGameInstance::Add_AudioClip(const string& _strFilePath, const string& _strTag, AUDIO_CLIP_TYPE _eType, _float _fVolume, _bool _bIsLoop)
{
	return m_pSound_Manager->Add_AudioClip(_strFilePath, _strTag, _eType, _fVolume, _bIsLoop);
}

HRESULT CGameInstance::Load_AudioClips(const string& _strFilePath)
{
	return m_pSound_Manager->Load_AudioClips(_strFilePath);
}

_int CGameInstance::Play_Audio(const string& _strTag, _float _fVolume, _bool _bUseFadeIn)
{
	return m_pSound_Manager->Play_Audio(_strTag, _fVolume, _bUseFadeIn);
}

void CGameInstance::Stop_Audio(const string& _strTag, _bool _bUseFadeOut)
{
	m_pSound_Manager->Stop_Audio(_strTag, _bUseFadeOut);
}

_int CGameInstance::Change_BGM(const string& _strTag, _float _fVolume, _bool _bUseFade)
{
	return m_pSound_Manager->Change_BGM(_strTag, _fVolume, _bUseFade);
}

_int CGameInstance::Stop_BGM(_bool _bUseFadeOut)
{
	return m_pSound_Manager->Stop_BGM(_bUseFadeOut);
}

void CGameInstance::FadeOut_BGM_Volume(_float _fVolume)
{
	m_pSound_Manager->FadeOut_BGM_Volume(_fVolume);
}

void CGameInstance::FadeIn_BGM_Volume(_float _fVolume)
{
	m_pSound_Manager->FadeIn_BGM_Volume(_fVolume);
}

void CGameInstance::Stop_Channel(size_t _iChannelIndex, _bool _bUseFadeOut)
{
	m_pSound_Manager->Stop_Channel(_iChannelIndex, _bUseFadeOut);
}

void CGameInstance::Stop_AllChannels()
{
	m_pSound_Manager->Stop_AllChannels();
}

void CGameInstance::Mute_Channel(size_t _iChannelIndex)
{
	m_pSound_Manager->Mute_Channel(_iChannelIndex);
}

void CGameInstance::Mute_AllChannels()
{
	m_pSound_Manager->Mute_AllChannels();
}

void CGameInstance::UnMute_Channel(size_t _iChannelIndex)
{
	m_pSound_Manager->UnMute_Channel(_iChannelIndex);
}

void CGameInstance::UnMute_AllChannels()
{
	m_pSound_Manager->UnMute_AllChannels();
}

void CGameInstance::Pause_Channel(size_t _iChannelIndex)
{
	m_pSound_Manager->Pause_Channel(_iChannelIndex);
}

void CGameInstance::Pause_AllChannels()
{
	m_pSound_Manager->Pause_AllChannels();
}

void CGameInstance::Resume_Channel(size_t _iChannelIndex)
{
	m_pSound_Manager->Resume_Channel(_iChannelIndex);
}

void CGameInstance::Resume_AllChannels()
{
	m_pSound_Manager->Resume_AllChannels();
}

void CGameInstance::Set_ChannelVolume(size_t _iChannelIndex, _float _fVolume)
{
	m_pSound_Manager->Set_ChannelVolume(_iChannelIndex, _fVolume);
}

void CGameInstance::Set_AllChannelVolume(_float _fVolume)
{
	m_pSound_Manager->Set_AllChannelsVolume(_fVolume);
}

void CGameInstance::Set_AudioVolume(const string& _strTag, _float _fVolume)
{
	m_pSound_Manager->Set_AudioVolume(_strTag, _fVolume);
}

const unordered_map<string, CAudioClip*>& CGameInstance::Get_AudioClips() const
{
	return m_pSound_Manager->Get_AudioClips();
}

void CGameInstance::Clear_AudioClips()
{
	m_pSound_Manager->Clear_AudioClips();
}

void CGameInstance::Remove_AudioClip(const string& _strTag)
{
	m_pSound_Manager->Remove_AudioClip(_strTag);
}
#pragma endregion

HRESULT CGameInstance::Add_Light(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const LIGHTDESC& _tDesc)
{
	return m_pLight_Manager->Add_Light(_pDevice, _pContext, _tDesc);
}

HRESULT CGameInstance::Change_Light(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const LIGHTDESC& _tDesc, _uint _uIdx)
{
	return m_pLight_Manager->Change_Light(_pDevice, _pContext, _tDesc, _uIdx);
}

const LIGHTDESC* CGameInstance::Get_Light(_uint _uIdx)
{
	return m_pLight_Manager->Get_LightDesc(_uIdx);
}

_uint CGameInstance::Get_LightCount()
{
	return m_pLight_Manager->Get_LightListSize();
}

HRESULT CGameInstance::Clear_Lights()
{
	return m_pLight_Manager->Clear_Lights();
}

HRESULT CGameInstance::Delete_Light(_uint uIdx)
{
	return m_pLight_Manager->Delete_Light(uIdx);
}

HRESULT CGameInstance::Bind_GameObject_To_Light(CGameObject* _pObject, _uint _iIndex)
{
	return m_pLight_Manager->Bind_GameObject(_pObject, _iIndex);
}

/*
HRESULT CGameInstance::Add_Font(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const wstring& _strFontTag, const wstring& _strFontFilePath)
{
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	return m_pFont_Manager->Add_Font(_pDevice, _pContext, _strFontTag, _strFontFilePath);
}

_float2 CGameInstance::Get_TextSize(const wstring& _wstrFontTag, const wstring& _wstrText, _float _fScale, _float _fWinSizeX, _float _fWinSizeY)
{
	if (nullptr == m_pFont_Manager)
		return _float2(0.f, 0.f);

	return m_pFont_Manager->Get_TextSize(_wstrFontTag, _wstrText, _fScale, _fWinSizeX, _fWinSizeY);
}

_float2 CGameInstance::Get_TextSize_Except_Color(const wstring& _wstrFontTag, const wstring& _wstrText, _float _fScale, _float _fWinSizeX, _float _fWinSizeY)
{
	if (nullptr == m_pFont_Manager)
		return _float2(0.f, 0.f);

	return m_pFont_Manager->Get_TextSize_Except_Color(_wstrFontTag, _wstrText, _fScale, _fWinSizeX, _fWinSizeY);
}

HRESULT CGameInstance::Render_Font(const wstring& _strFontTag, const wstring& _strText, const _float2 _vPos, _fvector _vColor, _float _fScale, _float _fWinSizeX, _float _fWinSizeY, _bool _bAutoCenter, const _float _fRotation)
{
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	return m_pFont_Manager->Render_Font(_strFontTag, _strText, _vPos, _vColor, _fScale, _fWinSizeX, _fWinSizeY, _bAutoCenter, _fRotation);
}

HRESULT CGameInstance::Render_Font_MultiLine(const wstring& _strFontTag, const wstring& _strText, const _float2 _vPos, _fvector _vColor, _float _fScale, _float _fWinSizeX, _float _fWinSizeY, _bool _bAutoCenter, const _float _fRotation)
{
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	return m_pFont_Manager->Render_Font_MultiLine(_strFontTag, _strText, _vPos, _vColor, _fScale, _fWinSizeX, _fWinSizeY, _bAutoCenter, _fRotation);
}

HRESULT CGameInstance::Render_Font_ApplyColorToSubstring(const wstring& _wstrFontTag, vector<wstring>& _vecApply, _fvector _vApplyColor, const wstring& _wstrText, const _float2 _vPos, _fvector _vColor, _float _fScale, _float _fWinSizeX, _float _fWinSizeY, _bool _bAutoCenter, const _float _fRotation)
{
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	return m_pFont_Manager->Render_Font_ApplyColorToSubstring(_wstrFontTag, _vecApply, _vApplyColor, _wstrText, _vPos, _vColor, _fScale, _fWinSizeX, _fWinSizeY, _bAutoCenter, _fRotation);
}

HRESULT CGameInstance::Render_Font_Color(const wstring& _wstrFontTag, const wstring& _wstrText, const _float2 _vPos, _float _fAlpha, _float _fScale, _float _fWinSizeX, _float _fWinSizeY, _bool _bAutoCenter, const _float _fRotation)
{
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	return m_pFont_Manager->Render_Font_Color(_wstrFontTag, _wstrText, _vPos, _fAlpha, _fScale, _fWinSizeX, _fWinSizeY, _bAutoCenter, _fRotation);
}

HRESULT CGameInstance::Render_Font_Color_V2(const wstring& _wstrFontTag, const wstring& _wstrText, const _float2 _vPos, _float _fAlpha, _float _fScale, _float _fWinSizeX, _float _fWinSizeY, _bool _bAutoCenter, const _float _fRotation)
{
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	return m_pFont_Manager->Render_Font_Color_V2(_wstrFontTag, _wstrText, _vPos, _fAlpha, _fScale, _fWinSizeX, _fWinSizeY, _bAutoCenter, _fRotation);
}
*/

HRESULT CGameInstance::Add_EventMsg(EventMsg _tEventMsg)
{
	if (nullptr == m_pEvent_Manager)
		return E_FAIL;

	return m_pEvent_Manager->Add_EventMsg(_tEventMsg);
}

_vector CGameInstance::Get_MouseRayPosLocal()
{
	return m_pPicking_Manager->Get_MouseRayPosLocal();
}

_vector CGameInstance::Get_MouseRayLocal()
{
	return m_pPicking_Manager->Get_MouseRayLocal();
}

_vector CGameInstance::Get_MouseRay()
{
	return m_pPicking_Manager->Get_MouseRay();
}

_vector CGameInstance::Get_MouseRayPos()
{
	return m_pPicking_Manager->Get_MouseRayPos();
}

/*
HRESULT CGameInstance::Add_Effect(string _szEffectName, CGameObject* _pEffectObject)
{
	return m_pEffect_Manager->Add_Effect(_szEffectName, _pEffectObject);
}

CGameObject* CGameInstance::Generate_Effect(string _szEffectName, void* _pArg)
{
	return m_pEffect_Manager->Create_Effect(_szEffectName, _pArg);
}

HRESULT CGameInstance::Clear_EffectMap()
{
	return m_pEffect_Manager->Clear_Effect_Map();
}
*/

void CGameInstance::Set_GameMode(CGameModeImpl* _pImpl)
{
	m_pGameMode->Set_GameMode(_pImpl);
}

void CGameInstance::Change_GameMode(CGameModeImpl* _pImpl)
{
	m_pGameMode->Change_GameMode(_pImpl);
}

void CGameInstance::Handling_Event_GameMode(EventMsg _tEventMsg)
{
	m_pGameMode->Handling_Event(_tEventMsg);
}

void CGameInstance::Reset_GameMode()
{
	m_pGameMode->Reset_GameMode();
}

HRESULT CGameInstance::Render_GameMode()
{
	return m_pGameMode->Render();
}

void CGameInstance::Release_Engine()
{
	CEvent_Manager::Destroy_Instance();
	CLevel_Manager::Destroy_Instance();
	CJson_Manager::Destroy_Instance();
	CRender_Manager::Destroy_Instance();
	CFont_Manager::Destroy_Instance();
	CFrustum::Destroy_Instance();
	CEffect_Manager::Destroy_Instance();
	CLight_Manager::Destroy_Instance();
	CObject_Manager::Destroy_Instance();
	CCollision_Manager::Destroy_Instance();
	CComponent_Manager::Destroy_Instance();
	CPicking_Manager::Destroy_Instance();
	CTimer_Manager::Destroy_Instance();
	CPipeLine::Destroy_Instance();
	CKey_Manager::Destroy_Instance();
	CRenderTarget_Manager::Destroy_Instance();
	CUI_Manager::Destroy_Instance();
	CGameMode::Destroy_Instance();
	CSound_Manager::Destroy_Instance();
	CGraphic_Device::Destroy_Instance();
}

void CGameInstance::Free()
{
	Safe_Release(m_pEvent_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pEffect_Manager);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pCollision_Manager);
	Safe_Release(m_pFrustum);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pKey_Manager);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pPicking_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pRenderTarget_Manager);
	Safe_Release(m_pUI_Manager);
	Safe_Release(m_pGameMode);
	Safe_Release(m_pSound_Manager);
	Safe_Release(m_pGraphic_Device);
}
