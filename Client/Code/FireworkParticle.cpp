#include "pch.h"
#include "FireworkParticle.h"

#include "GameMgr.h"
#include "Transform.h"

#include "Renderer.h"

CFireworkParticle::CFireworkParticle()
{
	m_fSize = 5.f;
	m_vbSize = 2048;
	m_vbOffset = 0;
	m_vbBatchSize = 512;
}

void CFireworkParticle::Setting(_vec3* _vOrigin, _int _iNumParticles)
{
	m_vOrigin = *_vOrigin;

	for (int i = 0; i < _iNumParticles; ++i)
		AddParticle();

	m_bOrthogonal = false;
}

CFireworkParticle::~CFireworkParticle()
{
}

_int CFireworkParticle::UpdateGameObject(const _float& fTimeDelta)
{
	if (m_bOrthogonal)
	{
		for (int i = 0; i < 50; ++i)
			AddParticle();
	}

	list<PARTICLE_ATTRIBUTE>::iterator i;

	for (i = m_listParticles.begin(); i != m_listParticles.end(); i++)
	{
		if (i->bIsAlive)
		{
			i->vPosition += i->vVelocity * fTimeDelta;

			i->fAge += fTimeDelta;

			_vec3 vLength = i->vPosition - m_vOrigin;

			if (D3DXVec3Length(&vLength) > m_fWeight)
				i->bIsAlive = false;

			if (i->fAge > i->fLifeTime)
				i->bIsAlive = false;

			i->vVelocity *= GetRandomFloat(0.98f, 0.99f);
		}
	}

	return S_OK;


	//list<PARTICLE_ATTRIBUTE>::iterator i;

	//for (i = m_listParticles.begin(); i != m_listParticles.end(); i++)
	//{
	//	i->vPosition += i->vVelocity * fTimeDelta;

	//	if (m_tBoundingShpere.isPointInside(i->vPosition) == false)
	//	{
	//		ResetParticle(&(*i));
	//	}

	//}
	//return S_OK;
}

void CFireworkParticle::LateUpdateGameObject()
{
	if (IsDead())
	{
		Reset();
		SetActive(false);
		return;
	}

	if (m_bOrthogonal)
		CRenderer::GetInstance()->AddRenderGroup(RENDER_UI, shared_from_this());

	else
		CRenderer::GetInstance()->AddRenderGroup(RENDER_ALPHA, shared_from_this());
}

void CFireworkParticle::RenderGameObject()
{
	printf("ºÒ²É³îÀÌ Ãâ·Â\n");
	printf("%f, %f, %f\n", m_pTransformCom->GetPos()->x, m_pTransformCom->GetPos()->x, m_pTransformCom->GetPos()->z);
	printf("%f, %f, %f\n\n", m_pTransformCom->GetScale()->x, m_pTransformCom->GetScale()->x, m_pTransformCom->GetScale()->z);

	__super::RenderGameObject();
}

void CFireworkParticle::PreRender()
{
	D3DXMATRIX matTriangleWorld;

	_vec3* pPlayerPos = dynamic_pointer_cast<CTransform>(CGameMgr::GetInstance()->GetPlayer()->GetComponent(L"Com_Transform", ID_DYNAMIC))->GetPos();
	__super::PreRender();

	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, false);
	m_pGraphicDev->SetRenderState(D3DRS_POINTSCALEENABLE, false);
}

void CFireworkParticle::PostRender()
{
	__super::PostRender();

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, true);
}

void CFireworkParticle::ResetParticle(PARTICLE_ATTRIBUTE* attribute)
{
	attribute->bIsAlive = true;
	attribute->vPosition = m_vOrigin;

	_vec3 min;
	_vec3 max;

	if (m_bOrthogonal)
	{
		min = _vec3(-1.f, -1.f, 0.f);
		max = _vec3(1.f, 1.f, 0.f);

		m_fWeight = 300.f;
	}

	else
	{
		min = _vec3(-1.f, -1.f, -1.f);
		max = _vec3(1.f, 1.f, 1.f);

		m_fWeight = 100.f;
	}

	GetRandomVector(
		&attribute->vVelocity,
		&min,
		&max);

	D3DXVec3Normalize(
		&attribute->vVelocity,
		&attribute->vVelocity);

	attribute->vVelocity *= m_fWeight;

	attribute->tColor = D3DXCOLOR(
		GetRandomFloat(0.f, 1.f),
		GetRandomFloat(0.f, 1.f),
		GetRandomFloat(0.f, 1.f),
		1.f);

	attribute->fAge = 0.f;
	attribute->fLifeTime = 2.f;
}

void CFireworkParticle::SettingOrthogonal(_vec3 _vOrigin, _vec3 _vScale, _int _iNumParticles)
{
	m_vOrigin = _vOrigin;
	m_vScale = _vScale;

	m_pTransformCom->SetPosition(m_vOrigin.x, m_vOrigin.y, m_vOrigin.z);
	m_pTransformCom->SetScale(m_vScale.x, m_vScale.y, m_vScale.z);

	for (int i = 0; i < _iNumParticles; ++i)
		AddParticle();

	m_bOrthogonal = true;
	m_iNumParticles = _iNumParticles;
}
