#include "Engine_pch.h"
#include "Sound_Manager.h"

// Singleton
#include "Json_Manager.h"

#include "AudioClip.h"
#include "AudioChannel.h"

IMPLEMENT_SINGLETON(CSound_Manager);

HRESULT CSound_Manager::Initialize()
{
	FMOD_RESULT eResult = FMOD_ERR_NOTREADY;
	_uint iVersion = FMOD_VERSION;

	eResult = FMOD::System_Create(&m_pSystem);

	if (FMOD_OK != eResult)
		return E_FAIL;

	eResult = m_pSystem->getVersion(&iVersion);

	if (FMOD_OK != eResult)
		return E_FAIL;

	eResult = m_pSystem->init(MAX_CHANNEL_SIZE, FMOD_INIT_NORMAL, nullptr);

	if (FMOD_OK != eResult)
		return E_FAIL;

	m_vecChannels.reserve(MAX_CHANNEL_SIZE);

	for (_uint i = 0; i < MAX_CHANNEL_SIZE; ++i)
	{
		auto pChannel = CAudioChannel::Create(m_pSystem, i, [this](_uint _iIndex) { m_emptyChannels.push(_iIndex); });

		if (nullptr == pChannel)
			return E_FAIL;

		m_vecChannels.push_back(pChannel);
		m_emptyChannels.push(i);
	}

	m_pSystem->update();

	return S_OK;
}

void CSound_Manager::Update()
{
	for (auto& channel : m_vecChannels)
	{
		channel->Update();
	}

	m_pSystem->update();
}

HRESULT CSound_Manager::Add_AudioClip(const string& _strFilePath, const string& _strTag, AUDIO_CLIP_TYPE _eClipType, _float _fVolume, _bool _bIsLoop)
{
	FMOD::Sound* pSound = nullptr;
	FMOD_MODE eMode = _bIsLoop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
	FMOD_RESULT eResult = m_pSystem->createSound(_strFilePath.c_str(), eMode, nullptr, &pSound);

	if (FMOD_OK != eResult)
	{
		MSG_BOX("Failed to create sound: CAudioManager::AddAudioClip()");
		return E_FAIL;
	}

	auto pAudioClip = CAudioClip::Create(pSound, _strTag, _eClipType, _fVolume, eMode);

	if (nullptr == pAudioClip)
	{
		MSG_BOX("Failed to Create audio clip: CAudioManager::AddAudioClip()");
		return E_FAIL;
	}

	m_mapAudioClips.emplace(_strTag, pAudioClip);
	m_pSystem->update();

	return S_OK;
}

HRESULT CSound_Manager::Load_AudioClips(const string& _strFilePath)
{
	nlohmann::ordered_json jsonFile;

	if (FAILED(CJson_Manager::Parse(_strFilePath, jsonFile)))
		return E_FAIL;

	for (auto& group : jsonFile)
	{
		for (auto it = group.begin(); it != group.end(); ++it)
		{
			string strTag = it.key().c_str();

			auto strAudioFilePath = it.value()["path"].get<string>();
			auto eClipType = static_cast<AUDIO_CLIP_TYPE>(it.value()["type"].get<_int>());
			auto fVolume = it.value()["volume"].get<_float>();
			auto bIsLoop = it.value()["loop"].get<_bool>();

			Add_AudioClip(strAudioFilePath, strTag, eClipType, fVolume, bIsLoop);

			if (FAILED(Add_AudioClip(strAudioFilePath, strTag, eClipType, fVolume, bIsLoop)))
				return E_FAIL;
		}
	}

	return S_OK;
}

_int CSound_Manager::Play_Audio(const string& _strTag, _float _fVolume, _bool _bUseFadeIn)
{
	auto iter = m_mapAudioClips.find(_strTag);

	if (iter == m_mapAudioClips.end())
	{
		return -1;
	}

	_int iChannelIndex = -1;

	if (m_emptyChannels.empty())
		int a = 0; // for debugging

	else
	{
		iChannelIndex = m_emptyChannels.front();
		m_emptyChannels.pop();

		m_vecChannels[iChannelIndex]->Play(iter->second, _fVolume, _bUseFadeIn);
	}

	return iChannelIndex;
}

void CSound_Manager::Stop_Audio(const string& _strTag, _bool _bUseFadeOut)
{
	for (auto& pChannel : m_vecChannels)
	{
		auto pClip = pChannel->Get_PlayingAudioClip();

		if (nullptr == pClip)
			continue;

		if (_strTag != pClip->Get_Name())
			continue;

		pChannel->Stop(_bUseFadeOut);
	}
}

_int CSound_Manager::Change_BGM(const string& _strTag, _float _fVolume, _bool _bUseFade)
{
	Stop_BGM(_bUseFade);

	return Play_Audio(_strTag, _fVolume, _bUseFade);
}

