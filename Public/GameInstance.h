#pragma once

#include "Component_Manager.h"
#include "PipeLine.h"

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public: /* For.Engine */
	HRESULT Initialize_Engine(HINSTANCE _hInstance, _uint iNumCollisionGroup, _uint iNumLevels, const GRAPHICDESC& GraphicDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext);
	void Tick_Engine(_float fTimeDelta);
	void Clear_All();
	void Clear(_uint iLevelIndex);
	void Clear_All_UI();
	void Clear_UI(_uint iLevelIndex);
	void Clear_Level(_uint _iLevelIndex);

public: /* For.Graphic_Device */
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);
	HRESULT Clear_DepthStencil_View();
	HRESULT Present();
	HRESULT Set_NormalRenderState();
	HRESULT Set_WireframeRenderState();

public: /* For.RenderTarget_Manager */
	class CRenderTarget* Find_RenderTarget(const wstring& _wstrTag);
	HRESULT Bind_ShaderResource_RTV(class CShader* pShader, const wstring& strTargetTag, const char* pConstantName);

public: /* For.Level_Manager */
	HRESULT Open_Level(_uint iLevelIndex, class CLevel* pLevel);
	Level   Get_CurLevel(_bool _bIsLoading = false);
	void* Get_SerializedData();
	function<void(any)> Get_Usual_Function_In_Level(MARKED_FUNCTION_TAG _eTag);

