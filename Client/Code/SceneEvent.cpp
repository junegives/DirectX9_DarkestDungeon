#include "pch.h"
#include "SceneEvent.h"

#include "UIMgr.h"
#include "EnvironmentObj.h"
#include "GameMgr.h"
#include "Player.h"

#include "ScreenEffect.h"
#include "UIMgr.h"
#include "SoundMgr.h"

CSceneEvent::CSceneEvent(LPDIRECT3DDEVICE9 _pGraphicDev)
    :CGameObject(_pGraphicDev)
{
}

CSceneEvent::~CSceneEvent()
{
}

HRESULT CSceneEvent::ReadyGameObject()
{
    CUIMgr::GetInstance()->AllVisibleOff();

    m_fEventTime = 15.f;

    //MagicRingSetting
    shared_ptr<CPlayer> pPlayer = dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer());
    _vec3* pPlayerTransform = dynamic_pointer_cast<CTransform>(pPlayer->GetComponent(L"Com_Transform", ID_DYNAMIC))->GetPos();

    _matrix matView;
    _vec3 vPos;
    m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);

    D3DXMatrixInverse(&matView, 0, &matView);
    memcpy(&vPos, &matView.m[3][0], sizeof(_vec3));
    

    m_pMainObject = make_shared<CEnvironmentObj>(m_pGraphicDev, L"Item_MagicRing", false, false, 1, false);
    m_pMainObject->SetScale(_vec3(0.7f, 0.7f, 0.7f));
    m_pMainObject->SetPos(_vec3(vPos.x, vPos.y- 3.5f , vPos.z + 3.f));
    m_pMainObject->SetAngle(_vec3(0.f, 0.f, 0.f));
    m_pMainObject->AwakeGameObject();
    m_pMainObject->ReadyGameObject();

    m_vCurrentPos = _vec3(vPos.x, vPos.y - 3.5f, vPos.z + 3.f);
    
   

    return S_OK;
}

_int CSceneEvent::UpdateGameObject(const _float& fTimeDelta)
{
    m_fSceneActTime += fTimeDelta;

    //지정한 이벤트 시간이 지나면 종료 
    if (m_fEventTime <= m_fSceneActTime)
        m_bEnable = false;

    if (m_fSceneActTime > 7.f) {
        m_bFirstEventDone = true;
    }

    if (m_fSceneActTime > 9.5f) {
        m_bSecondEventDone = true;
    }

    m_pMainObject->UpdateGameObject(fTimeDelta);


    if (!m_bFirstEventDone && m_fSceneActTime >=1.5f) {

        MoveObject(fTimeDelta);
        return 0;

    }
    
    if (!m_bSecondEventDone && m_fSceneActTime > 8.5f) {
        
        MoveObjectFront(fTimeDelta);
    }

    if (!m_bThirdEventDone && m_fSceneActTime > 9.1f) {

        shared_ptr<CUIObj> pScreenEffect = CUIMgr::GetInstance()->FindUI(L"UI_ScreenEffect");

        if (pScreenEffect) {
            dynamic_pointer_cast<CScreenEffect>(pScreenEffect)->SetScreenEffect(EScreenEffect::FADEIN_WHITE);
        }
        m_bThirdEventDone = true;

        CSoundMgr::GetInstance()->PlaySound(L"BossMap_Glow_Effect.wav", CHANNELID::JESTER, 1.f);
   }

    return 0;
}

void CSceneEvent::LateUpdateGameObject()
{
   m_pMainObject->LateUpdateGameObject();
}

void CSceneEvent::RenderGameObject()
{

    //m_pMainObject->RenderGameObject();

}

void CSceneEvent::MoveObject(_float _fTime)
{
    if (m_pMainObject) {
        m_vCurrentPos.y += _fTime * 0.5f;
        dynamic_pointer_cast<CTransform>(m_pMainObject->GetComponent(L"Com_Transform", ID_DYNAMIC))->SetPosition(m_vCurrentPos.x, m_vCurrentPos.y, m_vCurrentPos.z);
    }

}

void CSceneEvent::MoveObjectFront(_float _fTime)
{
    if (m_pMainObject) {
        m_vCurrentPos.z -= _fTime * 5.f;
        dynamic_pointer_cast<CTransform>(m_pMainObject->GetComponent(L"Com_Transform", ID_DYNAMIC))->SetPosition(m_vCurrentPos.x, m_vCurrentPos.y, m_vCurrentPos.z);
    }
}
