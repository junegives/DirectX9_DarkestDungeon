#include "pch.h"
#include "Hero.h"
#include "Export_Utility.h"
#include "StatView.h"
#include "HeroStat.h"
#include "UIMgr.h"
#include "Player.h"

#include "EffectMgr.h"

#include "UIMgr.h"
#include "DungeonStatus.h"

CHero::CHero(LPDIRECT3DDEVICE9 pGraphicDev)
	: CCreature(pGraphicDev)
{
	ZeroMemory(m_szString4, sizeof(m_szString4));
}

CHero::CHero(LPDIRECT3DDEVICE9 pGraphicDev, STAT _tCommonStat, _int _iPosition,
	vector<shared_ptr<CSkill>>& _pVecSkill, _float _fCritical)
	: CCreature(pGraphicDev, _tCommonStat, _iPosition, _pVecSkill), m_fCritical(_fCritical)
{
}

CHero::CHero(LPDIRECT3DDEVICE9 pGraphicDev, STAT _tCommonStat)
	: CCreature(pGraphicDev, _tCommonStat)
{
}

CHero::CHero(const CHero& rhs)
	: CCreature(rhs)
{
}

CHero::~CHero()
{

}

HRESULT CHero::ReadyGameObject()
{
	__super::ReadyGameObject();
	m_bIsHero = true;
	m_eCollideID = ECollideID::HERO;
	//m_pStatInfo->SetIsHero(true);

	return S_OK;
}

_int CHero::UpdateGameObject(const _float& fTimeDelta)
{
	_int	iExit = __super::UpdateGameObject(fTimeDelta);

	// ���Ȱ���
	m_pStatInfo->SetStress(m_iStress);

	ChangeAnimState();
	SetAnimDelay(fTimeDelta);

	// �׽�Ʈ��
	//{
	//	if (GetAsyncKeyState(VK_HOME) & 0x8000)
	//	{
	//		m_eCurAnimState = EAnimState::COMBAT;
	//	}

	//	else if (GetAsyncKeyState(VK_PRIOR) & 0x8000)
	//	{
	//		m_eCurAnimState = EAnimState::WALK;
	//	}

	//	else if (GetAsyncKeyState(VK_NEXT) & 0x8000)
	//	{
	//		m_eCurAnimState = EAnimState::SKILL1;
	//	}

	//	else if (GetAsyncKeyState(VK_END) & 0x8000)
	//	{
	//		m_eCurAnimState = EAnimState::SKILL2;
	//	}
	//}

	if (m_pStatUI) {
		m_pStatUI->UpdateGameObject(fTimeDelta);
	}

	return iExit;
}

void CHero::LateUpdateGameObject()
{
	__super::LateUpdateGameObject();
}

void CHero::RenderGameObject()
{
	__super::RenderGameObject();

	TCHAR szBuff[32] = { };
	_stprintf_s(szBuff, TEXT("%d"), GetStress());
	lstrcpy(m_szString4, szBuff);
	m_pColliderCom->RenderCollider();
}

void CHero::PickingObj()
{
	if (dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->GetInBattle())
	{
		m_bPicked = true;
	}
	else
	{
		shared_ptr<CPlayer> pPlayer = dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer());
		_vec3* pPlayerTransform = dynamic_pointer_cast<CTransform>(pPlayer->GetComponent(L"Com_Transform", ID_DYNAMIC))->GetPos();

		if ((*pPlayerTransform).y > -10)
		{
			CSoundMgr::GetInstance()->StopSound(CHANNELID::EFFECT);
			CSoundMgr::GetInstance()->PlaySound(L"ui_page_open.wav", CHANNELID::EFFECT, 1.f);

			m_pStatUI->SetForHire(true);
			m_pStatUI->SetVisible(true);
		}

		else if ((*pPlayerTransform).y < -50 && (*pPlayerTransform).y > -150)
		{
			m_eCurAnimState = EAnimState::COMBAT;
			pPlayer->ShowTavernUI(dynamic_pointer_cast<CHero>(shared_from_this()));
			m_bSelected = true;
		}

		CGameMgr::GetInstance()->SetGameState(EGameState::LOCK);
	}
}

