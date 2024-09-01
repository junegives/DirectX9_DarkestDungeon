#pragma once
#include "Inside.h"
class CGuild : public CInside
{
public:
	CGuild(LPDIRECT3DDEVICE9 pGrahpicDev);
	CGuild(const CGuild& rhs);
	virtual ~CGuild();

public:
	virtual HRESULT			ReadyGameObject();
	virtual _int			UpdateGameObject(const _float& fTimeDelta);
	virtual void			LateUpdateGameObject();
	virtual void			RenderGameObject();
};