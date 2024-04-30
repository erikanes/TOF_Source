#pragma once

#include "ImGui_Base.h"

BEGIN(Client)

class CImGui_BaseWindow abstract : public CImGui_Base
{
protected:
	CImGui_BaseWindow(const string& _strName, const ImVec2& _vPos = ImVec2(0.f, 0.f), const ImVec2& _vSize = ImVec2(0.f, 0.f));
	virtual ~CImGui_BaseWindow() = default;

public:
	_bool IsEnable() const { return m_bIsEnable; }

	void Enable() { m_bIsEnable = true; }
	void Disable() { m_bIsEnable = false; }

protected:
	_bool m_bIsEnable = { false };
	
	ImVec2 m_vSize = ImVec2(0.f, 0.f);
	ImVec2 m_vPos = ImVec2(0.f, 0.f);
};

END