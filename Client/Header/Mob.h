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
class CMobBullet;
class CLaser;

enum class EMobState
{
	// Phase1
	IDLE,
	ATTACK,
	GO_IN,
	GO_OUT,
	DEATH,

	ENUM_END
};

class CMob : public CCreature
{
public:
	explicit CMob(LPDIRECT3DDEVICE9 pGraphicDev);

	// 스킬까지 세팅할 때
	CMob(LPDIRECT3DDEVICE9 pGraphicDev, STAT _tCommonStat, _int _iPosition,
		vector<shared_ptr<CSkill>>& _pVecSkill);

	// 전투 위치와 스킬은 나중에 세팅할 때	
	CMob(LPDIRECT3DDEVICE9 pGraphicDev, STAT _tCommonStat);

	CMob(const CCreature& rhs);
	virtual ~CMob();

protected:
	virtual void		PickingObj();

public:
	virtual HRESULT			ReadyGameObject();
	virtual _int			UpdateGameObject(const _float& fTimeDelta);
	virtual void			LateUpdateGameObject();
	virtual void			RenderGameObject();

public:


protected:
	virtual void			AddComponent();
	virtual void			OnCollide(shared_ptr<CGameObject> _pObj) override;

	virtual void			FSM(const _float& fTimeDelta);
	virtual void			ChangeAnim();
	virtual void			AnimDuration(const _float& fTimeDelta);
	virtual void			KeyInput();

	void	DecreaseHp(_int _iHp) { m_iHp -= _iHp; }

protected:
	// 플레이어와 Object의 거리를 계산하는 함수
	_float	CalcDistance();
	// 플레이어와 Object의 거리차 벡터를 구하는 함수
	_vec3	CalcDirection();
	void	ChasePlayer(const _float& fTimeDelta, float _fSpeed);
	void	ShootBullet1();

public:
	void	SetBullet1(vector<shared_ptr<CMobBullet>>& _vVec)
	{
		m_pVecBullet1 = _vVec;
	}

protected:
	vector<shared_ptr<CMobBullet>> m_pVecBullet1;

	_int	m_iHp = 20;

	_bool	m_bIdle = true;
	_bool	m_bIn = false;
	_bool	m_bOut = false;
	_bool	m_bDeathMob = false;
	_bool	m_bAttack = false;

	_bool	m_bMakeEnable = false;
	
	_int	m_iBullet1TotalNum = 100;

	float	m_fIdleTime = 1.f;
	float	m_fAttackTime = 0.12f * 11;
	float	m_fInTime = 0.12f * 9;
	float	m_fOutTime = 0.12f * 7;
	float	m_fDeathTime = 0.12f * 11;

	float	m_fAttackIntervel = 0.12f * 8;

	_int	m_iAttackCount = 1000;

	EMobState m_eAnimState = EMobState::GO_OUT;	// 애니메이션 변경을 위한 상태값
	EMobState m_eCurAnimState = EMobState::GO_OUT;	// 애니메이션 변경을 위한 상태값
	EMobState m_ePrevAnimState = EMobState::GO_OUT;	// 애니메이션 변경을 위한 상태값
	tstring		m_strAnimKey = L"";				// 애니메이션 키
	tstring		m_strEffectKey = L"";				// 이펙트 키

	// 플레이어의 위치를 받아오기 위한 플레이어 오브젝트
	shared_ptr<CGameObject>	m_pPlayer = nullptr;
	shared_ptr<CTransform>	m_pPlayerTransformCom = nullptr;
};

