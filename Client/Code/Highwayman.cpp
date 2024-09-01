#include "pch.h"
#include "Highwayman.h"

#include "HeroStat.h"
#include"Export_Utility.h"
#include"StatView.h"
#include "UIMgr.h"

#include"SoundMgr.h"

CHighwayman::CHighwayman(LPDIRECT3DDEVICE9 pGraphicDev) : CHero(pGraphicDev)
{
}

CHighwayman::CHighwayman(const CHighwayman& rhs) : CHero(rhs)
{
}

CHighwayman::~CHighwayman()
{
}

HRESULT CHighwayman::ReadyGameObject()
{
	if (m_bReady)
		return S_OK;

	__super::ReadyGameObject();

	m_bIsHero = true;
	m_strName = L"Highwayman";

	m_eHeroType = EHeroType::HIGHWAYMAN;

	// 스킬 세팅
	{
		_int  DotDamZero[2] = { 0, 0 };

		// 사약한 칼질
		_bool arrActivatePos1[4] = { 1, 1, 1, 0 };
		_bool arrTargetPos1[4] = { 1, 1, 0, 0 };
		_bool arrAttack1[8] = { 1, 0, 0, 0, 0, 0, 0, 0 };
		_bool arrToEnemy1[6] = { 1, 1, 1, 1, 1, 1 };

		/*
		영웅 스킬 용
		(스킬 이름, 스킬 애니메이션, 스킬 이미지, 이펙트키, 활성 위치, 타겟 위치, 공격종류(단순 공격, 중독, 출혈, 기절, 이동, 힐), 
		각 공격 타겟이 적인지 아닌지, 도트뎀[데미지][라운드], 스턴 확률, 공격력 계수, 치명타 확률, 라운드 당 사용 가능 횟수, 이동량, 힐량, 타겟 대상을 모두 공격하는지)
		*/

		shared_ptr<CSkill> pSkill1 = make_shared<CSkill>
			(L"WickedSlice", L"Highwayman_WickedSlice", L"WickedSlice_Img", L"", L"", L"",
				arrActivatePos1, arrTargetPos1, arrAttack1, arrToEnemy1, DotDamZero, -3.f, 1.f, 1.15f, 1.5f, -1, 0, 0, 0, 1, 0);

		{
			pSkill1->GetDescription()->m_eDescriptionType = EDescriptionType::SKILL1;
			pSkill1->GetDescription()->m_strTitle = L"사악한 칼질";
			pSkill1->GetDescription()->m_strSubTitle = L"근거리";
			pSkill1->GetDescription()->m_strMainContent = L"피해 보정 : +15%%\n치명타 보정 : +50%%";
		}
		
		// 권총 사격
		_bool arrActivatePos2[4] = { 0, 1, 1, 1 };
		_bool arrTargetPos2[4] = { 0, 1, 1, 1 };
		_bool arrAttack2[8] = { 1, 0, 0, 0, 0, 0, 0, 0 };
		_bool arrToEnemy2[6] = { 1, 1, 1, 1, 1, 1 };

		shared_ptr<CSkill> pSkill2 = make_shared<CSkill>
			(L"PistolShot", L"Highwayman_PistolShot", L"PistolShot_Img", L"", L"PistolShot_Target_Effect", L"",
				arrActivatePos2, arrTargetPos2, arrAttack2, arrToEnemy2, DotDamZero, 0.f, 1.f, 0.85f, 1.2f, -1, 0, 0, 0, 0, 0);

		{
			pSkill2->GetDescription()->m_eDescriptionType = EDescriptionType::SKILL1;
			pSkill2->GetDescription()->m_strTitle = L"권총 사격";
			pSkill2->GetDescription()->m_strSubTitle = L"원거리";
			pSkill2->GetDescription()->m_strMainContent = L"피해 보정 : -15%%\n치명타 보정 : +20%%";
		}

		// 영거리 사격
		_bool arrActivatePos3[4] = { 1, 1, 0, 0 };
		_bool arrTargetPos3[4] = { 1, 1, 0, 0 };
		_bool arrAttack3[8] = { 1, 0, 0, 0, 0, 0, 0, 0 };
		_bool arrToEnemy3[6] = { 1, 1, 1, 1, 1, 1 };

		shared_ptr<CSkill> pSkill3 = make_shared<CSkill>
			(L"PointBlankShot", L"Highwayman_PointBlankShot", L"PointBlankShot_Img", L"PointBlankShot_Effect", L"PointBlankShot_Target_Effect", L"",
				arrActivatePos3, arrTargetPos3, arrAttack3, arrToEnemy3, DotDamZero, 0.f, 1.f, 1.5f, 1.9f, -1, 1, 0, 0, 0, -1);

		{
			pSkill3->GetDescription()->m_eDescriptionType = EDescriptionType::SKILL3;
			pSkill3->GetDescription()->m_strTitle = L"영거리 사격";
			pSkill3->GetDescription()->m_strSubTitle = L"원거리";
			pSkill3->GetDescription()->m_strMainContent = L"피해 보정 : +50%%\n치명타 보정 : +90%%";
			pSkill3->GetDescription()->m_strColorTitle1 = L"이동";
			pSkill3->GetDescription()->m_tColor1 = SKYBLUE;
			pSkill3->GetDescription()->m_strColorContent1 = L"자신 : 1열 뒤로";
			pSkill3->GetDescription()->m_strColorTitle2 = L"이동";
			pSkill3->GetDescription()->m_tColor2 = SKYBLUE;
			pSkill3->GetDescription()->m_strColorContent2 = L"상대 : 1열 밀치기";
		}

		// 결투가의 진격
		_bool arrActivatePos4[4] = { 0, 1, 1, 1 };
		_bool arrTargetPos4[4] = { 1, 1, 1, 0 };
		_bool arrAttack4[8] = { 1, 0, 0, 0, 0, 0, 0, 0 };
		_bool arrToEnemy4[6] = { 1, 1, 1, 1, 1, 1 };

		shared_ptr<CSkill> pSkill4 = make_shared<CSkill>
			(L"DuelistsAdvance", L"Highwayman_DuelistsAdvance", L"DuelistsAdvance_Img", L"", L"", L"",
				arrActivatePos4, arrTargetPos4, arrAttack4, arrToEnemy4, DotDamZero, -2.5f, 1.f, 0.8f, 1.1f, -1, 0, 0, 0, 1, 1);

		{
			pSkill4->GetDescription()->m_eDescriptionType = EDescriptionType::SKILL3;
			pSkill4->GetDescription()->m_strTitle = L"결투가의 진격";
			pSkill4->GetDescription()->m_strSubTitle = L"근거리";
			pSkill4->GetDescription()->m_strMainContent = L"피해 보정 : -20%%\n치명타 보정 : +10%%";
			pSkill4->GetDescription()->m_strColorTitle1 = L"이동";
			pSkill4->GetDescription()->m_tColor1 = SKYBLUE;
			pSkill4->GetDescription()->m_strColorContent1 = L"자신 : 1열 앞으로";
		}

		shared_ptr<CSkill> pSkill5 = make_shared<CSkill>
			(L"GrapeshotBlast", L"Highwayman_GrapeshotBlast", L"GrapeshotBlast_Img", L"", L"", L"",
				arrActivatePos4, arrTargetPos4, arrAttack4, arrToEnemy4, DotDamZero, 0.f, 1.f, 0.8f, 1.1f, -1, 0, 0, 0, 1, 1);

		{
			pSkill5->GetDescription()->m_eDescriptionType = EDescriptionType::SKILL2;
			pSkill5->GetDescription()->m_strTitle = L"포도탄 사격";
			pSkill5->GetDescription()->m_strSubTitle = L"원거리";
			pSkill5->GetDescription()->m_strMainContent = L"피해 보정 : -50%%\n치명타 보정 : -10%%";
			pSkill5->GetDescription()->m_strColorTitle1 = L"이동";
			pSkill5->GetDescription()->m_tColor1 = SKYBLUE;
			pSkill5->GetDescription()->m_strColorContent1 = L"상대 : 1열 뒤로";
		}

		shared_ptr<CSkill> pSkill6 = make_shared<CSkill>
			(L"TrackingShot", L"Highwayman_TrackingShot", L"TrackingShot_Img", L"", L"", L"",
				arrActivatePos4, arrTargetPos4, arrAttack4, arrToEnemy4, DotDamZero, 0.f, 1.f, 0.8f, 1.1f, -1, 0, 0, 0, 1, 1);

		{
			pSkill6->GetDescription()->m_eDescriptionType = EDescriptionType::SKILL2;
			pSkill6->GetDescription()->m_strTitle = L"추적탄";
			pSkill6->GetDescription()->m_strSubTitle = L"원거리";
			pSkill6->GetDescription()->m_strMainContent = L"피해 보정 : -80%%\n치명타 보정 : 0%%";
			pSkill6->GetDescription()->m_strColorTitle1 = L"특성";
			pSkill6->GetDescription()->m_tColor1 = BROWN;
			pSkill6->GetDescription()->m_strColorContent1 = L"전투 당 1회 사용 가능";
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
		m_tCommonStat.iHp = 35;
		m_tCommonStat.iMaxHp = 35;
		m_tCommonStat.iDodge = 20;
		m_tCommonStat.iSpeed = 6;
		m_tCommonStat.iAttackPower = 8;
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

		m_pTextureCom->SetAnimKey(L"Highwayman_Idle", 0.04f);
		m_vOriginSize = m_pTextureCom->GetTextureSize();

	}

	// 영웅 스탯 출력창
	m_pStatUI = make_shared<CHeroStat>(m_pGraphicDev);
	m_pStatUI->SetHero(dynamic_pointer_cast<CHighwayman>(shared_from_this()));
	m_pStatUI->AwakeGameObject();
	m_pStatUI->ReadyGameObject();

	CUIMgr::GetInstance()->AddUIObject(L"UI_HeroStat_Highwayman", m_pStatUI);

	m_pStatInfo->SettingInit(*(m_pTransformCom->GetPos()),
		m_tCommonStat.iHp, m_tCommonStat.iMaxHp, m_bIsHero);
	m_pStatInfo->SetIsHeroType(2);

	m_bReady = true;

	return S_OK;
}

_int CHighwayman::UpdateGameObject(const _float& fTimeDelta)
{
	Engine::AddRenderGroup(RENDER_ALPHA, shared_from_this());

	_int iExit = __super::UpdateGameObject(fTimeDelta);

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

void CHighwayman::LateUpdateGameObject()
{
	__super::LateUpdateGameObject();
}

void CHighwayman::RenderGameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->GetWorld());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);


	m_pTextureCom->SetAnimTexture();
	m_pBufCom->RenderBuffer();
	m_pEffectBufCom->RenderBuffer();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	__super::RenderGameObject();
}

