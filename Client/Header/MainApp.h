#pragma once

#include "Engine_Define.h"
#include "Export_System.h"
#include"Export_Utility.h"
#include"Engine_Macro.h"


BEGIN(Engine)
class CGraphicDev;
END


class CMainApp
{

public:
	explicit CMainApp();
	virtual ~CMainApp();

public:
	HRESULT		Ready_MainApp();
	_int		Update_MainApp(const _float& fTimeDelta);
	void		LateUpdate_MainApp();
	void		Render_MainApp();


private:
	HRESULT			SetUp_DefaultSetting(LPDIRECT3DDEVICE9* ppGraphicDev);

private:
	Engine::CGraphicDev* m_pDeviceClass;
	LPDIRECT3DDEVICE9	 m_pGraphicDev;

private:
	virtual void			Free();

};

