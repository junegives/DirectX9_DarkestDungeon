#pragma once

#include"GameObject.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;
class CAnimator;

END

enum class EMobBulletState
{
	IDLE,
	SHOOT,

	ENUM_END
};

class CMobBullet : public CGameObject
{
public:
	CMobBullet(LPDIRECT3DDEVICE9 pGraphicDev);
	CMobBullet(const CMobBullet& rhs);
	virtual ~CMobBullet();

public:
	virtual HRESULT			ReadyGameObject();
	virtual _int			UpdateGameObject(const _float& fTimeDelta);
	virtual void			LateUpdateGameObject();
	virtual void			RenderGameObject();

public:
	void	SetTarget(_vec3 _vPos) { m_vTargetPos = _vPos; }
	void	SetOnCollision(_bool _bOn) { m_bCollsion = _bOn; }

protected:
	virtual void			AddComponent();
	virtual void			OnCollide(shared_ptr<CGameObject> _pObj) override;

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

	_bool	m_bMakeEnable = false;

	_float	m_fShootSpeed = 100.f;

	float	m_fAttackTime = 0.06f * 9;

	EMobBulletState m_eAnimState = EMobBulletState::IDLE;	// �ִϸ��̼� ������ ���� ���°�
	EMobBulletState m_eCurAnimState = EMobBulletState::IDLE;	// �ִϸ��̼� ������ ���� ���°�
	EMobBulletState m_ePrevAnimState = EMobBulletState::IDLE;	// �ִϸ��̼� ������ ���� ���°�
	tstring		m_strAnimKey = L"";				// �ִϸ��̼� Ű
	tstring		m_strEffectKey = L"";				// ����Ʈ Ű
	tstring m_strEffectAnimKey;

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

	_bool	m_bCollsion = false;
};