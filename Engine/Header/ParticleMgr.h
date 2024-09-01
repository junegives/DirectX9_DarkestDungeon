#pragma once

#include "Engine_Define.h"
#include "ExplosionParticle.h"

#define PARTICLE_POOL 50

BEGIN(Engine)

class ENGINE_DLL CParticleMgr
{
	DECLARE_SINGLETON(CParticleMgr)

public:
	explicit CParticleMgr();
	virtual ~CParticleMgr();

public:
	int		Update(const float& _fDeltaTime);
	void	LateUpdate();
	void	Render(LPDIRECT3DDEVICE9 _pGraphicDev);

public:
	void	SetParticleMgr(LPDIRECT3DDEVICE9 _pGraphicDev);

public:
	shared_ptr<CParticleSystem> GetExplosionParticle();
	void	ReturnParticle(shared_ptr<CParticleSystem> _pParticle);

private:
	vector<shared_ptr<CParticleSystem>> m_pExplosionParticleList;
	LPDIRECT3DDEVICE9	m_pGraphicDev;
};

END