void CHero::IncreaseStress(_int _iValue, _bool _bStressEvent)
{
	if (_iValue == 0) return;

	m_iStress += _iValue;

	shared_ptr<CEffect> pEffect;

	GetHeroStatusEvent(false);

	CSoundMgr::GetInstance()->StopSound(CHANNELID::STATUS);
	CSoundMgr::GetInstance()->PlaySound(L"Gen_Status_StressUp.wav", CHANNELID::STATUS, 1.f);
	// ����Ʈ ���� ����
	// ��� ����Ʈ
	{
		pEffect = CEffectMgr::GetInstance()->GetEffect();
		if (m_eHeroType == EHeroType::SHILEDBREAKER)
			pEffect->SetHeadEffect(L"UI_Head_StressUp", m_pTransformCom->GetPos(), ATTACKTIME, false, true);
		else
			pEffect->SetHeadEffect(L"UI_Head_StressUp", m_pTransformCom->GetPos(), ATTACKTIME, false);
		pEffect->SetActive(true);
	}

	// ��Ʈ ����Ʈ
	{
		if (_iValue >= 100) _iValue = 99;
		pEffect = CEffectMgr::GetInstance()->GetEffect();

		// �ر�/��� �̺�Ʈ�� ���� �������� ���
		if (_bStressEvent)
		{
			m_vStressEventPos = m_pTransformCom->GetPos();
			m_vStressEventPos->y -= 2.f;
			pEffect->SetDamageEffect(2, _iValue, m_vStressEventPos, ATTACKTIME);
		}

		// �� ���� ���
		else
		{
			pEffect->SetDamageEffect(2, _iValue, m_pTransformCom->GetPos(), ATTACKTIME);
		}
		pEffect->SetActive(true);
	}

	if (m_iStress >= 100 && !m_bVirtue && !m_bAffliction)
	{
		tstring strHeroName = L"";

		switch (m_eHeroType)
		{
		case EHeroType::VESTAL:
			strHeroName = L"����";
			break;
		case EHeroType::HIGHWAYMAN:
			strHeroName = L"��󰭵�";
			break;
		case EHeroType::SHILEDBREAKER:
			strHeroName = L"�����ı���";
			break;
		case EHeroType::JESTER:
			strHeroName = L"����";
			break;
		case EHeroType::ENUM_END:
			break;
		default:
			break;
		}

		CSoundMgr::GetInstance()->StopSound(CHANNELID::NPC);
		CSoundMgr::GetInstance()->PlaySound(L"dun_test_long.wav", CHANNELID::NPC, 3.f);

		CUIMgr::GetInstance()->TextBoardOn(strHeroName + L"�� ������ ����ް� �ֽ��ϴ�...", {0.f, 200.f, 0.3f}, {500.f, 70.f, 1.f}, 5.f);

		int iNum = 0;

		if (dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer()))
		{
			if (dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->GetPrevVirtue())
			{
				iNum = 1;
				dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SetPrevVirtue(false);
			}
			else
			{
				iNum = 0;
				dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SetPrevVirtue(true);
			}
		}

		switch (iNum)
		{
		case 0:
			m_bVirtue = true;
			switch (rand() % 2)
			{
			case 0:
				m_eVirtue = EVirtue::COURAGEOUS;
				break;
			case 1:
				m_eVirtue = EVirtue::VIGOROUS;
				break;
			}
			break;
		case 1:
			m_bAffliction = true;
			switch (rand() % 2)
			{
			case 0:
				m_eAffliction = EAffliction::SELFISH;
				break;
			case 1:
				m_eAffliction = EAffliction::IRRATIONAL;
				break;
			}
			break;
		}
		//m_bAffliction = true;
		//m_eAffliction = EAffliction::SELFISH;
	}
	if (m_iStress > 200)
	{
		m_iStress = 200;
	}
}

