#include "Engine_pch.h"
#include "Engine_Defines.h"
#include "Engine_Functions.h"

// Singleton
#include "Event_Manager.h"

// GameObject
#include "GameObject.h"

// Component
#include "Collider.h"

// GameInstance
#include "GameInstance.h"

// Test, need to erase when not to debug
#include "Animator.h"

namespace CFunc
{
	function<void()> CFunc::Create_Notify_Collider_On(CGameObject* _pOwner, class CAnimator* _pAnimator, const string& _strTag)
	{
		function<void()> func = [pOwner = _pOwner, pAnimator = _pAnimator, strTag = wstring(_strTag.begin(), _strTag.end())]()
		{
			auto pCollider = pOwner->Find_ColliderCom(strTag);

			if (nullptr == pCollider)
				return;

			cout << "TrackPosition : " + to_string(pAnimator->Get_TrackPosition()) << "\n";
			cout << "On collider\n";


			static_cast<CCollider*>(pCollider)->Set_Enable(true);
		};

		return func;
	}

	function<void()> CFunc::Create_Notify_Collider_Off(class CGameObject* _pOwner, class CAnimator* _pAnimator, const string& _strTag)
	{
		function<void()> func = [pOwner = _pOwner, pAnimator = _pAnimator, strTag = wstring(_strTag.begin(), _strTag.end())]()
		{
			auto pCollider = pOwner->Find_ColliderCom(strTag);

			if (nullptr == pCollider)
				return;

			cout << "TrackPosition : " + to_string(pAnimator->Get_TrackPosition()) << "\n";
			cout << "Off collider\n";

			static_cast<CCollider*>(pCollider)->Set_Enable(false);
		};

		return func;
	}

	function<void()> CFunc::Create_Notify_Sound_Play(class CGameObject* _pOwner, class CAnimator* _pAnimator, string _szSoundTag)
	{
		function<void()> func = [strTag = _szSoundTag, pAnimator = _pAnimator]()
		{
			CGameInstance::Get_Instance()->Play_Audio(strTag);
			cout << "TrackPosition : " + to_string(pAnimator->Get_TrackPosition()) << "\n";
			cout << strTag << ", " << "Player sound\n";
		};

		return func;
	}

	function<void()> CFunc::Create_Notify_Effect_Create(class CGameObject* _pOwner, class CAnimator* _pAnimator, string _szEffectTag, any _Args)
	{
		// To-Do : Effect_Desc에서 가져가야 할 내용이랑
		// 이펙트 owner는 desc에서 가질지 아니면 매개 변수로 던질지 정해야 함.
		function<void()> func = [pAnimator = _pAnimator]()
		{
			cout << "TrackPosition : " + to_string(pAnimator->Get_TrackPosition()) << "\n";
			cout << "Effect create\n";
		};

		return func;
	}

	function<void()> CFunc::Create_Notify_GameObject_Event(class CGameObject* _pOwner, _int _iIndex, any _Args)
	{
		function<void()> func = [pOwner = _pOwner, iIndex = _iIndex]()
		{
			pOwner->Execute_Event_Function(iIndex);
		};

		return func;
	}

	_uint CFunc::Add_Event_PopObject(const wstring& _strPoolTag, any _args)
	{
		EventMsg tMsg;
		tMsg.eEventType = EVENT_TYPE::POP_OBJECTPOOL;

		auto pTag = new wstring(_strPoolTag);

		tMsg.wParam = reinterpret_cast<WPARAM>(pTag);
		tMsg.cParam = _args;

		CEvent_Manager::Get_Instance()->Add_EventMsg(tMsg);

		return 0;
	}

	HRESULT Add_Event_DeleteObject(CGameObject* _pObject)
	{
		EventMsg tMsg;
		tMsg.eEventType = EVENT_TYPE::DESTROY_INSTANCE;
		tMsg.wParam = reinterpret_cast<WPARAM>(_pObject);

		return CGameInstance::Get_Instance()->Add_EventMsg(tMsg);
	}

	HRESULT Add_Event_Enable_UI(LPARAM _lParam, WPARAM _wParam, any _cParam)
	{
		EventMsg msg;
		msg.eEventType = EVENT_TYPE::UI_ENABLE;
		msg.lParam = _lParam;
		msg.wParam = _wParam;
		msg.cParam = _cParam;

		return CGameInstance::Get_Instance()->Add_EventMsg(msg);
	}

	HRESULT Add_Event_Inteval_Function(float _fWaitTime, float _fInterval, int _iCount, function<void(float)> _func, CGameObject* _pCaller, bool _bRepeat)
	{
		INTERVAL_FUNCTION_DESC desc;
		desc.fWaitTime = _fWaitTime;
		desc.fInterval = _fInterval;
		desc.iCount = _iCount;
		desc.func = _func;
		desc.pCaller = _pCaller;
		desc.bRepeat = _bRepeat;

		EventMsg msg;
		msg.eEventType = EVENT_TYPE::ADD_INTERVAL_FUNCTION;
		msg.cParam = desc;

		return CGameInstance::Get_Instance()->Add_EventMsg(msg);
	}

