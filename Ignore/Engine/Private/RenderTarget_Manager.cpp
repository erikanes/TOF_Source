#include "Engine_pch.h"
#include "RenderTarget_Manager.h"
#include "RenderTarget.h"

IMPLEMENT_SINGLETON(CRenderTarget_Manager)

CRenderTarget_Manager::CRenderTarget_Manager()
{
}

HRESULT CRenderTarget_Manager::Add_RenderTarget(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const wstring& _wszRenderTargetTag,
	_uint _iSizeX, _uint _iSizeY, DXGI_FORMAT _ePixelFormat, const _float4& _vClearColor,
	const D3D11_RENDER_TARGET_VIEW_DESC* _pRTVdesc,
	const D3D11_SHADER_RESOURCE_VIEW_DESC* _pSRVdesc,
	const D3D11_UNORDERED_ACCESS_VIEW_DESC* _pUAVdesc)
{
	if (nullptr != Find_RenderTarget(_wszRenderTargetTag))
	{
		MSG_BOX("Already exist the render target which that name.");
		return E_FAIL;
	}

	CRenderTarget* pRenderTarget = CRenderTarget::Create(_pDevice, _pContext, _iSizeX, _iSizeY, _ePixelFormat, _vClearColor, _pRTVdesc, _pSRVdesc, _pUAVdesc);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	m_RenderTargets.emplace(_wszRenderTargetTag, pRenderTarget);

	return S_OK;
}

HRESULT CRenderTarget_Manager::Add_MRT(const wstring& _wszMultiRenderTargetTag, const wstring& _wszRenderTargetTag)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(_wszRenderTargetTag);

	if (nullptr == pRenderTarget)
	{
		MSG_BOX("There is no such render target when Add_MultiRenderTarget");
		return E_FAIL;
	}

	list<CRenderTarget*>* pMRTList = _Find_MRT(_wszMultiRenderTargetTag);

	if (nullptr == pMRTList)
	{
		list<CRenderTarget*>	MRTList;
		MRTList.push_back(pRenderTarget);
		m_MRTs.emplace(_wszMultiRenderTargetTag, MRTList);
	}
	else
		pMRTList->push_back(pRenderTarget);

	Safe_AddRef(pRenderTarget);

	return S_OK;
}

HRESULT CRenderTarget_Manager::Begin_MRT(ID3D11DeviceContext* _pContext, const wstring& _strMRTTag, ID3D11DepthStencilView* _pDepthStencilView)
{
	_pContext->CSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, m_SRVClear);
	_pContext->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, m_SRVClear);

	list<CRenderTarget*>* pMRTList = _Find_MRT(_strMRTTag);

	if (nullptr == pMRTList)
		return E_FAIL;

	ID3D11RenderTargetView* pRenderTargetView = nullptr;
	ID3D11DepthStencilView* pDepthStencilView = nullptr;

	_pContext->OMGetRenderTargets(1, &pRenderTargetView, &pDepthStencilView);

	// 별도로 가져와서 스택에 저장 (LIFO)
	m_resources.emplace(pRenderTargetView, pDepthStencilView);

	ID3D11RenderTargetView* pRenderTargets[8] = { nullptr };

	_uint		iNumRTVs = 0;
	for (auto& pRenderTarget : *pMRTList)
	{
		pRenderTarget->Clear();
		pRenderTargets[iNumRTVs++] = pRenderTarget->Get_RTV();
	}

	// 만약 외부에서 깊이 스텐실 버퍼가 주어지지 않는다면 기존 깊이 스텐실 버퍼를 사용한다
	// 새로운 Render Target으로 지정
	if (nullptr == _pDepthStencilView)
		_pContext->OMSetRenderTargets(iNumRTVs, pRenderTargets, pDepthStencilView);
	else
		_pContext->OMSetRenderTargets(iNumRTVs, pRenderTargets, _pDepthStencilView);

	pRenderTargetView->Release();
	pDepthStencilView->Release();

	return S_OK;
}