_int CSound_Manager::Stop_BGM(_bool _bUseFadeOut)
{
	_int iChannelIndex = -1;

	auto iter = find_if(m_vecChannels.begin(), m_vecChannels.end(),
		[](CAudioChannel* _pChannel) -> _bool
		{
			auto pPlayingClip = _pChannel->Get_PlayingAudioClip();

			if (nullptr == pPlayingClip)
				return false;

			return AUDIO_CLIP_TYPE::BGM == pPlayingClip->Get_Type();
		});

	if (iter != m_vecChannels.end())
	{
		iChannelIndex = (*iter)->Get_Index();
		Stop_Channel(iChannelIndex, _bUseFadeOut);
	}

	return iChannelIndex;
}

void CSound_Manager::FadeOut_BGM_Volume(_float _fVolume)
{
	auto pChannel = _Find_Playing_BGM_Channel();

	if (nullptr == pChannel)
		return;

	pChannel->FadeOut(_fVolume);
}

void CSound_Manager::FadeIn_BGM_Volume(_float _fVolume)
{
	auto pChannel = _Find_Playing_BGM_Channel();

	if (nullptr == pChannel)
		return;

	pChannel->FadeIn(_fVolume);
}

void CSound_Manager::Stop_Channel(size_t _iChannelIndex, _bool _bUseFadeOut)
{
	if (_iChannelIndex >= m_vecChannels.size())
		return;

	m_vecChannels[_iChannelIndex]->Stop(_bUseFadeOut);
}

void CSound_Manager::Stop_AllChannels()
{
	for (auto& pChannel : m_vecChannels)
		pChannel->Stop();
}

void CSound_Manager::Mute_Channel(size_t _iChannelIndex)
{
	if (_iChannelIndex >= m_vecChannels.size())
		return;

	m_vecChannels[_iChannelIndex]->Mute();
}

void CSound_Manager::UnMute_Channel(size_t _iChannelIndex)
{
	if (_iChannelIndex >= m_vecChannels.size())
		return;

	m_vecChannels[_iChannelIndex]->UnMute();
}

void CSound_Manager::Mute_AllChannels()
{
	for (auto& pChannel : m_vecChannels)
		pChannel->Mute();
}

void CSound_Manager::UnMute_AllChannels()
{
	for (auto& pChannel : m_vecChannels)
		pChannel->UnMute();
}

void CSound_Manager::Pause_Channel(size_t _iChannelIndex)
{
	if (_iChannelIndex >= m_vecChannels.size())
		return;

	m_vecChannels[_iChannelIndex]->Pause();
}

void CSound_Manager::Resume_Channel(size_t _iChannelIndex)
{
	if (m_vecChannels.size() <= _iChannelIndex)
		return;

	m_vecChannels[_iChannelIndex]->Resume();
}

void CSound_Manager::Pause_AllChannels()
{
	for (auto& pChannel : m_vecChannels)
		pChannel->Pause();
}

void CSound_Manager::Resume_AllChannels()
{
	for (auto& pChannel : m_vecChannels)
		pChannel->Resume();
}

void CSound_Manager::Remove_AudioClip(const string& _strTag)
{
	auto iter = m_mapAudioClips.find(_strTag);

	if (iter == m_mapAudioClips.end())
		return;

	for (auto& channel : m_vecChannels)
	{
		auto pAudioClip = channel->Get_PlayingAudioClip();

		if (nullptr == pAudioClip)
			continue;

		if (_strTag == pAudioClip->Get_Name())
			channel->Stop();
	}

	m_mapAudioClips.erase(iter);
}

void CSound_Manager::Clear_AudioClips()
{
	Stop_AllChannels();

	for (auto& audio : m_mapAudioClips)
		Safe_Release(audio.second);

	m_mapAudioClips.clear();
}

void CSound_Manager::Set_ChannelVolume(size_t _iChannelIndex, _float _fVolume)
{
	if (m_vecChannels.size() <= _iChannelIndex)
		return;

	m_vecChannels[_iChannelIndex]->Set_Volume(_fVolume);
}

void CSound_Manager::Set_AllChannelsVolume(_float _fVolume)
{
	for (auto& pChannel : m_vecChannels)
		pChannel->Set_Volume(_fVolume);
}

void CSound_Manager::Set_AudioVolume(const string& _strTag, _float _fVolume)
{
	auto iter = m_mapAudioClips.find(_strTag);

	if (iter == m_mapAudioClips.end())
		return;

	iter->second->Set_Volume(_fVolume);
}

CAudioChannel* CSound_Manager::_Find_Playing_BGM_Channel()
{
	auto iter = find_if(m_vecChannels.begin(), m_vecChannels.end(),
		[](CAudioChannel* _pChannel) -> _bool
		{
			auto pPlayingClip = _pChannel->Get_PlayingAudioClip();

			if (nullptr == pPlayingClip)
			{
				return false;
			}

			return AUDIO_CLIP_TYPE::BGM == pPlayingClip->Get_Type();
		});

	if (iter != m_vecChannels.end())
		return *iter;

	return nullptr;
}

void CSound_Manager::Free()
{
	for (auto& pChannel : m_vecChannels)
		Safe_Release(pChannel);

	m_vecChannels.clear();

	for (auto& pairAudioClip : m_mapAudioClips)
		Safe_Release(pairAudioClip.second);

	m_mapAudioClips.clear();

	m_pSystem->release();
	m_pSystem->close();
}