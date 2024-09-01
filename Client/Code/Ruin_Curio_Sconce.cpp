#include "pch.h"
#include "Ruin_Curio_Sconce.h"
#include "Player.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "Creature.h"
#include "Hero.h"

CRuin_Curio_Sconce::CRuin_Curio_Sconce(LPDIRECT3DDEVICE9 pGraphicDev)
	: CInteractionObj(pGraphicDev)
{
}

CRuin_Curio_Sconce::CRuin_Curio_Sconce(const CRuin_Curio_Sconce& rhs)
	: CInteractionObj(rhs)
{
}

CRuin_Curio_Sconce::~CRuin_Curio_Sconce()
{
}

HRESULT CRuin_Curio_Sconce::ReadyGameObject()
{
	__super::ReadyGameObject();

	m_bInteractionKey = L"C";
	m_bInteractionInfo = L"불 붙이기";

	D3DLIGHT9 _pLightInfo1;

	_pLightInfo1.Diffuse = { 1.f , 0.2f , 0.2f , 1.f };
	_pLightInfo1.Specular = { 1.f , 0.2f , 0.2f , 1.f };
	_pLightInfo1.Ambient = { 1.f , 0.2f , 0.2f , 1.f };
	_pLightInfo1.Position = { m_vPos.x, m_vPos.y + 1.5f  , m_vPos.z };
	_pLightInfo1.Range = 30.f;


	_pLightInfo1.Attenuation0 = 0.f;
	_pLightInfo1.Attenuation1 = 0.2f;
	_pLightInfo1.Attenuation2 = 0.f;

	m_pLight = CLightMgr::GetInstance()->InitPointLight(m_pGraphicDev, _pLightInfo1, 95);

	return S_OK;
}

_int CRuin_Curio_Sconce::UpdateGameObject(const _float& fTimeDelta)
{
	_int	iExit = __super::UpdateGameObject(fTimeDelta);

	if (m_bActive)
	{
		m_fActiveTime -= fTimeDelta;
		if (m_fActiveTime < 0)
		{
			// 상호작용
			if (!dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->GetEventTrigger())
			{
				dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SetEventTrigger(true);
			}
			else if (!dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->GetEvent2Trigger())
			{
				dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SetEvent2Trigger(true);
			}
			else if (!dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->GetEvent3Trigger())
			{
				dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SetEvent3Trigger(true);
			}

			m_fActiveTime = CURIOACTIVETIME;
			m_bActive = false;
			m_ePrevAnimState = m_eCurAnimState;
			m_eCurAnimState = EState::FINISH;
			m_pLight->LightOn();
		}
	}

	// Animation
	if (m_eCurAnimState != m_ePrevAnimState)
	{
		switch (m_eCurAnimState)
		{
		case EState::IDLE:
			m_pAnimatorCom->SetAnimKey(L"Ruin_Sconce", 0.05f);
			//m_pTransformCom->SetScale(WEALD_PATHSIZEX / 3.f, WEALD_PATHSIZEX / 3.f, 1.f);
			break;
		case EState::ACTIVE:
			m_pAnimatorCom->SetAnimKey(L"Ruin_Sconce", 0.02f);
			//m_pTransformCom->SetScale(WEALD_PATHSIZEX / 3.f, WEALD_PATHSIZEX / 3.f, 1.f);
			break;
		case EState::FINISH:
			m_pAnimatorCom->SetAnimKey(L"Ruin_Sconce_Finish", 0.02f);
			//m_pTransformCom->SetScale(WEALD_PATHSIZEX / 3.f, WEALD_PATHSIZEX / 3.f, 1.f);
			break;
		}
	}

	//빌보드 시작
	_matrix matWorld, matWorld2;

	matWorld = *m_pTransformCom->GetWorld();
	SetBillBoard(matWorld);
	m_pTransformCom->SetWorld(&matWorld);

	matWorld2 = *m_pFireTransformCom->GetWorld();
	SetBillBoard(matWorld2);
	m_pFireTransformCom->SetWorld(&matWorld2);

	return iExit;
}

void CRuin_Curio_Sconce::LateUpdateGameObject()
{
	__super::LateUpdateGameObject();
}

