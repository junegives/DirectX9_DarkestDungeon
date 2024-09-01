#pragma once
#include "Engine_Define.h"
#include "GameObject.h"

#include "Animator.h"
#include "Transform.h"
#include "RcTex.h"
#include "Texture.h"

BEGIN(Engine)

class ENGINE_DLL CEffect : public CGameObject
{
public:
	explicit CEffect(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CEffect();

public:
	virtual HRESULT ReadyGameObject() override;
	virtual _int	UpdateGameObject(const _float& fTimeDelta);
	virtual void	LateUpdateGameObject();
	virtual void	RenderGameObject();

public:
	virtual void	AddComponent();

	void	SetNum(_int _iNum) { m_iNum = _iNum; }

	// 애니메이션 설정
	void	SetAnimEffect(tstring _strAnimKey, _vec3 _vPos, _vec3 _vScale, _float _fAnimTime, _bool _bOrthogonal, _bool _bZEnable = true, _bool _bLoop = false);
	void	SetTextureEffect(tstring _strTextureKey, _vec3 _vPos, _vec3 _vScale, _float _fShowTime, _int _iAlpha, _bool _bOrthogonal, _bool _bZEnable = true);

	void	SetSkillEffect(tstring _strAnimKey, _vec2 _vTextureScale, _vec3* _vPos, const _vec3* _vScale, _float _fAnimTime);
	void	SetDamageEffect(_int _iDamageType, _int _iDamage, _vec3* _vPos, _float _fAnimTime);
	void	SetFontEffect(tstring _strAnimKey, _vec3* _vPos, _float _fAnimTime);
	void	SetHeadEffect(tstring _strAnimKey, _vec3* _vPos, _float _fAnimTime, _bool _bLoop, _bool _bShieldBreaker = false);
	void	SetProjEffect(tstring _strAnimKey, _vec3 _vPos, _float _fAnimTime);


	void	SetAnimKey(tstring _strAnimKey) { m_strAnimKey = _strAnimKey; }
	void	SetTime(_float _fAnimTime) { m_fAnimTime = _fAnimTime; }
	void	SetLoop(_bool _bLoop) { m_bLoop = _bLoop; }

	// Transform 설정
	void	SetPos(_vec3* _vPos);
	void	SetScale(const _vec3* _vScale);

public:
	void	SetMove(_bool _bMove, _vec3 _vDir, _float _fTime);
	void	SetFade(_bool _bFade, _bool _bAppear, _float _fTime);
	_bool	GetFade() { return m_bFading; }

	void	Move();
	void	Fade();

	void	Reset();

private:
	shared_ptr<CTransform>	m_pTransformCom;
	shared_ptr<CTransform>	m_pTransformCom2;
	shared_ptr<CAnimator>	m_pAnimatorCom;
	shared_ptr<CRcTex>		m_pRcTexCom;

	shared_ptr<CTexture>	m_pTextureCom1;
	shared_ptr<CTexture>	m_pTextureCom2;

private:
	// 이펙트 식별 번호
	_int	m_iNum;

	_float	m_fDeltaTime;

	// 이펙트를 호출한 객체의 포지션
	_vec3* m_vPos;
	// 이펙트를 호출한 객체의 스케일
	//const _vec3*	m_vScale;
	// 객체와 상관없이 사용할 포지션
	_vec3	m_vPosOrigin;
	// 객체와 상관없이 사용할 스케일
	_vec3	m_vScale;

	// 이펙트 출력을 위한 갭 (포지션, 스케일 차이 세팅)
	_vec3	m_vPosGap = { 0.f, 0.f, 0.f };
	_vec3	m_vScaleGap = { 0.f, 0.f, 0.f };

	// 애니메이션 키
	tstring	m_strAnimKey;
	// 애니메이션 동작 속도
	_float	m_fAnimTime;
	// 반복 여부
	_bool	m_bLoop;
	// 투명도
	_int	m_iAlpha = 255;

	// 특정 기능 누적 시간
	_float	m_fAccumMoveTime = 0.f;
	_float	m_fAccumFadeTime = 0.f;

	// 특정 기능 종료 시간
	_float	m_fMoveTime = 0.f;
	_float	m_fFadeTime = 0.f;

	// 특정 기능 상태
	_bool	m_bMoving = false;
	_bool	m_bFading = false;

	// 무브 방향
	_vec3	m_vMoveDir;

	// 페이드 인 or 아웃
	_bool	m_bAppear;

	// 애니메이션인지 or 텍스처인지
	_bool	m_bAnimation;
	// 텍스처 1개 or 2개
	_bool	m_bTwoTexture;

	// 직교 투영인지
	_bool	m_bOrthogonal = false;

	// 뒤에서 볼 시 트랜스폼 2개 자리 뒤집기
	_bool	m_bChange = false;

	// z정렬 할지말지
	_bool	m_bZEnable = false;
};

END