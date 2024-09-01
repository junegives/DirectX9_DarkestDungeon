#pragma once
#include "Creature.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;
class CAnimator;

END

class CBullet1;
class CBullet2;
class CBullet3;
class CLaser;
class CMob;
class CSpike;
class CSunken;
class CVideo;

enum class EBossState
{
	// Phase1
	P1_IDLE,
	P1_TELEPORT,
	P1_PATTERN1,
	P1_PATTERN1FIRE,
	P1_PATTERN2DASHREADY,
	P1_PATTERN2CHARGE,
	P1_PATTERN2DASH,
	P1_PATTERN3LASERREADY,
	P1_PATTERN3LASER,
	P1_PATTERN4SHOTREADY,
	P1_PATTERN4SHOT,
	P1_LASER,
	P1_ATTACK,
	P1_LASER1,
	P1_LASER2,
	P1_DASH,
	P1_CHANGE,
	P1_GROGGY,

	// Phase2
	P2_IDLE,
	P2_SUMMON,
	P2_SPIKE,
	P2_SUNKEN,
	P2_SUNKENALL,
	P2_DEATH,

	ENUM_END
};

class CBoss2 : public CCreature
{
public:
	explicit CBoss2(LPDIRECT3DDEVICE9 pGraphicDev);

	// 스킬까지 세팅할 때
	CBoss2(LPDIRECT3DDEVICE9 pGraphicDev, STAT _tCommonStat, _int _iPosition,
		vector<shared_ptr<CSkill>>& _pVecSkill);

	// 전투 위치와 스킬은 나중에 세팅할 때	
	CBoss2(LPDIRECT3DDEVICE9 pGraphicDev, STAT _tCommonStat);

	CBoss2(const CCreature& rhs);
	virtual ~CBoss2();

protected:
	virtual void		PickingObj();

public:
	virtual HRESULT			ReadyGameObject();
	virtual _int			UpdateGameObject(const _float& fTimeDelta);
	virtual void			LateUpdateGameObject();
	virtual void			RenderGameObject();

public:
	void	SetPattern1Ready(_bool _bReady) { m_bPattern1Ready = _bReady; }

protected:
	virtual void			AddComponent();
	virtual void			OnCollide(shared_ptr<CGameObject> _pObj) override;

	virtual void			FSM(const _float& fTimeDelta);
	virtual void			ChangeAnim();
	virtual void			AnimDuration(const _float& fTimeDelta);
	virtual void			KeyInput();

	// 지면 아래로 못내려가게 하기
	void					OnFloor(_float _fHeight);
	// 텔레포트
	void					Teleport(_vec3 vPos);
	// 영역 벗어났는지 알려주기
	_bool					OutArea();

protected:
	// 플레이어와 Object의 거리를 계산하는 함수
	_float	CalcDistance();
	// 플레이어와 Object의 거리차 벡터를 구하는 함수
	_vec3	CalcDirection();
	void	ChasePlayer(const _float& fTimeDelta, float _fSpeed);
	// 특정 방향으로 돌진
	void	DashTarget(const _float& fTimeDelta, float _fSpeed, _vec3& _vDir);
	// 플레이어한테 유도탄 쏘기
	void	ShootBullet1();
	// 플레이어한테 쏘기
	void	ShootBullet2();
	// 사방으로 쏘기
	void	ShootBullet3();
	// 1페이즈 특정 위치로 쏘기
	void	ShootBullet4();
	// 2페이즈 특정 위치로 쏘기
	void	ShootBullet5();

	void	ShootLaser();

	void	DecreaseHp(_int _iHp) { m_iHp -= _iHp; }

public:
	void	SetBullet1(vector<shared_ptr<CBullet1>>& _vVec)
	{
		m_pVecBullet1 = _vVec;
	}

	void	SetBullet2(vector<shared_ptr<CBullet2>>& _vVec)
	{
		m_pVecBullet2 = _vVec;
	}

	void	SetBullet3(vector<shared_ptr<CBullet3>>& _vVec)
	{
		m_pVecBullet3 = _vVec;
	}

