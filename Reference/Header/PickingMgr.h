#pragma once

#include"Engine_Define.h"

BEGIN(Engine)

class CGameObject;

class ENGINE_DLL CPickingMgr
{
	DECLARE_SINGLETON(CPickingMgr)

private:
	explicit CPickingMgr();
	~CPickingMgr();

public:

	void ReadyPickingMgr(LPDIRECT3DDEVICE9 _pGraphicDev) {
		m_pGraphicDev = _pGraphicDev;
	}

	void RayPicking(LONG _lX, LONG _lY); // ������(Ray) ���� ��ȯ ����
	_bool IntersectingRay(_vec3 _vRayPos, _vec3 _vRayNormal, _vec3 _vMin, _vec3 _vMax); //AABB �ڽ�-������ ��ŷ(��������)
	_bool InterSectingSphere(_vec3& _vRayPos, _vec3& _vRayNormal, _vec3& _vCenter, _float _dRadius); //�� - ������ ��ŷ

	void RemoveList();
	void AddList(shared_ptr<CGameObject> _pObj);

private:

	LPDIRECT3DDEVICE9 m_pGraphicDev = nullptr;
	list<shared_ptr<CGameObject>> m_PickingList;

};

END
