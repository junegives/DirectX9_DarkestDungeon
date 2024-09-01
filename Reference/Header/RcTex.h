#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CRcTex : public CVIBuffer
{
public:
	explicit CRcTex();
	explicit CRcTex(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CRcTex(const CRcTex& rhs);
	virtual ~CRcTex();

public:
	virtual HRESULT ReadyBuffer(void) override;
	virtual void RenderBuffer(void) override;

private:
	virtual void Free() override;

};

END