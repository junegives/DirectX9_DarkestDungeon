#pragma once

#include"Scene.h"

class CVillage : public Engine::CScene
{
public:
	explicit CVillage(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CVillage();

public:
	virtual HRESULT ReadyScene() override;
	virtual _int UpdateScene(const _float& fTimeDelta) override;
	virtual void LateUpdateScene() override;
	virtual void RenderScene() override;

public:

	virtual void SetLight();

public:
	void KeyInput();

private:

	HRESULT				Ready_Layer_Environment(tstring pLayerTag);
	HRESULT				Ready_Layer_SkyBox(tstring pLayerTag);
	HRESULT				Ready_Layer_GameObject(tstring pLayerTag);
	HRESULT				Ready_Layer_UI(tstring pLayerTag);
	HRESULT				Ready_Layer_Camera(tstring pLayerTag);

	_bool				m_bInitNarr = false;

};

