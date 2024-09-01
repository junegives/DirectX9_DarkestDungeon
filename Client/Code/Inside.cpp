#include "pch.h"
#include "Inside.h"

#include "Export_Utility.h"
#include "Export_System.h"

#include "Terrain.h"

CInside::CInside(LPDIRECT3DDEVICE9 pGraphicDev)
    : Engine::CGameObject(pGraphicDev)
{
}

CInside::CInside(const CInside& rhs)
    : Engine::CGameObject(rhs)
{
}

CInside::~CInside()
{
}

HRESULT CInside::ReadyGameObject()
{
    for (auto& iter : m_vecGameObject) {
        iter->AwakeGameObject();
        iter->ReadyGameObject();
    }

    return S_OK;
}

_int CInside::UpdateGameObject(const _float& fTimeDelta)
{
    Engine::AddRenderGroup(RENDER_ALPHA, shared_from_this());

    for (auto& iter : m_vecGameObject) {
        iter->UpdateGameObject(fTimeDelta);
    }

    _int iExit = __super::UpdateGameObject(fTimeDelta);

    return iExit;
}

void CInside::LateUpdateGameObject()
{
    for (auto& iter : m_vecGameObject) {
        iter->LateUpdateGameObject();

        if (iter->IsColliding())
        {
            CCollisionMgr::GetInstance()->CheckCollision(iter);
        }
    }
}

void CInside::RenderGameObject()
{
	for (auto& iter : m_vecGameObject) {
		iter->RenderGameObject();
	}
}
