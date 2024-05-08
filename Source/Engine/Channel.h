#pragma once

#include "Base.h"

BEGIN(Engine)
class CChannel final
	: public CBase
{
private:
	CChannel();
	~CChannel() = default;

public:
	HRESULT Initialize(const CHANNELDATA& _tChannelData);
	HRESULT Update_TransformationMatrices(_uint* _pCurKeyFrame, _float _fTrackPosition,
		_float _fTrackPositionBefore, const vector<class CBone*>& _refVecBones, function<void(_uint)>& _funcNotify);

	HRESULT Update_TransformationMatrices(_float _fTrackPosition, CChannel* _pDstChannel, _uint _uToKeyFrame, _float _fTransitionTime,
		const vector<class CBone*>& _refVecBones);

	HRESULT Update_TransformationMatrices(_uint _uFromKEyFrame, _uint _uToKeyFrame, _float _fTrackPosition, CChannel* _pDstChannel, _float _fTransitionTime,
		const vector<class CBone*>& _refVecBones);

	//Rotation 애니메이션이 있는 경우 사용함.
	HRESULT Update_TransitionMatricesNoRotation(_uint _uFromKEyFrame, _uint _uToKeyFrame, _float _fTrackPosition, CChannel* _pDstChannel, _float _fTransitionTime,
		const vector<class CBone*>& _refVecBones);

public:
	static CChannel* Create(const CHANNELDATA& _tChannelData);
	virtual void Free() override;

private:
	_uint				m_uNumKeyFrame = { 0 };
	_uint				m_uBoneIdx = { 0 };
	vector<KEYFRAME>	m_vecKeyFrames;
	

public:
	_uint	GetBoneIdx() { return m_uBoneIdx; }
	void	Update_KeyFrameIdx(_float _fTrackPosition, _uint* _pCurKeyFrame);
};

END