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

	// ��ų���� ������ ��
	CMob(LPDIRECT3DDEVICE9 pGraphicDev, STAT _tCommonStat, _int _iPosition,
		vector<shared_ptr<CSkill>>& _pVecSkill);

	// ���� ��ġ�� ��ų�� ���߿� ������ ��	
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
	// �÷��̾�� Object�� �Ÿ��� ����ϴ� �Լ�
	_float	CalcDistance();
	// �÷��̾�� Object�� �Ÿ��� ���͸� ���ϴ� �Լ�
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

	EMobState m_eAnimState = EMobState::GO_OUT;	// �ִϸ��̼� ������ ���� ���°�
	EMobState m_eCurAnimState = EMobState::GO_OUT;	// �ִϸ��̼� ������ ���� ���°�
	EMobState m_ePrevAnimState = EMobState::GO_OUT;	// �ִϸ��̼� ������ ���� ���°�
	tstring		m_strAnimKey = L"";				// �ִϸ��̼� Ű
	tstring		m_strEffectKey = L"";				// ����Ʈ Ű

	// �÷��̾��� ��ġ�� �޾ƿ��� ���� �÷��̾� ������Ʈ
	shared_ptr<CGameObject>	m_pPlayer = nullptr;
	shared_ptr<CTransform>	m_pPlayerTransformCom = nullptr;
};

