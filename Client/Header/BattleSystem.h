#pragma once
#include"GameObject.h"

class CBattleHeroUI;


class CBattleSystem
{
public:
	CBattleSystem();
	~CBattleSystem();

public:
	vector<shared_ptr<CGameObject>> GetHeroesVector() { return m_vHeroes; }
	vector<shared_ptr<CGameObject>> GetMonstersVector() { return m_vMonsters; }
	void SetAutoBattle(_bool _bAutoBattle) { m_bAutoBattle = _bAutoBattle; }
	void SetCurrentCreature(shared_ptr<CGameObject> _pCrea) { m_pCurrentCreature = _pCrea; }
	void PushCreaturesVector(vector<shared_ptr<CGameObject>>& _vVec)
	{
		m_vCreatures = _vVec;
	}
	void PushHeroesVector(vector<shared_ptr<CGameObject>>& _vVec)
	{
		m_vHeroes = _vVec;
	}
	void PushMonstersVector(vector<shared_ptr<CGameObject>>& _vVec)
	{
		m_vMonsters = _vVec;
	}

public:
	_bool Update(const _float& fTimeDelta); // 턴 진행중

public:
	// 턴 시작시 호출
	void StartTurn();

	// 매 크리처 턴 시작시 호출
	shared_ptr<CGameObject> NextCreature(); // 영웅부터 몬스터까지 빠른 속도인 creature를 반환

	// 매 크리처 턴 종료시 호출
	void CreatureTurnEnd();

	// 턴 종료시 호출
	void NextTurn();	// 다음 턴

	// 배틀 끝
	_bool EndBattle();	// 전투 종료

	_bool HeroesAllDead();
	_bool MonstersAllDead();

	// 죽은 애 있으면 뒤로 보내기
	void	DeadCheck();

	// 전투진형 배치
	void	FormBattlePosition(vector<shared_ptr<CGameObject>>& _vHeroes, vector<shared_ptr<CGameObject>>& _vMonsters, _float _fAngle1,
		_float _fAngle2, _vec3 _vOrigin);

	void GetHeroPanelUI(shared_ptr<CBattleHeroUI> _pHeroUI) { m_pHeroUI = _pHeroUI; }

public:
	void SetCenterPos(_vec3 _vPos) { m_vCenterPos = _vPos; }
	void SetBattleType(_int _iType) { m_iBattleType = _iType; }

	void SetGraphicDev(LPDIRECT3DDEVICE9 _pGraphicDev) {m_pGraphicDev = _pGraphicDev;}


protected:
	// 위치 변경
	void SwitchPosition(int _iCurrentIndex, int _iMoveCnt, _bool _bHero = true);
	void CmpBySpeed(vector<shared_ptr<CGameObject>>& _vCreatures);
	_bool SkillKeyInput();		// 스킬 사용 키
	_bool PositionKeyInput();		// 스킬 사용 키
	void AutoBattleKeyInput();	// 자동 전투 키
	void Battle(int _iNum);		// 전투 알고리즘
	void OffTargetCursor();		// 타겟 커서 다 끄기
	void OnTurnUi();			// 턴 ui 다 키기

private:
	vector<shared_ptr<CGameObject>> m_vCreatures;		// 영웅, 몬스터 포함 리스트
	vector<shared_ptr<CGameObject>> m_vHeroes;			// 영웅 리스트		0: 제일 앞
	vector<shared_ptr<CGameObject>> m_vMonsters;		// 몬스터 리스트


	//공격시 다가가는 거리 
	_vec3	m_vApproachingGapR = _vec3{ -2.f, 0.f, -1.f };
	_vec3	m_vApproachingGapL = _vec3{ 2.f, 0.f, -1.f };

	_int m_iTurn = 1;	// 현재 턴
	shared_ptr<CGameObject>			m_pCurrentCreature = nullptr;

	_float  m_fBattleTime = BATTLEINTERVEL;		// 매 크리처 턴사이 간격
	_float	m_fDeathMovingTime = DEATHMOVINGINTERVEL;		// 크리처 죽으면 옮기는 시간
	_float	m_fSkillMovingTime = SKILLMOVINGINTERVEL;		// 스킬로 인한 움직이는 시간
	_float	m_fSkillMoveTime = SKILLMOVEINTERVEL;				// 스킬로 스스로 움직이는 시간
	_float	m_fAttackSkillMovingTime = ATTACKSKILLMOVINGINTERVEL;		// 스킬로 인한 움직이는 시간
	_float	m_fBackMovingTime = MOVINGBACKINTERVEL;		// 스킬 이후 돌아가는 시간
	_float	m_fWhileAttackingTime = WHILEATTACKINTERVEL;	// 멈춰서 스킬 사용 시간
	_float	m_fSwitchingTime = SWITCHINGINTERVEL;	// 자리 교체 시간
	_float	m_fStressEventTime = STRESSEVENTINTERVEL;	// 스트레스 이벤트 시간
	_float	m_fStressOutputTime = STRESSOUTPUTINTERVEL; // 스트레스 영웅 이벤트 시간
	_float	m_fKeyInputIntervel = KEYINPUTINTERVEL;

