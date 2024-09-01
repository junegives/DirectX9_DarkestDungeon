#include "pch.h"
#include "BattleSystem.h"
#include "Creature.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include"BattleHeroUI.h"
#include"Hero.h"
#include "Effect.h"

#include"CameraMgr.h"
#include "PickingMgr.h"
#include "EffectMgr.h"
#include "UIMgr.h"

#include "SceneEvent.h"

CBattleSystem::CBattleSystem()
{
}

CBattleSystem::~CBattleSystem()
{
}

_bool CBattleSystem::Update(const _float& fTimeDelta)
{

	//근접 공격을 제외한 공격 시 일정시간 이후 원상복구로 돌려놓는 부분 
	if (m_bChangeSkillAnim) {

		m_fTurnResetTime += fTimeDelta;

		if (m_fTurnResetTime > 1.5f) {
			m_fTurnResetTime = 0.f;
			m_bChangeSkillAnim = false;
			CCameraMgr::GetInstance()->MovingStraight(ECameraMode::ZOOMOUT, m_vCenterPos);

			if (m_fAngle != 0.f) {
				CCameraMgr::GetInstance()->CameraRotation(ECameraMode::BATTLE, m_fAngle, 0.4f);
				m_fAngle = 0.f;
			}

		}

		if (m_fTurnResetTime > 0.5f && !m_bShakingDone) {
			CCameraMgr::GetInstance()->AddEffectInfo(EEffectState::SHAKING, 0.2f, 0.15f);
			m_bShakingDone = true;
		}

	}


	for (auto& iter : m_vecPickingObject)
	{
		CPickingMgr::GetInstance()->AddList(iter);
	}

	// 자동전투 여부
	AutoBattleKeyInput();

	// 자리 스위칭 이동중
	if (m_bSwitching)
	{
		m_fSwitchingTime -= fTimeDelta;
		for (auto& iter : m_vHeroes)
		{
			if (dynamic_pointer_cast<CCreature>(iter)->GetMoving())
			{

				dynamic_pointer_cast<CCreature>(iter)->MovePos(dynamic_pointer_cast<CCreature>(iter)->GetTargetPos(),
					fTimeDelta, dynamic_pointer_cast<CCreature>(iter)->GetMovingSpeed());
			}
		}

		if (m_fSwitchingTime < 0.f)
		{
			m_bSwitching = false;
			m_fSwitchingTime = SWITCHINGINTERVEL;
			for (auto& iter : m_vHeroes)
			{
				dynamic_pointer_cast<CCreature>(iter)->SetAttackMoving(false);
			}
		}
	}

	// 공격하며 움직이는 스킬 이동중
	if (m_bAttackSkillMoving)
	{
		m_fAttackSkillMovingTime -= fTimeDelta;
		for (auto& iter : m_vHeroes)
		{
			if (dynamic_pointer_cast<CCreature>(iter)->GetAttackMoving())
			{
				//근거리 공격
				_vec3 vTargetPos = dynamic_pointer_cast<CCreature>(iter)->GetTargetPos();
				CCameraMgr::GetInstance()->MovingStraight(ECameraMode::ZOOMIN, { vTargetPos.x + 2.f , vTargetPos.y , vTargetPos.z - 8.f });
				dynamic_pointer_cast<CCreature>(iter)->MovePos(vTargetPos, fTimeDelta, dynamic_pointer_cast<CCreature>(iter)->GetMovingSpeed());
			}
		}
		for (auto& iter : m_vMonsters)
		{
			if (dynamic_pointer_cast<CCreature>(iter)->GetAttackMoving())
			{
				//근거리 공격
				_vec3 vTargetPos = dynamic_pointer_cast<CCreature>(iter)->GetTargetPos();
				CCameraMgr::GetInstance()->MovingStraight(ECameraMode::ZOOMIN, { vTargetPos.x - 2.f , vTargetPos.y , vTargetPos.z - 8.f });
				dynamic_pointer_cast<CCreature>(iter)->MovePos(dynamic_pointer_cast<CCreature>(iter)->GetTargetPos(),
					fTimeDelta, dynamic_pointer_cast<CCreature>(iter)->GetMovingSpeed());

			}
		}

		if (m_fAttackSkillMovingTime < 0.f)
		{
			m_bAttackSkillMoving = false;
			m_bWhileAttack = true;
			m_fAttackSkillMovingTime = ATTACKSKILLMOVINGINTERVEL;

			// 원래 위치로 돌아가는 속도 잡아주기
			for (auto& iter : m_vHeroes)
			{
				if (dynamic_pointer_cast<CCreature>(iter)->GetAttackMoving())
				{

					iter->SetPos(dynamic_pointer_cast<CCreature>(iter)->GetTargetPos());
					dynamic_pointer_cast<CCreature>(iter)->SetMovingSpeed(
						dynamic_pointer_cast<CCreature>(iter)->MovingSpeed
						(dynamic_pointer_cast<CCreature>(iter)->GetTargetPos2(), MOVINGBACKINTERVEL));
				}
			}
			for (auto& iter : m_vMonsters)
			{
				if (dynamic_pointer_cast<CCreature>(iter)->GetAttackMoving())
				{
					iter->SetPos(dynamic_pointer_cast<CCreature>(iter)->GetTargetPos());
					dynamic_pointer_cast<CCreature>(iter)->SetMovingSpeed(
						dynamic_pointer_cast<CCreature>(iter)->MovingSpeed
						(dynamic_pointer_cast<CCreature>(iter)->GetTargetPos2(), MOVINGBACKINTERVEL));
				}
			}
		}
	}

	// 멈춰서 공격하는 시간
	if (m_bWhileAttack)
	{
		m_fWhileAttackingTime -= fTimeDelta;

		//카메라 무빙 : 멈춰서 공격하는 동안 옆으로 이동 
		if (m_bHero && m_bCamEffectCheck && m_fWhileAttackingTime < 1.5f) {

			if (!m_bRotationCheck) {
				CCameraMgr::GetInstance()->CameraRotation(ECameraMode::BATTLE, -20.f, m_fWhileAttackingTime);
				CCameraMgr::GetInstance()->MovingRightVec(true, 1.f);
				m_bRotationCheck = true;
			}

		}
		else if (!m_bHero && m_bCamEffectCheck && m_fWhileAttackingTime < 1.5f) {

			if (!m_bRotationCheck) {
				CCameraMgr::GetInstance()->CameraRotation(ECameraMode::BATTLE, 20.f, m_fWhileAttackingTime);
				CCameraMgr::GetInstance()->MovingRightVec(true, -1.f);
				m_bRotationCheck = true;
			}
		}

		//줌인이 끝나는 시점에 셰이킹 
		if (!m_bCamEffectCheck && m_fWhileAttackingTime < 1.5f) {
			CCameraMgr::GetInstance()->AddEffectInfo(EEffectState::SHAKING, 0.2f, 0.15f);
			m_bCamEffectCheck = true;
		}

		if (m_fWhileAttackingTime < 0.f)
		{
			CCameraMgr::GetInstance()->MovingRightVec(false, 1.f);
			m_bWhileAttack = false;
			m_bBackMoving = true;
			m_fWhileAttackingTime = WHILEATTACKINTERVEL;

			//원상태로 돌아오기
			CCameraMgr::GetInstance()->MovingStraight(ECameraMode::ZOOMOUT, m_vCenterPos);

			if (m_bHero)
				CCameraMgr::GetInstance()->CameraRotation(ECameraMode::BATTLE, 20.f, 0.7f);
			else
				CCameraMgr::GetInstance()->CameraRotation(ECameraMode::BATTLE, -20.f, 0.7f);

			m_bCamEffectCheck = false;
			m_bRotationCheck = false;

		}
	}

	// 공격하며 움직이는 스킬 이동 이후에 돌아가기
	if (m_bBackMoving)
	{
		m_fBackMovingTime -= fTimeDelta;
		for (auto& iter : m_vHeroes)
		{
			if (dynamic_pointer_cast<CCreature>(iter)->GetAttackMoving())
			{
				dynamic_pointer_cast<CCreature>(iter)->MovePos(dynamic_pointer_cast<CCreature>(iter)->GetTargetPos2(),
					fTimeDelta, dynamic_pointer_cast<CCreature>(iter)->GetMovingSpeed());
			}
		}
		for (auto& iter : m_vMonsters)
		{
			if (dynamic_pointer_cast<CCreature>(iter)->GetAttackMoving())
			{
				dynamic_pointer_cast<CCreature>(iter)->MovePos(dynamic_pointer_cast<CCreature>(iter)->GetTargetPos2(),
					fTimeDelta, dynamic_pointer_cast<CCreature>(iter)->GetMovingSpeed());
			}
		}

		if (m_fBackMovingTime < 0.f)
		{
			m_bBackMoving = false;
			m_fBackMovingTime = MOVINGBACKINTERVEL;
			for (auto& iter : m_vHeroes)
			{
				dynamic_pointer_cast<CCreature>(iter)->SetAttackMoving(false);
			}
			for (auto& iter : m_vMonsters)
			{
				dynamic_pointer_cast<CCreature>(iter)->SetAttackMoving(false);
			}
		}
	}

	// 죽은 애들 체크
	if (m_bDeadCheck && !m_bAttackSkillMoving && !m_bWhileAttack)
	{
		DeadCheck();
		m_bDeadCheck = false;
	}

	// 죽은애 있으면 위치 갱신
	if (m_bDeathMoving && !m_bAttackSkillMoving && !m_bWhileAttack)
	{
		m_fDeathMovingTime -= fTimeDelta;
		for (auto& iter : m_vHeroes)
		{
			if (dynamic_pointer_cast<CCreature>(iter)->GetMoving())
			{
				dynamic_pointer_cast<CCreature>(iter)->MovePos(dynamic_pointer_cast<CCreature>(iter)->GetTargetPos(),
					fTimeDelta, dynamic_pointer_cast<CCreature>(iter)->GetMovingSpeed());
			}
		}
		for (auto& iter : m_vMonsters)
		{
			if (dynamic_pointer_cast<CCreature>(iter)->GetMoving())
			{
				dynamic_pointer_cast<CCreature>(iter)->MovePos(dynamic_pointer_cast<CCreature>(iter)->GetTargetPos(),
					fTimeDelta, dynamic_pointer_cast<CCreature>(iter)->GetMovingSpeed());
			}
		}

		if (m_fDeathMovingTime < 0.f)
		{
			m_bDeathMoving = false;
			m_fDeathMovingTime = DEATHMOVINGINTERVEL;
			for (auto& iter : m_vHeroes)
			{
				dynamic_pointer_cast<CCreature>(iter)->SetMoving(false);
			}
			for (auto& iter : m_vMonsters)
			{
				dynamic_pointer_cast<CCreature>(iter)->SetMoving(false);
			}
		}
	}

	// 움직이는 스킬 위치 갱신
	if (m_bSkillMoving && !m_bAttackSkillMoving && !m_bWhileAttack)
	{
		m_fSkillMovingTime -= fTimeDelta;
		for (auto& iter : m_vHeroes)
		{
			if (dynamic_pointer_cast<CCreature>(iter)->GetMoving())
			{
				dynamic_pointer_cast<CCreature>(iter)->MovePos(dynamic_pointer_cast<CCreature>(iter)->GetTargetPos(),
					fTimeDelta, dynamic_pointer_cast<CCreature>(iter)->GetMovingSpeed());
			}
		}
		for (auto& iter : m_vMonsters)
		{
			if (dynamic_pointer_cast<CCreature>(iter)->GetMoving())
			{
				dynamic_pointer_cast<CCreature>(iter)->MovePos(dynamic_pointer_cast<CCreature>(iter)->GetTargetPos(),
					fTimeDelta, dynamic_pointer_cast<CCreature>(iter)->GetMovingSpeed());
			}
		}

		if (m_fSkillMovingTime < 0.f)
		{
			m_bSkillMoving = false;
			m_fSkillMovingTime = SKILLMOVINGINTERVEL;
			for (auto& iter : m_vHeroes)
			{
				dynamic_pointer_cast<CCreature>(iter)->SetMoving(false);
			}
			for (auto& iter : m_vMonsters)
			{
				dynamic_pointer_cast<CCreature>(iter)->SetMoving(false);
			}
		}
	}

	// 스킬 쓴애가 움직이는 위치 갱신
	if (m_bSkillMove && !m_bAttackSkillMoving && !m_bWhileAttack)
	{
		m_fSkillMoveTime -= fTimeDelta;
		for (auto& iter : m_vHeroes)
		{
			if (dynamic_pointer_cast<CCreature>(iter)->GetMoving())
			{
				dynamic_pointer_cast<CCreature>(iter)->MovePos(dynamic_pointer_cast<CCreature>(iter)->GetTargetPos(),
					fTimeDelta, dynamic_pointer_cast<CCreature>(iter)->GetMovingSpeed());

			}
		}
		for (auto& iter : m_vMonsters)
		{
			if (dynamic_pointer_cast<CCreature>(iter)->GetMoving())
			{

				dynamic_pointer_cast<CCreature>(iter)->MovePos(dynamic_pointer_cast<CCreature>(iter)->GetTargetPos(),
					fTimeDelta, dynamic_pointer_cast<CCreature>(iter)->GetMovingSpeed());
			}
		}

		if (m_fSkillMoveTime < 0.f)
		{
			m_bSkillMove = false;
			m_fSkillMoveTime = SKILLMOVEINTERVEL;
			for (auto& iter : m_vHeroes)
			{
				dynamic_pointer_cast<CCreature>(iter)->SetMoving(false);
			}
			for (auto& iter : m_vMonsters)
			{
				dynamic_pointer_cast<CCreature>(iter)->SetMoving(false);
			}
		}
	}

	if (m_pCurrentCreature == nullptr) StartTurn();

	// 상태에 따른 턴 사이 간격 설정
	if (!m_bNext && !m_bCounting && !m_bWhileStressEvent && !m_bWhileStressOutput)
	{
		if (!dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetIsCorpse() &&
			!dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetIsDeath())
		{
			m_fBattleTime = BATTLEINTERVEL;
		}
		else if (dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetIsCorpse())
		{
			m_fBattleTime = CORPSEINTERVEL;
		}
		else
		{
			m_fBattleTime = 0.f;
		}
		m_bCounting = true;
	}

	if (!m_bNext && m_bCounting && !m_bWhileStressEvent && !m_bWhileStressOutput)
	{
		m_fBattleTime -= fTimeDelta;
		if(dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetPrevStun())
		{
			m_fBattleTime = 1.f;
			dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->SetPrevStun(false);
		}

		if (m_fBattleTime < 0.f)
		{
			// 배틀 ui 끄기, 턴 ui 끄기
			dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->OffTurnCursor();
			if (m_bHero)
			{
				if (m_pHeroUI)
					m_pHeroUI->SetVisible(false);
			}

			m_pCurrentCreature = NextCreature();
			m_bNext = true;
			m_bCounting = false;

			// 각 객체의 자신의 위치 전달
			for (int i = 0; i < size(m_vHeroes); i++)
			{
				dynamic_pointer_cast<CCreature>(m_vHeroes[i])->SetPosition(i + 1);
			}
			for (int i = 0; i < size(m_vMonsters); i++)
			{
				dynamic_pointer_cast<CCreature>(m_vMonsters[i])->SetPosition(i + 1);
			}

			// 스트레스 이벤트
			for (int i = 0; i < size(m_vHeroes); i++)
			{
				if (!dynamic_pointer_cast<CCreature>(m_vHeroes[i])->GetIsDeath())
				{
					if (!dynamic_pointer_cast<CHero>(m_vHeroes[i])->GetStressEvent())
					{
						// 붕괴 발생한 영웅
						if (dynamic_pointer_cast<CHero>(m_vHeroes[i])->IsAffliction())
						{
							dynamic_pointer_cast<CHero>(m_vHeroes[i])->SetStressEvent(true);
							dynamic_pointer_cast<CHero>(m_vHeroes[i])->OnAffliction();
							m_bNext = false;
							m_bWhileStressEvent = true;
						}
						// 기상 발생한 영웅
						else if (dynamic_pointer_cast<CHero>(m_vHeroes[i])->IsVirtue())
						{
							dynamic_pointer_cast<CHero>(m_vHeroes[i])->SetStressEvent(true);
							dynamic_pointer_cast<CHero>(m_vHeroes[i])->OnVirtue();
							m_bNext = false;
							m_bWhileStressEvent = true;
						}
					}
				}
			}

			// 스트레스 영웅 이벤트 발생
			if (m_bHero && !m_bWhileStressEvent)
			{
				for (int i = 0; i < size(m_vHeroes); i++)
				{
					if (!dynamic_pointer_cast<CCreature>(m_vHeroes[i])->GetIsDeath())
					{
						// selfish 영웅 있는지
						if (dynamic_pointer_cast<CHero>(m_vHeroes[i])->GetAffliction() == EAffliction::SELFISH)
						{
							if ((rand() % 2) == 1)
							{
								m_bNext = false;
								m_bWhileStressOutput = true;
								iStressOutputType[0] = 1;
							}
						}
					}
				}
			}

			if (m_bHero && !m_bWhileStressEvent)
			{
				for (int i = 0; i < size(m_vHeroes); i++)
				{
					if (!dynamic_pointer_cast<CCreature>(m_vHeroes[i])->GetIsDeath())
					{
						// irrational 영웅 있는지
						if (dynamic_pointer_cast<CHero>(m_vHeroes[i])->GetAffliction() == EAffliction::IRRATIONAL)
						{
							if (m_pCurrentCreature == m_vHeroes[i])
							{
								if ((rand() % 2) == 1)
								{
									m_bNext = false;
									m_bWhileStressOutput = true;
									iStressOutputType[1] = 1;
								}
							}
						}
					}
				}
			}

			if (m_bHero && !m_bWhileStressEvent)
			{
				for (int i = 0; i < size(m_vHeroes); i++)
				{
					if (!dynamic_pointer_cast<CCreature>(m_vHeroes[i])->GetIsDeath())
					{
						// COURAGEOUS 영웅 있는지
						if (dynamic_pointer_cast<CHero>(m_vHeroes[i])->GetVirtue() == EVirtue::COURAGEOUS)
						{
							if (m_pCurrentCreature == m_vHeroes[i])
							{
								if ((rand() % 2) == 1)
								{
									m_bNext = false;
									m_bWhileStressOutput = true;
									iStressOutputType[2] = 1;
								}
							}
						}
					}
				}
			}

			if (m_bHero && !m_bWhileStressEvent)
			{
				for (int i = 0; i < size(m_vHeroes); i++)
				{
					if (!dynamic_pointer_cast<CCreature>(m_vHeroes[i])->GetIsDeath())
					{
						// VIGOROUS 영웅 있는지
						if (dynamic_pointer_cast<CHero>(m_vHeroes[i])->GetVirtue() == EVirtue::VIGOROUS)
						{
							if (m_pCurrentCreature == m_vHeroes[i])
							{
								if ((rand() % 2) == 1)
								{
									m_bNext = false;
									m_bWhileStressOutput = true;
									iStressOutputType[3] = 1;
								}
							}
						}
					}
				}
			}
		}
	}

	// 스트레스 이벤트 발생중인 시간
	if (m_bWhileStressEvent)
	{
		printf("스트레스 이벤트 발생 시간\m_fStressEventTime : %f\n", m_fStressEventTime);

		m_fStressEventTime -= fTimeDelta;

		if (m_fStressEventTime < 0.f)
		{
			for (int i = 0; i < size(m_vHeroes); i++)
			{
				dynamic_pointer_cast<CCreature>(m_vHeroes[i])->OffVirtue();
				dynamic_pointer_cast<CCreature>(m_vHeroes[i])->OffAffliction();
			}

			m_fStressEventTime = STRESSEVENTINTERVEL;
			m_bWhileStressEvent = false;
			m_bNext = true;
		}
	}

	// 스트레스 영웅 행동 발생 시간
	if (m_bWhileStressOutput)
	{
		printf("스트레스 영웅 행동 발생 시간\m_fStressOutputTime : %f\n", m_fStressOutputTime);

		if (!m_bStressOutputEffectRender)
		{
			m_bStressOutputEffectRender = true;

			if (iStressOutputType[0])
			{
				dynamic_pointer_cast<CHero>(m_pCurrentCreature)->IncreaseStress(10, true);
			}
			if (iStressOutputType[1])
			{
				dynamic_pointer_cast<CHero>(m_pCurrentCreature)->DecreaseHP(5, true);
			}
			if (iStressOutputType[2])
			{
				for (int i = 0; i < size(m_vHeroes); i++)
				{
					dynamic_pointer_cast<CHero>(m_vHeroes[i])->DecreaseStress(10, true);
				}
			}
			if (iStressOutputType[3])
			{
				dynamic_pointer_cast<CHero>(m_pCurrentCreature)->IncreaseHP(10, true);
			}

			for (int i = 0; i < 4; i++)
			{
				iStressOutputType[i] = 0;
			}
		}

		m_fStressOutputTime -= fTimeDelta;


		if (m_fStressOutputTime < 0.f)
		{
			m_fStressOutputTime = STRESSOUTPUTINTERVEL;
			m_bWhileStressOutput = false;
			m_bStressOutputEffectRender = false;
			m_bNext = true;
		}
	}

	// 종료조건
	if (m_bEndBattleStart && m_bNext)
	{
		m_bNext = false;

		// 보스전이였을 경우
		if (m_iBattleType == 1)
		{
			dynamic_pointer_cast<CCreature>(m_vMonsters[2])->SetMoving(true);
			dynamic_pointer_cast<CCreature>(m_vMonsters[2])->SetTargetPos(_vec3(300.f, 7.5f, 230.f));
			dynamic_pointer_cast<CCreature>(m_vMonsters[2])->SetMovingSpeed(
			dynamic_pointer_cast<CCreature>(m_vMonsters[2])->MovingSpeed(_vec3(300.f, 7.5f, 230.f), 14.f));
		}
	}
	// 일반전투에서 종료
	if (m_bEndBattleStart && !m_bNext && (m_iBattleType == 0))
	{
		//1회만 실행될 수 있도록 bool State로 관리
		if (!m_bBattleEndEffectOn) {
			//승리BGM 재생
			CSoundMgr::GetInstance()->StopSound(CHANNELID::BGM);
			CSoundMgr::GetInstance()->StopSound(CHANNELID::MONSTER);
			CSoundMgr::GetInstance()->PlaySound(L"Combat_Victory.wav", CHANNELID::MONSTER, 1.f);

			CUIMgr::GetInstance()->TextBoardOn(L"승리!", { 0.f, 150.f, 0.3f }, { 150.f, 70.f, 1.f }, STRESSEVENTINTERVEL - 1.f);
			m_bBattleEndEffectOn = true;
		}


		m_fEndTime1 -= fTimeDelta;
		if (m_fEndTime1 < 0.f)
		{
			m_fEndTime1 = 8.f;
			m_bEndBattleStart = false;
			m_bNext = true;

			return true;
		}
	}
	// 보스전에서 종료
	if (m_bEndBattleStart && !m_bNext && (m_iBattleType == 1))
	{
		//1회만 실행될 수 있도록 bool State로 관리
		if (!m_bBossBattleEndEffectOn && m_fEndTime2 < 9.f) {
			//승리BGM 재생
			CSoundMgr::GetInstance()->CSoundMgr::GetInstance()->StopSound(CHANNELID::BGM);
			CSoundMgr::GetInstance()->PlayBGM(L"Boss_Intro2.wav", 1.5f);
			m_bBossBattleEndEffectOn = true;
		}
		m_fEndTime2 -= fTimeDelta;

		if (!m_bBossEventStart) {

			shared_ptr<CGameObject> pEvent = make_shared<CSceneEvent>(m_pGraphicDev);
			pEvent->AwakeGameObject();
			pEvent->ReadyGameObject();

			CSceneMgr::GetInstance()->AddNewObject(L"Layer_4_GameObj", L"Obj_SceneEvent", pEvent);

			m_bBossEventStart = true;
		}

		// 여기서 뭔가의 연출
		if (m_fEndTime2 < 11.f)
		{
			dynamic_pointer_cast<CCreature>(m_vMonsters[2])->MovePos(dynamic_pointer_cast<CCreature>(m_vMonsters[2])->GetTargetPos(),
				fTimeDelta, dynamic_pointer_cast<CCreature>(m_vMonsters[2])->GetMovingSpeed());
		}

		if (m_fEndTime2 < 0.f)
		{
			dynamic_pointer_cast<CCreature>(m_vMonsters[2])->SetAttackMoving(false);
			m_fEndTime2 = 15.f;
			m_bEndBattleStart = false;
			m_bNext = true;

			return true;
		}
	}

	// 크리처의 턴일때
	if (m_bNext && dynamic_pointer_cast<CCreature>(m_pCurrentCreature) &&
		dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetTurn()
		&& !m_bDeathMoving && !m_bAttackSkillMoving && !m_bWhileAttack && !m_bWhileStressOutput)
	{

		// 턴 커서 키기
		dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->OnTurnCursor();

		// 턴 ui 끄기
		dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->OffTurnUi();

		// 배틀 ui 키기
		if (m_bHero && !m_bUISetDone)
		{
			shared_ptr<CHero> pHero = dynamic_pointer_cast<CHero>(m_pCurrentCreature);
			tstring HeroName;

			//UI Setting
			switch (pHero->GetHeroType())
			{
			case EHeroType::VESTAL:
				HeroName = L"VESTAL";
				break;
			case EHeroType::HIGHWAYMAN:
				HeroName = L"HIGHWAYMAN";
				break;
			case EHeroType::SHILEDBREAKER:
				HeroName = L"SHILEDBREAKER";
				break;
			case EHeroType::JESTER:
				HeroName = L"JESTER";
				break;
			default:
				break;
			}

			if (m_pHeroUI && !m_bUISetDone)
			{
				m_pHeroUI->SetVisible(true);
				m_pHeroUI->SettingHeroInfo(pHero->GetMaxHp(), pHero->GetHp(), pHero->GetStress(), HeroName, pHero->GetSkillVector());
				m_bUISetDone = true;
			}
		}

		// 영웅 차례이면서 자동전투가 아니면
		if (m_bHero && !m_bAutoBattle)
		{
			while (m_bNext && !m_bCalculate && !m_bWhileDotDamEffectRender && !m_bWhileStressEffectRender && !m_bWhileStunEffectRender)
			{
				// 출혈, 독뎀 반영, 기절이면 기절 줄어들기, 죽으면 죽음상태로
				if (!m_bCalculate)
				{
					int iDamage;

					// 출혈, 중독, 기절 상태인 경우 딜레이 주기
					if (!m_bCalcBlight && dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetIsBlight())
					{
						dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->StartCalcBlight(iDamage);

						shared_ptr<CEffect> pEffect = CEffectMgr::GetInstance()->GetEffect();

						pEffect->SetFontEffect(L"UI_Blight", dynamic_pointer_cast<CTransform>(m_pCurrentCreature->GetComponent(L"Com_Transform", ID_DYNAMIC))->GetPos(), DOTDAMEFFECTTIME);
						pEffect->SetActive(true);

						pEffect = CEffectMgr::GetInstance()->GetEffect();

						pEffect->SetDamageEffect(0, iDamage, dynamic_pointer_cast<CTransform>(m_pCurrentCreature->GetComponent(L"Com_Transform", ID_DYNAMIC))->GetPos(), DOTDAMEFFECTTIME);
						pEffect->SetActive(true);

						m_bWhileDotDamEffectRender = true;
						m_bCalcBlight = true;
						break;
					}

					if (!m_bCalcBleed && dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetIsBleed())
					{
						dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->StartCalcBleed(iDamage);

						shared_ptr<CEffect> pEffect = CEffectMgr::GetInstance()->GetEffect();

						pEffect->SetFontEffect(L"UI_Blood", dynamic_pointer_cast<CTransform>(m_pCurrentCreature->GetComponent(L"Com_Transform", ID_DYNAMIC))->GetPos(), DOTDAMEFFECTTIME);
						pEffect->SetActive(true);

						pEffect = CEffectMgr::GetInstance()->GetEffect();

						pEffect->SetDamageEffect(0, iDamage, dynamic_pointer_cast<CTransform>(m_pCurrentCreature->GetComponent(L"Com_Transform", ID_DYNAMIC))->GetPos(), DOTDAMEFFECTTIME);
						pEffect->SetActive(true);

						m_bWhileDotDamEffectRender = true;
						m_bCalcBleed = true;
						break;
					}

					if (!m_bCalcStun && dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetIsStun())
					{
						dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->StartCalcStun(iDamage);

						shared_ptr<CEffect> pEffect = CEffectMgr::GetInstance()->GetEffect();

						pEffect->SetFontEffect(L"UI_Stun", dynamic_pointer_cast<CTransform>(m_pCurrentCreature->GetComponent(L"Com_Transform", ID_DYNAMIC))->GetPos(), DOTDAMEFFECTTIME);
						pEffect->SetActive(true);

						m_bWhileStunEffectRender = true;
						m_bCalcStun = true;

						dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->SetStun(false);
						break;
					}

					if (!m_bCalcBuff)
					{
						dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->StartCalcBuff(iDamage);

						m_bCalcBuff = true;
					}

					else
					{
						m_bCalcBlight = false;
						m_bCalcBleed = false;
						m_bCalcStun = false;
						m_bCalcBuff = false;
						m_bCalculate = true;
					}
				}

				// 종료 조건
				if (HeroesAllDead() || MonstersAllDead())
				{
					if (EndBattle())
					{
						m_bEndBattleStart = true;
					}
				}

				if (dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetIsCorpse() ||
					dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetIsDeath() ||
					!dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetTurn())
				{
					CreatureTurnEnd();
					m_bNext = false;
				}
			}


			// 중독, 출혈 데미지 이펙트가 발생중인 시간
			if (m_bWhileDotDamEffectRender)
			{
				printf("중독, 출혈 데미지 이펙트 발생 시간\m_fDotDamageEffectTime : %f\n", m_fDotDamageEffectTime);

				m_fDotDamageEffectTime -= fTimeDelta;

				if (m_fDotDamageEffectTime < 0.f)
				{
					m_bWhileDotDamEffectRender = false;
					m_fDotDamageEffectTime = DOTDAMEFFECTTIME;
				}
			}

			// 기절 이펙트가 발생중인 시간
			if (m_bWhileStunEffectRender)
			{
				printf("기절 이펙트 시간\m_fStunEffectTime : %f\n", m_fDotDamageEffectTime);

				m_fDotDamageEffectTime -= fTimeDelta;

				if (m_fDotDamageEffectTime < 0.f)
				{
					m_bWhileStunEffectRender = false;
					// 기절 이펙트 없애는 타이밍
					dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetLoopEffect()->Reset();
					dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetLoopEffect()->SetActive(false);
					dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->SetTurn(false);
					m_fDotDamageEffectTime = DOTDAMEFFECTTIME;

					// 크리처 턴 엔드
					CreatureTurnEnd();
					m_bNext = false;
					m_bBattle = false;
					m_bCalculate = false;
					m_bCalcBlight = false;
					m_bCalcBleed = false;
					m_bCalcStun = false;
					m_bCalcBuff = false;

					m_vStressTargetHeroes.clear();
					m_bStressEffectRender = false;
				}
			}

			// 스트레스 획득 이펙트가 발생중인 시간
			if (m_bWhileStressEffectRender)
			{
				printf("스트레스 획득 이펙트 발생 시간\m_fStressEffectTime : %f\n", m_fStressEffectTime);

				m_fStressEffectTime -= fTimeDelta;

				if (m_fStressEffectTime <= STRESSEFFECTTIME / 2.f && !m_bStressEffectRender)
				{
					for (auto& iter : m_vStressTargetHeroes)
						dynamic_pointer_cast<CHero>(iter)->IncreaseStress(m_iStressValue);

					m_bStressEffectRender = true;
				}

				if (m_fStressEffectTime < 0.f)
				{
					m_iStressValue = 0;
					m_vStressTargetHeroes.clear();
					m_bWhileStressEffectRender = false;
					m_fStressEffectTime = STRESSEFFECTTIME;
				}
			}

			if (m_bCalculate && !m_bWhileDotDamEffectRender && !m_bWhileStressEffectRender && !m_bWhileStunEffectRender)
			{
				if (!m_bAblePickingInput)
				{
					for (int i = 0; i < size(m_vMonsters); i++)
					{
						m_vecPickingObject.push_back(m_vMonsters[i]);
					}
					for (int i = 0; i < size(m_vHeroes); i++)
					{
						m_vecPickingObject.push_back(m_vHeroes[i]);
					}
					m_bAblePickingInput = true;
				}

				if (m_bAblePickingInput)
				{
					// 스킬 받아오기
					m_iSelectSkill = m_pHeroUI->GetSkill();

					// 활성화 위치여야만 스킬 선택되도록
					if (m_iSelectSkill != 0 && (m_iSelectSkill != 5) && !dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(m_iSelectSkill - 1)
						->GetActivatePos()[dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetPosition() - 1])
					{
						m_iSelectSkill = 0;
						OffTargetCursor();

						for (int j = 0; j < size(m_vHeroes); j++)
						{
							dynamic_pointer_cast<CCreature>(m_vHeroes[j])->SetPicked(false);
						}
						for (int j = 0; j < size(m_vMonsters); j++)
						{
							dynamic_pointer_cast<CCreature>(m_vMonsters[j])->SetPicked(false);
						}
					}

					// 경우에 따른 피킹 분류, 타겟 ui 띄우기 설정
					if (m_iSelectPosition == 0 && m_iSelectSkill != 0)
					{
						OffTargetCursor();
						// 자리 이동인경우
						if (m_iSelectSkill == 5)
						{
							for (int i = 0; i < size(m_vHeroes); i++)
							{
								// 앞뒤만 켜지게
								if (!dynamic_pointer_cast<CCreature>(m_vHeroes[i])->GetIsDeath())
								{
									if (fabs(i - dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetPosition() + 1) == 1)
										dynamic_pointer_cast<CCreature>(m_vHeroes[i])->OnTargetTeamCursor();
								}
							}
							for (int i = 0; i < size(m_vHeroes); i++)
							{
								if (dynamic_pointer_cast<CCreature>(m_vHeroes[i])->GetPicked())
								{
									m_iSelectPosition = dynamic_pointer_cast<CCreature>(m_vHeroes[i])->GetPosition();
									for (int j = 0; j < size(m_vHeroes); j++)
									{
										dynamic_pointer_cast<CCreature>(m_vHeroes[j])->SetPicked(false);
									}
									break;
								}
							}
						}
						// 힐, 버프인경우
						else if (dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(m_iSelectSkill - 1)->GetHeal() != 0)
						{
							// 단일 힐, 단일 버프
							if (!dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(m_iSelectSkill - 1)->IsTargetAll())
							{
								for (int i = 0; i < size(m_vHeroes); i++)
								{
									if (!dynamic_pointer_cast<CCreature>(m_vHeroes[i])->GetIsDeath())
									{
										dynamic_pointer_cast<CCreature>(m_vHeroes[i])->OnTargetTeamCursor();
									}
								}
							}
							// 광역 힐, 광역 버프
							else
							{
								for (int i = 0; i < size(m_vHeroes); i++)
								{
									if (!dynamic_pointer_cast<CCreature>(m_vHeroes[i])->GetIsDeath())
									{
										dynamic_pointer_cast<CCreature>(m_vHeroes[i])->OnTargetTeamCursor();
										dynamic_pointer_cast<CCreature>(m_vHeroes[i])->OnTargetTeamsCursor();
									}
								}
								dynamic_pointer_cast<CCreature>(m_vHeroes[0])->OffTargetTeamsCursor();
							}
							for (int i = 0; i < size(m_vHeroes); i++)
							{
								if (dynamic_pointer_cast<CCreature>(m_vHeroes[i])->GetPicked())
								{
									m_iSelectPosition = dynamic_pointer_cast<CCreature>(m_vHeroes[i])->GetPosition();
									for (int j = 0; j < size(m_vHeroes); j++)
									{
										dynamic_pointer_cast<CCreature>(m_vHeroes[j])->SetPicked(false);
									}
									break;
								}
							}
						}
						// 일반 적 타겟팅
						else
						{
							// 단일 공격
							if (!dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(m_iSelectSkill - 1)->IsTargetAll())
							{
								for (int i = 0; i < size(m_vMonsters); i++)
								{
									if (!dynamic_pointer_cast<CCreature>(m_vMonsters[i])->GetIsDeath() &&
										dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(m_iSelectSkill - 1)->GetTargetPos()[i])
									{
										dynamic_pointer_cast<CCreature>(m_vMonsters[i])->OnTargetEnemyCursor();
									}
								}
							}
							// 광역 공격
							else
							{
								int iLast = 0;
								for (int i = 0; i < size(m_vMonsters); i++)
								{
									if (!dynamic_pointer_cast<CCreature>(m_vMonsters[i])->GetIsDeath())
									{
										dynamic_pointer_cast<CCreature>(m_vMonsters[i])->OnTargetEnemyCursor();
										dynamic_pointer_cast<CCreature>(m_vMonsters[i])->OnTargetEnemiesCursor();
										iLast = i;
									}
								}
								dynamic_pointer_cast<CCreature>(m_vMonsters[iLast])->OffTargetEnemiesCursor();
							}
							for (int i = 0; i < size(m_vMonsters); i++)
							{
								if (dynamic_pointer_cast<CCreature>(m_vMonsters[i])->GetPicked())
								{
									m_iSelectPosition = dynamic_pointer_cast<CCreature>(m_vMonsters[i])->GetPosition();
									for (int j = 0; j < size(m_vMonsters); j++)
									{
										dynamic_pointer_cast<CCreature>(m_vMonsters[j])->SetPicked(false);
									}
									break;
								}
							}
						}
					}
				}

				if (m_iSelectPosition != 0 && m_iSelectSkill != 0)
				{
					//m_bSkillInput = false;
					m_bAblePickingInput = false;
					//m_bPositionInput = false;

					// 타겟 커서 다 끄기
					OffTargetCursor();

					Battle(m_iSelectSkill);
					// 크리처 턴 엔드
					CreatureTurnEnd();

					m_bNext = false;
					m_bCalculate = false;

					m_pHeroUI->SetSkill(0);
					m_iSelectPosition = 0;
					m_vecPickingObject.clear();

				}
			}
		}
		// 자동전투이면
		else
		{
			while (m_bNext && !m_bWhileDotDamEffectRender && !m_bWhileStressEffectRender && !m_bWhileStunEffectRender)
			{
				// 출혈, 독뎀 반영, 기절이면 기절 줄어들기, 죽으면 죽음상태로
				if (!m_bCalculate)
				{
					int iDamage;

					//dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->StartCalculate(false, iDamage);

					// 출혈, 중독, 기절 상태인 경우 딜레이 주기
					if (!m_bCalcBlight && dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetIsBlight())
					{
						dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->StartCalcBlight(iDamage);

						shared_ptr<CEffect> pEffect = CEffectMgr::GetInstance()->GetEffect();

						pEffect->SetFontEffect(L"UI_Blight", dynamic_pointer_cast<CTransform>(m_pCurrentCreature->GetComponent(L"Com_Transform", ID_DYNAMIC))->GetPos(), DOTDAMEFFECTTIME);
						pEffect->SetActive(true);

						pEffect = CEffectMgr::GetInstance()->GetEffect();

						pEffect->SetDamageEffect(0, iDamage, dynamic_pointer_cast<CTransform>(m_pCurrentCreature->GetComponent(L"Com_Transform", ID_DYNAMIC))->GetPos(), DOTDAMEFFECTTIME);
						pEffect->SetActive(true);

						m_bWhileDotDamEffectRender = true;
						m_bCalcBlight = true;
						break;
					}

					if (!m_bCalcBleed && dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetIsBleed())
					{
						dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->StartCalcBleed(iDamage);

						shared_ptr<CEffect> pEffect = CEffectMgr::GetInstance()->GetEffect();

						pEffect->SetFontEffect(L"UI_Blood", dynamic_pointer_cast<CTransform>(m_pCurrentCreature->GetComponent(L"Com_Transform", ID_DYNAMIC))->GetPos(), DOTDAMEFFECTTIME);
						pEffect->SetActive(true);

						pEffect = CEffectMgr::GetInstance()->GetEffect();

						pEffect->SetDamageEffect(0, iDamage, dynamic_pointer_cast<CTransform>(m_pCurrentCreature->GetComponent(L"Com_Transform", ID_DYNAMIC))->GetPos(), DOTDAMEFFECTTIME);
						pEffect->SetActive(true);

						m_bWhileDotDamEffectRender = true;
						m_bCalcBleed = true;
						break;
					}

					if (!m_bCalcStun && dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetIsStun())
					{
						dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->StartCalcStun(iDamage);

						shared_ptr<CEffect> pEffect = CEffectMgr::GetInstance()->GetEffect();

						pEffect->SetFontEffect(L"UI_Stun", dynamic_pointer_cast<CTransform>(m_pCurrentCreature->GetComponent(L"Com_Transform", ID_DYNAMIC))->GetPos(), DOTDAMEFFECTTIME);
						pEffect->SetActive(true);

						m_bWhileStunEffectRender = true;
						m_bCalcStun = true;

						dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->SetStun(false);
						break;
					}

					else
					{
						m_bCalcBlight = false;
						m_bCalcBleed = false;
						m_bCalcStun = false;
						m_bCalcBuff = false;
						m_bCalculate = true;
					}
				}

				// 종료 조건
				if (HeroesAllDead() || MonstersAllDead())
				{
					if (EndBattle())
					{
						m_bEndBattleStart = true;
					}
				}

				if (!m_bBattle)
				{

					if (!dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetIsCorpse() &&
						!dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetIsDeath() &&
						dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetTurn())
					{
						Battle(0);
						m_bBattle = true;
					}
				}

				// 여기서 스트레스 획득 시간동안 붙잡아야됨
				if (m_bWhileStressEffectRender)
					break;

				// 크리처 턴 엔드
				CreatureTurnEnd();
				m_bNext = false;
				m_bBattle = false;
				m_bCalculate = false;
				m_bCalcBlight = false;
				m_bCalcBleed = false;
				m_bCalcStun = false;

				m_vStressTargetHeroes.clear();
				m_bStressEffectRender = false;
			}


			// 중독, 출혈 데미지 이펙트가 발생중인 시간
			if (m_bWhileDotDamEffectRender)
			{

				m_fDotDamageEffectTime -= fTimeDelta;

				if (m_fDotDamageEffectTime < 0.f)
				{
					m_bWhileDotDamEffectRender = false;
					m_fDotDamageEffectTime = DOTDAMEFFECTTIME;
				}
			}

			// 기절 이펙트가 발생중인 시간
			if (m_bWhileStunEffectRender)
			{
				printf("기절 이펙트 시간\m_fStunEffectTime : %f\n", m_fDotDamageEffectTime);

				m_fDotDamageEffectTime -= fTimeDelta;

				if (m_fDotDamageEffectTime < 0.f)
				{
					m_bWhileStunEffectRender = false;
					// 기절 이펙트 없애는 타이밍
					dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetLoopEffect()->Reset();
					dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetLoopEffect()->SetActive(false);
					dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->SetTurn(false);
					m_fDotDamageEffectTime = DOTDAMEFFECTTIME;

					// 크리처 턴 엔드
					CreatureTurnEnd();
					m_bNext = false;
					m_bBattle = false;
					m_bCalculate = false;
					m_bCalcBlight = false;
					m_bCalcBleed = false;
					m_bCalcStun = false;

					m_vStressTargetHeroes.clear();
					m_bStressEffectRender = false;
				}
			}

			// 스트레스 획득 이펙트가 발생중인 시간
			if (m_bWhileStressEffectRender)
			{

				m_fStressEffectTime -= fTimeDelta;

				if (m_fStressEffectTime <= STRESSEFFECTTIME / 2.f && !m_bStressEffectRender)
				{
					for (auto& iter : m_vStressTargetHeroes)
						dynamic_pointer_cast<CHero>(iter)->IncreaseStress(m_iStressValue);

					m_bStressEffectRender = true;
				}

				if (m_fStressEffectTime < 0.f)
				{
					m_iStressValue = 0;
					m_vStressTargetHeroes.clear();
					m_bWhileStressEffectRender = false;
					m_fStressEffectTime = STRESSEFFECTTIME;
				}
			}
		}
	}

	// 종료 조건
	if (HeroesAllDead() || MonstersAllDead())
	{
		if (EndBattle())
			m_bEndBattleStart = true;
	}

	//UI Update
	m_pHeroUI->UpdateGameObject(fTimeDelta);

	return false;
}

