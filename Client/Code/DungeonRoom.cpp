#include "pch.h"
#include "DungeonRoom.h"
#include "Creature.h"
#include "BattleSystem.h"

CDungeonRoom::CDungeonRoom()
{
}

CDungeonRoom::~CDungeonRoom()
{
}

void CDungeonRoom::MakeDisableAllObject()
{
	if (size(m_vGameObject) == 0) return;
	for (auto& iter : m_vGameObject) {
		if (iter->GetIsEnable()) iter->SetEnable(false);
	}
}

void CDungeonRoom::MakeAbleAllObject()
{
	if (size(m_vGameObject) == 0) return;
	for (auto& iter : m_vGameObject) {
		if (!iter->GetIsEnable()) iter->SetEnable(true);
	}
}

_bool CDungeonRoom::BattleUpdate(const _float& fTimeDelta)
{
	//FormBattlePosition(m_vHeroes, m_vMonsters, -PI / 2.f, _vec3(WALLSIZEX + PATHSIZEX, WALLSIZEX * 14.f, 0.f));

	if (m_pBattleSystem->Update(fTimeDelta))
		return true;

	return false;
}

void CDungeonRoom::BattleReady()
{
	m_pBattleSystem->FormBattlePosition(m_vHeroes, m_vMonsters, 0.f, PI, m_vBattleCameraPos);
	m_pBattleSystem->SetCenterPos(m_vBattleCameraOriginPos);
}
