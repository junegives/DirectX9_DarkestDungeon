#pragma once
#include"Engine_Define.h"

BEGIN(Engine)


class ENGINE_DLL CLight
{
public:
	explicit CLight(LPDIRECT3DDEVICE9 _pGraphicDev);
	~CLight();

public:
	HRESULT			ReadyLight(const D3DLIGHT9* _pLightInfo, const _uint& _iIndex , int _iRoomNum = 0);

	void			SetPosition(_vec3 _vPos);
	_uint			GetLightNum() { return m_iIndex; }
	_int			GetDungeonNum() { return m_iDungeonRoomNum; }

public:

public:

	void			LightOn();
	void			LightOff();


public:

	void			LightIntensity(_int _iLevel);
	void			ResetPlrLight();

private:
	LPDIRECT3DDEVICE9			m_pGraphicDev;
	D3DLIGHT9					m_tLightInfo;
	_uint						m_iIndex;

	//��� �����Ǿ���ϴ� �������� ����(Directional Light & PlayerLight) = 0 , Object�� �پ��ִ� ������ ���� ���� Room Number�� ���� ��ȣ �־��ֱ�
	_int						m_iDungeonRoomNum = 0;

private:
	void Free();

};

END