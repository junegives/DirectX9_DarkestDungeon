#pragma once
#include "ParticleSystem.h"

BEGIN(Engine)

class ENGINE_DLL CGlowParticle : public CParticleSystem
{
public:
	explicit CGlowParticle();
	virtual ~CGlowParticle();

public:
	virtual _int UpdateGameObject(const _float& fTimeDelta) override;
	virtual void RenderGameObject() override;

	virtual void PreRender() override;
	virtual void PostRender() override;

	void	ResetParticle(PARTICLE_ATTRIBUTE* attribute);
};

END