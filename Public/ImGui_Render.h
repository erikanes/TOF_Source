#pragma once

#include "ImGui_BaseWindow.h"

#include "Renderer.h"

BEGIN(Engine)
class CRender_Manager;
class CCamera;
END

BEGIN(Client)

class CImGui_Window_RenderTarget;

class CImGui_Render final : public CImGui_BaseWindow
{
private:
	typedef struct tagDebugCameraDesc
	{
		_float4x4 matWorld;
		_float fRotateSpeed;
		_float fSpeed;
	} DEBUG_CAMERA_DESC;

private:
	CImGui_Render();
	virtual ~CImGui_Render() = default;

public:
	virtual HRESULT Initialize(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Tick(_float _fTimeDelta) override;

private:
	void _Draw_DebugInfo();

	void _Control_HDR();
	void _Control_Adaptation();
	void _Control_Bloom();
	void _Control_Glow();
	void _Control_SSAO();
	void _Control_RimLight();
	void _Control_DepthOfField();
	void _Control_Fog();

#pragma region Test
#ifdef _DEBUG
	void _Control_Shader();
	void _Control_Other();
	void _Control_Level();
	void _Control_Dialog();

	CCamera* _Create_Camera();
	void _Change_Camera();
#endif
#pragma endregion

public:
	static CImGui_Render* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;

#ifdef  _DEBUG
	_bool Is_Render_Map();
#endif


private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	CRender_Manager* m_pRender_Manager = { nullptr };
	CImGui_Window_RenderTarget* m_pRenderTargetWindow = { nullptr };

	CCamera* m_pCamera = { nullptr };

	_bool m_bRenderMap = { false };
	_bool m_bCameraInitialized = { false };
	_bool m_bUseFreeCamera = { false };

	// Levels
	map<LEVEL, string> m_mapLevels;
};

END