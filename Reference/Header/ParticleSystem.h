#pragma once
#include "GameObject.h"

#include "Animator.h"
#include "Transform.h"
#include "Texture.h"

BEGIN(Engine)

class ENGINE_DLL CParticleSystem : public CGameObject
{
public:
	explicit CParticleSystem();
	virtual ~CParticleSystem();

public:
	virtual	void	Init(LPDIRECT3DDEVICE9 _pGraphicDev, tstring strTexFileName);
	virtual _int	UpdateGameObject(const _float& fTimeDelta) override;
	virtual void	LateUpdateGameObject() override;
	virtual void	RenderGameObject() override;

	virtual void	PreRender();
	virtual void	PostRender();

	void			SetNum(_int _iNum) { m_iNum = _iNum; }

public:
	virtual void	AddComponent();

	// �ý��� ���� ��� ��ƼŬ �Ӽ��� �����ϴ� �Լ�
	virtual void	Reset();
	virtual void	SettingOrigin(_vec3* _vOrigin, _int _iNumParticles);
	virtual void	SettingBounding(BOUNDING_BOX* _tBoundingBox, _vec3* _vPos, _int _iNumParticles);
	virtual void	SettingBoundingSphere(BOUNDING_SPHERE* _tBoundingSphere, _vec3 _vPos, _int _iNumParticles);

	
	// �� ��ƼŬ�� �Ӽ��� �����ϴ� �Լ�
	virtual void	ResetParticle(PARTICLE_ATTRIBUTE* attribute);

	// �ý��ۿ� ��ƼŬ�� �߰��ϴ� �Լ�
	virtual void	AddParticle();

	// ��� ��ƼŬ�� �׾����� Ȯ���ϴ� �Լ�
	_bool			IsDead();
	_bool			IsEmpty();

	// ��ƼŬ �ý����� ��ƼŬ�� ��� �����ϴ� �Լ�
	virtual void	RemoveAllParticles();

protected:
	// ���� ��ƼŬ�� ����Ʈ���� �����ϴ� �Լ�
	virtual void	RemoveDeadParticles();

	// ���� �Ǽ�, ���� ����
	_float			GetRandomFloat(_float _fLowBound, _float _fHighBound);
	void			GetRandomVector(_vec3* out, _vec3* min, _vec3* max);

	DWORD			FtoDw(_float f)
	{
		return *((DWORD*)&f);
	}

protected:
	_int			m_iNum;		// ��ƼŬ �Ŵ��� ���� ���� ��ȣ

	_vec3			m_vOrigin;	// ��ƼŬ ���� ��ġ
	BOUNDING_BOX	m_tBoundingBox;	// ��ƼŬ ���� ���
	BOUNDING_SPHERE m_tBoundingShpere;	// ��ƼŬ ���� ��� ��ü
	_float			m_fEmitRate;	// �ʴ� �����Ǵ� ��ƼŬ ��
	_float			m_fSize;		// ��ƼŬ ũ��

	IDirect3DTexture9*			m_pTex;
	IDirect3DVertexBuffer9*		m_pVB;
	list<PARTICLE_ATTRIBUTE>	m_listParticles;
	_int			m_iMaxParticles;

	DWORD			m_vbSize;		// ���ؽ� ���۰� ������ �� �ִ� ��ƼŬ ��
	DWORD			m_vbOffset;		// ���ؽ� ���ۿ��� ���縦 ������ ��ƼŬ �� ���� �ܰ���� ������ (��ƼŬ ����)
	DWORD			m_vbBatchSize;

	_bool	m_bOrthogonal = false;

protected:
	shared_ptr<CTransform> m_pTransformCom = nullptr;
	shared_ptr<CTexture> m_pTextureCom = nullptr;
	shared_ptr<CAnimator> m_pAnimatorCom = nullptr;
};

END