public: /* For.Object_Manager */
	class CComponent* Get_Component(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strComTag, _uint iIndex = 0);
	HRESULT Add_Prototype(const wstring& strPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_GameObject(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg = nullptr);
	HRESULT Add_GameObject(_uint _iLevelIndex, const wstring& _strLayerTag, CGameObject* _pGameObject);
	HRESULT	Set_Dead_Specific_Game_Objects(_uint _iLevelIndex, const wstring& _strLayerTag);
	HRESULT Create_GameObject(const wstring& _strPrototypeTag, CGameObject** _ppOut, void* _pArg = nullptr);
	HRESULT Create_ObjectPool(const wstring& _strPoolTag, _uint _iSize, CGameObject* _pPrototype, void* _pArg = nullptr, const wstring& _wstrLayerTag = TEXT("Layer_ObjectPool"));
	CGameObject* Pop_FromObjectPool(const wstring& _strPoolTag, any _args);
	CGameObject* Get_FromObjectPool(const wstring& _strPoolTag, any _args);
	CGameObject* Get_Prototype(const wstring& strPrototypeTag);
	HRESULT Set_RegisterUpdateLevel(_uint _uLevel, _bool _bRegistered);
	map<wstring, class CLayer*>* Get_SpecificLayer(_uint _uLevel);

public: /* For.UI_Manager */
	class CComponent* Get_Component_UI(_uint _iLevelIndex, const wstring& _strLayerTag, const wstring& _strComTag, _uint _iIndex = 0);
	HRESULT Add_Prototype_UI(const wstring& _strPrototypeTag, class CGameObject* _pPrototype);
	HRESULT Add_GameObject_UI(_uint _iLevelIndex, const wstring& _strLayerTag, const wstring& _strPrototypeTag, void* _pArg = nullptr);
	HRESULT Add_GameObject_UI(_uint _iLevelIndex, const wstring& _strLayerTag, CGameObject* _pGameObject);
	HRESULT Create_GameObject_UI(const wstring& _strPrototypeTag, CGameObject** _ppOut, void* _pArg = nullptr);

	HRESULT Set_UpdateLevel_UI(_uint _uLevel, _bool _bRegistered);
	map<wstring, class CLayer*>* Get_SpecificLayer_UI(_uint _uLevel);

	/* 해당 레벨의 해당 Layer Tag에 있는 모든 UI 활성화 */
	HRESULT Enable_UI(_uint _iNumLevels, const wstring& _strLayerTag);
	/* 해당 레벨의 해당 Layer Tag에 있는 모든 UI 비활성화 */
	HRESULT Disable_UI(_uint _iNumLevels, const wstring& _strLayerTag);
	/* 활성화 하면서 필요한 정보 전달 */
	HRESULT Enable_UI_InteractionInfo(_uint _iNumLevels, const wstring& _wstrLayerTag, const wstring& _wstrInteractionObjectName, _uint _eInteractionType);

	/// 나중에 다 밑에 껄로 바꿀 생각
	/* 해당 레벨의 해당 Layer Tag에 있는 모든 UI 활성화 */
	HRESULT Enable_UI(_uint _iNumLevels, const wstring& _wstrLayerTag, any _msgDesc); // any넘겨서 가상함수 호출
	/* 해당 레벨의 해당 Layer Tag에 있는 특정 UI 활성화 */
	HRESULT Enable_UI_Specific(_uint _iNumLevels, const wstring& _wstrLayerTag, const wstring& _wstrName, any msgDesc);
	/* 해당 레벨의 해당 Layer Tag에 있는 모든 UI 비활성화 */
	HRESULT Disable_UI(_uint _iNumLevels, const wstring& _wstrLayerTag, any _msgDesc);
	/* 해당 레벨의 해당 Layer Tag에 있는 특정 UI 비활성화 */
	HRESULT Disable_UI_Specific(_uint _iNumLevels, const wstring& _wstrLayerTag, const wstring& _wstrName, any _msgDesc);

	// 마우스 동작 활성화 여부 제어
	/* 해당 레벨의 해당 Layer Tag에 있는 모든 UI 마우스 동작 활성화 */
	HRESULT MouseEnable_UI(_uint _iNumLevels, const wstring& _wstrLayerTag);
	/* 해당 레벨의 해당 Layer Tag에 있는 특정 UI 마우스 동작 활성화 */
	HRESULT MouseEnable_UI_Specific(_uint _iNumLevels, const wstring& _wstrLayerTag, const wstring& _wstrName);
	/* 해당 레벨의 해당 Layer Tag에 있는 모든 UI 마우스 동작 비활성화 */
	HRESULT MouseDisable_UI(_uint _iNumLevels, const wstring& _wstrLayerTag);
	/* 해당 레벨의 해당 Layer Tag에 있는 특정 UI 마우스 동작 비활성화 */
	HRESULT MouseDisable_UI_Specific(_uint _iNumLevels, const wstring& _wstrLayerTag, const wstring& _wstrName);

public: /* For.Component_Manager */
	HRESULT Add_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag, class CComponent* pPrototype);
	CComponent* Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, void* pArg = nullptr);
	CComponent* Add_ColliderComponent(_uint iLevelIndex, const wstring& strPrototypeTag, class CGameObject* _pOwner, _int eCollisionGroup, void* pArg = nullptr);
	HRESULT Clear_PrototpyeComponent(_int _eLevel);
	HRESULT	RegisterNoDeleteLevel(_int _eLevel);
	ID3D11ShaderResourceView* Get_Texture_SRV(_uint iLevelIndex, const wstring& strPrototypeTag, _uint _uIdx);

public: /* For.Collision_Manger*/
	HRESULT Register_Collider(_int _eCollisionGroup, CCollider* _pCollider);
	//HRESULT Release_Collider(CCollider* _pCollider);
	HRESULT Register_Collision_Group(_uint eLeftGroupType, _uint eRightGroupType);
	HRESULT ResetCollisionGroup();
	HRESULT ClearCollisionGroup();

#ifdef _DEBUG
	void	Set_Collider_Render(_bool _bRender);
	_int	Get_Collider_Count();
	_int	Get_Collider_Count_For_Specific_Collision_Group(_int _eGroupType);
#endif

public: /* For.Timer_Manager */
	HRESULT Add_Timer(const wstring& strTimerTag);
	_float Compute_TimeDelta(const wstring& strTimerTag);
	void Set_TimeScale(const wstring& _strTimerTag, _float _fTimeScale);

