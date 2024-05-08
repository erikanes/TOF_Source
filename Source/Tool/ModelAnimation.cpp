#include "Tool_BinaryExporter_pch.h"
#include "ModelAnimation.h"
#include "ModelChannel.h"
#include "ModelBone.h"

// Singleton
#include "GameInstance.h"

CModelAnimation::CModelAnimation()
{
	m_callback = bind(&CModelAnimation::_Run_Notify, this, placeholders::_1);
}

/*
CModelAnimation* CModelAnimation::Create(const aiAnimation* _pAnim, CModelData* _pModel)
{
	CModelAnimation* pInstance = new CModelAnimation;

	if (FAILED(pInstance->Initialize(_pAnim, _pModel)))
	{
		MSG_BOX("Failed to create : CAnimation");
		Safe_Release(pInstance);
	}

	return pInstance;
}
*/

HRESULT CModelAnimation::Initialize(const aiAnimation* _pAnim, CModelData* _pModel)
{
	m_fDuration = (_float)_pAnim->mDuration;
	m_fTickPerSec = (_float)_pAnim->mTicksPerSecond;

	strcpy_s(m_szName, _pAnim->mName.data);

	m_uNumChannels = _pAnim->mNumChannels;
	m_vCurKeyFrames.resize(m_uNumChannels);

	for (_uint i = 0; i < m_uNumChannels; ++i)
	{
		CModelChannel* pChannel = CModelChannel::Create(_pAnim->mChannels[i], _pModel);
		if (nullptr == pChannel)
			return E_FAIL;

		m_vChannels.push_back(pChannel);
		m_mapChannelBoneIdx.insert({ pChannel->GetBoneIdx(), i });

		auto vKeyFramDatas = pChannel->Get_vKeyFrames();
		for (_uint j = 0; j < vKeyFramDatas.size(); ++j)
		{
			if (m_fFirstTrackPosition > vKeyFramDatas[j].fTime)
				m_fFirstTrackPosition = vKeyFramDatas[j].fTime;
			else if (m_fLastTrackPosition < vKeyFramDatas[j].fTime)
				m_fLastTrackPosition = vKeyFramDatas[j].fTime;
		}
	}

	return S_OK;
}

/*
CModelAnimation* CModelAnimation::Create(ANIMDATA& _tAnimData, class CModelData* _pModel)
{
	CModelAnimation* pInstance = new CModelAnimation;

	if (FAILED(pInstance->Initialize(_tAnimData, _pModel)))
	{
		MSG_BOX("Failed to create : CAnimation");
		Safe_Release(pInstance);
	}

	return pInstance;
}
*/

HRESULT CModelAnimation::Initialize(ANIMDATA& _tAnimData, class CModelData* _pModel)
{
	m_fDuration = _tAnimData.fDuration;
	m_fTickPerSec = _tAnimData.fTickPerSec;
	strcpy_s(m_szName, _tAnimData.szName);
	m_uNumChannels = _tAnimData.uNumChannels;
	m_vCurKeyFrames.resize(m_uNumChannels);

	for (_uint i = 0; i < m_uNumChannels; ++i)
	{
		CModelChannel* pChannel = CModelChannel::Create(_tAnimData.pChannelDatas[i], _pModel);
		if (nullptr == pChannel)
			return E_FAIL;

		auto vKeyFramDatas = pChannel->Get_vKeyFrames();
		for (_uint j = 0; j < vKeyFramDatas.size(); ++j)
		{
			if (m_fFirstTrackPosition > vKeyFramDatas[j].fTime)
				m_fFirstTrackPosition = vKeyFramDatas[j].fTime;
			else if (m_fLastTrackPosition < vKeyFramDatas[j].fTime)
				m_fLastTrackPosition = vKeyFramDatas[j].fTime;
		}

		m_vChannels.push_back(pChannel);
		m_mapChannelBoneIdx.insert({ pChannel->GetBoneIdx(), i });
	}

	return S_OK;
}

