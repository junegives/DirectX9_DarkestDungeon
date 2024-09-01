#include "ParticleMgr.h"

IMPLEMENT_SINGLETON(CParticleMgr)

CParticleMgr::CParticleMgr()
{
}

CParticleMgr::~CParticleMgr()
{
}

int CParticleMgr::Update(const float& _fDeltaTime)
{
	for (auto& iter : m_pExplosionParticleList)
	{
		if (iter->GetIsActive())
			iter->UpdateGameObject(_fDeltaTime);
	}

	return S_OK;
}

void CParticleMgr::LateUpdate()
{
	for (auto& iter : m_pExplosionParticleList)
	{
		if (iter->GetIsActive())
			iter->LateUpdateGameObject();
	}
}

void CParticleMgr::Render(LPDIRECT3DDEVICE9 _pGraphicDev)
{
}

void CParticleMgr::SetParticleMgr(LPDIRECT3DDEVICE9 _pGraphicDev)
{
	m_pGraphicDev = _pGraphicDev;

	shared_ptr<CParticleSystem> pParticle;

	// ��ƼŬ �ý��� Ǯ �ʱ�ȭ
	for (int i = 0; i < PARTICLE_POOL; ++i)
	{
		pParticle = make_shared<CExplosionParticle>();
		pParticle->SetNum(i);
		pParticle->AddComponent();
		pParticle->Reset();
		pParticle->Init(m_pGraphicDev, L"../Bin/Resource/Image/Particle/sprite_blood_particle 1.png");
		m_pExplosionParticleList.push_back(pParticle);
	}
}

shared_ptr<CParticleSystem> CParticleMgr::GetExplosionParticle()
{
	for (auto& iter : m_pExplosionParticleList)
	{
		if (!iter->GetIsActive())
			return iter;
	}

	return nullptr;
}

void CParticleMgr::ReturnParticle(shared_ptr<CParticleSystem> _pParticle)
{
	// ��ƼŬ �ý��� �ʱ�ȭ
	_pParticle->Reset();
}
