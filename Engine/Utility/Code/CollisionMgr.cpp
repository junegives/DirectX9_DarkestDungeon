#include "CollisionMgr.h"
#include "GameObject.h"
#include "Collider.h"
#include "GameMgr.h"

IMPLEMENT_SINGLETON(CCollisionMgr)

CCollisionMgr::CCollisionMgr()
{
	list<shared_ptr<CGameObject>> pPlayerList;
	list<shared_ptr<CGameObject>> pBossList;
	list<shared_ptr<CGameObject>> pWallList;
	list<shared_ptr<CGameObject>> pPlayerProjectileList;
	list<shared_ptr<CGameObject>> pBossProjectileList;
	list<shared_ptr<CGameObject>> pSceneChangeTrigger;
	list<shared_ptr<CGameObject>> pMobList;
	list<shared_ptr<CGameObject>> pSunkenList;

	m_pCollisionGroupList.emplace(ECollideID::PLAYER, pPlayerList);
	m_pCollisionGroupList.emplace(ECollideID::BOSS, pBossList);
	m_pCollisionGroupList.emplace(ECollideID::WALL, pWallList);
	m_pCollisionGroupList.emplace(ECollideID::PLAYER_PROJECTILE, pPlayerProjectileList);
	m_pCollisionGroupList.emplace(ECollideID::BOSS_PROJECTILE, pBossProjectileList);
	m_pCollisionGroupList.emplace(ECollideID::SCENE_CHANGE_TRIGGER, pSceneChangeTrigger);
	m_pCollisionGroupList.emplace(ECollideID::MOB, pMobList);
	m_pCollisionGroupList.emplace(ECollideID::BOSS_SUNKEN, pSunkenList);
}

CCollisionMgr::~CCollisionMgr()
{
}

void CCollisionMgr::CheckCollision(shared_ptr<CGameObject> _pDst)
{
	shared_ptr<CGameObject> pPlayer = CGameMgr::GetInstance()->GetPlayer();

	AABB_AABB(pPlayer, _pDst);
}

void CCollisionMgr::CheckCollisionList(ECollideID _pSrcType, ECollideID _pDstType)
{
	auto SrcList = m_pCollisionGroupList.find(_pSrcType);

	auto DstList = m_pCollisionGroupList.find(_pDstType);

	if (_pSrcType == ECollideID::PLAYER)
	{
		if (DstList == m_pCollisionGroupList.end())
			return;

		for (auto& iterDst : m_pCollisionGroupList.find(_pDstType)->second)
		{
			AABB_AABB(CGameMgr::GetInstance()->GetPlayer(), iterDst);
		}
	}

	else
	{
		if (SrcList == m_pCollisionGroupList.end() || DstList == m_pCollisionGroupList.end())
			return;

		for (auto& iterSrc : m_pCollisionGroupList.find(_pSrcType)->second)
		{
			for (auto& iterDst : m_pCollisionGroupList.find(_pDstType)->second)
			{
				// dst그룹 벽과 충돌
				if (ECollideID::WALL == _pDstType)
				{
					AABB_AABB(iterSrc, iterDst);
				}
				// src sunken과 충돌
				else if (ECollideID::BOSS_SUNKEN == _pSrcType)
				{
					AABB_AABB(iterSrc, iterDst);
				}
				// src sunken과 충돌
				else if (ECollideID::MOB == _pSrcType)
				{
					AABB_AABB(iterSrc, iterDst);
				}
				// 그 외
				else
				{
					Sphere_Sphere(iterSrc, iterDst);
				}
			}
		}
	}
}

