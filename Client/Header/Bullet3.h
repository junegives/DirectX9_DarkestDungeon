#pragma once

#include"GameObject.h"

BEGIN(Engine)

class CRcTex;
class CTexture;
class CTransform;
class CAnimator;

END

enum class EBullet3State
{
	IDLE,
	SHOOT,

	ENUM_END
};

class CBullet3 : public CGameObject
{
public:
	CBullet3(LPDIRECT3DDEVICE9 pGraphicDev);
	CBullet3(const CBullet3& rhs);
	virtual ~CBullet3();

public:
	virtual HRESULT			ReadyGameObject();
	virtual _int			UpdateGameObject(const _float& fTimeDelta);
	virtual void			LateUpdateGameObject();
	virtual void			RenderGameObject();

public:
	void	SetTarget(_vec3 _vPos) { m_vTargetPos = _vPos; }

protected:
	virtual void			AddComponent();

	virtual void			FSM(const _float& fTimeDelta);
	virtual void			ChangeAnim();
	virtual void			AnimDuration(const _float& fTimeDelta);
	virtual void			KeyInput();

protected:
	// 특정 위치와 Object의 거리를 계산하는 함수
	_float	CalcDistance(_vec3 _Pos);
	// 특정 위치와 Object의 거리차 벡터를 구하는 함수
	_vec3	CalcDirection(_vec3 _Pos);
	void	FireToPosition(const _float& fTimeDelta, float _fSpeed, _vec3 _Pos);
	_bool	OutArea();

public:


protected:
	_vec3	m_vTargetPos;

	_float	m_fShootSpeed = 100.f;

	_float	m_fAliveTime = 6.f;

	float	m_fAttackTime = 0.06f * 9;

	EBullet3State m_eAnimState = EBullet3State::IDLE;	// 애니메이션 변경을 위한 상태값
	EBullet3State m_eCurAnimState = EBullet3State::IDLE;	// 애니메이션 변경을 위한 상태값
	EBullet3State m_ePrevAnimState = EBullet3State::IDLE;	// 애니메이션 변경을 위한 상태값
	tstring		m_strAnimKey = L"";				// 애니메이션 키
	tstring		m_strEffectKey = L"";				// 이펙트 키

	// 본체 관련 Component
	shared_ptr<CTransform>	m_pTransformCom = nullptr;
	shared_ptr<CRcTex>		m_pBufCom = nullptr;
	shared_ptr<CAnimator>	m_pTextureCom = nullptr;

	// 본체 피격, 타격 이펙트 관련 Component
	shared_ptr<CTransform> m_pEffectTransformCom = nullptr;
	shared_ptr<CRcTex> m_pEffectBufCom = nullptr;
	shared_ptr<CAnimator> m_pEffectTextureCom = nullptr;

	// 플레이어의 위치를 받아오기 위한 플레이어 오브젝트
	shared_ptr<CGameObject>	m_pPlayer = nullptr;
	shared_ptr<CTransform>	m_pPlayerTransformCom = nullptr;
};