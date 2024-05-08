#pragma once

#include "VIBuffer_Instance.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instance_Combined final : public CVIBuffer_Instance
{
private:
	CVIBuffer_Instance_Combined(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CVIBuffer_Instance_Combined(const CVIBuffer_Instance_Combined& _rhs);
	virtual ~CVIBuffer_Instance_Combined() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;
	virtual HRESULT Initialize_Prototype(ID3D11Buffer* _pVB, ID3D11Buffer* _pIB, const VIBUFFERINFO& _tInfo, const vector<MESH_INSTANCE_DATA>& _vecData);
	virtual HRESULT Initialize_Prototype(ID3D11Buffer* _pVB, ID3D11Buffer* _pIB, ID3D11Buffer* _pInstanceVB, const VIBUFFERINFO& _tInfo, _uint _iNumInstance, _uint _iInstanceStride);

public:
	static CVIBuffer_Instance_Combined* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext,
		ID3D11Buffer* _pVB, ID3D11Buffer* _pIB, const VIBUFFERINFO& _tInfo, const vector<MESH_INSTANCE_DATA>& _vecData);
	
	static CVIBuffer_Instance_Combined* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext,
		ID3D11Buffer* _pVB, ID3D11Buffer* _pIB, ID3D11Buffer* _pInstanceVB, const VIBUFFERINFO& _tInfo, _uint _iNumInstance, _uint _iInstanceStride = sizeof(VTXMESHTOONINSTANCE));

	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};

END