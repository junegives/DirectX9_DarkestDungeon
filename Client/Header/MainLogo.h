#pragma once

#include"Export_Utility.h"
#include"Loading.h"
#include "Video.h"

class CBackGround;

class CMainLogo : public CScene
{
public:

	explicit CMainLogo(LPDIRECT3DDEVICE9 _pGraphicDev);
	virtual ~CMainLogo();

public:
	virtual HRESULT			ReadyScene();
	virtual _int			UpdateScene(const _float& _fTimeDelta);
	virtual void			LateUpdateScene();
	virtual void			RenderScene();


public:

	HRESULT				Ready_Layer_UI(tstring pLayerTag);

private:
	
	_bool m_bLoadingFin = false;
	_bool m_bIsFirstView = true;
	shared_ptr<CLoading> m_pLoading;
	shared_ptr<CVideo>	m_pVideo;

	shared_ptr<CBackGround> m_pBackGround;
};

