#include "Renderer.h"

IMPLEMENT_SINGLETON(CRenderer)

CRenderer::CRenderer()
{
}

CRenderer::~CRenderer()
{
}

void CRenderer::AddRenderGroup(RENDERID eType, shared_ptr<CGameObject> pGameObject)
{
	if(RENDER_END < eType || nullptr == pGameObject)
		return;

	m_RenderList[eType].push_back(pGameObject);
}

void CRenderer::RenderGameObject(LPDIRECT3DDEVICE9& pGraphicDev)
{
	//원근투영
	SetProjPerspective(pGraphicDev);

	RenderPriority(pGraphicDev);
	RenderNonalpha(pGraphicDev);
	RenderAlpha(pGraphicDev);
	RenderFront(pGraphicDev);


	//직교투영
	SetProjOrth(pGraphicDev);
	RenderUI(pGraphicDev);

	// UI 위의 UI
	RenderLast(pGraphicDev);
	RenderMouse(pGraphicDev);

	pGraphicDev->SetTransform(D3DTS_VIEW, &m_MatOrigin);

	ClearRenderGroup();

}

void CRenderer::ClearRenderGroup()
{

	for (size_t i = 0; i < RENDER_END; ++i)
	{
		m_RenderList[i].clear();
	}

}


void CRenderer::RenderPriority(LPDIRECT3DDEVICE9& pGraphicDev)
{
	pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	for (auto& iter : m_RenderList[RENDER_PRIORITY])
		iter->RenderGameObject();

	pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
}

void CRenderer::RenderNonalpha(LPDIRECT3DDEVICE9& pGraphicDev)
{
	for (auto& iter : m_RenderList[RENDER_NONALPHA]) {

		iter->RenderGameObject();
	}
}

void CRenderer::RenderAlpha(LPDIRECT3DDEVICE9& pGraphicDev)
{
	//pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	//pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	//pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	//// 알파 테스트
	pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0xc0);
	pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_RenderList[RENDER_ALPHA].sort([](shared_ptr<CGameObject> pDst, shared_ptr<CGameObject> pSrc)->bool { return pDst->Get_ViewZ() > pSrc->Get_ViewZ();  });

	for (auto& iter : m_RenderList[RENDER_ALPHA])
		iter->RenderGameObject();

	pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	//pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);


}

void CRenderer::RenderFront(LPDIRECT3DDEVICE9& pGraphicDev)
{
	pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0xc0);
	pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	for (auto& iter : m_RenderList[RENDER_FRONT])
		iter->RenderGameObject();

	pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
}

void CRenderer::RenderUI(LPDIRECT3DDEVICE9& pGraphicDev)
{
	for (auto& iter : m_RenderList[RENDER_UI]) {
		iter->RenderGameObject();
	}
}

void CRenderer::RenderLast(LPDIRECT3DDEVICE9& pGraphicDev)
{
	for (auto& iter : m_RenderList[RENDER_LAST]) {
		iter->RenderGameObject();
	}
}

void CRenderer::RenderMouse(LPDIRECT3DDEVICE9& pGraphicDev)
{
	for (auto& iter : m_RenderList[RENDER_MOUSE]) {
		iter->RenderGameObject();
	}
}

void CRenderer::SetProjPerspective(LPDIRECT3DDEVICE9& pGraphicDev)
{
	_matrix matProj;

	D3DXMatrixPerspectiveFovLH(&matProj, D3DXToRadian(60.f), (_float)WINCX / WINCY, 0.1f, 1000.f);
	pGraphicDev->SetTransform(D3DTS_PROJECTION, &matProj);

}

void CRenderer::SetProjOrth(LPDIRECT3DDEVICE9& pGraphicDev)
{
	pGraphicDev->GetTransform(D3DTS_VIEW, &m_MatOrigin);

	_matrix matView;
	D3DXMatrixIdentity(&matView);
	pGraphicDev->SetTransform(D3DTS_VIEW, &matView);

	_matrix matProj;
	D3DXMatrixOrthoLH(&matProj, WINCX, WINCY, 0.f, 1.f);

	pGraphicDev->SetTransform(D3DTS_PROJECTION, &matProj);

}
