#pragma once

#include "Base.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)

class CDialog_Manager : public CBase
{
	DECLARE_SINGLETON(CDialog_Manager)

	using ReadIndex = int;
	using ReadCharIndex = unsigned int;

	using Description = pair<GameObjectID, wstring>;

private:
	CDialog_Manager();
	virtual ~CDialog_Manager() = default;

public:
	HRESULT Initialize(const string& _strFilePath);
	void Tick(_float _fTimeDelta);

	HRESULT Begin_Dialog(DialogSceneID _iID, CGameObject* _pCaller = nullptr, _uint _eDialogType = 0);
	HRESULT End_Dialog();
	HRESULT Next_Dialog();

	void Bind_Callback_EndOfDialog(function<void()> _func);

#pragma region Getter
	const wstring& Get_Descripntion() const { return m_wstrDescription; }
	const _float& Get_PosX() const { return m_fX; }
	const _float& Get_PosY() const { return m_fY; }
	GameObjectID Get_TalkerID() const { return m_iCurrentTalker; }
#pragma endregion

private:
	HRESULT _ExecuteEvent(const wstring& _wstrDescription);
	_bool _Calculate_Dialog(_float _fTimeDelta);

	_bool _Dummy(_float) { return true; }

public:
	virtual void Free() override;

private:
	unordered_map<DialogSceneID, vector<Description>> m_mapDescriptions;
	vector<Description>* m_pCurrentDescriptions = { nullptr };
	ReadIndex m_iCurrentReadIndex = { 0 }; // 몇 번째 대화?
	ReadCharIndex m_iCurrentReadCharBeginIndex = { 0 };
	ReadCharIndex m_iCurrentReadCharEndIndex = { 0 };
	wstring m_wstrDescription; // 이벤트 플래그가 모두 제거된 대화
	
	list<wstring> m_removeEventDescriptions;

	_float m_fProgressTime = { 0.f };
	_float m_fWaitTime = { 0.f };

	_float m_fX = { 0.f };		// 텍스트 X 위치
	_float m_fY = { 0.f };		// 텍스트 Y 위치

	GameObjectID m_iCurrentTalker = { 0 };

	_bool m_bAutoEnd = { false };

	function<_bool(CDialog_Manager*, _float)> m_funcTick = [](CDialog_Manager*, _float) { return true; };

	CGameObject* m_pCaller = { nullptr };

	function<void()> m_funcEndOfDialog = []() {};
};

END