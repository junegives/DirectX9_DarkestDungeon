#include "TimeMgr.h"

IMPLEMENT_SINGLETON(CTimerMgr)

CTimerMgr::CTimerMgr()
{
}

CTimerMgr::~CTimerMgr()
{
	Free();
}

_float CTimerMgr::Get_TimeDelta(const _tchar* pTimerTag)
{
	std::shared_ptr<CTimer> pTimer = Find_Timer(pTimerTag);

	if (nullptr == pTimer) {
		return 0.f;
	}

	return pTimer->Get_TimeDelta();
}

void CTimerMgr::Update_TimeDelta(const _tchar* pTimerTag)
{
	std::shared_ptr<CTimer> pTimer = Find_Timer(pTimerTag);

	if (nullptr == pTimer)
		return;

	pTimer->Update_TimeDelta();

}

HRESULT CTimerMgr::Ready_Timer(const _tchar* pTimerTag)
{
	shared_ptr<CTimer> pTimer = Find_Timer(pTimerTag);

	if (nullptr != pTimer)
		return E_FAIL;

	pTimer = CTimer::Create();

	if (nullptr == pTimer)
		return E_FAIL;

	m_mapTimers.insert({ pTimerTag, pTimer });

	return S_OK;
}

shared_ptr<CTimer> CTimerMgr::Find_Timer(const _tchar* pTimerTag)
{
	auto		iter = find_if(m_mapTimers.begin(), m_mapTimers.end(), CTag_Finder(pTimerTag));

	if (iter == m_mapTimers.end())
		return nullptr;

	return iter->second;
}

void CTimerMgr::Free(void)
{
	//for_each(m_mapTimers.begin(), m_mapTimers.end(), CDeleteMap());
	m_mapTimers.clear();
}
