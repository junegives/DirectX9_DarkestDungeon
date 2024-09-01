#include "pch.h"
#include "ShieldBreaker.h"

#include "HeroStat.h"
#include"Export_Utility.h"
#include"StatView.h"
#include "UIMgr.h"
#include"SoundMgr.h"

CShieldBreaker::CShieldBreaker(LPDIRECT3DDEVICE9 pGraphicDev) : CHero(pGraphicDev)
{
}

CShieldBreaker::CShieldBreaker(const CShieldBreaker& rhs) : CHero(rhs)
{
}

CShieldBreaker::~CShieldBreaker()
{
}

HRESULT CShieldBreaker::ReadyGameObject()
{
	if (m_bReady)
		return S_OK;

	__super::ReadyGameObject();

	m_bIsHero = true;
	m_strName = L"ShieldBreaker";

	m_eHeroType = EHeroType::SHILEDBREAKER;

	// 스킬 세팅
	{
		_int  DotDamZero[2] = { 0, 0 };

		// 찌르기
		_bool arrActivatePos1[4] = { 1, 1, 1, 0 };
		_bool arrTargetPos1[4] = { 1, 1, 1, 1 };
		_bool arrAttack1[8] = { 1, 0, 0, 0, 0, 0, 0, 0 };
		_bool arrToEnemy1[6] = { 1, 1, 1, 1, 1, 1 };

		shared_ptr<CSkill> pSkill1 = make_shared<CSkill>
			(L"Pierce", L"ShieldBreaker_Pierce", L"Pierce_Img", L"Pierce_Effect", L"", L"",
				arrActivatePos1, arrTargetPos1, arrAttack1, arrToEnemy1, DotDamZero, -4.f,  1.f, 0.9f, 1.3f, -1, 0, 0, 0, 1, 1);

		{
			pSkill1->GetDescription()->m_eDescriptionType = EDescriptionType::SKILL2;
			pSkill1->GetDescription()->m_strTitle = L"찌르기";
			pSkill1->GetDescription()->m_strSubTitle = L"근거리";
			pSkill1->GetDescription()->m_strMainContent = L"피해 보정 : -10%%\n치명타 보정 : +30%%";
			pSkill1->GetDescription()->m_strColorTitle1 = L"이동";
			pSkill1->GetDescription()->m_tColor1 = SKYBLUE;
			pSkill1->GetDescription()->m_strColorContent1 = L"자신 : 1열 앞으로";
		}

		// 구멍 내기
		_bool arrActivatePos2[4] = { 1, 1, 1, 1 };
		_bool arrTargetPos2[4] = { 0, 1, 1, 0 };
		_bool arrAttack2[8] = { 1, 0, 0, 0, 0, 0, 0, 0 };
		_bool arrToEnemy2[6] = { 1, 1, 1, 1, 1, 1 };
			
		shared_ptr<CSkill> pSkill2 = make_shared<CSkill>
			(L"Puncture", L"ShieldBreaker_Puncture", L"Puncture_Img", L"Puncture_Effect", L"", L"",
				arrActivatePos2, arrTargetPos2, arrAttack2, arrToEnemy2, DotDamZero, -3.f, 1.f, 0.5f, 0.8f, -1, -1,0,0, 1, 1);

		{
			pSkill2->GetDescription()->m_eDescriptionType = EDescriptionType::SKILL3;
			pSkill2->GetDescription()->m_strTitle = L"구멍내기";
			pSkill2->GetDescription()->m_strSubTitle = L"근거리";
			pSkill2->GetDescription()->m_strMainContent = L"피해 보정 : -50%%\n치명타 보정 : -20%%";
			pSkill2->GetDescription()->m_strColorTitle1 = L"이동";
			pSkill2->GetDescription()->m_tColor1 = SKYBLUE;
			pSkill2->GetDescription()->m_strColorContent1 = L"자신 : 1열 앞으로";
			pSkill2->GetDescription()->m_strColorTitle2 = L"이동";
			pSkill2->GetDescription()->m_tColor2 = SKYBLUE;
			pSkill2->GetDescription()->m_strColorContent2 = L"상대 : 1열 당기기";
		}

		// 독사의 입맞춤
		_bool arrActivatePos3[4] = { 1, 1, 0, 0 };
		_bool arrTargetPos3[4] = { 1, 1, 0, 0 };
		_bool arrAttack3[8] = { 0, 1, 0, 0, 0, 0, 0, 0 };
		_bool arrToEnemy3[6] = { 1, 1, 1, 1, 1, 1 };

		_int DotDam3[2] = { 3, 3 };

		shared_ptr<CSkill> pSkill3 = make_shared<CSkill>
			(L"AddersKiss", L"ShieldBreaker_AddersKiss", L"AddersKiss_Img", L"AddersKiss_Effect", L"AddersKiss_Target_Effect", L"",
				arrActivatePos3, arrTargetPos3, arrAttack3, arrToEnemy3, DotDam3, -3.f,  1.f, 0.5f, 0.8f, -1, 0, 0, 0, 1, -1);

		{
			pSkill3->GetDescription()->m_eDescriptionType = EDescriptionType::SKILL3;
			pSkill3->GetDescription()->m_strTitle = L"독사의 입맞춤";
			pSkill3->GetDescription()->m_strSubTitle = L"근거리";
			pSkill3->GetDescription()->m_strMainContent = L"피해 보정 : -50%%\n치명타 보정 : -20%%";
			pSkill3->GetDescription()->m_strColorTitle1 = L"이동";
			pSkill3->GetDescription()->m_tColor1 = SKYBLUE;
			pSkill3->GetDescription()->m_strColorContent1 = L"자신 : 1열 뒤로";
			pSkill3->GetDescription()->m_strColorTitle2 = L"중독";
			pSkill3->GetDescription()->m_tColor2 = LIME;
			pSkill3->GetDescription()->m_strColorContent2 = L"데미지:3 라운드:3";
		}

		// 꿰뚫기
		_bool arrActivatePos4[4] = { 1, 1, 0, 0 };
		_bool arrTargetPos4[4] = { 1, 1, 1, 1 };
		_bool arrAttack4[8] = { 0, 1, 0, 0, 0, 0, 0, 0 };
		_bool arrToEnemy4[6] = { 1, 1, 1, 1, 1, 1 };

		_int DotDam4[2] = { 1, 3 };

		shared_ptr<CSkill> pSkill4 = make_shared<CSkill>
			(L"Impale", L"ShieldBreaker_Impale", L"Impale_Img", L"Impale_Effect", L"Impale_Target_Effect", L"",
				arrActivatePos4, arrTargetPos4, arrAttack4, arrToEnemy4, DotDam4, -4.f, 1.f, 0.4f, 0.6f, -1, 0, 0, 1, 0, -1);

		{
			pSkill4->GetDescription()->m_eDescriptionType = EDescriptionType::SKILL3;
			pSkill4->GetDescription()->m_strTitle = L"꿰뚫기";
			pSkill4->GetDescription()->m_strSubTitle = L"원거리";
			pSkill4->GetDescription()->m_strMainContent = L"피해 보정 : -60%%\n치명타 보정 : -40%%";
			pSkill4->GetDescription()->m_strColorTitle1 = L"이동";
			pSkill4->GetDescription()->m_tColor1 = SKYBLUE;
			pSkill4->GetDescription()->m_strColorContent1 = L"자신 : 1열 뒤로";
			pSkill4->GetDescription()->m_strColorTitle2 = L"중독";
			pSkill4->GetDescription()->m_tColor2 = LIME;
			pSkill4->GetDescription()->m_strColorContent2 = L"데미지:1 라운드:3";
		}

		shared_ptr<CSkill> pSkill5 = make_shared<CSkill>
			(L"Captivate", L"ShieldBreaker_Captivate", L"Captivate_Img", L"Captivate_Effect", L"", L"",
				arrActivatePos4, arrTargetPos4, arrAttack4, arrToEnemy4, DotDam4, -3.f, 1.f, 0.4f, 0.6f, -1, 0, 0, 1, 1, -1);

		{
			pSkill5->GetDescription()->m_eDescriptionType = EDescriptionType::SKILL2;
			pSkill5->GetDescription()->m_strTitle = L"휘감기";
			pSkill5->GetDescription()->m_strSubTitle = L"근거리";
			pSkill5->GetDescription()->m_strMainContent = L"피해 보정 : -25%%\n치명타 보정 : +10%%";
			pSkill5->GetDescription()->m_strColorTitle1 = L"중독";
			pSkill5->GetDescription()->m_tColor1 = LIME;
			pSkill5->GetDescription()->m_strColorContent1 = L"데미지:3 라운드:3";
		}

		shared_ptr<CSkill> pSkill6 = make_shared<CSkill>
			(L"Expose", L"ShieldBreaker_Expose", L"Expose_Img", L"Expose_Effect", L"", L"",
				arrActivatePos4, arrTargetPos4, arrAttack4, arrToEnemy4, DotDam4, 0.f, 1.f, 0.4f, 0.6f, -1, 0, 0, 1, 1, -1);

		{
			pSkill6->GetDescription()->m_eDescriptionType = EDescriptionType::SKILL2;
			pSkill6->GetDescription()->m_strTitle = L"노출";
			pSkill6->GetDescription()->m_strSubTitle = L"근거리";
			pSkill6->GetDescription()->m_strMainContent = L"피해 보정 : -40%%\n치명타 보정 : +5%%";
			pSkill6->GetDescription()->m_strColorTitle1 = L"이동";
			pSkill6->GetDescription()->m_tColor1 = SKYBLUE;
			pSkill6->GetDescription()->m_strColorContent1 = L"자신 : 1열 뒤로";
		}

		pSkill5->SetEquipped(false);
		pSkill5->SetUnlocked(false);

		pSkill6->SetEquipped(false);
		pSkill6->SetUnlocked(false);

		m_pVecSkill.push_back(pSkill1);
		m_pVecSkill.push_back(pSkill2);
		m_pVecSkill.push_back(pSkill3);
		m_pVecSkill.push_back(pSkill4);

		m_pVecSkill.push_back(pSkill5);
		m_pVecSkill.push_back(pSkill6);


		m_pVecSkillAll.push_back(pSkill1);
		m_pVecSkillAll.push_back(pSkill2);
		m_pVecSkillAll.push_back(pSkill3);
		m_pVecSkillAll.push_back(pSkill4);
		m_pVecSkillAll.push_back(pSkill5);
		m_pVecSkillAll.push_back(pSkill6);
	}

	// 영웅 스탯
	{
		m_tCommonStat.iHp = 40;
		m_tCommonStat.iMaxHp = 40;
		m_tCommonStat.iDodge = 10;
		m_tCommonStat.iSpeed = 5;
		m_tCommonStat.iAttackPower = 7;
		m_tCommonStat.iOrder = 0;
	}

	// 테스트용
	{
		m_eAnimState = EAnimState::IDLE;
		//m_vAngle = { 0.f, PI / 2, 0.f };

		m_pTransformCom->SetPosition(m_vPos.x, m_vPos.y, m_vPos.z);
		m_pTransformCom->SetScale(2.f, 3.f, 1.f);

		m_pTransformCom->SetAngle(m_vAngle);
		m_pTransformCom->Rotation(ROT_Y, PI / 2.f);

		m_pTextureCom->SetAnimKey(L"ShieldBreaker_Idle", 0.04f);
		m_vOriginSize = m_pTextureCom->GetTextureSize();
	}

	// 영웅 스탯 출력창
	m_pStatUI = make_shared<CHeroStat>(m_pGraphicDev);
	m_pStatUI->SetHero(dynamic_pointer_cast<CShieldBreaker>(shared_from_this()));
	m_pStatUI->AwakeGameObject();
	m_pStatUI->ReadyGameObject();

	CUIMgr::GetInstance()->AddUIObject(L"UI_HeroStat_Shieldbreaker", m_pStatUI);

	m_pStatInfo->SettingInit(*(m_pTransformCom->GetPos()),
		m_tCommonStat.iHp, m_tCommonStat.iMaxHp, m_bIsHero);
	m_pStatInfo->SetIsHeroType(1);

	m_bReady = true;

	return S_OK;
}