void CHero::DecreaseStress(_int _iValue, _bool _bStressEvent)
{
	m_iStress -= _iValue;
	if (m_iStress < 0) m_iStress = 0;

	shared_ptr<CEffect> pEffect;

	// ����Ʈ ���� ����
	// ��� ����Ʈ
	{
		pEffect = CEffectMgr::GetInstance()->GetEffect();
		pEffect->SetHeadEffect(L"UI_Head_StressDown", m_pTransformCom->GetPos(), ATTACKTIME, false);
		pEffect->SetActive(true);
	}

	// ��Ʈ ����Ʈ
	{
		if (_iValue >= 100) _iValue = 99;
		pEffect = CEffectMgr::GetInstance()->GetEffect();

		// �ر�/��� �̺�Ʈ�� ���� �������� ���
		if (_bStressEvent)
		{
			m_vStressEventPos = m_pTransformCom->GetPos();
			m_vStressEventPos->y -= 2.f;
			pEffect->SetDamageEffect(2, _iValue, m_vStressEventPos, ATTACKTIME);
		}

		// �� ���� ���
		else
		{
			pEffect->SetDamageEffect(2, _iValue, m_pTransformCom->GetPos(), ATTACKTIME);
		}
		pEffect->SetActive(true);
	}
}

void CHero::DecreaseHP(_int _iValue, _bool _bStressEvent)
{
	CCreature::DecreaseHP(_iValue, _bStressEvent);
	GetHeroStatusEvent(true);

}

void CHero::OnAffliction()
{
	shared_ptr<CEffect> pEffect = CEffectMgr::GetInstance()->GetEffect();

	pEffect->SetTextureEffect(L"Alpha_Black", { 0.f, 0.f, 0.7f }, { 1280.f, 720.f, 1.f }, STRESSEVENTINTERVEL - 1.f, 200, true, true);
	pEffect->SetActive(true);

	pEffect = CEffectMgr::GetInstance()->GetEffect();

	pEffect->SetAnimEffect(m_strName + L"_Affliction", { 0.f, 0.f, 0.5f }, { 450.f, 420.f, 1.f }, STRESSEVENTINTERVEL - 1.f, true, true);
	pEffect->SetActive(true);

	CSoundMgr::GetInstance()->StopSound(CHANNELID::NPC);
	CSoundMgr::GetInstance()->PlaySound(L"dun_affliction.wav", CHANNELID::NPC, 1.f);

	tstring strAffiction;

	switch (m_eAffliction)
	{
	case EAffliction::SELFISH:
		strAffiction = L"�̱���";
		break;
	case EAffliction::IRRATIONAL:
		strAffiction = L"���̼���";
		break;
	default:
		strAffiction = L"";
		break;
	}

	CUIMgr::GetInstance()->TextBoardOn(strAffiction, { 0.f, -200.f, 0.3f }, { 230.f, 70.f, 1.f }, STRESSEVENTINTERVEL - 1.f);

	m_pStatInfo->SetAffliction(true);
}

void CHero::OnVirtue()
{
	shared_ptr<CEffect> pEffect = CEffectMgr::GetInstance()->GetEffect();

	pEffect->SetTextureEffect(L"Alpha_Black", { 0.f, 0.f, 0.7f }, { 1280.f, 720.f, 1.f }, STRESSEVENTINTERVEL - 1.f, 200, true, true);
	pEffect->SetActive(true);

	pEffect = CEffectMgr::GetInstance()->GetEffect();

	pEffect->SetAnimEffect(m_strName + L"_Virtue", { 0.f, 0.f, 0.5f }, { 400.f, 400.f, 1.f }, STRESSEVENTINTERVEL - 1.f, true, true);
	pEffect->SetActive(true);

	CSoundMgr::GetInstance()->StopSound(CHANNELID::NPC);
	CSoundMgr::GetInstance()->PlaySound(L"dun_virtue.wav", CHANNELID::NPC, 1.f);

	tstring strVirtue;
	
	switch (m_eVirtue)
	{
	case EVirtue::COURAGEOUS:
		strVirtue = L"�游�� ���";
		break;
	case EVirtue::VIGOROUS:
		strVirtue = L"���� �ռ�";
		break;
	case EVirtue::ENUM_END:
		break;
	default:
		break;
	}

	CUIMgr::GetInstance()->TextBoardOn(strVirtue, { 0.f, -200.f, 0.3f }, { 230.f, 70.f, 1.f }, STRESSEVENTINTERVEL - 1.f);

	m_pStatInfo->SetVirtue(true);
}

