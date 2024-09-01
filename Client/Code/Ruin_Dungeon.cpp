#include "pch.h"
#include "Ruin_Dungeon.h"
#include "Terrain.h"
#include "Wall.h"
#include "SkyBox.h"
#include "Layer.h"
#include "StaticCamera.h"
#include "DynamicCamera.h"
#include "InteractionObj.h"
#include "Player.h"
#include "DungeonSystem.h"
#include "DungeonRoom.h"
#include "BattleSystem.h"

#include"Inventory.h"
#include"PlayerHand.h"
#include"Item.h"

#include"GameMgr.h"
#include"CameraMgr.h"

#include"Village.h"

#include "RuinDungeonDoor.h"
#include "Ruin_Trap.h"
#include "Ruin_Obstacle.h"
#include "Ruin_Curio_Armor.h"
#include "Ruin_Curio_Fountain.h"
#include "Ruin_Curio_Sarcophagus.h"
#include "Ruin_Curio_Sconce.h"
#include "Ruin_Curio_Statue.h"

#include "Ruin_SecretDoor.h"

#include "BrigandCutthroat.h"
#include "BrigandFusilier.h"
#include "BrigandBloodletter.h"

#include "BoneCourtier.h"
#include "BoneDefender.h"
#include "BoneSoldier.h"

#include "Highwayman.h"
#include "Jester.h"
#include "Vestal.h"
#include "ShieldBreaker.h"

#include "Export_System.h"
#include "Export_Utility.h"

#include"ResourceMgr.h"

#include"UIMgr.h"
#include"BattleHeroUI.h"
#include"SoundMgr.h"

#include"Narration.h"

#include "BossMap.h"
#include "Weald_Dungeon.h"

#include"MouseCursor.h"

#include"PuzzleHint.h"
#include"Player.h"
#include"LightMgr.h"

#include "DungeonStatus.h"
#include"InteractionInfo.h"

#include "LoadingScreen.h"


CRuin_Dungeon::CRuin_Dungeon(LPDIRECT3DDEVICE9 pGraphicDev)
	:CScene(pGraphicDev)
{
	ZeroMemory(m_szString, sizeof(m_szString));
}

CRuin_Dungeon::~CRuin_Dungeon()
{
}

HRESULT CRuin_Dungeon::ReadyScene()
{
	// text
	lstrcpy(m_szString, L"Ruin Dungeon");
	// Weald 던전
	m_pRuinDungeon = make_shared<CDungeonSystem>();
	// 던전 방1
	m_pRoom1 = make_shared<CDungeonRoom>();
	m_pRoom2 = make_shared<CDungeonRoom>();
	m_pRoom3 = make_shared<CDungeonRoom>();
	m_pRoom4 = make_shared<CDungeonRoom>();
	m_pRoom5 = make_shared<CDungeonRoom>();
	m_pRoom6 = make_shared<CDungeonRoom>();

	Ready_Layer_Environment(L"Layer_3_Environment");
	Ready_Layer_SkyBox(L"Layer_1_SkyBox");
	Ready_Layer_GameObject(L"Layer_4_GameObj");
	Ready_Layer_UI(L"Layer_2_UI");
	CUIMgr::GetInstance()->SceneUIInitialize();
	Ready_Layer_Camera(L"Layer_5_Camera");

	for (auto& iter : m_mapLayer) {
		//GameComponenet Setting
		iter.second->ReadyLayer();
	}

	CSoundMgr::GetInstance()->PlayBGM(L"Amb_Ruin_Base.wav", 1.f);

	shared_ptr<CItem> pItem = make_shared<CItem>(m_pGraphicDev);
	pItem->GetUITextureKeyName(L"Player_Item_RedGem");
	pItem->SetOnField(false);

	shared_ptr<CItem> pItem2 = make_shared<CItem>(m_pGraphicDev);
	pItem2->GetUITextureKeyName(L"Player_Item_BlueGem");
	pItem2->SetOnField(false);

	shared_ptr<CItem> pItem3 = make_shared<CItem>(m_pGraphicDev);
	pItem3->GetUITextureKeyName(L"Player_Item_GreenGem");
	pItem3->SetOnField(false);

	dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->InsertItem(pItem);
	dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->InsertItem(pItem2);
	dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->InsertItem(pItem3);

	SetLight();

	return __super::ReadyScene();
}

_int CRuin_Dungeon::UpdateScene(const _float& fTimeDelta)
{
	if (!m_bInitNarr) {
		CUIMgr::GetInstance()->NarrationOn(L"Narr_SecondDungeon");
		m_bInitNarr = true;
	}


	// 방 이동했는지 체크
	if (dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->GetRoomChange())
	{
		dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SetRoomChange(false);
		m_pRuinDungeon->DisableAllRoom();
		m_pRuinDungeon->AbleRoom(dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->GetCurrentRoom());
	}
	
	// 세번째 방 전투 트리거
	if (dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->GetCurrentRoom() == 3 && !m_pRoom3->GetBattleStart())
	{
		shared_ptr<CTransform> pTransform = dynamic_pointer_cast<CTransform>((CGameMgr::GetInstance()->GetPlayer())->GetComponent(L"Com_Transform", ID_DYNAMIC));
	
		if (pTransform->GetPos()->z > RUIN_WALLSIZEX * 13.5f - 25.f)
		{
			_vec3* vPos = pTransform->GetPos();
			CCameraMgr::GetInstance()->SetState(ECameraMode::BATTLE);
			CCameraMgr::GetInstance()->MovingStraight(ECameraMode::ZOOMIN, { vPos->x , vPos->y + 5.f ,  RUIN_WALLSIZEX * 13.5f - 20.f });
			CUIMgr::GetInstance()->SelectUIVisibleOff(L"UI_Inventory");
			CUIMgr::GetInstance()->SelectUIVisibleOff(L"UI_DungeonStatus");

			//전투 시작 이펙트
			shared_ptr<CEffect> pEffect = CEffectMgr::GetInstance()->GetEffect();
			if (pEffect) {
				pEffect->SetAnimEffect(L"Effect_BattleStart", _vec3(vPos->x, vPos->y + 8.f, RUIN_WALLSIZEX * 13.5f -5.f), _vec3(3.5f, 3.5f, 3.5f), 1.5f, false);
				pEffect->SetActive(true);
			}

			CLightMgr::GetInstance()->DungeonBattleLightOn();


			dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SetBattleTrigger(true);
			CSoundMgr::GetInstance()->StopSound(CHANNELID::BGM);
			CSoundMgr ::GetInstance()->PlayBGM(L"Combat_Level2_Loop1.wav", 0.6f);
			pTransform->SetPosition(RUIN_WALLSIZEX*4.5f, 0.f, RUIN_WALLSIZEX * 13.5f - 20.f);
			m_pRoom3->SetBattleStart(true);
			dynamic_pointer_cast<CInteractionInfo>(CUIMgr::GetInstance()->FindUI(L"UI_InteractionInfo"))->SetIsBattle(true);

			m_pRoom3->SetBattleCameraOriginPos({ vPos->x , vPos->y + 5.f ,  RUIN_WALLSIZEX * 13.5f - 20.f });
		}
	}
	
	// 세번째 방에서 전투시작되는 메커니즘
	if (dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->GetCurrentRoom() == 3)
	{
		if (dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->GetBattleTrigger())
		{
			m_pRoom3->SetBattleTrigger(true);
			// 전투준비
			m_pRoom3->BattleReady();
			dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SetInBattle(true);
	
			dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SetBattleTrigger(false);
		}
	}
	
	// 세번째 방 전투
	if (dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->GetCurrentRoom() == 3)
	{
		if (m_pRoom3->GetBattleTrigger())
		{
			// 전투가 끝나면
			if (m_pRoom3->BattleUpdate(fTimeDelta))
			{
				CSoundMgr::GetInstance()->StopAll();
				CSoundMgr::GetInstance()->PlayBGM(L"Amb_Ruin_Base.wav", 1.f);


				CCameraMgr::GetInstance()->SetFPSMode();

				CUIMgr::GetInstance()->SelectUIVisibleOn(L"UI_Inventory");
				CUIMgr::GetInstance()->SelectUIVisibleOn(L"UI_DungeonStatus");
				CUIMgr::GetInstance()->SelectUIVisibleOff(L"Battle_Hero_UI");
				dynamic_pointer_cast<CInteractionInfo>(CUIMgr::GetInstance()->FindUI(L"UI_InteractionInfo"))->SetIsBattle(false);

				CLightMgr::GetInstance()->DungeonBattleLightOff();

	
				m_pRoom3->SetBattleTrigger(false);
				dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SetInBattle(false);
			}
		}
	}
	
	// 전리품 드롭
	// 4번방(횃불 방)
	if (dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->GetCurrentRoom() == 4)
	{
		// 횃불 다 켰을때
		if (dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->GetEventTrigger() &&
			dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->GetEvent2Trigger()&&
			dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->GetEvent3Trigger())
		{
			// 문 열기
			for (int i = 0; i < size(m_pRoom4->GetGameObjectVector()); i++)
			{
				if (dynamic_pointer_cast<CRuinSecretDoor>(m_pRoom4->GetGameObjectVector()[i]))
				{
					dynamic_pointer_cast<CRuinSecretDoor>(m_pRoom4->GetGameObjectVector()[i])->SetOpenStart(true);
				}
			}

			dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SetEventTrigger(false);
			dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SetEvent2Trigger(false);
			dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SetEvent3Trigger(false);
		}
	}

	// 5번방
	if (dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->GetCurrentRoom() == 5)
	{
		// 키로 열었을때
		if (dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->GetEventTrigger())
		{
			for (int i = 0; i < size(m_pRoom5->GetGameObjectVector()); i++)
			{
				float fNum = 1.f / (float)(rand() % 10 + 1) * 5.f;
				if (dynamic_pointer_cast<CItem>(m_pRoom5->GetGameObjectVector()[i]))
				{
					shared_ptr<CTransform> pTransform = dynamic_pointer_cast<CTransform>(m_pRoom5->GetGameObjectVector()[i]->GetComponent(L"Com_Transform", ID_DYNAMIC));
					pTransform->SetPosition(RUIN_WALLSIZEX * 17.5f + fNum - 0.5f, 1.f, RUIN_WALLSIZEX * 13.5f - 0.01f - 0.01f * i);
					dynamic_pointer_cast<CItem>(m_pRoom5->GetGameObjectVector()[i])->StartParticle();
				}
			}
			dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SetEventTrigger(false);
		}
	}
	// 전투 방 순간이동
	if (GetAsyncKeyState('8') & 0x8000) {
		shared_ptr<CTransform> pTransform = dynamic_pointer_cast<CTransform>((CGameMgr::GetInstance()->GetPlayer())->GetComponent(L"Com_Transform", ID_DYNAMIC));
		pTransform->SetPosition(RUIN_WALLSIZEX * 4.5f, 0.f, RUIN_WALLSIZEX * 11.5f);
		dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SetCurrentRoom(3);
		m_pRuinDungeon->DisableAllRoom();
		m_pRuinDungeon->AbleRoom(3);
	}
	// 전투 강제종료
	if (GetAsyncKeyState('9') & 0x8000) {
		m_pRoom3->GetBattleSystem()->EndBattle();
		CCameraMgr::GetInstance()->SetFPSMode();

		CSoundMgr::GetInstance()->StopAll();
		CSoundMgr::GetInstance()->PlayBGM(L"Amb_Ruin_Base.wav", 1.f);

		CUIMgr::GetInstance()->SelectUIVisibleOn(L"UI_Inventory");
		CUIMgr::GetInstance()->SelectUIVisibleOff(L"Battle_Hero_UI");
		CUIMgr::GetInstance()->SelectUIVisibleOn(L"UI_DungeonStatus");

		CLightMgr::GetInstance()->DungeonBattleLightOff();


		m_pRoom3->SetBattleTrigger(false);
		dynamic_pointer_cast<CInteractionInfo>(CUIMgr::GetInstance()->FindUI(L"UI_InteractionInfo"))->SetIsBattle(false);

		dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SetInBattle(false);
	}

	int iExit;
	iExit = __super::UpdateScene(fTimeDelta);

	// 보스 씬 순간이동
	if (GetAsyncKeyState('6') & 0x8000) {
		CSoundMgr::GetInstance()->StopAll();
		CSoundMgr::GetInstance()->StopSound(CHANNELID::BGM);
		shared_ptr<CBossMap> pScene = make_shared<CBossMap>(m_pGraphicDev);
		CSceneMgr::GetInstance()->ChangeScene(pScene);
		pScene->ReadyScene();
	}

	//보스 씬 이동
	if (m_bSceneChange) {
		
		CSoundMgr::GetInstance()->StopAll();
		CSoundMgr::GetInstance()->StopSound(CHANNELID::BGM);
		
		shared_ptr<CScene> pLoadingScreen = make_shared<CLoadingScreen>(m_pGraphicDev, ELoadingSceneType::BOSS);
		CSceneMgr::GetInstance()->SetLoadingState(false);
		CSceneMgr::GetInstance()->ChangeScene(pLoadingScreen);
		pLoadingScreen->ReadyScene();


	}

	return iExit;
}

