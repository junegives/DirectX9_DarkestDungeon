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
	// ���ͷ��� Ű UI �����ֱ�
	// ----------���� �ʿ�------------

	// Ű �Է¹ޱ�
	if (GetAsyncKeyState('C') & 0x8000)
	{
		m_bInteracting = true;

		// �÷��̾� ������ ����
		CGameMgr::GetInstance()->SetGameState(EGameState::LOCK);

		// ���� ���� UI �߱� (���� Ű�� �־��ֱ�)

		Interaction();
	}
}

void CStageCoachHero::Interaction()
{
	// UI���� ��ų, ���� ���� �ߵ���???
	{
		// ��ŷ �ޱ�..? Hoveró��..???? ����غ���

		// ����ϱ� Ű �Է� ó��
	}
}

_bool CStageCoachHero::IsFinish()
{
	// Ű �Է¹ޱ�
	if (GetAsyncKeyState('X') & 0x8000)
	{
		m_bInteracting = false;

		// �÷��̾� �ൿ Ǯ��
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
