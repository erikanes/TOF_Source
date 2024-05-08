#pragma once

#include "Base.h"

BEGIN(Engine)

class CAudioClip final : public CBase
{
private:
	CAudioClip() = default;
	virtual ~CAudioClip() = default;

public:
	void Add_UseCount() { ++m_iUseCount; }
	void Subtract_UseCount() { if (0 < m_iUseCount) --m_iUseCount; }

#pragma region Getter
	FMOD_MODE Get_Mode() const { return m_eMode; }
	_float Get_Volume() const { return m_fVolume; }
	const string& Get_Name() const { return m_strName; }
	FMOD::Sound* const Get_Sound() const { return m_pSound; }
	_uint Get_UseCount() const { return m_iUseCount; }
	AUDIO_CLIP_TYPE Get_Type() const { return m_eType; }
#pragma endregion

#pragma region Setter
	void Set_Volume(_float _fVolume) { m_fVolume = _fVolume; }
#pragma endregion

public:
	static CAudioClip* Create(FMOD::Sound* _pSound, const string& _strName, AUDIO_CLIP_TYPE _eType, _float _fVolume, FMOD_MODE _eMode);
	virtual void Free() override;

private:
	HRESULT _Initialize(FMOD::Sound* _pSound, const string& _strName, AUDIO_CLIP_TYPE _eType, _float _fVolume, FMOD_MODE _eMode);

private:
	FMOD::Sound* m_pSound = { nullptr };

	string m_strName;

	_uint m_iUseCount = { 0 };

	_float m_fVolume = { 1.f };
	_float m_fMinDistance = { 1.f };
	_float m_fMaxDistance = { 10.f };

	FMOD_MODE m_eMode = { FMOD_LOOP_OFF };
	AUDIO_CLIP_TYPE m_eType = { AUDIO_CLIP_TYPE::SFX };
};

END