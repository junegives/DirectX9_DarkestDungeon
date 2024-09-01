#include "Animator.h"
#include"ResourceMgr.h"

CAnimator::CAnimator(LPDIRECT3DDEVICE9 pGraphicDev)
    :Engine::CComponent(pGraphicDev)
{
}

CAnimator::~CAnimator()
{
}

void CAnimator::SetAnimTexture()
{
    m_pGraphicDev->SetTexture(0, ((*m_vecTexture)[m_tAnimFrame.iCurrentFrame])->pTexture);
}

_int CAnimator::UpdateComponent(const _float& fTimeDelta)
{
    if (!m_vecTexture)
        return 0;

    m_bFinishCount = false;
    m_fActTime += fTimeDelta;

    if (m_fActTime >= m_tAnimFrame.fTime) {
        m_tAnimFrame.iCurrentFrame += 1;
        m_fActTime = 0.f;

        if (m_tAnimFrame.iMaxFrame <= m_tAnimFrame.iCurrentFrame) {
            m_tAnimFrame.iCurrentFrame = 0;
            m_bFinishCount = true;
        }
    }

    return __super::UpdateComponent(fTimeDelta);
}

void CAnimator::LateUpdateComponent()
{
    __super::LateUpdateComponent();

}

HRESULT CAnimator::SetAnimKey(const tstring& _keyName, float _fAnimTime)
{

    m_vecTexture = CResourceMgr::GetInstance()->GetTexture(_keyName, TEXTUREID::TEX_NORMAL);

    if (m_vecTexture == nullptr) {
        m_bEnable = false;
        return E_FAIL;
    }

    m_tAnimFrame.iCurrentFrame = 0;
    m_tAnimFrame.fTime = _fAnimTime;
    m_tAnimFrame.iMaxFrame = (_uint)m_vecTexture->size();

    m_fActTime = 0.f;

    m_bEnable = true;

    return S_OK;
}
