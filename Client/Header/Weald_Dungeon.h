#pragma once

#include"Scene.h"
#include "Creature.h"

class CDungeonSystem;
class CDungeonRoom;

class CWeald_Dungeon : public Engine::CScene
{
public:
	explicit CWeald_Dungeon(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CWeald_Dungeon();

public:
	virtual HRESULT ReadyScene() override;
	virtual _int UpdateScene(const _float& fTimeDelta) override;
	virtual void LateUpdateScene() override;
	virtual void RenderScene() override;

public:
	const _tchar* Get_String() const { return m_szString; }

public:
	virtual void SetLight();

private:

	HRESULT				Ready_Layer_Environment(tstring pLayerTag);
	HRESULT				Ready_Layer_SkyBox(tstring pLayerTag);
	HRESULT				Ready_Layer_Camera(tstring pLayerTag);

	HRESULT				Ready_Layer_GameObject(tstring pLayerTag);
	HRESULT				Ready_Layer_UI(tstring pLayerTag);

	shared_ptr<CDungeonSystem> m_pWealdDungeon = {};	// ����1
	shared_ptr<CDungeonRoom> m_pRoom1 = {};				// ��1
	shared_ptr<CDungeonRoom> m_pRoom2 = {};				// ��2
	shared_ptr<CDungeonRoom> m_pRoom3 = {};				// ��3
	shared_ptr<CDungeonRoom> m_pRoom4 = {};				// ��4
	shared_ptr<CDungeonRoom> m_pRoom5 = {};				// ��5

	_bool				m_bTestBattle = false;
	_bool				m_binitNarrOn = false;

	_tchar		m_szString[128];

};