void CBattleSystem::StartTurn()
{
	// 턴 시작시 나오는 UI등 그런 것들
	if (m_pHeroUI)
	{
		m_pHeroUI->SetVisible(false);
	}
	m_pCurrentCreature = NextCreature();
}

shared_ptr<CGameObject> CBattleSystem::NextCreature()
{
	// 속도를 기준으로 내림차순
	CmpBySpeed(m_vCreatures);
	// 영웅부터 몬스터까지 빠른 속도인 creature를 반환
	for (int i = 0; i < size(m_vCreatures); i++)
	{
		for (int j = 0; j < size(m_vHeroes); j++)
		{
			if ((dynamic_pointer_cast<CCreature>(m_vHeroes[j])->GetCommonStat().iSpeed ==
				dynamic_pointer_cast<CCreature>(m_vCreatures[i])->GetCommonStat().iSpeed)
				&& !(dynamic_pointer_cast<CCreature>(m_vHeroes[j])->GetDone()))
			{

				//카메라 앵글 초기화 
		/*		if (m_fAngle != 0.f) {
					CCameraMgr::GetInstance()->CameraRotation(ECameraMode::BATTLE, m_fAngle, 0.5);
					m_fAngle = 0.f;
				}*/
				//	CCameraMgr::GetInstance()->MovingStraight(ECameraMode::ZOOMOUT, m_vCenterPos);			

				dynamic_pointer_cast<CCreature>(m_vHeroes[j])->SetTurn(true);
				m_bHero = true;

				iCurrentHeroIndex = j;

				return m_vHeroes[j];
			}
		}
		for (int j = 0; j < size(m_vMonsters); j++)
		{
			if ((dynamic_pointer_cast<CCreature>(m_vMonsters[j])->GetCommonStat().iSpeed ==
				dynamic_pointer_cast<CCreature>(m_vCreatures[i])->GetCommonStat().iSpeed)
				&& !(dynamic_pointer_cast<CCreature>(m_vMonsters[j])->GetDone()))
			{

				/*	if (m_fAngle != 0.f) {
						CCameraMgr::GetInstance()->CameraRotation(ECameraMode::BATTLE, m_fAngle, 0.5);
						m_fAngle = 0.f;
					}*/
					//	CCameraMgr::GetInstance()->MovingStraight(ECameraMode::ZOOMOUT, m_vCenterPos);


				dynamic_pointer_cast<CCreature>(m_vMonsters[j])->SetTurn(true);
				m_bHero = false;
				iCurrentMonsterIndex = j;

				return m_vMonsters[j];
			}
		}
	}
	// 모두 다 했을시에 다음 턴으로
	NextTurn();

	return shared_ptr<CGameObject>();
}

