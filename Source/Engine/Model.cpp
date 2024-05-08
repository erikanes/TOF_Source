#include "Engine_pch.h"
#include "Model.h"
#include "Bone.h"
#include "Mesh.h"
#include "Animator.h"
#include "Material.h"
#include "Animation.h"
#include "GameInstance.h"

#include "Json_Manager.h"

#include "CustomImporter.h"

#include "Engine_Functions.h"

/*
CModel::CModel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CComponent(_pDevice, _pContext)
{
	XMStoreFloat4x4(&m_matPivot, XMMatrixIdentity());
}

CModel::CModel(const CModel& _rhs)
	: CComponent(_rhs)
	, m_iNumMesh(_rhs.m_iNumMesh)
	, m_iNumMtrl(_rhs.m_iNumMtrl)
	, m_iNumAnim(_rhs.m_iNumAnim)
	, m_eModelType(_rhs.m_eModelType)
	, m_matPivot(_rhs.m_matPivot)
	, m_vMeshes(_rhs.m_vMeshes)
	, m_vMtrl(_rhs.m_vMtrl)
	, m_szRootMotionBoneName(_rhs.m_szRootMotionBoneName)
	, m_iRootMotionBoneIdx(_rhs.m_iRootMotionBoneIdx)
{
	for (_uint i = 0; i < m_iNumMesh; ++i)
		Safe_AddRef(m_vMeshes[i]);

	for (_uint i = 0; i < m_iNumMtrl; ++i)
		Safe_AddRef(m_vMtrl[i]);

	for (_uint i = 0; i < m_vBones.size(); ++i)
		Safe_AddRef(m_vBones[i]);

	for (auto& pOriginalBone : _rhs.m_vBones)
	{
		m_vBones.push_back(pOriginalBone->Clone());
	}

	for (auto pair : _rhs.m_mapBones)
	{
		m_mapBones.insert(pair);
	}

	for (auto& pair : _rhs.m_mapNotifications)
	{
		m_mapNotifications.insert({ pair.first, pair.second });
	}

	if (0 < _rhs.m_vAnimations.size())
		m_pAnimator = CAnimator::Create((_uint)_rhs.m_vAnimations.size(), this);

	for (auto& pOriginalAnim : _rhs.m_vAnimations)
	{
		CAnimation* pCloneAnim = pOriginalAnim->Clone();
		pCloneAnim->Set_Owner(m_pAnimator);
		m_vAnimations.push_back(pCloneAnim);

		m_pAnimator->Add_Animation(pCloneAnim);
	}

	for (auto& pair : _rhs.m_mapAnim)
		m_mapAnim.insert(pair);
}

HRESULT CModel::Render(_uint _uMeshIdx)
{
	if (m_iNumMesh <= _uMeshIdx)
		return E_FAIL;

	if (nullptr != m_vMeshes[_uMeshIdx])
		m_vMeshes[_uMeshIdx]->Render();


	return S_OK;
}

HRESULT CModel::Initialize_Prototype(const char* _pFilePath, const char* _pNotificationPath)
{
	m_pCustomScene = CCustomImporter::Create(_pFilePath);

	if (nullptr == m_pCustomScene)
		return E_FAIL;

	const MODELDATA& tModelData = m_pCustomScene->Get_ModelData();
	m_eModelType = (MODELTYPE)tModelData.uModelType;

	if (FAILED(Ready_Meshes(tModelData)))
	{
		m_iNumMesh = 0;
		return E_FAIL;
	}

	if (FAILED(Ready_Materials(tModelData, _pFilePath)))
	{
		m_iNumMtrl = 0;
		return E_FAIL;
	}

	if (FAILED(Ready_Bones(tModelData)))
	{
		return E_FAIL;
	}

	if (FAILED(Ready_Animations(tModelData)))
	{
		m_iNumAnim = 0;
		return E_FAIL;
	}

	if (FAILED(Ready_Notifications(_pNotificationPath)))
	{
		return E_FAIL;
	}


	Safe_Release(m_pCustomScene);

	return S_OK;
}


HRESULT CModel::Initialize(void* _pArg)
{
	if (FAILED(__super::Initialize(_pArg)))
	{
		return E_FAIL;
	}

	if (nullptr != _pArg && FAILED(Register_Notification_Functions(_pArg)))
	{
		MSG_BOX("Failed to register notification function");
		return E_FAIL;
	}

	return S_OK;
}
*/

