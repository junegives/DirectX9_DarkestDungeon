#include "pch.h"
#include "ScreenEffect.h"
#include "Export_Utility.h"

CScreenEffect::CScreenEffect(LPDIRECT3DDEVICE9 _pGraphicDev)
	:CUIObj(_pGraphicDev)
{
}

CScreenEffect::~CScreenEffect()
{
}

HRESULT CScreenEffect::ReadyGameObject()
{
    if (m_bReady) return S_OK;

    m_vSize = { WINCX * 0.5f, WINCY * 0.5f ,0.f };
    m_vAngle = { 0.f, 0.f, 0.f };
    m_vPos = { 0.f, 0.f, 0.5f };

    m_bVisible = false;
    m_bEnable = true;
    m_bActive = true;

    m_pTransCom->SetScale(m_vSize.x, m_vSize.y, m_vSize.z);
    m_pTransCom->SetPosition(m_vPos.x, m_vPos.y, m_vPos.z);
    m_pTransCom->SetAngle({ 0.f, 0.f, 0.f });

    CUIObj::ReadyGameObject();

    m_bReady = true;

    return S_OK;
}

_int CScreenEffect::UpdateGameObject(const _float& fTimeDelta)
{

    if (!m_bVisible)
        return 0;

    if (EScreenEffect::ENUM_END != m_eCurrentEffect) {


        if (EScreenEffect::FADEIN_WHITE == m_eCurrentEffect) {
            m_fActTime += fTimeDelta;

            if (m_fActTime < 2.f) {
                m_iAlpha += 3;

                if (m_iAlpha > 255) {
                    m_iAlpha = 255;
                }
            }
            else if (m_fActTime >= 2.f && m_fActTime < 5.f) {
                m_iAlpha = 255;

            }
            else {
                m_iAlpha -= 3;

                if (m_iAlpha < 0) {
                    m_iAlpha = 0;
                }
            }



        }
        else {

            m_fActTime += fTimeDelta;
            --m_iAlpha;

        }

        if (m_fActTime >= m_fEffectTime) {

            m_bVisible = false;
            m_fActTime = 0.f;
            m_eCurrentEffect = EScreenEffect::ENUM_END;
        }


    }


    int iExit(0);
    CGameObject::UpdateGameObject(fTimeDelta);
    AddRenderGroup(RENDER_UI, shared_from_this());

	return iExit;
}

void CScreenEffect::LateUpdateGameObject()
{
    if (!m_bVisible)
        return;

    __super::LateUpdateGameObject();

}

void CScreenEffect::RenderGameObject()
{
    if (!m_bVisible)
        return;
    
    if (EScreenEffect::ENUM_END != m_eCurrentEffect) {

        m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransCom->GetWorld());
        m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
        m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
        m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);

        m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
        m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
        
        if (EScreenEffect::ATTACKED == m_eCurrentEffect || EScreenEffect::FADEIN_WHITE == m_eCurrentEffect) {
            m_pGraphicDev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

            m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
            m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
            m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);

            m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(m_iAlpha, 255, 255, 255));
        }

        m_pTextureCom->SetTexture(static_cast<_uint>(m_eCurrentEffect));
        m_pRcTexCom->RenderBuffer();

        m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);

        if (EScreenEffect::ATTACKED == m_eCurrentEffect) {
            m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, 0xffffffff);
        }


        m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, TRUE);
        m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
        m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    }

}

void CScreenEffect::SetScreenEffect(EScreenEffect _eEffect)
{
    if (m_bVisible)
        return;

    m_eCurrentEffect = _eEffect;
    m_bVisible = true;
    if (EScreenEffect::BLOOD == _eEffect) {
        m_fEffectTime = 3.f;
    }
    else if (EScreenEffect::ATTACKED == _eEffect) 
    {
        m_fEffectTime = 1.5f;
        m_iAlpha = 110;

    }
    else if (EScreenEffect::FADEIN_WHITE == _eEffect) {

        m_fEffectTime = 7.f;
        m_iAlpha = 0;
    }


}

void CScreenEffect::AddComponent()
{
    if (m_bReady) return;

    shared_ptr<CComponent> pComponent;

    CUIObj::AddComponent();

    pComponent = m_pTextureCom = make_shared<CTexture>(m_pGraphicDev);
    m_pTextureCom->SetTextureKey(L"UI_ScreenEffect", TEX_NORMAL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture",pComponent });

}