public: /* For.PipeLine */
	_matrix		Get_Transform_Matrix(CPipeLine::TRANSFORMSTATE _eState);
	_float4x4	Get_Transform_Float4x4(CPipeLine::TRANSFORMSTATE _eState);
	_matrix		Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORMSTATE _eState);
	_float4x4	Get_Transform_Float4x4_Inverse(CPipeLine::TRANSFORMSTATE _eState);
	_float4		Get_CamPosition();
	_float3		Get_CamForward();
	_float		Get_CameraNear() const;
	_float		Get_CameraFar() const;
	_float		Get_CameraFov() const;

public: /* For.Key_Manager */
	KEY_STATE	Get_KeyState(_ubyte ubyKeyID);
	KEY_STATE	Get_MouseState(MOUSEKEYSTATE _eMouseID);
	_float2		Get_MouseCurPos();
	_long		Get_MouseMoveState(MOUSEMOVESTATE _eMouseMove);
	_bool		Is_MouseInRestrictedArea();
	void		Set_MouseRestrictedArea(RECT _rt);

public: /* For.Sound_Manager */
	HRESULT Add_AudioClip(const string& _strFilePath, const string& _strTag, AUDIO_CLIP_TYPE _eType, _float _fVolume = -1.f, _bool _bIsLoop = false);
	HRESULT Load_AudioClips(const string& _strFilePath);
	
	_int Play_Audio(const string& _strTag, _float _fVolume = -1.f, _bool _bUseFadeIn = false);
	void Stop_Audio(const string& _strTag, _bool _bUseFadeOut = false);
	_int Change_BGM(const string& _strTag, _float _fVolume = -1.f, _bool _bUseFade = false);
	_int Stop_BGM(_bool _bUseFadeOut = false);

	void FadeOut_BGM_Volume(_float _fVolume = 0.2f);
	void FadeIn_BGM_Volume(_float _fVolume = 1.f);

	void Stop_Channel(size_t _iChannelIndex, _bool _bUseFadeOut = false);
	void Stop_AllChannels();

	void Mute_Channel(size_t _iChannelIndex);
	void Mute_AllChannels();

	void UnMute_Channel(size_t _iChannelIndex);
	void UnMute_AllChannels();

	void Pause_Channel(size_t _iChannelIndex);
	void Pause_AllChannels();

	void Resume_Channel(size_t _iChannelIndex);
	void Resume_AllChannels();

	void Set_ChannelVolume(size_t _iChannelIndex, _float _fVolume);
	void Set_AllChannelVolume(_float _fVolume);
	void Set_AudioVolume(const string& _strTag, _float _fVolume);

	const unordered_map<string, class CAudioClip*>& Get_AudioClips() const;

	void Clear_AudioClips();
	void Remove_AudioClip(const string& _strTag);

public: /* For.Light_Manager */
	HRESULT				Add_Light(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const LIGHTDESC& _tDesc);
	HRESULT				Change_Light(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const LIGHTDESC& _tDec, _uint _uIdx);
	const LIGHTDESC* Get_Light(_uint uIdx);
	_uint				Get_LightCount();
	HRESULT				Clear_Lights();
	HRESULT				Delete_Light(_uint uIdx);

	HRESULT				Bind_GameObject_To_Light(CGameObject* _pObject, _uint _iIndex);

public: /* For.Event_Manager */
	HRESULT				Add_EventMsg(EventMsg _tEventMsg);

