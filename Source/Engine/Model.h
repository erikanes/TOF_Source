#pragma once

#include "Component.h"
#include "GameObject.h"
#include "Engine_Struct_Data.h"

BEGIN(Engine)
class ENGINE_DLL CModel final
	: public CComponent
{
public:
	typedef struct tModelDesc
	{
		string			jsonTag;
		CGameObject*	pOwner = { nullptr };
	}MODELDESC;

public:
	enum MODELTYPE { ANIM_TYPE, NONANIM_TYPE, MODEL_END };

private:
	CModel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CModel(const CModel& _rhs);
	~CModel() = default;

public:
	virtual HRESULT Initialize_Prototype(const char* _pFilePath, const char* _pNotificationsPath = nullptr);

	virtual HRESULT Initialize(void* _pArg) override;
	HRESULT Bind_ShaderResourceView(class CShader* _pShader, _uint _iMeshIdx);
	HRESULT Bind_ShaderBoneMatricies(class CShader* _pShader, _uint _uMeshIdx, const char* _szConstantName);

	HRESULT Play_Animation(_float _fTimeDelta);

	HRESULT Render(_uint _uMeshIdx);

	class CGameObject* Get_Owner() { return m_pOwner; }

public:
	static CModel* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const char* _pFilePath, const char* _pNotificationPath = nullptr);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;

private:
	HRESULT Ready_Meshes(const MODELDATA& _tModelData);
	HRESULT Ready_Materials(const MODELDATA& _tModelData, const char* _szPath);
	HRESULT Ready_Bones(const MODELDATA& _tModelData);
	HRESULT Ready_Animations(const MODELDATA& _tModelData);
	HRESULT Ready_Notifications(const char* _szPath);

	HRESULT	Register_Notification_Functions(void* _pArg);

public:
	typedef vector<class CBone*>	BONES;

private:
	class CGameObject*		m_pOwner = { nullptr };

	MODELTYPE				m_eModelType = { MODEL_END };
	_uint					m_iNumMesh = { 0 };
	_uint					m_iNumMtrl = { 0 };
	_uint					m_iNumAnim = { 0 };
	_float4x4				m_matPivot;

	class CCustomImporter*	m_pCustomScene = { nullptr };

private:
	_int					m_iCurAnimIdx = { 7 };
	_int					m_iNextAnimIdx = { 7 };

private:
	vector <class CMesh*>		m_vMeshes;
	vector <class CMaterial*>	m_vMtrl;

	BONES						m_vBones;
	vector <class CAnimation*>	m_vAnimations;

	map<string, _uint>			m_mapBones;//자주 발생하는 bone검색용
	map<string, _uint>			m_mapAnim; //자주 발생하는 Animation 검색용<애니메이션 이름, 애니메이션 vector 인덱스>

	class CAnimator* m_pAnimator = { nullptr };

	_bool						m_bLastAnimCheck = { false };

	_int						m_iRootMotionBoneIdx = { -1 };
	string						m_szRootMotionBoneName = "Bip001";

	static inline _float4x4 m_boneMatricies[256] = {};

	
	map<_int, list<NOTIFICATIONMSG>> m_mapNotifications;
	// animation_idx, notification_list

public:
	_uint	Get_NumMesh() { return m_iNumMesh; }
	string	Get_szMeshName(_uint _uIdx);
	_uint	Get_BoneIndex(string _szBoneName);
	string	Get_BoneName(_int _iBoneIdx);

	// Test For binary handling
	vector<class CMesh*>& Get_vMeshes() { return m_vMeshes; }
	vector<class CMaterial*> Get_vMaterials() { return m_vMtrl; }
	vector<class CBone*>& Get_vBones() { return m_vBones; }
	vector<class CAnimation*>& Get_vAnimations() { return m_vAnimations; }

	_uint		Get_NumMaterial() { return m_iNumMtrl; }
	_uint		Get_NumAnimation() { return m_iNumAnim; }
	MODELTYPE	Get_ModelType() { return m_eModelType; }

	void Set_NumMeshes(_uint _uNumMesh) { m_iNumMesh = _uNumMesh; }
	void Set_NumMaterials(_uint _uNumMtrls) { m_iNumMtrl = _uNumMtrls; }
	void Set_NumAnimations(_uint _uNumAnims) { m_iNumAnim = _uNumAnims; }

	void Add_Mesh(class CMesh* _pNewMesh) { m_vMeshes.push_back(_pNewMesh); }
	void Add_Material(class CMaterial* _pNewMtrl) { m_vMtrl.push_back(_pNewMtrl); }
	void Add_Bone(class CBone* _pNewBone) { m_vBones.push_back(_pNewBone); }
	void Add_Animation(class CAnimation* _pNewAnim) { m_vAnimations.push_back(_pNewAnim); }

	//Test
	void Add_EffectFunc(_int _iTrackPosition, string _szTag, string _szAnimName, function<void()> _funcCreateEffect);

	HRESULT			Set_BoneAdjustMatrix(Matrix _matAdjust, string _szBoneName);
	Matrix			Get_AdjustMatrix(string _szBoneName);
	_float4x4*		Get_AdjustMatrixPtr(string _szBoneName);

public:
	void Next_Anim() { ++m_iCurAnimIdx; if (m_iCurAnimIdx >= (_int)m_iNumAnim) m_iCurAnimIdx = 0; }

	void Change_Animation(_uint _uNextAnim);
	void Change_Animation(const char* _pszNextAnim);
	void Change_Animation(AnimState _eAnimState);

	_bool Get_IntersectMesh(_matrix _matWorld, _float3& _vOutPos, _float& _fOutDist);

	class CAnimator* Get_Animator() { return m_pAnimator; }

	_float	Get_FirstRootBone_XZDelta();
	_float	Get_SecondRootBone_YDelta();
	Vector4 Get_FirstRootBone_AccumulatedRootMotion();
	_float	Get_SecondRootBone_AccumulatedRootMotion();

	const _float4x4* Get_BoneCombinedMatrix(const char* _szBoneName);
	Matrix Get_AllNormalizedCombinedMatrix(const char* _szBoneName);
	Matrix Get_AllNormalizedCombinedMatrix(_int _iBondIdx);
	Matrix Get_RightUpLookNormalizedCombinedMatrix(const char* _szBoneName);
	Matrix Get_RightUpLookNormalizedCombinedMatrix(_int _iBoneIdx);

	_float4x4 Get_PivotMatrix() { return m_matPivot; }
	void Set_PivotMatrix(_float4x4 _matPivot) { m_matPivot = _matPivot; }

	CAnimation* Find_Animation(const string& _strName);
	CAnimation* Get_Animation(_uint _iIndex);
};
END