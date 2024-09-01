#include "pch.h"
#include "Card.h"
#include "PickingMgr.h"
#include "Export_Utility.h"

#include "CardGame.h"
#include "SoundMgr.h"

CCard::CCard(LPDIRECT3DDEVICE9 _pGraphicDev, _int _iCardNum, _int _iCardImgNum)
    :CGameObject(_pGraphicDev), m_iCardImgNum(_iCardImgNum), m_iCardNum(_iCardNum)
{
}

CCard::~CCard()
{
}

HRESULT CCard::ReadyGameObject()
{
    return S_OK;
}

_int CCard::UpdateGameObject(const _float& fTimeDelta)
{
    _int iExit = __super::UpdateGameObject(fTimeDelta);

    if (!m_bIsCorrect)
        CPickingMgr::GetInstance()->AddList(shared_from_this());
    

    return iExit;
}

void CCard::LateUpdateGameObject()
{
}

void CCard::RenderGameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->GetWorld());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    if (m_bIsFlip) {
        m_pTextureCom->SetTexture(m_iCardImgNum);
    }
    else {
        m_pTextureCom->SetTexture(0);
    }

    m_pRCtexCom->RenderBuffer();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

}

void CCard::AddComponent()
{
    shared_ptr<CComponent> pComponent;

    pComponent = m_pRCtexCom = make_shared<CRcTex>(m_pGraphicDev);
    dynamic_pointer_cast<CRcTex>(m_pRCtexCom)->ReadyBuffer();
    m_mapComponent[ID_STATIC].insert({ L"Com_RcTex", pComponent });

    pComponent = m_pTransformCom = make_shared<CTransform>(_vec3(0.f, 0.f, 0.f), _vec3(1.f, 1.f, 1.f), _vec3(0.f, 0.f, 0.f));
    m_pTransformCom->ReadyTransform();
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });
    m_pTransformCom->SetPosition(m_vPos.x, m_vPos.y, m_vPos.z);
    m_pTransformCom->SetScale(m_vScale.x, m_vScale.y, m_vScale.z);

    pComponent = m_pTextureCom = make_shared<CTexture>(m_pGraphicDev);
    m_pTextureCom->SetTextureKey(L"Minigame_CardBack", TEX_NORMAL);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Texture",pComponent });

    pComponent = m_pColliderCom = make_shared<CCollider>(m_pGraphicDev);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider",pComponent });
    m_pColliderCom->SetScale({ 1.3f, 1.3f, 1.3f });
    m_pColliderCom->SetPos(m_pTransformCom->GetPos());

}

void CCard::PickingObj()
{
    if (!m_bIsFlip)
        m_bIsFlip = true;

    m_pTextureCom->SetTextureKey(L"Minigame_Card", TEX_NORMAL);

    if (nullptr != m_pCardGame.lock()) {
        m_pCardGame.lock()->FlipCard(dynamic_pointer_cast<CCard>(shared_from_this()));
        
        CSoundMgr::GetInstance()->StopSound(CHANNELID::PLAYER);
        CSoundMgr::GetInstance()->PlaySound(L"Minigame_Flip.wav", CHANNELID::PLAYER, 1.f);

        shared_ptr<CEffect> pEffect = CEffectMgr::GetInstance()->GetEffect();


        if (pEffect) {
            pEffect->SetAnimEffect(L"Minigame_ClickEffect", _vec3(m_vPos.x, m_vPos.y, m_vPos.z -0.3f), _vec3(1.5f, 1.5f, 1.5f), 0.7f, false);
            pEffect->SetActive(true);
        }

        //Minigame_Flip
    }


}

void CCard::FlipCard()
{
}

void CCard::ResetFlip()
{
    m_pTextureCom->SetTextureKey(L"Minigame_CardBack", TEX_NORMAL);
    m_bIsFlip = false;
}
