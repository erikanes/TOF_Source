#include "Client_pch.h"
#include "Client_Defines.h"
#include "Quest_Manager.h"

// Singleton
#include "GameInstance.h"
#include "Data_Manager.h"
#include "Inventory.h"
#include "Quest_Manager.h"

// Thirdparty lib
#include "rapidcsv/rapidcsv.h"

// Observer
#include "Observer.h"

#include "Client_Functions.h"
#include "Engine_Functions.h"

IMPLEMENT_SINGLETON(CQuest_Manager)

HRESULT CQuest_Manager::Initialize(const string& _strFilePath)
{
	auto pData_Manager = CData_Manager::Get_Instance();

	rapidcsv::Document document(_strFilePath);

	auto iRowCount = document.GetRowCount();

	auto vecID = document.GetColumn<_uint>("ID");
	auto vecType = document.GetColumn<_uint>("Type");
	auto vecTargetID = document.GetColumn<_uint>("TargetID");
	auto vecCondition = document.GetColumn<_uint>("Condition");
	auto vecRewardItemID = document.GetColumn<_uint>("RewardItemID");
	auto vecRewardCount = document.GetColumn<_uint>("RewardCount");
	auto vecNextID = document.GetColumn<_uint>("NextID");
	auto vecAutoClear = document.GetColumn<_uint>("AutoClear");

	for (_uint i = 0; i < iRowCount; ++i)
	{
		QUEST_DESC desc;

		desc.iQuestID = vecID[i];
		desc.iQuestType = vecType[i];
		desc.iTargetID = vecTargetID[i];
		desc.iCondition = vecCondition[i];
		desc.iRewardItemID = vecRewardItemID[i];
		desc.iRewardItemCount = vecRewardCount[i];
		desc.iNextQuestID = vecNextID[i];
		desc.bAutoClear = 0 < vecAutoClear[i] ? true : false;

		desc.iStatus = QUEST_STATUS_WAIT;
		desc.iCount = 0;
		
		const auto& questDB = pData_Manager->Get_Quest_DB(desc.iQuestID);

		desc.wstrTitle = questDB.wstrTitle;

		m_mapQuests.emplace(desc.iQuestID, desc);
	}

	m_mapQuests.emplace(0, QUEST_DESC());

	return S_OK;
}

HRESULT CQuest_Manager::Begin_Quest(_uint _iQuestID)
{
	// 퀘스트 목록에 없는 퀘스트
	if (m_mapQuests.find(_iQuestID) == m_mapQuests.end())
		return E_FAIL;

	// 이미 등록되어있는 퀘스트
	if (m_progressQuests.find(_iQuestID) != m_progressQuests.end())
		return E_FAIL;

	auto& quest = m_mapQuests[_iQuestID];

	// 이미 진행중이거나 클리어된 퀘스트
	if (QUEST_STATUS_WAIT != quest.iStatus)
		return E_FAIL;

	quest.iStatus = QUEST_STATUS_PROGRESS;

	m_progressQuests.insert(_iQuestID);

	auto pGameInstance = CGameInstance::Get_Instance();

	// ui 호출
	auto eLevel = CFunc::Get_CurLevel();
	pGameInstance->Enable_UI(eLevel, TEXT("Layer_UI_QuestInfo_New"), 0);

	// 사운드 재생
	CFunc::Play_Audio_UI("UI_Quest_New");

	Notify_Observers(OBSERVER_NOTIFY_TYPE::QUEST_BEGIN, _iQuestID);

	// 수집 퀘스트인 경우 현재 인벤토리에 있는 아이템의 개수도 포함시킨다.
	if (quest.iQuestType == 1)
	{
		auto iCount = CInventory::Get_Instance()->Get_ItemCount(quest.iTargetID);

		quest.iCount += iCount;

		if (quest.iCount >= quest.iCondition)
		{
			quest.iStatus = QUEST_STATUS_ACHIEVEMENT;

			Notify_Observers(OBSERVER_NOTIFY_TYPE::QUEST_ACHIEVEMENT, quest.iQuestID);

			if (quest.bAutoClear)
				Clear_Quest(_iQuestID);
		}
	}

	// TODO : TargetID 여부와 현재 레벨에 따라 추적 마크 생성될 위치 체크하기
	// 생성하면서 포인터나 특정 위치 던져주고 그 위치 참조하도록.
	// Enable_UI 호출하면서 TargetID와 현재 레벨에 따라서 참조할 객체를 다르게 설정
	// 특정 객체가 아닌 장소를 참조하는거라면 위치를 보내주기
	// 즉, Enable_UI에 넘겨줘야 할 정보는 object pointer, offset, position 이 세가지이다. 세가지 정보를 다쓰는건 아니고 object pointer or position 이렇게 될 것 같음

	return S_OK;
}

HRESULT CQuest_Manager::Remove_Quest(_uint _iQuestID)
{
	if (m_mapQuests.find(_iQuestID) == m_mapQuests.end())
		return E_FAIL;

	if (m_progressQuests.find(_iQuestID) == m_progressQuests.end())
		return E_FAIL;

	m_mapQuests[_iQuestID].iCount = 0;
	m_mapQuests[_iQuestID].iStatus = QUEST_STATUS_WAIT;

	m_progressQuests.erase(_iQuestID);

	return S_OK;
}