void CBattleSystem::CreatureTurnEnd()
{
	dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->SetTurn(false);
	dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->SetDone(true);
	//m_pCurrentCreature = NextCreature();
	// 죽은애 체크
	m_bDeadCheck = true;
	m_bUISetDone = false;
}

void CBattleSystem::NextTurn()
{
	// 모두 초기화
	for (auto& iter : m_vCreatures)
	{
		dynamic_pointer_cast<CCreature>(iter)->SetDone(false);
	}
	for (auto& iter : m_vHeroes)
	{
		dynamic_pointer_cast<CCreature>(iter)->SetDone(false);
	}
	for (auto& iter : m_vMonsters)
	{
		dynamic_pointer_cast<CCreature>(iter)->SetDone(false);
	}
	//m_pCurrentCreature = nullptr;
	// turn UI 키기
	OnTurnUi();

	m_iTurn++;
}

_bool CBattleSystem::EndBattle()
{
	dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->OffTurnCursor();

	// 영웅들 버프, 출혈, 중독, 디버프, 죽음의일격상태 다 초기화
	for (auto& iter : m_vHeroes)
	{
		dynamic_pointer_cast<CCreature>(iter)->BlightCure();
		dynamic_pointer_cast<CCreature>(iter)->BleedCure();
		dynamic_pointer_cast<CCreature>(iter)->BuffReset();
		dynamic_pointer_cast<CCreature>(iter)->SetBeforeDeath(false);
		dynamic_pointer_cast<CCreature>(iter)->UpdateAttribute();
	}

	// 죽은 진영 시체 전부 없애기

	for (auto& iter : m_vCreatures)
	{
		dynamic_pointer_cast<CCreature>(iter)->SetDone(false);
	}
	for (auto& iter : m_vHeroes)
	{
		dynamic_pointer_cast<CCreature>(iter)->SetDone(false);
	}
	for (auto& iter : m_vMonsters)
	{
		dynamic_pointer_cast<CCreature>(iter)->SetDone(false);
	}
	OnTurnUi();

	if (HeroesAllDead())
	{
		for (auto& iter : m_vHeroes)
		{
			dynamic_pointer_cast<CCreature>(iter)->SetHp(-100);
		}
		return true;
	}
	else
	{
		for (auto& iter : m_vMonsters)
		{
			dynamic_pointer_cast<CCreature>(iter)->SetHp(-100);
		}
		return true;
	}
	m_pCurrentCreature = nullptr;

	return false;
}