void CModel::Add_EffectFunc(_int _iTrackPosition, string _szTag, string _szAnimName, function<void()> _funcCreateEffect)
{
	auto iter = m_mapNotifications.find(_iTrackPosition);
	NOTIFICATIONMSG tMsg;

	_int iAnimIdx = 0;
	for (auto& pAnim : m_vAnimations)
	{
		if (_szAnimName == pAnim->Get_Name())

			break;
		else
			++iAnimIdx;
	}

	m_vAnimations[iAnimIdx]->Add_Notify(_iTrackPosition, (_uint)NOTIFICATION_TYPE::EFFECT, _funcCreateEffect);
}

HRESULT CModel::Register_Notification_Functions(void* _pArg)
{
	MODELDESC* pDesc = (MODELDESC*)_pArg;
	m_pOwner = pDesc->pOwner;
	auto jsonFile = CJson_Manager::Get_Instance()->Get_Json(pDesc->jsonTag);

	for (auto& pair : m_mapNotifications)
	{
		auto iterAnim = m_vAnimations[pair.first];
		auto& iterList = pair.second;

		for (auto& tMsg : iterList)
		{
			if (NOTIFICATION_TYPE::SOUND == tMsg.eNotificationType)
			{
				function<void()> funcSound = CFunc::Create_Notify_Sound_Play(m_pOwner, m_pAnimator, tMsg.tNotificationSoundData.szTag);
				iterAnim->Add_Notify(tMsg.iTrackPosition, (_int)tMsg.eNotificationType, funcSound);
			}
			else if (NOTIFICATION_TYPE::COLLISION == tMsg.eNotificationType)
			{
				// Zero is off
				if (0 == tMsg.tNotificationCollisionData.iOperation)
				{
					function<void()> funcCollisionOff = CFunc::Create_Notify_Collider_Off(m_pOwner, m_pAnimator, tMsg.tNotificationCollisionData.szTag);
					iterAnim->Add_Notify(tMsg.iTrackPosition, (_int)tMsg.eNotificationType, funcCollisionOff);
				}
				else
				{
					function<void()> funcCollisionOn = CFunc::Create_Notify_Collider_On(m_pOwner, m_pAnimator, tMsg.tNotificationCollisionData.szTag);
					iterAnim->Add_Notify(tMsg.iTrackPosition, (_int)tMsg.eNotificationType, funcCollisionOn);
				}

			}
			else if (NOTIFICATION_TYPE::EFFECT == tMsg.eNotificationType)
			{
				function<void()> functionEffect = CFunc::Create_Notify_Effect_Create(m_pOwner, m_pAnimator, "", nullptr);
				iterAnim->Add_Notify(tMsg.iTrackPosition, (_int)tMsg.eNotificationType, functionEffect);
			}
			else if (NOTIFICATION_TYPE::OBJECT_CREATE == tMsg.eNotificationType)
			{

			}
			else if (NOTIFICATION_TYPE::COLLISION_TRANSFORM_CHANGE == tMsg.eNotificationType)
			{

			}
			else if (NOTIFICATION_TYPE::CUSTOM_EVENT == tMsg.eNotificationType)
			{
				function<void()> functionCustomEvent = CFunc::Create_Notify_GameObject_Event(m_pOwner, tMsg.tNotificationCustomEventData.iIndex, nullptr);
				iterAnim->Add_Notify(tMsg.iTrackPosition, (_int)tMsg.eNotificationType, functionCustomEvent);
			}
		}
	}

	return S_OK;
}

