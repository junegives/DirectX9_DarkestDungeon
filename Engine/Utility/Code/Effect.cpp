#include "Effect.h"

#include "Component.h"
#include "RcTex.h"
#include "Transform.h"
#include "Animator.h"
#include "Texture.h"

#include "ResourceMgr.h"
#include "EffectMgr.h"
#include "Renderer.h"
#include "GameMgr.h"


CEffect::CEffect(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{ 
}

CEffect::~CEffect()
{
}

HRESULT CEffect::ReadyGameObject()
{
    return S_OK;
}

_int CEffect::UpdateGameObject(const _float& fTimeDelta)
{
    m_fDeltaTime = fTimeDelta;

    _int iExit = __super::UpdateGameObject(m_fDeltaTime);

    if (!m_bLoop)
    {
        if (m_bAnimation)
        {
            if (m_pAnimatorCom->CheckFinish())
            {
                CEffectMgr::GetInstance()->ReturnEffect(dynamic_pointer_cast<CEffect>(shared_from_this()));
            }
        }

        else
        {
            if (m_fAnimTime <= 0.f)
            {
                CEffectMgr::GetInstance()->ReturnEffect(dynamic_pointer_cast<CEffect>(shared_from_this()));
            }

            else
            {
                m_fAnimTime -= m_fDeltaTime;
            }
        }
    }

    if (m_bMoving)
        Move();

    if (m_bFading)
        Fade();

    //ºôº¸µå ½ÃÀÛ
    _matrix matWorld;

    matWorld = *m_pTransformCom->GetWorld();
    SetBillBoard(matWorld);
    m_pTransformCom->SetWorld(&matWorld);

    _vec3 _fPlayerPos = *dynamic_pointer_cast<CTransform>(CGameMgr::GetInstance()->GetPlayer()->GetComponent(L"Com_Transform", ID_DYNAMIC))->GetPos();

    if (matWorld.m[3][2] - _fPlayerPos.z < 0.f)
        m_bChange = true;

    return iExit;
}

void CEffect::LateUpdateGameObject()
{
    if (m_bOrthogonal)
        CRenderer::GetInstance()->AddRenderGroup(RENDER_UI, shared_from_this());
    else
        CRenderer::GetInstance()->AddRenderGroup(RENDER_ALPHA, shared_from_this());
}

void CEffect::RenderGameObject()
{
    if (!m_bActive) return;
     
    if(m_vPos)
        m_pTransformCom->SetPosition(m_vPos->x + m_vPosGap.x, m_vPos->y + m_vPosGap.y, m_vPos->z + m_vPosGap.z);

    else
        m_pTransformCom->SetPosition(m_vPosOrigin.x + m_vPosGap.x, m_vPosOrigin.y + m_vPosGap.y, m_vPosOrigin.z + m_vPosGap.z);

    m_pTransformCom->SetScale(m_vScale.x * m_vScaleGap.x, m_vScale.y * m_vScaleGap.y, m_vScale.z * m_vScaleGap.z);

    //printf("Effect : %d\n", m_iNum);
    
    D3DXMATRIX mat = *m_pTransformCom->GetWorld();

    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->GetWorld());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, m_bZEnable);

    //m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    m_pGraphicDev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

    //m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);

    m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(m_iAlpha, 0, 0, 0));

    if (m_bAnimation)
    {
        m_pAnimatorCom->SetAnimTexture();
        m_pRcTexCom->RenderBuffer();
    }

    else
    {
        if (m_bTwoTexture)
        {
            if (m_bChange)
            {
                m_pTransformCom->SetPosition(m_vPos->x + m_vPosGap.x + 0.4f, m_vPos->y + m_vPosGap.y, m_vPos->z + m_vPosGap.z);
                m_pTransformCom2->SetPosition(m_vPos->x + m_vPosGap.x - 0.4f, m_vPos->y + m_vPosGap.y, m_vPos->z + m_vPosGap.z);
            }

            else
            {
                m_pTransformCom->SetPosition(m_vPos->x + m_vPosGap.x - 0.4f, m_vPos->y + m_vPosGap.y, m_vPos->z + m_vPosGap.z);
                m_pTransformCom2->SetPosition(m_vPos->x + m_vPosGap.x + 0.4f, m_vPos->y + m_vPosGap.y, m_vPos->z + m_vPosGap.z);
            }

            m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->GetWorld());
            m_pTextureCom1->SetTexture(0);
            m_pRcTexCom->RenderBuffer();

            m_pTransformCom2->SetScale(m_vScale.x * m_vScaleGap.x, m_vScale.y * m_vScaleGap.y, m_vScale.z * m_vScaleGap.z);
            m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom2->GetWorld());
            m_pTextureCom2->SetTexture(0);
            m_pRcTexCom->RenderBuffer();
        }

        else
        {
            m_strAnimKey;
            m_pTextureCom1->SetTexture(0);
            m_pRcTexCom->RenderBuffer();
        }
    }

    m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, 0xffffffff);

    m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

    __super::RenderGameObject();
}

