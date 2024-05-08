#include "Tool_BinaryExporter_pch.h"
#include "ToolLayout_RightProperties.h"

#include "Tool_Defines.h"

#include "Imgui_Tool.h"
#include "Character.h"

#include "ToolLayout_LeftAnimModelList.h"
#include "ToolLayout_BottomSequencer.h"

#include "ModelAnimation.h"

#include "Tool_Util.h"

#include "AnimationState.h"

#include "Json_Manager.h"
#include <fstream>
#include <istream>

/*

IMPLEMENT_SINGLETON(CA_LayoutRight_Properties);

CA_LayoutRight_Properties::CA_LayoutRight_Properties()
{
}


HRESULT CA_LayoutRight_Properties::Initialize(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	m_pDevice = _pDevice;
	m_pContext = _pContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	return S_OK;
}

HRESULT CA_LayoutRight_Properties::Tick(_float fTimeDelta)
{
	if (!m_bInitializeWindowSizePosSetting)
	{
		_float2 vOffsetPos = CImgui_Tool::Get_Instance()->Get_DrawableWinPos();
		_float2 vDrawableSize = CImgui_Tool::Get_Instance()->Get_DrawableWinSize();

		ImGui::SetNextWindowSize(ImVec2(vDrawableSize.x * 0.25f, vDrawableSize.y * 0.6f));
		ImGui::SetNextWindowPos(ImVec2(vOffsetPos.x +vDrawableSize.x - vDrawableSize.x * 0.25f, vOffsetPos.y));

		m_bInitializeWindowSizePosSetting = true;
	}

	m_pSelectedModel = CA_LayoutLeft_AnimModelList::Get_Instance()->Get_SelectedAnimModel();

	ImGui::Begin("Properties", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoNavInputs);

	if (nullptr != m_pSelectedModel)
	{
		if (ImGui::CollapsingHeader("Animation List"))
		{
			Handling_AnimList(m_pSelectedModel->Get_ModelData());
		}

		if (ImGui::CollapsingHeader("Model Settings For View"))
		{
			Handling_PivotMatrix(m_pSelectedModel);

			Handling_RootBone(m_pSelectedModel->Get_ModelData());
		}

		if (ImGui::CollapsingHeader("Animation State"))
		{
			Handling_AnimationStateData(m_pSelectedModel->Get_ModelData());
		}

		if (ImGui::CollapsingHeader("Animation Event"))
		{
			Handling_AnimationEvent(m_pSelectedModel->Get_ModelData());
		}



		Handling_Layout();
	}


	ImGui::End();

	return S_OK;
}

HRESULT CA_LayoutRight_Properties::Handling_Layout()
{
	ImVec2 vWinSize = ImGui::GetWindowSize();
	ImVec2 vWinPos = ImGui::GetWindowPos();

	ImGui::BeginChild("###Layout", ImVec2(vWinSize.x * 0.95f, vWinSize.y * 0.3f), true);

	ImVec2 vChildSize = ImGui::GetWindowSize();

	if (ImGui::Button("Save Anim Event", ImVec2(vChildSize.x * 0.5f, 25.f)))
	{       
		ifd::FileDialog::Instance().Save("NotificationSaveDialog", "Save Notification", 
			"BinFile (*.txt){.txt},.*");
	}

	ImGui::SameLine();

	if (ImGui::Button("Load Anim Event", ImVec2(vChildSize.x * 0.5f, 25.f)))
	{
		ifd::FileDialog::Instance().Open("NotificationLoadDialog", "Load Notification",
			"BinFile (*.txt){.txt},.*", false);
	}

	CModelData* pModelData = m_pSelectedModel->Get_ModelData();

	Handling_Save(pModelData);
	Handling_Load(pModelData);

	ImGui::EndChild();
	return S_OK;
}

HRESULT	CA_LayoutRight_Properties::Handling_AnimList(class CModelData* _pModelData)
{
	ImVec2 vWinSize = ImGui::GetWindowSize();

	ImGui::BeginChild("###Animation Info", ImVec2(vWinSize.x * 0.95f, vWinSize.y * 0.3f), true);
	ImGui::Text("Animation Info");
	ImGui::Text("Number of Animation : %d", _pModelData->Get_NumAnimation());
	ImGui::NewLine();
	ImGui::NewLine();

	const vector<CModelAnimation*>& vecAnims = _pModelData->Get_vAnimations();
	m_szSelectedAnimName = vecAnims[_pModelData->Get_CurAnimIdx()]->Get_Name();

	ImGui::Text("Animation List");
	if (ImGui::BeginListBox("##Animation List"))
	{
		for (auto& pAnim : vecAnims)
		{
			_bool bSelected = false;
			if (pAnim->Get_Name() == m_szSelectedAnimName)
			{
				bSelected = true;
			}

			if (ImGui::Selectable(pAnim->Get_Name(), &bSelected))
			{
				_pModelData->Change_Animation(pAnim->Get_Name());
				CA_LayoutBottom_Sequencer::Get_Instance()->Set_TrackPosition(0.f);
				_pModelData->Play_Animation_By_TrackPosition(0.f);
			}
		}
		ImGui::EndListBox();
	}


	ImGui::Separator();

   
	ImGui::Text("Occur Interpolation");
	if (ImGui::BeginListBox("##Next Animation List"))
	{
		for (auto& pAnim : vecAnims)
		{
			_bool bSelected = false;
			if (pAnim->Get_Name() == m_szSelectedAnimName)
			{
				bSelected = true;
			}

			if (ImGui::Selectable(pAnim->Get_Name(), &bSelected))
			{
				CModelData* pModelData = m_pSelectedModel->Get_ModelData();
				map<string, CAnimationState::TRANSITIONDESC>* pMapTransition = (pModelData->Get_TransitionMap(m_szDstTransitionAnimName));

				if (nullptr != pMapTransition)
				{
					m_pSelectedModel->Play_TransitionAnimationFromCurAnimation(m_szSelectedAnimName, m_szDstTransitionAnimName);
				}
				else
				{
					_pModelData->Change_Animation(pAnim->Get_Name());
					CA_LayoutBottom_Sequencer::Get_Instance()->Set_TrackPosition(0.f);
					_pModelData->Play_Animation_By_TrackPosition(0.f);
				}
			}
		}
		ImGui::EndListBox();
	}


	ImGui::EndChild();


	return S_OK;
}

HRESULT CA_LayoutRight_Properties::Handling_RootBone(CModelData* _pModelData)
{
	string szRootBoneName = _pModelData->Get_RootBoneName();
	char szArrRootBone[MAX_PATH] = { "" };

	strcpy_s(szArrRootBone, szRootBoneName.c_str());


	if (ImGui::InputText("##RootBone", szArrRootBone, sizeof(char) * MAX_PATH))
	{
		_pModelData->Set_RootBoneName(szArrRootBone);
	}


	return S_OK;
}

HRESULT CA_LayoutRight_Properties::Handling_PivotMatrix(CCharacter* _pPlayerCharacter)
{   
	ImVec2 vWinSize = ImGui::GetWindowSize();

	Matrix matPivot = _pPlayerCharacter->Get_Pivot_Model_Matrix();
	Vector3 vScale, vRotation, vTranslation;
	Quaternion vQuaternion;
	matPivot.Decompose(vScale, vQuaternion, vTranslation);
	vRotation = vQuaternion.ToEuler();
	


	ImGui::BeginChild("Pivot Matrix", ImVec2(vWinSize.x , vWinSize.y * 0.3f), true);

	ImGui::Text("Scale");
	_float3 vScaleDisplay = vScale;
	ImGui::InputFloat3("##Scale", (_float*)&vScaleDisplay);
	vScale = vScaleDisplay;

	ImGui::Text("Rotation");
	_float3 vRotationDisplay = { XMConvertToDegrees(vRotation.x), XMConvertToDegrees(vRotation.y), XMConvertToDegrees(vRotation.z) };
	if (ImGui::InputFloat3("##Rotation", (_float*)&vRotationDisplay))
	{
		vRotationDisplay = vRotationDisplay;
	}
	vRotation = vRotationDisplay;

	ImGui::Text("Translation");
	_float3 vTranslationDisplay = vTranslation;
	ImGui::InputFloat3("##Translation", (_float*)&vTranslationDisplay);
	vTranslation = vTranslationDisplay;


	Matrix matResult;
	_vector vRotationInput = {XMConvertToRadians(vRotation.x), XMConvertToRadians(vRotation.y), XMConvertToRadians(vRotation.z)};
	_vector vScaleInput = vScale;
	_vector vTranslationInput = { vTranslation.x, vTranslation.y, vTranslation.z, 1.f };
	
	matResult = XMMatrixAffineTransformation(vScaleInput, vRotationInput, {0.f, 0.f, 0.f, 0.f}, vTranslationInput);
	matResult = XMMatrixScalingFromVector(vScaleInput) * XMMatrixRotationRollPitchYawFromVector(vRotationInput) * XMMatrixTranslationFromVector(vTranslationInput);
	_pPlayerCharacter->Set_Pivot_Model_Matrix(matResult);


	ImGui::EndChild();
	return S_OK;
}


HRESULT CA_LayoutRight_Properties::Handling_Save(CModelData* _pModelData)
{
	if (ifd::FileDialog::Instance().IsDone("NotificationSaveDialog")) {
		if (ifd::FileDialog::Instance().HasResult()) {
			auto rPath = ifd::FileDialog::Instance().GetResult();
			printf("OPEN[%s]\n", rPath.u8string().c_str());
			Save_Notification_Data(_pModelData, rPath.u8string().c_str());
		}
		ifd::FileDialog::Instance().Close();
	}

	return S_OK;
}

HRESULT CA_LayoutRight_Properties::Handling_Load(CModelData* _pModelData)
{
	if (ifd::FileDialog::Instance().IsDone("NotificationLoadDialog")) {
		if (ifd::FileDialog::Instance().HasResult()) {
			auto rPath = ifd::FileDialog::Instance().GetResult();
			printf("OPEN[%s]\n", rPath.u8string().c_str());
			Load_Notification_Data(_pModelData, rPath.u8string().c_str());          
		}
		ifd::FileDialog::Instance().Close();
	}

	return S_OK;
}
*/