_bool CBattleSystem::HeroesAllDead()
{
	for (auto& iter : m_vHeroes)
	{
		if (!((dynamic_pointer_cast<CCreature>(iter)->GetIsCorpse()) ||
			(dynamic_pointer_cast<CCreature>(iter)->GetIsDeath())))
		{
			return false;
		}
	}
	return true;
}

_bool CBattleSystem::MonstersAllDead()
{
	for (auto& iter : m_vMonsters)
	{
		if (!((dynamic_pointer_cast<CCreature>(iter)->GetIsCorpse()) ||
			(dynamic_pointer_cast<CCreature>(iter)->GetIsDeath())))
		{
			return false;
		}
	}
	return true;
}

void CBattleSystem::DeadCheck()
{
	// 죽은애 있으면 앞으로 당겨주기
	for (int i = (_int)size(m_vHeroes) - 2; i >= 0; i--)
	{
		if (dynamic_pointer_cast<CCreature>(m_vHeroes[i])->GetIsDeath())
		{
			for (int j = i + 1; j < size(m_vHeroes); j++)
			{
				dynamic_pointer_cast<CCreature>(m_vHeroes[j])->SetMoving(true);
				dynamic_pointer_cast<CCreature>(m_vHeroes[j])->SetTargetPos(m_vHeroLocation[j - 1] );
				dynamic_pointer_cast<CCreature>(m_vHeroes[j])->SetMovingSpeed(
					dynamic_pointer_cast<CCreature>(m_vHeroes[j])->MovingSpeed(m_vHeroLocation[j - 1], DEATHMOVINGINTERVEL));
			}

			for (int j = i + 1; j < size(m_vHeroes); j++)
			{
				shared_ptr<CGameObject> pObj = m_vHeroes[j - 1];
				m_vHeroes[j - 1] = m_vHeroes[j];
				m_vHeroes[j] = pObj;
			}

			m_bDeathMoving = true;
		}
	}

	for (int i = (_int)size(m_vMonsters) - 2; i >= 0; i--)
	{
		if (dynamic_pointer_cast<CCreature>(m_vMonsters[i])->GetIsDeath())
		{
			for (int j = i + 1; j < size(m_vMonsters); j++)
			{
				_vec3 vPos = *dynamic_pointer_cast<CTransform>(m_pCurrentCreature->GetComponent(L"Com_Transform", COMPONENTID::ID_DYNAMIC))->GetPos();

				dynamic_pointer_cast<CCreature>(m_vMonsters[j])->SetMoving(true);
				dynamic_pointer_cast<CCreature>(m_vMonsters[j])->SetTargetPos({ m_vMonsterLocation[j - 1].x, vPos.y,  m_vMonsterLocation[j - 1].z });
				dynamic_pointer_cast<CCreature>(m_vMonsters[j])->SetMovingSpeed(
					dynamic_pointer_cast<CCreature>(m_vMonsters[j])->MovingSpeed({ m_vMonsterLocation[j - 1].x, vPos.y,  m_vMonsterLocation[j - 1].z }, DEATHMOVINGINTERVEL));
			}

			for (int j = i + 1; j < size(m_vMonsters); j++)
			{
				shared_ptr<CGameObject> pObj = m_vMonsters[j - 1];
				m_vMonsters[j - 1] = m_vMonsters[j];
				m_vMonsters[j] = pObj;
			}

			m_bDeathMoving = true;
		}
	}
}

