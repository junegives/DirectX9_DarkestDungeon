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

	void RayPicking(LONG _lX, LONG _lY); // 반직선(Ray) 월드 변환 연산
	_bool IntersectingRay(_vec3 _vRayPos, _vec3 _vRayNormal, _vec3 _vMin, _vec3 _vMax); //AABB 박스-반직선 피킹(오류있음)
	_bool InterSectingSphere(_vec3& _vRayPos, _vec3& _vRayNormal, _vec3& _vCenter, _float _dRadius); //구 - 반직선 피킹

	void RemoveList();
	void AddList(shared_ptr<CGameObject> _pObj);

private:

	LPDIRECT3DDEVICE9 m_pGraphicDev = nullptr;
	list<shared_ptr<CGameObject>> m_PickingList;

};

END