bool CCollisionMgr::AABB_AABB(shared_ptr<CGameObject> _pSrc, shared_ptr<CGameObject> _pDst)
{
	shared_ptr<CCollider> pSrcCollider =
		dynamic_pointer_cast<CCollider>(_pSrc->GetComponent(L"Com_Collider", ID_DYNAMIC));

	shared_ptr<CCollider> pDsCollider =
		dynamic_pointer_cast<CCollider>(_pDst->GetComponent(L"Com_Collider", ID_DYNAMIC));

	_vec3 vSrcPos = *pSrcCollider->GetPos();
	_vec3 vSrcScale = *pSrcCollider->GetScale();

	_vec3 vDstPos = *pDsCollider->GetPos();
	_vec3 vDstScale = *pDsCollider->GetScale();

	if ((vSrcPos.x - vSrcScale.x / 2.f < vDstPos.x + vDstScale.x / 2.f) &&
		(vSrcPos.x + vSrcScale.x / 2.f > vDstPos.x - vDstScale.x / 2.f) &&
		(vSrcPos.y - vSrcScale.y / 2.f < vDstPos.y + vDstScale.y / 2.f) &&
		(vSrcPos.y + vSrcScale.y / 2.f > vDstPos.y - vDstScale.y / 2.f) &&
		(vSrcPos.z - vSrcScale.z / 2.f < vDstPos.z + vDstScale.z / 2.f) &&
		(vSrcPos.z + vSrcScale.z / 2.f > vDstPos.z - vDstScale.z / 2.f))
	{
		// 충돌량을 알아야 하는 경우
		if (ECollideID::PLAYER == _pSrc->GetColType() && (ECollideID::WALL == _pDst->GetColType() || ECollideID::ENVIRONMENT == _pDst->GetColType()))
		{
			// x충돌 (동시검사)
			if (abs(vSrcPos.x - vDstPos.x) < ((vSrcScale.x + vDstScale.x) / 2.f))
			{
				_float fGap = (vSrcScale.x + vDstScale.x) / 2.f - abs(vSrcPos.x - vDstPos.x);

				if (vSrcPos.x < vDstPos.x)
					_pSrc->OnCollide(_pDst, fGap, EDirection::RIGHT);
				else
					_pSrc->OnCollide(_pDst, fGap, EDirection::LEFT);
			}

			// z충돌 (동시검사)
			if (abs(vSrcPos.z - vDstPos.z) < ((vSrcScale.z + vDstScale.z) / 2.f))
			{
				_float fGap = (vSrcScale.z + vDstScale.z) / 2.f - abs(vSrcPos.z - vDstPos.z);

				if (vSrcPos.z < vDstPos.z)
					_pSrc->OnCollide(_pDst, fGap, EDirection::TOP);
				else
					_pSrc->OnCollide(_pDst, fGap, EDirection::BOTTOM);
			}

			return true;
		}
		
		// 충돌 여부만 알면 되는 경우
		_pSrc->OnCollide(_pDst);
		_pDst->OnCollide(_pSrc);

		return true;
	}

	return false;
}

bool CCollisionMgr::Sphere_Sphere(shared_ptr<CGameObject> _pSrc, shared_ptr<CGameObject> _pDst)
{
	shared_ptr<CCollider> pSrcCollider =
		dynamic_pointer_cast<CCollider>(_pSrc->GetComponent(L"Com_Collider", ID_DYNAMIC));

	shared_ptr<CCollider> pDstCollider =
		dynamic_pointer_cast<CCollider>(_pDst->GetComponent(L"Com_Collider", ID_DYNAMIC));

	_vec3 vSrcPos = *pSrcCollider->GetPos();
	_float fSrcRadius = pSrcCollider->GetRadius();

	_vec3 vDstPos = *pDstCollider->GetPos();
	_float fDstRadius = pDstCollider->GetRadius();

	const _vec3 vLength = vSrcPos - vDstPos;

	if (D3DXVec3Length(&vLength) < fSrcRadius + fDstRadius)
	{
		_pSrc->OnCollide(_pDst);
		_pDst->OnCollide(_pSrc);

		return true;
	}

	return false;
}

void CCollisionMgr::AddCollisionGroup(ECollideID _eType, shared_ptr<CGameObject> pGameObject)
{
	m_pCollisionGroupList.find(_eType)->second.push_back(pGameObject);
}

void CCollisionMgr::ClearCollisionGroup()
{
	for (auto& iter : m_pCollisionGroupList)
		iter.second.clear();
}