	void	SetLaser(vector<shared_ptr<CLaser>>& _vVec)
	{
		m_pVecLaser = _vVec;
	}

	void	SetMob(vector<shared_ptr<CMob>>& _vVec)
	{
		m_pVecMob = _vVec;
	}

	void	SetSpike(vector<shared_ptr<CSpike>>& _vVec)
	{
		m_pVecSpike = _vVec;
	}

	void	SetSunken(vector<shared_ptr<CSunken>>& _vVec)
	{
		m_pVecSunken = _vVec;
	}

protected:
	vector<shared_ptr<CBullet1>> m_pVecBullet1;
	vector<shared_ptr<CBullet2>> m_pVecBullet2;
	vector<shared_ptr<CBullet3>> m_pVecBullet3;
	vector<shared_ptr<CLaser>> m_pVecLaser;
	// 몹
	vector<shared_ptr<CMob>> m_pVecMob;
	// 스파이크
	vector<shared_ptr<CSpike>> m_pVecSpike;
	// 성큰
	vector<shared_ptr<CSunken>> m_pVecSunken;


	// 가운데
	_vec3	MiddleTop = _vec3(300.f, 80.f, 300.f);
	_vec3	RightTop = _vec3(500.f, 80.f, 500.f);
	_vec3	LeftTop = _vec3(100.f, 80.f, 500.f);
	_vec3	LeftBottom = _vec3(100.f, 80.f, 100.f);
	_vec3	RightBottom = _vec3(500.f, 80.f, 100.f);
	_vec3	Right = _vec3(500.f, 80.f, 300.f);
	_vec3	Left = _vec3(100.f, 80.f, 300.f);
	_vec3	Top = _vec3(300.f, 80.f, 500.f);
	_vec3	Bottom = _vec3(300.f, 80.f, 100.f);

	_vec3	DashDir;

	_int	m_iHp = 200;
	_float	m_fP1_IdleSpeed = 10.f;
	_float	m_fP1_DashSpeed = 700.f;

	_bool	m_bPhase2 = false;

	_bool	m_bBullet1Fire = false;
	_bool	m_bBullet2Fire = false;
	_bool	m_bLaserFire = false;

	_bool	m_bTeleportEffect = false;
	_bool	m_bWhileTeleport = false;
	_bool	m_bDoTeleport = false;

	_bool	m_bIdle = true;
	_bool	m_bPattern1 = false;
	_bool	m_bPattern1Before = false;
	_bool	m_bPattern1Start = false;
	_bool	m_bPattern1Ready = false;
	_bool	m_bGroggy = false;

	_bool	m_bPattern2 = false;
	_bool	m_bPattern2DashReady = false;
	_bool	m_bPattern2DashCharge = false;
	_bool	m_bPattern2Dash = false;

	_bool	m_bPattern3 = false;
	_bool	m_bPattern3LaserReady = false;
	_bool	m_bPattern3Laser = false;

	_bool	m_bPattern4 = false;
	_bool	m_bPattern4ShotReady = false;
	_bool	m_bPattern4Shot = false;

	_int	m_iPattern1Count = 1;	// 2번 순서
	_int	m_iPattern2Count = 2;	// 3번 순서
	_int	m_iPattern4Count = 0;	// 1번 순서
	_bool	m_bPhase1PatternChoose = false;

	_int	m_iPhase1TotalPattern = 3;

	_bool	m_bChange = false;

	_bool	m_bPhase2Idle = false;
	_bool	m_bPhase2Spike2 = false;
	_bool	m_bPhase2Sunken = false;
	
	_bool	m_bPhase2Spike = false;
	_bool	m_bPhase2Summon = false;
	_bool	m_bPahse2SunkenReady = false;
	_bool	m_bPhase2SunkenAll = false;

	_int	m_iPhase2SpikeCount = 3;	// 4번 순서
	_int	m_iPhase2SummonCount = 2;	// 3번 순서
	_int	m_iPhase2SunkenReadyCount = 0;	// 1번 순서
	_int	m_iPhase2SunkenAllCount = 1;	// 2번 순서
	_bool	m_bPhase2PatternChoose = false;

	_int	m_iPhase2TotalPattern = 4;

