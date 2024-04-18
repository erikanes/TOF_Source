#pragma once

#include "Base.h"

BEGIN(Engine)

class CAudioClip;
class CAudioChannel;

class CSound_Manager final : public CBase
{
	DECLARE_SINGLETON(CSound_Manager);

private:
	CSound_Manager() = default;
	~CSound_Manager() = default;

public:
	HRESULT Initialize();
	void Update();

	
	HRESULT Add_AudioClip(const string& _strFilePath, const string& _strTag, AUDIO_CLIP_TYPE _eClipType, _float _fVolume, _bool _bIsLoop);
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
	void UnMute_Channel(size_t _iChannelIndex);
	void Mute_AllChannels();
	void UnMute_AllChannels();

	void Pause_Channel(size_t _iChannelIndex);
	void Resume_Channel(size_t _iChannelIndex);
	void Pause_AllChannels();
	void Resume_AllChannels();

	void Remove_AudioClip(const string& _strTag);
	void Clear_AudioClips();

#pragma region Getter
	const queue<_uint>& Get_EmptyChannels() const { return m_emptyChannels; }
	const vector<CAudioChannel*>& Get_Channels() const { return m_vecChannels; }
	const unordered_map<string, CAudioClip*>& Get_AudioClips() const { return m_mapAudioClips; }
	string Get_CurrentPlaying_BGMName() const { return ""; }
#pragma endregion

#pragma region Setter
	void Set_ChannelVolume(size_t _iChannelIndex, _float _fVolume);
	void Set_AllChannelsVolume(_float _fVolume);
	void Set_AudioVolume(const string& _strTag, _float _fVolume);
#pragma endregion

private:
	CAudioChannel* _Find_Playing_BGM_Channel();

public:
	virtual void Free();

private:
	FMOD::System*		m_pSystem{ nullptr };
	unordered_map<string, CAudioClip*> m_mapAudioClips;
	vector<CAudioChannel*> m_vecChannels;

	queue<_uint> m_emptyChannels;

	static constexpr const _uint MAX_CHANNEL_SIZE = { 32 };
};
END
