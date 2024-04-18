#pragma once

#include "VIBuffer.h"

BEGIN(Client)

class CVIBuffer_LensFlare final : public CVIBuffer
{
private:
	CVIBuffer_LensFlare(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CVIBuffer_LensFlare(const CVIBuffer_LensFlare& _rhs);
	virtual ~CVIBuffer_LensFlare() = default;

public:
	virtual HRESULT Initialize(void* _pArg);
	virtual HRESULT Initialize_Prototype() override;

public:
	static CVIBuffer_LensFlare* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};

END