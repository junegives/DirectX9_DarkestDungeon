#pragma once

#include"GameObject.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;
class CAnimator;

END

enum class EBullet1State
{
	IDLE,
	SHOOT,

	ENUM_END
};

class CBullet1 : public CGameObject
{
public:
	CBullet1(LPDIRECT3DDEVICE9 pGraphicDev);
	CBullet1(const CBullet1& rhs);
	virtual ~CBullet1();

public:
	virtual HRESULT			ReadyGameObject();
	virtual _int			UpdateGameObject(const _float& fTimeDelta);
	virtual void			LateUpdateGameObject();
	virtual void			RenderGameObject();

protected:
	virtual void			AddComponent();
	virtual void			OnCollide(shared_ptr<CGameObject> _pObj) override;

	virtual void			FSM(const _float& fTimeDelta);
	virtual void			ChangeAnim();
	virtual void			AnimDuration(const _float& fTimeDelta);
	virtual void			KeyInput();

protected:
	// �÷��̾�� Object�� �Ÿ��� ����ϴ� �Լ�
	_float	CalcDistance();
	_float	CalcDistance(_vec3 _Pos);
	// �÷��̾�� Object�� �Ÿ��� ���͸� ���ϴ� �Լ�
	_vec3	CalcDirection();
	// Ư�� ��ġ�� Object�� �Ÿ��� ���͸� ���ϴ� �Լ�
	_vec3	CalcDirection(_vec3 _Pos);
	void	FireToPosition(const _float& fTimeDelta, float _fSpeed, _vec3 _Pos);
	void	FireToTarget(const _float& fTimeDelta, float _fSpeed, _vec3& _vDir);
	// �÷��̾����� ����ź ���
	void	ChasePlayer(const _float& fTimeDelta, float _fSpeed);
	_bool	OutArea();

public:
	void	SetTarget(_vec3 _vPos) { m_vTargetPos = _vPos; }
	void	SetTargetDir(_vec3 _vDir) { m_vTargetDir = _vDir; }
	void	SetOnCollision(_bool _bOn) { m_bCollsion = _bOn; }

protected:
	_vec3	m_vTargetPos;
	_vec3	m_vTargetDir;

	_float	m_fShootSpeed = 150.f;

	float	m_fAttackTime = 0.06f * 9;

	EBullet1State m_eAnimState = EBullet1State::IDLE;	// �ִϸ��̼� ������ ���� ���°�
	EBullet1State m_eCurAnimState = EBullet1State::IDLE;	// �ִϸ��̼� ������ ���� ���°�
	EBullet1State m_ePrevAnimState = EBullet1State::IDLE;	// �ִϸ��̼� ������ ���� ���°�
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