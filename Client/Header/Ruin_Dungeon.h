#pragma once

#include"Scene.h"
#include "Creature.h"

class CDungeonSystem;
class CDungeonRoom;

class CRuin_Dungeon : public Engine::CScene
{
public:
	explicit CRuin_Dungeon(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CRuin_Dungeon();

public:
	virtual HRESULT ReadyScene() override;
	virtual _int UpdateScene(const _float& fTimeDelta) override;
	virtual void LateUpdateScene() override;
	virtual void RenderScene() override;

public:
	const _tchar* Get_String() const { return m_szString; }
	virtual void SetLight();

private:

	HRESULT				Ready_Layer_Environment(tstring pLayerTag);
	HRESULT				Ready_Layer_SkyBox(tstring pLayerTag);
	HRESULT				Ready_Layer_Camera(tstring pLayerTag);

	HRESULT				Ready_Layer_GameObject(tstring pLayerTag);
	HRESULT				Ready_Layer_UI(tstring pLayerTag);

	shared_ptr<CDungeonSystem>	m_pRuinDungeon = {};		// 带傈2
	shared_ptr<CDungeonRoom>	m_pRoom1 = {};				// 规1
	shared_ptr<CDungeonRoom>	m_pRoom2 = {};				// 规2(烹肺)
	shared_ptr<CDungeonRoom>	m_pRoom3 = {};				// 规3
	shared_ptr<CDungeonRoom>	m_pRoom4 = {};				// 规4(烹肺)
	shared_ptr<CDungeonRoom>	m_pRoom5 = {};				// 规5
	shared_ptr<CDungeonRoom>	m_pRoom6 = {};				// 规6(焊胶规)

	_bool				m_bTestBattle = false;
	_bool				m_bInitNarr = false;


	_tchar				m_szString[128];
};