void CA_LayoutRight_Properties::Save_Notification_Data(CModelData* _pModelData, const char* _szSavePath)
{
	_int iCurAnimIdx = m_pSelectedModel->Get_ModelData()->Get_CurAnimIdx();
	const vector<CModelAnimation*>& vecAnims = _pModelData->Get_vAnimations();

	CModelAnimation* pAnim = vecAnims[iCurAnimIdx];

	ofstream ReadFile(_szSavePath);
	nlohmann::ordered_json savefile;

	for (auto& animation : vecAnims)
	{
		auto& notifications = animation->Get_Notifications();

		if (0 >= static_cast<_uint>(notifications.size()))
			continue;

		string strName = animation->Get_Name();

		for (auto& notify : notifications)
		{
			string strKeyframe = to_string(notify.first);
			size_t i = 0;

			for (auto& evt : notify.second)
			{
				string strIndex = to_string(i);
				savefile[strName][strKeyframe][strIndex]["type"] = static_cast<_uint>(evt.eNotificationType);

				if (NOTIFICATION_TYPE::SOUND == evt.eNotificationType)
				{
					savefile[strName][strKeyframe][strIndex]["audiotag"] = evt.tNotificationSoundData.szTag;
					savefile[strName][strKeyframe][strIndex]["dimension"] = 0u;
				}

				else if (NOTIFICATION_TYPE::EFFECT == evt.eNotificationType)
				{
					savefile[strName][strKeyframe][strIndex]["effecttag"] = evt.tNotificationEffectData.szTag;
					savefile[strName][strKeyframe][strIndex]["dimension"] = static_cast<_uint>(evt.tNotificationEffectData.eBatchType);
				}
				else if (NOTIFICATION_TYPE::COLLISION == evt.eNotificationType)
				{
					savefile[strName][strKeyframe][strIndex]["collisiontag"] = evt.tNotificationCollisionData.szTag;
					savefile[strName][strKeyframe][strIndex]["operation"] = evt.tNotificationCollisionData.iOperation;
				}
				else if (NOTIFICATION_TYPE::COLLISION_TRANSFORM_CHANGE == evt.eNotificationType)
				{
					savefile[strName][strKeyframe][strIndex]["collisiontag"] = evt.tNotificationCollisionTransformChangeData.szTag;
				}
				else if (NOTIFICATION_TYPE::OBJECT_CREATE == evt.eNotificationType)
				{
					savefile[strName][strKeyframe][strIndex]["createobjecttag"] = evt.tNotificationObjectCreateData.szTag;
				}
				else if (NOTIFICATION_TYPE::CUSTOM_EVENT == evt.eNotificationType)
				{
					savefile[strName][strKeyframe][strIndex]["customeventtag"] = evt.tNotificationCustomEventData.szTag;
					savefile[strName][strKeyframe][strIndex]["customeventindex"] = evt.tNotificationCustomEventData.iIndex;
				}

				++i;
			}                      

			// first : keyframe
			// second : list
		}
	}

	ReadFile << savefile.dump(4);
	ReadFile.close();
}

