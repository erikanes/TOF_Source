#pragma once

#include "ImGui_Base.h"

BEGIN(Client)

class CImGui_MainMenuBar final : public CImGui_Base
{
private:
	CImGui_MainMenuBar();
	virtual ~CImGui_MainMenuBar();

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_float _fTimeDelta) override;

public:
	static CImGui_MainMenuBar* Create();
	virtual void Free() override;
};

END