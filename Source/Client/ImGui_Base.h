#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CImGui_Base abstract : public CBase
{
protected:
	CImGui_Base();
	virtual ~CImGui_Base() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_float _fTimeDelta) = 0;

#pragma region Getter
	const string& Get_Name() const { return m_strName; }
#pragma endregion

public:
	virtual void Free() = 0;

protected:
	string m_strName;
	const _int m_ID = { 0 };

private:
	static inline _int g_uniqueID = { static_cast<_int>(typeid(int).hash_code()) & 0x3fffffff };
};

END