_int CShieldBreaker::UpdateGameObject(const _float& fTimeDelta)
{
	_int iExit = __super::UpdateGameObject(fTimeDelta);

	Engine::AddRenderGroup(RENDER_ALPHA, shared_from_this());

	ChangeAnim();

	if ((IsAttacking()))
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
	else if (m_bHitted == true)
	{
		m_ePrevAnimState = m_eCurAnimState;
		m_eCurAnimState = EAnimState::BESHOT;
	}
	else
	{
		m_ePrevAnimState = m_eCurAnimState;
		m_eCurAnimState = EAnimState::COMBAT;
	}

	// 사망 여부
	//if (m_tCommonStat.iHp <= 0 && !m_bBeforeDeath)
	//{
	//	m_bBeforeDeath = true;
	//}

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
		m_fAttackTime -= fTimeDelta;
		if (m_fAttackTime < 0.f)
		{
			SetAttacking(false, 0);
			m_fAttackTime = ATTACKTIME;
		}
	}

	// 공격2 시간
	if (GetAttacking(1))
	{
		m_fAttackTime -= fTimeDelta;
		if (m_fAttackTime < 0.f)
		{
			SetAttacking(false, 1);
			m_fAttackTime = ATTACKTIME;
		}
	}

	// 공격3 시간
	if (GetAttacking(2))
	{
		m_fAttackTime -= fTimeDelta;
		if (m_fAttackTime < 0.f)
		{
			SetAttacking(false, 2);
			m_fAttackTime = ATTACKTIME;
		}
	}

	// 공격4 시간
	if (GetAttacking(3))
	{
		m_fAttackTime -= fTimeDelta;
		if (m_fAttackTime < 0.f)
		{
			SetAttacking(false, 3);
			m_fAttackTime = ATTACKTIME;
		}
	}

	// 마을용
	if (!m_bInDungeon)
	{
		if (m_bSelected)
			m_eCurAnimState = EAnimState::COMBAT;

		else
			m_eCurAnimState = EAnimState::IDLE;
	}

	return iExit;
}

