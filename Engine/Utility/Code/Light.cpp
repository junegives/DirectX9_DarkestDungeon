#include "Light.h"

CLight::CLight(LPDIRECT3DDEVICE9 _pGraphicDev)
	: m_pGraphicDev(_pGraphicDev), m_iIndex(0)

{
}

CLight::~CLight()
{
}

HRESULT CLight::ReadyLight(const D3DLIGHT9* _pLightInfo, const _uint& _iIndex , int _iRoomNum)
{
	memcpy(&m_tLightInfo, _pLightInfo, sizeof(D3DLIGHT9));
	m_iIndex = _iIndex;

	m_pGraphicDev->SetLight(_iIndex, _pLightInfo);


	m_iDungeonRoomNum =  _iRoomNum;

	if (0 == _iRoomNum) {
		m_pGraphicDev->LightEnable(_iIndex, TRUE);
	}

	return S_OK;
}

void CLight::SetPosition(_vec3 _vPos)
{
	m_tLightInfo.Position.x = _vPos.x;
	m_tLightInfo.Position.y = _vPos.y;
	m_tLightInfo.Position.z = _vPos.z;

	m_pGraphicDev->SetLight(m_iIndex, &m_tLightInfo);

}

void CLight::LightOn()
{
	m_pGraphicDev->LightEnable(m_iIndex, TRUE);

}

void CLight::LightOff()
{
	m_pGraphicDev->LightEnable(m_iIndex, FALSE);
}

void CLight::LightIntensity(_int _iLevel)
{

	if (1 == _iLevel) {
		m_tLightInfo.Attenuation1 = 0.1f;
	}
	else if (2 == _iLevel) {

		m_tLightInfo.Attenuation1 = 0.06f;

	}
	else if (3 == _iLevel) {

		m_tLightInfo.Attenuation1 = 0.f;
	}

	m_pGraphicDev->SetLight(m_iIndex, &m_tLightInfo);

}

void CLight::ResetPlrLight()
{

	m_tLightInfo.Attenuation1 = 0.f;
	m_pGraphicDev->SetLight(m_iIndex, &m_tLightInfo);

}



void CLight::Free()
{
	m_pGraphicDev->LightEnable(m_iIndex, FALSE);
	Safe_Release(m_pGraphicDev);
}
