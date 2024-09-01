#include"PickingMgr.h"
#include"GameObject.h"

IMPLEMENT_SINGLETON(CPickingMgr)

CPickingMgr::CPickingMgr()
{
}

CPickingMgr::~CPickingMgr()
{
}

void CPickingMgr::RayPicking(LONG _lX, LONG _lY)
{
 	if (m_PickingList.empty())
		return;

	_matrix mat;

	D3DXMatrixPerspectiveFovLH(&mat, D3DXToRadian(60.f), (_float)WINCX / WINCY, 0.1f, 1000.f);
	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &mat);

	D3DVIEWPORT9		ViewPort;
	ZeroMemory(&ViewPort, sizeof(D3DVIEWPORT9));
	m_pGraphicDev->GetViewport(&ViewPort);


	_matrix matProj, matView;
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	D3DXVECTOR3	vMouse;

	vMouse.x = (((2.f * _lX) / ViewPort.Width) - 1.f) / matProj(0, 0);
	vMouse.y = (((-2.f * _lY) / ViewPort.Height) + 1.f) / matProj(1, 1);

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, 0, &matView);

	_vec3		vRayPos, vRayDir;

	vRayPos = { 0.f, 0.f, 0.f };
	vRayDir = { vMouse.x, vMouse.y , 1.f};

	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matView);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matView);
	D3DXVec3Normalize(&vRayDir, &vRayDir);

	_vec3 vMin, vMax;
	_vec3	vCenter;
	_float	fRad;

	for (auto iter = m_PickingList.begin(); iter != m_PickingList.end(); ++iter) {
		(*iter)->GetSphere(vCenter, fRad);
		if (InterSectingSphere(vRayPos, vRayDir, vCenter, fRad)) {
			(*iter)->PickingObj();
			m_PickingList.erase(iter);
			break;
		}
	}
}

_bool CPickingMgr::IntersectingRay(_vec3 _vRayPos, _vec3 _vRayNormal, _vec3 _vMin, _vec3 _vMax)
{

	_float fMinX = (_vMin.x - _vRayPos.x) / _vRayNormal.x;
	_float fMaxX = (_vMax.x - _vRayPos.x) / _vRayNormal.x;

	if (fMinX > fMaxX)
		swap(fMinX, fMaxX);

	_float fMinY = (_vMin.y - _vRayPos.y) / _vRayNormal.y;
	_float fMaxY = (_vMax.y - _vRayPos.y) / _vRayNormal.y;

	if (fMinY > fMaxY)
		swap(fMinY, fMaxY);


	if (fMinX > fMaxY || fMaxY > fMaxX)
		return false;

	_float fMinZ = (_vMin.z - _vRayPos.z) / _vRayNormal.z;
	_float fMaxZ = (_vMax.z - _vRayPos.z) / _vRayNormal.z;

	if (fMinZ > fMaxZ)
		swap(fMinZ, fMaxZ);

	 //if (fMinX > fMaxZ || fMinZ > fMaxX)
		//return false;


	return true;

}

_bool CPickingMgr::InterSectingSphere(_vec3& _vRayPos, _vec3& _vRayNormal, _vec3& _vCenter, _float _dRadius)
{
	_float a = D3DXVec3Dot(&_vRayNormal, &_vRayNormal);
	_vec3 vPos = (_vRayPos - _vCenter);
	_float b = 2 * D3DXVec3Dot(&_vRayNormal, &vPos);
	_float c = D3DXVec3Dot(&vPos, &vPos) - _dRadius * _dRadius;
	_float discriminant = b * b - (4.f* c);

	if (discriminant < 0) {

		return false;

	}
	
	discriminant = sqrtf(discriminant);

	float s0 = (-b + discriminant) / 2.f;
	float s1 = (-b - discriminant) / 2.f;

	if (s0 >= 0.f || s1 >= 0.0f)
		return true;

	return false;


}

void CPickingMgr::RemoveList()
{
	m_PickingList.clear();
}

void CPickingMgr::AddList(shared_ptr<CGameObject> _pObj)
{
	m_PickingList.push_back(_pObj);
}