void CBattleSystem::FormBattlePosition(vector<shared_ptr<CGameObject>>& _vHeroes, vector<shared_ptr<CGameObject>>& _vMonsters, _float _fAngle1,
	_float _fAngle2, _vec3 _vOrigin)
{
	shared_ptr<CTransform> pTransform;
	for (int i = 0; i < size(_vHeroes); i++)
	{
		switch (i)
		{
		case 0:
			pTransform = dynamic_pointer_cast<CTransform>(
				_vHeroes[i]->GetComponent(L"Com_Transform", ID_DYNAMIC));
			pTransform->SetPosition(_vOrigin.x - 6.f, _vOrigin.y, _vOrigin.z);
			pTransform->SetAngle(_vec3(0.f, _fAngle1, 0.f));
			m_vHeroLocation.push_back(_vec3(_vOrigin.x - 6.f, _vOrigin.y, _vOrigin.z));
			break;
		case 1:
			pTransform = dynamic_pointer_cast<CTransform>(
				_vHeroes[i]->GetComponent(L"Com_Transform", ID_DYNAMIC));
			pTransform->SetPosition(_vOrigin.x - 8.f, _vOrigin.y, _vOrigin.z - 2.f);
			pTransform->SetAngle(_vec3(0.f, _fAngle1, 0.f));
			m_vHeroLocation.push_back(_vec3(_vOrigin.x - 8.f, _vOrigin.y, _vOrigin.z - 2.f));
			break;
		case 2:
			pTransform = dynamic_pointer_cast<CTransform>(
				_vHeroes[i]->GetComponent(L"Com_Transform", ID_DYNAMIC));
			pTransform->SetPosition(_vOrigin.x - 14.f, _vOrigin.y, _vOrigin.z + 4.f);
			pTransform->SetAngle(_vec3(0.f, _fAngle1, 0.f));
			m_vHeroLocation.push_back(_vec3(_vOrigin.x - 14.f, _vOrigin.y, _vOrigin.z + 4.f));
			break;
		case 3:
			pTransform = dynamic_pointer_cast<CTransform>(
				_vHeroes[i]->GetComponent(L"Com_Transform", ID_DYNAMIC));
			pTransform->SetPosition(_vOrigin.x - 16.f, _vOrigin.y, _vOrigin.z + 2.f);
			pTransform->SetAngle(_vec3(0.f, _fAngle1, 0.f));
			m_vHeroLocation.push_back(_vec3(_vOrigin.x - 16.f, _vOrigin.y, _vOrigin.z + 2.f));
			break;
		}
	}

	for (int i = 0; i < size(_vMonsters); i++)
	{
		switch (i)
		{
		case 0:
			pTransform = dynamic_pointer_cast<CTransform>(
				_vMonsters[i]->GetComponent(L"Com_Transform", ID_DYNAMIC));
			pTransform->SetPosition(_vOrigin.x + 6.f, _vOrigin.y, _vOrigin.z);
			pTransform->SetAngle(_vec3(0.f, _fAngle2, 0.f));
			m_vMonsterLocation.push_back(_vec3(_vOrigin.x + 6.f, _vOrigin.y, _vOrigin.z));
			break;
		case 1:
			pTransform = dynamic_pointer_cast<CTransform>(
				_vMonsters[i]->GetComponent(L"Com_Transform", ID_DYNAMIC));
			pTransform->SetPosition(_vOrigin.x + 8.f, _vOrigin.y, _vOrigin.z - 2.f);
			pTransform->SetAngle(_vec3(0.f, _fAngle2, 0.f));
			m_vMonsterLocation.push_back(_vec3(_vOrigin.x + 8.f, _vOrigin.y, _vOrigin.z - 2.f));
			break;
		case 2:
			pTransform = dynamic_pointer_cast<CTransform>(
				_vMonsters[i]->GetComponent(L"Com_Transform", ID_DYNAMIC));
			pTransform->SetPosition(_vOrigin.x + 14.f, _vOrigin.y, _vOrigin.z + 4.f);
			pTransform->SetAngle(_vec3(0.f, _fAngle2, 0.f));
			m_vMonsterLocation.push_back(_vec3(_vOrigin.x + 14.f, _vOrigin.y, _vOrigin.z + 4.f));
			break;
		case 3:
			pTransform = dynamic_pointer_cast<CTransform>(
				_vMonsters[i]->GetComponent(L"Com_Transform", ID_DYNAMIC));
			pTransform->SetPosition(_vOrigin.x + 16.f, _vOrigin.y, _vOrigin.z + 2.f);
			pTransform->SetAngle(_vec3(0.f, _fAngle2, 0.f));
			m_vMonsterLocation.push_back(_vec3(_vOrigin.x + 16.f, _vOrigin.y, _vOrigin.z + 2.f));
			break;
		}
	}
}