void CA_LayoutRight_Properties::Load_Notification_Data(CModelData* _pModelData, const char* _szLoadPath)
{
	_int iCurAnimIdx = m_pSelectedModel->Get_ModelData()->Get_CurAnimIdx();
	const vector<CModelAnimation*>& vecAnims = _pModelData->Get_vAnimations();

	CModelAnimation* pAnim = vecAnims[iCurAnimIdx];


	nlohmann::ordered_json Loadfile;
	CJson_Manager::Get_Instance()->Parse(_szLoadPath, Loadfile);

	for (auto iter = Loadfile.begin(); iter != Loadfile.end(); ++iter)
	{
		auto& strName = iter.key();
		
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
				else if (NOTIFICATION_TYPE::OBJECT_CREATE == eType)
				{

				}
				else if (NOTIFICATION_TYPE::COLLISION_TRANSFORM_CHANGE == eType)
				{

				}
				else if (NOTIFICATION_TYPE::CUSTOM_EVENT == eType)
				{
					strcpy_s(tMsg.tNotificationCustomEventData.szTag, MAX_PATH, evt["customeventtag"].get<string>().c_str());
					tMsg.tNotificationCustomEventData.iIndex = evt["customeventindex"].get<_int>();
				}


				m_pSelectedModel->Get_ModelData()->Add_Notifications(strName, tMsg);
			}
		}
	}
}

