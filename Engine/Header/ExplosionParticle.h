#pragma once
#include "ParticleSystem.h"

BEGIN(Engine)

class ENGINE_DLL CExplosionParticle : public CParticleSystem
{
public:
	explicit CExplosionParticle();
	virtual ~CExplosionParticle();

public:
	virtual _int UpdateGameObject(const _float& fTimeDelta) override;
	virtual void RenderGameObject() override;

	virtual void PreRender() override;
	virtual void PostRender() override;

	void	ResetParticle(PARTICLE_ATTRIBUTE* attribute);
};

END