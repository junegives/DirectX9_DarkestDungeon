#pragma once

#include"GameObject.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;
class CAnimator;

END

enum class ELaserState
{
	IDLE,
	SHOOT,

	ENUM_END
};

class CLaser : public CGameObject
{
public:
	CLaser(LPDIRECT3DDEVICE9 pGraphicDev);
	CLaser(const CLaser& rhs);
	virtual ~CLaser();

public:
	virtual HRESULT			ReadyGameObject();
	virtual _int			UpdateGameObject(const _float& fTimeDelta);
	virtual void			LateUpdateGameObject();
	virtual void			RenderGameObject();

public:
	void	SetTarget(_vec3 _vPos) { m_vTargetPos = _vPos; }
	void	SetStart(_vec3 _vPos) { m_vStartPos = _vPos; }
	void	SetBossAngle(_vec3 _vAngle) { m_vBossAngle = _vAngle; }

protected:
	virtual void			AddComponent();

	virtual void			FSM(const _float& fTimeDelta);
	virtual void			ChangeAnim();
	virtual void			AnimDuration(const _float& fTimeDelta);
	virtual void			KeyInput();

protected:
	// Ư�� ��ġ�� Object�� �Ÿ��� ����ϴ� �Լ�
	_float	CalcDistance(_vec3 _Pos);
	// Ư�� ��ġ�� Object�� �Ÿ��� ���͸� ���ϴ� �Լ�
	_vec3	CalcDirection(_vec3 _Pos);
	void	FireToPosition(const _float& fTimeDelta, float _fSpeed, _vec3 _Pos);

public:


protected:
	_vec3	m_vTargetPos;
	_vec3	m_vStartPos;

	_vec3	m_vBossAngle;

	_float	m_fShootSpeed = 30.f;

	float	m_fAttackTime = 2.f;

	ELaserState m_eAnimState = ELaserState::IDLE;	// �ִϸ��̼� ������ ���� ���°�
	ELaserState m_eCurAnimState = ELaserState::IDLE;	// �ִϸ��̼� ������ ���� ���°�
	ELaserState m_ePrevAnimState = ELaserState::IDLE;	// �ִϸ��̼� ������ ���� ���°�
	tstring		m_strAnimKey = L"";				// �ִϸ��̼� Ű
	tstring		m_strEffectKey = L"";				// ����Ʈ Ű

	// ��ü ���� Component
	shared_ptr<CTransform>	m_pTransformCom = nullptr;
	shared_ptr<CRcTex>		m_pBufCom = nullptr;
	shared_ptr<CAnimator>	m_pTextureCom = nullptr;

	// ��ü �ǰ�, Ÿ�� ����Ʈ ���� Component
	shared_ptr<CTransform> m_pEffectTransformCom = nullptr;
	shared_ptr<CRcTex> m_pEffectBufCom = nullptr;
	shared_ptr<CAnimator> m_pEffectTextureCom = nullptr;

	// �÷��̾��� ��ġ�� �޾ƿ��� ���� �÷��̾� ������Ʈ
	shared_ptr<CGameObject>	m_pPlayer = nullptr;
	shared_ptr<CTransform>	m_pPlayerTransformCom = nullptr;
};