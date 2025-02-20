#include "pch.h"
#include "BrigandBloodletter.h"
#include"Export_Utility.h"
#include"StatView.h"


CBrigandBloodletter::CBrigandBloodletter(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMonster(pGraphicDev)
{
}

CBrigandBloodletter::CBrigandBloodletter(const CCreature& rhs)
	: CMonster(rhs)
{
}

CBrigandBloodletter::~CBrigandBloodletter()
{
}

HRESULT CBrigandBloodletter::ReadyGameObject()
{
	__super::ReadyGameObject();

	m_iSize = 1;

	// 스탯 설정
	m_tCommonStat.iHp = 40;
	m_tCommonStat.iMaxHp = 40;
	m_tCommonStat.iDodge = 0;
	m_tCommonStat.iSpeed = 2;
	m_tCommonStat.iAttackPower = 5;

	// 스킬 넣어주기
	vector<shared_ptr<CSkill>>	pVecSkill = {};
	int Skill1_Dot[2] = { 1,3 };
	_bool	m_bArrAttack1[8] = { 0, 0, 1, 0, 0, 0, 0, 0 };
	_bool	bTargetPos1[4] = { 1,1,1,1 };
	shared_ptr<CSkill> m_pBrigandBloodletter_1 = make_shared<CSkill>
		(L"Attack1", L"Brigand Bloodletter_Attack1", bTargetPos1, Skill1_Dot, 4.f, 0.f, 0.2f, 0.3f,
			m_bArrAttack1, 0, 10, 1, 0, 0);
	pVecSkill.push_back(m_pBrigandBloodletter_1);

	int Skill2_Dot[2] = { 0,0 };
	_bool	bArrAttack2[8] = { 1, 0, 0, 0, 0, 0, 0, 0 };
	_bool	bTargetPos2[4] = { 1,0,0,0 };
	shared_ptr<CSkill> m_pBrigandBloodletter_2 = make_shared<CSkill>
		(L"Attack2", L"Brigand Bloodletter_Attack2", bTargetPos2, Skill2_Dot, 0.f, 0.f, 2.f, 2.2f,
			bArrAttack2, 1, 30, 0, 0, 0);
	pVecSkill.push_back(m_pBrigandBloodletter_2);
	SetSkill(pVecSkill);

	m_pTransformCom->SetPosition(m_vPos.x, m_vPos.y, m_vPos.z);
	m_pTransformCom->SetScale(3.f, 3.f, 1.f);

	m_pTransformCom->SetAngle(m_vAngle);
	m_pTransformCom->Rotation(ROT_Y, PI / 2.f);

	m_pStatInfo->SettingInit(*(m_pTransformCom->GetPos()),
		m_tCommonStat.iHp, m_tCommonStat.iMaxHp, m_bIsHero);

	return E_NOTIMPL;
}

_int CBrigandBloodletter::UpdateGameObject(const _float& fTimeDelta)
{
	_int	iExit = __super::UpdateGameObject(fTimeDelta);

	Engine::AddRenderGroup(RENDER_ALPHA, shared_from_this());

	// Animation
	if (m_eCurAnimState != m_ePrevAnimState)
	{
		switch (m_eCurAnimState)
		{
		case EAnimState::COMBAT:
			m_pTextureCom->SetAnimKey(L"Brigand Bloodletter_Combat", 0.05f);
			m_pTransformCom->SetScale(3.f, 3.f, 3.f);
			m_pTransformCom->SetPosition(m_vPos.x, 3.f, m_vPos.z);

			break;
		case EAnimState::BESHOT:
			m_pTextureCom->SetAnimKey(L"Brigand Bloodletter_Hitted", 0.02f);
			m_pTransformCom->SetScale(3.f * 330.f / 303.f, 3.f * 345.f / 382.f, 3.f * 330.f / 303.f);
			m_pTransformCom->SetPosition(m_vPos.x, 3.f * 345.f / 382.f, m_vPos.z);

			break;
		case EAnimState::SKILL1:
			m_pTextureCom->SetAnimKey(L"Brigand Bloodletter_Attack1", 0.02f);
			m_pTransformCom->SetScale(3.f * 482.f / 303.f, 3.f * 383.f / 382.f, 3.f * 482.f / 303.f);
			m_pTransformCom->SetPosition(m_vPos.x, 3.f * 383.f / 382.f, m_vPos.z);

			CSoundMgr::GetInstance()->StopSound(CHANNELID::MONSTER);
			CSoundMgr::GetInstance()->PlaySound(L"En_brigblood_whipsingle.wav", CHANNELID::MONSTER, 1.f);

			break;
		case EAnimState::SKILL2:
			m_pTextureCom->SetAnimKey(L"Brigand Bloodletter_Attack2", 0.02f);
			m_pTransformCom->SetScale(3.f * 474.f / 303.f, 3.f * 379.f / 382.f, 3.f * 474.f / 303.f);
			m_pTransformCom->SetPosition(m_vPos.x, 3.f * 379.f / 382.f, m_vPos.z);
			CSoundMgr::GetInstance()->StopSound(CHANNELID::MONSTER);
			CSoundMgr::GetInstance()->PlaySound(L"En_brigblood_pointblank.wav", CHANNELID::MONSTER, 1.f);
			break;
		case EAnimState::CORPSE:
			m_pTextureCom->SetAnimKey(L"Brigand Bloodletter_Dead", 0.02f);
			m_pTransformCom->SetScale(3.f * 375.f / 303.f, 3.f * 167.f / 382.f, 3.f * 375.f / 303.f);
			m_pTransformCom->SetPosition(m_vPos.x, 3.f * 167.f / 382.f, m_vPos.z);

			break;
		case EAnimState::DEATH:
			m_pTextureCom->SetAnimKey(L"Brigand Bloodletter_Death", 0.02f);
			m_pTransformCom->SetScale(3.f, 3.f, 3.f);
			break;
		}
		m_ePrevAnimState = m_eCurAnimState;
	}

	// 임시
	if ((IsAttacking()) && !m_bCorpse)
	{
		for (int i = 0; i < 7; i++)
		{
			if (GetAttacking(i))
			{
				switch (i)
				{
				case 0:
					m_eCurAnimState = EAnimState::SKILL1;
					break;
				case 1:
					m_eCurAnimState = EAnimState::SKILL2;
					break;
				case 2:
					m_eCurAnimState = EAnimState::SKILL3;
					break;
				case 3:
					m_eCurAnimState = EAnimState::SKILL4;
					break;
				case 4:
					m_eCurAnimState = EAnimState::SKILL5;
					break;
				case 5:
					m_eCurAnimState = EAnimState::SKILL6;
					break;
				case 6:
					m_eCurAnimState = EAnimState::SKILL7;
					break;
				}
				break;
			}

		}
	}
	else if (m_bDeath)
	{
		m_ePrevAnimState = m_eCurAnimState;
		m_eCurAnimState = EAnimState::DEATH;
	}
	else if (m_bCorpse)
	{
		m_ePrevAnimState = m_eCurAnimState;
		m_eCurAnimState = EAnimState::CORPSE;
	}
	else if (m_bHitted == true && !m_bCorpse)
	{
		m_ePrevAnimState = m_eCurAnimState;
		m_eCurAnimState = EAnimState::BESHOT;
	}
	else
	{
		m_ePrevAnimState = m_eCurAnimState;
		m_eCurAnimState = EAnimState::COMBAT;
	}

	// 시체 여부
	if (m_tCommonStat.iHp <= 0 && !m_bCorpse && !m_bDeath)
	{
		BleedCure();
		BlightCure();
		m_bCorpse = true;

		m_tCommonStat.iHp = 10;
		m_tCommonStat.iMaxHp = 10;
		// 스탯갱신
		m_pStatInfo->SetMaxHp(m_tCommonStat.iMaxHp);
		m_pStatInfo->SetHp(m_tCommonStat.iHp);
		m_pStatInfo->SetCorpse(true);
	}

	// 사망 여부
	if (m_tCommonStat.iHp <= 0 && m_bCorpse)
	{
		m_bCorpse = false;
		m_bDeath = true;
		m_tCommonStat.iHp = -100;

		bStatBarOn = false;
	}

	// 피격 시간
	if (m_bHitted)
	{
		m_fHittedTime -= fTimeDelta;
		if (m_fHittedTime < 0.f)
		{
			m_bHitted = false;
			m_fHittedTime = HITTEDTIME;
		}
	}

	// 공격1 시간
	if (GetAttacking(0))
	{
		m_fAttack1Time -= fTimeDelta;
		if (m_fAttack1Time < 0.f)
		{
			SetAttacking(false, 0);
			m_fAttack1Time = ATTACKTIME;
		}
	}

	// 공격2 시간
	if (GetAttacking(1))
	{
		m_fAttack2Time -= fTimeDelta;
		if (m_fAttack2Time < 0.f)
		{
			SetAttacking(false, 1);
			m_fAttack2Time = ATTACKTIME;
		}
	}


	return iExit;
}

void CBrigandBloodletter::LateUpdateGameObject()
{
	__super::LateUpdateGameObject();
}

void CBrigandBloodletter::RenderGameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->GetWorld());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	if (m_bSetAlpha) {
		
	}
	

	m_pTextureCom->SetAnimTexture();
	m_pBufCom->RenderBuffer();
	m_pEffectBufCom->RenderBuffer();

	if (m_bSetAlpha) {
	
	}

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	__super::RenderGameObject();
}

