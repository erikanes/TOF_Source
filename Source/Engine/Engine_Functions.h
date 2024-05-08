#pragma once

namespace Engine
{
	class CGameObject;
	class CAnimator;
	class CLevel;
}

namespace CFunc
{
	typedef CLevel*(*pFuncLevelCreator)(ID3D11Device*, ID3D11DeviceContext*, _uint);

#pragma region Event
	extern ENGINE_DLL function<void()> Create_Notify_Collider_On(class CGameObject* _pOwner, class CAnimator* _pAnimator, const string& _strTag);
	extern ENGINE_DLL function<void()> Create_Notify_Collider_Off(class CGameObject* _pOwner, class CAnimator* _pAnimator, const string& _strTag);
	extern ENGINE_DLL function<void()> Create_Notify_Sound_Play(class CGameObject* _pOwner, class CAnimator* _pAnimator, string _szSoundTag);
	extern ENGINE_DLL function<void()> Create_Notify_Effect_Create(class CGameObject* _pOwner, class CAnimator* _pAnimator, string _szEffectTag, any _Args);
	extern ENGINE_DLL function<void()> Create_Notify_GameObject_Event(class CGameObject* _pOwner, _int _iIndex, any _Args);
#pragma endregion

#pragma region Event
	extern ENGINE_DLL _uint Add_Event_PopObject(const wstring& _strPoolTag, any _args);
	extern ENGINE_DLL HRESULT Add_Event_DeleteObject(class CGameObject* _pObject);
	extern ENGINE_DLL HRESULT Add_Event_Enable_UI(LPARAM _lParam, WPARAM _wParam, any _cParam);
	extern ENGINE_DLL HRESULT Add_Event_Inteval_Function(float _fWaitTime, float _fInterval, int _iCount, function<void(float)> _func, class CGameObject* _pCaller, bool _bRepeat);
#pragma endregion

#pragma region Random Generator
	extern ENGINE_DLL _int Generate_Random_Int(_int _iMin, _int _iMax);
	extern ENGINE_DLL _float Generate_Random_Float(_float _fMin, _float _fMax);
	extern ENGINE_DLL _float3 Generate_Random_Vector3(const _float3& _vMin, const _float3& _vMax);
	extern ENGINE_DLL _float4 Generate_Random_Vector4(const _float4& _vMin, const _float4& _vMax);
#pragma endregion

#pragma region Hash
	extern ENGINE_DLL size_t Generate_Hash(const string& _str);
	extern ENGINE_DLL size_t Generate_Hash(const wstring& _wstr);
#pragma endregion

#pragma region Create InstanceBuffer
	extern ENGINE_DLL ID3D11Buffer* Create_InstanceBuffer(ID3D11Device* _pDevice, const vector<MESH_INSTANCE_DATA>& _vecData, _uint _iMeshType = 1);
	extern ENGINE_DLL ID3D11Buffer* Create_InstanceBuffer(ID3D11Device* _pDevice, const vector<CELL_INSTANCE_DATA>& _vecData);
#pragma endregion

#pragma region Json Convert
	extern ENGINE_DLL string To_utf8(const wstring& _wstr);
#pragma endregion

#pragma region General
	extern ENGINE_DLL unsigned int Get_CurLevel(_bool _bIsLoading = false);
#pragma endregion
};
