#include "GlowParticle.h"
#include "Renderer.h"

CGlowParticle::CGlowParticle()
{
	m_fSize = 0.1f;
	m_vbSize = 2048;
	m_vbOffset = 0;
	m_vbBatchSize = 512;
}

CGlowParticle::~CGlowParticle()
{
}

_int CGlowParticle::UpdateGameObject(const _float& fTimeDelta)
{
	list<PARTICLE_ATTRIBUTE>::iterator i;

	for (i = m_listParticles.begin(); i != m_listParticles.end(); i++)
	{
		i->vPosition += i->vVelocity * fTimeDelta;

		if (m_tBoundingBox.isPointInside(i->vPosition) == false)
		{
			ResetParticle(&(*i));
		}
	}

	return S_OK;
}

void CGlowParticle::RenderGameObject()
{
	__super::RenderGameObject();
}

void CGlowParticle::PreRender()
{
	__super::PreRender();

	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, false);
}

void CGlowParticle::PostRender()
{
	__super::PostRender();

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, true);
}

void CGlowParticle::ResetParticle(PARTICLE_ATTRIBUTE* attribute)
{
	attribute->bIsAlive = true;

	GetRandomVector(
		&attribute->vPosition,
		&m_tBoundingBox._min,
		&m_tBoundingBox._max);

	attribute->vPosition.y = m_tBoundingBox._min.y;

	attribute->vVelocity.x = GetRandomFloat(0.f, 1.f) - 0.5f;
	attribute->vVelocity.y = GetRandomFloat(0.f, 1.f) * 3.f;
	attribute->vVelocity.z = 0.f;
}
