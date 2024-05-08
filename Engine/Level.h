#pragma once

#include <mutex>
#include <future>
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CLevel abstract : public CBase
{
public:
	typedef struct tInitData
	{
		Level eCurLevel;
	}LEVELDESC;

protected:
	CLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel() = default;

public:
	virtual HRESULT Initialize(void* _pArg = nullptr);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void*	SendDataWhenRelease() = 0;
	virtual void	ReceiveDataWhenCreate(void* _pData) = 0;

public:
	virtual Level			Get_Level(_bool _bIsLoading = false) { return m_eLevel; }
	HRESULT					Add_Usual_Marked_Function(MARKED_FUNCTION_TAG _eTag, function<void(any)> _func);
	function<void(any)>		Get_Usual_Marked_Function(MARKED_FUNCTION_TAG _eTag);

	_bool Is_CleaningUp() const { return !m_bPrevLevelCleared; }

protected:
	virtual void Initialize_RenderSetting();

private:
	HRESULT _ClearPrevLevelResources(_uint _eCurrentLevel);

public:
	virtual void Free() override;

protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	Level m_eLevel = static_cast<Level>(0);
	map<MARKED_FUNCTION_TAG, function<void(any)>> m_MapUsualMarkedFunctions;

	_bool m_bPrevLevelCleared = { false };
	future<HRESULT> m_future;

	_bool m_bKeepPrototypeObject = { true };

private:
	mutex m_mutex = {};
	thread m_thread = {};
	promise<HRESULT> m_promise;
};

END