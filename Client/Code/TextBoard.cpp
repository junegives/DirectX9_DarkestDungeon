#include "pch.h"
#include "TextBoard.h"
#include "Export_Utility.h"
#include "Export_System.h"

CTextBoard::CTextBoard(LPDIRECT3DDEVICE9 _pGraphicDev)
    : CUIObj(_pGraphicDev)
{
}

CTextBoard::~CTextBoard()
{
}

HRESULT CTextBoard::ReadyGameObject()
{
    m_bVisible = false;
    m_bEnable = true;
    m_bActive = true;

    return S_OK;
}

_int CTextBoard::UpdateGameObject(const _float& fTimeDelta)
{
    if (!m_bVisible)
        return 0;

    if (fTimeDelta > 5.f)
        return 0;

    m_fActTime -= fTimeDelta;

    if (m_fActTime <= 0.f)
    {
        m_bVisible = false;
        m_fActTime = 0.f;
        m_strText = L"";
    }

    CGameObject::UpdateGameObject(fTimeDelta);
    AddRenderGroup(RENDER_UI, shared_from_this());

    return 0;
}

void CTextBoard::LateUpdateGameObject()
{
}

void CTextBoard::RenderGameObject()
{
    if (!m_bVisible)
        return;

    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransCom->GetWorld());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, TRUE);

    m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    m_pGraphicDev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

    m_pTextureCom->SetTexture(0);
    m_pRcTexCom->RenderBuffer();

    _vec2 vPosLT = { 0.f, (-m_vPos.y + WINCY / 2.f - m_vScale.y / 2.f) / 720.f * 490.f };
    _vec2 vPosLT2 = { 0.f, (WINCY / 2.f - m_vPos.y + - m_vScale.y / 2.f)};
    _vec2 vPosTestLT = { 0.f, 450.f };
    _vec2 vPosRB = { 1280.f, (-m_vPos.y + WINCY / 2.f + m_vScale.y / 2.f) / 720.f * 490.f };
    _vec2 vPosRB2 = { 1280.f, (WINCY / 2.f - m_vPos.y + m_vScale.y / 2.f)};
    _vec2 vPosTestRB = { 1280.f, 500.f };

    Engine::RenderFontCenter(L"Font_Point_Title", m_strText.c_str(), &vPosLT, &vPosRB, D3DXCOLOR(0.878f, 0.807f, 0.6f, 1.f));

    m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

    m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

}

void CTextBoard::ActiveUI(tstring _strKey, _vec3 _vPos, _vec3 _vScale, _float _fActTime)
{
    m_strText = _strKey;
    m_vPos = _vPos;
    m_vScale = _vScale;

    m_pTransCom->SetPosition(m_vPos.x, m_vPos.y, m_vPos.z);
    m_pTransCom->SetScale(m_vScale.x, m_vScale.y, m_vScale.z);

    m_fActTime = _fActTime;
    m_bVisible = true;
}

void CTextBoard::AddComponent()
{
    shared_ptr<CComponent> pComponent;
    CUIObj::AddComponent();

    pComponent = m_pTextureCom = make_shared<CTexture>(m_pGraphicDev);
    m_pTextureCom->SetTextureKey(L"Backboard_Text", TEX_NORMAL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });
}