#include "FrameMgr.h"

IMPLEMENT_SINGLETON(CFrameMgr)

CFrameMgr::CFrameMgr()
{
}

CFrameMgr::~CFrameMgr()
{
	Free();
}

_bool CFrameMgr::IsPermit_Call(const _tchar* pFrameTag, const _float& fTimeDelta)
{
	shared_ptr<CFrame> pFrame = Find_Frame(pFrameTag);

	if (nullptr == pFrame)
		return false;

	return pFrame->IsPermit_Call(fTimeDelta);
}

HRESULT CFrameMgr::Ready_Frame(const _tchar* pFrameTag, const _float& fCallLimit)
{
	shared_ptr<CFrame> pFrame = Find_Frame(pFrameTag);

	if (nullptr != pFrame)
		return E_FAIL;

	pFrame = CFrame::Create(fCallLimit);

	if (nullptr == pFrame) {
		return E_FAIL;
	}

	m_mapFrame.insert({ pFrameTag, pFrame });

	return S_OK;
}

shared_ptr<CFrame> CFrameMgr::Find_Frame(const _tchar* pFrameTag)
{
	auto	iter = find_if(m_mapFrame.begin(), m_mapFrame.end(), CTag_Finder(pFrameTag));

	if (iter == m_mapFrame.end())
		return nullptr;

	return iter->second;
}

void CFrameMgr::Free()
{
	//for_each(m_mapFrame.begin(), m_mapFrame.end(), CDeleteMap());
	//m_mapFrame.clear();
}
