#include "SceneMgr.h"
#include"Export_Utility.h"

#include"UIMgr.h"
#include"LightMgr.h"

IMPLEMENT_SINGLETON(CSceneMgr)

CSceneMgr::CSceneMgr() : m_pCurrentScene(nullptr)
{
}

CSceneMgr::~CSceneMgr()
{
}

int CSceneMgr::Update(const float& _fDeltaTime)
{
	if (nullptr == m_pCurrentScene)
		return -1;

	return m_pCurrentScene->UpdateScene(_fDeltaTime);
}

void CSceneMgr::LateUpdate()
{
	m_pCurrentScene->LateUpdateScene();

}

void CSceneMgr::Render(LPDIRECT3DDEVICE9 _pGraphicDev)
{
	Engine::RenderGameObject(_pGraphicDev);

	if (m_pCurrentScene)
		m_pCurrentScene->RenderScene();

}

HRESULT CSceneMgr::ChangeScene(shared_ptr<CScene> _newScene)
{

	if (!m_bIsLoadingScene && m_pCurrentScene) {
		CResourceMgr::GetInstance()->RemoveSceneTexture();
		CUIMgr::GetInstance()->SceneChange();
		CLightMgr::GetInstance()->RemoveList();
	}


	if (m_pCurrentScene)
	{
		m_pCurrentScene.reset();
	}
	

	Engine::ClearRenderGroup();

	m_pCurrentScene = _newScene;
	m_bIsLoadingScene = false;


	return S_OK;
}



void CSceneMgr::CreateScene(const tstring& _strSceneName, shared_ptr<CScene> _newScene)
{

}

shared_ptr<CComponent> CSceneMgr::GetComponenet(const tstring& _strLayerName, const tstring& _strObjName, const tstring& _strComName, COMPONENTID _eID)
{
	if (!m_pCurrentScene)
		return nullptr;

	if (m_bIsLoadingScene) {
		return 	m_pReadyScene->GetComponent(_strLayerName, _strObjName, _strComName, _eID);
	}
	return 	m_pCurrentScene->GetComponent(_strLayerName, _strObjName, _strComName, _eID);
}

HRESULT CSceneMgr::AddNewObject(const tstring& _LayerName, const tstring& _ObjKeyName, shared_ptr<CGameObject> _pObj)
{
	if (m_bIsLoadingScene) {
		return 	m_pReadyScene->AddNewObject(_LayerName, _ObjKeyName, _pObj);
	}

	if (m_pCurrentScene)
		return m_pCurrentScene->AddNewObject(_LayerName, _ObjKeyName, _pObj);


	return E_FAIL;
}
