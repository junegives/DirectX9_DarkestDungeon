#pragma once
#include "Creature.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;
class CAnimator;

END

enum class ESunkenState
{
	IDLE,
	ATTACK,

	ENUM_END
};

class CSunken : public CCreature
{
public:
	explicit CSunken(LPDIRECT3DDEVICE9 pGraphicDev);

	// ��ų���� ������ ��
	CSunken(LPDIRECT3DDEVICE9 pGraphicDev, STAT _tCommonStat, _int _iPosition,
		vector<shared_ptr<CSkill>>& _pVecSkill);

	// ���� ��ġ�� ��ų�� ���߿� ������ ��	
	CSunken(LPDIRECT3DDEVICE9 pGraphicDev, STAT _tCommonStat);

	CSunken(const CCreature& rhs);
	virtual ~CSunken();

protected:
	virtual void		PickingObj();

public:
	virtual HRESULT			ReadyGameObject();
	virtual _int			UpdateGameObject(const _float& fTimeDelta);
	virtual void			LateUpdateGameObject();
	virtual void			RenderGameObject();

public:
	void	SetAnimState(ESunkenState _State) { m_eCurAnimState = _State; }

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
	_bool	m_bMakeEnable = false;

	_bool	m_bReady = false;
	_bool	m_bAttack = false;

	float	m_fIdleTime = 5.f;
	float	m_fAttackTime = 0.12f * 13;

	ESunkenState m_eAnimState = ESunkenState::IDLE;	// �ִϸ��̼� ������ ���� ���°�
	ESunkenState m_eCurAnimState = ESunkenState::IDLE;	// �ִϸ��̼� ������ ���� ���°�
	ESunkenState m_ePrevAnimState = ESunkenState::IDLE;	// �ִϸ��̼� ������ ���� ���°�
	tstring		m_strAnimKey = L"";				// �ִϸ��̼� Ű
	tstring		m_strEffectKey = L"";				// ����Ʈ Ű

	// �÷��̾��� ��ġ�� �޾ƿ��� ���� �÷��̾� ������Ʈ
	shared_ptr<CGameObject>	m_pPlayer = nullptr;
	shared_ptr<CTransform>	m_pPlayerTransformCom = nullptr;

	_bool	m_bCollsion = false;
};

