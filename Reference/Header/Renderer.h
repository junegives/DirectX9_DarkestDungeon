#pragma once

#include"GameObject.h"

class ENGINE_DLL CRenderer
{
	DECLARE_SINGLETON(CRenderer)

private:
	explicit CRenderer();
	~CRenderer();

public:
	void AddRenderGroup(RENDERID eType, shared_ptr<CGameObject> pGameObject);
	void RenderGameObject(LPDIRECT3DDEVICE9& pGraphicDev);
	void ClearRenderGroup();


public:
	void		RenderPriority(LPDIRECT3DDEVICE9& pGraphicDev);
	void		RenderNonalpha(LPDIRECT3DDEVICE9& pGraphicDev);
	void		RenderAlpha(LPDIRECT3DDEVICE9& pGraphicDev);
	void		RenderFront(LPDIRECT3DDEVICE9& pGraphicDev);
	void		RenderUI(LPDIRECT3DDEVICE9& pGraphicDev);
	void		RenderLast(LPDIRECT3DDEVICE9& pGraphicDev);
	void		RenderMouse(LPDIRECT3DDEVICE9& pGraphicDev);


public:

	void SetProjPerspective(LPDIRECT3DDEVICE9& pGraphicDev);
	void SetProjOrth(LPDIRECT3DDEVICE9& pGraphicDev);


private:
	list<shared_ptr<CGameObject>> m_RenderList[RENDER_END];
	
	_matrix m_MatOrigin;
};

