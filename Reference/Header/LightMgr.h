#pragma once

#include"Engine_Define.h"
#include"Light.h"

BEGIN(Engine)

class ENGINE_DLL CLightMgr
{
	DECLARE_SINGLETON(CLightMgr)

private:
	explicit CLightMgr();
	~CLightMgr();

public:
	HRESULT ReadyLight(LPDIRECT3DDEVICE9 _pGraphicDev,
		const D3DLIGHT9* _pLightInfo,
		const _uint& iIndex);

	void RemoveList();
	void AddList(shared_ptr<CLight> _pLight);

	void LightOff();
	void LightOn();


	void DungeonLightOn(_int _iDungeonNum);
	void DungeonLightOff(_int _iDungeonNum);

	void DungeonLightAllOff();

	void DungeonBattleLightOn();
	void DungeonBattleLightOff();

public:

	shared_ptr<CLight> InitPointLight(LPDIRECT3DDEVICE9 _pGraphicDev, D3DLIGHT9 _pLightInfo , _int _iDungeonNum = 0);
	shared_ptr<CLight> InitDirectionLight(LPDIRECT3DDEVICE9 _pGraphicDev, D3DLIGHT9 _pLightInfo, _int _iDungeonNum = 0);
	shared_ptr<CLight> InitSpotLight(LPDIRECT3DDEVICE9 _pGraphicDev, D3DLIGHT9 _pLightInfo, _int _iDungeonNum = 0);

private:
	 
	list<shared_ptr<CLight>> m_LightList;
};


END