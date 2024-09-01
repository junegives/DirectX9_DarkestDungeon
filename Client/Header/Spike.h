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
class CLaser;

enum class ESpikeState
{
	IDLE,
	READY,
	ATTACK,

	ENUM_END
};

class CSpike : public CCreature
{
public:
	explicit CSpike(LPDIRECT3DDEVICE9 pGraphicDev);

	// 스킬까지 세팅할 때
	CSpike(LPDIRECT3DDEVICE9 pGraphicDev, STAT _tCommonStat, _int _iPosition,
		vector<shared_ptr<CSkill>>& _pVecSkill);

	// 전투 위치와 스킬은 나중에 세팅할 때	
	CSpike(LPDIRECT3DDEVICE9 pGraphicDev, STAT _tCommonStat);

	CSpike(const CCreature& rhs);
	virtual ~CSpike();

protected:
	virtual void		PickingObj();

public:
	virtual HRESULT			ReadyGameObject();
	virtual _int			UpdateGameObject(const _float& fTimeDelta);
	virtual void			LateUpdateGameObject();
	virtual void			RenderGameObject();

public:
	void	SetAnimState(ESpikeState _State) { m_eCurAnimState = _State; }

protected:
	virtual void			AddComponent();
	virtual void			OnCollide(shared_ptr<CGameObject> _pObj) override;

	virtual void			FSM(const _float& fTimeDelta);
	virtual void			ChangeAnim();
	virtual void			AnimDuration(const _float& fTimeDelta);
	virtual void			KeyInput();

protected:

public:

protected:
	_bool	m_bReady = false;
	_bool	m_bAttack = false;

	float	m_fIdleTime = 5.f;
	float	m_fReadyTime = 1.f;
	float	m_fAttackTime = 0.12f * 17;

	_int	m_iAttackCount = 3;

	ESpikeState m_eAnimState = ESpikeState::ATTACK;	// 애니메이션 변경을 위한 상태값
	ESpikeState m_eCurAnimState = ESpikeState::ATTACK;	// 애니메이션 변경을 위한 상태값
	ESpikeState m_ePrevAnimState = ESpikeState::ATTACK;	// 애니메이션 변경을 위한 상태값
	tstring		m_strAnimKey = L"";				// 애니메이션 키
	tstring		m_strEffectKey = L"";				// 이펙트 키

	// 플레이어의 위치를 받아오기 위한 플레이어 오브젝트
	shared_ptr<CGameObject>	m_pPlayer = nullptr;
	shared_ptr<CTransform>	m_pPlayerTransformCom = nullptr;

	_bool	m_bCollsion = false;
};

