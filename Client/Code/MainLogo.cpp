#include"pch.h"
#include "MainLogo.h"
#include"SoundMgr.h"
#include"Weald_Dungeon.h"
#include"BackGround.h"
#include"UIMgr.h"
#include"BackGround.h"
#include"Export_System.h"
#include "Video.h"
#include "GameMgr.h"


CMainLogo::CMainLogo(LPDIRECT3DDEVICE9 _pGraphicDev)
	:CScene(_pGraphicDev)
{
}

CMainLogo::~CMainLogo()
{
}

HRESULT CMainLogo::ReadyScene()
{
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_pLoading = make_shared<CLoading>(m_pGraphicDev);
	m_pLoading->Ready_Loading(CLoading::LOADING_STAGE);

	if (!m_pLoading)
		return E_FAIL;

	Ready_Layer_UI(L"Layer_UI");

	for (auto& iter : m_mapLayer) {
		iter.second->ReadyLayer();
	}

	m_pVideo = make_shared<CVideo>(m_pGraphicDev);
	m_pVideo->Ready_Video(CVideo::VIDEOID::VIDEO_START);

	if (!m_pVideo)
		return E_FAIL;

	//CSoundMgr::GetInstance()->PlayBGM(L"Main_Loading_loop.wav", 1.f);
	//PlayVideo(L"../Bin/Resource/Video/old_road.wmv", g_hWnd);


	return S_OK;
}

_int CMainLogo::UpdateScene(const _float& _fTimeDelta)
{
	_int iExit = __super::UpdateScene(_fTimeDelta);

	if (true == m_pLoading->Get_Finish())
	{
		m_pVideo->ChangeVideo();

		if (!m_bLoadingFin && !m_bIsFirstView) {
			m_pBackGround->SetLoadingState();
			CSoundMgr::GetInstance()->StopAll();
			CSoundMgr::GetInstance()->PlayBGM(L"Main_theme.wav", 1.f);
			m_bLoadingFin = true;
		}

	}

	else
		m_pVideo->Update(_fTimeDelta);

	return iExit;
}

void CMainLogo::LateUpdateScene()
{

	if (true == m_pLoading->Get_Finish())
	{
		if (GetAsyncKeyState(VK_RETURN))
		{
			if (!m_bIsFirstView && m_bLoadingFin) {
				CSoundMgr::GetInstance()->StopAll();
				Engine::ChangeScene(m_pLoading->GetCompleteScene());
				CSceneMgr::GetInstance()->SetLoadingState(false);
			}
		}

		if (GetAsyncKeyState(VK_SPACE)) {

			if (m_bIsFirstView) {
				m_bIsFirstView = false;
				CGameMgr::GetInstance()->SetGameState(EGameState::PRCESS);
				if (m_pVideo)
				{
					m_pVideo->StopVideo();
				}
			}

		}
	}
}

void CMainLogo::RenderScene()
{
	if (m_bIsFirstView && true == m_pLoading->Get_Finish()) {
		tstring strStart = L"Loading Complete";
		_vec2 vStart = { 0.f, 400.f };
		_vec2 vEnd = { 300.f, 500.f };

		RenderFontCenter(L"Font_Point", strStart.c_str(), &vStart, &vEnd, D3DXCOLOR(0.878f, 0.807f, 0.6f, 1.f));
	}
}

HRESULT CMainLogo::Ready_Layer_UI(tstring pLayerTag)
{
	shared_ptr<CLayer> m_pLayer = make_shared<CLayer>();
	m_mapLayer.insert({ pLayerTag, m_pLayer });

	shared_ptr<CGameObject> m_pBG = make_shared<CBackGround>(m_pGraphicDev);
	m_pBackGround = dynamic_pointer_cast<CBackGround>(m_pBG);
	m_pLayer->CreateGameObject(L"Obj_BackGround", m_pBG);

	dynamic_pointer_cast<CLayer>(m_pLayer)->AwakeLayer();

	return S_OK;
}