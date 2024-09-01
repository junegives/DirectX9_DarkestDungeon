#pragma once
#include "Engine_Define.h"
#include"Scene.h"

class CLoading
{
public:
	enum LOADINGID { LOADING_STAGE, LOADING_VILLAGE, LOADING_RUIN, LOADING_BOSSMAP, LOADING_END };

public:
	explicit CLoading(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLoading();

public:
	_bool					Get_Finish() const { return m_bFinish; }
	_bool					GetLoadingDone() const { return m_bLoadingDone; }
	CRITICAL_SECTION* Get_Crt() { return &m_Crt; }
	LOADINGID				Get_LoadingID() const { return m_eLoading; }
	const _tchar* Get_String() const { return m_szString; }

public:
	shared_ptr<CScene> GetCompleteScene() { return m_pNextScene; }

public:
	HRESULT				Ready_Loading(LOADINGID eLoading);
	_uint				Loading_ForStage();
	_uint				Loading_ForVillage();
	_uint				Loading_ForRuin();
	_uint				Loading_ForBossMap();


public:
	static unsigned int CALLBACK		Thread_Main(void* pArg);

private:
	HANDLE				m_hThread;
	CRITICAL_SECTION	m_Crt;
	LPDIRECT3DDEVICE9	m_pGraphicDev;

	//Checking State
	_bool				m_bFinish;
	_bool				m_bLoadingDone = false;

	LOADINGID			m_eLoading;
	_tchar				m_szString[128];

private:

	shared_ptr<CScene> m_pNextScene;

private:
	virtual void			Free();

};

