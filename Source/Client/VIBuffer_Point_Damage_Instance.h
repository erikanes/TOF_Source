#pragma once

#include "Client_Defines.h"
#include "VIBuffer_Instance.h"

BEGIN(Client)

class CVIBuffer_Point_Damage_Instance : public CVIBuffer_Instance
{
private:
	CVIBuffer_Point_Damage_Instance(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CVIBuffer_Point_Damage_Instance(const CVIBuffer_Point_Damage_Instance& _rhs);
	virtual ~CVIBuffer_Point_Damage_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Late_Tick(_float _fTimeDelta, const vector<VTXDAMAGEINSTANCE>& _vecDesc);

public:
	static CVIBuffer_Point_Damage_Instance* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CComponent* Clone(void* _pArg);
	virtual void Free() override;
};

END