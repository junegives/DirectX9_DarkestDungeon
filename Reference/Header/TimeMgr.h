#pragma once
#include "Timer.h"

BEGIN(Engine)

class ENGINE_DLL CTimerMgr 
{
	DECLARE_SINGLETON(CTimerMgr)
private:
	explicit CTimerMgr();
	virtual ~CTimerMgr();

public:
	_float		Get_TimeDelta(const _tchar* pTimerTag);
	void		Update_TimeDelta(const _tchar* pTimerTag);

public:
	HRESULT		Ready_Timer(const _tchar* pTimerTag);

private:
	shared_ptr<CTimer> Find_Timer(const _tchar* pTimerTag);

private:
	map<const _tchar*, shared_ptr<CTimer>>		m_mapTimers;

private:
	virtual void		Free(void);
};

END