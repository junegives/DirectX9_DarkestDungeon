#include "Scene.h"
#include "CollisionMgr.h"
#include "PickingMgr.h"
#include "EffectMgr.h"
#include "ParticleMgr.h"

#include"LightMgr.h"

CScene::CScene(LPDIRECT3DDEVICE9 _pGraphicDev) : m_pGraphicDev(_pGraphicDev)
{
}

CScene::~CScene()
{
}

HRESULT CScene::ReadyScene()
{
	return S_OK;
}

_int CScene::UpdateScene(const _float& _fTimeDelta)
{
	KeyInput();

	_int		iResult(0);

	for (auto& iter : m_mapLayer)
	{
		iResult = iter.second->UpdateLayer(_fTimeDelta);

		if (iResult & 0x80000000)
			return iResult;
	}

	CEffectMgr::GetInstance()->Update(_fTimeDelta);
	CParticleMgr::GetInstance()->Update(_fTimeDelta);

	return _int();
}

void CScene::LateUpdateScene()
{
	for (auto& iter : m_mapLayer)
		iter.second->LateUpdateLayer();

	CCollisionMgr::GetInstance()->CheckCollisionList(ECollideID::PLAYER_PROJECTILE, ECollideID::WALL);
	CCollisionMgr::GetInstance()->CheckCollisionList(ECollideID::PLAYER_PROJECTILE, ECollideID::BOSS);
	CCollisionMgr::GetInstance()->CheckCollisionList(ECollideID::PLAYER_PROJECTILE, ECollideID::MOB);

	CCollisionMgr::GetInstance()->CheckCollisionList(ECollideID::BOSS_PROJECTILE, ECollideID::WALL);
	CCollisionMgr::GetInstance()->CheckCollisionList(ECollideID::BOSS_PROJECTILE, ECollideID::PLAYER);

	CCollisionMgr::GetInstance()->CheckCollisionList(ECollideID::BOSS, ECollideID::WALL);
	CCollisionMgr::GetInstance()->CheckCollisionList(ECollideID::BOSS, ECollideID::PLAYER);

	CCollisionMgr::GetInstance()->CheckCollisionList(ECollideID::BOSS_SUNKEN, ECollideID::PLAYER);


	CParticleMgr::GetInstance()->LateUpdate();

	CEffectMgr::GetInstance()->LateUpdate();

	CPickingMgr::GetInstance()->RemoveList();

	CCollisionMgr::GetInstance()->CheckCollisionList(ECollideID::PLAYER, ECollideID::SCENE_CHANGE_TRIGGER);

	CCollisionMgr::GetInstance()->ClearCollisionGroup();

}

void CScene::RenderScene()
{
	//로그 출력 및 확인용 함수
}

void CScene::CreateNewLayer(const tstring& _newLayerName)
{
	shared_ptr<CLayer> m_NewLayer = shared_ptr<CLayer>();
	m_NewLayer->ReadyLayer();
	m_mapLayer.insert({_newLayerName, m_NewLayer});

}

HRESULT CScene::AddNewObject(const tstring& _strLayerName, const tstring& _strObjKey, shared_ptr<CGameObject> _pObj)
{
	auto iter = m_mapLayer.find(_strLayerName);

	if (iter != m_mapLayer.end())
		return iter->second->AddNewObject(_strObjKey, _pObj);

	return E_FAIL;
}

void CScene::KeyInput()
{
	if (GetAsyncKeyState(VK_F1) & 0x8000)
		CCollisionMgr::GetInstance()->SetVisible(true);

	else if (GetAsyncKeyState(VK_F2) & 0x8000)
		CCollisionMgr::GetInstance()->SetVisible(false);
}


shared_ptr<CComponent> CScene::GetComponent(const tstring& _strLayerName, const tstring& _strObjName, const tstring& _strComName, COMPONENTID _eID)
{
	auto iter = m_mapLayer.find(_strLayerName);

	if (m_mapLayer.end() == iter) {
		return nullptr;
	}
	return (iter->second)->GetComponent(_strObjName, _strComName, _eID);
}
