#include "pch.h"
#include "BoneSoldier.h"
#include"Export_Utility.h"
#include"StatView.h"

CBoneSoldier::CBoneSoldier(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMonster(pGraphicDev)
{
}

CBoneSoldier::CBoneSoldier(const CCreature& rhs)
	: CMonster(rhs)
{
}

CBoneSoldier::~CBoneSoldier()
{
}

HRESULT CBoneSoldier::ReadyGameObject()
{
	__super::ReadyGameObject();

	m_iSize = 1;

	// 스탯 설정
	m_tCommonStat.iHp = 20;
	m_tCommonStat.iMaxHp = 20;
	m_tCommonStat.iDodge = 15;
	m_tCommonStat.iSpeed = 6;
	m_tCommonStat.iAttackPower = 5;

	// 스킬 넣어주기
	vector<shared_ptr<CSkill>>	pVecSkill = {};
	int Skill1_Dot[2] = { 1,2 };
	_bool	m_bArrAttack1[8] = { 0, 1, 0, 0, 0, 0, 0, 0 };
	_bool	bTargetPos1[4] = { 1,1,0,0 };
	shared_ptr<CSkill> m_pBoneSoldier_1 = make_shared<CSkill>
		(L"Attack1", L"Bone Soldier_Attack1", bTargetPos1, Skill1_Dot, 0.f, 0.f, 0.8f, 1.f,
			m_bArrAttack1, 0, 0, 0, true);
	pVecSkill.push_back(m_pBoneSoldier_1);

	SetSkill(pVecSkill);

	m_pTransformCom->SetPosition(m_vPos.x, m_vPos.y, m_vPos.z);
	m_pTransformCom->SetScale(2.5f, 2.5f, 1.f);

	m_pTransformCom->SetAngle(m_vAngle);
	m_pTransformCom->Rotation(ROT_Y, PI / 2.f);

	m_pStatInfo->SettingInit(*(m_pTransformCom->GetPos()),
		m_tCommonStat.iHp, m_tCommonStat.iMaxHp, m_bIsHero);

	return E_NOTIMPL;
}

_int CBoneSoldier::UpdateGameObject(const _float& fTimeDelta)
{
	_int	iExit = __super::UpdateGameObject(fTimeDelta);

	Engine::AddRenderGroup(RENDER_ALPHA, shared_from_this());

	// Animation
	if (m_eCurAnimState != m_ePrevAnimState)
	{
		switch (m_eCurAnimState)
		{
		case EAnimState::COMBAT:
			m_pTextureCom->SetAnimKey(L"Bone Soldier_Combat", 0.05f);
			m_pTransformCom->SetScale(2.5f, 2.5f, 2.5f);
			m_pTransformCom->SetPosition(m_vPos.x, 2.5f, m_vPos.z);
			break;
		case EAnimState::BESHOT:
			m_pTextureCom->SetAnimKey(L"Bone Soldier_Hitted", 0.02f);
			m_pTransformCom->SetScale(2.5f * 171.f / 205.f, 2.5f * 269.f / 310.f, 2.5f * 171.f / 205.f);
			m_pTransformCom->SetPosition(m_vPos.x, 2.5f * 269.f / 310.f, m_vPos.z);
			break;
		case EAnimState::SKILL1:
			m_pTextureCom->SetAnimKey(L"Bone Soldier_Attack1", 0.02f);
			m_pTransformCom->SetScale(2.5f * 323.f / 205.f, 2.5f * 300.f / 310.f, 2.5f * 323.f / 205.f);
			m_pTransformCom->SetPosition(m_vPos.x, 2.5f * 269.f / 310.f, m_vPos.z);

			CSoundMgr::GetInstance()->StopSound(CHANNELID::MONSTER);
			CSoundMgr::GetInstance()->PlaySound(L"En_BoneSol_Attack.wav", CHANNELID::MONSTER, 1.f);

			break;
		case EAnimState::CORPSE:
			m_pTextureCom->SetAnimKey(L"Bone Soldier_Dead", 0.02f);
			m_pTransformCom->SetScale(2.5f * 215.f / 205.f, 2.5f * 146.f / 310.f, 2.5f * 215.f / 205.f);
			m_pTransformCom->SetPosition(m_vPos.x, 2.5f * 146.f / 310.f, m_vPos.z);
			break;
		case EAnimState::DEATH:
			m_pTextureCom->SetAnimKey(L"Bone Soldier_Death", 0.02f);
			m_pTransformCom->SetScale(2.5f, 2.5f, 2.5f);
			break;
		}
		m_ePrevAnimState = m_eCurAnimState;

	}

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


	return iExit;
}

void CBoneSoldier::LateUpdateGameObject()
{
	__super::LateUpdateGameObject();
}

void CBoneSoldier::RenderGameObject()
{

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->GetWorld());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	//m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	// m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_pTextureCom->SetAnimTexture();
	m_pBufCom->RenderBuffer();
	m_pEffectBufCom->RenderBuffer();

	//m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	__super::RenderGameObject();
}

void CBoneSoldier::AddComponent()
{

	shared_ptr<CComponent> pComponent;

	_vec3 vPosTemp = { 0.f,0.f,0.f };

	pComponent = m_pTransformCom = make_shared<CTransform>();
	if (nullptr == pComponent) {
		//MSG_BOX("Make BoneSoldier TransformCom Failed");
	}
	m_pTransformCom->ReadyTransform();
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform",pComponent });

	pComponent = m_pTextureCom = make_shared<CAnimator>(m_pGraphicDev);
	m_pTextureCom->SetAnimKey(L"Bone Soldier_Combat", 0.05f);
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
	//m_pEffectTextureCom->SetAnimKey(L"Bone Soldier_Combat", 0.05f);
	//m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animator",pComponent });

	__super::AddComponent();
}

void CBoneSoldier::Free()
{
	__super::Free();
}