	_float	m_fDotDamageEffectTime = DOTDAMEFFECTTIME;
	_float	m_fStressEffectTime = STRESSEFFECTTIME * 2.f;
	
	_bool	m_bNext = false;
	_bool	m_bDeathMoving = false;			// 사망으로 인한 이동
	_bool	m_bSkillMoving = false;			// 기술로 인한 이동
	_bool	m_bSkillMove = false;			// 기술로 인한 스스로의 이동
	_bool	m_bAttackSkillMoving = false;	// 공격 스킬 시전중에 다가가는 이동
	_bool	m_bBackMoving = false;			// 공격 스킬 시전 이후에 제자리로 돌아가는 이동
	_bool	m_bSwitching = false;			// 자료 교체 이후에 서로 교체되는 이동
	_bool	m_bWhileAttack = false;			// 공격 스킬을 사용하는 중
	_bool	m_bWhileStressEvent = false;	// 스트레스 이벤트가 발생중
	_bool	m_bWhileStressOutput = false;	// 스트레스 영웅 이벤트 발생중
	_bool	m_bHero = false;				// 공격 주체가 영웅인지?
	_bool	m_bCounting = false;
	_bool	m_bDeadCheck = false;

	_bool	m_bWhileDotDamEffectRender = false;	// 도트뎀 이펙트가 뜨는 시간
	_bool	m_bWhileStunEffectRender = false;	// 기절 이펙트가 뜨는 시간
	_bool	m_bWhileStressEffectRender = false;	// 스트레스 이펙트가 뜨는 시간
	_bool	m_bStressEffectRender = false;	// 스트레스 이펙트가 출력되었는지

	// 스트레스가 출력될 영웅 목록
	vector<shared_ptr<CGameObject>> m_vStressTargetHeroes;
	_int	m_iStressValue = 0;

	_bool	m_bCalcBlight = false;
	_bool	m_bCalcBleed = false;
	_bool	m_bCalcStun = false;
	_bool	m_bCalcBuff = false;

	_bool	m_bAutoBattle = false;
	_bool	m_bSkillInput = false;
	_bool	m_bPositionInput = false;
	_bool	m_bAblePickingInput = false;
	_int	m_iSelectSkill = 0;
	_int	m_iSelectPosition = 0;
	_bool	m_bCalculate = false;
	_bool	m_bUISetDone = false;

	_bool	m_bBattle = false;

	_bool	m_bEndBattleStart = false;
	_bool	m_bBattleEndEffectOn = false;
	_bool	m_bBossEventStart = false;
	_bool	m_bBossBattleEndEffectOn = false;

	_bool	m_bStressOutputEffectRender = false;


	vector<_vec3> m_vHeroLocation = {};			// hero position
	vector<_vec3> m_vMonsterLocation = {};		// monster position
	_int iCurrentHeroIndex = 0;
	_int iCurrentMonsterIndex = 0;

	_int iStressOutputType[4] = { 0 };
		// 0: selfish, 1: irrational, 2: COURAGEOUS, 3: VIGOROUS

	_vec3		  m_vCenterPos = { 0.f, 0.f, 0.f };

	//Camera Effect Debounce State
	_bool	m_bCamEffectCheck = false;
	_bool	m_bRotationCheck = false;
	_float	m_fAngle = 0.f;
	_float	m_fActTime = 0.f;

	float	m_fEndTime1 = 8.f;
	float	m_fEndTime2 = 15.f;
	//공격이후 애니메이션이 풀리고 턴 리셋을 위해 기록하는 시간
	_float	m_fTurnResetTime = 0.f;
	//스킬 사용이후 텍스쳐(애니메이션)이 변경했는지 여부
	_bool	m_bChangeSkillAnim = false;
	//특정 시간 이후 셰이킹 해야하는 경우 
	_bool	m_bShakingDone = true;
	

	// 피킹오브젝트
	vector<shared_ptr<CGameObject>>		m_vecPickingObject = {};

	_int	m_iBattleType = 0; // 0: 일반 전투  1: 보스 전투

//UI Test
private:
	shared_ptr<CBattleHeroUI> m_pHeroUI = nullptr;
	
	LPDIRECT3DDEVICE9 m_pGraphicDev = nullptr;


	_int iLiveHeroes = 0;
	_int iLiveMonsters = 0;
};

