#include "pch.h"
#include "StageCoachHero.h"

#include "GameMgr.h"
#include "CameraMgr.h"

#include "Export_System.h"
#include "Export_Utility.h"

CStageCoachHero::CStageCoachHero(LPDIRECT3DDEVICE9 pGraphicDev, EHeroType _eType)
	: CInteractionObj(pGraphicDev), m_eHeroType(_eType)
{
}

CStageCoachHero::CStageCoachHero(const CStageCoachHero& rhs)
	: CInteractionObj(rhs)
{
}

CStageCoachHero::~CStageCoachHero()
{
}

HRESULT CStageCoachHero::ReadyGameObject()
{
	__super::ReadyGameObject();

	return S_OK;
}

_int CStageCoachHero::UpdateGameObject(const _float& fTimeDelta)
{
	_int	iExit = __super::UpdateGameObject(fTimeDelta);

	return iExit;
}

void CStageCoachHero::LateUpdateGameObject()
{
	__super::LateUpdateGameObject();
}

void CStageCoachHero::RenderGameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->GetWorld());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pAnimatorCom->SetAnimTexture();
	m_pBufferCom->RenderBuffer();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	//__super::RenderGameObject();
}

void CStageCoachHero::AddComponent()
{
	shared_ptr<CComponent> pComponent;

	pComponent = m_pBufferCom = make_shared<CRcTex>(m_pGraphicDev);
	dynamic_pointer_cast<CRcTex>(m_pBufferCom)->ReadyBuffer();
	m_mapComponent[ID_STATIC].insert({ L"Com_RcTex", pComponent });

	pComponent = m_pTransformCom = make_shared<CTransform>(_vec3(0.f, 0.f, 0.f), _vec3(1.f, 1.f, 1.f), _vec3(0.f, 0.f, 0.f));
	m_pTransformCom->ReadyTransform();
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });
	m_pTransformCom->SetPosition(m_vPos.x, m_vPos.y, m_vPos.z);
	m_pTransformCom->SetScale(m_vScale.x, m_vScale.y, m_vScale.z);

	pComponent = m_pAnimatorCom = make_shared<CAnimator>(m_pGraphicDev);

	switch (m_eHeroType)
	{
	case EHeroType::VESTAL:
		m_strKey = L"Vestal_Idle";
		break;
	case EHeroType::HIGHWAYMAN:
		m_strKey = L"Highwayman_Idle";
		break;
	case EHeroType::SHILEDBREAKER:
		m_strKey = L"ShieldBreaker_Idle";
		break;
	case EHeroType::JESTER:
		m_strKey = L"Jester_Idle";
		break;
	case EHeroType::ENUM_END:
		m_strKey = L"Empty";
		break;
	}

	pComponent = m_pAnimatorCom = make_shared<CAnimator>(m_pGraphicDev);
	m_pAnimatorCom->SetAnimKey(m_strKey, 0.05f);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animator", pComponent });
}

void CStageCoachHero::GetInteractionKey(const _float& fTimeDelta)
{
	// 인터랙션 키 UI 보여주기
	// ----------수정 필요------------

	// 키 입력받기
	if (GetAsyncKeyState('C') & 0x8000)
	{
		m_bInteracting = true;

		// 플레이어 움직임 막기
		CGameMgr::GetInstance()->SetGameState(EGameState::LOCK);

		// 영웅 스탯 UI 뜨기 (영웅 키값 넣어주기)

		Interaction();
	}
}

void CStageCoachHero::Interaction()
{
	// UI에서 스킬, 스탯 설명 뜨도록???
	{
		// 피킹 받기..? Hover처리..???? 고민해보기

		// 고용하기 키 입력 처리
	}
}

_bool CStageCoachHero::IsFinish()
{
	// 키 입력받기
	if (GetAsyncKeyState('X') & 0x8000)
	{
		m_bInteracting = false;

		// 플레이어 행동 풀기
		CGameMgr::GetInstance()->SetGameState(EGameState::PRCESS);

		return true;
	}

	return false;
}

void CStageCoachHero::FinishInteraction()
{
}

void CStageCoachHero::ChangeTexture()
{
}
