#include "Engine_pch.h"
#include "Engine_Defines.h"
#include "Json_Manager.h"

IMPLEMENT_SINGLETON(CJson_Manager);

map<string, nlohmann::ordered_json>  CJson_Manager::m_mapJsons;

CJson_Manager::CJson_Manager()
{
}

HRESULT CJson_Manager::Save_File(const string& _strFilePath, nlohmann::ordered_json& _json)
{
	ofstream file(_strFilePath);
	
	file << _json.dump(4);

	file.close();

	return S_OK;
}

HRESULT CJson_Manager::Parse(const string& _strFilePath, nlohmann::ordered_json& _json)
{
	ifstream file(_strFilePath);

	if (!file.is_open())
		return E_FAIL;

	char szFileName[MAX_PATH] = { "" };
	_splitpath_s(_strFilePath.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);

	_json = nlohmann::ordered_json::parse(file);
	m_mapJsons.insert({ szFileName , _json});

	if (_json.empty())
		return E_FAIL;

	return S_OK;
}


nlohmann::ordered_json CJson_Manager::Get_Json(const string& _szKey)
{
	auto iter = m_mapJsons.find(_szKey);

	if (m_mapJsons.end() == iter)
		return string("");
	else
		return iter->second;
}

void CJson_Manager::Free()
{
	m_mapJsons.clear();
}