	_int Generate_Random_Int(_int _iMin, _int _iMax)
	{
		static random_device randomDevice;
		static mt19937_64 generator(randomDevice());

		if (_iMin > _iMax)
			swap(_iMin, _iMax);

		uniform_int_distribution distribution(_iMin, _iMax);

		return distribution(generator);
	}

	_float Generate_Random_Float(_float _fMin, _float _fMax)
	{
		static random_device randomDevice;
		static mt19937_64 generator(randomDevice());

		if (_fMin > _fMax)
			swap(_fMin, _fMax);

		uniform_real_distribution<_float> distribution(_fMin, _fMax);

		return distribution(generator);
	}

	_float3 Generate_Random_Vector3(const _float3& _vMin, const _float3& _vMax)
	{
		_float3 v = {};

		v.x = Generate_Random_Float(_vMin.x, _vMax.x);
		v.y = Generate_Random_Float(_vMin.y, _vMax.y);
		v.z = Generate_Random_Float(_vMin.z, _vMax.z);

		return v;
	}

	_float4 Generate_Random_Vector4(const _float4& _vMin, const _float4& _vMax)
	{
		_float4 v = {};

		v.x = Generate_Random_Float(_vMin.x, _vMax.x);
		v.y = Generate_Random_Float(_vMin.y, _vMax.y);
		v.z = Generate_Random_Float(_vMin.z, _vMax.z);
		v.w = Generate_Random_Float(_vMin.w, _vMax.w);

		return v;
	}

	size_t Generate_Hash(const string& _str)
	{
		static hash<string> generator;

		return generator(_str);
	}

	size_t Generate_Hash(const wstring& _wstr)
	{
		static hash<wstring> generator;

		return generator(_wstr);
	}

	ID3D11Buffer* Create_InstanceBuffer(ID3D11Device* _pDevice, const vector<MESH_INSTANCE_DATA>& _vecData, _uint _iMeshType)
	{
		ID3D11Buffer* pBuffer = nullptr;

		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));

		D3D11_SUBRESOURCE_DATA initData;
		ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));

		_uint iNumInstance = static_cast<_uint>(_vecData.size());
		_uint iInstanceStride = 0;

		void* pInstance = nullptr;

		// 0 : Anim, 1 : NonAnim
		if (0 == _iMeshType)
		{
			iInstanceStride = sizeof(VTXMESHTOONANIMINSTANCE);
			pInstance = new VTXMESHTOONANIMINSTANCE[iNumInstance];

			memcpy(pInstance, &_vecData.front(), sizeof(_float4x4) * 2 * iNumInstance);
		}

		else if (1 == _iMeshType)
		{
			iInstanceStride = sizeof(VTXMESHTOONINSTANCE);
			pInstance = new VTXMESHTOONINSTANCE[iNumInstance];

			memcpy(pInstance, &_vecData.front(), sizeof(_float4x4) * 2 * iNumInstance);
		}

		bufferDesc.ByteWidth = iInstanceStride * iNumInstance;
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;

		initData.pSysMem = pInstance;

		_pDevice->CreateBuffer(&bufferDesc, &initData, &pBuffer);

		Safe_Delete_Array(pInstance);

		return pBuffer;
	}

	ID3D11Buffer* Create_InstanceBuffer(ID3D11Device* _pDevice, const vector<CELL_INSTANCE_DATA>& _vecData)
	{
		ID3D11Buffer* pBuffer = nullptr;

		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));

		D3D11_SUBRESOURCE_DATA initData;
		ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));

		_uint iNumInstance = static_cast<_uint>(_vecData.size());
		_uint iInstanceStride = sizeof(VTXCELLINSTANCE);

		VTXCELLINSTANCE* pInstance = nullptr;

		pInstance = new VTXCELLINSTANCE[iNumInstance];

		memcpy(pInstance, &_vecData.front(), iInstanceStride * iNumInstance);

		bufferDesc.ByteWidth = iInstanceStride * iNumInstance;
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;

		initData.pSysMem = pInstance;

		_pDevice->CreateBuffer(&bufferDesc, &initData, &pBuffer);

		Safe_Delete_Array(pInstance);

		return pBuffer;
	}

	string To_utf8(const wstring& _wstr)
	{
		//static wstring_convert<codecvt_utf8<wchar_t>> utf8_conv;

		//return utf8_conv.to_bytes(_wstr);
		return string();

	}

	unsigned int Get_CurLevel(_bool _bIsLoading)
	{
		return static_cast<unsigned int>(CGameInstance::Get_Instance()->Get_CurLevel(_bIsLoading));
	}
}