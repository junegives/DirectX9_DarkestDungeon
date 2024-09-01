#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CCubeTex : public CVIBuffer
{
public:
	explicit CCubeTex();
	explicit CCubeTex(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCubeTex(const CCubeTex& rhs);
	virtual ~CCubeTex();

public:
	virtual HRESULT ReadyBuffer(void) override;
	virtual void RenderBuffer(void) override;


private:
	virtual void Free() override;

};

END
