#include "pch.h"
#include "Ruin_Trap.h"
#include "Player.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "Creature.h"
#include "Hero.h"

#include "UIMgr.h"
#include "ScreenEffect.h"

CRuin_Trap::CRuin_Trap(LPDIRECT3DDEVICE9 pGraphicDev)
	: CInteractionObj(pGraphicDev)
{
}

CRuin_Trap::CRuin_Trap(const CRuin_Trap& rhs)
	: CInteractionObj(rhs)
{
}

CRuin_Trap::~CRuin_Trap()
{
}

HRESULT CRuin_Trap::ReadyGameObject()
{
	__super::ReadyGameObject();

	m_bInteractionKey = L"C";
	m_bInteractionInfo = L"함정 제거";

	return S_OK;
}

_int CRuin_Trap::UpdateGameObject(const _float& fTimeDelta)
{
	_int	iExit = __super::UpdateGameObject(fTimeDelta);

	if (m_bActive)
	{
		m_fActiveTime -= fTimeDelta;
		if (m_fActiveTime < 0)
		{
			// 해제 성공
			if (m_bSuccess)
			{
				vector<shared_ptr<CGameObject>>* pHeroVec =
					dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->GetHeroVec();
				for (int i = 0; i < size(*pHeroVec); i++)
				{
					dynamic_pointer_cast<CHero>((*pHeroVec)[i])->DecreaseStress(10);
				}
			}
			// 해제 실패
			else
			{
				CUIMgr::GetInstance()->NarrationOn(L"Narr_Trap_Fail");
				
				//Blood Screen Effect
				shared_ptr<CUIObj> pScreenEffect = CUIMgr::GetInstance()->FindUI(L"UI_ScreenEffect");

				if (pScreenEffect) {
					dynamic_pointer_cast<CScreenEffect>(pScreenEffect)->SetScreenEffect(EScreenEffect::BLOOD);
				}

				vector<shared_ptr<CGameObject>>* pHeroVec =
					dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->GetHeroVec();
				for (int i = 0; i < size(*pHeroVec); i++)
				{
					dynamic_pointer_cast<CCreature>((*pHeroVec)[i])->DecreaseHP(5);
					dynamic_pointer_cast<CHero>((*pHeroVec)[i])->IncreaseStress(20);
				}
			}

			m_fActiveTime = TRAPACTIVETIME;
			m_bActive = false;
			m_bFinish = true;

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
			m_pAnimatorCom->SetAnimKey(L"Ruin_Traps_Remains", 0.05f);
			//m_pTransformCom->SetScale(WEALD_PATHSIZEX / 3.f, WEALD_PATHSIZEX / 3.f, 1.f);
			break;
		case EState::ACTIVE:
			m_pAnimatorCom->SetAnimKey(L"Ruin_Traps_Sprung", 0.02f);
			//m_pTransformCom->SetScale(WEALD_PATHSIZEX / 3.f, WEALD_PATHSIZEX / 3.f, 1.f);
			break;
		case EState::FINISH:
			m_pAnimatorCom->SetAnimKey(L"Hero_Death", 0.02f);
			//m_pTransformCom->SetScale(WEALD_PATHSIZEX / 3.f, WEALD_PATHSIZEX / 3.f, 1.f);
			break;
		}
	}

	if (!m_bFinish && !m_bSuccess && m_bActive)
	{
		m_ePrevAnimState = m_eCurAnimState;
		m_eCurAnimState = EState::ACTIVE;
	}
	else if (m_bFinish)
	{
		m_ePrevAnimState = m_eCurAnimState;
		m_eCurAnimState = EState::FINISH;
	}
	else
	{
		m_ePrevAnimState = m_eCurAnimState;
		m_eCurAnimState = EState::IDLE;
	}

	//빌보드 시작
	_matrix matWorld;

	matWorld = *m_pTransformCom->GetWorld();
	SetBillBoard(matWorld);
	m_pTransformCom->SetWorld(&matWorld);

	return iExit;
}

void CRuin_Trap::LateUpdateGameObject()
{
	__super::LateUpdateGameObject();
}

void CRuin_Trap::RenderGameObject()
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

void CRuin_Trap::AddComponent()
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

	pComponent = m_pAnimatorCom = make_shared<CAnimator>(m_pGraphicDev);
	m_pAnimatorCom->SetAnimKey(L"Ruin_Traps_Remains", 0.05f);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animator",pComponent });
}

void CRuin_Trap::GetInteractionKey(const _float& fTimeDelta)
{
	// 인터랙션 키 UI 보여주기
	// ----------수정 필요------------

	// 키 입력받기
	if (GetAsyncKeyState('C') & 0x8000)
	{
		// 아직 활성화 되지 않았고, 끝나지 않았다면
		if (!m_bActive && !m_bFinish)
		{
			m_bActive = true;
			m_bTabInteractionKey = true;

			int iNum = 0;
			//iNum = rand() % 2;
			iNum = 0;
			if (iNum == 0)
			{
				CSoundMgr::GetInstance()->PlaySound(L"prop_trap_bladewheel.wav", CHANNELID::TRAP, 1.f);


				m_bSuccess = false;
			}
			else
			{
				CSoundMgr::GetInstance()->PlaySound(L"prop_trap_disarm.wav", CHANNELID::TRAP, 1.f);


				m_bSuccess = true;
			}
		}



		Interaction();
	}
}

void CRuin_Trap::Interaction()
{
}

void CRuin_Trap::FinishInteraction()
{
}

void CRuin_Trap::ChangeTexture()
{
}

_bool CRuin_Trap::IsFinish()
{

	return false;
}