void CRuin_Dungeon::LateUpdateScene()
{
	__super::LateUpdateScene();

}

void CRuin_Dungeon::RenderScene()
{
	// Font
	//_vec2 vDungeonName = _vec2(0.f, 0.f);
	//Engine::Render_Font(L"Font_Default", m_szString,
	//	&vDungeonName, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
	//_vec2 vFontPos;

	/*for (int i = 0; i < 4; i++)
	{
		vFontPos = _vec2(400.f - i*100.f, 100.f);
		Engine::Render_Font(L"Font_Default",
			dynamic_pointer_cast<CCreature>((m_pRoom3->GetBattleSystem()->GetHeroesVector())[i])->Get_String(),
			&vFontPos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

		vFontPos = _vec2(400.f - i * 100.f, 200.f);
		Engine::Render_Font(L"Font_Default",
			dynamic_pointer_cast<CHero>((m_pRoom3->GetBattleSystem()->GetHeroesVector())[i])->Get_String4(),
			&vFontPos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

		vFontPos = _vec2(400.f - i * 100.f, 300.f);
		Engine::Render_Font(L"Font_Default",
			dynamic_pointer_cast<CCreature>((m_pRoom3->GetBattleSystem()->GetHeroesVector())[i])->Get_String2(),
			&vFontPos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

		vFontPos = _vec2(400.f - i * 100.f, 400.f);
		Engine::Render_Font(L"Font_Default",
			dynamic_pointer_cast<CCreature>((m_pRoom3->GetBattleSystem()->GetHeroesVector())[i])->Get_String3(),
			&vFontPos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
	}

	for (int i = 0; i < 4; i++)
	{
		vFontPos = _vec2(600.f + i * 100.f, 100.f);
		Engine::Render_Font(L"Font_Default",
			dynamic_pointer_cast<CCreature>((m_pRoom3->GetBattleSystem()->GetMonstersVector())[i])->Get_String(),
			&vFontPos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

		vFontPos = _vec2(600.f + i * 100.f, 300.f);
		Engine::Render_Font(L"Font_Default",
			dynamic_pointer_cast<CCreature>((m_pRoom3->GetBattleSystem()->GetMonstersVector())[i])->Get_String2(),
			&vFontPos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

		vFontPos = _vec2(600.f + i * 100.f, 400.f);
		Engine::Render_Font(L"Font_Default",
			dynamic_pointer_cast<CCreature>((m_pRoom3->GetBattleSystem()->GetMonstersVector())[i])->Get_String3(),
			&vFontPos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
	}*/
}

void CRuin_Dungeon::SetLight()
{
	D3DLIGHT9 _pLightInfo1;

	_pLightInfo1.Diffuse = { 0.3f , 0.3f , 0.3f , 1.f };
	_pLightInfo1.Specular = { 0.3f , 0.3f , 0.3f , 1.f };
	_pLightInfo1.Ambient = { 0.3f , 0.3f , 0.3f , 1.f };
	_pLightInfo1.Direction = { 1.f, -1.f, 1.f };

	CLightMgr::GetInstance()->InitDirectionLight(m_pGraphicDev, _pLightInfo1);

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SettingLight();

}

