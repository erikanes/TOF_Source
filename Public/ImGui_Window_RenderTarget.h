#pragma once

#include "ImGui_BaseWindow.h"

BEGIN(Engine)
class CRenderTarget;
END

BEGIN(Client)

class CImGui_Window_RenderTarget final : public CImGui_BaseWindow
{
private:
	typedef struct tagRenderTargetValue
	{
		const char* szName = nullptr;
		ID3D11ShaderResourceView* pTexture = nullptr;
		ImVec4 vColor;
	} RENDERTARGET_VALUE;

private:
	CImGui_Window_RenderTarget();
	virtual ~CImGui_Window_RenderTarget() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_float _fTimeDelta) override;

private:
	void _Add_RenderTarget(const string& _strTag, const char* _szName, CRenderTarget* _pRenderTarget, _bool _bKeepAlpha = true);
	void _Draw_RenderTargets();

public:
	static CImGui_Window_RenderTarget* Create();
	virtual void Free() override;

private:
	map<string, vector<RENDERTARGET_VALUE>> m_renderTargets;
};

END