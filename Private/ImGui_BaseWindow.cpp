#include "Client_pch.h"
#include "ImGui_BaseWindow.h"

CImGui_BaseWindow::CImGui_BaseWindow(const string& _strName, const ImVec2& _vPos, const ImVec2& _vSize)
{
	m_strName = _strName;

	m_vPos = _vPos;
	m_vSize = _vSize;
}