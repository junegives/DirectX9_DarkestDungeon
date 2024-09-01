#pragma once
#include "GameObject.h"

class CInside : public CGameObject
{
public:
	CInside(LPDIRECT3DDEVICE9 pGraphicDev);
	CInside(const CInside& rhs);
	virtual ~CInside();

public:
	virtual HRESULT			ReadyGameObject();
	virtual _int			UpdateGameObject(const _float& fTimeDelta);
	virtual void			LateUpdateGameObject();
	virtual void			RenderGameObject();

protected:
	vector<shared_ptr<CGameObject>>		m_vecGameObject = {};
};

