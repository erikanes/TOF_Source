#pragma once

#include "VIBuffer.h"

BEGIN(Client)

class CVIBuffer_Cloud final : public CVIBuffer
{
private:
	CVIBuffer_Cloud(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CVIBuffer_Cloud(const CVIBuffer_Cloud& _rhs);
	virtual ~CVIBuffer_Cloud() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint _iResolution, _float _fWidth);
	virtual HRESULT Initialize(void* _pArg) override;

public:
	static CVIBuffer_Cloud* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _uint _iResolution, _float _fWidth);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};

END