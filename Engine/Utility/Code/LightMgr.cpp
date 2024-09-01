#include"LightMgr.h"

IMPLEMENT_SINGLETON(CLightMgr)

CLightMgr::CLightMgr()
{
}

CLightMgr::~CLightMgr()
{
}

HRESULT CLightMgr::ReadyLight(LPDIRECT3DDEVICE9 _pGraphicDev, const D3DLIGHT9* _pLightInfo, const _uint& iIndex)
{
	shared_ptr<CLight> pLight = make_shared<CLight>(_pGraphicDev);

	if (nullptr != pLight) {
		pLight->ReadyLight(_pLightInfo, iIndex);
		m_LightList.push_back(pLight);
	}

	return S_OK;
}

void CLightMgr::RemoveList()
{

	for (auto& iter : m_LightList) {
		iter->LightOff();
	}

	m_LightList.clear();



}

void CLightMgr::AddList(shared_ptr<CLight> _pLight)
{
	m_LightList.push_back(_pLight);
}

void CLightMgr::LightOff()
{


}

void CLightMgr::LightOn()
{
}

void CLightMgr::DungeonLightOn(_int _iDungeonNum)
{
	for (auto& iter : m_LightList) {
		if (iter->GetDungeonNum() == _iDungeonNum) {
			iter->LightOn();
		}
	}

}

void CLightMgr::DungeonLightOff(_int _iDungeonNum)
{
	for (auto& iter : m_LightList) {
		if (iter->GetDungeonNum() == _iDungeonNum) {
			iter->LightOff();
		}
	}
}

void CLightMgr::DungeonLightAllOff()
{
	for (auto& iter : m_LightList) {
		if (0 != iter->GetDungeonNum()) {
			iter->LightOff();
		}
	}
}

void CLightMgr::DungeonBattleLightOn()
{

	for (auto& iter : m_LightList) {
		if (99 == iter->GetDungeonNum()) {
			iter->LightOn();
		}
	}

}

void CLightMgr::DungeonBattleLightOff()
{
	for (auto& iter : m_LightList) {
		if (99 == iter->GetDungeonNum()) {
			iter->LightOff();
		}
	}
}


shared_ptr<CLight> CLightMgr::InitPointLight(LPDIRECT3DDEVICE9 _pGraphicDev, D3DLIGHT9 _pLightInfo, _int _iDungeonNum)
{
	D3DLIGHT9 pLightInfo;

	ZeroMemory(&pLightInfo, sizeof(D3DLIGHT9));

	pLightInfo.Type = D3DLIGHT_POINT;
	pLightInfo.Range = _pLightInfo.Range;
	
	pLightInfo.Diffuse = _pLightInfo.Diffuse;
	pLightInfo.Specular = _pLightInfo.Specular;
	pLightInfo.Ambient = _pLightInfo.Ambient;
	pLightInfo.Direction = _pLightInfo.Direction;

	pLightInfo.Position = _pLightInfo.Position;

	pLightInfo.Attenuation0 = _pLightInfo.Attenuation0;
	pLightInfo.Attenuation1 = _pLightInfo.Attenuation1;
	pLightInfo.Attenuation2 = _pLightInfo.Attenuation2;

	shared_ptr<CLight> pLight = make_shared<CLight>(_pGraphicDev);
	pLight->ReadyLight(&pLightInfo, (_uint)m_LightList.size(), _iDungeonNum);

	m_LightList.push_back(pLight);

	return pLight;
}

shared_ptr<CLight> CLightMgr::InitDirectionLight(LPDIRECT3DDEVICE9 _pGraphicDev , D3DLIGHT9 _pLightInfo, _int _iDungeonNum)
{
	D3DLIGHT9 pLightInfo;

	ZeroMemory(&pLightInfo, sizeof(D3DLIGHT9));

	pLightInfo.Type = D3DLIGHT_DIRECTIONAL;

	pLightInfo.Diffuse = _pLightInfo.Diffuse;
	pLightInfo.Specular = _pLightInfo.Specular;	
	pLightInfo.Ambient = _pLightInfo.Ambient;
	pLightInfo.Direction = _pLightInfo.Direction;
	
	shared_ptr<CLight> pLight = make_shared<CLight>(_pGraphicDev);
	pLight->ReadyLight(&pLightInfo, (_uint)m_LightList.size(), _iDungeonNum);

	m_LightList.push_back(pLight);

	return pLight;
}

shared_ptr<CLight> CLightMgr::InitSpotLight(LPDIRECT3DDEVICE9 _pGraphicDev, D3DLIGHT9 _pLightInfo, _int _iDungeonNum)
{
	D3DLIGHT9 pLightInfo;

	ZeroMemory(&pLightInfo, sizeof(D3DLIGHT9));
	pLightInfo.Type = D3DLIGHT_DIRECTIONAL;

	shared_ptr<CLight> pLight = make_shared<CLight>(_pGraphicDev);


	return pLight;
}
