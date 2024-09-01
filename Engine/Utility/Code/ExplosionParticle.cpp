#include "ExplosionParticle.h"
#include "Renderer.h"

CExplosionParticle::CExplosionParticle()
{
	m_fSize = 2.f;
	m_vbSize = 2048;
	m_vbOffset = 0;
	m_vbBatchSize = 512;
}

CExplosionParticle::~CExplosionParticle()
{
}

_int CExplosionParticle::UpdateGameObject(const _float& fTimeDelta)
{
	list<PARTICLE_ATTRIBUTE>::iterator i;

	for (i = m_listParticles.begin(); i != m_listParticles.end(); i++)
	{
		if (i->bIsAlive)
		{
			i->vVelocity.x -= GetRandomFloat(0.9f, 1.f) * fTimeDelta;
			i->vVelocity.z -= GetRandomFloat(0.9f, 1.f) * fTimeDelta;
			i->vVelocity.y += -9.8f * fTimeDelta * GetRandomFloat(-3.f, 10.f);
			i->vPosition += i->vVelocity * fTimeDelta;

			i->fAge += fTimeDelta;

			if (i->fAge > i->fLifeTime)
				i->bIsAlive = false;
		}
	}

	return S_OK;
}

void CExplosionParticle::RenderGameObject()
{
	__super::RenderGameObject();
}

void CExplosionParticle::PreRender()
{
	__super::PreRender();

	/*m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);*/

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, false);
}

void CExplosionParticle::PostRender()
{
	__super::PostRender();

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, true);
}

void CExplosionParticle::ResetParticle(PARTICLE_ATTRIBUTE* attribute)
{
	attribute->bIsAlive = true;
	attribute->vPosition = m_vOrigin;

	_vec3 min = _vec3(-1.f, 0.f, -1.f);
	_vec3 max = _vec3(1.f, 1.f, 1.f);

	GetRandomVector(
		&attribute->vVelocity,
		&min,
		&max);

	D3DXVec3Normalize(
		&attribute->vVelocity,
		&attribute->vVelocity);

	attribute->vVelocity *= 10.f;

	/*attribute->tColor = D3DXCOLOR(
		GetRandomFloat(0.f, 1.f),
		GetRandomFloat(0.f, 1.f),
		GetRandomFloat(0.f, 1.f),
		1.f);*/

	attribute->fAge = 0.f;
	attribute->fLifeTime = 1.5f;
}
