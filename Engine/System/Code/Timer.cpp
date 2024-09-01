#include "Timer.h"

CTimer::CTimer() : m_fTimeDelta(0.f)
{									   
}									  

CTimer::~CTimer()
{
}

HRESULT CTimer::Ready_Timer(void)
{
	QueryPerformanceCounter(&m_FrameTime);
	QueryPerformanceCounter(&m_FixTime);
	QueryPerformanceCounter(&m_LastTime);
	QueryPerformanceFrequency(&m_CpuTick);

	return S_OK;
}

void CTimer::Update_TimeDelta(void)
{
	QueryPerformanceCounter(&m_FrameTime);

	if (m_FrameTime.QuadPart - m_FixTime.QuadPart >= m_CpuTick.QuadPart)
	{
		QueryPerformanceFrequency(&m_CpuTick);
		m_FixTime = m_FrameTime;
	}

	m_fTimeDelta = float(m_FrameTime.QuadPart - m_LastTime.QuadPart) / m_CpuTick.QuadPart;

	m_LastTime = m_FrameTime;
}

shared_ptr<CTimer> CTimer::Create(void)
{
	std::shared_ptr<CTimer> pInstance = std::make_shared<CTimer>();

	if (FAILED(pInstance->Ready_Timer()))
	{
		pInstance.reset();
		MSG_BOX("Timer Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CTimer::Free()
{
}
