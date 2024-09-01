#pragma once

#include "GameObject.h"

class CSceneEvent : public CGameObject
{
public:
	explicit CSceneEvent(LPDIRECT3DDEVICE9 _pGraphicDev);
	virtual ~CSceneEvent();

public:
	virtual HRESULT			ReadyGameObject();
	virtual _int			UpdateGameObject(const _float& fTimeDelta);
	virtual void			LateUpdateGameObject();
	virtual void			RenderGameObject();


public:

	void MoveObject(_float _fTime);
	void MoveObjectFront(_float _fTime);

private:

	_float m_fEventTime = 0.f;
	_float m_fSceneActTime = 0.f;

	_bool m_bFirstEventDone = false;
	_bool m_bSecondEventDone = false;
	_bool m_bThirdEventDone = false;

	shared_ptr<CGameObject> m_pMainObject = nullptr;
	_vec3 m_vCurrentPos;


};

