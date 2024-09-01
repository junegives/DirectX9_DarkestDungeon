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

	// 시스템 내의 모든 파티클 속성을 리셋하는 함수
	virtual void	Reset();
	virtual void	SettingOrigin(_vec3* _vOrigin, _int _iNumParticles);
	virtual void	SettingBounding(BOUNDING_BOX* _tBoundingBox, _vec3* _vPos, _int _iNumParticles);
	virtual void	SettingBoundingSphere(BOUNDING_SPHERE* _tBoundingSphere, _vec3 _vPos, _int _iNumParticles);

	
	// 한 파티클의 속성을 리셋하는 함수
	virtual void	ResetParticle(PARTICLE_ATTRIBUTE* attribute);

	// 시스템에 파티클을 추가하는 함수
	virtual void	AddParticle();

	// 모든 파티클이 죽었는지 확인하는 함수
	_bool			IsDead();
	_bool			IsEmpty();

	// 파티클 시스템의 파티클을 모두 제거하는 함수
	virtual void	RemoveAllParticles();

protected:
	// 죽은 파티클을 리스트에서 제거하는 함수
	virtual void	RemoveDeadParticles();

	// 랜덤 실수, 벡터 생성
	_float			GetRandomFloat(_float _fLowBound, _float _fHighBound);
	void			GetRandomVector(_vec3* out, _vec3* min, _vec3* max);

	DWORD			FtoDw(_float f)
	{
		return *((DWORD*)&f);
	}

protected:
	_int			m_iNum;		// 파티클 매니저 내의 관리 번호

	_vec3			m_vOrigin;	// 파티클 시작 위치
	BOUNDING_BOX	m_tBoundingBox;	// 파티클 유지 경계
	BOUNDING_SPHERE m_tBoundingShpere;	// 파티클 유지 경계 구체
	_float			m_fEmitRate;	// 초당 생성되는 파티클 수
	_float			m_fSize;		// 파티클 크기

	IDirect3DTexture9*			m_pTex;
	IDirect3DVertexBuffer9*		m_pVB;
	list<PARTICLE_ATTRIBUTE>	m_listParticles;
	_int			m_iMaxParticles;

	DWORD			m_vbSize;		// 버텍스 버퍼가 보관할 수 있는 파티클 수
	DWORD			m_vbOffset;		// 버텍스 버퍼에서 복사를 시작할 파티클 내 다음 단계로의 오프셋 (파티클 단위)
	DWORD			m_vbBatchSize;

	_bool	m_bOrthogonal = false;

protected:
	shared_ptr<CTransform> m_pTransformCom = nullptr;
	shared_ptr<CTexture> m_pTextureCom = nullptr;
	shared_ptr<CAnimator> m_pAnimatorCom = nullptr;
};

END