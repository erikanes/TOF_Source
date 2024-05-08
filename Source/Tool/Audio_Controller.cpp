#include "Tool_BinaryExporter_pch.h"
#include "Audio_Controller.h"

#include <shlwapi.h>
#pragma comment(lib, "shlwapi")

// Singleton
#include "GameInstance.h"
#include "Json_Manager.h"
#include "AudioChannel.h"
#include "AudioClip.h"

#include "ImGuiFileDialog.h"

CS_Audio_Controller::CS_Audio_Controller()
	: CImGui_BaseWindow("Audio Controller")
{
}

HRESULT CS_Audio_Controller::Initialize()
{ 
	_Load_JsonList("../Bin/DataFiles/Audios/");

	Disable();

	m_completedInitialize.clear();

	return S_OK;
}

void CS_Audio_Controller::Tick(_float _fTimeDelta)
{
	if (!m_bIsEnable)
		return;

	ImGui::Begin(m_strName.c_str(), &m_bIsEnable);

	_Show_Category();
	ImGui::SameLine();

	_Show_Subclass();
	ImGui::SameLine();

	_Show_Audios();

	_Naive_List();

	_Channel_Controller();

	// 사운드 로드 (원하는 json 파일)
	// 불러들인걸로 그룹 리스트 생성. 미리 듣기 (재생, 일시중지, 중지)

	ImGui::End();
}

void CS_Audio_Controller::_Load_JsonList(const string& _strDirectory)
{
	_finddata_t fd;

	string strFindPath = _strDirectory + "*.json";
	auto iHandle = _findfirst(strFindPath.c_str(), &fd);

	if (-1 == iHandle)
		return;

	auto iResult = 0;
	char szFullPath[MAX_PATH] = "";

	while (-1 != iResult)
	{
		string strFileName = fd.name;
		nlohmann::ordered_json jsonFile;

		if (FAILED(CJson_Manager::Parse(_strDirectory + strFileName, jsonFile)))
		{
			iResult = _findnext(iHandle, &fd);
			continue;
		}

		strFileName.erase(strFileName.begin() + strFileName.size() - 5, strFileName.end());

		for (auto itSubclass = jsonFile.begin(); itSubclass != jsonFile.end(); ++itSubclass)
		{
			for (auto itFile = itSubclass.value().begin(); itFile != itSubclass.value().end(); ++itFile)
				m_audios[strFileName][itSubclass.key()].push_back(itFile.key());
		}

		iResult = _findnext(iHandle, &fd);
	}
}

void CS_Audio_Controller::_Show_Category()
{
	auto vMainSize = ImGui::GetWindowSize();

	ImGui::BeginChild("##Category Child", ImVec2(vMainSize.x * 0.3f, 300.f), true);

	ImGui::SeparatorText("Category");

	if (ImGui::BeginListBox("##Category List", ImVec2(200.f, 0.f)))
	{
		for (auto& pairCategory : m_audios)
		{
			bool bIsSelected = m_strSelectCategory == pairCategory.first;

			auto it = m_completedInitialize.find(pairCategory.first);

			if (it != m_completedInitialize.end())
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.f, 1.f, 0.f, 1.f));
			else
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 1.f));

			if (ImGui::Selectable(pairCategory.first.c_str(), bIsSelected))
				m_strSelectCategory = pairCategory.first;

			ImGui::PopStyleColor(1);
		}

		ImGui::EndListBox();
	}

	ImGui::EndChild();
}

void CS_Audio_Controller::_Show_Subclass()
{
	auto vMainSize = ImGui::GetWindowSize();

	ImGui::BeginChild("##Sub class Child", ImVec2(vMainSize.x * 0.3f, 300.f), true);

	ImGui::SeparatorText("Sub class");

	if (ImGui::BeginListBox("##Sub class List", ImVec2(200.f, 0.f)))
	{
		auto iter = m_audios.find(m_strSelectCategory);
		
		if (iter == m_audios.end())
		{
			ImGui::EndListBox();
			ImGui::EndChild();

			return;
		}
		
		for (auto& pairSubclass : m_audios[m_strSelectCategory])
		{
			bool bIsSelected = m_strSelectSubclass == pairSubclass.first;

			if (ImGui::Selectable(pairSubclass.first.c_str(), bIsSelected))
				m_strSelectSubclass = pairSubclass.first;
		}

		ImGui::EndListBox();
	}

	ImGui::EndChild();
}

