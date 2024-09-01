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

	//���� ������ ������ ���� �� �����ð� ���� ���󺹱��� �������� �κ� 
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

	// �ڵ����� ����
	AutoBattleKeyInput();

	// �ڸ� ����Ī �̵���
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

	// �����ϸ� �����̴� ��ų �̵���
	if (m_bAttackSkillMoving)
	{
		m_fAttackSkillMovingTime -= fTimeDelta;
		for (auto& iter : m_vHeroes)
		{
			if (dynamic_pointer_cast<CCreature>(iter)->GetAttackMoving())
			{
				//�ٰŸ� ����
				_vec3 vTargetPos = dynamic_pointer_cast<CCreature>(iter)->GetTargetPos();
				CCameraMgr::GetInstance()->MovingStraight(ECameraMode::ZOOMIN, { vTargetPos.x + 2.f , vTargetPos.y , vTargetPos.z - 8.f });
				dynamic_pointer_cast<CCreature>(iter)->MovePos(vTargetPos, fTimeDelta, dynamic_pointer_cast<CCreature>(iter)->GetMovingSpeed());
			}
		}
		for (auto& iter : m_vMonsters)
		{
			if (dynamic_pointer_cast<CCreature>(iter)->GetAttackMoving())
			{
				//�ٰŸ� ����
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

			// ���� ��ġ�� ���ư��� �ӵ� ����ֱ�
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

	// ���缭 �����ϴ� �ð�
	if (m_bWhileAttack)
	{
		m_fWhileAttackingTime -= fTimeDelta;

		//ī�޶� ���� : ���缭 �����ϴ� ���� ������ �̵� 
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

		//������ ������ ������ ����ŷ 
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

			//�����·� ���ƿ���
			CCameraMgr::GetInstance()->MovingStraight(ECameraMode::ZOOMOUT, m_vCenterPos);

			if (m_bHero)
				CCameraMgr::GetInstance()->CameraRotation(ECameraMode::BATTLE, 20.f, 0.7f);
			else
				CCameraMgr::GetInstance()->CameraRotation(ECameraMode::BATTLE, -20.f, 0.7f);

			m_bCamEffectCheck = false;
			m_bRotationCheck = false;

		}
	}

	// �����ϸ� �����̴� ��ų �̵� ���Ŀ� ���ư���
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

	// ���� �ֵ� üũ
	if (m_bDeadCheck && !m_bAttackSkillMoving && !m_bWhileAttack)
	{
		DeadCheck();
		m_bDeadCheck = false;
	}

	// ������ ������ ��ġ ����
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

	// �����̴� ��ų ��ġ ����
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

	// ��ų ���ְ� �����̴� ��ġ ����
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

	// ���¿� ���� �� ���� ���� ����
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
			// ��Ʋ ui ����, �� ui ����
			dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->OffTurnCursor();
			if (m_bHero)
			{
				if (m_pHeroUI)
					m_pHeroUI->SetVisible(false);
			}

			m_pCurrentCreature = NextCreature();
			m_bNext = true;
			m_bCounting = false;

			// �� ��ü�� �ڽ��� ��ġ ����
			for (int i = 0; i < size(m_vHeroes); i++)
			{
				dynamic_pointer_cast<CCreature>(m_vHeroes[i])->SetPosition(i + 1);
			}
			for (int i = 0; i < size(m_vMonsters); i++)
			{
				dynamic_pointer_cast<CCreature>(m_vMonsters[i])->SetPosition(i + 1);
			}

			// ��Ʈ���� �̺�Ʈ
			for (int i = 0; i < size(m_vHeroes); i++)
			{
				if (!dynamic_pointer_cast<CCreature>(m_vHeroes[i])->GetIsDeath())
				{
					if (!dynamic_pointer_cast<CHero>(m_vHeroes[i])->GetStressEvent())
					{
						// �ر� �߻��� ����
						if (dynamic_pointer_cast<CHero>(m_vHeroes[i])->IsAffliction())
						{
							dynamic_pointer_cast<CHero>(m_vHeroes[i])->SetStressEvent(true);
							dynamic_pointer_cast<CHero>(m_vHeroes[i])->OnAffliction();
							m_bNext = false;
							m_bWhileStressEvent = true;
						}
						// ��� �߻��� ����
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

			// ��Ʈ���� ���� �̺�Ʈ �߻�
			if (m_bHero && !m_bWhileStressEvent)
			{
				for (int i = 0; i < size(m_vHeroes); i++)
				{
					if (!dynamic_pointer_cast<CCreature>(m_vHeroes[i])->GetIsDeath())
					{
						// selfish ���� �ִ���
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
						// irrational ���� �ִ���
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
						// COURAGEOUS ���� �ִ���
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
						// VIGOROUS ���� �ִ���
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

	// ��Ʈ���� �̺�Ʈ �߻����� �ð�
	if (m_bWhileStressEvent)
	{
		printf("��Ʈ���� �̺�Ʈ �߻� �ð�\m_fStressEventTime : %f\n", m_fStressEventTime);

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

	// ��Ʈ���� ���� �ൿ �߻� �ð�
	if (m_bWhileStressOutput)
	{
		printf("��Ʈ���� ���� �ൿ �߻� �ð�\m_fStressOutputTime : %f\n", m_fStressOutputTime);

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

	// ��������
	if (m_bEndBattleStart && m_bNext)
	{
		m_bNext = false;

		// �������̿��� ���
		if (m_iBattleType == 1)
		{
			dynamic_pointer_cast<CCreature>(m_vMonsters[2])->SetMoving(true);
			dynamic_pointer_cast<CCreature>(m_vMonsters[2])->SetTargetPos(_vec3(300.f, 7.5f, 230.f));
			dynamic_pointer_cast<CCreature>(m_vMonsters[2])->SetMovingSpeed(
			dynamic_pointer_cast<CCreature>(m_vMonsters[2])->MovingSpeed(_vec3(300.f, 7.5f, 230.f), 14.f));
		}
	}
	// �Ϲ��������� ����
	if (m_bEndBattleStart && !m_bNext && (m_iBattleType == 0))
	{
		//1ȸ�� ����� �� �ֵ��� bool State�� ����
		if (!m_bBattleEndEffectOn) {
			//�¸�BGM ���
			CSoundMgr::GetInstance()->StopSound(CHANNELID::BGM);
			CSoundMgr::GetInstance()->StopSound(CHANNELID::MONSTER);
			CSoundMgr::GetInstance()->PlaySound(L"Combat_Victory.wav", CHANNELID::MONSTER, 1.f);

			CUIMgr::GetInstance()->TextBoardOn(L"�¸�!", { 0.f, 150.f, 0.3f }, { 150.f, 70.f, 1.f }, STRESSEVENTINTERVEL - 1.f);
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
	// ���������� ����
	if (m_bEndBattleStart && !m_bNext && (m_iBattleType == 1))
	{
		//1ȸ�� ����� �� �ֵ��� bool State�� ����
		if (!m_bBossBattleEndEffectOn && m_fEndTime2 < 9.f) {
			//�¸�BGM ���
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

		// ���⼭ ������ ����
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

	// ũ��ó�� ���϶�
	if (m_bNext && dynamic_pointer_cast<CCreature>(m_pCurrentCreature) &&
		dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetTurn()
		&& !m_bDeathMoving && !m_bAttackSkillMoving && !m_bWhileAttack && !m_bWhileStressOutput)
	{

		// �� Ŀ�� Ű��
		dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->OnTurnCursor();

		// �� ui ����
		dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->OffTurnUi();

		// ��Ʋ ui Ű��
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

		// ���� �����̸鼭 �ڵ������� �ƴϸ�
		if (m_bHero && !m_bAutoBattle)
		{
			while (m_bNext && !m_bCalculate && !m_bWhileDotDamEffectRender && !m_bWhileStressEffectRender && !m_bWhileStunEffectRender)
			{
				// ����, ���� �ݿ�, �����̸� ���� �پ���, ������ �������·�
				if (!m_bCalculate)
				{
					int iDamage;

					// ����, �ߵ�, ���� ������ ��� ������ �ֱ�
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

				// ���� ����
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


			// �ߵ�, ���� ������ ����Ʈ�� �߻����� �ð�
			if (m_bWhileDotDamEffectRender)
			{
				printf("�ߵ�, ���� ������ ����Ʈ �߻� �ð�\m_fDotDamageEffectTime : %f\n", m_fDotDamageEffectTime);

				m_fDotDamageEffectTime -= fTimeDelta;

				if (m_fDotDamageEffectTime < 0.f)
				{
					m_bWhileDotDamEffectRender = false;
					m_fDotDamageEffectTime = DOTDAMEFFECTTIME;
				}
			}

			// ���� ����Ʈ�� �߻����� �ð�
			if (m_bWhileStunEffectRender)
			{
				printf("���� ����Ʈ �ð�\m_fStunEffectTime : %f\n", m_fDotDamageEffectTime);

				m_fDotDamageEffectTime -= fTimeDelta;

				if (m_fDotDamageEffectTime < 0.f)
				{
					m_bWhileStunEffectRender = false;
					// ���� ����Ʈ ���ִ� Ÿ�̹�
					dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetLoopEffect()->Reset();
					dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetLoopEffect()->SetActive(false);
					dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->SetTurn(false);
					m_fDotDamageEffectTime = DOTDAMEFFECTTIME;

					// ũ��ó �� ����
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

			// ��Ʈ���� ȹ�� ����Ʈ�� �߻����� �ð�
			if (m_bWhileStressEffectRender)
			{
				printf("��Ʈ���� ȹ�� ����Ʈ �߻� �ð�\m_fStressEffectTime : %f\n", m_fStressEffectTime);

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
					// ��ų �޾ƿ���
					m_iSelectSkill = m_pHeroUI->GetSkill();

					// Ȱ��ȭ ��ġ���߸� ��ų ���õǵ���
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

					// ��쿡 ���� ��ŷ �з�, Ÿ�� ui ���� ����
					if (m_iSelectPosition == 0 && m_iSelectSkill != 0)
					{
						OffTargetCursor();
						// �ڸ� �̵��ΰ��
						if (m_iSelectSkill == 5)
						{
							for (int i = 0; i < size(m_vHeroes); i++)
							{
								// �յڸ� ������
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
						// ��, �����ΰ��
						else if (dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(m_iSelectSkill - 1)->GetHeal() != 0)
						{
							// ���� ��, ���� ����
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
							// ���� ��, ���� ����
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
						// �Ϲ� �� Ÿ����
						else
						{
							// ���� ����
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
							// ���� ����
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

					// Ÿ�� Ŀ�� �� ����
					OffTargetCursor();

					Battle(m_iSelectSkill);
					// ũ��ó �� ����
					CreatureTurnEnd();

					m_bNext = false;
					m_bCalculate = false;

					m_pHeroUI->SetSkill(0);
					m_iSelectPosition = 0;
					m_vecPickingObject.clear();

				}
			}
		}
		// �ڵ������̸�
		else
		{
			while (m_bNext && !m_bWhileDotDamEffectRender && !m_bWhileStressEffectRender && !m_bWhileStunEffectRender)
			{
				// ����, ���� �ݿ�, �����̸� ���� �پ���, ������ �������·�
				if (!m_bCalculate)
				{
					int iDamage;

					//dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->StartCalculate(false, iDamage);

					// ����, �ߵ�, ���� ������ ��� ������ �ֱ�
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

				// ���� ����
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

				// ���⼭ ��Ʈ���� ȹ�� �ð����� ����ƾߵ�
				if (m_bWhileStressEffectRender)
					break;

				// ũ��ó �� ����
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


			// �ߵ�, ���� ������ ����Ʈ�� �߻����� �ð�
			if (m_bWhileDotDamEffectRender)
			{

				m_fDotDamageEffectTime -= fTimeDelta;

				if (m_fDotDamageEffectTime < 0.f)
				{
					m_bWhileDotDamEffectRender = false;
					m_fDotDamageEffectTime = DOTDAMEFFECTTIME;
				}
			}

			// ���� ����Ʈ�� �߻����� �ð�
			if (m_bWhileStunEffectRender)
			{
				printf("���� ����Ʈ �ð�\m_fStunEffectTime : %f\n", m_fDotDamageEffectTime);

				m_fDotDamageEffectTime -= fTimeDelta;

				if (m_fDotDamageEffectTime < 0.f)
				{
					m_bWhileStunEffectRender = false;
					// ���� ����Ʈ ���ִ� Ÿ�̹�
					dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetLoopEffect()->Reset();
					dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetLoopEffect()->SetActive(false);
					dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->SetTurn(false);
					m_fDotDamageEffectTime = DOTDAMEFFECTTIME;

					// ũ��ó �� ����
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

			// ��Ʈ���� ȹ�� ����Ʈ�� �߻����� �ð�
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

	// ���� ����
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
	// �� ���۽� ������ UI�� �׷� �͵�
	if (m_pHeroUI)
	{
		m_pHeroUI->SetVisible(false);
	}
	m_pCurrentCreature = NextCreature();
}

shared_ptr<CGameObject> CBattleSystem::NextCreature()
{
	// �ӵ��� �������� ��������
	CmpBySpeed(m_vCreatures);
	// �������� ���ͱ��� ���� �ӵ��� creature�� ��ȯ
	for (int i = 0; i < size(m_vCreatures); i++)
	{
		for (int j = 0; j < size(m_vHeroes); j++)
		{
			if ((dynamic_pointer_cast<CCreature>(m_vHeroes[j])->GetCommonStat().iSpeed ==
				dynamic_pointer_cast<CCreature>(m_vCreatures[i])->GetCommonStat().iSpeed)
				&& !(dynamic_pointer_cast<CCreature>(m_vHeroes[j])->GetDone()))
			{

				//ī�޶� �ޱ� �ʱ�ȭ 
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
	// ��� �� �����ÿ� ���� ������
	NextTurn();

	return shared_ptr<CGameObject>();
}

void CBattleSystem::CreatureTurnEnd()
{
	dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->SetTurn(false);
	dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->SetDone(true);
	//m_pCurrentCreature = NextCreature();
	// ������ üũ
	m_bDeadCheck = true;
	m_bUISetDone = false;
}

void CBattleSystem::NextTurn()
{
	// ��� �ʱ�ȭ
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
	// turn UI Ű��
	OnTurnUi();

	m_iTurn++;
}

_bool CBattleSystem::EndBattle()
{
	dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->OffTurnCursor();

	// ������ ����, ����, �ߵ�, �����, �������ϰݻ��� �� �ʱ�ȭ
	for (auto& iter : m_vHeroes)
	{
		dynamic_pointer_cast<CCreature>(iter)->BlightCure();
		dynamic_pointer_cast<CCreature>(iter)->BleedCure();
		dynamic_pointer_cast<CCreature>(iter)->BuffReset();
		dynamic_pointer_cast<CCreature>(iter)->SetBeforeDeath(false);
		dynamic_pointer_cast<CCreature>(iter)->UpdateAttribute();
	}

	// ���� ���� ��ü ���� ���ֱ�

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
	// ������ ������ ������ ����ֱ�
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
	// ������ ��� �̵�
	if (_bHero)
	{
		// �� �� �̵��� ���
		if ((_iCurrentIndex - _iMoveCnt) <= 0)
		{
			dynamic_pointer_cast<CCreature>(m_vHeroes[_iCurrentIndex])->SetMoving(true);
			dynamic_pointer_cast<CCreature>(m_vHeroes[_iCurrentIndex])->SetTargetPos(m_vHeroLocation[0]);
			dynamic_pointer_cast<CCreature>(m_vHeroes[_iCurrentIndex])->SetMovingSpeed(
				dynamic_pointer_cast<CCreature>(m_vHeroes[_iCurrentIndex])->MovingSpeed(m_vHeroLocation[0], SKILLMOVEINTERVEL));

			// �ڷ� ��ĭ�� �̵�
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
		// �� �� �̵�
		else if ((_iCurrentIndex - _iMoveCnt) >= iLiveHeroes)
		{
			dynamic_pointer_cast<CCreature>(m_vHeroes[_iCurrentIndex])->SetMoving(true);
			dynamic_pointer_cast<CCreature>(m_vHeroes[_iCurrentIndex])->SetTargetPos(m_vHeroLocation[iLiveHeroes - 1]);
			dynamic_pointer_cast<CCreature>(m_vHeroes[_iCurrentIndex])->SetMovingSpeed(
				dynamic_pointer_cast<CCreature>(m_vHeroes[_iCurrentIndex])->MovingSpeed(m_vHeroLocation[iLiveHeroes - 1], SKILLMOVEINTERVEL));

			// ������ ��ĭ�� �̵�
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
		// �� ĭ ������ �̵�
		else if (_iMoveCnt == 1)
		{
			dynamic_pointer_cast<CCreature>(m_vHeroes[_iCurrentIndex])->SetMoving(true);
			dynamic_pointer_cast<CCreature>(m_vHeroes[_iCurrentIndex])->SetTargetPos(m_vHeroLocation[_iCurrentIndex - 1]);
			dynamic_pointer_cast<CCreature>(m_vHeroes[_iCurrentIndex])->SetMovingSpeed(
				dynamic_pointer_cast<CCreature>(m_vHeroes[_iCurrentIndex])->MovingSpeed(m_vHeroLocation[_iCurrentIndex - 1], SKILLMOVEINTERVEL));

			// �տ� �ִ� �� �ڷ� ��ĭ �̵�
			dynamic_pointer_cast<CCreature>(m_vHeroes[_iCurrentIndex - 1])->SetMoving(true);
			dynamic_pointer_cast<CCreature>(m_vHeroes[_iCurrentIndex - 1])->SetTargetPos(m_vHeroLocation[_iCurrentIndex]);
			dynamic_pointer_cast<CCreature>(m_vHeroes[_iCurrentIndex - 1])->SetMovingSpeed(
				dynamic_pointer_cast<CCreature>(m_vHeroes[_iCurrentIndex - 1])->MovingSpeed(m_vHeroLocation[_iCurrentIndex], SKILLMOVEINTERVEL));

			m_bSkillMove = true;

			shared_ptr<CGameObject> pObj = m_vHeroes[_iCurrentIndex];
			m_vHeroes[_iCurrentIndex] = m_vHeroes[_iCurrentIndex - 1];
			m_vHeroes[_iCurrentIndex - 1] = pObj;
		}
		// �� ĭ �ڷ� �̵�
		else if (_iMoveCnt == -1)
		{
			dynamic_pointer_cast<CCreature>(m_vHeroes[_iCurrentIndex])->SetMoving(true);
			dynamic_pointer_cast<CCreature>(m_vHeroes[_iCurrentIndex])->SetTargetPos(m_vHeroLocation[_iCurrentIndex + 1]);
			dynamic_pointer_cast<CCreature>(m_vHeroes[_iCurrentIndex])->SetMovingSpeed(
				dynamic_pointer_cast<CCreature>(m_vHeroes[_iCurrentIndex])->MovingSpeed(m_vHeroLocation[_iCurrentIndex + 1], SKILLMOVEINTERVEL));

			// �ڿ� �ִ� �� �����η� ��ĭ �̵�
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
	// ��ġ ����
	if (_iNum == 5)
	{
		iNum = 5;
	}
	// ��ų ����
	else if (_iNum >= 1)
	{
		iNum = _iNum - 1;
	}
	// auto battle �϶�
	else
		iNum = rand() % dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkillNum();

	int iTarget = 0;
	int iTargetTeam = 0;


	// ���� �ֵ� ���ֱ�
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

	// �ڵ������� �ƴҶ�
	if (!m_bAutoBattle)
	{
		// ������� ���� ���� �������ϱ�
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
					// ���� �����̰� �� ����� �������϶� �ٸ� ����� ������
					if(!(dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum)->GetArrAttack()[3] &&
						dynamic_pointer_cast<CHero>(m_vHeroes[iTarget])->GetHeroType() == EHeroType::SHILEDBREAKER))
					break;
				}
			}
		}
	}
	// �ڵ������϶�
	else
	{
		// ������� ���� ���� �������ϱ�
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
		// �̵��Ҷ�
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
			// �ٰ����� ��ų�� ��쿡 �̵��ϰ�
			if (dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum)->IsApproach())
			{
				// �ٰ����� ��ų ���� �̵��ؾ��ϴ� ������ ���
				if (dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum)->GetMovingCnt() != 0)
				{
					int iMovePos = dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum)->GetMovingCnt();
					// �� �� �̵��� ���
					if ((iCurrentHeroIndex - iMovePos) <= 0)
					{
						// �ڷ� ��ĭ�� �̵�
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
					// �� �� �̵�
					else if ((iCurrentHeroIndex - iMovePos) >= iLiveHeroes)
					{
						// ������ ��ĭ�� �̵�
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
					// �� ĭ ������ �̵�
					else if (iMovePos == 1)
					{
						// �տ� �ִ� �� �ڷ� ��ĭ �̵�
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
					// �� ĭ �ڷ� �̵�
					else if (iMovePos == -1)
					{
						// �ڿ� �ִ� �� �����η� ��ĭ �̵�
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
					//��ų���� ���� �Ÿ� �߰��� ���ϴ� �κ� 
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


			// �ٰ����� ��ų�� �ƴϰ� �̵��ϴ� ������ ���
			if (dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum)->GetMovingCnt() != 0 &&
				!dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum)->IsApproach())
			{
				int iMovePos = dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum)->GetMovingCnt();

				SwitchPosition(iCurrentHeroIndex, iMovePos, true);
			}

			// ������ �����̰� ���� �ƴҰ��
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
			// ���̰� ������ ���
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
			// ���� ���ϰ��
			else if (dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum)->GetHeal() > 0 &&
				!dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum)->IsTargetAll())
			{
				CCameraMgr::GetInstance()->MovingStraight(ECameraMode::ZOOMIN, { m_vCenterPos.x - 10.f , m_vCenterPos.y , m_vCenterPos.z + 4.f });
				m_bChangeSkillAnim = true;
				dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->AttackCreature
				(dynamic_pointer_cast<CCreature>(m_vHeroes[iTargetTeam]), dynamic_pointer_cast<CCreature>(m_pCurrentCreature), dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum));
				dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->SetAttacking(true, iNum);
			}
			// ���� �����ϰ��
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
			// �̵���Ű�� ������ ���
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
			// �Ϲ� ������ ���
			else
			{
				//���Ÿ� ����
				if (!static_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum)->IsApproach()) {

					//ī�޶� ����Ʈ
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

					//�� ���İ�

					//for (int i = 0; i < iTarget; ++i) {
					//	m_vMonsters[i]->SetRenderState(true);
					//}
				}


				dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->AttackCreature
				(dynamic_pointer_cast<CCreature>(m_vMonsters[iTarget]), dynamic_pointer_cast<CCreature>(m_pCurrentCreature), dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum));
				dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->SetAttacking(true, iNum);
			}

			// ����Ʈ
			{
				// �ִϸ��̼�, ����Ʈ �ٲٴ� �ڵ� �־���ҵ�
				if (dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum)->GetEffectAnimKey() != L"")
				{
					dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->SetEffectInfo(dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum), false, false);
				}
			}
		}
	}
	else
	{
		// �ٰ����� ��ų�� ��쿡 �̵��ϰ�
		if (dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum)->IsApproach())
		{
			dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->SetAttackMoving(true);
			dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->SetTargetPos(m_vHeroLocation[iTarget] + m_vApproachingGapL);
			dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->SetTargetPos2(m_pCurrentCreature->GetPos());
			dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->SetMovingSpeed(
				dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->MovingSpeed(m_vHeroLocation[iTarget] + m_vApproachingGapL, ATTACKSKILLMOVINGINTERVEL));
			m_bAttackSkillMoving = true;
		}

		// ������ ������ ���
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
		// �̵���Ű�� ������ ���
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
		// �Ϲ� ������ ���
		else
		{
			m_vStressTargetHeroes.push_back(m_vHeroes[iTarget]);

			dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->AttackCreature
			(dynamic_pointer_cast<CCreature>(m_vHeroes[iTarget]), dynamic_pointer_cast<CCreature>(m_pCurrentCreature), dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->GetSkill(iNum));
			dynamic_pointer_cast<CCreature>(m_pCurrentCreature)->SetAttacking(true, iNum);
		}

		// ��Ʈ���� ������ ���
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
	// Ÿ�� Ŀ�� �� ����
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
