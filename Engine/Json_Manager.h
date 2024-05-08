#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CJson_Manager final : public CBase
{
	DECLARE_SINGLETON(CJson_Manager);
private:
	static map<string, nlohmann::ordered_json> m_mapJsons;

private:
	CJson_Manager();
	~CJson_Manager() = default;

public:
	static HRESULT					Parse(const string& _strFilePath, nlohmann::ordered_json& _json);
	static HRESULT					Save_File(const string& _strFilePath, nlohmann::ordered_json& _json);
	static nlohmann::ordered_json	Get_Json(const string& _szKey);

public:
	virtual void Free() override;

};

END