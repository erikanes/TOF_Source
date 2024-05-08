#include "Engine_pch.h"
#include "Channel.h"
#include "Model.h"
#include "Bone.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(const CHANNELDATA& _tChannelData)
{
	m_uBoneIdx = _tChannelData.uBoneIdx;
	m_uNumKeyFrame = _tChannelData.uNumKeyFrame;

	m_vecKeyFrames.reserve(m_uNumKeyFrame);

	for (_uint i = 0; i < m_uNumKeyFrame; ++i)
		m_vecKeyFrames.push_back(_tChannelData.pKeyFrames[i]);

	return S_OK;
}

HRESULT CChannel::Update_TransformationMatrices(_uint* _pCurKeyFrame,
	_float _fTrackPosition,
	_float _fTrackPositionBefore,
	const vector<class CBone*>& _refVecBones,
	function<void(_uint)>& _funcNotify)
{
	/*
	if (0.0f == _fTrackPosition)
		*_pCurKeyFrame = 0;

	_vector			vScale;
	_vector			vRotation;
	_vector			vPosition = { 0.f, 0.f, 0.f, 1.f };
	_vector			vPositionSave = { 0.f, 0.f, 0.f, 1.f };

	_bool			bIsFirstXZRootBone = _refVecBones[m_uBoneIdx]->Is_FirstRootBone();
	_bool			bIsSecondYRootBone = _refVecBones[m_uBoneIdx]->Is_SecondRootBone();

	if (_fTrackPosition >= m_vecKeyFrames.back().fTime)
	{
		vScale = XMLoadFloat4(&m_vecKeyFrames.back().vScale);
		vRotation = XMLoadFloat4(&m_vecKeyFrames.back().vRotation);
		vPosition = XMLoadFloat4(&m_vecKeyFrames.back().vTranslation);
	}
	*/
	else
	{
		while (_fTrackPosition > m_vecKeyFrames[(*_pCurKeyFrame) + 1u].fTime)
		{
			// Run notify
			if (bIsFirstXZRootBone)
				_funcNotify((*_pCurKeyFrame));

			++(*_pCurKeyFrame);
		}

		/*
		_float		fRatio = (_fTrackPosition - m_vecKeyFrames[*_pCurKeyFrame].fTime) /
			(m_vecKeyFrames[(*_pCurKeyFrame) + 1u].fTime - m_vecKeyFrames[*_pCurKeyFrame].fTime);

		_vector		vSrcScale, vDestScale;
		_vector		vSrcRotation, vDestRotation;
		_vector		vSrcTranslation, vDestTranslation;

		vSrcScale = XMLoadFloat4(&m_vecKeyFrames[*_pCurKeyFrame].vScale);
		vSrcRotation = XMLoadFloat4(&m_vecKeyFrames[*_pCurKeyFrame].vRotation);
		vSrcTranslation = XMLoadFloat4(&m_vecKeyFrames[*_pCurKeyFrame].vTranslation);

		vDestScale = XMLoadFloat4(&m_vecKeyFrames[(*_pCurKeyFrame) + 1u].vScale);
		vDestRotation = XMLoadFloat4(&m_vecKeyFrames[(*_pCurKeyFrame) + 1u].vRotation);
		vDestTranslation = XMLoadFloat4(&m_vecKeyFrames[(*_pCurKeyFrame) + 1u].vTranslation);

		vScale = XMVectorLerp(vSrcScale, vDestScale, fRatio);
		vRotation = XMQuaternionSlerp(vSrcRotation, vDestRotation, fRatio);

		vPositionSave = XMVectorLerp(vSrcTranslation, vDestTranslation, fRatio);
		// For save interpolated keyframes in position.

		if (!bIsFirstXZRootBone && !bIsSecondYRootBone)
		{
			vPosition = XMVectorLerp(vSrcTranslation, vDestTranslation, fRatio);
		}
		else if (bIsFirstXZRootBone && !bIsSecondYRootBone)
		{
			_vector vInterPolatedTranslation = XMVectorLerp(vSrcTranslation, vDestTranslation, fRatio);
			if (0.0f == _fTrackPosition || 0.0f == _fTrackPositionBefore)
			{
				_refVecBones[m_uBoneIdx]->Set_Current_XZRootMotion(vInterPolatedTranslation);
				_refVecBones[m_uBoneIdx]->Set_Previous_XZRootMotion(vInterPolatedTranslation);
			}
			else
			{
				_refVecBones[m_uBoneIdx]->Set_Current_XZRootMotion(vInterPolatedTranslation);
			}

		}
		else if (!bIsFirstXZRootBone && bIsSecondYRootBone)
		{
			vPosition = Vector4(0.f, 0.f, 0.f, 1.f);
			vPosition = XMVectorLerp(vSrcTranslation, vDestTranslation, fRatio);
			_float fYInterpolatedPos = XMVectorGetZ(vPosition);

			// Need to check
			if (0.0f == _fTrackPosition || 0.0f == _fTrackPositionBefore)
			{
				_refVecBones[m_uBoneIdx]->Set_Current_YRootMotion(fYInterpolatedPos);
				_refVecBones[m_uBoneIdx]->Set_Previous_YRootMotion(fYInterpolatedPos);
			}
			else
			{
				_refVecBones[m_uBoneIdx]->Set_Current_YRootMotion(fYInterpolatedPos);
			}

			vPosition = XMVectorSetZ(vPosition, 0.f);
			// <- 3dx max에서 Z가 위쪽 방향으로 정해져 있음.
		}

	}

	_matrix			TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);



	_refVecBones[m_uBoneIdx]->Set_Interpolated_Keyframe(vScale, vRotation, vPositionSave);
	_refVecBones[m_uBoneIdx]->Set_TransformationMatrix(TransformationMatrix);

	return S_OK;
}

HRESULT CChannel::Update_TransitionMatricesNoRotation(_uint _uFromKEyFrame, _uint _uToKeyFrame, _float _fTrackPosition, CChannel* _pDstChannel, _float _fTransitionTime,
	const vector<class CBone*>& _refVecBones)
{
	Vector4			vScale = { 1.f, 1.f, 1.f, 0.f };
	Vector4			vRotation = { 0.f, 0.f, 0.f, 0.f };
	Vector4			vPosition = { 0.f, 0.f, 0.f, 1.f };
	Vector4			vPositionSave = { 0.f, 0.f, 0.f, 1.f };

	const KEYFRAME& tSrcKeyFrame = _refVecBones[m_uBoneIdx]->Get_Interpolated_Keyframe();
	const KEYFRAME& tDstKeyFrame = _pDstChannel->m_vecKeyFrames[_uToKeyFrame];

	_bool			bIsFirstXZRootBone = _refVecBones[m_uBoneIdx]->Is_FirstRootBone();
	_bool			bIsSecondYRootBone = _refVecBones[m_uBoneIdx]->Is_SecondRootBone();

	_float fRatio = _fTrackPosition / _fTransitionTime;

	if (fRatio > 1.0f)
		fRatio = 1.0f;

	Vector4		vSourScale, vDestScale;
	Vector4		vDestRotation;
	Vector4		vSourTranslation, vDestTranslation;

	vSourScale = XMLoadFloat4(&tSrcKeyFrame.vScale);
	vSourTranslation = XMLoadFloat4(&tSrcKeyFrame.vTranslation);

	vDestScale = XMLoadFloat4(&tDstKeyFrame.vScale);
	vDestRotation = XMLoadFloat4(&tDstKeyFrame.vRotation);
	vDestTranslation = XMLoadFloat4(&tDstKeyFrame.vTranslation);

	vPositionSave = XMVectorLerp(vSourTranslation, vDestTranslation, fRatio);
	vScale = XMVectorLerp(vSourScale, vDestScale, fRatio);
	vRotation = vDestRotation;
	// Destination rotation의 회전값을 그대로 사용.

	if (!bIsFirstXZRootBone && !bIsSecondYRootBone)
	{
		vPosition = XMVectorLerp(vSourTranslation, vDestTranslation, fRatio);
	}
	else if (bIsFirstXZRootBone && !bIsSecondYRootBone)
	{
		_vector vInterPolatedTranslation = XMVectorLerp(vSourTranslation, vDestTranslation, fRatio);
		_refVecBones[m_uBoneIdx]->Set_Current_XZRootMotion(vInterPolatedTranslation);
		_refVecBones[m_uBoneIdx]->Set_Previous_XZRootMotion(vInterPolatedTranslation);

	}
	else if (!bIsFirstXZRootBone && bIsSecondYRootBone)
	{
		vPosition = XMVectorLerp(vSourTranslation, vDestTranslation, fRatio);

		_float fYInterpolated = XMVectorGetZ(vPosition);

		_refVecBones[m_uBoneIdx]->Set_Current_YRootMotion(fYInterpolated);
		_refVecBones[m_uBoneIdx]->Set_Previous_YRootMotion(fYInterpolated);

		vPosition = XMVectorSetZ(vPosition, 0.f);

	}

	_matrix			TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

	_refVecBones[m_uBoneIdx]->Set_Interpolated_Keyframe(vScale, vRotation, vPositionSave);
	_refVecBones[m_uBoneIdx]->Set_TransformationMatrix(TransformationMatrix);

	return S_OK;
}

HRESULT CChannel::Update_TransformationMatrices(_float _fTrackPosition, CChannel* _pDstChannel, _uint _uToKeyFrame, _float _fTransitionTime,
	const vector<class CBone*>& _refVecBones)
{
	_vector			vScale = { 1.f, 1.f, 1.f, 0.f };
	_vector			vRotation = { 0.f, 0.f, 0.f, 0.f };
	_vector			vPosition = { 0.f, 0.f, 0.f, 1.f };

	_bool			bIsFirstXZRootBone = _refVecBones[m_uBoneIdx]->Is_FirstRootBone();
	_bool			bIsSecondYRootBone = _refVecBones[m_uBoneIdx]->Is_SecondRootBone();

	const KEYFRAME& tSrcKeyFrame = _refVecBones[m_uBoneIdx]->Get_Interpolated_Keyframe();
	const KEYFRAME& tDstKeyFrame = _pDstChannel->m_vecKeyFrames[_uToKeyFrame];

	_float fRatio = _fTrackPosition / _fTransitionTime;

	if (fRatio > 1.0f)
		fRatio = 1.0f;

	_vector		vSourScale, vDestScale;
	_vector		vSourRotation, vDestRotation;
	_vector		vSourTranslation, vDestTranslation;

	vSourScale = XMLoadFloat4(&tSrcKeyFrame.vScale);
	vSourRotation = XMLoadFloat4(&tSrcKeyFrame.vRotation);
	vSourTranslation = XMLoadFloat4(&tSrcKeyFrame.vTranslation);

	vDestScale = XMLoadFloat4(&tDstKeyFrame.vScale);
	vDestRotation = XMLoadFloat4(&tDstKeyFrame.vRotation);
	vDestTranslation = XMLoadFloat4(&tDstKeyFrame.vTranslation);

	vScale = XMVectorLerp(vSourScale, vDestScale, fRatio);
	vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, fRatio);


	if (!bIsFirstXZRootBone && !bIsSecondYRootBone)
	{
		vPosition = XMVectorLerp(vSourTranslation, vDestTranslation, fRatio);
	}
	else if (bIsFirstXZRootBone && !bIsSecondYRootBone)
	{
		_vector vInterPolatedTranslation = XMVectorLerp(vSourTranslation, vDestTranslation, fRatio);
		_refVecBones[m_uBoneIdx]->Set_Current_XZRootMotion(vInterPolatedTranslation);
		_refVecBones[m_uBoneIdx]->Set_Previous_XZRootMotion(vInterPolatedTranslation);

	}
	else if (!bIsFirstXZRootBone && bIsSecondYRootBone)
	{
		vPosition = XMVectorLerp(vSourTranslation, vDestTranslation, fRatio);

		_float fYInterpolated = XMVectorGetZ(vPosition);

		_refVecBones[m_uBoneIdx]->Set_Current_YRootMotion(fYInterpolated);
		_refVecBones[m_uBoneIdx]->Set_Previous_YRootMotion(fYInterpolated);

		vPosition = XMVectorSetZ(vPosition, 0.f);

	}

	_matrix			TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
	_refVecBones[m_uBoneIdx]->Set_TransformationMatrix(TransformationMatrix);

	return S_OK;
}

HRESULT CChannel::Update_TransformationMatrices(_uint _uFromKEyFrame, _uint _uToKeyFrame, _float _fTrackPosition,
	CChannel* _pDstChannel, _float _fTransitionTime, const vector<class CBone*>& _refVecBones)
{
	_vector			vScale;
	_vector			vRotation;
	_vector			vPosition = { 0.f, 0.f, 0.f, 1.f };

	const KEYFRAME& tSrcKeyFrame = m_vecKeyFrames[_uFromKEyFrame];
	const KEYFRAME& tDstKeyFrame = _pDstChannel->m_vecKeyFrames[_uToKeyFrame];

	_bool			bIsFirstXZRootBone = _refVecBones[m_uBoneIdx]->Is_FirstRootBone();
	_bool			bIsSecondYRootBone = _refVecBones[m_uBoneIdx]->Is_SecondRootBone();

	_float fRatio = _fTrackPosition / _fTransitionTime;

	if (fRatio > 1.0f)
		fRatio = 1.0f;

	_vector		vSourScale, vDestScale;
	_vector		vSourRotation, vDestRotation;
	_vector		vSourTranslation, vDestTranslation;

	vSourScale = XMLoadFloat4(&tSrcKeyFrame.vScale);
	vSourRotation = XMLoadFloat4(&tSrcKeyFrame.vRotation);
	vSourTranslation = XMLoadFloat4(&tSrcKeyFrame.vTranslation);

	vDestScale = XMLoadFloat4(&tDstKeyFrame.vScale);
	vDestRotation = XMLoadFloat4(&tDstKeyFrame.vRotation);
	vDestTranslation = XMLoadFloat4(&tDstKeyFrame.vTranslation);

	vScale = XMVectorLerp(vSourScale, vDestScale, fRatio);
	vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, fRatio);


	if (!bIsFirstXZRootBone && !bIsSecondYRootBone)
	{
		vPosition = XMVectorLerp(vSourTranslation, vDestTranslation, fRatio);
	}
	else if (bIsFirstXZRootBone && !bIsSecondYRootBone)
	{
		_vector vInterPolatedTranslation = XMVectorLerp(vSourTranslation, vDestTranslation, fRatio);
		_refVecBones[m_uBoneIdx]->Set_Current_XZRootMotion(vInterPolatedTranslation);
		_refVecBones[m_uBoneIdx]->Set_Previous_XZRootMotion(vInterPolatedTranslation);

	}
	else if (!bIsFirstXZRootBone && bIsSecondYRootBone)
	{
		vPosition = XMVectorLerp(vSourTranslation, vDestTranslation, fRatio);

		_float fYInterpolated = XMVectorGetZ(vPosition);

		_refVecBones[m_uBoneIdx]->Set_Current_YRootMotion(fYInterpolated);
		_refVecBones[m_uBoneIdx]->Set_Previous_YRootMotion(fYInterpolated);

		vPosition = XMVectorSetZ(vPosition, 0.f);

	}

	_matrix			TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

	_refVecBones[m_uBoneIdx]->Set_TransformationMatrix(TransformationMatrix);

	return S_OK;
}

CChannel* CChannel::Create(const CHANNELDATA& _tChannelData)
{
	CChannel* pInstance = new CChannel;

	if (FAILED(pInstance->Initialize(_tChannelData)))
	{
		MSG_BOX("Failed to create : CChannel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChannel::Update_KeyFrameIdx(_float _fTrackPosition, _uint* _pCurKeyFrame)
{
	if (1 == m_vecKeyFrames.size())
	{
		*_pCurKeyFrame = 0;
		return;
	}

	*_pCurKeyFrame = 0u;

	for (_uint i = 1; i < m_vecKeyFrames.size(); ++i)
	{
		if (m_vecKeyFrames[i].fTime > _fTrackPosition)
		{
			*_pCurKeyFrame = i - 1;
			return;
		}
	}

	*_pCurKeyFrame = (_int)m_vecKeyFrames.size() - 1;
	return;
}

void CChannel::Free()
{
	m_vecKeyFrames.clear();
}
*/