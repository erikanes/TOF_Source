#include "Engine_pch.h"
#include "Animation.h"
#include "Channel.h"
#include "Animator.h"

#include "Bone.h"

CAnimation::CAnimation()
{
	m_callback = bind(&CAnimation::_Run_Notify, this, placeholders::_1);
}

CAnimation::CAnimation(const CAnimation& _rhs)
	: m_fDuration(_rhs.m_fDuration)
	, m_fTickPerSec(_rhs.m_fTickPerSec)
	, m_uNumChannels(_rhs.m_uNumChannels)
	, m_vCurKeyFrames(_rhs.m_vCurKeyFrames)
	, m_vChannels(_rhs.m_vChannels)
	, m_mapChannelBoneIdx(_rhs.m_mapChannelBoneIdx)
{
	strcpy_s(m_szName, _rhs.m_szName);

	for (auto& pChannel : m_vChannels)
		Safe_AddRef(pChannel);

	m_callback = bind(&CAnimation::_Run_Notify, this, placeholders::_1);
}

/*
CAnimation* CAnimation::Create(const ANIMDATA& _tAnimData)
{
	CAnimation* pInstance = new CAnimation;

	if (FAILED(pInstance->Initialize(_tAnimData)))
	{
		MSG_BOX("Failed to create : CAnimation");
		Safe_Release(pInstance);
	}

	return pInstance;

}


HRESULT CAnimation::Initialize(const ANIMDATA& _tAnimData)
{
	m_fDuration = _tAnimData.fDuration;
	m_fTickPerSec = _tAnimData.fTickPerSec;
	strcpy_s(m_szName, _tAnimData.szName);
	m_uNumChannels = _tAnimData.uNumChannels;
	m_vCurKeyFrames.resize(m_uNumChannels);

	for (_uint i = 0; i < m_uNumChannels; ++i)
	{
		CChannel* pChannel = CChannel::Create(_tAnimData.pChannelDatas[i]);
		if (nullptr == pChannel)
			return E_FAIL;

		m_vChannels.push_back(pChannel);
		m_mapChannelBoneIdx.insert({ pChannel->GetBoneIdx(), i });
	}

	return S_OK;
}


HRESULT CAnimation::Update_TransformationMatriciesNoRotationBetweenOtherAnim(_float _fTimeDelta, const vector<class CBone*>& _refVecBones, 
	CAnimation* _pOtherAnim, _float _fTransitionTime, _float _fStartTrackPosition, _float _fTransitionTickPerSec)
{
	_float fTrackPosition = m_pOwner->Get_TrackPosition();

	if (_fTransitionTickPerSec <= 0.f)
		fTrackPosition += _pOtherAnim->m_fTickPerSec * _fTimeDelta;
	else
		fTrackPosition += _fTimeDelta * _fTransitionTickPerSec;

	m_pOwner->Set_TrackPosition(fTrackPosition);

	for (_uint i = 0; i < m_uNumChannels; ++i)
	{

		auto iter = _pOtherAnim->m_mapChannelBoneIdx.find(m_vChannels[i]->GetBoneIdx());

		if (_pOtherAnim->m_mapChannelBoneIdx.end() == iter)
		{
			continue;
		}

		CChannel* pDstChannel = _pOtherAnim->m_vChannels[(*iter).second];

		_uint uFromKeyFrame = m_vCurKeyFrames[i];
		_uint uToKeyFrame = _pOtherAnim->m_vCurKeyFrames[(*iter).second];

		m_vChannels[i]->Update_TransitionMatricesNoRotation(uFromKeyFrame, uToKeyFrame, fTrackPosition - _fStartTrackPosition, pDstChannel, _fTransitionTime, _refVecBones);
	}

	return S_OK;
}


HRESULT CAnimation::Update_TransofrmationMatriciesBetweenOtherAnim(_float _fTimeDelta, const vector<class CBone*>& _refVecBones,
	CAnimation* _pOtherAnim, _float _fTransitionTime, _float _fStartTrackPosition, _float _fTransitionTickPerSec)
{
	//A -> B로 애니메이션 전환이 일어날때
	//B애니메이션을 기준으로 전환을 함. 그래서 TickPerSec도 B애니메이션 기준으로 전환하게 됨.
	_float fTrackPosition = m_pOwner->Get_TrackPosition();

	if (_fTransitionTickPerSec <= 0.f)
		fTrackPosition += _pOtherAnim->m_fTickPerSec * _fTimeDelta;
	else
		fTrackPosition += _fTimeDelta * _fTransitionTickPerSec;

	m_pOwner->Set_TrackPosition(fTrackPosition);

	for (_uint i = 0; i < m_uNumChannels; ++i)
	{

		auto iter = _pOtherAnim->m_mapChannelBoneIdx.find(m_vChannels[i]->GetBoneIdx());

		CChannel* pDstChannel = _pOtherAnim->m_vChannels[(*iter).second];

		_uint uFromKeyFrame = m_vCurKeyFrames[i];
		_uint uToKeyFrame = _pOtherAnim->m_vCurKeyFrames[(*iter).second];

		m_vChannels[i]->Update_TransformationMatrices(fTrackPosition - _fStartTrackPosition, pDstChannel, uToKeyFrame, _fTransitionTime, _refVecBones);
	}

	return S_OK;
}

_bool CAnimation::Is_LastDuration(_float _fTimeDelta)
{
	_float fTrackPosition = m_pOwner->Get_TrackPosition();
	if (fTrackPosition + m_fTickPerSec * _fTimeDelta >= m_fDuration)
		return true;
	else
		return false;
}

HRESULT CAnimation::Update_TransformationMatriciesBetweenOtherAnimForSpecificBoneRange(const vector<class CBone*>& _refVecBones,
	const set<_int>& _vBoneIndicies, CAnimation* _pOtherAnim, _float _fTransitionTime, _float _fTrackPosition, _float _fStartTrackPosition,
	_float _fTransitionTickPerSec)
{
	for (_uint i = 0; i < m_uNumChannels; ++i)
	{
		if (_vBoneIndicies.end() == _vBoneIndicies.find(m_vChannels[i]->GetBoneIdx()))
		{
			continue;
		}


		auto iter = _pOtherAnim->m_mapChannelBoneIdx.find(m_vChannels[i]->GetBoneIdx());

		if (_pOtherAnim->m_mapChannelBoneIdx.end() == iter)
		{
#ifdef _DEBUG
			cout << "There is no same channel\n";
#endif // _DEBUG

			continue;
		}

		CChannel* pDstChannel = _pOtherAnim->m_vChannels[(*iter).second];

		_uint uFromKeyFrame = m_vCurKeyFrames[i];
		_uint uToKeyFrame = _pOtherAnim->m_vCurKeyFrames[(*iter).second];

		m_vChannels[i]->Update_TransformationMatrices(uFromKeyFrame, uToKeyFrame, _fTrackPosition - _fStartTrackPosition, pDstChannel, _fTransitionTime, _refVecBones);
	}




	return S_OK;
}
*/