HRESULT CRuin_Dungeon::Ready_Layer_Environment(tstring pLayerTag)
{
	shared_ptr<CLayer> m_pLayer = make_shared<CLayer>();
	m_mapLayer.insert({ pLayerTag, m_pLayer });

	shared_ptr<CGameObject> m_pTerrain = make_shared<CTerrain>(m_pGraphicDev, L"Com_Ruin_FloorTexture", ETerrainType::DUNGEON2);
	shared_ptr<CGameObject> m_pTerrain2 = make_shared<CTerrain>(m_pGraphicDev, L"Com_Ruin_RoofTexture", ETerrainType::DUNGEON2_ROOF);
	m_pLayer->CreateGameObject(L"OBJ_Terrain", m_pTerrain);
	m_pLayer->CreateGameObject(L"OBJ_Terrain", m_pTerrain2);
	//dynamic_pointer_cast<CTransform>(m_pTerrain2->GetComponent(L"Com_Transform", ID_DYNAMIC))->SetPosition( 0.f, 80.f, 0.f );
	shared_ptr<CGameObject> m_pDoor;
	shared_ptr<CGameObject> m_pWall;
	shared_ptr<CGameObject> m_pBackWall;

	// 문
	m_pDoor = make_shared<CRuinDungeonDoor>(m_pGraphicDev, ERuinDungeonDoorType::ROOM1);
	m_pDoor->SetPos(_vec3(RUIN_WALLSIZEX * 3.f + RUIN_WALLSIZEX / 2.f + RUIN_WALLSIZEX * 1.f, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX * 0.f));
	m_pDoor->SetAngle(_vec3(0.f, 0.f, 0.f));
	m_pDoor->SetScale(_vec3(RUIN_PATHSIZEX / 2.f, RUIN_PATHSIZEX / 2.f, 1.f));
	m_pLayer->CreateGameObject(L"OBJ_Door", m_pDoor);

	m_pDoor = make_shared<CRuinDungeonDoor>(m_pGraphicDev, ERuinDungeonDoorType::ROOM1);
	m_pDoor->SetPos(_vec3(RUIN_WALLSIZEX * 3.f + RUIN_WALLSIZEX / 2.f + RUIN_WALLSIZEX * 1.f, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX * 3.f));
	m_pDoor->SetAngle(_vec3(0.f, 0.f, 0.f));
	m_pDoor->SetScale(_vec3(RUIN_PATHSIZEX/2.f, RUIN_PATHSIZEX / 2.f, 1.f));
	m_pLayer->CreateGameObject(L"OBJ_Door", m_pDoor);
	
	m_pDoor = make_shared<CRuinDungeonDoor>(m_pGraphicDev, ERuinDungeonDoorType::ROOM2);
	m_pDoor->SetPos(_vec3(RUIN_WALLSIZEX * 3.f + RUIN_WALLSIZEX / 2.f + RUIN_WALLSIZEX * 1.f, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX * 11.f));
	m_pDoor->SetAngle(_vec3(0.f, 0.f, 0.f));
	m_pDoor->SetScale(_vec3(RUIN_PATHSIZEX / 2.f, RUIN_PATHSIZEX / 2.f, 1.f));
	m_pLayer->CreateGameObject(L"OBJ_Door", m_pDoor);
	
	m_pDoor = make_shared<CRuinDungeonDoor>(m_pGraphicDev, ERuinDungeonDoorType::ROOM3);
	m_pDoor->SetPos(_vec3(RUIN_WALLSIZEX * 5.5f + RUIN_WALLSIZEX / 2.f + RUIN_WALLSIZEX * 1.f, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX * 13.5f));
	m_pDoor->SetAngle(_vec3(0.f, PI / 2.f, 0.f));
	m_pDoor->SetScale(_vec3(RUIN_PATHSIZEX / 2.f, RUIN_PATHSIZEX / 2.f, 1.f));
	m_pLayer->CreateGameObject(L"OBJ_Door", m_pDoor);
	
	m_pDoor = make_shared<CRuinDungeonDoor>(m_pGraphicDev, ERuinDungeonDoorType::ROOM4);
	m_pDoor->SetPos(_vec3(RUIN_WALLSIZEX * 13.5f + RUIN_WALLSIZEX / 2.f + RUIN_WALLSIZEX * 1.f, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX * 13.5f));
	m_pDoor->SetAngle(_vec3(0.f, PI / 2.f, 0.f));
	m_pDoor->SetScale(_vec3(RUIN_PATHSIZEX / 2.f, RUIN_PATHSIZEX / 2.f, 1.f));
	m_pLayer->CreateGameObject(L"OBJ_Door", m_pDoor);

	// 배경(데코)
	m_pWall = make_shared<CWall>(m_pGraphicDev, L"Ruin_Image_Altar", 1, true);
	m_pWall->SetPos(_vec3(RUIN_WALLSIZEX * 4.5f, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX * 16.f - 0.1f));
	m_pWall->SetAngle(_vec3(0.f, PI / 2.f, 0.f));
	m_pWall->SetScale(_vec3(RUIN_WALLSIZEX * 1.5f, RUIN_WALLSIZEX * 0.5f, 1.f));
	m_pLayer->CreateGameObject(L"OBJ_Wall", m_pWall);

	// 방1 벽
	for (int i = 0; i < 3; i++)
	{
		m_pWall = make_shared<CWall>(m_pGraphicDev, L"Com_Ruin_WallTexture", 7, true);
		m_pWall->SetPos(_vec3(RUIN_WALLSIZEX * 2.f, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX / 2.f + RUIN_WALLSIZEX * i));
		m_pWall->SetAngle(_vec3(0.f, 0.f, 0.f));
		m_pWall->SetScale(_vec3(RUIN_WALLSIZEX / 2.f, RUIN_WALLSIZEX / 2.f, 1.f));
		m_pLayer->CreateGameObject(L"OBJ_Wall", m_pWall);

		m_pBackWall = make_shared<CWall>(m_pGraphicDev, L"Com_Ruin_BackWallTexture", 1, false);
		m_pBackWall->SetPos(_vec3(RUIN_WALLSIZEX * 2.f - 0.1f, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX / 2.f + RUIN_WALLSIZEX * i));
		m_pBackWall->SetAngle(_vec3(0.f, 0.f, 0.f));
		m_pBackWall->SetScale(_vec3(RUIN_WALLSIZEX / 2.f, RUIN_WALLSIZEX / 2.f, 1));
		m_pLayer->CreateGameObject(L"OBJ_Back", m_pBackWall);
	}

	for (int i = 0; i < 3; i++)
	{
		m_pWall = make_shared<CWall>(m_pGraphicDev,L"Com_Ruin_WallTexture", 7, true);
		m_pWall->SetPos(_vec3(RUIN_WALLSIZEX * 4.f + RUIN_PATHSIZEX + RUIN_WALLSIZEX * 2.f, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX / 2.f + RUIN_WALLSIZEX * i));
		m_pWall->SetAngle(_vec3(0.f, 0.f, 0.f));
		m_pWall->SetScale(_vec3(RUIN_WALLSIZEX / 2.f, RUIN_WALLSIZEX / 2.f, 1));
		m_pLayer->CreateGameObject(L"OBJ_Wall", m_pWall);

		m_pBackWall = make_shared<CWall>(m_pGraphicDev, L"Com_Ruin_BackWallTexture", 1, false);
		m_pBackWall->SetPos(_vec3(RUIN_WALLSIZEX * 4.f + RUIN_PATHSIZEX + RUIN_WALLSIZEX * 2.f + 0.1f, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX / 2.f + RUIN_WALLSIZEX * i));
		m_pBackWall->SetAngle(_vec3(0.f, 0.f, 0.f));
		m_pBackWall->SetScale(_vec3(RUIN_WALLSIZEX / 2.f, RUIN_WALLSIZEX / 2.f, 1));
		m_pLayer->CreateGameObject(L"OBJ_Back", m_pBackWall);
	}

	for (int i = 0; i < 2; i++)
	{
		m_pWall = make_shared<CWall>(m_pGraphicDev,L"Com_Ruin_WallTexture", 7, true);
		m_pWall->SetPos(_vec3(RUIN_WALLSIZEX * 2.f + RUIN_WALLSIZEX / 2.f + RUIN_WALLSIZEX * i, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX * 3.f));
		m_pWall->SetAngle(_vec3(0.f, PI / 2.f, 0.f));
		m_pWall->SetScale(_vec3(RUIN_WALLSIZEX / 2.f, RUIN_WALLSIZEX / 2.f, 1.f));
		m_pLayer->CreateGameObject(L"OBJ_Wall", m_pWall);

		m_pBackWall = make_shared<CWall>(m_pGraphicDev, L"Com_Ruin_BackWallTexture", 1, false);
		m_pBackWall->SetPos(_vec3(RUIN_WALLSIZEX * 2.f + RUIN_WALLSIZEX / 2.f + RUIN_WALLSIZEX * i, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX * 3.f + 0.1f));
		m_pBackWall->SetAngle(_vec3(0.f, PI / 2.f, 0.f));
		m_pBackWall->SetScale(_vec3(RUIN_WALLSIZEX / 2.f, RUIN_WALLSIZEX / 2.f, 1));
		m_pLayer->CreateGameObject(L"OBJ_Back", m_pBackWall);
	}

	for (int i = 0; i < 2; i++)
	{
		m_pWall = make_shared<CWall>(m_pGraphicDev,L"Com_Ruin_WallTexture", 7, true);
		m_pWall->SetPos(_vec3(RUIN_WALLSIZEX * 3.f + RUIN_PATHSIZEX + RUIN_WALLSIZEX * 1.5f + RUIN_WALLSIZEX * i, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX * 3.f));
		m_pWall->SetAngle(_vec3(0.f, PI / 2.f, 0.f));
		m_pWall->SetScale(_vec3(RUIN_WALLSIZEX / 2.f, RUIN_WALLSIZEX / 2.f, 1.f));
		m_pLayer->CreateGameObject(L"OBJ_Wall", m_pWall);

		m_pBackWall = make_shared<CWall>(m_pGraphicDev, L"Com_Ruin_BackWallTexture", 1, false);
		m_pBackWall->SetPos(_vec3(RUIN_WALLSIZEX * 3.f + RUIN_PATHSIZEX + RUIN_WALLSIZEX * 1.5f + RUIN_WALLSIZEX * i, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX * 3.f + 0.1f));
		m_pBackWall->SetAngle(_vec3(0.f, PI / 2.f, 0.f));
		m_pBackWall->SetScale(_vec3(RUIN_WALLSIZEX / 2.f, RUIN_WALLSIZEX / 2.f, 1));
		m_pLayer->CreateGameObject(L"OBJ_Back", m_pBackWall);
	}

	for (int i = 0; i < 2; i++)
	{
		m_pWall = make_shared<CWall>(m_pGraphicDev, L"Com_Ruin_WallTexture", 7, true);
		m_pWall->SetPos(_vec3(RUIN_WALLSIZEX * 2.f + RUIN_WALLSIZEX / 2.f + RUIN_WALLSIZEX * i, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX * 0.f));
		m_pWall->SetAngle(_vec3(0.f, PI / 2.f, 0.f));
		m_pWall->SetScale(_vec3(RUIN_WALLSIZEX / 2.f, RUIN_WALLSIZEX / 2.f, 1.f));
		m_pLayer->CreateGameObject(L"OBJ_Wall", m_pWall);

		m_pBackWall = make_shared<CWall>(m_pGraphicDev, L"Com_Ruin_BackWallTexture", 1, false);
		m_pBackWall->SetPos(_vec3(RUIN_WALLSIZEX * 2.f + RUIN_WALLSIZEX / 2.f + RUIN_WALLSIZEX * i, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX * 0.f - 0.1f));
		m_pBackWall->SetAngle(_vec3(0.f, PI / 2.f, 0.f));
		m_pBackWall->SetScale(_vec3(RUIN_WALLSIZEX / 2.f, RUIN_WALLSIZEX / 2.f, 1));
		m_pLayer->CreateGameObject(L"OBJ_Back", m_pBackWall);
	}

	for (int i = 0; i < 2; i++)
	{
		m_pWall = make_shared<CWall>(m_pGraphicDev, L"Com_Ruin_WallTexture", 7, true);
		m_pWall->SetPos(_vec3(RUIN_WALLSIZEX * 5.f + RUIN_WALLSIZEX / 2.f + RUIN_WALLSIZEX * i, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX * 0.f));
		m_pWall->SetAngle(_vec3(0.f, PI / 2.f, 0.f));
		m_pWall->SetScale(_vec3(RUIN_WALLSIZEX / 2.f, RUIN_WALLSIZEX / 2.f, 1.f));
		m_pLayer->CreateGameObject(L"OBJ_Wall", m_pWall);

		m_pBackWall = make_shared<CWall>(m_pGraphicDev, L"Com_Ruin_BackWallTexture", 1, false);
		m_pBackWall->SetPos(_vec3(RUIN_WALLSIZEX * 5.f + RUIN_WALLSIZEX / 2.f + RUIN_WALLSIZEX * i, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX * 0.f - 0.1f));
		m_pBackWall->SetAngle(_vec3(0.f, PI / 2.f, 0.f));
		m_pBackWall->SetScale(_vec3(RUIN_WALLSIZEX / 2.f, RUIN_WALLSIZEX / 2.f, 1));
		m_pLayer->CreateGameObject(L"OBJ_Back", m_pBackWall);
	}

	// 통로 1 벽
	for (int i = 0; i < 8; i++)
	{
		m_pWall = make_shared<CWall>(m_pGraphicDev,L"Com_Ruin_WallTexture", 7, true);
		m_pWall->SetPos(_vec3(RUIN_WALLSIZEX * 3.f + RUIN_WALLSIZEX * 1.f, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX * 3.f + RUIN_WALLSIZEX / 2.f + RUIN_WALLSIZEX * i));
		m_pWall->SetAngle(_vec3(0.f, 0.f, 0.f));
		m_pWall->SetScale(_vec3(RUIN_WALLSIZEX / 2.f, RUIN_WALLSIZEX / 2.f, 1.f));
		m_pLayer->CreateGameObject(L"OBJ_Wall", m_pWall);

		m_pBackWall = make_shared<CWall>(m_pGraphicDev, L"Com_Ruin_BackWallTexture", 1, false);
		m_pBackWall->SetPos(_vec3(RUIN_WALLSIZEX * 3.f + RUIN_WALLSIZEX * 1.f - 0.1f, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX * 3.f + RUIN_WALLSIZEX / 2.f + RUIN_WALLSIZEX * i));
		m_pBackWall->SetAngle(_vec3(0.f, 0.f, 0.f));
		m_pBackWall->SetScale(_vec3(RUIN_WALLSIZEX / 2.f, RUIN_WALLSIZEX / 2.f, 1));
		m_pLayer->CreateGameObject(L"OBJ_Back", m_pBackWall);
	}

	for (int i = 0; i < 8; i++)
	{
		m_pWall = make_shared<CWall>(m_pGraphicDev,L"Com_Ruin_WallTexture", 7, true);
		m_pWall->SetPos(_vec3(RUIN_WALLSIZEX * 3.f + RUIN_PATHSIZEX + RUIN_WALLSIZEX * 1.f, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX * 3.f + RUIN_WALLSIZEX / 2.f + RUIN_WALLSIZEX * i));
		m_pWall->SetAngle(_vec3(0.f, 0.f, 0.f));
		m_pWall->SetScale(_vec3(RUIN_WALLSIZEX / 2.f, RUIN_WALLSIZEX / 2.f, 1.f));
		m_pLayer->CreateGameObject(L"OBJ_Wall", m_pWall);

		m_pBackWall = make_shared<CWall>(m_pGraphicDev, L"Com_Ruin_BackWallTexture", 1, false);
		m_pBackWall->SetPos(_vec3(RUIN_WALLSIZEX * 3.f + RUIN_PATHSIZEX + RUIN_WALLSIZEX * 1.f + 0.1f, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX * 3.f + RUIN_WALLSIZEX / 2.f + RUIN_WALLSIZEX * i));
		m_pBackWall->SetAngle(_vec3(0.f, 0.f, 0.f));
		m_pBackWall->SetScale(_vec3(RUIN_WALLSIZEX / 2.f, RUIN_WALLSIZEX / 2.f, 1));
		m_pLayer->CreateGameObject(L"OBJ_Back", m_pBackWall);
	}

	// 방 2 벽
	for (int i = 0; i < 5; i++)
	{
		m_pWall = make_shared<CWall>(m_pGraphicDev,L"Com_Ruin_WallTexture", 7, true);
		m_pWall->SetPos(_vec3(RUIN_WALLSIZEX * 2.f, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX / 2.f + RUIN_WALLSIZEX * 11.f + RUIN_WALLSIZEX * i));
		m_pWall->SetAngle(_vec3(0.f, 0.f, 0.f));
		m_pWall->SetScale(_vec3(RUIN_WALLSIZEX / 2.f, RUIN_WALLSIZEX / 2.f, 1.f));
		m_pLayer->CreateGameObject(L"OBJ_Wall", m_pWall);

		m_pBackWall = make_shared<CWall>(m_pGraphicDev, L"Com_Ruin_BackWallTexture", 1, false);
		m_pBackWall->SetPos(_vec3(RUIN_WALLSIZEX * 2.f - 0.1f, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX / 2.f + RUIN_WALLSIZEX * 11.f + RUIN_WALLSIZEX * i));
		m_pBackWall->SetAngle(_vec3(0.f, 0.f, 0.f));
		m_pBackWall->SetScale(_vec3(RUIN_WALLSIZEX / 2.f, RUIN_WALLSIZEX / 2.f, 1));
		m_pLayer->CreateGameObject(L"OBJ_Back", m_pBackWall);
	}

	for (int i = 0; i < 2; i++)
	{
		m_pWall = make_shared<CWall>(m_pGraphicDev,L"Com_Ruin_WallTexture", 7, true);
		m_pWall->SetPos(_vec3(RUIN_WALLSIZEX * 4.f + RUIN_PATHSIZEX + RUIN_WALLSIZEX * 2.f, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX / 2.f + RUIN_WALLSIZEX * 11.f + RUIN_WALLSIZEX * i));
		m_pWall->SetAngle(_vec3(0.f, 0.f, 0.f));
		m_pWall->SetScale(_vec3(RUIN_WALLSIZEX / 2.f, RUIN_WALLSIZEX / 2.f, 1.f));
		m_pLayer->CreateGameObject(L"OBJ_Wall", m_pWall);

		m_pBackWall = make_shared<CWall>(m_pGraphicDev, L"Com_Ruin_BackWallTexture", 1, false);
		m_pBackWall->SetPos(_vec3(RUIN_WALLSIZEX * 4.f + RUIN_PATHSIZEX + RUIN_WALLSIZEX * 2.f + 0.1f, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX / 2.f + RUIN_WALLSIZEX * 11.f + RUIN_WALLSIZEX * i));
		m_pBackWall->SetAngle(_vec3(0.f, 0.f, 0.f));
		m_pBackWall->SetScale(_vec3(RUIN_WALLSIZEX / 2.f, RUIN_WALLSIZEX / 2.f, 1));
		m_pLayer->CreateGameObject(L"OBJ_Back", m_pBackWall);
	}

	for (int i = 0; i < 2; i++)
	{
		m_pWall = make_shared<CWall>(m_pGraphicDev, L"Com_Ruin_WallTexture", 7, true);
		m_pWall->SetPos(_vec3(RUIN_WALLSIZEX * 4.f + RUIN_PATHSIZEX + RUIN_WALLSIZEX * 2.f, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX / 2.f + RUIN_WALLSIZEX * 14.f + RUIN_WALLSIZEX * i));
		m_pWall->SetAngle(_vec3(0.f, 0.f, 0.f));
		m_pWall->SetScale(_vec3(RUIN_WALLSIZEX / 2.f, RUIN_WALLSIZEX / 2.f, 1.f));
		m_pLayer->CreateGameObject(L"OBJ_Wall", m_pWall);

		m_pBackWall = make_shared<CWall>(m_pGraphicDev, L"Com_Ruin_BackWallTexture", 1, false);
		m_pBackWall->SetPos(_vec3(RUIN_WALLSIZEX * 4.f + RUIN_PATHSIZEX + RUIN_WALLSIZEX * 2.f + 0.1f, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX / 2.f + RUIN_WALLSIZEX * 14.f + RUIN_WALLSIZEX * i));
		m_pBackWall->SetAngle(_vec3(0.f, 0.f, 0.f));
		m_pBackWall->SetScale(_vec3(RUIN_WALLSIZEX / 2.f, RUIN_WALLSIZEX / 2.f, 1));
		m_pLayer->CreateGameObject(L"OBJ_Back", m_pBackWall);
	}

	for (int i = 0; i < 2; i++)
	{
		m_pWall = make_shared<CWall>(m_pGraphicDev,L"Com_Ruin_WallTexture", 7, true);
		m_pWall->SetPos(_vec3(RUIN_WALLSIZEX * 2.5f + RUIN_WALLSIZEX * i, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX * 11.f));
		m_pWall->SetAngle(_vec3(0.f, PI / 2.f, 0.f));
		m_pWall->SetScale(_vec3(RUIN_WALLSIZEX / 2.f, RUIN_WALLSIZEX / 2.f, 1.f));
		m_pLayer->CreateGameObject(L"OBJ_Wall", m_pWall);

		m_pBackWall = make_shared<CWall>(m_pGraphicDev, L"Com_Ruin_BackWallTexture", 1, false);
		m_pBackWall->SetPos(_vec3(RUIN_WALLSIZEX * 2.5f + RUIN_WALLSIZEX * i, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX * 11.f - 0.1f));
		m_pBackWall->SetAngle(_vec3(0.f, PI / 2.f, 0.f));
		m_pBackWall->SetScale(_vec3(RUIN_WALLSIZEX / 2.f, RUIN_WALLSIZEX / 2.f, 1));
		m_pLayer->CreateGameObject(L"OBJ_Back", m_pBackWall);
	}

	for (int i = 0; i < 2; i++)
	{
		m_pWall = make_shared<CWall>(m_pGraphicDev,L"Com_Ruin_WallTexture", 7, true);
		m_pWall->SetPos(_vec3(RUIN_WALLSIZEX * 4.5f + RUIN_PATHSIZEX + RUIN_WALLSIZEX * i, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX * 11.f));
		m_pWall->SetAngle(_vec3(0.f, PI / 2.f, 0.f));
		m_pWall->SetScale(_vec3(RUIN_WALLSIZEX / 2.f, RUIN_WALLSIZEX / 2.f, 1.f));
		m_pLayer->CreateGameObject(L"OBJ_Wall", m_pWall);

		m_pBackWall = make_shared<CWall>(m_pGraphicDev, L"Com_Ruin_BackWallTexture", 1, false);
		m_pBackWall->SetPos(_vec3(RUIN_WALLSIZEX * 4.5f + RUIN_PATHSIZEX + RUIN_WALLSIZEX * i, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX * 11.f - 0.1f));
		m_pBackWall->SetAngle(_vec3(0.f, PI / 2.f, 0.f));
		m_pBackWall->SetScale(_vec3(RUIN_WALLSIZEX / 2.f, RUIN_WALLSIZEX / 2.f, 1));
		m_pLayer->CreateGameObject(L"OBJ_Back", m_pBackWall);
	}

	for (int i = 0; i < 5; i++)
	{
		m_pWall = make_shared<CWall>(m_pGraphicDev,L"Com_Ruin_WallTexture", 7, true);
		m_pWall->SetPos(_vec3(RUIN_WALLSIZEX * 2.5f + RUIN_WALLSIZEX * i, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX * 16.f));
		m_pWall->SetAngle(_vec3(0.f, PI / 2.f, 0.f));
		m_pWall->SetScale(_vec3(RUIN_WALLSIZEX / 2.f, RUIN_WALLSIZEX / 2.f, 1.f));
		m_pLayer->CreateGameObject(L"OBJ_Wall", m_pWall);

		m_pBackWall = make_shared<CWall>(m_pGraphicDev, L"Com_Ruin_BackWallTexture", 1, false);
		m_pBackWall->SetPos(_vec3(RUIN_WALLSIZEX * 2.5f + RUIN_WALLSIZEX * i, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX * 16.f + 0.1f));
		m_pBackWall->SetAngle(_vec3(0.f, PI / 2.f, 0.f));
		m_pBackWall->SetScale(_vec3(RUIN_WALLSIZEX / 2.f, RUIN_WALLSIZEX / 2.f, 1));
		m_pLayer->CreateGameObject(L"OBJ_Back", m_pBackWall);
	}

	// 통로2 벽
	for (int i = 0; i < 8; i++)
	{
		m_pWall = make_shared<CWall>(m_pGraphicDev, L"Com_Ruin_WallTexture", 7, true);
		m_pWall->SetPos(_vec3(RUIN_WALLSIZEX * 7.5f + RUIN_WALLSIZEX * i, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX * 14.f));
		m_pWall->SetAngle(_vec3(0.f, PI / 2.f, 0.f));
		m_pWall->SetScale(_vec3(RUIN_WALLSIZEX / 2.f, RUIN_WALLSIZEX / 2.f, 1.f));
		m_pLayer->CreateGameObject(L"OBJ_Wall", m_pWall);

		m_pBackWall = make_shared<CWall>(m_pGraphicDev, L"Com_Ruin_BackWallTexture", 1, false);
		m_pBackWall->SetPos(_vec3(RUIN_WALLSIZEX * 7.5f + RUIN_WALLSIZEX * i, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX * 14.f + 0.1f));
		m_pBackWall->SetAngle(_vec3(0.f, PI / 2.f, 0.f));
		m_pBackWall->SetScale(_vec3(RUIN_WALLSIZEX / 2.f, RUIN_WALLSIZEX / 2.f, 1));
		m_pLayer->CreateGameObject(L"OBJ_Back", m_pBackWall);
	}

	for (int i = 0; i < 8; i++)
	{
		m_pWall = make_shared<CWall>(m_pGraphicDev, L"Com_Ruin_WallTexture", 7, true);
		m_pWall->SetPos(_vec3(RUIN_WALLSIZEX * 7.5f + RUIN_WALLSIZEX * i, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX * 13.f));
		m_pWall->SetAngle(_vec3(0.f, PI / 2.f, 0.f));
		m_pWall->SetScale(_vec3(RUIN_WALLSIZEX / 2.f, RUIN_WALLSIZEX / 2.f, 1.f));
		m_pLayer->CreateGameObject(L"OBJ_Wall", m_pWall);

		m_pBackWall = make_shared<CWall>(m_pGraphicDev, L"Com_Ruin_BackWallTexture", 1, false);
		m_pBackWall->SetPos(_vec3(RUIN_WALLSIZEX * 7.5f + RUIN_WALLSIZEX * i, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX * 13.f - 0.1f));
		m_pBackWall->SetAngle(_vec3(0.f, PI / 2.f, 0.f));
		m_pBackWall->SetScale(_vec3(RUIN_WALLSIZEX / 2.f, RUIN_WALLSIZEX / 2.f, 1));
		m_pLayer->CreateGameObject(L"OBJ_Back", m_pBackWall);
	}

	// 방 3 벽
	for (int i = 0; i < 2; i++)
	{
		m_pWall = make_shared<CWall>(m_pGraphicDev, L"Com_Ruin_WallTexture", 7, true);
		m_pWall->SetPos(_vec3(RUIN_WALLSIZEX * 15.f, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX / 2.f + RUIN_WALLSIZEX * 11.f + RUIN_WALLSIZEX * i));
		m_pWall->SetAngle(_vec3(0.f, 0.f, 0.f));
		m_pWall->SetScale(_vec3(RUIN_WALLSIZEX / 2.f, RUIN_WALLSIZEX / 2.f, 1.f));
		m_pLayer->CreateGameObject(L"OBJ_Wall", m_pWall);

		m_pBackWall = make_shared<CWall>(m_pGraphicDev, L"Com_Ruin_BackWallTexture", 1, false);
		m_pBackWall->SetPos(_vec3(RUIN_WALLSIZEX * 15.f - 0.1f, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX / 2.f + RUIN_WALLSIZEX * 11.f + RUIN_WALLSIZEX * i));
		m_pBackWall->SetAngle(_vec3(0.f, 0.f, 0.f));
		m_pBackWall->SetScale(_vec3(RUIN_WALLSIZEX / 2.f, RUIN_WALLSIZEX / 2.f, 1));
		m_pLayer->CreateGameObject(L"OBJ_Back", m_pBackWall);
	}

	for (int i = 0; i < 2; i++)
	{
		m_pWall = make_shared<CWall>(m_pGraphicDev, L"Com_Ruin_WallTexture", 7, true);
		m_pWall->SetPos(_vec3(RUIN_WALLSIZEX * 15.f, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX / 2.f + RUIN_WALLSIZEX * 14.f + RUIN_WALLSIZEX * i));
		m_pWall->SetAngle(_vec3(0.f, 0.f, 0.f));
		m_pWall->SetScale(_vec3(RUIN_WALLSIZEX / 2.f, RUIN_WALLSIZEX / 2.f, 1.f));
		m_pLayer->CreateGameObject(L"OBJ_Wall", m_pWall);

		m_pBackWall = make_shared<CWall>(m_pGraphicDev, L"Com_Ruin_BackWallTexture", 1, false);
		m_pBackWall->SetPos(_vec3(RUIN_WALLSIZEX * 15.f - 0.1f, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX / 2.f + RUIN_WALLSIZEX * 14.f + RUIN_WALLSIZEX * i));
		m_pBackWall->SetAngle(_vec3(0.f, 0.f, 0.f));
		m_pBackWall->SetScale(_vec3(RUIN_WALLSIZEX / 2.f, RUIN_WALLSIZEX / 2.f, 1));
		m_pLayer->CreateGameObject(L"OBJ_Back", m_pBackWall);
	}

	for (int i = 0; i < 5; i++)
	{
		m_pWall = make_shared<CWall>(m_pGraphicDev, L"Com_Ruin_WallTexture", 7, true);
		m_pWall->SetPos(_vec3(RUIN_WALLSIZEX * 17.f + RUIN_PATHSIZEX + RUIN_WALLSIZEX * 2.f, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX / 2.f + RUIN_WALLSIZEX * 11.f + RUIN_WALLSIZEX * i));
		m_pWall->SetAngle(_vec3(0.f, 0.f, 0.f));
		m_pWall->SetScale(_vec3(RUIN_WALLSIZEX / 2.f, RUIN_WALLSIZEX / 2.f, 1.f));
		m_pLayer->CreateGameObject(L"OBJ_Wall", m_pWall);

		m_pBackWall = make_shared<CWall>(m_pGraphicDev, L"Com_Ruin_BackWallTexture", 1, false);
		m_pBackWall->SetPos(_vec3(RUIN_WALLSIZEX * 17.f + RUIN_PATHSIZEX + RUIN_WALLSIZEX * 2.f + 0.1f, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX / 2.f + RUIN_WALLSIZEX * 11.f + RUIN_WALLSIZEX * i));
		m_pBackWall->SetAngle(_vec3(0.f, 0.f, 0.f));
		m_pBackWall->SetScale(_vec3(RUIN_WALLSIZEX / 2.f, RUIN_WALLSIZEX / 2.f, 1));
		m_pLayer->CreateGameObject(L"OBJ_Back", m_pBackWall);
	}

	for (int i = 0; i < 5; i++)
	{
		m_pWall = make_shared<CWall>(m_pGraphicDev, L"Com_Ruin_WallTexture", 7, true);
		m_pWall->SetPos(_vec3(RUIN_WALLSIZEX * 15.5f + RUIN_WALLSIZEX * i, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX * 11.f));
		m_pWall->SetAngle(_vec3(0.f, PI / 2.f, 0.f));
		m_pWall->SetScale(_vec3(RUIN_WALLSIZEX / 2.f, RUIN_WALLSIZEX / 2.f, 1.f));
		m_pLayer->CreateGameObject(L"OBJ_Wall", m_pWall);

		m_pBackWall = make_shared<CWall>(m_pGraphicDev, L"Com_Ruin_BackWallTexture", 1, false);
		m_pBackWall->SetPos(_vec3(RUIN_WALLSIZEX * 15.5f + RUIN_WALLSIZEX * i, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX * 11.f - 0.1f));
		m_pBackWall->SetAngle(_vec3(0.f, PI / 2.f, 0.f));
		m_pBackWall->SetScale(_vec3(RUIN_WALLSIZEX / 2.f, RUIN_WALLSIZEX / 2.f, 1));
		m_pLayer->CreateGameObject(L"OBJ_Back", m_pBackWall);
	}

	for (int i = 0; i < 5; i++)
	{
		m_pWall = make_shared<CWall>(m_pGraphicDev, L"Com_Ruin_WallTexture", 7, true);
		m_pWall->SetPos(_vec3(RUIN_WALLSIZEX * 15.5f + RUIN_WALLSIZEX * i, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX * 16.f));
		m_pWall->SetAngle(_vec3(0.f, PI / 2.f, 0.f));
		m_pWall->SetScale(_vec3(RUIN_WALLSIZEX / 2.f, RUIN_WALLSIZEX / 2.f, 1.f));
		m_pLayer->CreateGameObject(L"OBJ_Wall", m_pWall);

		m_pBackWall = make_shared<CWall>(m_pGraphicDev, L"Com_Ruin_BackWallTexture", 1, false);
		m_pBackWall->SetPos(_vec3(RUIN_WALLSIZEX * 15.5f + RUIN_WALLSIZEX * i, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX * 16.f + 0.1f));
		m_pBackWall->SetAngle(_vec3(0.f, PI / 2.f, 0.f));
		m_pBackWall->SetScale(_vec3(RUIN_WALLSIZEX / 2.f, RUIN_WALLSIZEX / 2.f, 1));
		m_pLayer->CreateGameObject(L"OBJ_Back", m_pBackWall);
	}

	// 보스방 벽
	for (int i = 0; i < 5; i++)
	{
		m_pWall = make_shared<CWall>(m_pGraphicDev, L"Com_Ruin_WallTexture", 7, true);
		m_pWall->SetPos(_vec3(RUIN_WALLSIZEX * 15.f, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX / 2.f + RUIN_WALLSIZEX * 0.f + RUIN_WALLSIZEX * i));
		m_pWall->SetAngle(_vec3(0.f, 0.f, 0.f));
		m_pWall->SetScale(_vec3(RUIN_WALLSIZEX / 2.f, RUIN_WALLSIZEX / 2.f, 1.f));
		m_pLayer->CreateGameObject(L"OBJ_Wall", m_pWall);

		m_pBackWall = make_shared<CWall>(m_pGraphicDev, L"Com_Ruin_BackWallTexture", 1, false);
		m_pBackWall->SetPos(_vec3(RUIN_WALLSIZEX * 15.f - 0.1f, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX / 2.f + RUIN_WALLSIZEX * 0.f + RUIN_WALLSIZEX * i));
		m_pBackWall->SetAngle(_vec3(0.f, 0.f, 0.f));
		m_pBackWall->SetScale(_vec3(RUIN_WALLSIZEX / 2.f, RUIN_WALLSIZEX / 2.f, 1));
		m_pLayer->CreateGameObject(L"OBJ_Back", m_pBackWall);
	}

	for (int i = 0; i < 5; i++)
	{
		m_pWall = make_shared<CWall>(m_pGraphicDev, L"Com_Ruin_WallTexture", 7, true);
		m_pWall->SetPos(_vec3(RUIN_WALLSIZEX * 17.f + RUIN_PATHSIZEX + RUIN_WALLSIZEX * 2.f, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX / 2.f + RUIN_WALLSIZEX * 0.f + RUIN_WALLSIZEX * i));
		m_pWall->SetAngle(_vec3(0.f, 0.f, 0.f));
		m_pWall->SetScale(_vec3(RUIN_WALLSIZEX / 2.f, RUIN_WALLSIZEX / 2.f, 1.f));
		m_pLayer->CreateGameObject(L"OBJ_Wall", m_pWall);

		m_pBackWall = make_shared<CWall>(m_pGraphicDev, L"Com_Ruin_BackWallTexture", 1, false);
		m_pBackWall->SetPos(_vec3(RUIN_WALLSIZEX * 17.f + RUIN_PATHSIZEX + RUIN_WALLSIZEX * 2.f + 0.1f, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX / 2.f + RUIN_WALLSIZEX * 0.f + RUIN_WALLSIZEX * i));
		m_pBackWall->SetAngle(_vec3(0.f, 0.f, 0.f));
		m_pBackWall->SetScale(_vec3(RUIN_WALLSIZEX / 2.f, RUIN_WALLSIZEX / 2.f, 1));
		m_pLayer->CreateGameObject(L"OBJ_Back", m_pBackWall);
	}

	for (int i = 0; i < 5; i++)
	{
		m_pWall = make_shared<CWall>(m_pGraphicDev, L"Com_Ruin_WallTexture", 7, true);
		m_pWall->SetPos(_vec3(RUIN_WALLSIZEX * 15.5f + RUIN_WALLSIZEX * i, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX * 0.f));
		m_pWall->SetAngle(_vec3(0.f, PI / 2.f, 0.f));
		m_pWall->SetScale(_vec3(RUIN_WALLSIZEX / 2.f, RUIN_WALLSIZEX / 2.f, 1.f));
		m_pLayer->CreateGameObject(L"OBJ_Wall", m_pWall);

		m_pBackWall = make_shared<CWall>(m_pGraphicDev, L"Com_Ruin_BackWallTexture", 1, false);
		m_pBackWall->SetPos(_vec3(RUIN_WALLSIZEX * 15.5f + RUIN_WALLSIZEX * i, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX * 0.f - 0.1f));
		m_pBackWall->SetAngle(_vec3(0.f, PI / 2.f, 0.f));
		m_pBackWall->SetScale(_vec3(RUIN_WALLSIZEX / 2.f, RUIN_WALLSIZEX / 2.f, 1));
		m_pLayer->CreateGameObject(L"OBJ_Back", m_pBackWall);
	}

	for (int i = 0; i < 5; i++)
	{
		m_pWall = make_shared<CWall>(m_pGraphicDev, L"Com_Ruin_WallTexture", 7, true);
		m_pWall->SetPos(_vec3(RUIN_WALLSIZEX * 15.5f + RUIN_WALLSIZEX * i, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX * 5.f));
		m_pWall->SetAngle(_vec3(0.f, PI / 2.f, 0.f));
		m_pWall->SetScale(_vec3(RUIN_WALLSIZEX / 2.f, RUIN_WALLSIZEX / 2.f, 1.f));
		m_pLayer->CreateGameObject(L"OBJ_Wall", m_pWall);

		m_pBackWall = make_shared<CWall>(m_pGraphicDev, L"Com_Ruin_BackWallTexture", 1, false);
		m_pBackWall->SetPos(_vec3(RUIN_WALLSIZEX * 15.5f + RUIN_WALLSIZEX * i, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX * 5.f + 0.1f));
		m_pBackWall->SetAngle(_vec3(0.f, PI / 2.f, 0.f));
		m_pBackWall->SetScale(_vec3(RUIN_WALLSIZEX / 2.f, RUIN_WALLSIZEX / 2.f, 1));
		m_pLayer->CreateGameObject(L"OBJ_Back", m_pBackWall);
	}

	//가장 최하위 순서에 돌려줄 것
	dynamic_pointer_cast<CLayer>(m_pLayer)->AwakeLayer();


	return S_OK;
}