void CShieldBreaker::LateUpdateGameObject()
{
	__super::LateUpdateGameObject();
}

void CShieldBreaker::RenderGameObject()
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

void CShieldBreaker::AddComponent()
{
	__super::AddComponent();
}

void CShieldBreaker::ChangeAnim()
{
	_float fXpos, fYpos;
	// Animation
	if (m_eCurAnimState != m_ePrevAnimState)
	{
		switch (m_eCurAnimState)
		{
		case EAnimState::IDLE:
			m_pTextureCom->SetAnimKey(L"ShieldBreaker_Idle", 0.04f);
			break;
		case EAnimState::WALK:
			m_pTextureCom->SetAnimKey(L"ShieldBreaker_Walk", 0.04f);
			break;
		case EAnimState::COMBAT:
			m_pTextureCom->SetAnimKey(L"ShieldBreaker_Combat", 0.04f);
			break;
		case EAnimState::BESHOT:
			m_pTextureCom->SetAnimKey(L"ShieldBreaker_Defend", 0.04f);
			break;
		case EAnimState::SKILL1:
			m_pTextureCom->SetAnimKey(L"ShieldBreaker_Pierce", 0.04f);
			CSoundMgr::GetInstance()->PlaySound(L"Hero_sb_pierce.wav", CHANNELID::SHIELDBREAKER, 1.f);

			break;
		case EAnimState::SKILL2:
			m_pTextureCom->SetAnimKey(L"ShieldBreaker_Puncture", 0.04f);
			CSoundMgr::GetInstance()->PlaySound(L"Hero_sb_puncher.wav", CHANNELID::SHIELDBREAKER, 1.f);

			break;
		case EAnimState::SKILL3:
			m_pTextureCom->SetAnimKey(L"ShieldBreaker_AddersKiss", 0.04f);
			CSoundMgr::GetInstance()->PlaySound(L"Hero_sb_adderskiss.wav", CHANNELID::SHIELDBREAKER, 1.f);

			break;
		case EAnimState::SKILL4:
			m_pTextureCom->SetAnimKey(L"ShieldBreaker_Impale", 0.04f);
			CSoundMgr::GetInstance()->PlaySound(L"Hero_sb_impale.wav", CHANNELID::SHIELDBREAKER, 1.f);

			break;
		case EAnimState::AFFLICTION:
			m_pTextureCom->SetAnimKey(L"ShieldBreaker_Affliction", 0.04f);
			break;
		case EAnimState::VIRTUE:
			m_pTextureCom->SetAnimKey(L"ShieldBreaker_Virtue", 0.04f);
			break;
		case EAnimState::DEATH:
			m_pTextureCom->SetAnimKey(L"Hero_Death", 0.02f);
			break;
		}
		m_ePrevAnimState = m_eCurAnimState;

	}

	_vec2 vcurPos = m_pTextureCom->GetTextureSize();

	if (m_eCurAnimState == EAnimState::IDLE || m_eCurAnimState == EAnimState::WALK || m_eCurAnimState == EAnimState::COMBAT) {
		fXpos = (vcurPos.x / m_vOriginSize.x);
		fYpos = (vcurPos.y / m_vOriginSize.y);
	}
	else {
		fXpos = (vcurPos.x / m_vOriginSize.x) / 1.9f;
		fYpos = (vcurPos.y / m_vOriginSize.y) / 1.9f;
	}

	m_pTransformCom->SetScale(2.f * fXpos, 3.f * fYpos, 2.f * fXpos);

}