HRESULT CAnimation::Update_TransformationMatriciesSpecificBoneRange(const vector<class CBone*>& _refVecBones, 
	const set<_int>& _vBoneIndicies, _bool _bLoop, _float _fTrackPosition, _float _fTrackPositionBefore)
{
	// TrackPosition은 외부에서 관리하도록 했음.
	// 서로 다른 2개의 애니메이션을 재생해야 하기 때문.

	for (_int i = 0; i < m_vChannels.size(); ++i)
	{
		_int iBoneIdx = m_vChannels[i]->GetBoneIdx();
		if (_vBoneIndicies.end() == _vBoneIndicies.find(iBoneIdx))
			continue;
		else
			m_vChannels[i]->Update_TransformationMatrices(&m_vCurKeyFrames[i], _fTrackPosition, _fTrackPositionBefore,
				_refVecBones, m_callback);
	}

	return S_OK;
}

HRESULT CAnimation::Update_TransformationMatricies(_float _fTimeDelta, const vector<class CBone*>& _refVecBones, _bool _bLoop)
{
	_float fTrackPosition = m_pOwner->Get_TrackPosition();

	_float fTrackPositionBefore = fTrackPosition;
	// 외부에서 trackPosition을 변경해서 0값으로 맞춘 경우, 이 함수에 들어오면 0아닌 값에서 부터 시작하게됨.
	// Root Motion translation 값 초기화 시점 때문에 필요.

	if (m_bUseCustomTickPerSec)
	{
		fTrackPosition += m_fCustomTickPerSec * _fTimeDelta;
	}
	else
	{
		fTrackPosition += m_fTickPerSec * _fTimeDelta;
	}

	if (!_bLoop && m_fDuration <= fTrackPosition)
	{
		return S_OK;
	}

	if (m_fDuration <= fTrackPosition)
	{
		fTrackPosition = 0.f;
	}
	m_pOwner->Set_TrackPosition(fTrackPosition);

	for (_uint i = 0; i < m_uNumChannels; ++i)
	{
		m_vChannels[i]->Update_TransformationMatrices(&m_vCurKeyFrames[i], 
			fTrackPosition, fTrackPositionBefore, _refVecBones, m_callback);
	}

	return S_OK;
}

/*
void CAnimation::Update_VecKeyFrames(_float _fUntilDuration)
{
	for (_uint i = 0; i < m_vChannels.size(); ++i)
	{
		m_vChannels[i]->Update_KeyFrameIdx(_fUntilDuration, &m_vCurKeyFrames[i]);
	}
}

void CAnimation::Reset_KeyFrameIdx()
{
	for (_uint i = 0; i < m_vCurKeyFrames.size(); ++i)
	{
		m_vCurKeyFrames[i] = 0;
	}

}
*/

void CAnimation::Add_Notify(_uint _iFrame, _uint _eType, function<void()> _funcNotify)
{
	m_notifications[_iFrame].emplace_back(_eType, _funcNotify);
}

void CAnimation::Remove_Notify(_uint _iFrame, _uint _iIndex)
{
	auto iter = m_notifications.find(_iFrame);
	
	if (iter == m_notifications.end())
		return;
	
	auto& notifications = iter->second;
	
	if (static_cast<_uint>(notifications.size()) <= _iIndex)
		return;
	
	notifications.erase(notifications.begin() + _iIndex);
}

void CAnimation::_Run_Notify(_uint _iFrame)
{
	auto iter = m_notifications.find(_iFrame);

	if (iter == m_notifications.end())
		return;

	auto iterList = m_notifications[_iFrame];

	for (auto& notify : iterList)
		notify.second();
}

/*
void CAnimation::Change_AnimState()
{
	m_pOwner->Set_TrackPosition(0.f);
}

CAnimation* CAnimation::Clone()
{
	return new CAnimation(*this);
}

void CAnimation::Free()
{
	for (auto& pChannel : m_vChannels)
	{
		Safe_Release(pChannel);
	}
	m_vChannels.clear();

	for (auto& notifications : m_notifications)
	{
		notifications.second.clear();
	}
	m_notifications.clear();

	m_mapChannelBoneIdx.clear();
}
*/