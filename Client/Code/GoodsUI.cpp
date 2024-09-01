#include"pch.h"
#include "GoodsUI.h"
#include"Export_Utility.h"
#include"Export_System.h"

CGoodsUI::CGoodsUI(LPDIRECT3DDEVICE9 _pGraphicDev)
    :CUIObj(_pGraphicDev)
{
}

CGoodsUI::~CGoodsUI()
{
}

HRESULT CGoodsUI::ReadyGameObject()
{
    m_vSize = { 211.f , 42.f ,0.f };
    m_vAngle = { 0.f, 0.f, 0.f };
    m_vPos = { -430.f, -310.f, 0.5f };

    m_bVisible = true;
    m_bEnable = true;
    m_bActive = true;

    m_pTransCom->SetScale(m_vSize.x, m_vSize.y, m_vSize.z);
    m_pTransCom->SetPosition(m_vPos.x, m_vPos.y, m_vPos.z);
    m_pTransCom->SetAngle({ 0.f, 0.f, 0.f });

    CUIObj::ReadyGameObject();

    return S_OK;
}

_int CGoodsUI::UpdateGameObject(const _float& fTimeDelta)
{
    int iExit(0);

    //µ· ¿À¸£´Â È¿°ú
    if (m_bGoldChange)
        IncreaseGoods(true);
    else if(m_bHeirloomChange)
        IncreaseGoods(false);

    iExit =  CGameObject::UpdateGameObject(fTimeDelta);
    AddRenderGroup(RENDER_UI, shared_from_this());

    return iExit;
}

void CGoodsUI::LateUpdateGameObject()
{
    __super::LateUpdateGameObject();
}

void CGoodsUI::RenderGameObject()
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

    TCHAR buf[64];

    _stprintf_s(buf, TEXT("%d"), m_iGold);
    _vec2 vPos = { m_vPos.x + WINCX * 0.5f - 110.f, (m_vPos.y * -1.f) + WINCY * 0.5f - 5.f};

    //Gold
    Engine::Render_Font(L"Font_Point", buf, &vPos, D3DXCOLOR(0.878f, 0.807f, 0.6f, 1.f));
    
    _stprintf_s(buf, TEXT("%d"), m_iHeirloom);
    vPos = { m_vPos.x + WINCX * 0.5f +90.f, (m_vPos.y * -1.f) + WINCY * 0.5f - 5.f };
    //Heirloom
    Engine::Render_Font(L"Font_Point", buf, &vPos, D3DXCOLOR(0.878f, 0.807f, 0.6f, 1.f));


    m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

    m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

}

void CGoodsUI::AddComponent()
{
    CUIObj::AddComponent();

    shared_ptr<CComponent> pComponent;

    pComponent = m_pTextureCom = make_shared<CTexture>(m_pGraphicDev);
    m_pTextureCom->SetTextureKey(L"UI_Goods", TEX_NORMAL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture",pComponent });
}

void CGoodsUI::IncreaseGoods(_bool _bGold)
{
    _int iSign = 1;

    if (m_iGoodsGap < 0)
        iSign = -1;

    //°ñµå Value Change
    if (_bGold) {
        if (m_iGoodsGap * iSign < 7) {
            m_iGold += m_iGoodsGap;
            m_iGoodsGap = 0;
            m_bGoldChange = false;
        }
        else {
            m_iGold += 7 * iSign;
            m_iGoodsGap -= 7 * iSign;
        }
    }
    else {
        if (m_iGoodsGap == 0) {
            m_bHeirloomChange = false;
        }
        else {
            m_iHeirloom += 1 * iSign;
            m_iGoodsGap -= 1 * iSign;
        }
    }
}

