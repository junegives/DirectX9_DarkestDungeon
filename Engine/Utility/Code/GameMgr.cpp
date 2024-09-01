#include "GameMgr.h"

IMPLEMENT_SINGLETON(CGameMgr)

CGameMgr::CGameMgr()
{
}

CGameMgr::~CGameMgr()
{
}

// 추후 수정 필요할수도,,,,
void CGameMgr::SetGameState(EGameState _eState)
{
	m_eCurrentState = _eState;

	switch (m_eCurrentState)
	{
	case Engine::EGameState::PRCESS:
		m_pPlayer->SetLock(false);
		break;
	case Engine::EGameState::PAUSE:
		break;
	case Engine::EGameState::LOCK:
		m_pPlayer->SetLock(true);
		break;
	case Engine::EGameState::ENUM_END:
		break;
	default:
		break;
	}
}