#include "Engine_pch.h"
#include "AudioClip.h"

CAudioClip* CAudioClip::Create(FMOD::Sound* _pSound, const string& _strName, AUDIO_CLIP_TYPE _eType, _float _fVolume, FMOD_MODE _eMode)
{
	auto pInstance = new CAudioClip;

	if (FAILED(pInstance->_Initialize(_pSound, _strName, _eType, _fVolume, _eMode)))
	{
		MSG_BOX("Failed to Create: CAudioClip");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAudioClip::Free()
{
	if (nullptr != m_pSound)
		m_pSound->release();
}

HRESULT CAudioClip::_Initialize(FMOD::Sound* _pSound, const string& _strName, AUDIO_CLIP_TYPE _eType, _float _fVolume, FMOD_MODE _eMode)
{
	m_pSound = _pSound;
	m_strName = _strName;
	m_eType = _eType;
	m_fVolume = _fVolume;
	m_eMode = _eMode;

	return S_OK;
}
