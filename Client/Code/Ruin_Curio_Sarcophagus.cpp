#include "pch.h"
#include "Ruin_Curio_Sarcophagus.h"
#include "Player.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "Creature.h"
#include "Hero.h"
#include"Ruin_Curio_Statue.h"

#include"Item.h"
#include "CameraMgr.h"
#include"UIMgr.h"

#include"BossRoomDoor.h"

#include"SoundMgr.h"

#include "LoadingScreen.h"

CRuin_Curio_Sarcophagus::CRuin_Curio_Sarcophagus(LPDIRECT3DDEVICE9 pGraphicDev)
	: CInteractionObj(pGraphicDev)
{
}

CRuin_Curio_Sarcophagus::CRuin_Curio_Sarcophagus(const CRuin_Curio_Sarcophagus& rhs)
	: CInteractionObj(rhs)
{
}

CRuin_Curio_Sarcophagus::~CRuin_Curio_Sarcophagus()
{
}

HRESULT CRuin_Curio_Sarcophagus::ReadyGameObject()
{
	__super::ReadyGameObject();

	m_bInteractionKey = L"C";
	m_bInteractionInfo = L"상자 윗면 보기";

	m_StatueVec.reserve(3);

	return S_OK;
}

_int CRuin_Curio_Sarcophagus::UpdateGameObject(const _float& fTimeDelta)
{
	_int	iExit = __super::UpdateGameObject(fTimeDelta);

	if (m_bDebounce) {

		m_fDebTime += fTimeDelta;

		if (m_fDebTime > 0.5f) {
			m_bDebounce = false;
			m_fDebTime = 0.f;
		}

	}


	if (!m_bAnimComplete && m_bPuzzleDone) {

		
		if (!m_bEffectDone && m_fSceneTime > 2.f) {
			CCameraMgr::GetInstance()->SetState(ECameraMode::IDLE);
			CCameraMgr::GetInstance()->MovingStraight(ECameraMode::ZOOMIN, { m_vPos.x - 7.f , m_vPos.y+1.f , m_vPos.z });

			m_bEffectDone = true;
		}

		m_fSceneTime += fTimeDelta;

		if (m_fSceneTime > 3.f && !m_bEffectDone2) {
			ChangeTexture();

			shared_ptr<CGameObject> pItem = make_shared<CItem>(m_pGraphicDev);
			dynamic_pointer_cast<CItem>(pItem)->SetDropItemInfo({ m_vPos.x - 3.f , m_vPos.y - 0.5f, m_vPos.z }, L"Item_MagicRing");
			pItem->SetColliding(true);
			pItem->AwakeGameObject();
			pItem->ReadyGameObject();
			dynamic_pointer_cast<CItem>(pItem)->StartParticle();
			dynamic_pointer_cast<CItem>(pItem)->SetItemTypeEnum(EHandItem::RING);
			CSceneMgr::GetInstance()->AddNewObject(L"Layer_4_GameObj", L"Obj_Item_Ring", pItem);
		
			CSoundMgr::GetInstance()->StopSound(CHANNELID::EFFECT);
			CSoundMgr::GetInstance()->PlaySound(L"Effect_Find_MagicRing.wav", CHANNELID::EFFECT, 0.7f);

			shared_ptr<CBossRoomDoor> pBossDoor = make_shared<CBossRoomDoor>(m_pGraphicDev);
			pBossDoor->SetPos(_vec3(m_vPos.x , m_vPos.y + 1.5f, m_vPos.z + 12.f));
			pBossDoor->SetAngle(_vec3(0.f, 0.f, 0.f));
			pBossDoor->SetScale(_vec3(RUIN_WALLSIZEX / 5.f, (RUIN_WALLSIZEX / 5.f) / 819.f * 816.f, RUIN_WALLSIZEX / 5.f));
			pBossDoor->AwakeGameObject();
			pBossDoor->ReadyGameObject();
			CSceneMgr::GetInstance()->AddNewObject(L"Layer_4_GameObj", L"Obj_BossDoor", pBossDoor);


			m_eCurAnimState = EState::FINISH;
			m_bEffectDone2 = true;
		}

		if (m_fSceneTime > 6.f) {
			CCameraMgr::GetInstance()->SetFPSMode();
			m_bAnimComplete = true;
			CUIMgr::GetInstance()->NarrationOn(L"Narr_Darkest_Return1");
			
		}
	}



	if (m_bActive)
	{
		m_fActiveTime -= fTimeDelta;
		if (m_fActiveTime < 0)
		{
			// 제대로된 상호작용(키)
			/*if (dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->GetCurrentItem()
				== EHandItem::KEYS)
			{
				dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SetCurrentItem(EHandItem::ENUM_END);
				dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->DeleteItem(L"Item_UI_Key");
				dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SetEventTrigger(true);
			}

			m_fActiveTime = CURIOACTIVETIME;*/

			if (!m_bIsUION) {
				CUIMgr::GetInstance()->SelectUIVisibleOn(L"UI_PuzzleHint");
				m_bIsUION = true;
			}
			else {
				CUIMgr::GetInstance()->SelectUIVisibleOff(L"UI_PuzzleHint");
				m_bIsUION = false;
			}
			m_bActive = false;
			//m_ePrevAnimState = m_eCurAnimState;
			//m_eCurAnimState = EState::FINISH;
		}
	}

	// Animation
	if (m_eCurAnimState != m_ePrevAnimState)
	{
		switch (m_eCurAnimState)
		{
		case EState::IDLE:
			m_pAnimatorCom->SetAnimKey(L"Ruin_locked_sarcophagus", 0.05f);
			//m_pTransformCom->SetScale(WEALD_PATHSIZEX / 3.f, WEALD_PATHSIZEX / 3.f, 1.f);
			break;
		case EState::ACTIVE:
			m_pAnimatorCom->SetAnimKey(L"Ruin_locked_sarcophagus", 0.02f);
			//m_pTransformCom->SetScale(WEALD_PATHSIZEX / 3.f, WEALD_PATHSIZEX / 3.f, 1.f);
			break;
		case EState::FINISH:
			m_pAnimatorCom->SetAnimKey(L"Ruin_locked_sarcophagus_Finish", 0.02f);
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

void CRuin_Curio_Sarcophagus::LateUpdateGameObject()
{
	__super::LateUpdateGameObject();

	if (!m_bPuzzleDone) {
		for (auto& iter : m_StatueVec) {

			m_bPuzzleDone = iter->GetIsAnswer();

			if (!m_bPuzzleDone)
				break;
		}


		if (m_bPuzzleDone) {

			for (auto& iter : m_StatueVec) {
				iter->SetTabInteractionKeyState();
			}

			//EarthQuake Effect
			CSoundMgr::GetInstance()->PlaySound(L"Effect_earthquake.wav", CHANNELID::PLAYER, 0.7f);
			CCameraMgr::GetInstance()->AddEffectInfo(EEffectState::SHAKING, 2.f, 0.1f , EShakingType::HORIZON);
		}
	}
}

void CRuin_Curio_Sarcophagus::RenderGameObject()
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

void CRuin_Curio_Sarcophagus::AddComponent()
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
	m_pTransformCom->SetScale(m_vScale.x, m_vScale.y, m_vScale.x);

	//pComponent = m_pTextureCom = make_shared<CTexture>(m_pGraphicDev);
	//m_pTextureCom->SetTextureKey(L"Weald_Traps_Remains", TEX_NORMAL);
	//m_mapComponent[ID_DYNAMIC].insert({ L"Com_Texture",pComponent });

	pComponent = m_pAnimatorCom = make_shared<CAnimator>(m_pGraphicDev);
	m_pAnimatorCom->SetAnimKey(L"Ruin_locked_sarcophagus", 0.05f);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animator",pComponent });
}

void CRuin_Curio_Sarcophagus::GetInteractionKey(const _float& fTimeDelta)
{
	// 인터랙션 키 UI 보여주기
	// ----------수정 필요------------

	// 키 입력받기
	if (GetAsyncKeyState('C') & 0x8000)
	{

		if (m_bDebounce)
			return;

		//CSoundMgr::GetInstance()->PlaySound(L"prop_curio_secretstash.wav", CHANNELID::CURIO, 1.f);


		m_bActive = true;
		m_bDebounce = true;
		//UI 띄우기

	}
}

void CRuin_Curio_Sarcophagus::Interaction()
{
}

void CRuin_Curio_Sarcophagus::FinishInteraction()
{
	//m_bInteracting = false;

	//m_pTextureCom->SetTextureKey(L"Weald_Traps_Remains", TEX_NORMAL);
}

void CRuin_Curio_Sarcophagus::ChangeTexture()
{
	m_ePrevAnimState = m_eCurAnimState;
	m_eCurAnimState = EState::ACTIVE;
}

_bool CRuin_Curio_Sarcophagus::IsFinish()
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