	_bool	m_bPhase2FireCool = false;
	_bool	m_bPhase2FireReady = false;
	_bool	m_bPhase2Fire = false;

	_bool	m_bPhaseDeath = false;

	_int	m_iBullet1TotalNum = 50;
	_int	m_iBullet2TotalNum = 50;
	_int	m_iLaserTotalNum = 3;
	_int	m_iMobTotalNum = 6;
	_int	m_iSpikeTotalNum = 200;
	_int	m_iSunkenTotalNum = 400;

	_int	m_iSunkenAttackNum = 0;
	_int	m_iSunkenAttackAllNum = 0;
	_int	m_iSummonNum = 3;

	float	m_iSunkentIntervel = 5.f;
	float	m_fIdleTime = 2.f;
	float	m_fPattern1BeforeTime = 1.f;
	float	m_fPattern1ReadyTime = 4.f;
	float	m_fPattern1FireTime = 3.f;
	float	m_fPattern1FireIntervel = 0.15f;
	float	m_fDashReadyTime = 0.25f * 4;
	float	m_fDashChargeTime = 3.f;
	float	m_fDashTime = 0.12f * 11;
	float	m_fPattern3LaserReadyTime = 1.f;
	float	m_fPattern3LaserTime = 0.12 * 25;

	float	m_fPattern4ShotReadyTime = 1.f;
	float	m_fPattern4ShotTime = 3.f;
	float	m_fPattern4FireIntervel = 0.1f;

	float	m_fTeleportWhileTime = 1.2f;

	float	m_fDeadTime = 0.24f * 26;

	float	m_fGroggyTime = 5.f;

	float	m_fP2IdleTime = 2.f;
	float	m_fP2SummonTime = 1.f;
	float	m_fP2SpikeTime = 1.5f;
	float	m_fP2SpikeTime2 = 1.f;
	float	m_fP2SunkenReadyTime = 1.f;
	float	m_fP2SunkenTime = 3.f;
	float	m_fP2SunkenIntervel = 0.1f;
	float	m_fP2SunkenAllIntervel = 0.1f;
	float	m_fP2SunkenAllTime = 3.f;
	
	float	m_fP2FireCoolTime = 10.f;
	float	m_fP2FireReadyTime = 3.f;
	float	m_fP2FireShootTime = 3.f;
	float	m_fP2FireIntervel = 0.15f;

	float	m_fP1AttackTime = 0.06f * 12;
	float	m_fP1Laser1Time = 0.06f * 7;
	float	m_fP1Laser2Time = 0.06f * 4;
	float	m_fP1DashTime = 0.06f * 11;
	float	m_fP1ChangeTime = 0.12f * 43;

	float	m_fBullet1Intervel = 0.5f;
	float	m_fBullet2Intervel = 0.06f;
	float	m_fLaserIntervel = 0.5f;

	float	m_fHittedTime = 0.5f;
	float	m_fHittedIntervel = 0.01f;

	EBossState m_eAnimState = EBossState::P1_IDLE;	// 애니메이션 변경을 위한 상태값
	EBossState m_eCurAnimState = EBossState::P1_IDLE;	// 애니메이션 변경을 위한 상태값
	EBossState m_ePrevAnimState = EBossState::P1_IDLE;	// 애니메이션 변경을 위한 상태값
	tstring		m_strAnimKey = L"";				// 애니메이션 키
	tstring		m_strEffectKey = L"";				// 이펙트 키

	// 플레이어의 위치를 받아오기 위한 플레이어 오브젝트
	shared_ptr<CGameObject>	m_pPlayer = nullptr;
	shared_ptr<CTransform>	m_pPlayerTransformCom = nullptr;
	shared_ptr<CVideo>   m_pVideo;

	_bool	m_bAbleWallCollsion = false;
	_bool	m_bWallCollision = false;
	_bool	m_bHitByPlayer = false;
	_bool	m_bChangeColor = false;

	_bool	m_bSuperArmor = false;

	_int	m_iAlpha = 255;
	_int	m_iRed = 255;
	_int	m_iGreen = 255;
	_int	m_iBlue = 255;

};

