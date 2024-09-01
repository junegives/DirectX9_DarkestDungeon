#include "EffectMgr.h"

IMPLEMENT_SINGLETON(CEffectMgr)

CEffectMgr::CEffectMgr()
{
}

CEffectMgr::~CEffectMgr()
{
}

int CEffectMgr::Update(const float& _fDeltaTime)
{
	for (auto& iter : m_pEffectList)
	{
		if (iter->GetIsActive())
			iter->UpdateGameObject(_fDeltaTime);
	}

	return S_OK;
}

void CEffectMgr::LateUpdate()
{
	for (auto& iter : m_pEffectList)
	{
		if (iter->GetIsActive())
			iter->LateUpdateGameObject();
	}
}

void CEffectMgr::Render(LPDIRECT3DDEVICE9 _pGraphicDev)
{
	//for (auto& iter : m_pEffectList)
	//{
	//	if (iter->GetIsActive())
	//		iter->RenderGameObject();
	//}
}

void CEffectMgr::SetEffectMgr(LPDIRECT3DDEVICE9 _pGraphicDev)
{
	m_pGraphicDev = _pGraphicDev;

	// 이펙트 풀 초기화
	for (int i = 0; i < POOLSIZE; ++i)
	{
		shared_ptr<CEffect> pEffect = make_shared<CEffect>(m_pGraphicDev);
		pEffect->SetActive(false);
		pEffect->AddComponent();
		pEffect->SetNum(i);
		m_pEffectList.push_back(pEffect);
	}
}

shared_ptr<CEffect> CEffectMgr::GetEffect()
{
	for (auto& iter : m_pEffectList)
	{
		if (!iter->GetIsActive())
		{
			return iter;
		}
	}

	return nullptr;
}

void CEffectMgr::ReturnEffect(shared_ptr<CEffect> _pEffect)
{
	// 이펙트 초기화
	_pEffect->Reset();
}
