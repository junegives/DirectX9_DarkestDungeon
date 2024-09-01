#pragma once

#include"Engine_Define.h"

BEGIN(Engine)

class ENGINE_DLL CFrame
{
public:
	explicit CFrame();
	virtual ~CFrame();

public:
	_bool		IsPermit_Call(const _float& fTimeDelta);

public:
	HRESULT		Ready_Frame(const _float& fCallLimit);

private:
	_float				m_fCallLimit;
	_float				m_fAccTimeDelta;

public:
	static shared_ptr<CFrame> Create(const _float& fCallLimit);

private:
	virtual void	Free();
};


END