shared_ptr<CSkill> CHero::SelectSkill(_int _iSkillID)
{
	shared_ptr<CSkill> m_pSelectedSkill = (m_pVecSkill)[_iSkillID];

	// �������� �ʾҰų� ��� �� ���� ��ų�̸� ���� �Ұ�
	if (!m_pSelectedSkill->IsEquipped() || !m_pSelectedSkill->IsUnlocked())
	{
		return nullptr;
	}

	// ���� ��ġ���� ����� �� ���� ��ų�̸� ���� �Ұ�
	if (!m_pSelectedSkill->IsActivatePos(m_tCommonStat.iOrder))
	{
		return nullptr;
	}

	// �� ���� �� ��� Ƚ���� �� �� ��ų�̸� ���� �Ұ�
	if (!m_pSelectedSkill->CanActivateCnt())
	{
		return nullptr;
	}

	return m_pSelectedSkill;
}

void CHero::StressUp()
{
	srand(unsigned(time(NULL)));

	if (m_iStress >= 100)
	{
		// ���� ���� 0~1 ����
		_int iRand = rand() % 2;

		// ���� ���� 0~3 ����
		_int iAorV = rand() % 4;

		// 3 ������ ���
		if (3 == iAorV)
		{
			m_bVirtue = true;
			m_eVirtue = (EVirtue)iRand;
		}
		// �ƴϸ� �ر�
		else
		{
			m_bAffliction = true;
			m_eAffliction = (EAffliction)iRand;
		}
	}

	else if (m_iStress >= 200)
		m_bDeathDoor = true;
}

void CHero::AddComponent()
{
	if (m_bReady)
		return;

	shared_ptr<CComponent> pComponent;

	pComponent = m_pTransformCom = make_shared<CTransform>();
	m_pTransformCom->ReadyTransform();
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform",pComponent });

	pComponent = m_pBufCom = make_shared <CRcTex>(m_pGraphicDev);
	m_pBufCom->ReadyBuffer();
	m_mapComponent[ID_STATIC].insert({ L"Com_RCTex",pComponent });

	pComponent = m_pEffectBufCom = make_shared<CRcTex>(m_pGraphicDev);
	m_pEffectBufCom->ReadyBuffer();
	m_mapComponent[ID_STATIC].insert({ L"Com_EffectRCTex",pComponent });

	pComponent = m_pTextureCom = make_shared<CAnimator>(m_pGraphicDev);
	m_pTextureCom->SetAnimKey(m_strAnimKey, 0.05f);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animator", pComponent });

	pComponent = m_pColliderCom = make_shared<CCollider>(m_pGraphicDev);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider",pComponent });
	m_pColliderCom->SetScale({ 3.f, 3.f, 1.f });
	m_pColliderCom->SetPos(m_pTransformCom->GetPos());
}

void CHero::SwapSkill(_int _iIdx1, _int _iIdx2)
{
	swap(m_pVecSkill[_iIdx1], m_pVecSkill[_iIdx2]);
}

void CHero::GetHeroStatusEvent(_bool _bHpEvent)
{	
	shared_ptr<CUIObj> pFindUI = CUIMgr::GetInstance()->FindUI(L"UI_DungeonStatus");

	if (pFindUI) {

		//ü������ �̺�Ʈ
		if (_bHpEvent) {

			dynamic_pointer_cast<CDungeonStatus>(pFindUI)->GetEvent(true, m_eHeroType);

		}
		//��Ʈ���� �̺�Ʈ
		else {

			dynamic_pointer_cast<CDungeonStatus>(pFindUI)->GetEvent(false, m_eHeroType);

		}

	}


}

