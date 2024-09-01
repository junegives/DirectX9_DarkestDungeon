#pragma once
#include "GameObject.h"

class CPickingGame : public CGameObject
{
public:
	explicit CPickingGame(LPDIRECT3DDEVICE9 pGraphicDev);
	~CPickingGame();

public:
	void	Start();
	void	End();

private:
	LPDIRECT3DDEVICE9 m_pGraphicDev;
};