HRESULT CModel::Ready_Notifications(const char* _szPath)
{
	/*
		Model prototype 생성할 때, notification message 미리 만들기
		Clone 될 때, 직접 함수 등록할 거임.
	*/

	if (MODELTYPE::ANIM_TYPE != m_eModelType || nullptr == _szPath)
		return S_OK;

	NOTIFICATIONMSG tMsg;

	nlohmann::ordered_json Loadfile;
	CJson_Manager::Parse(_szPath, Loadfile);

	for (auto iter = Loadfile.begin(); iter != Loadfile.end(); ++iter)
	{
		auto& strName = iter.key();
		auto iterAnim = m_mapAnim.find(strName);

		if (iterAnim == m_mapAnim.end())
			return E_FAIL;

		_int iAnimIdx = iterAnim->second;
		auto iterList = m_mapNotifications.find(iAnimIdx);

		if (m_mapNotifications.end() == iterList)
		{
			m_mapNotifications.insert({ iAnimIdx, list<NOTIFICATIONMSG>() });
			iterList = m_mapNotifications.find(iAnimIdx);
		}

		for (auto keyFrameIter = iter.value().begin(); keyFrameIter != iter.value().end(); ++keyFrameIter)
		{
			_uint iKeyFrame = stoi(keyFrameIter.key());

			for (auto& evt : keyFrameIter.value())
			{
				auto eType = static_cast<NOTIFICATION_TYPE>(evt["type"].get<_uint>());

				NOTIFICATIONMSG tMsg;
				tMsg.iTrackPosition = iKeyFrame;
				tMsg.eNotificationType = eType;

				if (NOTIFICATION_TYPE::SOUND == eType)
				{
					strcpy_s(tMsg.tNotificationSoundData.szTag, MAX_PATH, evt["audiotag"].get<string>().c_str());
					tMsg.tNotificationSoundData.iDimension = evt["dimension"].get<_uint>();
				}

				else if (NOTIFICATION_TYPE::EFFECT == eType)
				{
					strcpy_s(tMsg.tNotificationEffectData.szTag, MAX_PATH, evt["effecttag"].get<string>().c_str());
					tMsg.tNotificationEffectData.eBatchType = static_cast<EFFECT_BATCH_TYPE>(evt["dimension"].get<_uint>());
				}
				else if (NOTIFICATION_TYPE::COLLISION == eType)
				{
					strcpy_s(tMsg.tNotificationCollisionData.szTag, MAX_PATH, evt["collisiontag"].get<string>().c_str());
					tMsg.tNotificationCollisionData.iOperation = evt["operation"].get<_uint>();
				}
				else if (NOTIFICATION_TYPE::COLLISION_TRANSFORM_CHANGE == eType)
				{
				}
				else if (NOTIFICATION_TYPE::OBJECT_CREATE == eType)
				{
				}
				else if (NOTIFICATION_TYPE::CUSTOM_EVENT == eType)
				{
					strcpy_s(tMsg.tNotificationCustomEventData.szTag, MAX_PATH, evt["customeventtag"].get<string>().c_str());
					tMsg.tNotificationCustomEventData.iIndex = evt["customeventindex"].get<_int>();
				}

				iterList->second.push_back(tMsg);
			}
		}
	}

	return S_OK;
}