/*
HRESULT CModelAnimation::Update_TransformationMatricies_By_TrackpPosition(_float _fTrackPosition, const vector<class CModelBone*>& _refVecBones)
{
	Update_VecKeyFrames(0);

	for (_uint i = 0; i < m_uNumChannels; ++i)
	{
		_bool bIsRoot = false;
		m_vCurKeyFrames[i] = 0;
		m_vChannels[i]->Update_TransformationMatrices(&m_vCurKeyFrames[i], _fTrackPosition, _refVecBones, m_szRootBone, m_callback);
	}

	return S_OK;
}

HRESULT CModelAnimation::Effect_Update_TransformationMatricies_By_TrackpPosition(_float _fTrackPosition, const vector<class CModelBone*>& _refVecBones)
{
	Update_VecKeyFrames(_fTrackPosition);
	Set_TrackPosition(_fTrackPosition);
	for (_uint i = 0; i < m_uNumChannels; ++i)
		m_vChannels[i]->Update_TransformationMatrices(&m_vCurKeyFrames[i], _fTrackPosition, _refVecBones, m_szRootBone, m_callback);

	return S_OK;
}
*/

void CModelAnimation::_Run_Notify(_uint _iFrame)
{
	auto iter = m_notifications.find(_iFrame);

	if (iter == m_notifications.end())
		return;

	for (auto& notify : m_notifications[_iFrame])
		notify();
}

void CModelAnimation::Add_Notify(NOTIFICATIONMSG _tMsg)
{
	_int iTrackPosition = _tMsg.iTrackPosition;
	auto eNotificationType = _tMsg.eNotificationType;

	switch (eNotificationType)
	{
	case NOTIFICATION_TYPE::SOUND:
	{
		string strTag = _tMsg.tNotificationSoundData.szTag;
		function<void()> cFunc = [strTag]() { cout << "Play sound\n"; CGameInstance::Get_Instance()->Play_Audio(strTag); };
		m_notifications[iTrackPosition].emplace_back(cFunc);
		break;
	}
	case NOTIFICATION_TYPE::EFFECT:
	{
		function<void()> cFunc = []() { cout << "Play Effect\n"; };
		m_notifications[iTrackPosition].emplace_back(cFunc);
		break;
	}
	case NOTIFICATION_TYPE::COLLISION:
	{
		function<void()> cFunc = []() { cout << "Play Collision\n"; };
		m_notifications[iTrackPosition].emplace_back(cFunc);
		break;
	}
	case NOTIFICATION_TYPE::COLLISION_TRANSFORM_CHANGE:
	{
		break;
	}
	case NOTIFICATION_TYPE::OBJECT_CREATE:
	{
		break;
	}
	case NOTIFICATION_TYPE::CUSTOM_EVENT:
	{
		function<void()> cFunc = []() { cout << "custom event\n"; };
		m_notifications[iTrackPosition].emplace_back(cFunc);
		break;
	}
	default:
		break;
	}
}

/*
_bool CModelAnimation::Is_Same_Notification(NOTIFICATIONMSG _tFirst, NOTIFICATIONMSG _tSecond)
{
	_bool bSame = false;

	return bSame;
}

void CModelAnimation::Remove_Notify(_int _iFrame, NOTIFICATIONMSG _tMsg)
{
	auto iter = m_notifications.find(_iFrame);
}
*/

void CModelAnimation::Remove_Notify(_uint _iFrame)
{
	auto iter = m_notifications.find(_iFrame);

	if (m_notifications.end() != iter)
		iter->second.pop_back();
}

HRESULT CModelAnimation::Add_NotificationMsg(NOTIFICATIONMSG _tMsg)
{
	auto iterPair = m_mapNotifications.find(_tMsg.iTrackPosition);

	if (m_mapNotifications.end() == iterPair)
	{
		m_mapNotifications.insert({_tMsg.iTrackPosition, list<NOTIFICATIONMSG>() });
		iterPair = m_mapNotifications.find(_tMsg.iTrackPosition);
	}
	
	auto& listNotifications = iterPair->second;
	listNotifications.push_back(_tMsg);
	Add_Notify(_tMsg);

	return S_OK;
}

