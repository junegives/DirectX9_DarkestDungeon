#include "pch.h"
#include "Weald_Curio_Luggage.h"
#include "Player.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "Creature.h"
#include "Hero.h"

CWeald_Curio_Luggage::CWeald_Curio_Luggage(LPDIRECT3DDEVICE9 pGraphicDev)
	: CInteractionObj(pGraphicDev)
{
}

CWeald_Curio_Luggage::CWeald_Curio_Luggage(const CWeald_Curio_Luggage& rhs)
	: CInteractionObj(rhs)
{
}

CWeald_Curio_Luggage::~CWeald_Curio_Luggage()
{
}

HRESULT CWeald_Curio_Luggage::ReadyGameObject()
{
	__super::ReadyGameObject();

	m_bInteractionKey = L"C";
	m_bInteractionInfo = L"독성 상자 열기";

	return S_OK;
}

_int CWeald_Curio_Luggage::UpdateGameObject(const _float& fTimeDelta)
{
	_int	iExit = __super::UpdateGameObject(fTimeDelta);

	if (m_bActive)
	{
		m_fActiveTime -= fTimeDelta;
		if (m_fActiveTime < 0)
		{
			// 제대로된 상호작용(해독제)
			if (dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->GetCurrentItem()
				== EHandItem::ANTI_VENOM)
			{
				dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SetCurrentItem(EHandItem::ENUM_END);
				dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->DeleteItem(L"Item_UI_Antivenom");
				dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SetEventTrigger(true);
			}
			// 해독제없는 상호작용
			else
			{
				vector<shared_ptr<CGameObject>>* pHeroVec =
					dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->GetHeroVec();
				for (int i = 0; i < size(*pHeroVec); i++)
				{
					_int  DotDam[2] = { 1, 2 };
					dynamic_pointer_cast<CCreature>((*pHeroVec)[i])->BlightAttack(DotDam);
					dynamic_pointer_cast<CCreature>((*pHeroVec)[i])->SetBlight(true);
				}
			}

			m_fActiveTime = CURIOACTIVETIME;
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
			m_pAnimatorCom->SetAnimKey(L"Weald_lost_luggage", 0.05f);
			//m_pTransformCom->SetScale(WEALD_PATHSIZEX / 3.f, WEALD_PATHSIZEX / 3.f, 1.f);
			break;
		case EState::ACTIVE:
			m_pAnimatorCom->SetAnimKey(L"Weald_lost_luggage", 0.02f);
			//m_pTransformCom->SetScale(WEALD_PATHSIZEX / 3.f, WEALD_PATHSIZEX / 3.f, 1.f);
			break;
		case EState::FINISH:
			m_pAnimatorCom->SetAnimKey(L"Weald_lost_luggage_Finish", 0.02f);
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

void CWeald_Curio_Luggage::LateUpdateGameObject()
{
	__super::LateUpdateGameObject();
}

void CWeald_Curio_Luggage::RenderGameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->GetWorld());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	SetMaterial();

	m_pAnimatorCom->SetAnimTexture();

	m_pBufferCom->RenderBuffer();

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CWeald_Curio_Luggage::AddComponent()
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
	m_pAnimatorCom->SetAnimKey(L"Weald_lost_luggage", 0.05f);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animator",pComponent });
}

void CWeald_Curio_Luggage::GetInteractionKey(const _float& fTimeDelta)
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

		// 텍스처 또는 애니메이션 변경
		ChangeTexture();

		if (dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->GetCurrentItem()
			== EHandItem::ANTI_VENOM)
		{
			CSoundMgr::GetInstance()->PlaySound(L"prop_curio_lostluggage.wav", CHANNELID::CURIO, 1.f);
			CSoundMgr::GetInstance()->PlaySound(L"prop_curio_use_antivenom.wav", CHANNELID::INTERACTION, 1.f);
		}
		// 삽없는 상호작용
		else
		{
			CSoundMgr::GetInstance()->PlaySound(L"prop_curio_lostluggage.wav", CHANNELID::CURIO, 1.f);
		}


		Interaction();
	}
}

void CWeald_Curio_Luggage::Interaction()
{
}

void CWeald_Curio_Luggage::FinishInteraction()
{
	//m_bInteracting = false;

	//m_pTextureCom->SetTextureKey(L"Weald_Traps_Remains", TEX_NORMAL);
}

void CWeald_Curio_Luggage::ChangeTexture()
{
	m_ePrevAnimState = m_eCurAnimState;
	m_eCurAnimState = EState::ACTIVE;
}

_bool CWeald_Curio_Luggage::IsFinish()
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