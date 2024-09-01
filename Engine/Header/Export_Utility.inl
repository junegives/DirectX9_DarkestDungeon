#include "Export_Utility.h"

inline shared_ptr<CComponent> Get_Component(const tstring& pLayerTag, const tstring& pObjTag, const tstring& pComponentTag, COMPONENTID eID)
{
    return CSceneMgr::GetInstance()->GetComponenet(pLayerTag, pObjTag, pComponentTag, eID);
}

inline void ChangeScene(shared_ptr<CScene> _pScene)
{
    CSceneMgr::GetInstance()->ChangeScene(_pScene);
}


inline _int UpdateScene(const _float& fTimeDelta)
{
    return CSceneMgr::GetInstance()->Update(fTimeDelta);
}

inline void LateUpdateScene()
{
    CSceneMgr::GetInstance()->LateUpdate();
}

inline void RenderScene(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CSceneMgr::GetInstance()->Render(pGraphicDev);
}

inline void AddRenderGroup(RENDERID eType, shared_ptr<CGameObject> pGameObject)
{
    CRenderer::GetInstance()->AddRenderGroup(eType, pGameObject);
}

inline void RenderGameObject(LPDIRECT3DDEVICE9& pGraphicDev)
{
    CRenderer::GetInstance()->RenderGameObject(pGraphicDev);
}

inline void ClearRenderGroup()
{
    CRenderer::GetInstance()->ClearRenderGroup();
}

inline void Release_Utility()
{
    CParticleMgr::GetInstance()->DestroyInstance();
    CEffectMgr::GetInstance()->DestroyInstance();
    CRenderer::GetInstance()->DestroyInstance();
    CSceneMgr::GetInstance()->DestroyInstance();
    CGameMgr::GetInstance()->DestroyInstance();
    CResourceMgr::GetInstance()->DestroyInstance();
    CCollisionMgr::GetInstance()->DestroyInstance();
}

inline void ReadyResourceMgr(LPDIRECT3DDEVICE9 _pGraphicDev)
{
    CResourceMgr::GetInstance()->ReadyResource(_pGraphicDev);
}
