#pragma once

#include "Engine_Define.h"

BEGIN(Engine)

class ENGINE_DLL CTimer 
{
public:
	explicit CTimer();
	virtual ~CTimer();

public:
	_float			Get_TimeDelta() { return m_fTimeDelta; }

public:
	HRESULT		Ready_Timer(void);
	void		Update_TimeDelta(void);

private:
	LARGE_INTEGER			m_FrameTime;
	LARGE_INTEGER			m_FixTime;
	LARGE_INTEGER			m_LastTime;
	LARGE_INTEGER			m_CpuTick;

	_float					m_fTimeDelta;

public:
	static shared_ptr<CTimer> Create(void);

private:
	virtual void	Free();
};

END