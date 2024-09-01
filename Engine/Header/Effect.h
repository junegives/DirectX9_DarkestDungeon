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

	// �ִϸ��̼� ����
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

	// Transform ����
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
	// ����Ʈ �ĺ� ��ȣ
	_int	m_iNum;

	_float	m_fDeltaTime;

	// ����Ʈ�� ȣ���� ��ü�� ������
	_vec3* m_vPos;
	// ����Ʈ�� ȣ���� ��ü�� ������
	//const _vec3*	m_vScale;
	// ��ü�� ������� ����� ������
	_vec3	m_vPosOrigin;
	// ��ü�� ������� ����� ������
	_vec3	m_vScale;

	// ����Ʈ ����� ���� �� (������, ������ ���� ����)
	_vec3	m_vPosGap = { 0.f, 0.f, 0.f };
	_vec3	m_vScaleGap = { 0.f, 0.f, 0.f };

	// �ִϸ��̼� Ű
	tstring	m_strAnimKey;
	// �ִϸ��̼� ���� �ӵ�
	_float	m_fAnimTime;
	// �ݺ� ����
	_bool	m_bLoop;
	// ����
	_int	m_iAlpha = 255;

	// Ư�� ��� ���� �ð�
	_float	m_fAccumMoveTime = 0.f;
	_float	m_fAccumFadeTime = 0.f;

	// Ư�� ��� ���� �ð�
	_float	m_fMoveTime = 0.f;
	_float	m_fFadeTime = 0.f;

	// Ư�� ��� ����
	_bool	m_bMoving = false;
	_bool	m_bFading = false;

	// ���� ����
	_vec3	m_vMoveDir;

	// ���̵� �� or �ƿ�
	_bool	m_bAppear;

	// �ִϸ��̼����� or �ؽ�ó����
	_bool	m_bAnimation;
	// �ؽ�ó 1�� or 2��
	_bool	m_bTwoTexture;

	// ���� ��������
	_bool	m_bOrthogonal = false;

	// �ڿ��� �� �� Ʈ������ 2�� �ڸ� ������
	_bool	m_bChange = false;

	// z���� ��������
	_bool	m_bZEnable = false;
};

END