void CEffect::AddComponent()
{
    shared_ptr<CComponent> pComponent;

    // RcTex
    pComponent = m_pRcTexCom = make_shared<CRcTex>(m_pGraphicDev);
    m_pRcTexCom->ReadyBuffer();
    m_mapComponent[ID_STATIC].insert({ L"Com_RcTex",pComponent });

    // Transform
    pComponent = m_pTransformCom = make_shared<CTransform>();
    m_pTransformCom->ReadyTransform();
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform",pComponent });

    pComponent = m_pTransformCom2 = make_shared<CTransform>();
    m_pTransformCom2->ReadyTransform();
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform2",pComponent });

    // Animator
    pComponent = m_pAnimatorCom = make_shared<CAnimator>(m_pGraphicDev);
    m_pAnimatorCom->SetAnimKey(m_strAnimKey, 0.05f);
    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animator", pComponent });

    // Texture
    pComponent = m_pTextureCom1 = make_shared<CTexture>(m_pGraphicDev);
    m_pTextureCom1->SetTextureKey(L"", TEX_NORMAL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture1", pComponent });

    pComponent = m_pTextureCom2 = make_shared<CTexture>(m_pGraphicDev);
    m_pTextureCom2->SetTextureKey(L"", TEX_NORMAL);
    m_mapComponent[ID_STATIC].insert({ L"Com_Texture2", pComponent });
}

void CEffect::SetAnimEffect(tstring _strAnimKey, _vec3 _vPos, _vec3 _vScale, _float _fAnimTime, _bool _bOrthogonal, _bool _bZEnable, _bool _bLoop)
{
    printf("Effect %d\n%s\n", m_iNum, _strAnimKey);

    m_strAnimKey = _strAnimKey;
    m_fAnimTime = _fAnimTime;
    m_bLoop = _bLoop;

    m_vPos = nullptr;
    m_vPosOrigin = _vPos;
    m_vScale = _vScale;

    _float fTest = m_fAnimTime / CResourceMgr::GetInstance()->GetTexture(m_strAnimKey, TEX_NORMAL)->size();

    m_pAnimatorCom->SetAnimKey(m_strAnimKey, m_fAnimTime / CResourceMgr::GetInstance()->GetTexture(m_strAnimKey, TEX_NORMAL)->size());

    m_vScaleGap = { 1.f, 1.f, 1.f };
    m_vPosGap = { 0.f, 0.f, 0.f };

    m_bAnimation = true;
    m_bOrthogonal = _bOrthogonal;
    m_bZEnable = _bZEnable;

    //if (m_bOrthogonal) m_bZEnable = false;
}

void CEffect::SetTextureEffect(tstring _strTextureKey, _vec3 _vPos, _vec3 _vScale, _float _fShowTime, _int _iAlpha, _bool _bOrthogonal, _bool _bZEnable)
{
     printf("Effect %d\n%s\n", m_iNum, _strTextureKey);

    m_strAnimKey = _strTextureKey;
    m_fAnimTime = _fShowTime;
    m_bLoop = false;

    m_vPos = nullptr;
    m_vPosOrigin = _vPos;
    m_vScale = _vScale;

    m_pTextureCom1->SetTextureKey(m_strAnimKey, TEX_NORMAL);


    m_vScaleGap = { 1.f, 1.f, 1.f };
    m_vPosGap = { 0.f, 0.f, 0.f };

    m_bAnimation = false;
    m_iAlpha = _iAlpha;
    m_bOrthogonal = _bOrthogonal;
    m_bZEnable = _bZEnable;

    //if (m_bOrthogonal) m_bZEnable = false;
}

void CEffect::SetSkillEffect(tstring _strAnimKey, _vec2 _vTextureScale, _vec3* _vPos, const _vec3* _vScale, _float _fAnimTime)
{

    m_strAnimKey = _strAnimKey;
    m_fAnimTime = _fAnimTime;
    m_bLoop = false;

    m_vPos = _vPos;
    m_vScale = *_vScale;

    _float fTest = m_fAnimTime / CResourceMgr::GetInstance()->GetTexture(m_strAnimKey, TEX_NORMAL)->size();

    m_pAnimatorCom->SetAnimKey(m_strAnimKey, m_fAnimTime / CResourceMgr::GetInstance()->GetTexture(m_strAnimKey, TEX_NORMAL)->size());

    shared_ptr< tagTextureInfo> p = *CResourceMgr::GetInstance()->GetTexture(m_strAnimKey, TEX_NORMAL)->begin();
    _vec2 vSize = p->vImgSize;

    m_vScaleGap = { vSize.x / _vTextureScale.x, vSize.y / _vTextureScale.y, 1.f };
    m_vPosGap = { 0.f, 0.f, 0.f };

    if (m_strAnimKey == L"Pierce_Effect")
    {
        m_vPosGap.x = 0.6f;
        m_vScaleGap.x = 1.6f;
    }

    else if (m_strAnimKey == L"Puncture_Effect")
    {
        m_vScaleGap.x = 2.f;
    }

    m_bAnimation = true;

    m_bZEnable = false;
}

void CEffect::SetDamageEffect(_int _iDamageType, _int _iDamage, _vec3* _vPos, _float _fAnimTime)
{
    tstring strType;

    switch (_iDamageType)
    {
    case 0:
        strType = L"UI_Damage";
        m_vPosGap = { 0.f, 0.f, 0.f };
        break;

    case 1:
        strType = L"UI_Heal";
        m_vPosGap = { 0.f, 0.f, 0.f };
        break;

    case 2:
        strType = L"UI_Stress";
        m_vPosGap = { 0.f, 1.f, 0.f };
        break;

    default:
        break;
    }

    if (_iDamage < 10)
    {
        m_pTextureCom1->SetTextureKey(strType + to_wstring(_iDamage), TEX_NORMAL);

        m_bTwoTexture = false;
    }

    else
    {
        m_pTextureCom1->SetTextureKey(strType + to_wstring(_iDamage / 10), TEX_NORMAL);
        m_pTextureCom2->SetTextureKey(strType + to_wstring(_iDamage % 10), TEX_NORMAL);

        m_bTwoTexture = true;
        m_bChange = false;
    }

    m_vPos = _vPos;
    m_pTransformCom->SetPosition(m_vPos->x, m_vPos->y, m_vPos->z);
    m_vScale = { 0.5f, 0.5f, 0.5f };
    m_pTransformCom->SetScale(m_vScale.x, m_vScale.y, m_vScale.z);
    m_vScaleGap = { 1.f, 1.f, 1.f };
    m_bLoop = false;
    m_bAnimation = false;
    m_fAnimTime = _fAnimTime;
    m_bZEnable = false;

    SetMove(true, { 0.f, 1.5f, 0.f }, m_fAnimTime);

}

void CEffect::SetFontEffect(tstring _strAnimKey, _vec3* _vPos, _float _fAnimTime)
{
    m_strAnimKey = _strAnimKey;

    m_pTextureCom1->SetTextureKey(m_strAnimKey, TEX_NORMAL);
    m_bTwoTexture = false;

    m_vPos = _vPos;
    m_pTransformCom->SetPosition(m_vPos->x, m_vPos->y, m_vPos->z);
    m_vPosGap = { 0.f, 1.f, 0.f };
    m_vScale = { 2.f, 0.7f, 0.5f };
    m_pTransformCom->SetScale(m_vScale.x, m_vScale.y, m_vScale.z);
    m_vScaleGap = { 1.f, 1.f, 1.f };
    m_bLoop = false;
    m_bAnimation = false;
    m_fAnimTime = _fAnimTime;
    m_bZEnable = false;

    SetMove(true, { 0.f, 1.5f, 0.f }, m_fAnimTime);
}

void CEffect::SetHeadEffect(tstring _strAnimKey, _vec3* _vPos, _float _fAnimTime, _bool _bLoop, _bool _bShieldBreaker)
{
    m_strAnimKey = _strAnimKey;
    m_fAnimTime = _fAnimTime;
    m_bLoop = _bLoop;

    m_vPos = _vPos;
    m_pTransformCom->SetPosition(m_vPos->x, m_vPos->y, m_vPos->z);
    m_vPosGap = { 0.f, 2.3f, 0.f };
    if (_bShieldBreaker)
    {
        m_vPosGap.x += 1.f;
    }
    m_vScale = {2.5f, 2.f, 3.f};
    m_pTransformCom->SetScale(m_vScale.x, m_vScale.y, m_vScale.z);
    m_vScaleGap = { 1.f, 1.f, 1.f };

   // _float fTest = m_fAnimTime / CResourceMgr::GetInstance()->GetTexture(m_strAnimKey, TEX_NORMAL)->size();

    m_pAnimatorCom->SetAnimKey(m_strAnimKey, m_fAnimTime / CResourceMgr::GetInstance()->GetTexture(m_strAnimKey, TEX_NORMAL)->size());

    m_bAnimation = true;
    m_bZEnable = false;
}

void CEffect::SetProjEffect(tstring _strAnimKey, _vec3 _vPos, _float _fAnimTime)
{

    m_strAnimKey = _strAnimKey;
    m_fAnimTime = _fAnimTime;

    _float _fTest = m_fAnimTime / CResourceMgr::GetInstance()->GetTexture(m_strAnimKey, TEX_NORMAL)->size();
    m_pAnimatorCom->SetAnimKey(m_strAnimKey, m_fAnimTime / CResourceMgr::GetInstance()->GetTexture(m_strAnimKey, TEX_NORMAL)->size());
    m_bTwoTexture = false;

    m_vPos = nullptr;
    m_vPosOrigin = _vPos;
    m_pTransformCom->SetPosition(m_vPosOrigin.x, m_vPosOrigin.y, m_vPosOrigin.z);
    m_vPosGap = { 0.f, 0.f, 0.f };
    m_vScale = { 2.f , 2.f, 2.f };
    m_pTransformCom->SetScale(m_vScale.x, m_vScale.y, m_vScale.z);
    //  m_vScale = *_vScale;
    m_vScaleGap = { 1.f, 1.f, 1.f };
    m_bLoop = false;
    m_bAnimation = true;

    m_bZEnable = true;
}

void CEffect::SetPos(_vec3* _vPos)
{
    m_vPos = _vPos;
}

void CEffect::SetScale(const _vec3* _vScale)
{
    m_vScale = *_vScale;
}

void CEffect::SetMove(_bool _bMove, _vec3 _vDir, _float _fTime)
{
    m_bMoving = _bMove;
    m_vMoveDir = _vDir;
    m_fMoveTime = _fTime;
}

void CEffect::SetFade(_bool _bFade, _bool _bAppear, _float _fTime)
{
    m_bFading = _bFade;
    m_bAppear = _bAppear;
    m_fFadeTime = _fTime;

    if (m_bAppear)
        m_iAlpha = 0;

    else
        m_iAlpha = 255;
}

void CEffect::Move()
{
    m_vPosGap += m_vMoveDir * m_fDeltaTime;

    m_fAccumMoveTime += m_fDeltaTime;

    // ¹«ºù Á¾·á
    if (m_fAccumMoveTime >= m_fMoveTime)
    {
        m_bMoving = false;

        m_fAccumMoveTime = 0.f;
        m_fMoveTime = 0.f;
        m_vMoveDir = { 0.f, 0.f, 0.f };
    }
}

void CEffect::Fade()
{
    if (m_bAppear)
    {
        m_iAlpha += 1;
        if (m_iAlpha >= 255)
            m_iAlpha = 255;
    }

    else
    {
        m_iAlpha -= 1;
        if (m_iAlpha <= 0) m_iAlpha = 0;
    }

    m_fAccumFadeTime += m_fDeltaTime;

    // ¹«ºù Á¾·á
    if (m_fAccumFadeTime >= m_fFadeTime)
    {
        m_bFading = false;

        m_fAccumFadeTime = 0.f;
        m_fFadeTime = 0.f;
    }
}

void CEffect::Reset()
{
    printf("Effect : %d ", m_iNum);
    printf("¸®¼Â\n");

    m_mapComponent->clear();
    AddComponent();

    m_fDeltaTime = 0.f;
    m_vScale = { 0.f, 0.f, 0.f };
    m_vPos = nullptr;
    m_vPosOrigin = { 0.f, 0.f, 0.f };
    m_vPosGap = { 0.f, 0.f, 0.f };
    m_vScaleGap = { 0.f, 0.f, 0.f };

    m_strAnimKey = L"";
    m_fAnimTime = 0.f;
    m_bLoop = false;
    m_iAlpha = 255;

    m_pTransformCom->SetPosition(-100.f, 0.f, -100.f);
    m_pTransformCom->SetScale(m_vScale.x, m_vScale.y, m_vScale.z);
    m_pTextureCom1->SetTextureKey(L"", TEX_NORMAL);
    m_pTextureCom2->SetTextureKey(L"", TEX_NORMAL);

    m_pAnimatorCom->SetAnimKey(L"", 0.f);

    m_fAccumMoveTime = 0.f;
    m_fAccumFadeTime = 0.f;

    m_fMoveTime = 0.f;
    m_fFadeTime = 0.f;

    m_bMoving = false;
    m_bFading = false;

    m_vMoveDir = { 0.f, 0.f, 0.f };
    m_bAppear = false;
    m_bAnimation = false;
    m_bTwoTexture = false;
    m_bActive = false;
    m_bOrthogonal = false;
    m_bZEnable = false;
    m_bChange = false;
}