void CRuin_Curio_Sconce::RenderGameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->GetWorld());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	SetMaterial();

	m_pAnimatorCom->SetAnimTexture();

	m_pBufferCom->RenderBuffer();


	if (EState::FINISH == m_eCurAnimState) {

		m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

		m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pFireTransformCom->GetWorld());
		m_pFireAnimatorCom->SetAnimTexture();
		m_pFireBufferCom->RenderBuffer();

		m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);



	}

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CRuin_Curio_Sconce::AddComponent()
{

	shared_ptr<CComponent> pComponent;

	pComponent = m_pBufferCom = make_shared<CRcTex>(m_pGraphicDev);
	dynamic_pointer_cast<CRcTex>(m_pBufferCom)->ReadyBuffer();
	m_mapComponent[ID_STATIC].insert({ L"Com_RcTex", pComponent });

	//pComponent = m_pTextureCom = make_shared<CTexture>(m_pGraphicDev);
	//m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pTransformCom = make_shared<CTransform>(_vec3(0.f, 0.f, 0.f), _vec3(1.f, 1.f, 1.f), _vec3(0.f, 0.f, 0.f));
	m_pTransformCom->ReadyTransform();
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });
	m_pTransformCom->SetPosition(m_vPos.x, m_vPos.y, m_vPos.z);
	m_pTransformCom->SetScale(m_vScale.x, m_vScale.y, m_vScale.z);

	//pComponent = m_pTextureCom = make_shared<CTexture>(m_pGraphicDev);
	//m_pTextureCom->SetTextureKey(L"Weald_Traps_Remains", TEX_NORMAL);
	//m_mapComponent[ID_DYNAMIC].insert({ L"Com_Texture",pComponent });

	pComponent = m_pAnimatorCom = make_shared<CAnimator>(m_pGraphicDev);
	m_pAnimatorCom->SetAnimKey(L"Ruin_Sconce", 0.05f);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animator",pComponent });


	pComponent = m_pFireBufferCom = make_shared<CRcTex>(m_pGraphicDev);
	dynamic_pointer_cast<CRcTex>(m_pFireBufferCom)->ReadyBuffer();
	m_mapComponent[ID_STATIC].insert({ L"Com_FireRcTex", pComponent });

	pComponent = m_pFireTransformCom = make_shared<CTransform>(_vec3(0.f, 0.f, 0.f), _vec3(1.f, 1.f, 1.f), _vec3(0.f, 0.f, 0.f));
	m_pFireTransformCom->ReadyTransform();
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_FireTransform", pComponent });
	m_pFireTransformCom->SetPosition(m_vPos.x, m_vPos.y+ 2.5f, m_vPos.z);
	m_pFireTransformCom->SetScale(1.5f, 2.5f, 1.5f);

	pComponent = m_pFireAnimatorCom = make_shared<CAnimator>(m_pGraphicDev);
	m_pFireAnimatorCom->SetAnimKey(L"Ruin_Sconce_Fire", 0.1f);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_FireAnimator", pComponent });
}

void CRuin_Curio_Sconce::GetInteractionKey(const _float& fTimeDelta)
{
	// 인터랙션 키 UI 보여주기
	// ----------수정 필요------------

	// 키 입력받기
	if (GetAsyncKeyState('C') & 0x8000)
	{
		//m_bInteracting = true;
		m_bActive = true;

		// 플레이어 행동 막기
		//CGameMgr::GetInstance()->SetGameState(EGameState::LOCK);

		// 텍스처 또는 애니메이션 변경
		ChangeTexture();

		CSoundMgr::GetInstance()->PlaySound(L"prop_curio_sconce.wav", CHANNELID::CURIO, 1.f);


		Interaction();
	}
}

void CRuin_Curio_Sconce::Interaction()
{
}

void CRuin_Curio_Sconce::FinishInteraction()
{
	//m_bInteracting = false;

	//m_pTextureCom->SetTextureKey(L"Weald_Traps_Remains", TEX_NORMAL);
}

void CRuin_Curio_Sconce::ChangeTexture()
{
	m_ePrevAnimState = m_eCurAnimState;
	m_eCurAnimState = EState::ACTIVE;
}

_bool CRuin_Curio_Sconce::IsFinish()
{
	//// 키 입력받기
	//if (GetAsyncKeyState('X') & 0x8000)
	//{
	//	m_bInteracting = false;
	//	m_ePrevAnimState = m_eCurAnimState;
	//	m_eCurAnimState = EState::FINISH;
	//
	//	// 플레이어 행동 풀기
	//	//CGameMgr::GetInstance()->SetGameState(EGameState::PRCESS);
	//
	//	return true;
	//}

	return false;
}