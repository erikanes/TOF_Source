#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CRenderTarget_Manager final : public CBase
{
	DECLARE_SINGLETON(CRenderTarget_Manager)
private:
	CRenderTarget_Manager();
	virtual ~CRenderTarget_Manager() = default;

public:
	HRESULT Add_RenderTarget(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const wstring& _strTargetTag, _uint _iSizeX, _uint _iSizeY, DXGI_FORMAT _ePixelFormat, const _float4& _vClearColor,
		const D3D11_RENDER_TARGET_VIEW_DESC* _pRTVdesc = nullptr,
		const D3D11_SHADER_RESOURCE_VIEW_DESC* _pSRVdesc = nullptr,
		const D3D11_UNORDERED_ACCESS_VIEW_DESC* _pUAVdesc = nullptr);

	HRESULT Add_MRT(const wstring& _strMRTTag, const wstring& _strTargetTag);

	HRESULT Begin_MRT(ID3D11DeviceContext* _pContext, const wstring& _strMRTTag, ID3D11DepthStencilView* _pDepthStencilView = nullptr);
	HRESULT Begin_MRT_NoClear(ID3D11DeviceContext* _pContext, const wstring& _strMRTTag, ID3D11DepthStencilView* _pDepthStencilView = nullptr);
	HRESULT End_MRT(ID3D11DeviceContext* _pContext);

	HRESULT Bind_ShaderResource(class CShader* _pShader, const wstring& _strTargetTag, const char* _pConstantName);
	HRESULT Bind_UnorderedAccess(class CShader* _pShader, const wstring& _strTargetTag, const char* _pConstantName);

	HRESULT Clear_RenderTarget(const wstring& _wstrTag);

	HRESULT Ready_Debug(const wstring& _strTargetTag, _float _fX, _float _fY, _float _fSizeX, _float _fSizeY);
	HRESULT Debug_Render(class CShader* _pShader, class CVIBuffer_Rect* _pVIBuffer, const wstring& _strMRTTag);

	class CRenderTarget* Find_RenderTarget(const wstring& _strTargetTag);

public:
	virtual void Free() override;

private:
	list<class CRenderTarget*>* _Find_MRT(const wstring& _strMRTTag);

private:
	map<const wstring, list<class CRenderTarget*>>		m_MRTs;
	map<const wstring, class CRenderTarget*>			m_RenderTargets;
	ID3D11ShaderResourceView* m_SRVClear[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = { nullptr };

	stack<pair<ID3D11RenderTargetView*, ID3D11DepthStencilView*>> m_resources;
};

END