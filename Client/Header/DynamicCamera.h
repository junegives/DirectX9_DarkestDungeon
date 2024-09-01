#pragma once

#include"Camera.h"

class CDynamicCamera : public CCamera
{
public:

	explicit CDynamicCamera(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CDynamicCamera();

public:
	virtual HRESULT			ReadyGameObject();

	virtual _int		UpdateGameObject(const _float& fTimeDelta);
	virtual void		LateUpdateGameObject();

private:
	
	void		Key_Input(const _float& fTimeDelta);
	void		Mouse_Move();
	void		Mouse_Fix();
	
private:
	_float		m_fSpeed = 50.f;
	_bool		m_bFix = false;
	_bool		m_bCheck = false;
	_float		m_fTimeDelta = 0.f;


	_float m_fAngle= 0.f;



};