void CHighwayman::AddComponent()
{
	__super::AddComponent();
}

void CHighwayman::ChangeAnim()
{
	_float fXpos, fYpos;
	// Animation
	if (m_eCurAnimState != m_ePrevAnimState)
	{
		switch (m_eCurAnimState)
		{
		case EAnimState::IDLE:
			m_pTextureCom->SetAnimKey(L"Highwayman_Idle", 0.04f);
			//m_pTransformCom->SetScale(2.f, 3.f, 1.f);
			break;
		case EAnimState::WALK:
			m_pTextureCom->SetAnimKey(L"Highwayman_Walk", 0.04f);
			//m_pTransformCom->SetScale(2.f, 3.f, 1.f);
			break;
		case EAnimState::COMBAT:
			m_pTextureCom->SetAnimKey(L"Highwayman_Combat", 0.04f);
			//m_pTransformCom->SetScale(2.f, 3.f, 1.f);
			break;
		case EAnimState::BESHOT:
			m_pTextureCom->SetAnimKey(L"Highwayman_Defend", 0.04f);
			//m_pTransformCom->SetScale(2.f, 3.f, 1.f);
			break;
		case EAnimState::SKILL1:
			m_pTextureCom->SetAnimKey(L"Highwayman_WickedSlice", 0.04f);
			CSoundMgr::GetInstance()->PlaySound(L"Hero_hwy_wickedslice.wav", CHANNELID::HIGHWAYMAN, 1.f);

			//m_pTransformCom->SetScale(2.f, 3.f, 1.f);
			break;
		case EAnimState::SKILL2:
			m_pTextureCom->SetAnimKey(L"Highwayman_PistolShot", 0.04f);
			CSoundMgr::GetInstance()->PlaySound(L"Hero_hwy_pistolshot.wav", CHANNELID::HIGHWAYMAN, 1.f);

			//m_pTransformCom->SetScale(2.f, 3.f, 1.f);
			break;
		case EAnimState::SKILL3:
			m_pTextureCom->SetAnimKey(L"Highwayman_PointBlankShot", 0.04f);
			CSoundMgr::GetInstance()->PlaySound(L"Hero_hwy_pointblank.wav", CHANNELID::HIGHWAYMAN, 1.f);

			//m_pTransformCom->SetScale(2.f, 3.f, 1.f);
			break;
		case EAnimState::SKILL4:
			m_pTextureCom->SetAnimKey(L"Highwayman_DuelistsAdvance", 0.04f);
			CSoundMgr::GetInstance()->PlaySound(L"Hero_hwy_duelistadvance.wav", CHANNELID::HIGHWAYMAN, 1.f);

			break;
		case EAnimState::AFFLICTION:
			m_pTextureCom->SetAnimKey(L"Highwayman_Affliction", 0.04f);

			break;
		case EAnimState::VIRTUE:
			m_pTextureCom->SetAnimKey(L"Highwayman_Virtue", 0.04f);
			break;
		case EAnimState::DEATH:
			m_pTextureCom->SetAnimKey(L"Hero_Death", 0.02f);
			break;
		}

		m_ePrevAnimState = m_eCurAnimState;

		_vec2 vcurPos = m_pTextureCom->GetTextureSize(); 
		
		if (m_eCurAnimState == EAnimState::IDLE || m_eCurAnimState == EAnimState::WALK || m_eCurAnimState == EAnimState::COMBAT) {
			fXpos = (vcurPos.x / m_vOriginSize.x);
			fYpos = (vcurPos.y / m_vOriginSize.y);
		}
		else {
			fXpos = (vcurPos.x / m_vOriginSize.x)/1.9f;
			fYpos = (vcurPos.y / m_vOriginSize.y)/1.9f;
		}

		m_pTransformCom->SetScale(2.f* fXpos, 3.f * fYpos, 2.f * fXpos);

	}
}