/*
CModel* CModel::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const char* _pFilePath, const char* _pNotificationPath)
{
	CModel* pInstance = new CModel(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype(_pFilePath, _pNotificationPath)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to create : CModel");
	}
	return pInstance;
}

HRESULT CModel::Ready_Meshes(const MODELDATA& _tModelData)
{
	m_iNumMesh = _tModelData.uMeshNums;

	for (_uint i = 0; i < m_iNumMesh; ++i)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, this, _tModelData.pMeshes[i], XMMatrixIdentity());

		m_vMeshes.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel::Ready_Bones(const MODELDATA& _tModelData)
{
	_uint uNumBones = _tModelData.uBoneNums;

	m_vBones.reserve(uNumBones);
	for (_uint i = 0; i < uNumBones; ++i)
	{
		CBone* pBone = CBone::Create(_tModelData.pBones[i]);

		if (nullptr == pBone)
			return E_FAIL;

		if (m_szRootMotionBoneName == pBone->Get_Name())
			m_iRootMotionBoneIdx = i;

		m_vBones.push_back(pBone);
		m_mapBones.insert({ pBone->Get_Name(), i });
	}
	return S_OK;
}

HRESULT CModel::Ready_Animations(const MODELDATA& _tModelData)
{
	m_iNumAnim = _tModelData.uAnimNums;

	for (_uint i = 0; i < m_iNumAnim; ++i)
	{
		CAnimation* pAnim = CAnimation::Create(_tModelData.pAnims[i]);

		if (nullptr == pAnim)
			return E_FAIL;

		m_vAnimations.push_back(pAnim);
		m_mapAnim.insert({ pAnim->Get_Name(), i });
	}

	return S_OK;
}

HRESULT CModel::Bind_ShaderResourceView(CShader* _pShader, _uint _iMeshIdx)
{
	if (m_iNumMesh <= _iMeshIdx)
		return E_FAIL;

	_uint iMtrlIdx = m_vMeshes[_iMeshIdx]->GetIndexMtrl();

	if (m_iNumMtrl <= iMtrlIdx)
		return E_FAIL;

	return m_vMtrl[iMtrlIdx]->Bind_ShaderResources(_pShader);
}

HRESULT CModel::Bind_ShaderBoneMatricies(CShader* _pShader, _uint _uMeshIdx, const char* _szConstantName)
{
	if (m_iNumMesh <= _uMeshIdx)
		return E_FAIL;

	m_vMeshes[_uMeshIdx]->Get_BoneMatricies(m_boneMatricies, m_vBones, m_matPivot);

	return _pShader->Bind_Matricies(_szConstantName, m_boneMatricies, 256);
}


HRESULT CModel::Ready_Materials(const MODELDATA& _tModelData, const char* _szPath)
{
	char szDir[MAX_PATH] = "";
	_splitpath_s(_szPath, nullptr, 0, szDir, MAX_PATH, nullptr, 0, nullptr, 0);

	m_iNumMtrl = _tModelData.uMtrlNums;
	m_vMtrl.reserve(m_iNumMtrl);

	// 현재 구현된 내용
	// 1. 저장해둔 머티리얼 개수만큼 머티리얼 생성
	// 2. 머티리얼 안에 텍스쳐가 존재하면 해당 텍스쳐를 불러들여서 머티리얼에 추가. 보통은 1장임
	// 3. 텍스쳐 타입이 어떤 것인지, 이름, 경로를 같이 넘겨준다

	for (_uint i = 0; i < m_iNumMtrl; ++i)
	{
		CMaterial* pMtrl = (CMaterial*)CMaterial::Create(m_pDevice, m_pContext);
		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
		{
			_uint szTexNameLength = { 0 };
			char szTexName[MAX_PATH] = { "" };

			strcpy_s(szTexName, _tModelData.pMaterials[i].szTexName[j]);
			TEXTUREMTRLINFO tMtrlTex;
			ZeroMemory(&tMtrlTex, sizeof(TEXTUREMTRLINFO));

			if (0 != strcmp("", szTexName))
			{
				char szTexFullPath[MAX_PATH] = { "" };
				sprintf_s(szTexFullPath, "%s%s", szDir, szTexName);
				string strTexFullPath = szTexFullPath;
				wstring wstrTexFullPath;
				wstrTexFullPath.assign(strTexFullPath.begin(), strTexFullPath.end());

				CTexture* pTexture = CTexture::Create(m_pDevice, m_pContext, wstrTexFullPath.c_str());

				if(nullptr != pTexture)
					pMtrl->Add_Texture(aiTextureType(j), szTexName, szTexFullPath, pTexture);
			}
		}

		m_vMtrl.push_back(pMtrl);
	}

	return S_OK;
}

string CModel::Get_szMeshName(_uint _uIdx)
{
	if (m_iNumMesh <= _uIdx)
		return string("");
	else
		return m_vMeshes[_uIdx]->GetMeshName();
}

_uint CModel::Get_BoneIndex(string _szBoneName)
{
	_uint uIdx = { 0 };
	_uint iNumBones = (_uint)m_vBones.size();

	for (_uint i = 0; i < iNumBones; ++i)
	{
		if (m_vBones[i]->Get_Name() == _szBoneName)
		{
			return i;
		}
	}

	return UINT32_MAX;
}

string CModel::Get_BoneName(_int _iBoneIdx)
{
	return m_vBones[_iBoneIdx]->Get_Name();
}

void CModel::Change_Animation(_uint _uNextAnim)
{
	m_pAnimator->Change_Animation(_uNextAnim);
}

void CModel::Change_Animation(const char* _pszNextAnim)
{
	m_pAnimator->Change_Animation(_pszNextAnim);
}

void CModel::Change_Animation(AnimState _eAnimState)
{
	m_pAnimator->Change_Animation(_eAnimState);
}

HRESULT CModel::Play_Animation(_float _fTimeDelta)
{
	if (nullptr != m_pAnimator)
	{
		m_pAnimator->Tick(_fTimeDelta, m_vBones);
	}


	for (auto pBone : m_vBones)
	{
		pBone->Update_CombinedTransformationMatrix(m_vBones);
	}
	//// 특정 뼈의 애니메이션을 재생하고 나면, 해당 뼈의 transformation이 변경됨.
	//// 근데, 그 계층 관계 속에서 어떤 뼈들이 영향이 갔는지 확인하기 어려움.
	//// 즉 해당 뼈가 변경되면, 하위 관계에 있던 모든 뼈들이 바뀌게 됨.
	//// 그래서 전체 탐색하면서 뼈의 상태를 바꿔줘야 함.




	return S_OK;
}

_bool CModel::Get_IntersectMesh(_matrix _matWorld, _float3& _vOutPos, _float& _fOutDist)
{
	for (_uint i = 0; i < m_iNumMesh; ++i)
	{
		_bool bResult = m_vMeshes[i]->Get_IntersectPolygonPos(_matWorld, _vOutPos, _fOutDist);

		if (bResult)
			return true;
	}
	return false;
}


_float CModel::Get_FirstRootBone_XZDelta()
{
	_int iXZRootBoneIdx = m_pAnimator->Get_XZRootBoneIdx();

	if (0 > iXZRootBoneIdx || m_vBones.size() <= iXZRootBoneIdx)
	{
		return 0.f;
	}
	else
	{
		return m_vBones[iXZRootBoneIdx]->Get_XZ_RootMotion_Delta();
	}

}


_float CModel::Get_SecondRootBone_YDelta()
{
	_int iYRootBoneIdx = m_pAnimator->Get_YRootBoneIdx();

	if (0 > iYRootBoneIdx || m_vBones.size() <= iYRootBoneIdx)
	{
		return 0.f;
	}
	else
	{
		return m_vBones[iYRootBoneIdx]->Get_Y_RootMotion_Delta();
	}

}


Vector4 CModel::Get_FirstRootBone_AccumulatedRootMotion()
{
	_int iXZRootBoneIdx = m_pAnimator->Get_XZRootBoneIdx();

	if (0 > iXZRootBoneIdx || m_vBones.size() <= iXZRootBoneIdx)
	{
		return { 0.f, 0.f, 0.f, 1.f };
	}
	else
	{
		return m_vBones[iXZRootBoneIdx]->Get_Current_XZRootMotion();
	}


}



_float CModel::Get_SecondRootBone_AccumulatedRootMotion()
{
	_int iYRootBoneIdx = m_pAnimator->Get_YRootBoneIdx();


	if (0 > iYRootBoneIdx || m_vBones.size() <= iYRootBoneIdx)
	{
		return 0.f;
	}
	else
	{
		return m_vBones[iYRootBoneIdx]->Get_Current_YRootMotion();
	}
}



const _float4x4* CModel::Get_BoneCombinedMatrix(const char* _szBoneName)
{
	//for (_uint i = 0; i < m_vBones.size(); ++i)
	//{
	//    if (0 == strcmp(_szBoneName, m_vBones[i]->Get_Name()))
	//        return m_vBones[i]->Get_CombinedTransformationFloat4x4Ptr();
	//}
	auto iter = m_mapBones.find(_szBoneName);
	auto end = m_mapBones.end();

	if (end != iter)
	{
		_uint uBoneIdx = iter->second;
		return m_vBones[uBoneIdx]->Get_CombinedTransformationFloat4x4Ptr();
	}
	else
	{
		return nullptr;
	}

}

CAnimation* CModel::Find_Animation(const string& _strName)
{
	auto iter = find_if(m_vAnimations.begin(), m_vAnimations.end(),
		[_strName](CAnimation* _pAnimation)
		{
			return _strName == _pAnimation->Get_Name();
		});

	if (iter == m_vAnimations.end())
	{
		return nullptr;
	}

	return *iter;
}

_float4x4* CModel::Get_AdjustMatrixPtr(string _szBoneName)
{
	auto iter = m_mapBones.find(_szBoneName);

	if (m_mapBones.end() == iter)
	{
		return nullptr;
	}
	else
	{
		return m_vBones[iter->second]->Get_AdjustMatrixPtr();
	}
}

Matrix	CModel::Get_AdjustMatrix(string _szBoneName)
{

	auto iter = m_mapBones.find(_szBoneName);

	if (m_mapBones.end() == iter)
	{
		return IDENTITY_MATRIX;
	}
	else
	{
		return m_vBones[iter->second]->Get_AdjustMatrix();
	}

}

HRESULT CModel::Set_BoneAdjustMatrix(Matrix _matAdjust, string _szBoneName)
{
	auto iter = m_mapBones.find(_szBoneName);


	if (m_mapBones.end() == iter)
	{
		return E_FAIL;
	}
	else
	{
		m_vBones[iter->second]->Set_AdjustMatrix(_matAdjust);
	}


	return S_OK;
}

CAnimation* CModel::Get_Animation(_uint _iIndex)
{
	if (static_cast<_uint>(m_vAnimations.size()) <= _iIndex)
	{
		return nullptr;
	}

	return m_vAnimations[_iIndex];
}


Matrix CModel::Get_AllNormalizedCombinedMatrix(const char* _szBoneName)
{
	auto iter = m_mapBones.find(_szBoneName);

	if (m_mapBones.end() == iter)
	{
		return Matrix::Identity;
	}
	else
	{
		return m_vBones[iter->second]->Get_AllNormalizedCombinedMatrix();
	}
}

Matrix CModel::Get_AllNormalizedCombinedMatrix(_int _iBoneIdx)
{
	if (0 > _iBoneIdx || m_vBones.size() <= _iBoneIdx)
	{
		return Matrix::Identity;
	}
	else
	{
		return m_vBones[_iBoneIdx]->Get_AllNormalizedCombinedMatrix();
	}
}

Matrix CModel::Get_RightUpLookNormalizedCombinedMatrix(const char* _szBoneName)
{
	auto iter = m_mapBones.find(_szBoneName);

	if (m_mapBones.end() == iter)
	{
		return Matrix::Identity;
	}
	else
	{
		return m_vBones[iter->second]->Get_NormalizedRightUpLookCombinedMatrix();
	}
}

Matrix CModel::Get_RightUpLookNormalizedCombinedMatrix(_int _iBoneIdx)
{
	if (0 > _iBoneIdx || m_vBones.size() <= _iBoneIdx)
	{
		return Matrix::Identity;
	}
	else
	{
		return m_vBones[_iBoneIdx]->Get_NormalizedRightUpLookCombinedMatrix();
	}
}

CComponent* CModel::Clone(void* _pArg)
{
	CModel* pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to clone : CModel");
	}

	return pInstance;
}


void CModel::Free()
{
	__super::Free();

	for (_uint i = 0; i < m_iNumMesh; ++i)
	{
		Safe_Release(m_vMeshes[i]);
	}
	m_vMeshes.clear();

	for (_uint i = 0; i < m_iNumMtrl; ++i)
	{
		Safe_Release(m_vMtrl[i]);
	}
	m_vMtrl.clear();

	for (_uint i = 0; i < m_vAnimations.size(); ++i)
	{
		Safe_Release(m_vAnimations[i]);
	}
	m_vAnimations.clear();

	Safe_Release(m_pAnimator);

	for (_uint i = 0; i < m_vBones.size(); ++i)
	{
		Safe_Release(m_vBones[i]);
	}
	m_vBones.clear();
	m_mapAnim.clear();


}
*/