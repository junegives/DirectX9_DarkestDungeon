#include "pch.h"
#include "Weald_Obstacle.h"
#include "Player.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "Creature.h"
#include "Hero.h"

#include"CameraMgr.h"
#include "UIMgr.h"
#include"ScreenEffect.h"

CWeald_Obstacle::CWeald_Obstacle(LPDIRECT3DDEVICE9 pGraphicDev)
	: CInteractionObj(pGraphicDev)
{
}

CWeald_Obstacle::CWeald_Obstacle(const CWeald_Obstacle& rhs)
	: CInteractionObj(rhs)
{
}

CWeald_Obstacle::~CWeald_Obstacle()
{
}

HRESULT CWeald_Obstacle::ReadyGameObject()
{
	__super::ReadyGameObject();
	
	m_bInteractionKey = L"C";
	m_bInteractionInfo = L"장애물 제거하기";

	return S_OK; 
}

_int CWeald_Obstacle::UpdateGameObject(const _float& fTimeDelta)
{
	_int	iExit = __super::UpdateGameObject(fTimeDelta);

	if (m_bActive)
	{
		m_fActiveTime -= fTimeDelta;
		if (m_fActiveTime < 0)
		{
			// 제대로된 상호작용(삽)
			if (dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->GetCurrentItem()
				== EHandItem::SHOVEL)
			{
				dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SetCurrentItem(EHandItem::ENUM_END);
				dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->DeleteItem(L"Item_UI_Shovel");
			}
			// 삽없는 상호작용
			else
			{
				shared_ptr<CUIObj> pScreenEffect = CUIMgr::GetInstance()->FindUI(L"UI_ScreenEffect");

				if (pScreenEffect) {
					dynamic_pointer_cast<CScreenEffect>(pScreenEffect)->SetScreenEffect(EScreenEffect::ATTACKED );
				}

				vector<shared_ptr<CGameObject>> *pHeroVec =
				dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->GetHeroVec();
				for (int i = 0; i < size(*pHeroVec); i++)
				{
					dynamic_pointer_cast<CCreature>((*pHeroVec)[i])->DecreaseHP(10);
					dynamic_pointer_cast<CHero>((*pHeroVec)[i])->IncreaseStress(30);
				}
			}

			m_fActiveTime = OBSTACLECLEARTIME;
			m_bActive = false;
			m_ePrevAnimState = m_eCurAnimState;
			m_eCurAnimState = EState::FINISH;
		}
	}

	// Animation
	if (m_eCurAnimState != m_ePrevAnimState)
	{
		switch (m_eCurAnimState)
		{
		case EState::IDLE:
			m_pAnimatorCom->SetAnimKey(L"Weald_Obstacles_Remains", 0.05f);
			//m_pTransformCom->SetScale(WEALD_PATHSIZEX / 3.f, WEALD_PATHSIZEX / 3.f, 1.f);
			break;
		case EState::ACTIVE:
			m_pAnimatorCom->SetAnimKey(L"Weald_Obstacles", 0.02f);
			//m_pTransformCom->SetScale(WEALD_PATHSIZEX / 3.f, WEALD_PATHSIZEX / 3.f, 1.f);
			break;
		case EState::FINISH:
			m_pAnimatorCom->SetAnimKey(L"Hero_Death", 0.02f);
			//m_pTransformCom->SetScale(WEALD_PATHSIZEX / 3.f, WEALD_PATHSIZEX / 3.f, 1.f);
			break;
		}
	}

	//빌보드 시작
	_matrix matWorld;

	matWorld = *m_pTransformCom->GetWorld();
	SetBillBoard(matWorld);
	m_pTransformCom->SetWorld(&matWorld);

	return iExit;
}

void CWeald_Obstacle::LateUpdateGameObject()
{
	__super::LateUpdateGameObject();
}

void CWeald_Obstacle::RenderGameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->GetWorld());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	SetMaterial();

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	m_pAnimatorCom->SetAnimTexture();

	m_pBufferCom->RenderBuffer();

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CWeald_Obstacle::AddComponent()
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
	m_pAnimatorCom->SetAnimKey(L"Weald_Obstacles_Remains", 0.05f);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animator",pComponent });
}

void CWeald_Obstacle::GetInteractionKey(const _float& fTimeDelta)
{
	// 인터랙션 키 UI 보여주기
	// ----------수정 필요------------

	// 키 입력받기
	if (GetAsyncKeyState('C') & 0x8000)
	{
		//m_bInteracting = true;
		m_bActive = true;
		m_bTabInteractionKey = true;

		// 플레이어 행동 막기
		//CGameMgr::GetInstance()->SetGameState(EGameState::LOCK);

		if (dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->GetCurrentItem()
			== EHandItem::SHOVEL)
		{
			CSoundMgr::GetInstance()->PlaySound(L"prop_obstacle_thorny_pick.wav", CHANNELID::OBSTACLE, 1.f);
			CSoundMgr::GetInstance()->PlaySound(L"prop_curio_use_shovel.wav", CHANNELID::INTERACTION, 1.f);
		}
		// 삽없는 상호작용
		else
		{
			CSoundMgr::GetInstance()->PlaySound(L"prop_obstacle_thorny_hand.wav", CHANNELID::OBSTACLE, 1.f);
		}

		// 텍스처 또는 애니메이션 변경
		ChangeTexture();

		Interaction();
	}
}

void CWeald_Obstacle::Interaction()
{
}

void CWeald_Obstacle::FinishInteraction()
{
	//m_bInteracting = false;

	//m_pTextureCom->SetTextureKey(L"Weald_Traps_Remains", TEX_NORMAL);
}

void CWeald_Obstacle::ChangeTexture()
{
	m_ePrevAnimState = m_eCurAnimState;
	m_eCurAnimState = EState::ACTIVE;
}

_bool CWeald_Obstacle::IsFinish()
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