void CBrigandBloodletter::AddComponent()
{

	shared_ptr<CComponent> pComponent;

	_vec3 vPosTemp = { 0.f,0.f,0.f };

	pComponent = m_pTransformCom = make_shared<CTransform>();
	m_pTransformCom->ReadyTransform();
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform",pComponent });

	pComponent = m_pTextureCom = make_shared<CAnimator>(m_pGraphicDev);
	m_pTextureCom->SetAnimKey(L"Brigand Bloodletter_Combat", 0.05f);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animator",pComponent });

	pComponent = m_pColliderCom = make_shared<CCollider>(m_pGraphicDev);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider",pComponent });
	m_pColliderCom->SetScale({ 3.f, 3.f, 1.f });
	m_pColliderCom->SetPos(m_pTransformCom->GetPos());

	//pComponent = m_pEffectTransformCom = make_shared<CTransform>();
	//NULL_CHECK_MSG(pComponent, L"Make Player TransformCom Failed");
	//m_pEffectTransformCom->ReadyTransform();
	//m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform",pComponent });
	//
	//pComponent = m_pEffectTextureCom = make_shared<CAnimator>(m_pGraphicDev);
	//m_pEffectTextureCom->SetAnimKey(L"Brigand Bloodletter_Combat", 0.05f);
	//m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animator",pComponent });

	__super::AddComponent();
}

void CBrigandBloodletter::Free()
{
	__super::Free();
}
