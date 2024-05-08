#pragma once

#include "Tool_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameObject;
END

/*
BEGIN(Tool)
class CA_LayoutRight_Properties final
	: public CBase
{
	DECLARE_SINGLETON(CA_LayoutRight_Properties);

private:
	CA_LayoutRight_Properties();
	~CA_LayoutRight_Properties() = default;

	HRESULT Handling_Save(class CModelData* _pModelData);
	HRESULT Handling_Load(class CModelData* _pModelData);
	HRESULT Handling_Layout();
	*/

	void Save_Notification_Data(class CModelData* _pModelData, const char* _szSavePath);
	void Load_Notification_Data(class CModelData* _pModelData, const char* _szLoadPath);

	/*
public:
	HRESULT			Initialize(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	HRESULT			Tick(_float fTimeDelta);
	virtual void	Free() override;

	HRESULT			Handling_AnimList(class CModelData* _pModelData);
	HRESULT			Handling_PivotMatrix(class CCharacter* _pPlayerCharacter);
	HRESULT			Handling_AnimationStateData(class CModelData* _pModelData);
	HRESULT			Handling_RootBone(class CModelData* _pModelData);
	HRESULT			Handling_AnimationEvent(CModelData* _pModelData);

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	_bool					m_bInitializeWindowSizePosSetting = { false };
	string					m_szSelectedItem;
	string					m_szSelectedAnimName;
	class CCharacter*		m_pSelectedModel = { nullptr };


	string					m_szSrcTransitionAnimName = { "" };
	string					m_szDstTransitionAnimName = { "" };

	string					m_szSrcAnimName = { "" };
	string					m_szDstAnimName = { "" };

	_float					m_fTransitionTime = { 0.f };
	_float					m_fTransitionTickPerSec = { 60.f };
	_float					m_fTransitionNextTime = { 0.f };


	// For. Notification
	const char*				m_arrComboNotificationType[6] = { "SOUND", "EFFECT", "COLLISION", "COLLISION_CHANGE", "OBJECT_CREATE", "CUSTOM_EVENT"};
	const char*				m_arrEffectBatchType[2] = { "LOCAL", "WORLD"};
	const char*				m_arrCollisionOperationType[2] = { "Off", "On" };
	_int					m_iNotificationTypeIdx = { 0 };
	_int					m_iEffectBatchTypeIdx = { 0 };


	NOTIFICATION_TYPE		m_eNotificationType = { NOTIFICATION_TYPE::NONE };
	_int					m_iTrackPosition = { 0 };
	char					m_szArrSoundTag[MAX_PATH] = { "" };


	EFFECT_BATCH_TYPE		m_eEffectBatchType = { EFFECT_BATCH_TYPE::EFFECT_LOCAL };
	char					m_szArrEffectTag[MAX_PATH] = { "" };

	char					m_szCollisionTag[MAX_PATH] = { "" };
	_uint					m_uCollisionOperation = { 0u };

	// For. Notification collision transform change data
	char					m_szCollisionTransformChangeTag[MAX_PATH] = { "" };
	_float3					m_vCollisionChangeTranslation = { 0.f ,0.f, 0.f };
	_float3					m_vCollisionChangeScale = { 1.f, 1.f, 1.f };
	_float3					m_vCollisionChangeRotation = { 0.f, 0.f, 0.f };

	// For. Object create data
	char					m_szObjectCreateTag[MAX_PATH] = { "" };
	_int					m_iObjectCreateCount = { 0 };


	// For. Custom event notification
	char					m_szCustomEventTag[MAX_PATH] = { "" };
	_int					m_iCustomEventIndex = { -1 };
};
END
*/