void CBattleSystem::SwitchPosition(int _iCurrentIndex, int _iMoveCnt, _bool _bHero)
{
	// 영웅의 경우 이동
	if (_bHero)
	{
		// 맨 앞 이동일 경우
		if ((_iCurrentIndex - _iMoveCnt) <= 0)
		{
			dynamic_pointer_cast<CCreature>(m_vHeroes[_iCurrentIndex])->SetMoving(true);
			dynamic_pointer_cast<CCreature>(m_vHeroes[_iCurrentIndex])->SetTargetPos(m_vHeroLocation[0]);
			dynamic_pointer_cast<CCreature>(m_vHeroes[_iCurrentIndex])->SetMovingSpeed(
				dynamic_pointer_cast<CCreature>(m_vHeroes[_iCurrentIndex])->MovingSpeed(m_vHeroLocation[0], SKILLMOVEINTERVEL));

			// 뒤로 한칸씩 이동
			for (int i = 0; i < _iCurrentIndex; i++)
			{
				dynamic_pointer_cast<CCreature>(m_vHeroes[i])->SetMoving(true);
				dynamic_pointer_cast<CCreature>(m_vHeroes[i])->SetTargetPos(m_vHeroLocation[i + 1]);
				dynamic_pointer_cast<CCreature>(m_vHeroes[i])->SetMovingSpeed(
					dynamic_pointer_cast<CCreature>(m_vHeroes[i])->MovingSpeed(m_vHeroLocation[i + 1], SKILLMOVEINTERVEL));
			}

			m_bSkillMove = true;
			shared_ptr<CGameObject> pObj1 = m_vHeroes[_iCurrentIndex];
			// swap
			for (int i = _iCurrentIndex - 1; i >= 0; i--)
			{
				shared_ptr<CGameObject> pObj = m_vHeroes[i];
				m_vHeroes[i + 1] = m_vHeroes[i];
				m_vHeroes[i + 1] = pObj;
			}
			m_vHeroes[0] = pObj1;
		}
		// 맨 뒤 이동
		else if ((_iCurrentIndex - _iMoveCnt) >= iLiveHeroes)
		{
			dynamic_pointer_cast<CCreature>(m_vHeroes[_iCurrentIndex])->SetMoving(true);
			dynamic_pointer_cast<CCreature>(m_vHeroes[_iCurrentIndex])->SetTargetPos(m_vHeroLocation[iLiveHeroes - 1]);
			dynamic_pointer_cast<CCreature>(m_vHeroes[_iCurrentIndex])->SetMovingSpeed(
				dynamic_pointer_cast<CCreature>(m_vHeroes[_iCurrentIndex])->MovingSpeed(m_vHeroLocation[iLiveHeroes - 1], SKILLMOVEINTERVEL));

			// 앞으로 한칸씩 이동
			for (int i = iLiveHeroes - 1; i > _iCurrentIndex; i--)
			{
				dynamic_pointer_cast<CCreature>(m_vHeroes[i])->SetMoving(true);
				dynamic_pointer_cast<CCreature>(m_vHeroes[i])->SetTargetPos(m_vHeroLocation[i - 1]);
				dynamic_pointer_cast<CCreature>(m_vHeroes[i])->SetMovingSpeed(
					dynamic_pointer_cast<CCreature>(m_vHeroes[i])->MovingSpeed(m_vHeroLocation[i - 1], SKILLMOVEINTERVEL));
			}

			m_bSkillMove = true;
			shared_ptr<CGameObject> pObj1 = m_vHeroes[_iCurrentIndex];
			// swap
			for (int i = _iCurrentIndex; i < iLiveHeroes - 1; i++)
			{
				shared_ptr<CGameObject> pObj = m_vHeroes[i];
				m_vHeroes[i] = m_vHeroes[i + 1];
				m_vHeroes[i + 1] = pObj;
			}
			m_vHeroes[iLiveHeroes - 1] = pObj1;
		}
		// 한 칸 앞으로 이동
		else if (_iMoveCnt == 1)
		{
			dynamic_pointer_cast<CCreature>(m_vHeroes[_iCurrentIndex])->SetMoving(true);
			dynamic_pointer_cast<CCreature>(m_vHeroes[_iCurrentIndex])->SetTargetPos(m_vHeroLocation[_iCurrentIndex - 1]);
			dynamic_pointer_cast<CCreature>(m_vHeroes[_iCurrentIndex])->SetMovingSpeed(
				dynamic_pointer_cast<CCreature>(m_vHeroes[_iCurrentIndex])->MovingSpeed(m_vHeroLocation[_iCurrentIndex - 1], SKILLMOVEINTERVEL));

			// 앞에 있는 애 뒤로 한칸 이동
			dynamic_pointer_cast<CCreature>(m_vHeroes[_iCurrentIndex - 1])->SetMoving(true);
			dynamic_pointer_cast<CCreature>(m_vHeroes[_iCurrentIndex - 1])->SetTargetPos(m_vHeroLocation[_iCurrentIndex]);
			dynamic_pointer_cast<CCreature>(m_vHeroes[_iCurrentIndex - 1])->SetMovingSpeed(
				dynamic_pointer_cast<CCreature>(m_vHeroes[_iCurrentIndex - 1])->MovingSpeed(m_vHeroLocation[_iCurrentIndex], SKILLMOVEINTERVEL));

			m_bSkillMove = true;

			shared_ptr<CGameObject> pObj = m_vHeroes[_iCurrentIndex];
			m_vHeroes[_iCurrentIndex] = m_vHeroes[_iCurrentIndex - 1];
			m_vHeroes[_iCurrentIndex - 1] = pObj;
		}
		// 한 칸 뒤로 이동
		else if (_iMoveCnt == -1)
		{
			dynamic_pointer_cast<CCreature>(m_vHeroes[_iCurrentIndex])->SetMoving(true);
			dynamic_pointer_cast<CCreature>(m_vHeroes[_iCurrentIndex])->SetTargetPos(m_vHeroLocation[_iCurrentIndex + 1]);
			dynamic_pointer_cast<CCreature>(m_vHeroes[_iCurrentIndex])->SetMovingSpeed(
				dynamic_pointer_cast<CCreature>(m_vHeroes[_iCurrentIndex])->MovingSpeed(m_vHeroLocation[_iCurrentIndex + 1], SKILLMOVEINTERVEL));

			// 뒤에 있는 애 앞으로로 한칸 이동
			dynamic_pointer_cast<CCreature>(m_vHeroes[_iCurrentIndex + 1])->SetMoving(true);
			dynamic_pointer_cast<CCreature>(m_vHeroes[_iCurrentIndex + 1])->SetTargetPos(m_vHeroLocation[_iCurrentIndex]);
			dynamic_pointer_cast<CCreature>(m_vHeroes[_iCurrentIndex + 1])->SetMovingSpeed(
				dynamic_pointer_cast<CCreature>(m_vHeroes[_iCurrentIndex + 1])->MovingSpeed(m_vHeroLocation[_iCurrentIndex], SKILLMOVEINTERVEL));

			m_bSkillMove = true;

			shared_ptr<CGameObject> pObj = m_vHeroes[_iCurrentIndex];
			m_vHeroes[_iCurrentIndex] = m_vHeroes[_iCurrentIndex + 1];
			m_vHeroes[_iCurrentIndex + 1] = pObj;
		}
	}
}

void CBattleSystem::CmpBySpeed(vector<shared_ptr<CGameObject>>& _vCreatures)
{
	int size = (_int)_vCreatures.size();
	for (int i = 0; i < size; i++) {
		for (int j = 1; j < size - i; j++) {
			if (dynamic_pointer_cast<CCreature>(_vCreatures[j])->GetCommonStat().iSpeed >
				dynamic_pointer_cast<CCreature>(_vCreatures[j - 1])->GetCommonStat().iSpeed) {
				shared_ptr<CGameObject> pObj = _vCreatures[j];
				_vCreatures[j] = _vCreatures[j - 1];
				_vCreatures[j - 1] = pObj;
			}
		}
	}
}

_bool CBattleSystem::SkillKeyInput()
{
	if (GetAsyncKeyState('1') & 0x8000) {
		m_iSelectSkill = 1;
		return true;
	}
	if (GetAsyncKeyState('2') & 0x8000) {
		m_iSelectSkill = 2;
		return true;
	}
	if (GetAsyncKeyState('3') & 0x8000) {
		m_iSelectSkill = 3;
		return true;
	}
	if (GetAsyncKeyState('4') & 0x8000) {
		m_iSelectSkill = 4;
		return true;
	}
	if (GetAsyncKeyState('5') & 0x8000) {
		m_iSelectSkill = 5;
		return true;
	}

	m_iSelectSkill = 0;
	return false;
}

_bool CBattleSystem::PositionKeyInput()
{
	if (GetAsyncKeyState('1') & 0x8000) {
		m_iSelectPosition = 1;
		return true;
	}
	if (GetAsyncKeyState('2') & 0x8000) {
		m_iSelectPosition = 2;
		return true;
	}
	if (GetAsyncKeyState('3') & 0x8000) {
		m_iSelectPosition = 3;
		return true;
	}
	if (GetAsyncKeyState('4') & 0x8000) {
		m_iSelectPosition = 4;
		return true;
	}

	m_iSelectPosition = 0;
	return false;
}

void CBattleSystem::AutoBattleKeyInput()
{
	//if (GetAsyncKeyState('7') & 0x8000) {
	//	if (m_bAutoBattle)
	//		m_bAutoBattle = false;
	//	else
	//		m_bAutoBattle = true;
	//}
}

