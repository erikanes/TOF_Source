#pragma once

#include "Base.h"
#include "Subject.h"

BEGIN(Client)

class CQuest_Manager final : public CBase, public ISubject
{
	DECLARE_SINGLETON(CQuest_Manager)

private:
	CQuest_Manager() = default;
	virtual ~CQuest_Manager() = default;

public:
	HRESULT Initialize(const string& _strFilePath);

	HRESULT Begin_Quest(_uint _iQuestID);
	HRESULT Remove_Quest(_uint _iQuestID);
	HRESULT Clear_Quest(_uint _iQuestID);
	void Refresh_Quest();

	HRESULT Notify(const _uint _iObjectID, const _uint _iCount);
	void Clear_Observers();

#pragma region Getter
	const QUEST_DESC& Get_QuestDesc(_uint _iQuestID);
	const set<_uint>& Get_ProgressQuests() const { return m_progressQuests; }
	_uint Get_CurrentClearQuest() const { return m_iCurrentClearQuest; }
#pragma endregion

public:
	virtual void Register_Observer(IObserver* _pObserver) override;
	virtual void Remove_Observer(IObserver* _pObserver) override;
	virtual void Notify_Observers(OBSERVER_NOTIFY_TYPE _eType, any _arg) override;

public:
	virtual void Free() override;

private:
	unordered_map<_uint, QUEST_DESC> m_mapQuests;
	set<_uint> m_progressQuests;

	list<IObserver*> m_listObservers;

	_uint m_iCurrentClearQuest = { 0 };
};

END