HRESULT CQuest_Manager::Clear_Quest(_uint _iQuestID)
{
	if (m_mapQuests.find(_iQuestID) == m_mapQuests.end())
		return E_FAIL;

	if (m_progressQuests.find(_iQuestID) == m_progressQuests.end())
		return E_FAIL;

	m_mapQuests[_iQuestID].iStatus = QUEST_STATUS_COMPLETE;
	m_iCurrentClearQuest = _iQuestID;
	m_progressQuests.erase(_iQuestID);

	const auto& iRewardItemID = m_mapQuests[_iQuestID].iRewardItemID;
	const auto& iRewardCount = m_mapQuests[_iQuestID].iRewardItemCount;

	if (0 != iRewardItemID)
	{
		if (SUCCEEDED(CInventory::Get_Instance()->Add_Item(iRewardItemID, iRewardCount)))
		{
			auto pGameInstance = CGameInstance::Get_Instance();
			auto iCurrentLevel = static_cast<_uint>(pGameInstance->Get_CurLevel());

			SYSTEMINFO_GAIN_ITEM(iRewardItemID, iRewardCount);
		}
	}

	CFunc::Enable_QuestInfo_Complete(_iQuestID);

	CFunc::Play_Audio_UI("UI_Quest_Complete");

	const auto& iNextQuestID = m_mapQuests[_iQuestID].iNextQuestID;

	Notify_Observers(OBSERVER_NOTIFY_TYPE::QUEST_COMPLETE, _iQuestID);

	// 연계, 연속 퀘스트 대비
	if (0 != iNextQuestID)
		Begin_Quest(iNextQuestID);

	return S_OK;
}

void CQuest_Manager::Refresh_Quest()
{
	if (m_progressQuests.empty())
		return;

	for (auto& iQuest : m_progressQuests)
	{
		_uint iID = iQuest;
		const auto& quest = m_mapQuests[iQuest];
		const auto& iStatus = quest.iStatus;

		if (QUEST_STATUS_PROGRESS == iStatus)
			Notify_Observers(OBSERVER_NOTIFY_TYPE::QUEST_BEGIN, iID);

		else if (QUEST_STATUS_ACHIEVEMENT == iStatus)
			Notify_Observers(OBSERVER_NOTIFY_TYPE::QUEST_ACHIEVEMENT, iID);
	}

	auto eLevel = CFunc::Get_CurLevel();
	CGameInstance::Get_Instance()->Enable_UI(eLevel, TEXT("Layer_UI_QuestInfo_New"), 0);
}

HRESULT CQuest_Manager::Notify(const _uint _iObjectID, const _uint _iCount)
{
	/*
	아이템 획득, 몬스터 처치 등 퀘스트 목표 카운팅에 필요한 곳이라면 Notify를 호출하면 됨
	현재 추가된 곳
	- CInventory::Add_Item (아이템 획득)
	- CPlayer::OnCollisionBegin (몬스터 처치)
	*/

	if (m_progressQuests.empty())
		return S_OK;

	auto progressQuests = m_progressQuests;

	for (auto& progressQuest : progressQuests)
	{
		auto& quest = m_mapQuests[progressQuest];

		if (_iObjectID == quest.iTargetID)
		{
			quest.iCount += _iCount;
			
			if (quest.iCount >= quest.iCondition && quest.iStatus == QUEST_STATUS_PROGRESS)
			{
				quest.iStatus = QUEST_STATUS_ACHIEVEMENT;

				Notify_Observers(OBSERVER_NOTIFY_TYPE::QUEST_ACHIEVEMENT, quest.iQuestID);

				if (quest.bAutoClear)
					Clear_Quest(quest.iQuestID);
			}
		}
	}

	return S_OK;
}

void CQuest_Manager::Clear_Observers()
{
	m_listObservers.clear();
}

const QUEST_DESC& CQuest_Manager::Get_QuestDesc(_uint _iQuestID)
{
	// 0 == dummy data

	auto iter = m_mapQuests.find(_iQuestID);

	if (iter == m_mapQuests.end())
		return m_mapQuests[0];

	return m_mapQuests[_iQuestID];
}


void CQuest_Manager::Register_Observer(IObserver* _pObserver)
{
	auto iter = find(m_listObservers.begin(), m_listObservers.end(), _pObserver);

	if (iter != m_listObservers.end())
		return;

	m_listObservers.emplace_back(_pObserver);
}

void CQuest_Manager::Remove_Observer(IObserver* _pObserver)
{
	auto iter = find(m_listObservers.begin(), m_listObservers.end(), _pObserver);

	if (iter == m_listObservers.end())
		return;

	m_listObservers.erase(iter);
}

void CQuest_Manager::Notify_Observers(OBSERVER_NOTIFY_TYPE _eType, any _arg)
{
	for (auto& pObserver : m_listObservers)
		pObserver->Take_Notify(_eType, _arg);
}


void CQuest_Manager::Free()
{
	m_mapQuests.clear();
	m_progressQuests.clear();
}