void CS_Audio_Controller::_Show_Audios()
{
	auto vMainSize = ImGui::GetWindowSize();

	ImGui::BeginChild("##Audios Child", ImVec2(vMainSize.x * 0.3f, 300.f), true);

	ImGui::SeparatorText("Audios");

	if (ImGui::BeginListBox("##Audios List", ImVec2(200.f, 0.f)))
	{
		auto itCategory = m_audios.find(m_strSelectCategory);

		if (itCategory == m_audios.end())
		{
			ImGui::EndListBox();
			ImGui::EndChild();

			return;
		}

		auto itSubclass = m_audios[itCategory->first].find(m_strSelectSubclass);

		if (itSubclass == m_audios[itCategory->first].end())
		{
			ImGui::EndListBox();
			ImGui::EndChild();

			return;
		}

		if (ImGui::BeginTable("##Audio Table", 2, ImGuiTableFlags_ScrollX))
		{
			ImGui::TableSetupColumn("##Audio Tag");
			ImGui::TableSetupColumn("##Play Audio List");

			for (auto& strTag : m_audios[m_strSelectCategory][m_strSelectSubclass])
			{
				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::Text(strTag.c_str());

				ImGui::TableSetColumnIndex(1);

				ImGui::PushID(strTag.c_str());

				if (ImGui::SmallButton("Play"))
				{
					auto it = m_completedInitialize.find(m_strSelectCategory);

					// 못찾으면 없다는 뜻. 최초 초기화
					if (it == m_completedInitialize.end())
					{
						m_completedInitialize.emplace(m_strSelectCategory);
						CGameInstance::Get_Instance()->Load_AudioClips("../Bin/DataFiles/Audios/" + m_strSelectCategory + ".json");
					}

					CGameInstance::Get_Instance()->Play_Audio(strTag);
				}

				ImGui::PopID();
			}

			ImGui::EndTable();
		}

		ImGui::EndListBox();
	}

	ImGui::EndChild();
}

void CS_Audio_Controller::_Channel_Controller()
{
	if (ImGui::SmallButton("Reset Channels"))
		CGameInstance::Get_Instance()->Stop_AllChannels();

	ImGui::SameLine();

	if (ImGui::SmallButton("Reload"))
		_Reload("../Bin/DataFiles/Audios/");
}

void CS_Audio_Controller::_Naive_List()
{
	auto vMainSize = ImGui::GetWindowSize();

	ImGui::BeginChild("##Naive Child", ImVec2(vMainSize.x * 0.6f, 300.f), true);

	auto vChildSize = ImGui::GetWindowSize();

	if (ImGui::BeginListBox("##Naive List", ImVec2(vChildSize.x * 0.9f, vChildSize.y * 0.9f)))
	{
		if (ImGui::BeginTable("##Naive Table", 2, ImGuiTableFlags_ScrollX))
		{
			ImGui::TableSetupColumn("Tag");
			ImGui::TableSetupColumn("Play");

			for (auto& strTag : m_vecNaives)
			{
				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::Text(strTag.c_str());

				ImGui::TableSetColumnIndex(1);

				ImGui::PushID(strTag.c_str());

				if (ImGui::SmallButton("Play"))
					CGameInstance::Get_Instance()->Play_Audio(strTag);

				ImGui::PopID();
			}

			ImGui::EndTable();
		}

		ImGui::EndListBox();
	}

	if (ImGui::SmallButton("Load Folders"))
		ImGuiFileDialog::Instance()->OpenDialog("LoadNaives", "Choose a Directory", nullptr, ".", 1, nullptr, ImGuiFileDialogFlags_Modal);

	if (ImGuiFileDialog::Instance()->Display("LoadNaives"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			auto filePath = ImGuiFileDialog::Instance()->GetCurrentPath() + "\\";
			_Load_Folder(filePath);
		}

		ImGuiFileDialog::Instance()->Close();
	}

	ImGui::EndChild();
}

void CS_Audio_Controller::_Load_Folder(const string& _strDirectory)
{
	_finddata_t fd;

	string strFindPath = _strDirectory + "*.*";
	auto iHandle = _findfirst(strFindPath.c_str(), &fd);

	if (-1 == iHandle)
		return;

	auto iResult = 0;

	while (-1 != iResult)
	{
		string strFileName = fd.name;

		if (4 >= strFileName.size())
		{
			iResult = _findnext(iHandle, &fd);
			continue;
		}

		char szFileName[MAX_PATH] = "";
		strcpy_s(szFileName, strFileName.c_str());
		PathRemoveExtensionA(szFileName);
		
		string strTag = szFileName;

		if (FAILED(CGameInstance::Get_Instance()->Add_AudioClip(_strDirectory + strFileName, strTag, AUDIO_CLIP_TYPE::SFX, 1.f, false)))
		{
			iResult = _findnext(iHandle, &fd);
			continue;
		}

		m_vecNaives.push_back(strTag);		

		iResult = _findnext(iHandle, &fd);
	}
}

void CS_Audio_Controller::_Reload(const string& _strDirectory)
{
	for (auto& audios : m_audios)
	{
		for (auto& category : audios.second)
			category.second.clear();
		
		audios.second.clear();
	}

	m_audios.clear();
	m_strSelectCategory.clear();
	m_strSelectSubclass.clear();
	m_strSelectAudio.clear();
	m_completedInitialize.clear();
	m_vecNaives.clear();

	CGameInstance::Get_Instance()->Clear_AudioClips();

	_Load_JsonList(_strDirectory);
}

CS_Audio_Controller* CS_Audio_Controller::Create()
{
	auto pInstance = new CS_Audio_Controller();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created: CS_Audio_Controller::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CS_Audio_Controller::Free()
{
	for (auto& pairAudios : m_audios)
	{
		for (auto& keyValue : pairAudios.second)
			keyValue.second.clear();

		pairAudios.second.clear();
	}

	m_audios.clear();
}