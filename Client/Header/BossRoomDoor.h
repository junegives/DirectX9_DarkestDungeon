#pragma once
#include"InteractionObj.h"

class CBossRoomDoor : public CInteractionObj
{
public:
	explicit CBossRoomDoor(LPDIRECT3DDEVICE9 _pGraphicDev);
	virtual ~CBossRoomDoor();

public:
	virtual HRESULT			ReadyGameObject();
	virtual _int			UpdateGameObject(const _float& fTimeDelta);
	virtual void			LateUpdateGameObject();
	virtual void			RenderGameObject();

	_bool					GetEnterState() { return m_bEnter; }

protected:
	virtual void			AddComponent();
	virtual void			GetInteractionKey(const _float& fTimeDelta);

protected:
	virtual void	Interaction() {};
	virtual _bool	IsFinish() { return true; };
	virtual void	FinishInteraction() {};
	virtual void	ChangeTexture() {};

private:

	_bool m_bDebounce = false;
	_bool m_bEnter = false;

};

