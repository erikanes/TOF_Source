#pragma once

#include "Base.h"

BEGIN(Engine)

class CAudioClip;

class CAudioChannel final : public CBase
{
private:
	enum class AudioFadeType
	{
		FADE_IN, FADE_OUT, FADE_NONE
	};

private:
	CAudioChannel() = default;
	virtual ~CAudioChannel() = default;

public:
	void Update();

	void Play(CAudioClip* _pAudioClip, _float _fVolume = -1.f, _bool _bUseFadeIn = false);
	void Pause();
	void Resume();
	void Stop(_bool _bUseFadeOut = false);
	void Mute();
	void UnMute();

	void FadeIn(_float _fVolume);
	void FadeOut(_float _fVolume);

	void Reset();

#pragma region Getter
	_bool Is_Playing() const { return m_bIsPlaying; }
	_bool Is_Paused() const { return m_bIsPaused; }
	_bool Is_Muted() const { return m_bIsMuted; }
	_float Get_Volume() const { return m_fVolume; }
	const CAudioClip* Get_PlayingAudioClip() const { return m_pPlayingClip; }
	_uint Get_Index() { return m_iChannelIndex; }
#pragma endregion

#pragma region Setter
	void Set_Volume(_float _fVolume) { m_fVolume = _fVolume; }
#pragma endregion

public:
	static CAudioChannel* Create(FMOD::System* _pSystem, _uint _iChannelIndex, function<void(_uint)> _func);
	virtual void Free() override;

private:
	HRESULT _Initialize(FMOD::System* _pSystem, _uint _iChannelIndex, function<void(_uint)> _func);

private:
	FMOD::System*			m_pSystem = { nullptr };
	FMOD::Channel*			m_pChannel = { nullptr };

	CAudioClip*				m_pPlayingClip = { nullptr };

	_bool					m_bIsPlaying = { false };
	_bool					m_bIsPaused = { false };
	_bool					m_bIsMuted = { false };

	_uint					m_iChannelIndex = { 0 };

	_float					m_fVolume = { 1.f };
	_float					m_fInputVolume = { 1.f };
	_float					m_fVolumeRate = { 1.f };

	AudioFadeType			m_eFadeType = AudioFadeType::FADE_NONE;

	function<void(_uint)>	m_funcEmptySetter;
	function<void()>		m_funcVolumeCalculator;
};

END