public:/* For.Font_Manager */
	HRESULT				Add_Font(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const wstring& _strFontTag, const wstring& _strFontFilePath);
	_float2				Get_TextSize(const wstring& _wstrFontTag, const wstring& _wstrText, _float _fScale, _float _fWinSizeX, _float _fWinSizeY);
	/* 컬러 태그 제외하고 한 줄 기준 문자열의 사이즈 구함 */
	_float2				Get_TextSize_Except_Color(const wstring& _wstrFontTag, const wstring& _wstrText, _float _fScale, _float _fWinSizeX, _float _fWinSizeY);
	HRESULT				Render_Font(const wstring& _strFontTag, const wstring& _strText, const _float2 _vPos, _fvector _vColor, _float _fScale, _float _fWinSizeX, _float _fWinSizeY, _bool _bAutoCenter = true, const _float _fRotation = 0.f);
	HRESULT				Render_Font_MultiLine(const wstring& _strFontTag, const wstring& _strText, const _float2 _vPos, _fvector _vColor, _float _fScale, _float _fWinSizeX, _float _fWinSizeY, _bool _bAutoCenter = true, const _float _fRotation = 0.f);
	HRESULT				Render_Font_ApplyColorToSubstring(const wstring& _wstrFontTag, vector<wstring>& _vecApply, _fvector _vApplyColor, const wstring& _wstrText, const _float2 _vPos, _fvector _vColor,
					_float _fScale, _float _fWinSizeX, _float _fWinSizeY, _bool _bAutoCenter = true, const _float _fRotation = 0.f);
	/* 퀘스트 텍스트 출력할 때 쓸 것 */
	HRESULT Render_Font_Color(const wstring& _wstrFontTag, const wstring& _wstrText, const _float2 _vPos, _float _fAlpha, _float _fScale, _float _fWinSizeX, _float _fWinSizeY, _bool _bAutoCenter = true, const _float _fRotation = 0.f);
	HRESULT Render_Font_Color_V2(const wstring& _wstrFontTag, const wstring& _wstrText, const _float2 _vPos, _float _fAlpha, _float _fScale, _float _fWinSizeX, _float _fWinSizeY, _bool _bAutoCenter = true, const _float _fRotation = 0.f);

public: /* For.Picking_Manager */
	_vector Get_MouseRayPosLocal();
	_vector Get_MouseRayLocal();

	_vector Get_MouseRay();
	_vector Get_MouseRayPos();

public: /* For.Effect_Manager */
	HRESULT			Add_Effect(string _szEffectName, CGameObject* _pEffectObject);
	CGameObject* Generate_Effect(string _szEffectName, void* _pArg = nullptr);
	HRESULT			Clear_EffectMap();

public: /* For.GameMode */
	void Set_GameMode(class CGameModeImpl* _pImpl);
	void Change_GameMode(class CGameModeImpl* _pImpl);
	void Reset_GameMode();
	HRESULT Render_GameMode(); // Test Code
	void Handling_Event_GameMode(EventMsg _tEventMsg);

private:
	class CGraphic_Device* m_pGraphic_Device = { nullptr };
	class CRenderTarget_Manager* m_pRenderTarget_Manager = { nullptr };
	class CComponent_Manager* m_pComponent_Manager = { nullptr };
	class CCollision_Manager* m_pCollision_Manager = { nullptr };
	class CGameMode* m_pGameMode = { nullptr };
	class CJson_Manager* m_pJson_Manager = { nullptr };
	class CPicking_Manager* m_pPicking_Manager = { nullptr };
	class CObject_Manager* m_pObject_Manager = { nullptr };
	class CEffect_Manager* m_pEffect_Manager = { nullptr };
	class CLevel_Manager* m_pLevel_Manager = { nullptr };
	class CFont_Manager* m_pFont_Manager = { nullptr };
	class CUI_Manager* m_pUI_Manager = { nullptr };
	class CTimer_Manager* m_pTimer_Manager = { nullptr };
	class CLight_Manager* m_pLight_Manager = { nullptr };
	class CSound_Manager* m_pSound_Manager = { nullptr };
	class CEvent_Manager* m_pEvent_Manager = { nullptr };
	class CKey_Manager* m_pKey_Manager = { nullptr };
	class CPipeLine* m_pPipeLine = { nullptr };
	class CFrustum* m_pFrustum = { nullptr };

public:
	static void Release_Engine();
	virtual void Free() override;
};

END