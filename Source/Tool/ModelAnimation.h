#pragma once

#include "Tool_Defines.h"
#include "AnimationState.h"
#include "Engine_Struct_Data.h"

BEGIN(Tool)
class CModelAnimation final
	: public CBase
{
	using Notifications = map<_uint, list<function<void()>>>;

private:
	CModelAnimation();
	~CModelAnimation() = default;

public:
	static CModelAnimation* Create(const aiAnimation* _pAnim, class CModelData* _pModel);
	static CModelAnimation* Create(ANIMDATA& _tAnimData, class CModelData* _pModel);

	HRESULT Initialize(const aiAnimation* _pAnim, class CModelData* _pModel);
	HRESULT Initialize(ANIMDATA& _tAnimData, class CModelData* _pModel);

	HRESULT Update_TransformationMatricies(_float _fTimeDelta, const vector<class CModelBone*>& _refVecBones);

	HRESULT Update_TransofrmationMatriciesBetweenOtherAnim(_float _fTimeDelta, const vector<class CModelBone*>& _refVecBones,
		CModelAnimation* _pOtherAnim, _float _fTransitionTime, _float _fStartTrackPosition, _float _fTransitionTickPerSec);
	
	HRESULT Update_TransformationMatricies_By_TrackpPosition(_float _fTrackPosition, const vector<class CModelBone*>& _refVecBones);
	HRESULT Effect_Update_TransformationMatricies_By_TrackpPosition(_float _fTrackPosition, const vector<class CModelBone*>& _refVecBones);
	void	Update_VecKeyFrames(_float _fUntilDuration);

	virtual void Free();

	map<string, CAnimationState::TRANSITIONDESC>* Get_MapTransitionDesc() { return &m_mapTransitionDesc; }

	_bool					Is_Same_Notification(NOTIFICATIONMSG _tFirst, NOTIFICATIONMSG _tSecond);

	void					Add_Notify(NOTIFICATIONMSG _tMsg);

	void					Remove_Notify(_int _iFrame, NOTIFICATIONMSG _tMsg);
	void					Remove_Notify(_uint _iFrame);
	Notifications&			Get_NotificationsFunc() { return m_notifications; }

private:
	void _Run_Notify(_uint _iFrame);

private:
	string											m_szRootBone = { "Root_Bone" };
	char											m_szName[MAX_PATH] = { "" };
	_float											m_fDuration = { 0.f };
	_float											m_fTickPerSec = { 0.f };
	_float											m_fTrackPosition = { 0.f };
	_float											m_fCurTransitionTrackPosition = { 0.f };
	_float											m_fTransitionTrackPosition = { 0.f };

	_uint											m_uNumChannels = { 0 };
	vector <_uint>									m_vCurKeyFrames;
	vector <class CModelChannel*>					m_vChannels;

	_float											m_fFirstTrackPosition = { 0.f };
	_float											m_fLastTrackPosition = { 0.f };

	map<string, CAnimationState::TRANSITIONDESC>	m_mapTransitionDesc;
	map	<_uint, _uint>								m_mapChannelBoneIdx;

	map<_int, list<NOTIFICATIONMSG>>				m_mapNotifications;

	Notifications									m_notifications;

	function<void(_uint)>							m_callback;

public:
	const char* Get_Name() { return m_szName; }
	_float		Get_fDuration() { return m_fDuration; }
	_float		Get_fTickPerSec() { return m_fTickPerSec; }
	void		Set_fTickPerSec(_float _fTickPerSec) {  m_fTickPerSec = _fTickPerSec; }
	_float		Get_fTrackPosition() { return m_fTrackPosition; }
	_uint		Get_uNumChannels() { return m_uNumChannels; }

	_float		Get_fTransitionTrackPosition() { return m_fCurTransitionTrackPosition; }
	void		Set_fTransitionTrackPosition(_float _fTrackPosition) { m_fCurTransitionTrackPosition = _fTrackPosition; }

	vector<class CModelChannel*>& Get_vChannels() { return m_vChannels; }

	map<_int, list<NOTIFICATIONMSG>>& Get_Notifications() { return m_mapNotifications; }

	_float2 Get_TrackPositionRange() { return _float2(m_fFirstTrackPosition, m_fLastTrackPosition); }

	HRESULT Add_TransitionDesc(CAnimationState::TRANSITIONDESC _tNewDesc);
	HRESULT Add_NotificationMsg(NOTIFICATIONMSG _tMsg);
	
	CAnimationState::TRANSITIONDESC Get_TransitionDesc(string szAnimName);

	void Reset_TrackPosition();
	void Reset_KeyFrameIdx();
	void Reset_TransitionTrackPosition() { m_fCurTransitionTrackPosition = 0.f; }
	void Set_TrackPosition(_float _fTrackPosition) { m_fTrackPosition = _fTrackPosition; }
};
END