HRESULT CRuin_Dungeon::Ready_Layer_SkyBox(tstring pLayerTag)
{
	shared_ptr<CLayer> m_pLayer = make_shared<CLayer>();
	m_mapLayer.insert({ pLayerTag, m_pLayer });


	shared_ptr<CGameObject> m_pSkyBox = make_shared<CSkyBox>(m_pGraphicDev, L"Com_Weald_SkyBoxTexture");
	m_pLayer->CreateGameObject(L"OBJ_Weald_SkyBox", m_pSkyBox);

	dynamic_pointer_cast<CLayer>(m_pLayer)->AwakeLayer();

	return S_OK;
}

HRESULT CRuin_Dungeon::Ready_Layer_Camera(tstring pLayerTag)
{
	shared_ptr<CLayer> m_pLayer = make_shared<CLayer>();
	m_mapLayer.insert({ pLayerTag, m_pLayer });

	// Camera
	shared_ptr<CGameObject> m_pCamera = make_shared<CStaticCamera>(m_pGraphicDev);
	m_pLayer->CreateGameObject(L"OBJ_Camera", m_pCamera);

	CCameraMgr::GetInstance()->SetMainCamera(dynamic_pointer_cast<CStaticCamera>(m_pCamera));
	CCameraMgr::GetInstance()->SetFPSMode();

	dynamic_pointer_cast<CLayer>(m_pLayer)->AwakeLayer();

	return S_OK;
}

