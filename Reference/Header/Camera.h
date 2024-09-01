#pragma once

#include"GameObject.h"

BEGIN(Engine)

enum class EProjectionType
{
	PERSPECTIVE, // 원근
	ORTHOGRAPHIC, // 직교
};


class ENGINE_DLL CCamera : public CGameObject
{
public:
	explicit CCamera(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCamera();

public:
	virtual HRESULT			ReadyGameObject();
	virtual _int			UpdateGameObject(const _float& fTimeDelta);
	virtual void			LateUpdateGameObject();

protected:
	_matrix				m_matView, m_matProj;
	_vec3				m_vEye, m_vAt, m_vUp;
	_float				m_fFov, m_fAspect, m_fNear, m_fFar;

protected:
	EProjectionType		m_eCurrentType = EProjectionType::PERSPECTIVE;

protected:
	virtual void		Free();

};

END