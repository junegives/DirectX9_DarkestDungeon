#pragma once
#include"Engine_Define.h"
#include"Layer.h"

BEGIN(Engine)

class ENGINE_DLL CScene
{
public:
	explicit CScene(LPDIRECT3DDEVICE9 _pGraphicDev);
	virtual ~CScene();

public:
	virtual HRESULT			ReadyScene();
	virtual _int			UpdateScene(const _float& _fTimeDelta);
	virtual void			LateUpdateScene();
	virtual void			RenderScene();

public:
	void CreateNewLayer(const tstring& _newLayerName);
	HRESULT AddNewObject(const tstring& _strLayerName, const tstring& _strObjKey, shared_ptr<CGameObject> _pObj);
	void KeyInput();

	virtual void SetLight() {};

	void SetSceneChangeState(_bool _bSceneChange) { m_bSceneChange = _bSceneChange; }

public:
	shared_ptr<CComponent> GetComponent(const tstring& _strLayerName, const tstring& _strObjName, const tstring& _strComName, COMPONENTID _eID);

protected:
	LPDIRECT3DDEVICE9	m_pGraphicDev;
	map<tstring, std::shared_ptr<CLayer>> m_mapLayer;

	_bool m_bSceneChange = false;
};

END
