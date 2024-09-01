#include"pch.h"
#include "MouseCursor.h"
#include"Export_Utility.h"

CMouseCursor::CMouseCursor(LPDIRECT3DDEVICE9 _pGraphicDev)
    :CUIObj(_pGraphicDev)
{
}

CMouseCursor::~CMouseCursor()
{
}

HRESULT CMouseCursor::ReadyGameObject()
{
    if (m_bReady) return S_OK;

    m_vSize = { 16.f , 16.f ,0.f };
    m_vAngle = { 0.f, 0.f, 0.f };
    m_vPos = {0.f, 0.f, 0.8f };

    m_bVisible = true;
    m_bEnable = true;
    m_bActive = true;

    m_pTransCom->SetScale(m_vSize.x, m_vSize.y, m_vSize.z);
    m_pTransCom->SetPosition(m_vPos.x, m_vPos.y, m_vPos.z);
    m_pTransCom->SetAngle({ 0.f, 0.f, 0.f });

    CUIObj::ReadyGameObject();

    return S_OK;
}

_int CMouseCursor::UpdateGameObject(const _float& fTimeDelta)
{


    _int iExit(0);
    iExit = CGameObject::UpdateGameObject(fTimeDelta);

    _float fScreenX = 1280.f;
    _float fScreenY = 660.f;

    POINT	ptMouse{};
    GetCursorPos(&ptMouse);
    ScreenToClient(g_hWnd, &ptMouse);

    _float fYPos = (fScreenY * 0.5f + (ptMouse.y * -1.f));
    _float fXPos = ptMouse.x - fScreenX * 0.5f + m_vSize.x;

    _float fYScale = fYPos / 330.f;
    _float fXScale = fXPos / 635.f;


    _float fDir = -1.f;

   if (fXPos < 0) {
       fXScale = 1.f;
    }


    m_pTransCom->SetPosition(ptMouse.x - fScreenX * 0.5f + m_vSize.x , fYPos + (m_vSize.y *(fYScale)), 0.8f);
     

   


    AddRenderGroup(RENDER_MOUSE, shared_from_this());

    return iExit;
}

void CMouseCursor::LateUpdateGameObject() 
{
    CUIObj::LateUpdateGameObject();

}

void CMouseCursor::RenderGameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransCom->GetWorld());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);

    m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0xc0);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

    m_pTextureCom->SetTexture(0);
    m_pRcTexCom->RenderBuffer();

    m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

    m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

}

void CMouseCursor::AddComponent()
{
    CUIObj::AddComponent();

    shared_ptr<CComponent> pComponent;

    pComponent = m_pTextureCom = make_shared<CTexture>(m_pGraphicDev);
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture",pComponent });
    m_pTextureCom->SetTextureKey(L"UI_MouseCursor_Main", TEXTUREID::TEX_NORMAL);

}