void CHero::GetHeroBuffEvent()
{
	shared_ptr<CUIObj> pFindUI = CUIMgr::GetInstance()->FindUI(L"UI_DungeonStatus");

	if(pFindUI)
		dynamic_pointer_cast<CDungeonStatus>(pFindUI)->GetBuff(m_eHeroType);

}

void CHero::ChangeAnimState()
{

	//// �ӽ�
	//if ((m_bAttacking1 || m_bAttacking2 || m_bAttacking3 || m_bAttacking4) && m_tCommonStat.iHp >= 0)
	//{
	//	if (m_bAttacking1)
	//		m_eCurAnimState = EAnimState::SKILL1;
	//	else if (m_bAttacking2)
	//		m_eCurAnimState = EAnimState::SKILL2;
	//	else if (m_bAttacking3)
	//		m_eCurAnimState = EAnimState::SKILL3;
	//	else if (m_bAttacking4)
	//		m_eCurAnimState = EAnimState::SKILL4;
	//}
	//else if (m_bHitted == true && m_tCommonStat.iHp > 0)
	//{
	//	m_ePrevAnimState = m_eCurAnimState;
	//	m_eCurAnimState = EAnimState::BESHOT;
	//}
	//else
	//{
	//	m_ePrevAnimState = m_eCurAnimState;
	//	m_eCurAnimState = EAnimState::COMBAT;
	//}	
}

void CHero::SetAnimDelay(const _float& fTimeDelta)
{
	//// �ǰ� �ð�
	//if (m_bHitted)
	//{
	//	m_fHittedTime -= fTimeDelta;
	//	if (m_fHittedTime < 0.f)
	//	{
	//		m_bHitted = false;
	//		m_fHittedTime = HITTEDTIME;
	//	}
	//}
	//
	//// ���� �ð�
	//if (m_bAttacking1 || m_bAttacking2 || m_bAttacking3 || m_bAttacking4 )
	//{
	//	m_fAttackTime -= fTimeDelta;
	//	if (m_fAttackTime < 0.f)
	//	{
	//		m_bAttacking1 = false;
	//		m_bAttacking2 = false;
	//		m_bAttacking3 = false;
	//		m_bAttacking4 = false;
	//
	//		m_fAttackTime = ATTACKTIME;
	//	}
	//}

	// �ر� or ��� ���� �ð� (UI�� ����)
	if ((m_bVirtue || m_bAffliction) && !m_bStressChanged)
	{
		m_fChangeTime -= fTimeDelta;
		if (m_fChangeTime < 0.f)
		{
			m_bStressChanged = true;
		}
	}

	// ��Ÿ ������ Ÿ��
	if (m_bDelay)
	{
		m_fDelayTime -= fTimeDelta;
		if (m_fDelayTime < 0.f)
		{
			m_bDelay = false;
		}
	}
}

void CHero::StressEvent()
{
	if (m_bAffliction)
	{
		switch (m_eAffliction)
		{
			// ��Ʈ����
		case EAffliction::SELFISH:
			m_iStress += 5;
			m_bDelay = true;
			// ����Ʈ �־��ֱ�
			break;
			// ����
		case EAffliction::IRRATIONAL:
			m_tCommonStat.iHp -= 3;
			m_bHitted = true;
			// ����Ʈ �־��ֱ�
			break;
		case EAffliction::ENUM_END:
			break;
		}
	}
	else if (m_bVirtue)
	{
		switch (m_eVirtue)
		{
		case EVirtue::COURAGEOUS:
			m_tCommonStat.iHp += 3;
			m_bHitted = true;
			// ����Ʈ �־��ֱ�
			break;
		case EVirtue::VIGOROUS:
			m_iStress -= 4;
			m_bHitted = true;
			// ����Ʈ �־��ֱ�
			break;
		case EVirtue::ENUM_END:
			break;
		}
	}
}

void CHero::Free()
{
	__super::Free();
}
