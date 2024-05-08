#pragma once

#include "Base.h"

BEGIN(Engine)
class ENGINE_DLL CAnimation final : public CBase
{
	using Notifications = map<_uint, list<pair<_uint, function<void()>>>>;

private:
	CAnimation();
	CAnimation(const CAnimation& _rhs);
	~CAnimation() = default;

public:
	static CAnimation* Create(const ANIMDATA& _tAnimData);

	HRESULT Initialize(const ANIMDATA& _tAnimData);

	HRESULT Update_TransformationMatricies(_float _fTimeDelta, const vector<class CBone*>& _refVecBones, _bool _bLoop);
	HRESULT Update_TransformationMatriciesSpecificBoneRange(const vector<class CBone*>& _refVecBone, const set<_int>& _vBoneIndicies, _bool _bLoop, _float _fTrackPosition, _float _fTrackPositionBefore);
	HRESULT Update_TransformationMatriciesBetweenOtherAnimForSpecificBoneRange(const vector<class CBone*>& _refVecBones, const set<_int>& _vBoneIndicies, CAnimation* _pOtherAnim, _float _fTransitionTime, _float _fTrackPosition, _float _fStartTrackPosition, _float _fTransitionTickPerSec);
	HRESULT	Update_TransofrmationMatriciesBetweenOtherAnim(_float _fTimeDelta, const vector<class CBone*>& _refVecBones, CAnimation* _pOtherAnim, _float _fTransitionTime, _float _fStartTrackPosition, _float _fTransitionTickPerSec);
	HRESULT Update_TransformationMatriciesNoRotationBetweenOtherAnim(_float _fTimeDelta, const vector<class CBone*>& _refVecBones, CAnimation* _pOtherAnim, _float _fTransitionTime, _float _fStartTrackPosition, _float _fTransitionTickPerSec);
	CAnimation* Clone();
	virtual void Free() override;

public:
	void		Change_AnimState();
	_bool		Is_LastDuration(_float _fTimeDelta);

	void		Update_VecKeyFrames(_float _fUntilDuration);
	void		Reset_KeyFrameIdx();

	void		Add_Notify(_uint _iFrame, _uint _eType, function<void()> _funcNotify);
	void		Remove_Notify(_uint _iFrame, _uint _iIndex);

#pragma region Getter
	_float		Get_Duration() { return m_fDuration; }
	_float		Get_TickPerSec() { return m_fTickPerSec; }
	_float		Get_CustomTickPerSec() { return m_fCustomTickPerSec; }

	const char* Get_Name() { return m_szName; }

	_bool		Get_UseCustomTickPerSec() { return m_bUseCustomTickPerSec; }

	Notifications& Get_Notifications() { return m_notifications; }
#pragma endregion

#pragma region Setter
	void		Set_Owner(class CAnimator* _pOwner) { m_pOwner = _pOwner; }

	void		Set_TickPerSec(_float _fTickPerSec) { m_fCustomTickPerSec = _fTickPerSec; }
	void		Set_UseCustomTickPerSec(_bool _bUse) { m_bUseCustomTickPerSec = _bUse; }
#pragma endregion

private:
	void _Run_Notify(_uint _iFrame);

private:
	char						m_szName[MAX_PATH] = { "" };
	class CAnimator*			m_pOwner = { nullptr };
	_float						m_fDuration = { 0.f };

	_float						m_fCurDuration = { 0.f };

	_float						m_fTickPerSec = { 0.f };
	_float						m_fCustomTickPerSec = { 0.f };
	_bool						m_bUseCustomTickPerSec = { false };

	_uint						m_uNumChannels = { 0 };
	vector <_uint>				m_vCurKeyFrames;
	vector <class CChannel*>	m_vChannels;

	map	<_uint, _uint>			m_mapChannelBoneIdx;
	// 모델의 본 index가 자신의 채널 벡터에서는 몇 번째 인덱스인지 알려주는 map
	// <bone idx, vector channel idx>

	Notifications				m_notifications;

	function<void(_uint)>		m_callback;
};

END
