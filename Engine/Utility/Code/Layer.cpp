#include "Layer.h"
#include "CollisionMgr.h"
#include "Collider.h"

CLayer::CLayer()
{
}

CLayer::~CLayer()
{
}

void CLayer::AwakeLayer()
{
	for (auto& iter : m_objectMap) {
		for (auto& it : iter.second) {
			//GameComponenet Setting
			it->AwakeGameObject();
		}
	}


}

HRESULT CLayer::ReadyLayer()
{

	for (auto& iter : m_objectMap) {
		for (auto& it : iter.second) {
			//GameComponenet Setting
 			it->ReadyGameObject();
		}
	}


	return S_OK;
}

_int CLayer::UpdateLayer(const _float& fTimeDelta)
{
	_int		iResult(0);

	for (auto& iter : m_objectMap)
	{
		for (auto it = iter.second.begin(); it != iter.second.end();) {

			//Dead Check
			if (false == ((*it)->GetIsActive())) {
				it->reset();
				it = iter.second.erase(it);
				continue;
			}

			//Enable Check
			if (false == ((*it)->GetIsEnable())) {
				it++;
				continue;
			}

			iResult = (*it)->UpdateGameObject(fTimeDelta);

			if (iResult & 0x80000000)
				return iResult;

			it++;
		}
	}




	return iResult;
}

void CLayer::LateUpdateLayer()
{
	for (auto& iter : m_objectMap)
	{
		for (auto it = iter.second.begin(); it != iter.second.end();) {

			//Dead Check
			if (false == ((*it)->GetIsActive())) {
				it->reset();
				it = iter.second.erase(it);
				continue;
			}

			//Enable Check
 			if (false == ((*it)->GetIsEnable())) {
				it++;
				continue;
			}

			// Collision Check
			if ((*it)->IsColliding())
			{
				switch ((*it)->GetColType())
				{
				case Engine::ECollideID::PLAYER:
					CCollisionMgr::GetInstance()->AddCollisionGroup(ECollideID::PLAYER, *it);
					break;
				case Engine::ECollideID::WALL:
					CCollisionMgr::GetInstance()->AddCollisionGroup(ECollideID::WALL, *it);
					break;
				case Engine::ECollideID::BOSS:
					CCollisionMgr::GetInstance()->AddCollisionGroup(ECollideID::BOSS, *it);
					break;
				case Engine::ECollideID::PLAYER_PROJECTILE:
					CCollisionMgr::GetInstance()->AddCollisionGroup(ECollideID::PLAYER_PROJECTILE, *it);
					break;
				case Engine::ECollideID::BOSS_PROJECTILE:
					CCollisionMgr::GetInstance()->AddCollisionGroup(ECollideID::BOSS_PROJECTILE, *it);
					break;
				case Engine::ECollideID::BOSS_SUNKEN:
					CCollisionMgr::GetInstance()->AddCollisionGroup(ECollideID::BOSS_SUNKEN, *it);
					break;
				case Engine::ECollideID::MOB:
					CCollisionMgr::GetInstance()->AddCollisionGroup(ECollideID::MOB, *it);
					break;
				case Engine::ECollideID::SCENE_CHANGE_TRIGGER:
					CCollisionMgr::GetInstance()->AddCollisionGroup(ECollideID::SCENE_CHANGE_TRIGGER, *it);
				default:
					break;
				}

				// 플레이어 이동 충돌 검사
				if (ECollideID::PLAYER != (*it)->GetColType() && ECollideID::SCENE_CHANGE_TRIGGER != (*it)->GetColType())
					CCollisionMgr::GetInstance()->CheckCollision(*it);

			}

			(*it)->LateUpdateGameObject();


			it++;
		}
	}
}

HRESULT CLayer::CreateGameObject(tstring _strObjName, shared_ptr<CGameObject> _pObject)
{
	auto iter = m_objectMap.find(_strObjName);
	
	//이미 동일 오브젝트를 넣은 적이 있을 때
	if (m_objectMap.end() == iter) {
		list<shared_ptr<CGameObject>> vTempVec;
		vTempVec.push_back(_pObject);
		m_objectMap.insert({ _strObjName ,vTempVec });

	}
	else {
		m_objectMap[_strObjName].push_back(_pObject);
	}


	return E_NOTIMPL;
}

shared_ptr<CComponent> CLayer::GetComponent(const tstring& _strObjName, const tstring& _strComName, COMPONENTID _eID)
{
	auto iter = m_objectMap.find(_strObjName);

	if (iter == m_objectMap.end())
		return nullptr;

	return iter->second.front()->GetComponent(_strComName, _eID);
}

HRESULT CLayer::AddNewObject(const tstring& _strObjKey, shared_ptr<CGameObject> _pObj)
{
	auto iter = m_objectMap.find(_strObjKey);

	if (iter != m_objectMap.end()) {
		iter->second.push_back(_pObj);
	}
	else {
		list<shared_ptr<CGameObject>> tempList;
		tempList.push_back(_pObj);
		m_objectMap.insert({ _strObjKey , tempList });
	}

	return S_OK;
}