void CBattleSystem::Battle(int _iNum)
{

	int iNum = 0;
	// 위치 변경
	if (_iNum == 5)
	{
		iNum = 5;
	}
	// 스킬 지정
	else if (_iNum >= 1)
	{
		iNum = _iNum - 1;
	}
	// auto battle 일때
	else
		iNum = rand() % dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkillNum();

	int iTarget = 0;
	int iTargetTeam = 0;


	// 죽은 애들 빼주기
	int iDeathHeroes = 0;
	for (auto& iter : m_vHeroes)
	{
		if (dynamic_pointer_cast<CCreature>(iter)->GetIsDeath())
		{
			iDeathHeroes++;
		}
	}
	int iDeathMonsters = 0;
	for (auto& iter : m_vMonsters)
	{
		if (dynamic_pointer_cast<CCreature>(iter)->GetIsDeath())
		{
			iDeathMonsters++;
		}
	}
	iLiveMonsters = (_int)size(m_vMonsters) - iDeathMonsters;
	iLiveHeroes = (_int)size(m_vHeroes) - iDeathHeroes;

	// 자동전투가 아닐때
	if (!m_bAutoBattle)
	{
		// 사망하지 않은 적만 때리게하기
		if (m_bHero)
		{
			iTarget = m_iSelectPosition - 1;
			iTargetTeam = m_iSelectPosition - 1;
		}
		else
		{
			while (1)
			{
				if (HeroesAllDead() || MonstersAllDead())
				{
					EndBattle();
					break;
				}
				iTarget = rand() % (size(m_vHeroes) - iDeathHeroes);
				if (!dynamic_pointer_cast<CCreature>(m_vHeroes[iTarget])->GetIsDeath() &&
					dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum)->GetTargetPos()[iTarget])
				{
					// 기절 공격이고 그 대상이 방파자일때 다른 대상을 고르도록
					if(!(dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum)->GetArrAttack()[3] &&
						dynamic_pointer_cast<CHero>(m_vHeroes[iTarget])->GetHeroType() == EHeroType::SHILEDBREAKER))
					break;
				}
			}
		}
	}
	// 자동전투일때
	else
	{
		// 사망하지 않은 적만 때리게하기
		if (m_bHero)
		{
			while (1)
			{
				if (HeroesAllDead() || MonstersAllDead())
				{
					EndBattle();
					break;
				}
				iTarget = rand() % (size(m_vMonsters) - iDeathMonsters);
				if (!dynamic_pointer_cast<CCreature>(m_vMonsters[iTarget])->GetIsDeath() &&
					dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum)->GetTargetPos()[iTarget])
				{
					break;
				}
			}
			while (1)
			{
				iTargetTeam = rand() % (size(m_vHeroes) - iDeathHeroes);
				if (!dynamic_pointer_cast<CCreature>(m_vHeroes[iTargetTeam])->GetIsDeath() &&
					dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum)->GetTargetPos()[iTargetTeam])
				{
					break;
				}
			}
		}
		else
		{
			while (1)
			{
				if (HeroesAllDead() || MonstersAllDead())
				{
					EndBattle();
					break;
				}
				iTarget = rand() % (size(m_vHeroes) - iDeathHeroes);
				if (!dynamic_pointer_cast<CCreature>(m_vHeroes[iTarget])->GetIsDeath() &&
					dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum)->GetTargetPos()[iTarget])
				{
					break;
				}
			}
		}
	}

	if (m_bHero)
	{
		// 이동할때
		if (iNum == 5)
		{
			m_bSwitching = true;

			dynamic_pointer_cast<CCreature>(m_vHeroes[iCurrentHeroIndex])->SetMoving(true);
			dynamic_pointer_cast<CCreature>(m_vHeroes[iCurrentHeroIndex])->SetTargetPos(m_vHeroLocation[iTarget]);
			dynamic_pointer_cast<CCreature>(m_vHeroes[iCurrentHeroIndex])->SetMovingSpeed(
				dynamic_pointer_cast<CCreature>(m_vHeroes[iCurrentHeroIndex])->MovingSpeed(m_vHeroLocation[iTarget], SWITCHINGINTERVEL));

			dynamic_pointer_cast<CCreature>(m_vHeroes[iTarget])->SetMoving(true);
			dynamic_pointer_cast<CCreature>(m_vHeroes[iTarget])->SetTargetPos(m_vHeroLocation[iCurrentHeroIndex]);
			dynamic_pointer_cast<CCreature>(m_vHeroes[iTarget])->SetMovingSpeed(
				dynamic_pointer_cast<CCreature>(m_vHeroes[iTarget])->MovingSpeed(m_vHeroLocation[iCurrentHeroIndex], SWITCHINGINTERVEL));

			shared_ptr<CGameObject> pObj = m_vHeroes[iCurrentHeroIndex];
			m_vHeroes[iCurrentHeroIndex] = m_vHeroes[iTarget];
			m_vHeroes[iTarget] = pObj;
		}
		else
		{
			// 다가가는 스킬일 경우에 이동하게
			if (dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum)->IsApproach())
			{
				// 다가가는 스킬 이후 이동해야하는 공격일 경우
				if (dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum)->GetMovingCnt() != 0)
				{
					int iMovePos = dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum)->GetMovingCnt();
					// 맨 앞 이동일 경우
					if ((iCurrentHeroIndex - iMovePos) <= 0)
					{
						// 뒤로 한칸씩 이동
						for (int i = 0; i < iCurrentHeroIndex; i++)
						{
							dynamic_pointer_cast<CCreature>(m_vHeroes[i])->SetMoving(true);
							dynamic_pointer_cast<CCreature>(m_vHeroes[i])->SetTargetPos(m_vHeroLocation[i + 1]);
							dynamic_pointer_cast<CCreature>(m_vHeroes[i])->SetMovingSpeed(
								dynamic_pointer_cast<CCreature>(m_vHeroes[i])->MovingSpeed(m_vHeroLocation[i + 1], SKILLMOVEINTERVEL));
						}

						m_bSkillMove = true;
						shared_ptr<CGameObject> pObj1 = m_vHeroes[iCurrentHeroIndex];
						// swap
						for (int i = iCurrentHeroIndex - 1; i >= 0; i--)
						{
							shared_ptr<CGameObject> pObj = m_vHeroes[i];
							m_vHeroes[i + 1] = m_vHeroes[i];
							m_vHeroes[i + 1] = pObj;
						}
						m_vHeroes[0] = pObj1;
						dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->SetTargetPos2(m_vHeroLocation[0]);
					}
					// 맨 뒤 이동
					else if ((iCurrentHeroIndex - iMovePos) >= iLiveHeroes)
					{
						// 앞으로 한칸씩 이동
						for (int i = iLiveHeroes - 1; i > iCurrentHeroIndex; i--)
						{
							dynamic_pointer_cast<CCreature>(m_vHeroes[i])->SetMoving(true);
							dynamic_pointer_cast<CCreature>(m_vHeroes[i])->SetTargetPos(m_vHeroLocation[i - 1]);
							dynamic_pointer_cast<CCreature>(m_vHeroes[i])->SetMovingSpeed(
								dynamic_pointer_cast<CCreature>(m_vHeroes[i])->MovingSpeed(m_vHeroLocation[i - 1], SKILLMOVEINTERVEL));
						}

						m_bSkillMove = true;
						shared_ptr<CGameObject> pObj1 = m_vHeroes[iCurrentHeroIndex];
						// swap
						for (int i = iCurrentHeroIndex; i < iLiveHeroes - 1; i++)
						{
							shared_ptr<CGameObject> pObj = m_vHeroes[i];
							m_vHeroes[i] = m_vHeroes[i + 1];
							m_vHeroes[i + 1] = pObj;
						}
						m_vHeroes[iLiveHeroes - 1] = pObj1;
						dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->SetTargetPos2(m_vHeroLocation[iLiveHeroes - 1]);
					}
					// 한 칸 앞으로 이동
					else if (iMovePos == 1)
					{
						// 앞에 있는 애 뒤로 한칸 이동
						dynamic_pointer_cast<CCreature>(m_vHeroes[iCurrentHeroIndex - 1])->SetMoving(true);
						dynamic_pointer_cast<CCreature>(m_vHeroes[iCurrentHeroIndex - 1])->SetTargetPos(m_vHeroLocation[iCurrentHeroIndex]);
						dynamic_pointer_cast<CCreature>(m_vHeroes[iCurrentHeroIndex - 1])->SetMovingSpeed(
							dynamic_pointer_cast<CCreature>(m_vHeroes[iCurrentHeroIndex - 1])->MovingSpeed(m_vHeroLocation[iCurrentHeroIndex], SKILLMOVEINTERVEL));

						m_bSkillMove = true;

						shared_ptr<CGameObject> pObj = m_vHeroes[iCurrentHeroIndex];
						m_vHeroes[iCurrentHeroIndex] = m_vHeroes[iCurrentHeroIndex - 1];
						m_vHeroes[iCurrentHeroIndex - 1] = pObj;
						dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->SetTargetPos2(m_vHeroLocation[iCurrentHeroIndex - 1]);
					}
					// 한 칸 뒤로 이동
					else if (iMovePos == -1)
					{
						// 뒤에 있는 애 앞으로로 한칸 이동
						dynamic_pointer_cast<CCreature>(m_vHeroes[iCurrentHeroIndex + 1])->SetMoving(true);
						dynamic_pointer_cast<CCreature>(m_vHeroes[iCurrentHeroIndex + 1])->SetTargetPos(m_vHeroLocation[iCurrentHeroIndex]);
						dynamic_pointer_cast<CCreature>(m_vHeroes[iCurrentHeroIndex + 1])->SetMovingSpeed(
							dynamic_pointer_cast<CCreature>(m_vHeroes[iCurrentHeroIndex + 1])->MovingSpeed(m_vHeroLocation[iCurrentHeroIndex], SKILLMOVEINTERVEL));

						m_bSkillMove = true;

						shared_ptr<CGameObject> pObj = m_vHeroes[iCurrentHeroIndex];
						m_vHeroes[iCurrentHeroIndex] = m_vHeroes[iCurrentHeroIndex + 1];
						m_vHeroes[iCurrentHeroIndex + 1] = pObj;
						dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->SetTargetPos2(m_vHeroLocation[iCurrentHeroIndex + 1]);
					}


					_float fDistance = static_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum)->GetSkillDistance();
					_vec3 vPos = { fDistance  , 0.f , 0.f };
					dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->SetAttackMoving(true);
					dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->SetTargetPos(m_vMonsterLocation[iTarget] + m_vApproachingGapR + vPos);
					dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->SetMovingSpeed(
						dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->MovingSpeed(m_vMonsterLocation[iTarget] + m_vApproachingGapR + vPos, ATTACKSKILLMOVINGINTERVEL));
					m_bAttackSkillMoving = true;
				}
				else
				{
					//스킬별로 오차 거리 추가로 더하는 부분 
					_float fDistance = static_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum)->GetSkillDistance();
					_vec3 vPos = { fDistance  , 0.f , 0.f };

					dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->SetAttackMoving(true);
					dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->SetTargetPos(m_vMonsterLocation[iTarget] + m_vApproachingGapR + vPos);
					dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->SetTargetPos2(m_pCurrentCreature->GetPos());
					dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->SetMovingSpeed(
						dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->MovingSpeed(m_vMonsterLocation[iTarget] + m_vApproachingGapR + vPos, ATTACKSKILLMOVINGINTERVEL));
					m_bAttackSkillMoving = true;
				}
			}


			// 다가가는 스킬이 아니고 이동하는 공격일 경우
			if (dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum)->GetMovingCnt() != 0 &&
				!dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum)->IsApproach())
			{
				int iMovePos = dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum)->GetMovingCnt();

				SwitchPosition(iCurrentHeroIndex, iMovePos, true);
			}

			// 광역기 공격이고 힐이 아닐경우
			if (dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum)->IsTargetAll() &&
				(dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum)->GetHeal() == 0))
			{
				for (int i = 0; i < size(m_vMonsters); i++)
				{

					dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->AttackCreature
					(dynamic_pointer_cast<CCreature>(m_vMonsters[i]), dynamic_pointer_cast<CCreature>(m_pCurrentCreature), dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum));
					dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->SetAttacking(true, iNum);
				}
			}
			// 힐이고 광역일 경우
			else if ((dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum)->GetHeal() > 0) &&
				dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum)->IsTargetAll())
			{
				CCameraMgr::GetInstance()->MovingStraight(ECameraMode::ZOOMIN, { m_vCenterPos.x - 10.f , m_vCenterPos.y , m_vCenterPos.z + 4.f });
				m_bChangeSkillAnim = true;
				for (int i = 0; i < size(m_vHeroes); i++)
				{
					dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->AttackCreature
					(dynamic_pointer_cast<CCreature>(m_vHeroes[i]), dynamic_pointer_cast<CCreature>(m_pCurrentCreature), dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum));
					dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->SetAttacking(true, iNum);
				}

			}
			// 단일 힐일경우
			else if (dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum)->GetHeal() > 0 &&
				!dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum)->IsTargetAll())
			{
				CCameraMgr::GetInstance()->MovingStraight(ECameraMode::ZOOMIN, { m_vCenterPos.x - 10.f , m_vCenterPos.y , m_vCenterPos.z + 4.f });
				m_bChangeSkillAnim = true;
				dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->AttackCreature
				(dynamic_pointer_cast<CCreature>(m_vHeroes[iTargetTeam]), dynamic_pointer_cast<CCreature>(m_pCurrentCreature), dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum));
				dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->SetAttacking(true, iNum);
			}
			// 광역 버프일경우
			else if (dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum)->GetHeal() == -100 &&
				dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum)->IsTargetAll())
			{
				for (int i = 0; i < size(m_vHeroes); i++)
				{
					dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->AttackCreature
					(dynamic_pointer_cast<CCreature>(m_vHeroes[i]), dynamic_pointer_cast<CCreature>(m_pCurrentCreature), dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum));
					dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->SetAttacking(true, iNum);
				}
			}
			// 이동시키는 공격일 경우
			else if ((dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum)->GetMoveCnt() != 0))
			{
				dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->AttackCreature
				(dynamic_pointer_cast<CCreature>(m_vMonsters[iTarget]), dynamic_pointer_cast<CCreature>(m_pCurrentCreature), dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum));
				dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->SetAttacking(true, iNum);

				if (dynamic_pointer_cast<CCreature>(m_vMonsters[iTarget])->GetHitted())
				{
					int iMovePos = dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum)->GetMoveCnt();
					if (dynamic_pointer_cast<CCreature>(m_vMonsters[iTarget + iMovePos]) &&
						!dynamic_pointer_cast<CCreature>(m_vMonsters[iTarget + iMovePos])->GetIsDeath() &&
						!dynamic_pointer_cast<CCreature>(m_vMonsters[iTarget])->GetIsDeath())
					{
						_vec3 vPos = *dynamic_pointer_cast<CTransform>(m_pCurrentCreature->GetComponent(L"Com_Transform", COMPONENTID::ID_DYNAMIC))->GetPos();

						dynamic_pointer_cast<CCreature>(m_vMonsters[iTarget])->SetMoving(true);
						dynamic_pointer_cast<CCreature>(m_vMonsters[iTarget])->SetTargetPos({ m_vMonsterLocation[iTarget + iMovePos].x, vPos.y ,m_vMonsterLocation[iTarget + iMovePos].z });
						dynamic_pointer_cast<CCreature>(m_vMonsters[iTarget])->SetMovingSpeed(
							dynamic_pointer_cast<CCreature>(m_vMonsters[iTarget])->MovingSpeed({ m_vMonsterLocation[iTarget + iMovePos].x, vPos.y ,m_vMonsterLocation[iTarget + iMovePos].z }, SKILLMOVINGINTERVEL));

						dynamic_pointer_cast<CCreature>(m_vMonsters[iTarget + iMovePos])->SetMoving(true);
						dynamic_pointer_cast<CCreature>(m_vMonsters[iTarget + iMovePos])->SetTargetPos({ m_vMonsterLocation[iTarget].x, vPos.y ,m_vMonsterLocation[iTarget].z });
						dynamic_pointer_cast<CCreature>(m_vMonsters[iTarget + iMovePos])->SetMovingSpeed(
							dynamic_pointer_cast<CCreature>(m_vMonsters[iTarget + iMovePos])->MovingSpeed({ m_vMonsterLocation[iTarget].x, vPos.y ,m_vMonsterLocation[iTarget].z }, SKILLMOVINGINTERVEL));
						m_bSkillMoving = true;
						shared_ptr<CGameObject> pObj = m_vMonsters[iTarget];
						m_vMonsters[iTarget] = m_vMonsters[iTarget + iMovePos];
						m_vMonsters[iTarget + iMovePos] = pObj;
					}
				}
			}
			// 일반 공격일 경우
			else
			{
				//원거리 공격
				if (!static_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum)->IsApproach()) {

					//카메라 이펙트
					_int iPos = static_pointer_cast<CCreature>(m_pCurrentCreature)->GetPosition();
					_float fRevision = 0.f;

					if (3 == iPos || 4 == iPos) {
						fRevision = 10.f;
					}

					CCameraMgr::GetInstance()->MovingStraight(ECameraMode::ZOOMIN, { m_pCurrentCreature->GetPos().x + 2.f + fRevision , m_pCurrentCreature->GetPos().y , m_pCurrentCreature->GetPos().z - 5.f + (-1.f * 5.f * iPos) });
					CCameraMgr::GetInstance()->CameraRotation(ECameraMode::BATTLE, 20.f + iPos * -1.f * 5.f, 0.5f);
					// 20 degree 
					m_bChangeSkillAnim = true;
					m_bShakingDone = false;
					m_fAngle -= (20.f + iPos * -1.f * 5.f);

					//적 알파값

					//for (int i = 0; i < iTarget; ++i) {
					//	m_vMonsters[i]->SetRenderState(true);
					//}
				}


				dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->AttackCreature
				(dynamic_pointer_cast<CCreature>(m_vMonsters[iTarget]), dynamic_pointer_cast<CCreature>(m_pCurrentCreature), dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum));
				dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->SetAttacking(true, iNum);
			}

			// 이펙트
			{
				// 애니메이션, 이펙트 바꾸는 코드 넣어야할듯
				if (dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum)->GetEffectAnimKey() != L"")
				{
					dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->SetEffectInfo(dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum), false, false);
				}
			}
		}
	}
	else
	{
		// 다가가는 스킬일 경우에 이동하게
		if (dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum)->IsApproach())
		{
			dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->SetAttackMoving(true);
			dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->SetTargetPos(m_vHeroLocation[iTarget] + m_vApproachingGapL);
			dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->SetTargetPos2(m_pCurrentCreature->GetPos());
			dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->SetMovingSpeed(
				dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->MovingSpeed(m_vHeroLocation[iTarget] + m_vApproachingGapL, ATTACKSKILLMOVINGINTERVEL));
			m_bAttackSkillMoving = true;
		}

		// 광역기 공격일 경우
		if (dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum)->IsTargetAll())
		{
			for (int i = 0; i < size(m_vHeroes); i++)
			{
				m_vStressTargetHeroes.push_back(m_vHeroes[i]);
				dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->AttackCreature
				(dynamic_pointer_cast<CCreature>(m_vHeroes[i]), dynamic_pointer_cast<CCreature>(m_pCurrentCreature), dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum));
				dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->SetAttacking(true, iNum);
			}
		}
		// 이동시키는 공격일 경우
		else if (dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum)->GetMoveCnt() != 0)
		{
			m_vStressTargetHeroes.push_back(m_vHeroes[iTarget]);

			dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->AttackCreature
			(dynamic_pointer_cast<CCreature>(m_vHeroes[iTarget]), dynamic_pointer_cast<CCreature>(m_pCurrentCreature), dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum));
			dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->SetAttacking(true, iNum);

			if (dynamic_pointer_cast<CCreature>(m_vHeroes[iTarget])->GetHitted())
			{
				int iMovePos = dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum)->GetMoveCnt();
				if (dynamic_pointer_cast<CCreature>(m_vHeroes[iTarget + iMovePos]) &&
					!dynamic_pointer_cast<CCreature>(m_vHeroes[iTarget + iMovePos])->GetIsDeath() &&
					!dynamic_pointer_cast<CCreature>(m_vHeroes[iTarget])->GetIsDeath())
				{
					dynamic_pointer_cast<CCreature>(m_vHeroes[iTarget])->SetMoving(true);
					dynamic_pointer_cast<CCreature>(m_vHeroes[iTarget])->SetTargetPos(m_vHeroLocation[iTarget + iMovePos]);
					dynamic_pointer_cast<CCreature>(m_vHeroes[iTarget])->SetMovingSpeed(
						dynamic_pointer_cast<CCreature>(m_vHeroes[iTarget])->MovingSpeed(m_vHeroLocation[iTarget + iMovePos], SKILLMOVINGINTERVEL));

					dynamic_pointer_cast<CCreature>(m_vHeroes[iTarget + iMovePos])->SetMoving(true);
					dynamic_pointer_cast<CCreature>(m_vHeroes[iTarget + iMovePos])->SetTargetPos(m_vHeroLocation[iTarget]);
					dynamic_pointer_cast<CCreature>(m_vHeroes[iTarget + iMovePos])->SetMovingSpeed(
						dynamic_pointer_cast<CCreature>(m_vHeroes[iTarget + iMovePos])->MovingSpeed(m_vHeroLocation[iTarget], SKILLMOVINGINTERVEL));
					m_bSkillMoving = true;
					shared_ptr<CGameObject> pObj = m_vHeroes[iTarget];
					m_vHeroes[iTarget] = m_vHeroes[iTarget + iMovePos];
					m_vHeroes[iTarget + iMovePos] = pObj;
				}
			}
		}
		// 일반 공격일 경우
		else
		{
			m_vStressTargetHeroes.push_back(m_vHeroes[iTarget]);

			dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->AttackCreature
			(dynamic_pointer_cast<CCreature>(m_vHeroes[iTarget]), dynamic_pointer_cast<CCreature>(m_pCurrentCreature), dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum));
			dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->SetAttacking(true, iNum);
		}

		// 스트레스 공격인 경우
		if (dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum)->GetStress() != 0)
		{
			for (auto& iter : m_vStressTargetHeroes)
			{
				if (dynamic_pointer_cast<CCreature>(iter)->IsIncreaseStress())
				{
					m_bStressEffectRender = false;
					m_fStressEffectTime = STRESSEFFECTTIME;
					m_bWhileStressEffectRender = true;
					m_iStressValue = dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum)->GetStress();
				}
			}
		}
	}
}

