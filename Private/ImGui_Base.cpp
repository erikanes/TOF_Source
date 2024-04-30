#include "Client_pch.h"
#include "ImGui_Base.h"

CImGui_Base::CImGui_Base()
	: m_ID(g_uniqueID++)
{
}

HRESULT CImGui_Base::Initialize()
{
	return S_OK;
}