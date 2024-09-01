#include"pch.h"
#include "PlayerFPSUI.h"
#include"Export_Utility.h"
#include"Export_System.h"

CPlayerFPSUI::CPlayerFPSUI(LPDIRECT3DDEVICE9 _GraphicDev)
	:CUIObj(_GraphicDev)
{
}

CPlayerFPSUI::~CPlayerFPSUI()
{
}

HRESULT CPlayerFPSUI::ReadyGameObject()
{
    if (m_bReady) return S_OK;

    m_vSize = { 180.f , 118.f ,0.f };
    m_vAngle = { 0.f, 0.f, 0.f };
    m_vPos = { 640.f - m_vSize.x, -240.f, 0.5f };

    m_bVisible = false;
    m_bEnable = true;
    m_bActive = true;

    m_pTransCom->SetScale(m_vSize.x, m_vSize.y, m_vSize.z);
    m_pTransCom->SetPosition(m_vPos.x, m_vPos.y, m_vPos.z);
    m_pTransCom->SetAngle({ 0.f, 0.f, 0.f });

    CUIObj::ReadyGameObject();

	return S_OK;
}

_int CPlayerFPSUI::UpdateGameObject(const _float& fTimeDelta)
{

	_int iExit(0);
	iExit = CGameObject::UpdateGameObject(fTimeDelta);

    AddRenderGroup(RENDERID::RENDER_UI, shared_from_this());

	return iExit;
}

void CPlayerFPSUI::LateUpdateGameObject()
{



}

void CPlayerFPSUI::RenderGameObject()
{
    if (!m_bVisible)
        return;


    //Shield

    m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    //m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pShieldTransCom->GetWorld());

    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);

    m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(120, 255, 255, 255));

    //m_pShieldTextureCom->SetTexture(0);
    //m_pShieldRCTexCom->RenderBuffer();

    m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, 0xffffffff);

    m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);


    //Player HUD

    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransCom->GetWorld());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);

    m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0xc0);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

    m_pTextureCom->SetTexture(0);
    m_pRcTexCom->RenderBuffer();
    

    if (m_iHPGap != 0) {
       if (m_iHPGap >= 1) {

            m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pHPTransCom[0]->GetWorld());
            m_pHPTextureCom[0]->SetTexture(0);
            m_pHPRCTexCom[0]->RenderBuffer();


            for (int i = 0; i < m_iHPGap - 1; ++i) {
                m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pHPTransCom[1]->GetWorld());
                m_pHPTextureCom[1]->SetTexture(0);
                m_pHPRCTexCom[1]->RenderBuffer();
            }
        }
    }

    TCHAR buf[64];

    _stprintf_s(buf, TEXT("%d / %d"), m_iHP , m_iMaxHP);

    _vec2 vStart = { 745.f, 417.f };
    _vec2 vEnd = {795.f, 527.f };

    m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

    m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

    RenderFontCenter(L"Font_Default_Medium", buf, &vStart, &vEnd, WHITE);

}

void CPlayerFPSUI::AddComponent()
{
	shared_ptr<CComponent> pComponent;

	CUIObj::AddComponent();

	pComponent = m_pTextureCom = make_shared<CTexture>(m_pGraphicDev);
	m_pTextureCom->SetTextureKey(L"Player_HUD_bottom_left", TEX_NORMAL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture",pComponent });

    //Empty HP Bar
    for (int i = 0; i < 2; ++i) {

        tstring strCurNum = to_wstring(i);
        tstring strComName = L"Com_HP_Transform" + strCurNum;
        tstring strComName1 = L"Com_HP_RcTex" + strCurNum;
        tstring strComName2 = L"Com_HP_Texture" + strCurNum;


        pComponent = m_pHPTextureCom[i] = make_shared<CTexture>(m_pGraphicDev);
        m_mapComponent[ID_STATIC].insert({ strComName2 ,pComponent });

        pComponent = m_pHPRCTexCom[i] = make_shared<CRcTex>(m_pGraphicDev);
        m_pHPRCTexCom[i]->ReadyBuffer();
        m_mapComponent[ID_STATIC].insert({ strComName1 ,pComponent });

        pComponent = m_pHPTransCom[i] = make_shared<CTransform>();
        m_pHPTransCom[i]->ReadyTransform();
        m_mapComponent[ID_DYNAMIC].insert({ strComName ,pComponent });
    }


   // //Shield
   // pComponent = m_pShieldTextureCom = make_shared<CTexture>(m_pGraphicDev);
   // m_pShieldTextureCom->SetTextureKey(L"Player_UI_Shield", TEX_NORMAL);
   // m_mapComponent[ID_STATIC].insert({ L"Com_Shield_Texture_Shield",pComponent});
   //
   // pComponent = m_pShieldRCTexCom = make_shared<CRcTex>(m_pGraphicDev);
   // m_pShieldRCTexCom->ReadyBuffer();
   // m_mapComponent[ID_STATIC].insert({ L"Com_Shield_RcTex",pComponent });
   //
   //
   // pComponent = m_pShieldTransCom = make_shared<CTransform>();
   // m_pShieldTransCom->ReadyTransform();
   // m_pShieldTransCom->SetPosition(0.f, 0.f, 0.f);
   // m_pShieldTransCom->SetAngle({ 0.f, 0.f, 0.f });
   // m_pShieldTransCom->SetScale(WINCX * 0.5f, WINCY * 0.5f, 0.f);
   //
   // m_mapComponent[ID_DYNAMIC].insert({ L"Com_Shield_Transform",pComponent });

    //
    m_pHPTextureCom[0]->SetTextureKey(L"Player_HP_Empty2", TEX_NORMAL);
    m_pHPTextureCom[1]->SetTextureKey(L"Player_HP_Empty1", TEX_NORMAL);

    m_pHPTransCom[0]->SetPosition(382.f, -321, 0.f);
    m_pHPTransCom[0]->SetAngle({0.f, 0.f, 0.f});
    m_pHPTransCom[0]->SetScale(16.f, 16.f, 0.f);

    m_pHPTransCom[1]->SetAngle({ 0.f, 0.f, 0.f });
    m_pHPTransCom[1]->SetScale(11.f, 16.f, 0.f);
    m_pHPTransCom[1]->SetPosition(398.f + m_pHPTransCom[1]->GetScale()->x, -321.f , 0.f);



}

void CPlayerFPSUI::SetHP(_int _iHP)
{
    m_iHP = _iHP;
    m_iHPGap = static_cast<_int>((m_iMaxHP - m_iHP) / 10);

    m_pHPTransCom[1]->SetScale(11.f * static_cast<_float>(m_iHPGap), 16.f, 0.f);
    m_pHPTransCom[1]->SetPosition(398.f + m_pHPTransCom[1]->GetScale()->x, -321.f, 0.f);

}