HRESULT CRuin_Dungeon::Ready_Layer_GameObject(tstring pLayerTag)
{
	shared_ptr<CLayer> m_pLayer = make_shared<CLayer>();
	m_mapLayer.insert({ pLayerTag, m_pLayer });

	//Player
	shared_ptr<CGameObject> m_pPlayer;
	if (m_pPlayer = dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer()))
	{
		// 던전에서 시작해서 넘어온 경우
		m_pPlayer->SetPos({ 70.f, 0.f, 2.f });

		dynamic_pointer_cast<CPlayer>(m_pPlayer)->SetInDungeon(true);
		dynamic_pointer_cast<CTransform>(m_pPlayer->GetComponent(L"Com_Transform", ID_DYNAMIC))->SetPosition(m_pPlayer->GetPos().x, m_pPlayer->GetPos().y, m_pPlayer->GetPos().z);
		dynamic_pointer_cast<CTransform>(m_pPlayer->GetComponent(L"Com_Transform", ID_DYNAMIC))->SetAngle({ 0.f, 0.f, 0.f });

		for (auto& iter : *dynamic_pointer_cast<CPlayer>(m_pPlayer)->GetHeroVec())
		{
			m_pLayer->CreateGameObject(dynamic_pointer_cast<CHero>(iter)->GetObjKey(), iter);
		}

		dynamic_pointer_cast<CPlayer>(m_pPlayer)->SetInBattle(false);
	}

	for (auto& iter : *dynamic_pointer_cast<CPlayer>(m_pPlayer)->GetHeroVec())
	{
		dynamic_pointer_cast<CHero>(iter)->SetInDungeon(true);
	}

	//// GameObjects
	// 비밀문
	shared_ptr<CGameObject> m_pSecretDoor = make_shared<CRuinSecretDoor>(m_pGraphicDev);
	m_pSecretDoor->SetPos(_vec3(RUIN_WALLSIZEX * 13.f + RUIN_WALLSIZEX / 2.f + RUIN_WALLSIZEX * 1.f, RUIN_WALLSIZEUPY + 1.f, RUIN_WALLSIZEX * 13.5f));
	m_pSecretDoor->SetAngle(_vec3(0.f, PI / 2.f, 0.f));
	m_pSecretDoor->SetScale(_vec3(RUIN_PATHSIZEX / 2.f, RUIN_PATHSIZEX / 2.f, 1.f));
	
	// 함정
	shared_ptr<CGameObject>m_pTrap1 = make_shared<CRuin_Trap>(m_pGraphicDev);
	m_pTrap1->SetPos(_vec3(RUIN_WALLSIZEX * 4.5f, RUIN_WALLSIZEUPY + 1.f - 2.5f, RUIN_WALLSIZEX * 7.f));
	m_pTrap1->SetAngle(_vec3(0.f, 0.f, 0.f));
	m_pTrap1->SetScale(_vec3(RUIN_PATHSIZEX / 3.f, RUIN_PATHSIZEX / 3.f, 1.f));

	// 장애물
	shared_ptr<CGameObject>m_pObstacle1 = make_shared<CRuin_Obstacle>(m_pGraphicDev);
	m_pObstacle1->SetPos(_vec3(RUIN_WALLSIZEX * 4.5f, RUIN_WALLSIZEUPY + 2.f, RUIN_WALLSIZEX * 5.f));
	m_pObstacle1->SetAngle(_vec3(0.f, 0.f, 0.f));
	m_pObstacle1->SetScale(_vec3(RUIN_PATHSIZEX/2.f, RUIN_PATHSIZEX / 2.f, 1.f));

	shared_ptr<CGameObject>m_pObstacle2 = make_shared<CRuin_Obstacle>(m_pGraphicDev);
	m_pObstacle2->SetPos(_vec3(RUIN_WALLSIZEX * 4.5f, RUIN_WALLSIZEUPY + 2.f, RUIN_WALLSIZEX * 9.f));
	m_pObstacle2->SetAngle(_vec3(0.f, 0.f, 0.f));
	m_pObstacle2->SetScale(_vec3(RUIN_PATHSIZEX / 2.f, RUIN_PATHSIZEX / 2.f, 1.f));

	//shared_ptr<CGameObject>m_pTrap1 = make_shared<CRuin_Trap>(m_pGraphicDev);
	//m_pTrap1->SetPos(_vec3(RUIN_WALLSIZEX * 9.f, RUIN_WALLSIZEUPY + 1.f - 2.5f, RUIN_WALLSIZEX * 13.5f));
	//m_pTrap1->SetAngle(_vec3(0.f, PI / 2.f, 0.f));
	//m_pTrap1->SetScale(_vec3(RUIN_PATHSIZEX / 3.f, RUIN_PATHSIZEX / 3.f, 1.f));

	// 골동품
	// 갑옷
	shared_ptr<CGameObject>m_pCurioA1 = make_shared<CRuin_Curio_Armor>(m_pGraphicDev);
	m_pCurioA1->SetPos(_vec3(RUIN_WALLSIZEX * 4.5f, RUIN_WALLSIZEUPY, RUIN_WALLSIZEX * 2.f));
	m_pCurioA1->SetAngle(_vec3(0.f, 0.f, 0.f));
	m_pCurioA1->SetScale(_vec3(RUIN_WALLSIZEX / 7.f, (RUIN_WALLSIZEX / 7.f) / 165.f * 347.f, 1.f));
	
	// 신성한 호수
	shared_ptr<CGameObject>m_pCurioF1 = make_shared<CRuin_Curio_Fountain>(m_pGraphicDev);
	m_pCurioF1->SetPos(_vec3(RUIN_WALLSIZEX * 4.5f, RUIN_WALLSIZEUPY - 1.5f, RUIN_WALLSIZEX * 13.5f));
	m_pCurioF1->SetAngle(_vec3(0.f, 0.f, 0.f));
	m_pCurioF1->SetScale(_vec3(RUIN_WALLSIZEX / 4.f, (RUIN_WALLSIZEX / 4.f) / 388.f * 345.f, 1.f));


	D3DLIGHT9 _pLightInfo1;

	_pLightInfo1.Diffuse = { 0.5f , 0.5f , 0.5f , 1.f };
	_pLightInfo1.Specular = { 1.f , 1.f , 1.f , 1.f };
	_pLightInfo1.Ambient = { 1.f , 1.f , 1.f , 1.f };
	_pLightInfo1.Position = { RUIN_WALLSIZEX * 4.5f , RUIN_WALLSIZEUPY - 1.5f + 4.f ,RUIN_WALLSIZEX * 13.5f + 10.f };
	_pLightInfo1.Range = 20.f;

	_pLightInfo1.Attenuation0 = 0.f;
	_pLightInfo1.Attenuation1 = 0.f;
	_pLightInfo1.Attenuation2 = 0.f;

	CLightMgr::GetInstance()->InitPointLight(m_pGraphicDev, _pLightInfo1, 99);


	// 보물상자
	shared_ptr<CGameObject>m_pCurioT1 = make_shared<CRuin_Curio_Sarcophagus>(m_pGraphicDev);
	m_pCurioT1->SetPos(_vec3(RUIN_WALLSIZEX * 17.5f, RUIN_WALLSIZEUPY - 3.f, RUIN_WALLSIZEX * 13.5f));
	m_pCurioT1->SetAngle(_vec3(0.f, 0.f, 0.f));
	m_pCurioT1->SetScale(_vec3(RUIN_WALLSIZEX / 5.f, (RUIN_WALLSIZEX / 5.f) / 381.f * 265.f, 1.f));

	_vec3 vT1Pos = _vec3(RUIN_WALLSIZEX * 17.5f, RUIN_WALLSIZEUPY - 3.f, RUIN_WALLSIZEX * 13.5f);

	//스태츄
	shared_ptr<CGameObject>m_pCurioSt1 = make_shared<CRuin_Curio_Statue>(m_pGraphicDev , StatueState::STATUE_BLUE);
	m_pCurioSt1->SetPos(_vec3(vT1Pos.x , ((RUIN_WALLSIZEX / 5.f) / 500.f * 670.f) * 0.5f + 1.5f, vT1Pos.z + 15.f));
	m_pCurioSt1->SetAngle(_vec3(0.f, 0.f, 0.f));
	m_pCurioSt1->SetScale(_vec3(RUIN_WALLSIZEX / 5.f, (RUIN_WALLSIZEX / 5.f) / 500.f * 670.f, 1.f));

	shared_ptr<CGameObject>m_pCurioSt2 = make_shared<CRuin_Curio_Statue>(m_pGraphicDev, StatueState::STATUE_RED);
	m_pCurioSt2->SetPos(_vec3(vT1Pos.x + 15.f, ((RUIN_WALLSIZEX / 5.f) / 500.f * 670.f) * 0.5f + 1.5f, vT1Pos.z));
	m_pCurioSt2->SetAngle(_vec3(0.f, 0.f, 0.f));
	m_pCurioSt2->SetScale(_vec3(RUIN_WALLSIZEX / 5.f, (RUIN_WALLSIZEX / 5.f) / 500.f * 670.f, 1.f));

	shared_ptr<CGameObject>m_pCurioSt3 = make_shared<CRuin_Curio_Statue>(m_pGraphicDev, StatueState::STATUE_GREEN);
	m_pCurioSt3->SetPos(_vec3(vT1Pos.x,( (RUIN_WALLSIZEX / 5.f) / 500.f * 670.f ) *0.5f + 1.5f, vT1Pos.z - 15.f));
	m_pCurioSt3->SetAngle(_vec3(0.f, 0.f, 0.f));
	m_pCurioSt3->SetScale(_vec3(RUIN_WALLSIZEX / 5.f, (RUIN_WALLSIZEX / 5.f) / 500.f * 670.f, 1.f));

	dynamic_pointer_cast<CRuin_Curio_Sarcophagus>(m_pCurioT1)->InsertStatue(dynamic_pointer_cast<CRuin_Curio_Statue>(m_pCurioSt1));
	dynamic_pointer_cast<CRuin_Curio_Sarcophagus>(m_pCurioT1)->InsertStatue(dynamic_pointer_cast<CRuin_Curio_Statue>(m_pCurioSt2));
	dynamic_pointer_cast<CRuin_Curio_Sarcophagus>(m_pCurioT1)->InsertStatue(dynamic_pointer_cast<CRuin_Curio_Statue>(m_pCurioSt3));

	// 횃불
	shared_ptr<CGameObject>m_pCurioS1 = make_shared<CRuin_Curio_Sconce>(m_pGraphicDev);
	m_pCurioS1->SetPos(_vec3(RUIN_WALLSIZEX * 9.f, RUIN_WALLSIZEUPY - 1.f, RUIN_WALLSIZEX * 13.5f));
	m_pCurioS1->SetAngle(_vec3(0.f, 0.f, 0.f));
	m_pCurioS1->SetScale(_vec3(RUIN_WALLSIZEX / 15.f, (RUIN_WALLSIZEX / 15.f) / 116.f * 390.f, 1.f));

	shared_ptr<CGameObject>m_pCurioS2 = make_shared<CRuin_Curio_Sconce>(m_pGraphicDev);
	m_pCurioS2->SetPos(_vec3(RUIN_WALLSIZEX * 11.f, RUIN_WALLSIZEUPY - 1.f, RUIN_WALLSIZEX * 13.5f));
	m_pCurioS2->SetAngle(_vec3(0.f, 0.f, 0.f));
	m_pCurioS2->SetScale(_vec3(RUIN_WALLSIZEX / 15.f, (RUIN_WALLSIZEX / 15.f) / 116.f * 390.f, 1.f));

	shared_ptr<CGameObject>m_pCurioS3 = make_shared<CRuin_Curio_Sconce>(m_pGraphicDev);
	m_pCurioS3->SetPos(_vec3(RUIN_WALLSIZEX * 13.f, RUIN_WALLSIZEUPY - 1.f, RUIN_WALLSIZEX * 13.5f));
	m_pCurioS3->SetAngle(_vec3(0.f, 0.f, 0.f));
	m_pCurioS3->SetScale(_vec3(RUIN_WALLSIZEX / 15.f, (RUIN_WALLSIZEX / 15.f) / 116.f * 390.f, 1.f));



	// items
	// 4번방
	shared_ptr<CGameObject> m_pItem1_4 = make_shared<CItem>(m_pGraphicDev);
	m_pItem1_4->SetColliding(true);
	m_pLayer->CreateGameObject(L"Obj_TestItem11", m_pItem1_4);
	dynamic_pointer_cast<CItem>(m_pItem1_4)->SetDropItemInfo({ 56.f, -50.f, 14.f }, L"Item_Torch", 1);

	shared_ptr<CGameObject> m_pItem2_4 = make_shared<CItem>(m_pGraphicDev);
	m_pItem2_4->SetColliding(true);
	m_pLayer->CreateGameObject(L"Obj_TestItem12", m_pItem2_4);
	dynamic_pointer_cast<CItem>(m_pItem2_4)->SetDropItemInfo({ 60.f, -50.f, 14.f }, L"Item_Torch", 1);

	shared_ptr<CGameObject> m_pItem3_4 = make_shared<CItem>(m_pGraphicDev);
	m_pItem3_4->SetColliding(true);
	m_pLayer->CreateGameObject(L"Obj_TestItem13", m_pItem3_4);
	dynamic_pointer_cast<CItem>(m_pItem3_4)->SetDropItemInfo({ 64.f, -50.f, 14.f }, L"Item_Torch", 1);

	// 5번방
	shared_ptr<CGameObject> m_pItem1_5 = make_shared<CItem>(m_pGraphicDev);
	m_pItem1_5->SetColliding(true);
	m_pLayer->CreateGameObject(L"Obj_TestItem13", m_pItem1_5);
	dynamic_pointer_cast<CItem>(m_pItem1_5)->SetDropItemInfo({ 64.f, -50.f, 14.f }, L"Player_Item_RedGem", 1);

	shared_ptr<CGameObject> m_pItem2_5 = make_shared<CItem>(m_pGraphicDev);
	m_pItem2_5->SetColliding(true);
	m_pLayer->CreateGameObject(L"Obj_TestItem13", m_pItem2_5);
	dynamic_pointer_cast<CItem>(m_pItem2_5)->SetDropItemInfo({ 64.f, -50.f, 14.f }, L"Player_Item_GreenGem", 1);

	shared_ptr<CGameObject> m_pItem3_5 = make_shared<CItem>(m_pGraphicDev);
	m_pItem3_5->SetColliding(true);
	m_pLayer->CreateGameObject(L"Obj_TestItem13", m_pItem3_5);
	dynamic_pointer_cast<CItem>(m_pItem3_5)->SetDropItemInfo({ 64.f, -50.f, 14.f }, L"Player_Item_BlueGem", 1);

	shared_ptr<CGameObject> m_pItem4_5 = make_shared<CItem>(m_pGraphicDev);
	m_pItem4_5->SetColliding(true);
	m_pLayer->CreateGameObject(L"Obj_TestItem13", m_pItem4_5);
	dynamic_pointer_cast<CItem>(m_pItem4_5)->SetDropItemInfo({ 64.f, -50.f, 14.f }, L"Player_Item_Bandage", 1);

	// monsters
	shared_ptr<CGameObject> m_pBoneDefender1 = make_shared<CBoneDefender>(m_pGraphicDev);
	shared_ptr<CGameObject> m_pBoneSoldier_1 = make_shared<CBoneSoldier>(m_pGraphicDev);
	shared_ptr<CGameObject> m_pBoneSoldier_2 = make_shared<CBoneSoldier>(m_pGraphicDev);
	shared_ptr<CGameObject> m_pBoneCourtier_1 = make_shared<CBoneCourtier>(m_pGraphicDev);

	// heroes
	//shared_ptr<CGameObject> m_pSheldBreaker1 = make_shared<CShieldBreaker>(m_pGraphicDev);
	//shared_ptr<CGameObject> m_pHighwayman = make_shared<CHighwayman>(m_pGraphicDev);
	//shared_ptr<CGameObject> m_pJester = make_shared<CJester>(m_pGraphicDev);
	//shared_ptr<CGameObject> m_pVestal = make_shared<CVestal>(m_pGraphicDev);

	vector<shared_ptr<CGameObject>> Heroes_v;
	for (auto& iter : *dynamic_pointer_cast<CPlayer>(m_pPlayer)->GetHeroVec())
	{
		Heroes_v.push_back(iter);
	}
	//Heroes_v.push_back(m_pSheldBreaker1);
	//Heroes_v.push_back(m_pHighwayman);
	//Heroes_v.push_back(m_pJester);
	//Heroes_v.push_back(m_pVestal);
	//dynamic_pointer_cast<CPlayer>(m_pPlayer)->SetHeroVec(&Heroes_v);

	// 방에 GameObject 넣기
	// Room1
	vector<shared_ptr<CGameObject>> Room1_v1;
	Room1_v1.push_back(m_pCurioA1);
	m_pRoom1->PushGameObjectVector(Room1_v1);

	// Room2
	vector<shared_ptr<CGameObject>> Room2_v1;
	Room2_v1.push_back(m_pTrap1);
	Room2_v1.push_back(m_pObstacle1);
	Room2_v1.push_back(m_pObstacle2);
	m_pRoom2->PushGameObjectVector(Room2_v1);

	// Room3
	// objects
	vector<shared_ptr<CGameObject>> Room3_v1;
	Room3_v1.push_back(m_pBoneDefender1);
	Room3_v1.push_back(m_pBoneSoldier_1);
	Room3_v1.push_back(m_pBoneSoldier_2);
	Room3_v1.push_back(m_pBoneCourtier_1);

	Room3_v1.push_back(m_pCurioF1);

	m_pRoom3->PushGameObjectVector(Room3_v1);

	// creatures
	vector<shared_ptr<CGameObject>> Room3_v4;
	Room3_v4.push_back(m_pBoneDefender1);
	Room3_v4.push_back(m_pBoneSoldier_1);
	Room3_v4.push_back(m_pBoneSoldier_2);
	Room3_v4.push_back(m_pBoneCourtier_1);

	for (auto& iter : *dynamic_pointer_cast<CPlayer>(m_pPlayer)->GetHeroVec())
	{
		Room3_v4.push_back(iter);
	}

	//Room3_v4.push_back(m_pSheldBreaker1);
	//Room3_v4.push_back(m_pHighwayman);
	//Room3_v4.push_back(m_pJester);
	//Room3_v4.push_back(m_pVestal);

	// heroes
	vector<shared_ptr<CGameObject>> Room3_v2;
	for (auto& iter : *dynamic_pointer_cast<CPlayer>(m_pPlayer)->GetHeroVec())
	{
		Room3_v2.push_back(iter);
	}
	//Room3_v2.push_back(m_pSheldBreaker1);
	//Room3_v2.push_back(m_pHighwayman);
	//Room3_v2.push_back(m_pVestal);
	//Room3_v2.push_back(m_pJester);
	m_pRoom3->PushHeroesVector(Room3_v2);

	// monsters
	vector<shared_ptr<CGameObject>> Room3_v3;
	Room3_v3.push_back(m_pBoneDefender1);
	Room3_v3.push_back(m_pBoneSoldier_1);
	Room3_v3.push_back(m_pBoneSoldier_2);
	Room3_v3.push_back(m_pBoneCourtier_1);
	m_pRoom3->PushMonstersVector(Room3_v3);

	// 배틀시스템 넣기
	shared_ptr<CBattleSystem> pRoom3_Battle = make_shared<CBattleSystem>();
	//pRoom3_Battle->Ready();
	pRoom3_Battle->PushCreaturesVector(Room3_v4);
	pRoom3_Battle->PushHeroesVector(Room3_v2);
	pRoom3_Battle->PushMonstersVector(Room3_v3);
	m_pRoom3->SetBattleSystem(pRoom3_Battle);
	m_pRoom3->SetBattleCameraPos(_vec3(RUIN_WALLSIZEX * 4.5f, 3.f, RUIN_WALLSIZEX * 13.5f));

	//BattleUI Test
	shared_ptr<CBattleHeroUI> m_pHeroUI = make_shared<CBattleHeroUI>(m_pGraphicDev);
	m_pHeroUI->AwakeGameObject();
	m_pHeroUI->ReadyGameObject();
	CUIMgr::GetInstance()->AddUIObject(L"Battle_Hero_UI", dynamic_pointer_cast<CUIObj>(m_pHeroUI));
	pRoom3_Battle->GetHeroPanelUI(m_pHeroUI);

	// Room4
	vector<shared_ptr<CGameObject>> Room4_v1;
	Room4_v1.push_back(m_pCurioS1);
	Room4_v1.push_back(m_pCurioS2);
	Room4_v1.push_back(m_pCurioS3);
	Room4_v1.push_back(m_pItem1_4);
	Room4_v1.push_back(m_pItem2_4);
	Room4_v1.push_back(m_pItem3_4);
	Room4_v1.push_back(m_pSecretDoor);
	m_pRoom4->PushGameObjectVector(Room4_v1);

	// Room5
	vector<shared_ptr<CGameObject>> Room5_v1;
	Room5_v1.push_back(m_pCurioT1);
	Room5_v1.push_back(m_pItem1_5);
	Room5_v1.push_back(m_pItem2_5);
	Room5_v1.push_back(m_pItem3_5);
	Room5_v1.push_back(m_pItem4_5);
	Room5_v1.push_back(m_pCurioSt1);
	Room5_v1.push_back(m_pCurioSt2);
	Room5_v1.push_back(m_pCurioSt3);

	m_pRoom5->PushGameObjectVector(Room5_v1);

	// 던전에 방 넣기
	vector<shared_ptr<CDungeonRoom>> Dungeon1_v;
	Dungeon1_v.push_back(m_pRoom1);
	Dungeon1_v.push_back(m_pRoom2);
	Dungeon1_v.push_back(m_pRoom3);
	Dungeon1_v.push_back(m_pRoom4);
	Dungeon1_v.push_back(m_pRoom5);
	Dungeon1_v.push_back(m_pRoom6);
	m_pRuinDungeon->PushDungeonRoomVector(Dungeon1_v);
	
	// 현재 active 방
	m_pRuinDungeon->DisableAllRoom();
	m_pRuinDungeon->AbleRoom(1);
	dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SetCurrentRoom(1);

	// Layer에 GameObject 넣기
	m_pLayer->CreateGameObject(L"Obj_BrigandCutthroat", m_pBoneDefender1);
	m_pLayer->CreateGameObject(L"Obj_BrigandCutthroat", m_pBoneSoldier_1);
	m_pLayer->CreateGameObject(L"Obj_BrigandBloodletter", m_pBoneSoldier_2);
	m_pLayer->CreateGameObject(L"Obj_BrigandFusilier", m_pBoneCourtier_1);
	//m_pLayer->CreateGameObject(L"Obj_ShieldBreaker", m_pSheldBreaker1);
	//dynamic_pointer_cast<CHero>(m_pSheldBreaker1)->SetObjKey(L"Obj_ShieldBreaker");
	//m_pLayer->CreateGameObject(L"Obj_Highwayman", m_pHighwayman);
	//dynamic_pointer_cast<CHero>(m_pHighwayman)->SetObjKey(L"Obj_Highwayman");
	//m_pLayer->CreateGameObject(L"Obj_Jester", m_pJester);
	//dynamic_pointer_cast<CHero>(m_pJester)->SetObjKey(L"Obj_Jester");
	//m_pLayer->CreateGameObject(L"Obj_Vestal", m_pVestal);
	//dynamic_pointer_cast<CHero>(m_pVestal)->SetObjKey(L"Obj_Vestal");
	m_pLayer->CreateGameObject(L"OBJ_Trap", m_pTrap1);
	m_pLayer->CreateGameObject(L"OBJ_Obstacle", m_pObstacle1);
	m_pLayer->CreateGameObject(L"OBJ_Obstacle", m_pObstacle2);
	m_pLayer->CreateGameObject(L"OBJ_CurioArmor", m_pCurioA1);
	m_pLayer->CreateGameObject(L"OBJ_CurioFountain", m_pCurioF1);
	m_pLayer->CreateGameObject(L"OBJ_CurioSconce", m_pCurioS1);
	m_pLayer->CreateGameObject(L"OBJ_CurioSconce", m_pCurioS2);
	m_pLayer->CreateGameObject(L"OBJ_CurioSconce", m_pCurioS3);
	m_pLayer->CreateGameObject(L"OBJ_CurioSarcophagus", m_pCurioT1);

	m_pLayer->CreateGameObject(L"OBJ_SecretDoor", m_pSecretDoor);

	m_pLayer->CreateGameObject(L"OBJ_CurioStatue1", m_pCurioSt1);
	m_pLayer->CreateGameObject(L"OBJ_CurioStatue2", m_pCurioSt2);
	m_pLayer->CreateGameObject(L"OBJ_CurioStatue3", m_pCurioSt3);


	//PlayerObj
	m_pLayer->CreateGameObject(L"Obj_Player", m_pPlayer);
	m_pLayer->CreateGameObject(L"Obj_PlayerHand", dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->GetPlayerHand());

	dynamic_pointer_cast<CLayer>(m_pLayer)->AwakeLayer();

	return S_OK;
}

HRESULT CRuin_Dungeon::Ready_Layer_UI(tstring pLayerTag)
{
	shared_ptr<CLayer> m_pLayer = make_shared<CLayer>();
	m_mapLayer.insert({ pLayerTag, m_pLayer });


	shared_ptr<CGameObject> pDescriptionUI = make_shared<CPuzzleHint>(m_pGraphicDev);
	m_pLayer->CreateGameObject(L"Obj_PuzzleHintUI", pDescriptionUI);

	CUIMgr::GetInstance()->AddUIObject(L"UI_PuzzleHint", dynamic_pointer_cast<CUIObj>(pDescriptionUI));

	shared_ptr<CGameObject> pDungeonStatusUI = make_shared<CDungeonStatus>(m_pGraphicDev);
	m_pLayer->CreateGameObject(L"Obj_DungeonStatus", pDungeonStatusUI);
	CUIMgr::GetInstance()->AddUIObject(L"UI_DungeonStatus", dynamic_pointer_cast<CUIObj>(pDungeonStatusUI));

	dynamic_pointer_cast<CLayer>(m_pLayer)->AwakeLayer();

	return S_OK;
}