/*
HRESULT CA_LayoutRight_Properties::Handling_AnimationStateData(class CModelData* _pModelData)
{
	_int iCurAnimIdx = m_pSelectedModel->Get_ModelData()->Get_CurAnimIdx();
	const vector<CModelAnimation*>& vecAnims = _pModelData->Get_vAnimations();

	ImGui::Text("Transition Time");
	ImGui::InputFloat("##InputTransitionTime", &m_fTransitionTime);

	ImGui::Text("Transtion Tick Per Sec");
	ImGui::InputFloat("##InputTransitionTickPerSec", &m_fTransitionTickPerSec);

	ImGui::Text("Transition Next Time");
	ImGui::InputFloat("##InputTransitionNextTime", &m_fTransitionNextTime);  


	ImGui::Text("Select Source Animation");
	if (ImGui::BeginListBox("##Src Animation List"))
	{
		for (auto& pAnim : vecAnims)
		{
			_bool bSelected = false;
			if (ImGui::Selectable(pAnim->Get_Name(), &bSelected))
			{
				m_szSrcTransitionAnimName = pAnim->Get_Name();
			}
		}
		ImGui::EndListBox();
	}

	ImGui::Text("Select Destination Animation");
	if (ImGui::BeginListBox("##Dst Animation List"))
	{
		for (auto& pAnim : vecAnims)
		{
			_bool bSelected = false;
			if (ImGui::Selectable(pAnim->Get_Name(), &bSelected))
			{
				m_szDstTransitionAnimName = pAnim->Get_Name();
			}
		}
		ImGui::EndListBox();
	}

	ImGui::Separator();
	ImGui::Text("From Anim");
	ImGui::Text(m_szSrcTransitionAnimName.c_str());

	ImGui::NewLine();

	ImGui::Text("To Anim");
	ImGui::Text(m_szDstTransitionAnimName.c_str());

	ImGui::NewLine();

	if (ImGui::Button("Add Transition"))
	{
		CAnimationState::TRANSITIONDESC tTransitionDesc;
		tTransitionDesc.bTransition = true;
		tTransitionDesc.fStartTrackPositionTransition = m_fTransitionNextTime;
		tTransitionDesc.fTransitionTickPerSec = m_fTransitionTickPerSec;
		tTransitionDesc.fTransitionTime = m_fTransitionTime;
		tTransitionDesc.szSrcAnimName = m_szSrcTransitionAnimName;
		tTransitionDesc.szSrcAnimStateTag = "";

		m_pSelectedModel->Get_ModelData()->Add_TransitionData(m_szDstTransitionAnimName, tTransitionDesc);
	}
	ImGui::Separator();

	CModelData* pModelData = m_pSelectedModel->Get_ModelData();
	map<string, CAnimationState::TRANSITIONDESC>* pMapTransition = (pModelData->Get_TransitionMap(m_szDstTransitionAnimName));

	ImGui::Text("Animation Transition List");
	if (nullptr != pMapTransition)
	{
		if (ImGui::BeginListBox("##Animation Transition List"))
		{
			auto iter = pMapTransition->begin();

			while (pMapTransition->end() != iter)
			{
				_bool bSelected = false;
				if (ImGui::Selectable(iter->first.c_str(), &bSelected))
				{
					m_pSelectedModel->Play_TransitionAnimation(iter->first, m_szDstTransitionAnimName);

				}

				++iter;
			}
			ImGui::EndListBox();
		}

	}
	ImGui::Separator();


	

	return S_OK;
}

HRESULT			CA_LayoutRight_Properties::Handling_AnimationEvent(CModelData* _pModelData)
{
	CModelAnimation* pCurAnimation = _pModelData->Get_CurAnimation();
   
	CA_LayoutBottom_Sequencer* pBottomSeq = CA_LayoutBottom_Sequencer::Get_Instance();

	_int iCurrentTrackPosition = pBottomSeq->Get_CurrentTrackPosition();

	if (nullptr == pCurAnimation)
		return E_FAIL;

	auto& mapNotifications = pCurAnimation->Get_Notifications();

	static _int iType = 0;
	ImGui::Text("Notification Type");
	if (ImGui::BeginCombo("##Notification Type", m_arrComboNotificationType[m_iNotificationTypeIdx]))
	{
		for (_int i = 0; i < sizeof(m_arrComboNotificationType)/sizeof(char*); ++i)
		{
			if (ImGui::Selectable(m_arrComboNotificationType[i]))
			{
				m_iNotificationTypeIdx = i;
			}
		}
		ImGui::EndCombo();
	}

	if (0 == strcmp(m_arrComboNotificationType[m_iNotificationTypeIdx], "SOUND"))
	{
		m_eNotificationType = NOTIFICATION_TYPE::SOUND;
	}
	else if (0 == strcmp(m_arrComboNotificationType[m_iNotificationTypeIdx], "EFFECT"))
	{
		m_eNotificationType = NOTIFICATION_TYPE::EFFECT;
	}
	else if (0 == strcmp(m_arrComboNotificationType[m_iNotificationTypeIdx], "COLLISION"))
	{
		m_eNotificationType = NOTIFICATION_TYPE::COLLISION;
	}
	else if (0 == strcmp(m_arrComboNotificationType[m_iNotificationTypeIdx], "COLLISION_CHANGE"))
	{
		m_eNotificationType = NOTIFICATION_TYPE::COLLISION_TRANSFORM_CHANGE;
	}
	else if (0 == strcmp(m_arrComboNotificationType[m_iNotificationTypeIdx], "OBJECT_CREATE"))
	{
		m_eNotificationType = NOTIFICATION_TYPE::OBJECT_CREATE;
	}
	else if (0 == strcmp(m_arrComboNotificationType[m_iNotificationTypeIdx], "CUSTOM_EVENT"))
	{
		m_eNotificationType = NOTIFICATION_TYPE::CUSTOM_EVENT;
	}


	switch (m_iNotificationTypeIdx)
	{
	case 0 :
	{
		ImGui::Text("Sound Tag");
		if (ImGui::InputText("##SoundTag", m_szArrSoundTag, MAX_PATH))
		{
		}

		break;
	}

	case 1:
	{
		ImGui::Text("Effect Tag");
		if (ImGui::InputText("##EffectTag", m_szArrEffectTag, MAX_PATH))
		{
		}

		ImGui::Text("Batch Type");
		if (ImGui::BeginCombo("##Effect Batch Type", m_arrEffectBatchType[m_iEffectBatchTypeIdx]))
		{
			for (_int i = 0; i < 2; ++i)
			{
				if (ImGui::Selectable(m_arrEffectBatchType[i]))
				{
					m_iEffectBatchTypeIdx = i;
				}
			}
			ImGui::EndCombo();
		}


		break;
	}
	case 2:
	{
		ImGui::Text("Collision Tag");
		if (ImGui::InputText("##CollisionTag", m_szCollisionTag, MAX_PATH))
		{
		}

		ImGui::Text("Operation Type");
		if (ImGui::BeginCombo("##Collision Operation Type", m_arrCollisionOperationType[m_uCollisionOperation]))
		{
			for (_int i = 0; i < 2; ++i)
			{
				if (ImGui::Selectable(m_arrCollisionOperationType[i]))
				{
					m_uCollisionOperation = i;
				}
			}
			ImGui::EndCombo();
		}


		break;
	}
	case 3 :
	{
		ImGui::Text("Collision Tag");
		ImGui::InputText("##CollisionChangeTag", m_szCollisionTransformChangeTag, MAX_PATH);

		ImGui::Text("Translation");
		ImGui::InputFloat3("##Collision_Transform_Translation", (_float*)&m_vCollisionChangeTranslation);

		ImGui::Text("Scale");
		ImGui::InputFloat3("##Collision_Transform_Scale", (_float*)&m_vCollisionChangeScale);

		ImGui::Text("Orientation");
		ImGui::InputFloat3("##Collision_Transform_Orientation", (_float*)&m_vCollisionChangeRotation);

		break;
	}

	case 4 :
	{
		ImGui::Text("Object Tag");
		ImGui::InputText("##ObjectCreateTag", m_szObjectCreateTag, MAX_PATH);

		ImGui::Text("number of creation");
		ImGui::InputInt("##ObjectCreateCount", &m_iObjectCreateCount, 0);
		break;
	}
	case 5 :
	{
		ImGui::Text("Custom event tag");
		ImGui::InputText("##CustomEVentTag", m_szCustomEventTag, MAX_PATH);
		
		ImGui::Text("index");
		ImGui::InputInt("##CustomObjectFunction", &m_iCustomEventIndex, 0);
	}
	default:
	{

		break;
	}
	}


	if (ImGui::Button("Add notify"))
	{
		auto iter = mapNotifications.find(iCurrentTrackPosition);
		NOTIFICATIONMSG tMsg;
		tMsg.iTrackPosition = iCurrentTrackPosition;
		tMsg.eNotificationType = m_eNotificationType;

		if (NOTIFICATION_TYPE::SOUND == tMsg.eNotificationType)
		{
			NOTIFICATIONSOUNDDATA tNotificationSoundData;
			strcpy_s(tNotificationSoundData.szTag, MAX_PATH, m_szArrSoundTag);

			tMsg.tNotificationSoundData = tNotificationSoundData;
		}
		else if(NOTIFICATION_TYPE::EFFECT == tMsg.eNotificationType)
		{
			NOTIFICATIONEFFECTDATA tNotificationEffectData;
			tNotificationEffectData.eBatchType = (EFFECT_BATCH_TYPE)m_iEffectBatchTypeIdx;
			strcpy_s(tNotificationEffectData.szTag, MAX_PATH, m_szArrEffectTag);

			tMsg.tNotificationEffectData = tNotificationEffectData;
		}
		else if (NOTIFICATION_TYPE::COLLISION == tMsg.eNotificationType)
		{
			NOTIFICATIONCOLLISIONDATA tNotificationCollisionData;
			tNotificationCollisionData.iOperation = m_uCollisionOperation;
			strcpy_s(tNotificationCollisionData.szTag, MAX_PATH, m_szCollisionTag);

			tMsg.tNotificationCollisionData = tNotificationCollisionData;
		}
		else if (NOTIFICATION_TYPE::COLLISION_TRANSFORM_CHANGE == tMsg.eNotificationType)
		{
			NOTIFICATIONCOLLISIONTRANSFORMCHANGE tNotificationCollisionTransformChangeData;
			tNotificationCollisionTransformChangeData.vTranslation = m_vCollisionChangeTranslation;
			tNotificationCollisionTransformChangeData.vSize = m_vCollisionChangeScale;
			tNotificationCollisionTransformChangeData.vRotation = m_vCollisionChangeRotation;

			strcpy_s(tNotificationCollisionTransformChangeData.szTag, MAX_PATH, m_szCollisionTransformChangeTag);

			tMsg.tNotificationCollisionTransformChangeData = tNotificationCollisionTransformChangeData;
		}
		else if (NOTIFICATION_TYPE::OBJECT_CREATE == tMsg.eNotificationType)
		{
			NOTIFICATIONOBJECTCREATE tNotificationObjectCreate;
			strcpy_s(tNotificationObjectCreate.szTag, MAX_PATH, m_szObjectCreateTag);
			tNotificationObjectCreate.iCount = m_iObjectCreateCount;

			tMsg.tNotificationObjectCreateData = tNotificationObjectCreate;
		}
		else if (NOTIFICATION_TYPE::CUSTOM_EVENT == tMsg.eNotificationType)
		{
			NOTIFICATIONCUSTOMEVENT tNotificationCustomEvent;
			strcpy_s(tNotificationCustomEvent.szTag, MAX_PATH, m_szCustomEventTag);
			tNotificationCustomEvent.iIndex = m_iCustomEventIndex;

			tMsg.tNotificationCustomEventData = tNotificationCustomEvent;
		}

		//if (mapNotifications.end() == iter)
		//{
		//	mapNotifications.insert({ iCurrentTrackPosition, list<NOTIFICATIONMSG>() });
		//	iter = mapNotifications.find(iCurrentTrackPosition);
		//}
		_pModelData->Add_Notifications(_pModelData->Get_CurAnimIdx(), tMsg);

		//iter->second.push_back(tMsg);
	}

	ImGui::Separator();
	
	ImGui::BeginChild("Notification List", {400, 500}, true, ImGuiWindowFlags_AlwaysAutoResize | 
		ImGuiWindowFlags_AlwaysVerticalScrollbar);
	_int iNotificationSize = mapNotifications.size();
	_int iEraseButtonId = 0;


	if (0 < iNotificationSize)
	{
		auto iterList = mapNotifications.begin();	

		while (mapNotifications.end() != iterList)
		{
			auto& listMsg = iterList->second;
			string szMsg = "Frame : ";

			auto iterFirstMsg = listMsg.begin();

			while (listMsg.end() != iterFirstMsg)
			{
				szMsg = "";
				szMsg += to_string(iterFirstMsg->iTrackPosition);
				szMsg += "Notification type : ";
				if (iterFirstMsg->eNotificationType == NOTIFICATION_TYPE::SOUND)
				{
					szMsg += "Sound Notification\n";				
					szMsg += "\n";
					szMsg += "Tag : ";
					szMsg += iterFirstMsg->tNotificationSoundData.szTag;
				}
				else if (iterFirstMsg->eNotificationType == NOTIFICATION_TYPE::EFFECT)
				{
					szMsg += "Effect Notification\n";
					szMsg += to_string(iterFirstMsg->iTrackPosition);
					szMsg += "\n";
					szMsg += "Tag : ";
					szMsg += iterFirstMsg->tNotificationEffectData.szTag;
				}
				else if (iterFirstMsg->eNotificationType == NOTIFICATION_TYPE::COLLISION)
				{
					szMsg += "Collsion operation notification\n";
					szMsg += "\n";
					szMsg += "Tag : ";
					szMsg += iterFirstMsg->tNotificationCollisionData.szTag;
					szMsg += "\n";
					szMsg += "Operation";
					szMsg += iterFirstMsg->tNotificationCollisionData.iOperation == 0 ? "Off" : "On";
				}
				else if (iterFirstMsg->eNotificationType == NOTIFICATION_TYPE::CUSTOM_EVENT)
				{
					szMsg += "Custom event notification\n";
					szMsg += "\n";
					szMsg += "Tag : ";
					szMsg += iterFirstMsg->tNotificationCustomEventData.szTag;
					szMsg += "\n";
					szMsg += "Index";
					szMsg += to_string(iterFirstMsg->tNotificationCustomEventData.iIndex);
				}

				ImGui::Text(szMsg.c_str()); ImGui::SameLine();

				ImGui::PushID(iEraseButtonId);
				if (ImGui::Button("X##btn"))
				{
					pCurAnimation->Remove_Notify(iterFirstMsg->iTrackPosition);
					//pCurAnimation->Remove_Notify(iterFirstMsg);
					iterFirstMsg = listMsg.erase(iterFirstMsg);
				}
				else
				{
					++iterFirstMsg;
				}
				ImGui::PopID();

				ImGui::Separator();

				++iEraseButtonId;
			}
			++iterList;
		}
	}
	ImGui::EndChild();


	return S_OK;
}




void CA_LayoutRight_Properties::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
*/