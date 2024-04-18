#include "Engine_pch.h"
#include "AudioChannel.h"

#include "AudioClip.h"

void CAudioChannel::Update()
{
	m_pChannel->isPlaying(&m_bIsPlaying);

	if (!m_bIsPlaying && !m_bIsPaused)
	{
		if (nullptr != m_pPlayingClip)
		{
			m_funcEmptySetter(m_iChannelIndex);
			m_pPlayingClip->Subtract_UseCount();
			
			Safe_Release(m_pPlayingClip);
			m_pPlayingClip = nullptr;
		}
	}

	m_funcVolumeCalculator();
}

void CAudioChannel::Play(CAudioClip* _pAudioClip, _float _fVolume, _bool _bUseFadeIn)
{
	if (nullptr == _pAudioClip)
		return;

	m_pPlayingClip = _pAudioClip;
	Safe_AddRef(m_pPlayingClip);

	m_pSystem->playSound(m_pPlayingClip->Get_Sound(), nullptr, false, &m_pChannel);
	m_pChannel->setMode(m_pPlayingClip->Get_Mode());

	auto iClipUseCount = m_pPlayingClip->Get_UseCount();

	_float fVolumeRate = iClipUseCount >= 1 ? pow(0.9f, static_cast<_float>(iClipUseCount)) : 1.f;

	if (0.f > _fVolume)
		m_fInputVolume = m_pPlayingClip->Get_Volume() * fVolumeRate;
	else
		m_fInputVolume = _fVolume * fVolumeRate;

	m_pChannel->setVolume(m_fVolume * m_fInputVolume);

	if (_bUseFadeIn)
	{
		m_fVolumeRate = 0.f;

		m_funcVolumeCalculator = [this]() mutable
		{
			m_fVolumeRate = clamp(m_fVolumeRate + 0.01667f, 0.f, 1.f);

			m_pChannel->setVolume(m_fVolume * m_fInputVolume * m_fVolumeRate);

			if (1.f <= m_fVolumeRate)
				m_funcVolumeCalculator = [](){};
		};
	}

	m_pPlayingClip->Add_UseCount();
}

void CAudioChannel::Pause()
{
	m_bIsPaused = true;
	m_pChannel->setPaused(true);
}

void CAudioChannel::Resume()
{
	m_bIsPaused = false;
	m_pChannel->setPaused(false);
}

void CAudioChannel::Stop(_bool _bUseFadeOut)
{
	if (nullptr == m_pChannel)
		return;

	if (_bUseFadeOut)
	{
		m_fVolumeRate = 1.f;

		m_funcVolumeCalculator = [this]() mutable
		{
			m_fVolumeRate = clamp(m_fVolumeRate - 0.01667f, 0.f, 1.f);

			m_pChannel->setVolume(m_fVolume * m_fInputVolume * m_fVolumeRate);

			if (0.f >= m_fVolumeRate)
				this->Stop();
		};
	}

	else
	{
		m_bIsPaused = false;
		m_bIsPlaying = false;

		m_pChannel->stop();
		m_pChannel = nullptr;

		m_funcVolumeCalculator = [](){};
	}
}

void CAudioChannel::Mute()
{
	m_bIsMuted = true;
	m_pChannel->setMute(true);
}

void CAudioChannel::UnMute()
{
	m_bIsMuted = false;
	m_pChannel->setMute(false);
}

void CAudioChannel::FadeIn(_float _fVolume)
{
	if (nullptr == m_pChannel)
		return;

	m_funcVolumeCalculator = [this, fDestVolume = _fVolume] ()
	{
		m_fVolumeRate = clamp(m_fVolumeRate + 0.01667f, 0.f, fDestVolume);

		m_pChannel->setVolume(m_fVolume * m_fInputVolume * m_fVolumeRate);

		if (fDestVolume <= m_fVolumeRate)
			m_funcVolumeCalculator = [](){};
	};
}

void CAudioChannel::FadeOut(_float _fVolume)
{
	if (nullptr == m_pChannel)
		return;

	m_funcVolumeCalculator = [this, fDestVolume = _fVolume] ()
	{
		m_fVolumeRate = clamp(m_fVolumeRate - 0.01667f, fDestVolume, 1.f);

		m_pChannel->setVolume(m_fVolume * m_fInputVolume * m_fVolumeRate);

		if (fDestVolume >= m_fVolumeRate)
			m_funcVolumeCalculator = [](){};
	};
}

void CAudioChannel::Reset()
{
	if (nullptr != m_pChannel)
		m_pChannel->stop();

	if (nullptr != m_pPlayingClip)
	{
		m_pPlayingClip->Subtract_UseCount();
		Safe_Release(m_pPlayingClip);
		m_pPlayingClip = nullptr;
	}

	m_pChannel = nullptr;
	m_bIsPlaying = false;
	m_bIsPaused = false;
	m_bIsMuted = false;
	m_fVolume = 1.f;
	m_fInputVolume = 1.f;
	m_fVolumeRate = 1.f;
}

CAudioChannel* CAudioChannel::Create(FMOD::System* _pSystem, _uint _iChannelIndex, function<void(_uint)> _func)
{
	auto pInstance = new CAudioChannel;

	if (FAILED(pInstance->_Initialize(_pSystem, _iChannelIndex, _func)))
	{
		MSG_BOX("Failed to Create: CAudioChannel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAudioChannel::Free()
{
	if (nullptr != m_pChannel)
		m_pChannel->stop();

	Safe_Release(m_pPlayingClip);

	m_pSystem = nullptr;
	m_pChannel = nullptr;

	m_bIsPlaying = false;
	m_bIsPaused = false;
	m_bIsMuted = false;
}

HRESULT CAudioChannel::_Initialize(FMOD::System* _pSystem, _uint _iChannelIndex, function<void(_uint)> _func)
{
	m_pSystem = _pSystem;
	m_iChannelIndex = _iChannelIndex;
	m_funcEmptySetter = _func;
	m_pPlayingClip = nullptr;

	m_pChannel = nullptr;

	m_funcVolumeCalculator = [](){};

	return S_OK;
}