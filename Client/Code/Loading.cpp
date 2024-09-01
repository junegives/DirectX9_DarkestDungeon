#include <pch.h>
#include "Loading.h"
#include"ResourceMgr.h"
#include"Weald_Dungeon.h"
#include"SceneMgr.h"
#include"Village.h"
#include"Ruin_Dungeon.h"
#include "GameMgr.h"

#include "BossMap.h"

CLoading::CLoading(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
	, m_bFinish(false)
	, m_eLoading(LOADING_END)
{
	ZeroMemory(m_szString, sizeof(m_szString));
	m_pGraphicDev->AddRef();
}

CLoading::~CLoading()
{
	Free();
}

HRESULT CLoading::Ready_Loading(LOADINGID eLoading)
{
	InitializeCriticalSection(&m_Crt);
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Thread_Main, this, 0, nullptr);
	m_eLoading = eLoading;

	return S_OK;
}

_uint CLoading::Loading_ForStage()
{
	CResourceMgr::GetInstance()->BaseTextureLoad();
	CResourceMgr::GetInstance()->UITextureLoad();

	m_pNextScene = make_shared<CWeald_Dungeon>(m_pGraphicDev);
	CSceneMgr::GetInstance()->SetLoadingState(true);
	CSceneMgr::GetInstance()->SetReadyScene(m_pNextScene);
	m_pNextScene->ReadyScene();

	m_bFinish = true;

	//CGameMgr::GetInstance()->SetGameState(EGameState::PRCESS);

	return 0;

}

_uint CLoading::Loading_ForVillage()
{
	CResourceMgr::GetInstance()->VillageTextureLoad();

	m_pNextScene = make_shared<CVillage>(m_pGraphicDev);
	CSceneMgr::GetInstance()->SetReadyScene(m_pNextScene);
	m_pNextScene->ReadyScene();
	m_bFinish = true;

	return 0;
}

_uint CLoading::Loading_ForRuin()
{
	CResourceMgr::GetInstance()->RuinDungeonTextureLoad();

	m_pNextScene = make_shared<CRuin_Dungeon>(m_pGraphicDev);
	CSceneMgr::GetInstance()->SetReadyScene(m_pNextScene);
	m_pNextScene->ReadyScene();
	m_bFinish = true;

	return 0;
}

_uint CLoading::Loading_ForBossMap()
{
	CResourceMgr::GetInstance()->BossMapTextureLoad();

	m_pNextScene = make_shared<CBossMap>(m_pGraphicDev);
	CSceneMgr::GetInstance()->SetReadyScene(m_pNextScene);
	m_pNextScene->ReadyScene();
	m_bFinish = true;

	return 0;
}

unsigned int CLoading::Thread_Main(void* pArg)
{
	CLoading* pLoading = reinterpret_cast<CLoading*>(pArg);
	_uint			iFlag(0);

	EnterCriticalSection(pLoading->Get_Crt());

	switch (pLoading->Get_LoadingID())
	{
	case LOADING_STAGE:
		iFlag = pLoading->Loading_ForStage();
		break;
	case LOADING_VILLAGE:
		iFlag = pLoading->Loading_ForVillage();
		break;
	case LOADING_RUIN:
		iFlag = pLoading->Loading_ForRuin();
		break;
	case LOADING_BOSSMAP:
		iFlag = pLoading->Loading_ForBossMap();
		break;
	}

	LeaveCriticalSection(pLoading->Get_Crt());

	return iFlag;
}

void CLoading::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);
	CloseHandle(m_hThread);
	DeleteCriticalSection(&m_Crt);

	Safe_Release(m_pGraphicDev);
}
