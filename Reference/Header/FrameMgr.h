#pragma once
#include "Frame.h"

BEGIN(Engine)

class ENGINE_DLL CFrameMgr
{
	DECLARE_SINGLETON(CFrameMgr)

private:
	explicit CFrameMgr();
	~CFrameMgr();

public:
	_bool			IsPermit_Call(const _tchar* pFrameTag, const _float& fTimeDelta);
	HRESULT			Ready_Frame(const _tchar* pFrameTag, const _float& fCallLimit);

private:
	shared_ptr<CFrame> Find_Frame(const _tchar* pFrameTag);

private:
	map<const _tchar*, shared_ptr<CFrame>>			m_mapFrame;

private:
	virtual void		Free();
};

END
