#pragma once

#include"Scene.h"
#include"LoadingBack.h"
#include"Loading.h"

class CLoadingScreen : public CScene
{
public:

	explicit CLoadingScreen(LPDIRECT3DDEVICE9 _pGraphicDev , ELoadingSceneType _eType);
	virtual ~CLoadingScreen();

public:
	virtual HRESULT			ReadyScene();
	virtual _int			UpdateScene(const _float& _fTimeDelta);
	virtual void			LateUpdateScene();
	virtual void			RenderScene();

public:

	HRESULT				Ready_Layer_UI(tstring pLayerTag);

private:
	ELoadingSceneType m_eLoadingType;
	_bool m_bLoadingFin = false;

	shared_ptr<CLoading> m_pLoading;

};