HRESULT CRenderTarget_Manager::Begin_MRT_NoClear(ID3D11DeviceContext* _pContext, const wstring& _strMRTTag, ID3D11DepthStencilView* _pDepthStencilView)
{
	_pContext->CSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, m_SRVClear);
	_pContext->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, m_SRVClear);

	list<CRenderTarget*>* pMRTList = _Find_MRT(_strMRTTag);

	if (nullptr == pMRTList)
		return E_FAIL;

	ID3D11RenderTargetView* pRenderTargetView = nullptr;
	ID3D11DepthStencilView* pDepthStencilView = nullptr;

	_pContext->OMGetRenderTargets(1, &pRenderTargetView, &pDepthStencilView);

	// 별도로 가져와서 스택에 저장 (LIFO)
	m_resources.emplace(pRenderTargetView, pDepthStencilView);

	ID3D11RenderTargetView* pRenderTargets[8] = { nullptr };

	_uint		iNumRTVs = 0;

	for (auto& pRenderTarget : *pMRTList)
		pRenderTargets[iNumRTVs++] = pRenderTarget->Get_RTV();

	if (nullptr == _pDepthStencilView)
		_pContext->OMSetRenderTargets(iNumRTVs, pRenderTargets, pDepthStencilView);
	else
		_pContext->OMSetRenderTargets(iNumRTVs, pRenderTargets, _pDepthStencilView);

	pRenderTargetView->Release();
	pDepthStencilView->Release();

	return S_OK;
}

HRESULT CRenderTarget_Manager::End_MRT(ID3D11DeviceContext* _pContext)
{
	ID3D11RenderTargetView* pRenderTargets[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = { nullptr };

	auto& resource = m_resources.top();

	_pContext->OMSetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, pRenderTargets, nullptr);
	_pContext->OMSetRenderTargets(1, &resource.first, resource.second);

	m_resources.pop();

	return S_OK;
}

HRESULT CRenderTarget_Manager::Bind_ShaderResource(CShader* _pShader, const wstring& _strTargetTag, const char* _pConstantName)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(_strTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Bind_ShaderResource(_pShader, _pConstantName);
}

HRESULT CRenderTarget_Manager::Bind_UnorderedAccess(CShader* _pShader, const wstring& _strTargetTag, const char* _pConstantName)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(_strTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Bind_UnorderedAccess(_pShader, _pConstantName);
}

HRESULT CRenderTarget_Manager::Clear_RenderTarget(const wstring& _wstrTag)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(_wstrTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Clear();
}

HRESULT CRenderTarget_Manager::Ready_Debug(const wstring& _wszRenderTargetTag, _float _fCenterXPos, _float _fCenterYPos,
	_float _fSizeX, _float _fSizeY)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(_wszRenderTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Ready_Debug(_fCenterXPos, _fCenterYPos, _fSizeX, _fSizeY);
}

HRESULT CRenderTarget_Manager::Debug_Render(CShader* _pShader, CVIBuffer_Rect* _pVIBuffer, const wstring& _wszMultiRenderTargetTag)
{
	list<CRenderTarget*>* pMRTList = _Find_MRT(_wszMultiRenderTargetTag);

	for (auto& pRenderTarget : *pMRTList)
	{
		pRenderTarget->Debug_Render(_pShader, _pVIBuffer);
	}

	return S_OK;
}

CRenderTarget* CRenderTarget_Manager::Find_RenderTarget(const wstring& _wszRenderTargetTag)
{
	auto	iter = m_RenderTargets.find(_wszRenderTargetTag);

	if (iter == m_RenderTargets.end())
		return nullptr;

	return iter->second;
}

list<class CRenderTarget*>* CRenderTarget_Manager::_Find_MRT(const wstring& _wszMultiRenderTargetTag)
{
	auto	iter = m_MRTs.find(_wszMultiRenderTargetTag);

	if (iter == m_MRTs.end())
		return nullptr;

	return &iter->second;
}

void CRenderTarget_Manager::Free()
{
	for (auto& Pair : m_MRTs)
	{
		for (auto& pRenderTarget : Pair.second)
			Safe_Release(pRenderTarget);

		Pair.second.clear();
	}

	m_MRTs.clear();

	for (auto& Pair : m_RenderTargets)
		Safe_Release(Pair.second);

	m_RenderTargets.clear();
}
