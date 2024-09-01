#pragma once

#include "Engine_Define.h"

BEGIN(Engine)

class CGameObject;

class ENGINE_DLL CCollisionMgr
{
	DECLARE_SINGLETON(CCollisionMgr)

private:
	explicit CCollisionMgr();
	~CCollisionMgr();

public:
	void CheckCollision(shared_ptr<CGameObject> _pDst);
	void CheckCollisionList(ECollideID _pSrcType, ECollideID _pDstType);
	bool AABB_AABB(shared_ptr<CGameObject> _pSrc, shared_ptr<CGameObject> _pDst);
	bool Sphere_Sphere(shared_ptr<CGameObject> _pSrc, shared_ptr<CGameObject> _pDst);

public:
	void AddCollisionGroup(ECollideID _eType, shared_ptr<CGameObject> pGameObject);
	void ClearCollisionGroup();

	void	SetVisible(_bool _bVisible) { m_bVisible = _bVisible; }
	_bool	GetVisible() { return m_bVisible; }

private:
	_bool	m_bVisible = false;

private:
	map<ECollideID, list<shared_ptr<CGameObject>>> m_pCollisionGroupList;
};

END