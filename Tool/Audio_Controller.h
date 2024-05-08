#pragma once

#include "ImGui_BaseWindow.h"

BEGIN(Tool)

class CS_Audio_Controller final : public CImGui_BaseWindow
{
private:
	CS_Audio_Controller();
	virtual ~CS_Audio_Controller() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_float _fTimeDelta) override;

private:
	void _Load_JsonList(const string& _strDirectory);
	void _Show_Category();
	void _Show_Subclass();
	void _Show_Audios();

	void _Channel_Controller();
	void _Naive_List();
	void _Load_Folder(const string& _strDirectory);

	void _Reload(const string& _strDirectory);

public:
	static CS_Audio_Controller* Create();
	virtual void Free() override;

private:
	// <File name, <Json-key, Json-value>>
	unordered_map<string, unordered_map<string, list<string>>> m_audios;

	string m_strSelectCategory = "";
	string m_strSelectSubclass = "";
	string m_strSelectAudio = "";
	vector<string> m_vecNaives;

	set<string> m_completedInitialize;
};

END