void CBattleSystem::OffTargetCursor()
{
	// 타겟 커서 다 끄기
	for (int i = 0; i < size(m_vHeroes); i++)
	{
		dynamic_pointer_cast<CCreature>(m_vHeroes[i])->OffTargetTeamCursor();
		dynamic_pointer_cast<CCreature>(m_vHeroes[i])->OffTargetTeamsCursor();
		dynamic_pointer_cast<CCreature>(m_vHeroes[i])->OffTargetEnemyCursor();
		dynamic_pointer_cast<CCreature>(m_vHeroes[i])->OffTargetEnemiesCursor();
	}
	for (int i = 0; i < size(m_vMonsters); i++)
	{
		dynamic_pointer_cast<CCreature>(m_vMonsters[i])->OffTargetTeamCursor();
		dynamic_pointer_cast<CCreature>(m_vMonsters[i])->OffTargetTeamsCursor();
		dynamic_pointer_cast<CCreature>(m_vMonsters[i])->OffTargetEnemyCursor();
		dynamic_pointer_cast<CCreature>(m_vMonsters[i])->OffTargetEnemiesCursor();
	}
}

void CBattleSystem::OnTurnUi()
{
	for (int i = 0; i < size(m_vHeroes); i++)
	{
		if (!dynamic_pointer_cast<CCreature>(m_vHeroes[i])->GetIsDeath())
			dynamic_pointer_cast<CCreature>(m_vHeroes[i])->OnTurnUi();
	}
	for (int i = 0; i < size(m_vMonsters); i++)
	{
		if (!dynamic_pointer_cast<CCreature>(m_vMonsters[i])->GetIsDeath())
			dynamic_pointer_cast<CCreature>(m_vMonsters[i])->OnTurnUi();
	}
}
