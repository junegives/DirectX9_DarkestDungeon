#include "Frame.h"

CFrame::CFrame() : m_fCallLimit(0.f) , m_fAccTimeDelta(0.f)
{
}

CFrame::~CFrame()
{
}

_bool CFrame::IsPermit_Call(const _float& fTimeDelta)
{
	m_fAccTimeDelta += fTimeDelta;

	if (m_fAccTimeDelta >= m_fCallLimit)
	{
		m_fAccTimeDelta = 0.f;
		return true;
	}

	return false;
}

HRESULT CFrame::Ready_Frame(const _float& fCallLimit)
{
	m_fCallLimit = 1.f / fCallLimit;

	return S_OK;
}

shared_ptr<CFrame> CFrame::Create(const _float& fCallLimit)
{
	std::shared_ptr<CFrame> pInstance = std::make_shared<CFrame>();

	if (FAILED(pInstance->Ready_Frame(fCallLimit)))
	{
		pInstance.reset();

		//MSG_BOX("Frame Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CFrame::Free()
{
}
