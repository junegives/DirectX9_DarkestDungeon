#include"pch.h"
#include "MainApp.h"
#include"SceneMgr.h"
#include"Weald_Dungeon.h"
#include"Ruin_Dungeon.h"
#include "Village.h"
#include"CameraMgr.h"
#include"UIMgr.h"
#include"PickingMgr.h"
#include"SoundMgr.h"
#include"MainLogo.h"
#include"LightMgr.h"
#include "EffectMgr.h"
#include "ParticleMgr.h"
#include "GameMgr.h"

CMainApp::CMainApp()
{
}

CMainApp::~CMainApp()
{
	Free();
}

HRESULT CMainApp::Ready_MainApp()
{
	ShowCursor(false);
	AllocConsole();
	srand((unsigned int)time(NULL));
	if (FAILED(SetUp_DefaultSetting(&m_pGraphicDev))) {
		return E_FAIL;
	}

	CResourceMgr::GetInstance()->ReadyResource(m_pGraphicDev);
	CResourceMgr::GetInstance()->MainTitleTextureLoad();

	CEffectMgr::GetInstance()->SetEffectMgr(m_pGraphicDev);
	CParticleMgr::GetInstance()->SetParticleMgr(m_pGraphicDev);

	CSoundMgr::GetInstance()->Initialize();
	CPickingMgr::GetInstance()->ReadyPickingMgr(m_pGraphicDev);

    shared_ptr<CScene> pMainLogo = make_shared<CMainLogo>(m_pGraphicDev);
	Engine::ChangeScene(pMainLogo);
	pMainLogo->ReadyScene();

    return S_OK;
}

_int CMainApp::Update_MainApp(const _float& fTimeDelta)
{
	Engine::Update_InputDev();
	Engine::UpdateScene(fTimeDelta);
	
    return 0;
}

void CMainApp::LateUpdate_MainApp()
{
   Engine::LateUpdateScene();

}

void CMainApp::Render_MainApp()
{
	if (CGameMgr::GetInstance()->GetGameState() != EGameState::VIDEO)
	{
		m_pDeviceClass->Render_Begin(D3DXCOLOR(0.f, 0.f, 0.f, 1.f));

		Engine::RenderScene(m_pGraphicDev);

		Engine::Render_End();
	}
}

HRESULT CMainApp::SetUp_DefaultSetting(LPDIRECT3DDEVICE9* ppGraphicDev)
{
	if (FAILED(Engine::Ready_GraphicDev(g_hWnd, MODE_WIN, WINCX, WINCY, &m_pDeviceClass))) {
		//MSG_BOX("Ready GraphicDev Failed!");
		return E_FAIL;
	}

	*ppGraphicDev = m_pDeviceClass->Get_GraphicDev();
	(*ppGraphicDev)->AddRef();

	(*ppGraphicDev)->SetRenderState(D3DRS_LIGHTING, FALSE);

	(*ppGraphicDev)->SetRenderState(D3DRS_ZENABLE, TRUE);		// Z버퍼에 깊이 값을 무조건 기록은 하지만 자동 정렬을 할지 말지 결정
	(*ppGraphicDev)->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);  // Z버퍼에 깊이 값을 기록할 지 결정


	//Font Setting------------------------------------------------

	if (FAILED(Engine::Ready_Font((*ppGraphicDev), L"Font_Default", L"이순신 돋움체 M", 15, 25, FW_HEAVY))) {
		//MSG_BOX("Ready InputDev Failed!");
		return E_FAIL;
	}

	if (FAILED(Engine::Ready_Font((*ppGraphicDev), L"Font_Default_Small", L"이순신 돋움체 L", 5, 15, FW_HEAVY))) {
		//MSG_BOX("Ready InputDev Failed!");
		return E_FAIL;
	}

	if (FAILED(Engine::Ready_Font((*ppGraphicDev), L"Font_Default_Medium", L"이순신 돋움체 L", 10, 20, FW_HEAVY))) {
		//MSG_BOX("Ready InputDev Failed!");
		return E_FAIL;
	}

	if (FAILED(Engine::Ready_Font((*ppGraphicDev), L"Font_Default_Bold", L"이순신 돋움체 B", 15, 25, FW_HEAVY))) {
		//MSG_BOX("Ready InputDev Failed!");
		return E_FAIL;
	}

	if (FAILED(Engine::Ready_Font((*ppGraphicDev), L"Font_Default_Light", L"이순신 돋움체 L", 11, 22, FW_MEDIUM))) {
		//MSG_BOX("Ready InputDev Failed!");
		return E_FAIL;
	}

	if (FAILED(Engine::Ready_Font((*ppGraphicDev), L"Font_Point", L"이순신 Regular", 15, 27, FW_HEAVY))) {
		//MSG_BOX("Ready InputDev Failed!");
		return E_FAIL;
	}

	if (FAILED(Engine::Ready_Font((*ppGraphicDev), L"Font_Point_Title", L"이순신 Regular", 20, 33, FW_HEAVY))) {
		//MSG_BOX("Ready InputDev Failed!");
		return E_FAIL;
	}

	if (FAILED(Engine::Ready_Font((*ppGraphicDev), L"Font_Point_Small", L"이순신 Regular", 10, 20, FW_HEAVY))) {
		//MSG_BOX("Ready InputDev Failed!");
		return E_FAIL;
	}

	//Font Setting Fin--------------------------------------------------------


	// Dinput
	if (FAILED(Engine::Ready_InputDev(g_hInst, g_hWnd))) {
		//MSG_BOX("Ready InputDev Failed!");
		return E_FAIL;
	}

	(*ppGraphicDev)->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	(*ppGraphicDev)->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	return S_OK;
}

void CMainApp::Free()
{

	FreeConsole();

	CLightMgr::DestroyInstance();
	CEffectMgr::DestroyInstance();
	CParticleMgr::DestroyInstance();
	CSoundMgr::DestroyInstance();
	CPickingMgr::DestroyInstance();
	CUIMgr::DestroyInstance();
	CCameraMgr::DestroyInstance();
	Engine::Release_Utility();
	Engine::Release_System();
}
