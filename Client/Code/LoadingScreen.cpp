#include"pch.h"
#include "LoadingScreen.h"

#include"Village.h"
#include"Ruin_Dungeon.h"
#include"UIMgr.h"
#include"Export_System.h"

#include"Export_Utility.h"

CLoadingScreen::CLoadingScreen(LPDIRECT3DDEVICE9 _pGraphicDev, ELoadingSceneType _eType)
    :CScene(_pGraphicDev) , m_eLoadingType(_eType)
{
}

CLoadingScreen::~CLoadingScreen()
{
}

HRESULT CLoadingScreen::ReadyScene()
{

    CSceneMgr::GetInstance()->SetLoadingState(true);

    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

    m_pLoading = make_shared<CLoading>(m_pGraphicDev);

    if (ELoadingSceneType::VILLAGE == m_eLoadingType) {
        m_pLoading->Ready_Loading(CLoading::LOADING_VILLAGE);
    }
    else if (ELoadingSceneType::RUIN == m_eLoadingType) {
        m_pLoading->Ready_Loading(CLoading::LOADING_RUIN);
    }
    else if (ELoadingSceneType::BOSS == m_eLoadingType) {
        m_pLoading->Ready_Loading(CLoading::LOADING_BOSSMAP);
    }

    if (!m_pLoading)
        return E_FAIL;

    Ready_Layer_UI(L"Layer_UI");

    for (auto& iter : m_mapLayer) {
        iter.second->ReadyLayer();
    }

    return S_OK;
}

_int CLoadingScreen::UpdateScene(const _float& _fTimeDelta)
{
    _int iExit = __super::UpdateScene(_fTimeDelta);

    return iExit;
}

void CLoadingScreen::LateUpdateScene()
{
	if (true == m_pLoading->Get_Finish())
	{

		if (GetAsyncKeyState(VK_RETURN))
		{
            Engine::ChangeScene(m_pLoading->GetCompleteScene());
		}
	}

}

void CLoadingScreen::RenderScene()
{
    if (true == m_pLoading->Get_Finish())
    {
        _vec2 vInfoStartPos = { 0.f , 430.f };
        _vec2 vInfoEndPos = { WINCX , 480.f };
        
        RenderFontCenter(L"Font_Default_Light", L"계속 진행하려면 [Enter]를 누르세요. ", &vInfoStartPos, &vInfoEndPos, YELLOW);
    }
}

HRESULT CLoadingScreen::Ready_Layer_UI(tstring pLayerTag)
{
    shared_ptr<CLayer> m_pLayer = make_shared<CLayer>();
    m_mapLayer.insert({ pLayerTag, m_pLayer });

    shared_ptr<CGameObject> m_pBG = make_shared<CLoadingBack>(m_pGraphicDev, m_eLoadingType);
    m_pLayer->CreateGameObject(L"Obj_BackGround", m_pBG);

    dynamic_pointer_cast<CLayer>(m_pLayer)->AwakeLayer();

    return S_OK;
}
