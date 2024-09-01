#include "pch.h"
#include "Jester.h"

#include "HeroStat.h"
#include"Export_Utility.h"
#include"StatView.h"
#include "UIMgr.h"
#include"SoundMgr.h"

CJester::CJester(LPDIRECT3DDEVICE9 pGraphicDev)
	: CHero(pGraphicDev)
{
}

CJester::CJester(const CJester& rhs)
	: CHero(rhs)
{
}

CJester::~CJester()
{
}

HRESULT CJester::ReadyGameObject()
{
	if (m_bReady)
		return S_OK;

	__super::ReadyGameObject();

	m_bIsHero = true;
	m_strName = L"Jester";

	m_eHeroType = EHeroType::JESTER;

	// 스킬 세팅
	{
		_int  DotDamZero[2] = { 0, 0 };

		// 단검 찌르기
		_bool arrActivatePos1[4] = { 1, 1, 1, 1 };
		_bool arrTargetPos1[4] = { 1, 1, 1, 0 };
		_bool arrAttack1[8] = { 1, 0, 0, 0, 0, 0, 1, 0 };
		_bool arrToEnemy1[6] = { 0, 1, 1, 1, 1, 1 };

		shared_ptr<CSkill> pSkill1 = make_shared<CSkill>
			(L"DirkStab", L"Jester_DirkStab", L"DirkStab_Img", L"", L"", L"Hero_jest_sliceoff.wav",
				arrActivatePos1, arrTargetPos1, arrAttack1, arrToEnemy1, DotDamZero, -2.5f, 1.f, 1.f, 1.3f, -1, 0, 0, 0, 1, 1);

		{
			pSkill1->GetDescription()->m_eDescriptionType = EDescriptionType::SKILL3;
			pSkill1->GetDescription()->m_strTitle = L"단검 찌르기";
			pSkill1->GetDescription()->m_strSubTitle = L"근거리";
			pSkill1->GetDescription()->m_strMainContent = L"피해 보정 : 0%%\n치명타 보정 : +30%%";
			pSkill1->GetDescription()->m_strColorTitle1 = L"이동";
			pSkill1->GetDescription()->m_tColor1 = SKYBLUE;
			pSkill1->GetDescription()->m_strColorContent1 = L"자신 : 1열 앞으로";
			pSkill1->GetDescription()->m_strColorTitle2 = L"강화";
			pSkill1->GetDescription()->m_tColor2 = ORANGE;
			pSkill1->GetDescription()->m_strColorContent2 = L"대단원 +30%%";
		}

		// 수확의 낫질
		_bool arrActivatePos2[4] = { 0, 1, 1, 1 };
		_bool arrTargetPos2[4] = { 1, 1, 1, 1 };
		_bool arrAttack2[8] = { 0, 0, 1, 0, 0, 0, 1, 0 };
		_bool arrToEnemy2[6] = { 0, 1, 1, 1, 1, 1 };

		_int  DotDam2[2] = { 1, 3 };

		shared_ptr<CSkill> pSkill2 = make_shared<CSkill>
			(L"Harvest", L"Jester_Harvest", L"Harvest_Img", L"", L"", L"Hero_jest_harvest.wav",
				arrActivatePos2, arrTargetPos2, arrAttack2, arrToEnemy2, DotDam2, -3.f, 1.f, 0.5f, 0.8f, -1, 0, 0, 1, 1, 0);

		{
			pSkill2->GetDescription()->m_eDescriptionType = EDescriptionType::SKILL2;
			pSkill2->GetDescription()->m_strTitle = L"수확의 낫질";
			pSkill2->GetDescription()->m_strSubTitle = L"근거리";
			pSkill2->GetDescription()->m_strMainContent = L"피해 보정 : -50%%\n치명타 보정 : -20%%";
			pSkill2->GetDescription()->m_strColorTitle1 = L"출혈";
			pSkill2->GetDescription()->m_tColor1 = RED;
			pSkill2->GetDescription()->m_strColorContent1 = L"데미지:3 라운드:1";
		}

		// 대단원
		_bool arrActivatePos3[4] = { 1, 1, 0, 0 };
		_bool arrTargetPos3[4] = { 1, 1, 1, 1 };
		_bool arrAttack3[8] = { 1, 0, 0, 0, 0, 0, 1, 0 };
		_bool arrToEnemy3[6] = { 1, 1, 0, 1, 1, 1 };

		shared_ptr<CSkill> pSkill3 = make_shared<CSkill>
			(L"Finale", L"Jester_Finale", L"Finale_Img", L"Finale_Effect", L"Finale_Target_Effect", L"Hero_jest_finale.wav",
				arrActivatePos3, arrTargetPos3, arrAttack3, arrToEnemy3, DotDamZero, -2.f, 1.f, 1.5f, 2.f, -1, 0, 0, 0, 1, -5);

		{
			pSkill3->GetDescription()->m_eDescriptionType = EDescriptionType::SKILL3;
			pSkill3->GetDescription()->m_strTitle = L"대단원";
			pSkill3->GetDescription()->m_strSubTitle = L"근거리";
			pSkill3->GetDescription()->m_strMainContent = L"피해 보정:+50\n치명타 보정:+100%%";
			pSkill3->GetDescription()->m_strColorTitle1 = L"이동";
			pSkill3->GetDescription()->m_tColor1 = SKYBLUE;
			pSkill3->GetDescription()->m_strColorContent1 = L"자신 : 맨 뒤로";
			pSkill3->GetDescription()->m_strColorTitle2 = L"특수";
			pSkill3->GetDescription()->m_tColor2 = WHITE;
			pSkill3->GetDescription()->m_strColorContent2 = L"전투 당 1회";
		}

		// 독주
		_bool arrActivatePos4[4] = { 0, 0, 1, 1 };
		_bool arrTargetPos4[4] = { 1, 1, 1, 1 };
		_bool arrAttack4[8] = { 1, 0, 0, 0, 0, 0, 1, 1 };
		_bool arrToEnemy4[6] = { 1, 0, 1, 1, 0, 1 };

		shared_ptr<CSkill> pSkill4 = make_shared<CSkill>
			(L"Solo", L"Jester_Solo", L"Solo_Img", L"Solo_Effect", L"Solo_Target_Effect", L"Hero_jest_solo.wav",
				arrActivatePos4, arrTargetPos4, arrAttack4, arrToEnemy4, DotDamZero, -2.f, 1.f, 0.2f, 0.4f, -1, 0, 0, 1, 1, 5);

		{
			pSkill4->GetDescription()->m_eDescriptionType = EDescriptionType::SKILL3;
			pSkill4->GetDescription()->m_strTitle = L"독주";
			pSkill4->GetDescription()->m_strSubTitle = L"근거리";
			pSkill4->GetDescription()->m_strMainContent = L"피해 보정 : 80%%\n치명타 보정 : 60%%";
			pSkill4->GetDescription()->m_strColorTitle1 = L"이동";
			pSkill4->GetDescription()->m_tColor1 = SKYBLUE;
			pSkill4->GetDescription()->m_strColorContent1 = L"자신 : 맨 앞으로";
			pSkill4->GetDescription()->m_strColorTitle2 = L"특수";
			pSkill4->GetDescription()->m_tColor2 = BROWN;
			pSkill4->GetDescription()->m_strColorContent2 = L"적 전체 회피-10 대단원+75%%";
		}

		// 저며내기
		_bool arrActivatePos5[4] = { 0, 1, 1, 1 };
		_bool arrTargetPos5[4] = { 0, 1, 1, 1 };
		_bool arrAttack5[8] = { 0, 0, 1, 0, 0, 0, 1, 0 };
		_bool arrToEnemy5[6] = { 0, 1, 1, 1, 1, 1 };

		_int  DotDam5[2] = { 3, 3 };

		shared_ptr<CSkill> pSkill5 = make_shared<CSkill>
			(L"SliceOff", L"Jester_SliceOff", L"SliceOff_Img", L"", L"", L"Hero_jest_sliceoff.wav",
				arrActivatePos5, arrTargetPos5, arrAttack5, arrToEnemy5, DotDam5, 0.f,  1.f, 0.7f, 1.f, -1, 0, 0, 0, 1, 0);

		{
			pSkill5->GetDescription()->m_eDescriptionType = EDescriptionType::SKILL3;
			pSkill5->GetDescription()->m_strTitle = L"저며내기";
			pSkill5->GetDescription()->m_strSubTitle = L"근거리";
			pSkill5->GetDescription()->m_strMainContent = L"피해 보정 : -30%%\n치명타 보정 : 0%%";
			pSkill5->GetDescription()->m_strColorTitle1 = L"출혈";
			pSkill5->GetDescription()->m_tColor1 = RED;
			pSkill5->GetDescription()->m_strColorContent1 = L"데미지:3 라운드:3";
			pSkill5->GetDescription()->m_strColorTitle2 = L"강화";
			pSkill5->GetDescription()->m_tColor2 = ORANGE;
			pSkill5->GetDescription()->m_strColorContent2 = L"대단원+30%%";
		}

		// 전투의 발라드
		_bool arrActivatePos6[4] = { 0, 1, 1, 1 };
		_bool arrTargetPos6[4] = { 1, 1, 1, 1 };
		_bool arrAttack6[8] = { 0, 0, 0, 0, 0, 0, 1, 0 };
		_bool arrToEnemy6[6] = { 1, 1, 1, 0, 1, 1 };

		shared_ptr<CSkill> pSkill6 = make_shared<CSkill>
			(L"BattleBallad", L"Jester_BattleBallad", L"BattleBallad_Img", L"BattleBallad_Effect", L"BattleBallad_Target_Effect", L"Hero_jest_battleball.wav",
				arrActivatePos6, arrTargetPos6, arrAttack6, arrToEnemy6, DotDamZero, 0.f, 1.f, 0.7f, 1.f, -1, 0, 1, 1, 0, 0);

		{
			pSkill6->GetDescription()->m_eDescriptionType = EDescriptionType::SKILL2;
			pSkill6->GetDescription()->m_strTitle = L"전투의 발라드";
			pSkill6->GetDescription()->m_strSubTitle = L"원거리";
			pSkill6->GetDescription()->m_strMainContent = L"";
			pSkill6->GetDescription()->m_strColorTitle1 = L"강화";
			pSkill6->GetDescription()->m_tColor1 = SKYBLUE;
			pSkill6->GetDescription()->m_strColorContent1 = L"아군 전체 회피 +10";
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
		m_tCommonStat.iHp = 30;
		m_tCommonStat.iMaxHp = 30;
		m_tCommonStat.iDodge = 30;
		m_tCommonStat.iSpeed = 7;
		m_tCommonStat.iAttackPower = 6;
		m_tCommonStat.iOrder = 0;
	}


	// 테스트용
	{
		m_eAnimState = EAnimState::IDLE;
		//m_vAngle = { 0.f, PI / 2, 0.f };

		m_pTransformCom->SetPosition(m_vPos.x, m_vPos.y, m_vPos.z);
		m_pTransformCom->SetScale(2.f, 3.f, 3.f);

		m_pTransformCom->SetAngle(m_vAngle);
		m_pTransformCom->Rotation(ROT_Y, PI / 2.f);

		m_pTextureCom->SetAnimKey(L"Jester_Idle", 0.04f);
		m_vOriginSize =  m_pTextureCom->GetTextureSize();
	}

	// 영웅 스탯 출력창
	m_pStatUI = make_shared<CHeroStat>(m_pGraphicDev);
	m_pStatUI->SetHero(dynamic_pointer_cast<CJester>(shared_from_this()));
	m_pStatUI->AwakeGameObject();
	m_pStatUI->ReadyGameObject();

	CUIMgr::GetInstance()->AddUIObject(L"UI_HeroStat_Jester", m_pStatUI);

	m_pStatInfo->SettingInit(*(m_pTransformCom->GetPos()),
		m_tCommonStat.iHp, m_tCommonStat.iMaxHp, m_bIsHero);
	m_pStatInfo->SetIsHeroType(4);

	m_bReady = true;

	return S_OK;
}

_int CJester::UpdateGameObject(const _float& fTimeDelta)
{
	Engine::AddRenderGroup(RENDER_ALPHA, shared_from_this());

	_int iExit = __super::UpdateGameObject(fTimeDelta);

	ChangeAnim();

	if ((IsAttacking()))
	{
		m_eCurAnimState = EAnimState::SKILL;

		for (int i = 0; i < 7; i++)
		{
			if (GetAttacking(i))
			{
				m_strAnimKey = m_pVecSkill[i]->GetAnimKey();
				m_strSoundKey = m_pVecSkill[i]->GetSoundKey();

				/*switch (i)
				{
				case 0:
					m_eCurAnimState = EAnimState::SKILL1;
					break;
				case 1:
					m_eCurAnimState = EAnimState::SKILL6;
					break;
				case 2:
					m_eCurAnimState = EAnimState::SKILL3;
					break;
				case 3:
					m_eCurAnimState = EAnimState::SKILL4;
					break;
				case 4:
					m_eCurAnimState = EAnimState::SKILL2;
					break;
				case 5:
					m_eCurAnimState = EAnimState::SKILL5;
					break;
				case 6:
					m_eCurAnimState = EAnimState::SKILL7;
					break;
				}*/
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
	//if (m_tCommonStat.iHp <= 0)
	//{
	//	m_bDeath = true;
	//	m_tCommonStat.iHp = -100;

	//	bStatBarOn = false;
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

void CJester::LateUpdateGameObject()
{
	__super::LateUpdateGameObject();
}

void CJester::RenderGameObject()
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

void CJester::AddComponent()
{

	__super::AddComponent();
}

void CJester::ChangeAnim()
{
	_float fXpos, fYpos;
	// Animation
	if (m_eCurAnimState != m_ePrevAnimState)
	{
		switch (m_eCurAnimState)
		{
		case EAnimState::IDLE:
			m_pTextureCom->SetAnimKey(L"Jester_Idle", 0.04f);
			break;
		case EAnimState::WALK:
			m_pTextureCom->SetAnimKey(L"Jester_Walk", 0.04f);
			break;
		case EAnimState::COMBAT:
			m_pTextureCom->SetAnimKey(L"Jester_Combat", 0.04f);
			break;
		case EAnimState::BESHOT:
			m_pTextureCom->SetAnimKey(L"Jester_Defend", 0.04f);
			break;
		case EAnimState::SKILL:
			m_pTextureCom->SetAnimKey(m_strAnimKey, 0.04f);
			CSoundMgr::GetInstance()->PlaySound(m_strSoundKey.c_str(), CHANNELID::JESTER, 1.f);
			break;
		/*case EAnimState::SKILL1:
			m_pTextureCom->SetAnimKey(L"Jester_DirkStab", 0.04f);
			CSoundMgr::GetInstance()->PlaySound(L"Hero_jest_sliceoff.wav", CHANNELID::JESTER, 1.f);

			break;
		case EAnimState::SKILL2:
			m_pTextureCom->SetAnimKey(L"Jester_Harvest", 0.04f);
			CSoundMgr::GetInstance()->PlaySound(L"Hero_jest_harvest.wav", CHANNELID::JESTER, 1.f);
			break;
		case EAnimState::SKILL3:
			m_pTextureCom->SetAnimKey(L"Jester_Finale", 0.04f);
			CSoundMgr::GetInstance()->PlaySound(L"Hero_jest_finale.wav", CHANNELID::JESTER, 1.f);
			break;
		case EAnimState::SKILL4:
			m_pTextureCom->SetAnimKey(L"Jester_Solo", 0.04f);
			CSoundMgr::GetInstance()->PlaySound(L"Hero_jest_solo.wav", CHANNELID::JESTER, 1.f);
			break;
		case EAnimState::SKILL5:
			m_pTextureCom->SetAnimKey(L"Jester_SliceOff", 0.04f);
			CSoundMgr::GetInstance()->PlaySound(L"Hero_jest_sliceoff.wav", CHANNELID::JESTER, 1.f);
			break;
		case EAnimState::SKILL6:
			m_pTextureCom->SetAnimKey(L"Jester_BattleBallad", 0.04f);
			CSoundMgr::GetInstance()->PlaySound(L"Hero_jest_battleball.wav", CHANNELID::JESTER, 1.f);
			break;*/
		case EAnimState::AFFLICTION:
			m_pTextureCom->SetAnimKey(L"Jester_Affliction", 0.04f);
			break;
		case EAnimState::VIRTUE:
			m_pTextureCom->SetAnimKey(L"Jester_Virtue", 0.04f);
			break;
		case EAnimState::DEATH:
			m_pTextureCom->SetAnimKey(L"Hero_Death", 0.02f);
			break;
		}
		m_ePrevAnimState = m_eCurAnimState;

	}

	_vec2 vcurPos = m_pTextureCom->GetTextureSize();

	if (m_eCurAnimState == EAnimState::IDLE || m_eCurAnimState == EAnimState::WALK || m_eCurAnimState == EAnimState::COMBAT) {
		fXpos = (vcurPos.x / m_vOriginSize.x)* 1.4f;
		fYpos = (vcurPos.y / m_vOriginSize.y);
	}
	else {
		fXpos = (vcurPos.x / m_vOriginSize.x) / 1.6f;
		fYpos = (vcurPos.y / m_vOriginSize.y) / 1.8f;
	}

	m_pTransformCom->SetScale(2.f * fXpos, 3.f * fYpos, 2.f * fXpos);

}