/*
HRESULT CModelAnimation::Add_TransitionDesc(CAnimationState::TRANSITIONDESC _tNewDesc)
{
	auto iter = m_mapTransitionDesc.find(_tNewDesc.szSrcAnimName);

	if (m_mapTransitionDesc.end() != iter)
		m_mapTransitionDesc.erase(iter);

	m_mapTransitionDesc.insert({ _tNewDesc.szSrcAnimName, _tNewDesc });

	return S_OK;
}

CAnimationState::TRANSITIONDESC CModelAnimation::Get_TransitionDesc(string szAnimName)
{
	CAnimationState::TRANSITIONDESC tDesc;

	auto iter = m_mapTransitionDesc.find(szAnimName);

	if (m_mapTransitionDesc.end() != iter)
		return iter->second;
	else
		return tDesc;
}

void CModelAnimation::Reset_TrackPosition()
{
	m_fTrackPosition = 0.f;
}

void CModelAnimation::Reset_KeyFrameIdx()
{
	for (_int i = 0; i < m_vCurKeyFrames.size(); ++i)
		m_vCurKeyFrames[i] = 0;
}


HRESULT CModelAnimation::Update_TransofrmationMatriciesBetweenOtherAnim(_float _fTimeDelta, const vector<class CModelBone*>& _refVecBones,
	CModelAnimation* _pOtherAnim, _float _fTransitionTime, _float _fStartTrackPosition, _float _fTransitionTickPerSec)
{
	if (4 <= _refVecBones.size())
	{
		m_szRootBone = _refVecBones[3]->Get_Name();
		m_szRootBone = "Bip001";
	}

	m_fTrackPosition += _fTransitionTickPerSec * _fTimeDelta;

	for (_uint i = 0; i < m_uNumChannels; ++i)
	{
		auto iter = _pOtherAnim->m_mapChannelBoneIdx.find(m_vChannels[i]->GetBoneIdx());

		if (_pOtherAnim->m_mapChannelBoneIdx.end() == iter)
		{
#ifdef _DEBUG
			cout << "There is no same channel\n";
#endif // _DEBUG

			continue;
		}

		CModelChannel* pDstChannel = _pOtherAnim->m_vChannels[(*iter).second];

		_uint uFromKeyFrame = m_vCurKeyFrames[i];
		_uint uToKeyFrame = _pOtherAnim->m_vCurKeyFrames[(*iter).second];

		m_vChannels[i]->Update_TransformationMatrices(uFromKeyFrame, uToKeyFrame, m_fTrackPosition - _fStartTrackPosition, pDstChannel, _fTransitionTime, _refVecBones);
	}

	return S_OK;
}

void CModelAnimation::Update_VecKeyFrames(_float _fUntilDuration)
{
	Reset_KeyFrameIdx();

	for (_uint i = 0; i < m_vChannels.size(); ++i)
		m_vChannels[i]->Update_KeyFrameIdx(_fUntilDuration, &m_vCurKeyFrames[i]);
}

HRESULT CModelAnimation::Update_TransformationMatricies(_float _fTimeDelta, const vector<class CModelBone*>& _refVecBones)
{
	if (3 <= _refVecBones.size())
	{
		m_szRootBone = _refVecBones[2]->Get_Name();
		m_szRootBone = "Bip001";
	}

	m_fTrackPosition += m_fTickPerSec * _fTimeDelta;

	if (m_fDuration <= m_fTrackPosition)
		m_fTrackPosition = 0.f;

	for (_uint i = 0; i < m_uNumChannels; ++i)
		m_vChannels[i]->Update_TransformationMatrices(&m_vCurKeyFrames[i], m_fTrackPosition, _refVecBones, m_szRootBone, m_callback);

	return S_OK;
}
*/

void CModelAnimation::Free()
{
	for (auto& pChannel : m_vChannels)
		Safe_Release(pChannel);

	m_vChannels.clear();
}