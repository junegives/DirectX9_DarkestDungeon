#include "pch.h"
#include "Weald_Dungeon.h"
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
#include "Description.h"

#include"GameMgr.h"
#include"CameraMgr.h"

#include"Village.h"
#include"Ruin_Dungeon.h"

#include "WealdDungeonDoor.h"
#include "Weald_Trap.h"
#include "Weald_Obstacle.h"
#include "Weald_Curio_Chest.h"
#include "Weald_Curio_Luggage.h"
#include "Weald_Curio_Spider.h"
#include "Weald_Curio_Tent.h"

#include "BrigandCutthroat.h"
#include "BrigandFusilier.h"
#include "BrigandBloodletter.h"

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
#include"MouseCursor.h"

#include"Narration.h"
#include "TextBoard.h"

#include"PlayerFPSUI.h"

#include"LoadingScreen.h"
#include"InteractionInfo.h"

#include "DungeonStatus.h"
#include "ScreenEffect.h"

CWeald_Dungeon::CWeald_Dungeon(LPDIRECT3DDEVICE9 pGraphicDev)
	:CScene(pGraphicDev)
{
	ZeroMemory(m_szString, sizeof(m_szString));
}

CWeald_Dungeon::~CWeald_Dungeon()
{
}

HRESULT CWeald_Dungeon::ReadyScene()
{
	CResourceMgr::GetInstance()->WealdDungeonTextureLoad();

	// text
	lstrcpy(m_szString, L"Weald Dungeon");
	// Weald 던전
	m_pWealdDungeon = make_shared<CDungeonSystem>();
	// 던전 방1
	m_pRoom1 = make_shared<CDungeonRoom>();
	m_pRoom2 = make_shared<CDungeonRoom>();
	m_pRoom3 = make_shared<CDungeonRoom>();
	m_pRoom4 = make_shared<CDungeonRoom>();
	m_pRoom5 = make_shared<CDungeonRoom>();

	Ready_Layer_Environment(L"Layer_3_Environment");
	Ready_Layer_SkyBox(L"Layer_1_SkyBox");
	Ready_Layer_GameObject(L"Layer_4_GameObj");
	Ready_Layer_UI(L"Layer_2_UI");
	Ready_Layer_Camera(L"Layer_5_Camera");

	for (auto& iter : m_mapLayer) {
		//GameComponenet Setting
		iter.second->ReadyLayer();
	}

	SetLight();

	return __super::ReadyScene();
}

_int CWeald_Dungeon::UpdateScene(const _float& fTimeDelta)
{
	if (!m_binitNarrOn) {
		CUIMgr::GetInstance()->NarrationOn(L"Narr_tut_firstdungeon");
		CSoundMgr::GetInstance()->PlayBGM(L"Amb_Weald_Base.wav", 1.f);
		m_binitNarrOn = true;
	}


	// 방 이동했는지 체크
	if (dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->GetRoomChange())
	{
		dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SetRoomChange(false);
		m_pWealdDungeon->DisableAllRoom();
		m_pWealdDungeon->AbleRoom(dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->GetCurrentRoom());
	}

	// 세번째 방 전투 트리거
	if (dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->GetCurrentRoom() == 3 && !m_pRoom3->GetBattleStart())
	{
		shared_ptr<CTransform> pTransform = dynamic_pointer_cast<CTransform>((CGameMgr::GetInstance()->GetPlayer())->GetComponent(L"Com_Transform", ID_DYNAMIC));

		if (pTransform->GetPos()->z > 200.f)
		{
			_vec3* vPos = pTransform->GetPos();
			CCameraMgr::GetInstance()->SetState(ECameraMode::BATTLE);
			CCameraMgr::GetInstance()->MovingStraight(ECameraMode::ZOOMIN, { vPos->x , vPos->y+5.f , 210.f });
			CUIMgr::GetInstance()->SelectUIVisibleOff(L"UI_Inventory");
			CUIMgr::GetInstance()->SelectUIVisibleOff(L"UI_DungeonStatus");


			//전투 시작 이펙트

			shared_ptr<CEffect> pEffect = CEffectMgr::GetInstance()->GetEffect();
			if (pEffect) {
				pEffect->SetAnimEffect(L"Effect_BattleStart", _vec3(vPos->x, vPos->y +8.f, 220.f), _vec3(3.5f, 3.5f, 3.5f), 1.5f, false);
				pEffect->SetActive(true);
			}

			//전투 조명 on
			CLightMgr::GetInstance()->DungeonBattleLightOn();

			dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SetBattleTrigger(true);
			pTransform->SetPosition(WEALD_WALLSIZEX + WEALD_PATHSIZEX + 21.3f, 0.f, 210.f);
			m_pRoom3->SetBattleStart(true);
			CSoundMgr::GetInstance()->PlayBGM(L"Combat_Level2_Loop2.wav", 0.6f);
			CUIMgr::GetInstance()->NarrationOn(L"Narr_tut_firstBattle");
			CSoundMgr::GetInstance()->PlaySound(L"Gen_BattleStart.wav", CHANNELID::EFFECT, 1.f);
			dynamic_pointer_cast<CInteractionInfo>(CUIMgr::GetInstance()->FindUI(L"UI_InteractionInfo"))->SetIsBattle(true);
			m_pRoom3->SetBattleCameraOriginPos({ vPos->x , vPos->y + 5.f , 210.f });
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
				//SOUND 원상복구
				CSoundMgr::GetInstance()->StopAll();
				CSoundMgr::GetInstance()->PlayBGM(L"Amb_Weald_Base.wav", 1.f);

				//카메라 원상복구
				CCameraMgr::GetInstance()->SetFPSMode();

				//UI 원상복구
				CUIMgr::GetInstance()->SelectUIVisibleOn(L"UI_Inventory");
				CUIMgr::GetInstance()->SelectUIVisibleOn(L"UI_DungeonStatus");
				CUIMgr::GetInstance()->SelectUIVisibleOff(L"Battle_Hero_UI");
				dynamic_pointer_cast<CInteractionInfo>(CUIMgr::GetInstance()->FindUI(L"UI_InteractionInfo"))->SetIsBattle(false);

				//조명 원상복구
				CLightMgr::GetInstance()->DungeonBattleLightOff();

				m_pRoom3->SetBattleTrigger(false);
				dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SetInBattle(false);
			}
		}
	}

	// 전리품 드롭
	// 1번방
	if (dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->GetCurrentRoom() == 1)
	{
		if(dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->GetEventTrigger())
		{
			for (int i = 0; i < size(m_pRoom1->GetGameObjectVector()); i++)
			{
				float fNum = 1.f / (float)(rand() % 10 + 1) * 5.f;
				if (dynamic_pointer_cast<CItem>(m_pRoom1->GetGameObjectVector()[i]))
				{
					shared_ptr<CTransform> pTransform = dynamic_pointer_cast<CTransform>(m_pRoom1->GetGameObjectVector()[i]->GetComponent(L"Com_Transform", ID_DYNAMIC));
					pTransform->SetPosition(56.f + fNum - 0.5f, 1.f, 16.f - 0.01f - 0.01f*i);
					dynamic_pointer_cast<CItem>(m_pRoom1->GetGameObjectVector()[i])->StartParticle();
				}
			}
			dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SetEventTrigger(false);
		}
	}

	// 2번방
	if (dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->GetCurrentRoom() == 2)
	{
		// 붕대로 열었을때
		if (dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->GetEventTrigger())
		{
			for (int i = 0; i < size(m_pRoom2->GetGameObjectVector()); i++)
			{
				float fNum = 1.f / (float)(rand() % 10 + 1) * 5.f;
				if (dynamic_pointer_cast<CItem>(m_pRoom2->GetGameObjectVector()[i]))
				{
					shared_ptr<CTransform> pTransform = dynamic_pointer_cast<CTransform>(m_pRoom2->GetGameObjectVector()[i]->GetComponent(L"Com_Transform", ID_DYNAMIC));
					pTransform->SetPosition(WEALD_WALLSIZEX * 2.f + WEALD_WALLSIZEX / 2.f + WEALD_WALLSIZEX * 1.f + fNum - 0.5f, 1.f, WEALD_WALLSIZEX * 2.f + WEALD_WALLSIZEX * 9.f - 0.01f - 0.01f * i);
					dynamic_pointer_cast<CItem>(m_pRoom2->GetGameObjectVector()[i])->StartParticle();
				}
			}
			dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SetEventTrigger(false);
		}

		// 걍 열었을때
		if (dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->GetEvent2Trigger())
		{
			for (int i = 0; i < size(m_pRoom2->GetGameObjectVector()); i++)
			{
				if (i == 1) break;
				float fNum = 1.f / (float)(rand() % 10 + 1) * 5.f;
				if (dynamic_pointer_cast<CItem>(m_pRoom2->GetGameObjectVector()[i]))
				{
					shared_ptr<CTransform> pTransform = dynamic_pointer_cast<CTransform>(m_pRoom2->GetGameObjectVector()[i]->GetComponent(L"Com_Transform", ID_DYNAMIC));
					pTransform->SetPosition(WEALD_WALLSIZEX * 2.f + WEALD_WALLSIZEX / 2.f + WEALD_WALLSIZEX * 1.f + fNum - 0.5f, 1.f, WEALD_WALLSIZEX * 2.f + WEALD_WALLSIZEX * 9.f - 0.01f - 0.01f * i);
					dynamic_pointer_cast<CItem>(m_pRoom2->GetGameObjectVector()[i])->StartParticle();
				}
			}
			dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SetEvent2Trigger(false);
		}
	}

	// 3번방
	if (dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->GetCurrentRoom() == 3)
	{
		// 키로 열었을때
		if (dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->GetEventTrigger())
		{
			for (int i = 0; i < size(m_pRoom3->GetGameObjectVector()); i++)
			{
				float fNum = 1.f / (float)(rand() % 10 + 1) * 5.f;
				if (dynamic_pointer_cast<CItem>(m_pRoom3->GetGameObjectVector()[i]))
				{
					shared_ptr<CTransform> pTransform = dynamic_pointer_cast<CTransform>(m_pRoom3->GetGameObjectVector()[i]->GetComponent(L"Com_Transform", ID_DYNAMIC));
					pTransform->SetPosition(WEALD_WALLSIZEX + WEALD_PATHSIZEX + 22.f + fNum - 0.5f, 1.f, 230.f - 0.01f - 0.01f * i);
					dynamic_pointer_cast<CItem>(m_pRoom3->GetGameObjectVector()[i])->StartParticle();
				}
			}
			dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SetEventTrigger(false);
		}

		// 걍 열었을때
		if (dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->GetEvent2Trigger())
		{
			for (int i = 0; i < size(m_pRoom3->GetGameObjectVector()); i++)
			{
				if (i == 1) break;
				float fNum = 1.f / (float)(rand() % 10 + 1) * 5.f;
				if (dynamic_pointer_cast<CItem>(m_pRoom3->GetGameObjectVector()[i]))
				{
					shared_ptr<CTransform> pTransform = dynamic_pointer_cast<CTransform>(m_pRoom3->GetGameObjectVector()[i]->GetComponent(L"Com_Transform", ID_DYNAMIC));
					pTransform->SetPosition(WEALD_WALLSIZEX + WEALD_PATHSIZEX + 22.f + fNum - 0.5f, 1.f, 230.f - 0.01f - 0.01f * i);
					dynamic_pointer_cast<CItem>(m_pRoom3->GetGameObjectVector()[i])->StartParticle();
				}
			}
			dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SetEvent2Trigger(false);
		}
	}

	// 4번방
	if (dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->GetCurrentRoom() == 4)
	{
		if (dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->GetEventTrigger())
		{
			for (int i = 0; i < size(m_pRoom4->GetGameObjectVector()); i++)
			{
				float fNum = 1.f / (float)(rand() % 10 + 1) * 5.f;
				if (dynamic_pointer_cast<CItem>(m_pRoom4->GetGameObjectVector()[i]))
				{
					shared_ptr<CTransform> pTransform = dynamic_pointer_cast<CTransform>(m_pRoom4->GetGameObjectVector()[i]->GetComponent(L"Com_Transform", ID_DYNAMIC));
					pTransform->SetPosition(WEALD_WALLSIZEX * 2.f + WEALD_WALLSIZEX / 2.f + WEALD_WALLSIZEX * 1.f + fNum - 0.5f, 1.f, WEALD_WALLSIZEX * 2.f + WEALD_WALLSIZEX * 17.f - 0.01f - 0.01f * i);
					dynamic_pointer_cast<CItem>(m_pRoom4->GetGameObjectVector()[i])->StartParticle();
				}
			}
			dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SetEventTrigger(false);
		}
	}

	// 전투 방 순간이동
	if (GetAsyncKeyState('8') & 0x8000) {
		shared_ptr<CTransform> pTransform = dynamic_pointer_cast<CTransform>((CGameMgr::GetInstance()->GetPlayer())->GetComponent(L"Com_Transform", ID_DYNAMIC));
		pTransform->SetPosition(WEALD_WALLSIZEX + WEALD_PATHSIZEX + 21.3f, 0.f, 195.f);
		dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SetCurrentRoom(3);
		m_pWealdDungeon->DisableAllRoom();
		m_pWealdDungeon->AbleRoom(3);
	}
	// 전투 강제종료
	if (GetAsyncKeyState('9') & 0x8000) {
		m_pRoom3->GetBattleSystem()->EndBattle();
		CCameraMgr::GetInstance()->SetFPSMode();
		CSoundMgr::GetInstance()->StopAll();
		CSoundMgr::GetInstance()->PlayBGM(L"Amb_Weald_Base.wav", 1.f);
		CUIMgr::GetInstance()->SelectUIVisibleOn(L"UI_Inventory");
		CUIMgr::GetInstance()->SelectUIVisibleOff(L"Battle_Hero_UI");
		CUIMgr::GetInstance()->SelectUIVisibleOn(L"UI_DungeonStatus");
		dynamic_pointer_cast<CInteractionInfo>(CUIMgr::GetInstance()->FindUI(L"UI_InteractionInfo"))->SetIsBattle(false);
		CLightMgr::GetInstance()->DungeonBattleLightOff();


		m_pRoom3->SetBattleTrigger(false);
		dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SetInBattle(false);
	}
	
	int iExit;
	iExit = __super::UpdateScene(fTimeDelta);

	// 마을로 씬 변경
	if (GetAsyncKeyState('6') & 0x8000) {

		//로딩 스레드 적용
		CSoundMgr::GetInstance()->StopAll();
		CSoundMgr::GetInstance()->StopSound(CHANNELID::BGM);
		dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->DungeonClear();

		shared_ptr<CScene> pLoadingScreen = make_shared<CLoadingScreen>(m_pGraphicDev, ELoadingSceneType::VILLAGE);
		CSceneMgr::GetInstance()->SetLoadingState(false);
		CSceneMgr::GetInstance()->ChangeScene(pLoadingScreen);
		pLoadingScreen->ReadyScene();
	}

	// 끝에 도달하면 마을로 씬전환
	if (dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->GetCurrentRoom() == 5)
	{
		shared_ptr<CTransform> pTransform = dynamic_pointer_cast<CTransform>((CGameMgr::GetInstance()->GetPlayer())->GetComponent(L"Com_Transform", ID_DYNAMIC));

		CSoundMgr::GetInstance()->StopAll();
		CSoundMgr::GetInstance()->StopSound(CHANNELID::BGM);

		dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->DungeonClear();

		shared_ptr<CScene> pLoadingScreen = make_shared<CLoadingScreen>(m_pGraphicDev, ELoadingSceneType::VILLAGE);
		CSceneMgr::GetInstance()->SetLoadingState(false);
		CSceneMgr::GetInstance()->ChangeScene(pLoadingScreen);
		pLoadingScreen->ReadyScene();
	}

	return iExit;
}

void CWeald_Dungeon::LateUpdateScene()
{
	__super::LateUpdateScene();
	
}

void CWeald_Dungeon::RenderScene()
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

void CWeald_Dungeon::SetLight()
{

	D3DLIGHT9 _pLightInfo1;
	
	_pLightInfo1.Diffuse = { 0.4f , 0.4f , 0.4f , 1.f };
	_pLightInfo1.Specular = { 0.4f , 0.4f , 0.4f , 1.f };
	_pLightInfo1.Ambient = { 0.4f , 0.4f , 0.4f , 1.f };
	_pLightInfo1.Direction = { 1.f, -1.f, 1.f };

	CLightMgr::GetInstance()->InitDirectionLight(m_pGraphicDev, _pLightInfo1);

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_AMBIENT, 0x00202020);

	dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SettingLight();

}

HRESULT CWeald_Dungeon::Ready_Layer_Environment(tstring pLayerTag)
{
	shared_ptr<CLayer> m_pLayer = make_shared<CLayer>();
	m_mapLayer.insert({ pLayerTag, m_pLayer });

	shared_ptr<CGameObject> m_pTerrain = make_shared<CTerrain>(m_pGraphicDev, L"Com_Weald_FloorTexture", ETerrainType::DUNGEON);
	m_pLayer->CreateGameObject(L"OBJ_Terrain", m_pTerrain);

	shared_ptr<CGameObject> m_pDoor;
	shared_ptr<CGameObject> m_pWall;
	shared_ptr<CGameObject> m_pBackWall;

	// 문
	m_pDoor = make_shared<CWealdDungeonDoor>(m_pGraphicDev, EWealdDungeonDoorType::ROOM1);
	m_pDoor->SetPos(_vec3(WEALD_WALLSIZEX * 2.f + WEALD_WALLSIZEX / 2.f + WEALD_WALLSIZEX * 1.f, WEALD_WALLSIZEUPY + 2.f, WEALD_WALLSIZEX * 2.f));
	m_pDoor->SetAngle(_vec3(0.f, 0.f, 0.f));
	m_pDoor->SetScale(_vec3(WEALD_PATHSIZEX / 2.f + 1.5f, WEALD_PATHSIZEX / 2.f + 1.5f, 1.f));
	m_pLayer->CreateGameObject(L"OBJ_Door", m_pDoor);

	m_pDoor = make_shared<CWealdDungeonDoor>(m_pGraphicDev, EWealdDungeonDoorType::ROOM2);
	m_pDoor->SetPos(_vec3(WEALD_WALLSIZEX * 2.f + WEALD_WALLSIZEX / 2.f + WEALD_WALLSIZEX * 1.f, WEALD_WALLSIZEUPY + 2.f, WEALD_WALLSIZEX * 12.f));
	m_pDoor->SetAngle(_vec3(0.f, 0.f, 0.f));
	m_pDoor->SetScale(_vec3(WEALD_PATHSIZEX / 2.f + 1.5f, WEALD_PATHSIZEX / 2.f + 1.5f, 1.f));
	m_pLayer->CreateGameObject(L"OBJ_Door", m_pDoor);

	m_pDoor = make_shared<CWealdDungeonDoor>(m_pGraphicDev, EWealdDungeonDoorType::ROOM3);
	m_pDoor->SetPos(_vec3(WEALD_WALLSIZEX * 2.f + WEALD_WALLSIZEX / 2.f + WEALD_WALLSIZEX * 1.f, WEALD_WALLSIZEUPY + 2.f, WEALD_WALLSIZEX * 16.f));
	m_pDoor->SetAngle(_vec3(0.f, 0.f, 0.f));
	m_pDoor->SetScale(_vec3(WEALD_PATHSIZEX / 2.f + 1.5f, WEALD_PATHSIZEX / 2.f + 1.5f, 1.f));
	m_pLayer->CreateGameObject(L"OBJ_Door", m_pDoor);
	
	m_pDoor = make_shared<CWealdDungeonDoor>(m_pGraphicDev, EWealdDungeonDoorType::ROOM4);
	m_pDoor->SetPos(_vec3(WEALD_WALLSIZEX * 2.f + WEALD_WALLSIZEX / 2.f + WEALD_WALLSIZEX * 1.f, WEALD_WALLSIZEUPY + 2.f, WEALD_WALLSIZEX * 26.f));
	m_pDoor->SetAngle(_vec3(0.f, 0.f, 0.f));
	m_pDoor->SetScale(_vec3(WEALD_PATHSIZEX / 2.f + 1.5f, WEALD_PATHSIZEX / 2.f + 1.5f, 1.f));
	m_pLayer->CreateGameObject(L"OBJ_Door", m_pDoor);

	//// 배경(데코)
	//m_pWall = make_shared<CWall>(m_pGraphicDev, L"Weald_Image_Wagon", 1, true);
	//m_pWall->SetPos(_vec3(WEALD_WALLSIZEX * 4.5f, WEALD_WALLSIZEUPY + 8.f, 0.f));
	//m_pWall->SetAngle(_vec3(0.f, PI / 2.f, 0.f));
	//m_pWall->SetScale(_vec3(WEALD_WALLSIZEX * 2.f + WEALD_PATHSIZEX, (WEALD_WALLSIZEX * 2.f + WEALD_PATHSIZEX) / 192.f * 72.f, 1.f));
	//m_pLayer->CreateGameObject(L"OBJ_Wall", m_pWall);
	//
	//m_pWall = make_shared<CWall>(m_pGraphicDev, L"Weald_Image_Village", 1, true);
	//m_pWall->SetPos(_vec3(WEALD_WALLSIZEX * 0.5f + WEALD_PATHSIZEX + WEALD_WALLSIZEX, WEALD_WALLSIZEUPY + 30.f, WEALD_WALLSIZEX * 30.f + 60.f));
	//m_pWall->SetAngle(_vec3(0.f, PI / 2.f, 0.f));
	//m_pWall->SetScale(_vec3((WEALD_WALLSIZEX * 2.f + WEALD_PATHSIZEX)*6.f, ((WEALD_WALLSIZEX * 2.f + WEALD_PATHSIZEX) / 192.f * 72.f)*6.f, 1.f));
	//m_pLayer->CreateGameObject(L"OBJ_Wall", m_pWall);

	// 진입방 벽
	for (int i = 0; i < 2; i++)
	{
		m_pWall = make_shared<CWall>(m_pGraphicDev, L"Com_Weald_WallTexture", 9, true);	
		m_pWall->SetPos(_vec3(WEALD_WALLSIZEX * 2.f, WEALD_WALLSIZEUPY, WEALD_WALLSIZEX / 2.f + WEALD_WALLSIZEX * i));
		m_pWall->SetAngle(_vec3(0.f, -1.f * PI, 0.f));
		m_pWall->SetScale(_vec3(WEALD_WALLSIZEX / 2.f, WEALD_WALLSIZEX / 2.f, 1.f));
		m_pLayer->CreateGameObject(L"OBJ_Wall", m_pWall);

		m_pBackWall = make_shared<CWall>(m_pGraphicDev, L"Com_Weald_BackWallTexture", 1, false);
		m_pBackWall->SetPos(_vec3(WEALD_WALLSIZEX * 2.f - 0.1f, WEALD_WALLSIZEUPY + 2.f, WEALD_WALLSIZEX / 2.f + WEALD_WALLSIZEX * i));
		m_pBackWall->SetAngle(_vec3(0.f, -1.f * PI, 0.f));
		m_pBackWall->SetScale(_vec3(WEALD_WALLSIZEX / 2.f, WEALD_WALLSIZEX * 7.f / 10.f, 1));
		m_pLayer->CreateGameObject(L"OBJ_Back", m_pBackWall);
	}
	
	for (int i = 0; i < 2; i++)
	{
		m_pWall = make_shared<CWall>(m_pGraphicDev, L"Com_Weald_WallTexture", 9, true);
		m_pWall->SetPos(_vec3(WEALD_WALLSIZEX * 4.f + WEALD_PATHSIZEX + WEALD_WALLSIZEX * 2.f, WEALD_WALLSIZEUPY, WEALD_WALLSIZEX / 2.f + WEALD_WALLSIZEX * i));
		m_pWall->SetAngle(_vec3(0.f, 0.f, 0.f));
		m_pWall->SetScale(_vec3(WEALD_WALLSIZEX / 2.f, WEALD_WALLSIZEX / 2.f, 1));
		m_pLayer->CreateGameObject(L"OBJ_Wall", m_pWall);

		m_pBackWall = make_shared<CWall>(m_pGraphicDev, L"Com_Weald_BackWallTexture", 1, false);
		m_pBackWall->SetPos(_vec3(WEALD_WALLSIZEX * 4.f + WEALD_PATHSIZEX + WEALD_WALLSIZEX * 2.f + 0.1f, WEALD_WALLSIZEUPY + 2.f, WEALD_WALLSIZEX / 2.f + WEALD_WALLSIZEX * i));
		m_pBackWall->SetAngle(_vec3(0.f, 0.f, 0.f));
		m_pBackWall->SetScale(_vec3(WEALD_WALLSIZEX / 2.f, WEALD_WALLSIZEX * 7.f / 10.f, 1));
		m_pLayer->CreateGameObject(L"OBJ_Back", m_pBackWall);
	}

	for (int i = 0; i < 1; i++)
	{
		m_pWall = make_shared<CWall>(m_pGraphicDev, L"Com_Weald_WallTexture", 9, true);
		m_pWall->SetPos(_vec3(WEALD_WALLSIZEX * 2.f + WEALD_WALLSIZEX / 2.f + WEALD_WALLSIZEX * i, WEALD_WALLSIZEUPY, WEALD_WALLSIZEX * 2.f));
		m_pWall->SetAngle(_vec3(0.f, PI / -2.f, 0.f));
		m_pWall->SetScale(_vec3(WEALD_WALLSIZEX / 2.f, WEALD_WALLSIZEX / 2.f, 1.f));
		m_pLayer->CreateGameObject(L"OBJ_Wall", m_pWall);

		m_pBackWall = make_shared<CWall>(m_pGraphicDev, L"Com_Weald_BackWallTexture", 1, false);
		m_pBackWall->SetPos(_vec3(WEALD_WALLSIZEX * 2.f + WEALD_WALLSIZEX / 2.f + WEALD_WALLSIZEX * i, WEALD_WALLSIZEUPY + 2.f, WEALD_WALLSIZEX * 2.f + 0.1f));
		m_pBackWall->SetAngle(_vec3(0.f, PI / -2.f, 0.f));
		m_pBackWall->SetScale(_vec3(WEALD_WALLSIZEX / 2.f, WEALD_WALLSIZEX * 7.f / 10.f, 1));
		m_pLayer->CreateGameObject(L"OBJ_Back", m_pBackWall);
	}

	for (int i = 0; i < 3; i++)
	{
		m_pWall = make_shared<CWall>(m_pGraphicDev, L"Com_Weald_WallTexture", 9, true);
		m_pWall->SetPos(_vec3(WEALD_WALLSIZEX * 2.f + WEALD_PATHSIZEX + WEALD_WALLSIZEX * 1.5f + WEALD_WALLSIZEX * i, WEALD_WALLSIZEUPY, WEALD_WALLSIZEX * 2.f + 0.0001f));
		m_pWall->SetAngle(_vec3(0.f, PI / -2.f, 0.f));
		m_pWall->SetScale(_vec3(WEALD_WALLSIZEX / 2.f, WEALD_WALLSIZEX / 2.f, 1.f));
		m_pLayer->CreateGameObject(L"OBJ_Wall", m_pWall);

		m_pBackWall = make_shared<CWall>(m_pGraphicDev, L"Com_Weald_BackWallTexture", 1, false);
		m_pBackWall->SetPos(_vec3(WEALD_WALLSIZEX * 2.f + WEALD_PATHSIZEX + WEALD_WALLSIZEX * 1.5f + WEALD_WALLSIZEX * i, WEALD_WALLSIZEUPY + 2.f, WEALD_WALLSIZEX * 2.f + 0.1f));
		m_pBackWall->SetAngle(_vec3(0.f, PI / -2.f, 0.f));
		m_pBackWall->SetScale(_vec3(WEALD_WALLSIZEX / 2.f, WEALD_WALLSIZEX * 7.f / 10.f, 1));
		m_pLayer->CreateGameObject(L"OBJ_Back", m_pBackWall);
	}

	// 통로 1 벽
	for (int i = 0; i < 10; i++)
	{
		m_pWall = make_shared<CWall>(m_pGraphicDev, L"Com_Weald_WallTexture", 9, true);
		m_pWall->SetPos(_vec3(WEALD_WALLSIZEX * 2.f + WEALD_WALLSIZEX * 1.f, WEALD_WALLSIZEUPY, WEALD_WALLSIZEX * 2.f + WEALD_WALLSIZEX / 2.f + WEALD_WALLSIZEX * i));
		m_pWall->SetAngle(_vec3(0.f, -1.f * PI, 0.f));
		m_pWall->SetScale(_vec3(WEALD_WALLSIZEX / 2.f, WEALD_WALLSIZEX / 2.f, 1.f));
		m_pLayer->CreateGameObject(L"OBJ_Wall", m_pWall);

		m_pBackWall = make_shared<CWall>(m_pGraphicDev, L"Com_Weald_BackWallTexture", 1, false);
		m_pBackWall->SetPos(_vec3(WEALD_WALLSIZEX * 2.f + WEALD_WALLSIZEX * 1.f - 0.1f, WEALD_WALLSIZEUPY + 2.f, WEALD_WALLSIZEX * 2.f + WEALD_WALLSIZEX / 2.f + WEALD_WALLSIZEX * i));
		m_pBackWall->SetAngle(_vec3(0.f, -1.f * PI, 0.f));
		m_pBackWall->SetScale(_vec3(WEALD_WALLSIZEX / 2.f, WEALD_WALLSIZEX * 7.f / 10.f, 1));
		m_pLayer->CreateGameObject(L"OBJ_Back", m_pBackWall);
	}

	for (int i = 0; i < 10; i++)
	{
		m_pWall = make_shared<CWall>(m_pGraphicDev, L"Com_Weald_WallTexture", 9, true);
		m_pWall->SetPos(_vec3(WEALD_WALLSIZEX * 2.f + WEALD_PATHSIZEX + WEALD_WALLSIZEX * 1.f, WEALD_WALLSIZEUPY, WEALD_WALLSIZEX * 2.f + WEALD_WALLSIZEX / 2.f + WEALD_WALLSIZEX * i));
		m_pWall->SetAngle(_vec3(0.f, 0.f, 0.f));
		m_pWall->SetScale(_vec3(WEALD_WALLSIZEX / 2.f, WEALD_WALLSIZEX / 2.f, 1.f));
		m_pLayer->CreateGameObject(L"OBJ_Wall", m_pWall);

		m_pBackWall = make_shared<CWall>(m_pGraphicDev, L"Com_Weald_BackWallTexture", 1, false);
		m_pBackWall->SetPos(_vec3(WEALD_WALLSIZEX * 2.f + WEALD_PATHSIZEX + WEALD_WALLSIZEX * 1.f + 0.1f, WEALD_WALLSIZEUPY + 2.f, WEALD_WALLSIZEX * 2.f + WEALD_WALLSIZEX / 2.f + WEALD_WALLSIZEX * i));
		m_pBackWall->SetAngle(_vec3(0.f, 0.f, 0.f));
		m_pBackWall->SetScale(_vec3(WEALD_WALLSIZEX / 2.f, WEALD_WALLSIZEX * 7.f / 10.f, 1));
		m_pLayer->CreateGameObject(L"OBJ_Back", m_pBackWall);
	}

	// 방 2 벽
	for (int i = 0; i < 4; i++)
	{
		m_pWall = make_shared<CWall>(m_pGraphicDev, L"Com_Weald_WallTexture", 9, true);
		m_pWall->SetPos(_vec3(WEALD_WALLSIZEX * 1.f, WEALD_WALLSIZEUPY, WEALD_WALLSIZEX / 2.f + WEALD_WALLSIZEX * 12.f + WEALD_WALLSIZEX * i));
		m_pWall->SetAngle(_vec3(0.f, 0.f, 0.f));
		m_pWall->SetScale(_vec3(WEALD_WALLSIZEX / 2.f, WEALD_WALLSIZEX / 2.f, 1.f));
		m_pLayer->CreateGameObject(L"OBJ_Wall", m_pWall);

		m_pBackWall = make_shared<CWall>(m_pGraphicDev, L"Com_Weald_BackWallTexture", 1, false);
		m_pBackWall->SetPos(_vec3(WEALD_WALLSIZEX * 1.f - 0.1f, WEALD_WALLSIZEUPY + 2.f, WEALD_WALLSIZEX / 2.f + WEALD_WALLSIZEX * 12.f + WEALD_WALLSIZEX * i));
		m_pBackWall->SetAngle(_vec3(0.f, 0.f, 0.f));
		m_pBackWall->SetScale(_vec3(WEALD_WALLSIZEX / 2.f, WEALD_WALLSIZEX * 7.f / 10.f, 1));
		m_pLayer->CreateGameObject(L"OBJ_Back", m_pBackWall);
	}

	for (int i = 0; i < 4; i++)
	{
		m_pWall = make_shared<CWall>(m_pGraphicDev, L"Com_Weald_WallTexture", 9, true);
		m_pWall->SetPos(_vec3(WEALD_WALLSIZEX * 3.f + WEALD_PATHSIZEX + WEALD_WALLSIZEX * 2.f, WEALD_WALLSIZEUPY, WEALD_WALLSIZEX / 2.f + WEALD_WALLSIZEX * 12.f + WEALD_WALLSIZEX * i));
		m_pWall->SetAngle(_vec3(0.f, 0.f, 0.f));
		m_pWall->SetScale(_vec3(WEALD_WALLSIZEX / 2.f, WEALD_WALLSIZEX / 2.f, 1.f));
		m_pLayer->CreateGameObject(L"OBJ_Wall", m_pWall);

		m_pBackWall = make_shared<CWall>(m_pGraphicDev, L"Com_Weald_BackWallTexture", 1, false);
		m_pBackWall->SetPos(_vec3(WEALD_WALLSIZEX * 3.f + WEALD_PATHSIZEX + WEALD_WALLSIZEX * 2.f + 0.1f, WEALD_WALLSIZEUPY + 2.f, WEALD_WALLSIZEX / 2.f + WEALD_WALLSIZEX * 12.f + WEALD_WALLSIZEX * i));
		m_pBackWall->SetAngle(_vec3(0.f, 0.f, 0.f));
		m_pBackWall->SetScale(_vec3(WEALD_WALLSIZEX / 2.f, WEALD_WALLSIZEX * 7.f / 10.f, 1));
		m_pLayer->CreateGameObject(L"OBJ_Back", m_pBackWall);
	}

	for (int i = 0; i < 2; i++)
	{
		m_pWall = make_shared<CWall>(m_pGraphicDev, L"Com_Weald_WallTexture", 9, true);
		m_pWall->SetPos(_vec3(WEALD_WALLSIZEX * 1.5f + WEALD_WALLSIZEX * i, WEALD_WALLSIZEUPY, WEALD_WALLSIZEX * 12.f));
		m_pWall->SetAngle(_vec3(0.f, PI / 2.f, 0.f));
		m_pWall->SetScale(_vec3(WEALD_WALLSIZEX / 2.f, WEALD_WALLSIZEX / 2.f, 1.f));
		m_pLayer->CreateGameObject(L"OBJ_Wall", m_pWall);

		m_pBackWall = make_shared<CWall>(m_pGraphicDev, L"Com_Weald_BackWallTexture", 1, false);
		m_pBackWall->SetPos(_vec3(WEALD_WALLSIZEX * 1.5f + WEALD_WALLSIZEX * i, WEALD_WALLSIZEUPY + 2.f, WEALD_WALLSIZEX * 12.f - 0.1f));
		m_pBackWall->SetAngle(_vec3(0.f, PI / 2.f, 0.f));
		m_pBackWall->SetScale(_vec3(WEALD_WALLSIZEX / 2.f, WEALD_WALLSIZEX * 7.f / 10.f, 1));
		m_pLayer->CreateGameObject(L"OBJ_Back", m_pBackWall);
	}

	for (int i = 0; i < 2; i++)
	{
		m_pWall = make_shared<CWall>(m_pGraphicDev, L"Com_Weald_WallTexture", 9, true);
		m_pWall->SetPos(_vec3(WEALD_WALLSIZEX * 3.5f + WEALD_PATHSIZEX + WEALD_WALLSIZEX * i, WEALD_WALLSIZEUPY, WEALD_WALLSIZEX * 12.f));
		m_pWall->SetAngle(_vec3(0.f, PI / 2.f, 0.f));
		m_pWall->SetScale(_vec3(WEALD_WALLSIZEX / 2.f, WEALD_WALLSIZEX / 2.f, 1.f));
		m_pLayer->CreateGameObject(L"OBJ_Wall", m_pWall);

		m_pBackWall = make_shared<CWall>(m_pGraphicDev, L"Com_Weald_BackWallTexture", 1, false);
		m_pBackWall->SetPos(_vec3(WEALD_WALLSIZEX * 3.5f + WEALD_PATHSIZEX + WEALD_WALLSIZEX * i, WEALD_WALLSIZEUPY + 2.f, WEALD_WALLSIZEX * 12.f - 0.1f));
		m_pBackWall->SetAngle(_vec3(0.f, PI / 2.f, 0.f));
		m_pBackWall->SetScale(_vec3(WEALD_WALLSIZEX / 2.f, WEALD_WALLSIZEX * 7.f / 10.f, 1));
		m_pLayer->CreateGameObject(L"OBJ_Back", m_pBackWall);
	}

	for (int i = 0; i < 2; i++)
	{
		m_pWall = make_shared<CWall>(m_pGraphicDev, L"Com_Weald_WallTexture", 9, true);
		m_pWall->SetPos(_vec3(WEALD_WALLSIZEX * 1.5f + WEALD_WALLSIZEX * i, WEALD_WALLSIZEUPY, WEALD_WALLSIZEX * 16.f));
		m_pWall->SetAngle(_vec3(0.f, PI / 2.f, 0.f));
		m_pWall->SetScale(_vec3(WEALD_WALLSIZEX / 2.f, WEALD_WALLSIZEX / 2.f, 1.f));
		m_pLayer->CreateGameObject(L"OBJ_Wall", m_pWall);

		m_pBackWall = make_shared<CWall>(m_pGraphicDev, L"Com_Weald_BackWallTexture", 1, false);
		m_pBackWall->SetPos(_vec3(WEALD_WALLSIZEX * 1.5f + WEALD_WALLSIZEX * i, WEALD_WALLSIZEUPY + 2.f, WEALD_WALLSIZEX * 16.f + 0.1f));
		m_pBackWall->SetAngle(_vec3(0.f, PI / 2.f, 0.f));
		m_pBackWall->SetScale(_vec3(WEALD_WALLSIZEX / 2.f, WEALD_WALLSIZEX * 7.f / 10.f, 1));
		m_pLayer->CreateGameObject(L"OBJ_Back", m_pBackWall);
	}

	for (int i = 0; i < 2; i++)
	{
		m_pWall = make_shared<CWall>(m_pGraphicDev, L"Com_Weald_WallTexture", 9, true);
		m_pWall->SetPos(_vec3(WEALD_WALLSIZEX * 3.5f + WEALD_PATHSIZEX + WEALD_WALLSIZEX * i, WEALD_WALLSIZEUPY, WEALD_WALLSIZEX * 16.f));
		m_pWall->SetAngle(_vec3(0.f, PI / 2.f, 0.f));
		m_pWall->SetScale(_vec3(WEALD_WALLSIZEX / 2.f, WEALD_WALLSIZEX / 2.f, 1.f));
		m_pLayer->CreateGameObject(L"OBJ_Wall", m_pWall);

		m_pBackWall = make_shared<CWall>(m_pGraphicDev, L"Com_Weald_BackWallTexture", 1, false);
		m_pBackWall->SetPos(_vec3(WEALD_WALLSIZEX * 3.5f + WEALD_PATHSIZEX + WEALD_WALLSIZEX * i, WEALD_WALLSIZEUPY + 2.f, WEALD_WALLSIZEX * 16.f + 0.1f));
		m_pBackWall->SetAngle(_vec3(0.f, PI / 2.f, 0.f));
		m_pBackWall->SetScale(_vec3(WEALD_WALLSIZEX / 2.f, WEALD_WALLSIZEX * 7.f / 10.f, 1));
		m_pLayer->CreateGameObject(L"OBJ_Back", m_pBackWall);
	}

	// 통로 2 벽
	for (int i = 0; i < 10; i++)
	{
		m_pWall = make_shared<CWall>(m_pGraphicDev, L"Com_Weald_WallTexture", 9, true);
		m_pWall->SetPos(_vec3(WEALD_WALLSIZEX * 2.f + WEALD_WALLSIZEX * 1.f, WEALD_WALLSIZEUPY, WEALD_WALLSIZEX * 16.f + WEALD_WALLSIZEX / 2.f + WEALD_WALLSIZEX * i));
		m_pWall->SetAngle(_vec3(0.f, 0.f, 0.f));
		m_pWall->SetScale(_vec3(WEALD_WALLSIZEX / 2.f, WEALD_WALLSIZEX / 2.f, 1.f));
		m_pLayer->CreateGameObject(L"OBJ_Wall", m_pWall);

		m_pBackWall = make_shared<CWall>(m_pGraphicDev, L"Com_Weald_BackWallTexture", 1, false);
		m_pBackWall->SetPos(_vec3(WEALD_WALLSIZEX * 2.f + WEALD_WALLSIZEX * 1.f - 0.1f, WEALD_WALLSIZEUPY + 2.f, WEALD_WALLSIZEX * 16.f + WEALD_WALLSIZEX / 2.f + WEALD_WALLSIZEX * i));
		m_pBackWall->SetAngle(_vec3(0.f, 0.f, 0.f));
		m_pBackWall->SetScale(_vec3(WEALD_WALLSIZEX / 2.f, WEALD_WALLSIZEX * 7.f / 10.f, 1));
		m_pLayer->CreateGameObject(L"OBJ_Back", m_pBackWall);
	}

	for (int i = 0; i < 10; i++)
	{
		m_pWall = make_shared<CWall>(m_pGraphicDev, L"Com_Weald_WallTexture", 9, true);
		m_pWall->SetPos(_vec3(WEALD_WALLSIZEX * 2.f + WEALD_PATHSIZEX + WEALD_WALLSIZEX * 1.f, WEALD_WALLSIZEUPY, WEALD_WALLSIZEX * 16.f + WEALD_WALLSIZEX / 2.f + WEALD_WALLSIZEX * i));
		m_pWall->SetAngle(_vec3(0.f, 0.f, 0.f));
		m_pWall->SetScale(_vec3(WEALD_WALLSIZEX / 2.f, WEALD_WALLSIZEX / 2.f, 1.f));
		m_pLayer->CreateGameObject(L"OBJ_Wall", m_pWall);

		m_pBackWall = make_shared<CWall>(m_pGraphicDev, L"Com_Weald_BackWallTexture", 1, false);
		m_pBackWall->SetPos(_vec3(WEALD_WALLSIZEX * 2.f + WEALD_PATHSIZEX + WEALD_WALLSIZEX * 1.f + 0.1f, WEALD_WALLSIZEUPY + 2.f, WEALD_WALLSIZEX * 16.f + WEALD_WALLSIZEX / 2.f + WEALD_WALLSIZEX * i));
		m_pBackWall->SetAngle(_vec3(0.f, 0.f, 0.f));
		m_pBackWall->SetScale(_vec3(WEALD_WALLSIZEX / 2.f, WEALD_WALLSIZEX * 7.f / 10.f, 1));
		m_pLayer->CreateGameObject(L"OBJ_Back", m_pBackWall);
	}

	//for (int i = 6; i < 10; i++)
	//{
	//	m_pWall = make_shared<CWall>(m_pGraphicDev, L"Com_Weald_WallTexture", 9, true);
	//	m_pWall->SetPos(_vec3(WEALD_WALLSIZEX * 2.f + WEALD_PATHSIZEX + WEALD_WALLSIZEX * 1.f, WEALD_WALLSIZEUPY, WEALD_WALLSIZEX * 16.f + WEALD_WALLSIZEX / 2.f + WEALD_WALLSIZEX * i));
	//	m_pWall->SetAngle(_vec3(0.f, 0.f, 0.f));
	//	m_pWall->SetScale(_vec3(WEALD_WALLSIZEX / 2.f, WEALD_WALLSIZEX / 2.f, 1.f));
	//	m_pLayer->CreateGameObject(L"OBJ_Wall", m_pWall);
	//
	//	m_pBackWall = make_shared<CWall>(m_pGraphicDev, L"Com_Weald_BackWallTexture", 1, false);
	//	m_pBackWall->SetPos(_vec3(WEALD_WALLSIZEX * 2.f + WEALD_PATHSIZEX + WEALD_WALLSIZEX * 1.f + 0.1f, WEALD_WALLSIZEUPY + 2.f, WEALD_WALLSIZEX * 16.f + WEALD_WALLSIZEX / 2.f + WEALD_WALLSIZEX * i));
	//	m_pBackWall->SetAngle(_vec3(0.f, 0.f, 0.f));
	//	m_pBackWall->SetScale(_vec3(WEALD_WALLSIZEX / 2.f, WEALD_WALLSIZEX * 7.f / 10.f, 1));
	//	m_pLayer->CreateGameObject(L"OBJ_Back", m_pBackWall);
	//}

	// 방 3 벽
	/*for (int i = 0; i < 4; i++)
	{
		m_pWall = make_shared<CWall>(m_pGraphicDev, L"Com_Weald_WallTexture", 9, true);
		m_pWall->SetPos(_vec3(WEALD_WALLSIZEX * 1.f, WEALD_WALLSIZEUPY, WEALD_WALLSIZEX / 2.f + WEALD_WALLSIZEX * 26.f + WEALD_WALLSIZEX * i));
		m_pWall->SetAngle(_vec3(0.f, 0.f, 0.f));
		m_pWall->SetScale(_vec3(WEALD_WALLSIZEX / 2.f, WEALD_WALLSIZEX / 2.f, 1.f));
		m_pLayer->CreateGameObject(L"OBJ_Wall", m_pWall);

		m_pBackWall = make_shared<CWall>(m_pGraphicDev, L"Com_Weald_BackWallTexture", 1, false);
		m_pBackWall->SetPos(_vec3(WEALD_WALLSIZEX * 1.f - 0.1f, WEALD_WALLSIZEUPY + 2.f, WEALD_WALLSIZEX / 2.f + WEALD_WALLSIZEX * 26.f + WEALD_WALLSIZEX * i));
		m_pBackWall->SetAngle(_vec3(0.f, 0.f, 0.f));
		m_pBackWall->SetScale(_vec3(WEALD_WALLSIZEX / 2.f, WEALD_WALLSIZEX * 7.f / 10.f, 1));
		m_pLayer->CreateGameObject(L"OBJ_Back", m_pBackWall);
	}

	for (int i = 0; i < 4; i++)
	{
		m_pWall = make_shared<CWall>(m_pGraphicDev, L"Com_Weald_WallTexture", 9, true);
		m_pWall->SetPos(_vec3(WEALD_WALLSIZEX * 3.f + WEALD_PATHSIZEX + WEALD_WALLSIZEX * 2.f, WEALD_WALLSIZEUPY, WEALD_WALLSIZEX / 2.f + WEALD_WALLSIZEX * 26.f + WEALD_WALLSIZEX * i));
		m_pWall->SetAngle(_vec3(0.f, 0.f, 0.f));
		m_pWall->SetScale(_vec3(WEALD_WALLSIZEX / 2.f, WEALD_WALLSIZEX / 2.f, 1.f));
		m_pLayer->CreateGameObject(L"OBJ_Wall", m_pWall);

		m_pBackWall = make_shared<CWall>(m_pGraphicDev, L"Com_Weald_BackWallTexture", 1, false);
		m_pBackWall->SetPos(_vec3(WEALD_WALLSIZEX * 3.f + WEALD_PATHSIZEX + WEALD_WALLSIZEX * 2.f + 0.1f, WEALD_WALLSIZEUPY + 2.f, WEALD_WALLSIZEX / 2.f + WEALD_WALLSIZEX * 26.f + WEALD_WALLSIZEX * i));
		m_pBackWall->SetAngle(_vec3(0.f, 0.f, 0.f));
		m_pBackWall->SetScale(_vec3(WEALD_WALLSIZEX / 2.f, WEALD_WALLSIZEX * 7.f / 10.f, 1));
		m_pLayer->CreateGameObject(L"OBJ_Back", m_pBackWall);
	}

	for (int i = 0; i < 2; i++)
	{
		m_pWall = make_shared<CWall>(m_pGraphicDev, L"Com_Weald_WallTexture", 9, true);
		m_pWall->SetPos(_vec3(WEALD_WALLSIZEX * 1.5f + WEALD_WALLSIZEX * i, WEALD_WALLSIZEUPY, WEALD_WALLSIZEX * 26.f));
		m_pWall->SetAngle(_vec3(0.f, PI / 2.f, 0.f));
		m_pWall->SetScale(_vec3(WEALD_WALLSIZEX / 2.f, WEALD_WALLSIZEX / 2.f, 1.f));
		m_pLayer->CreateGameObject(L"OBJ_Wall", m_pWall);

		m_pBackWall = make_shared<CWall>(m_pGraphicDev, L"Com_Weald_BackWallTexture", 1, false);
		m_pBackWall->SetPos(_vec3(WEALD_WALLSIZEX * 1.5f + WEALD_WALLSIZEX * i, WEALD_WALLSIZEUPY + 2.f, WEALD_WALLSIZEX * 26.f - 0.1f));
		m_pBackWall->SetAngle(_vec3(0.f, PI / 2.f, 0.f));
		m_pBackWall->SetScale(_vec3(WEALD_WALLSIZEX / 2.f, WEALD_WALLSIZEX * 7.f / 10.f, 1));
		m_pLayer->CreateGameObject(L"OBJ_Back", m_pBackWall);
	}

	for (int i = 0; i < 2; i++)
	{
		m_pWall = make_shared<CWall>(m_pGraphicDev, L"Com_Weald_WallTexture", 9, true);
		m_pWall->SetPos(_vec3(WEALD_WALLSIZEX * 3.5f + WEALD_PATHSIZEX + WEALD_WALLSIZEX * i, WEALD_WALLSIZEUPY, WEALD_WALLSIZEX * 26.f));
		m_pWall->SetAngle(_vec3(0.f, PI / 2.f, 0.f));
		m_pWall->SetScale(_vec3(WEALD_WALLSIZEX / 2.f, WEALD_WALLSIZEX / 2.f, 1.f));
		m_pLayer->CreateGameObject(L"OBJ_Wall", m_pWall);

		m_pBackWall = make_shared<CWall>(m_pGraphicDev, L"Com_Weald_BackWallTexture", 1, false);
		m_pBackWall->SetPos(_vec3(WEALD_WALLSIZEX * 3.5f + WEALD_PATHSIZEX + WEALD_WALLSIZEX * i, WEALD_WALLSIZEUPY + 2.f, WEALD_WALLSIZEX * 26.f - 0.1f));
		m_pBackWall->SetAngle(_vec3(0.f, PI / 2.f, 0.f));
		m_pBackWall->SetScale(_vec3(WEALD_WALLSIZEX / 2.f, WEALD_WALLSIZEX * 7.f / 10.f, 1));
		m_pLayer->CreateGameObject(L"OBJ_Back", m_pBackWall);
	}

	for (int i = 0; i < 2; i++)
	{
		m_pWall = make_shared<CWall>(m_pGraphicDev, L"Com_Weald_WallTexture", 9, true);
		m_pWall->SetPos(_vec3(WEALD_WALLSIZEX * 1.5f + WEALD_WALLSIZEX * i, WEALD_WALLSIZEUPY, WEALD_WALLSIZEX * 30.f));
		m_pWall->SetAngle(_vec3(0.f, PI / 2.f, 0.f));
		m_pWall->SetScale(_vec3(WEALD_WALLSIZEX / 2.f, WEALD_WALLSIZEX / 2.f, 1.f));
		m_pLayer->CreateGameObject(L"OBJ_Wall", m_pWall);

		m_pBackWall = make_shared<CWall>(m_pGraphicDev, L"Com_Weald_BackWallTexture", 1, false);
		m_pBackWall->SetPos(_vec3(WEALD_WALLSIZEX * 1.5f + WEALD_WALLSIZEX * i, WEALD_WALLSIZEUPY + 2.f, WEALD_WALLSIZEX * 30.f + 0.1f));
		m_pBackWall->SetAngle(_vec3(0.f, PI / 2.f, 0.f));
		m_pBackWall->SetScale(_vec3(WEALD_WALLSIZEX / 2.f, WEALD_WALLSIZEX * 7.f / 10.f, 1));
		m_pLayer->CreateGameObject(L"OBJ_Back", m_pBackWall);
	}

	for (int i = 0; i < 2; i++)
	{
		m_pWall = make_shared<CWall>(m_pGraphicDev, L"Com_Weald_WallTexture", 9, true);
		m_pWall->SetPos(_vec3(WEALD_WALLSIZEX * 3.5f + WEALD_PATHSIZEX + WEALD_WALLSIZEX * i, WEALD_WALLSIZEUPY, WEALD_WALLSIZEX * 30.f));
		m_pWall->SetAngle(_vec3(0.f, PI / 2.f, 0.f));
		m_pWall->SetScale(_vec3(WEALD_WALLSIZEX / 2.f, WEALD_WALLSIZEX / 2.f, 1.f));
		m_pLayer->CreateGameObject(L"OBJ_Wall", m_pWall);

		m_pBackWall = make_shared<CWall>(m_pGraphicDev, L"Com_Weald_BackWallTexture", 1, false);
		m_pBackWall->SetPos(_vec3(WEALD_WALLSIZEX * 3.5f + WEALD_PATHSIZEX + WEALD_WALLSIZEX * i, WEALD_WALLSIZEUPY + 2.f, WEALD_WALLSIZEX * 30.f + 0.1f));
		m_pBackWall->SetAngle(_vec3(0.f, PI / 2.f, 0.f));
		m_pBackWall->SetScale(_vec3(WEALD_WALLSIZEX / 2.f, WEALD_WALLSIZEX * 7.f / 10.f, 1));
		m_pLayer->CreateGameObject(L"OBJ_Back", m_pBackWall);
	}*/

	//가장 최하위 순서에 돌려줄 것
	dynamic_pointer_cast<CLayer>(m_pLayer)->AwakeLayer();


	return S_OK;
}

HRESULT CWeald_Dungeon::Ready_Layer_SkyBox(tstring pLayerTag)
{
	shared_ptr<CLayer> m_pLayer = make_shared<CLayer>();
	m_mapLayer.insert({ pLayerTag, m_pLayer });


	shared_ptr<CGameObject> m_pSkyBox = make_shared<CSkyBox>(m_pGraphicDev, L"Com_Weald_SkyBoxTexture");
	m_pLayer->CreateGameObject(L"OBJ_Weald_SkyBox", m_pSkyBox);

	dynamic_pointer_cast<CLayer>(m_pLayer)->AwakeLayer();
	
	return S_OK;
}

HRESULT CWeald_Dungeon::Ready_Layer_Camera(tstring pLayerTag)
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

HRESULT CWeald_Dungeon::Ready_Layer_GameObject(tstring pLayerTag)
{
	shared_ptr<CLayer> m_pLayer = make_shared<CLayer>();
	m_mapLayer.insert({ pLayerTag, m_pLayer });

	//Player
	shared_ptr<CGameObject> m_pPlayer = make_shared<CPlayer>(m_pGraphicDev);
	m_pPlayer->SetPos({ 55.f, 0.f, 2.f });
	m_pLayer->CreateGameObject(L"Obj_Player", m_pPlayer);
	CGameMgr::GetInstance()->SetPlayer(m_pPlayer);
	dynamic_pointer_cast<CPlayer>(m_pPlayer)->SetInDungeon(true);

	// GameObjects
	// 함정
	shared_ptr<CGameObject>m_pTrap1 = make_shared<CWeald_Trap>(m_pGraphicDev);
	m_pTrap1->SetPos(_vec3(WEALD_WALLSIZEX * 2.f + WEALD_WALLSIZEX / 2.f + WEALD_WALLSIZEX * 1.f, WEALD_WALLSIZEUPY + 1.f, WEALD_WALLSIZEX * 2.f + WEALD_WALLSIZEX * 3.f));
	m_pTrap1->SetAngle(_vec3(0.f, 0.f, 0.f));
	m_pTrap1->SetScale(_vec3(WEALD_PATHSIZEX / 3.f, WEALD_PATHSIZEX / 3.f, 1.f));

	// 장애물
	shared_ptr<CGameObject>m_pObstacle1 = make_shared<CWeald_Obstacle>(m_pGraphicDev);
	m_pObstacle1->SetPos(_vec3(WEALD_WALLSIZEX * 2.f + WEALD_WALLSIZEX / 2.f + WEALD_WALLSIZEX * 1.f, WEALD_WALLSIZEUPY + 1.f, WEALD_WALLSIZEX * 2.f + WEALD_WALLSIZEX * 6.f));
	m_pObstacle1->SetAngle(_vec3(0.f, 0.f, 0.f));
	m_pObstacle1->SetScale(_vec3(WEALD_PATHSIZEX, WEALD_PATHSIZEX, 1.f));

	// 골동품
	// 텐트
	shared_ptr<CGameObject>m_pCurioT1 = make_shared<CWeald_Curio_Tent>(m_pGraphicDev);
	m_pCurioT1->SetPos(_vec3(WEALD_WALLSIZEX * 2.f + WEALD_WALLSIZEX / 2.f + WEALD_WALLSIZEX * 1.f, WEALD_WALLSIZEUPY - 2.f, WEALD_WALLSIZEX * 1.f));
	m_pCurioT1->SetAngle(_vec3(0.f, 0.f, 0.f));
	m_pCurioT1->SetScale(_vec3(WEALD_WALLSIZEX/3.f, (WEALD_WALLSIZEX / 3.f) / 480.f * 302.f, 1.f));

	// 거미줄
	shared_ptr<CGameObject>m_pCurioS1 = make_shared<CWeald_Curio_Spider>(m_pGraphicDev);
	m_pCurioS1->SetPos(_vec3(WEALD_WALLSIZEX * 2.f + WEALD_WALLSIZEX / 2.f + WEALD_WALLSIZEX * 1.f, WEALD_WALLSIZEUPY - 2.f, WEALD_WALLSIZEX * 2.f + WEALD_WALLSIZEX * 9.f));
	m_pCurioS1->SetAngle(_vec3(0.f, 0.f, 0.f));
	m_pCurioS1->SetScale(_vec3(WEALD_WALLSIZEX / 4.f, (WEALD_WALLSIZEX / 4.f) / 268.f * 340.f, 1.f));

	// 가보상자
	shared_ptr<CGameObject>m_pCurioC1 = make_shared<CWeald_Curio_Chest>(m_pGraphicDev);
	m_pCurioC1->SetPos(_vec3(WEALD_WALLSIZEX + WEALD_PATHSIZEX + 22.f, WEALD_WALLSIZEUPY - 2.5f, 230.f));
	m_pCurioC1->SetAngle(_vec3(0.f, 0.f, 0.f));
	m_pCurioC1->SetScale(_vec3(WEALD_WALLSIZEX / 5.f, (WEALD_WALLSIZEX / 5.f) / 279.f * 220.f, 1.f));

	//가보상자 조명
	D3DLIGHT9 _pLightInfo1;

	_pLightInfo1.Diffuse = { 0.5f , 0.5f , 0.5f , 1.f };
	_pLightInfo1.Specular = { 1.f , 1.f , 1.f , 1.f };
	_pLightInfo1.Ambient = { 1.f , 1.f , 1.f , 1.f };
	_pLightInfo1.Position = { WEALD_WALLSIZEX + WEALD_PATHSIZEX + 22.f , WEALD_WALLSIZEUPY + 4.f , 230.f + 10.f };
	_pLightInfo1.Range = 20.f;

	_pLightInfo1.Attenuation0 = 0.f;
	_pLightInfo1.Attenuation1 = 0.f;
	_pLightInfo1.Attenuation2 = 0.f;

	CLightMgr::GetInstance()->InitPointLight(m_pGraphicDev, _pLightInfo1, 99);


	// 독성상자
	shared_ptr<CGameObject>m_pCurioL1 = make_shared<CWeald_Curio_Luggage>(m_pGraphicDev);
	m_pCurioL1->SetPos(_vec3(WEALD_WALLSIZEX * 2.f + WEALD_WALLSIZEX / 2.f + WEALD_WALLSIZEX * 1.f, WEALD_WALLSIZEUPY - 2.5f, WEALD_WALLSIZEX * 2.f + WEALD_WALLSIZEX * 17.f));
	m_pCurioL1->SetAngle(_vec3(0.f, 0.f, 0.f));
	m_pCurioL1->SetScale(_vec3(WEALD_WALLSIZEX / 5.f, (WEALD_WALLSIZEX / 5.f) / 276.f * 182.f, 1.f));

	// 장애물
	shared_ptr<CGameObject>m_pObstacle2 = make_shared<CWeald_Obstacle>(m_pGraphicDev);
	m_pObstacle2->SetPos(_vec3(WEALD_WALLSIZEX * 2.f + WEALD_WALLSIZEX / 2.f + WEALD_WALLSIZEX * 1.f, WEALD_WALLSIZEUPY + 1.f, WEALD_WALLSIZEX * 2.f + WEALD_WALLSIZEX * 21.f));
	m_pObstacle2->SetAngle(_vec3(0.f, 0.f, 0.f));
	m_pObstacle2->SetScale(_vec3(WEALD_PATHSIZEX, WEALD_PATHSIZEX, 1.f));

	// items
	// 1번방
	shared_ptr<CGameObject> m_pItem1 = make_shared<CItem>(m_pGraphicDev);
	m_pItem1->SetColliding(true);
	m_pLayer->CreateGameObject(L"Obj_TestItem", m_pItem1);
	dynamic_pointer_cast<CItem>(m_pItem1)->SetDropItemInfo({ 56.f, -50.f, 14.f }, L"Player_Item_Bandage", 1);

	shared_ptr<CGameObject> m_pItem2 = make_shared<CItem>(m_pGraphicDev);
	m_pItem2->SetColliding(true);
	m_pLayer->CreateGameObject(L"Obj_TestItem2", m_pItem2);
	dynamic_pointer_cast<CItem>(m_pItem2)->SetDropItemInfo({ 60.f, -50.f, 14.f }, L"Item_Heirlooms",1);

	shared_ptr<CGameObject> m_pItem3 = make_shared<CItem>(m_pGraphicDev);
	m_pItem3->SetColliding(true);
	m_pLayer->CreateGameObject(L"Obj_TestItem3", m_pItem3);
	dynamic_pointer_cast<CItem>(m_pItem3)->SetDropItemInfo({ 64.f, -50.f, 14.f }, L"Item_Torch",3);

	// 2번방
	shared_ptr<CGameObject> m_pItem1_2 = make_shared<CItem>(m_pGraphicDev);
	m_pItem1_2->SetColliding(true);
	m_pLayer->CreateGameObject(L"Obj_TestItem5", m_pItem1_2);
	dynamic_pointer_cast<CItem>(m_pItem1_2)->SetDropItemInfo({ 56.f, -50.f, 14.f }, L"Item_Gold2", 1000);

	shared_ptr<CGameObject> m_pItem2_2 = make_shared<CItem>(m_pGraphicDev);
	m_pItem2_2->SetColliding(true);
	m_pLayer->CreateGameObject(L"Obj_TestItem6", m_pItem2_2);
	dynamic_pointer_cast<CItem>(m_pItem2_2)->SetDropItemInfo({ 60.f, -50.f, 14.f }, L"Item_Gold2", 1000);

	shared_ptr<CGameObject> m_pItem3_2 = make_shared<CItem>(m_pGraphicDev);
	m_pItem3_2->SetColliding(true);
	m_pLayer->CreateGameObject(L"Obj_TestItem7", m_pItem3_2);
	dynamic_pointer_cast<CItem>(m_pItem3_2)->SetDropItemInfo({ 64.f, -50.f, 14.f }, L"Item_Key",1);

	shared_ptr<CGameObject> m_pItem4_2 = make_shared<CItem>(m_pGraphicDev);
	m_pItem4_2->SetColliding(true);
	m_pLayer->CreateGameObject(L"Obj_TestItem4", m_pItem4_2);
	dynamic_pointer_cast<CItem>(m_pItem4_2)->SetDropItemInfo({ 68.f, -50.f, 14.f }, L"Player_Item_Shovel", 1);

	// 3번방
	shared_ptr<CGameObject> m_pItem1_3 = make_shared<CItem>(m_pGraphicDev);
	m_pItem1_3->SetColliding(true);
	m_pLayer->CreateGameObject(L"Obj_TestItem8", m_pItem1_3);
	dynamic_pointer_cast<CItem>(m_pItem1_3)->SetDropItemInfo({ 56.f, -50.f, 14.f }, L"Item_Heirlooms", 1);

	shared_ptr<CGameObject> m_pItem2_3 = make_shared<CItem>(m_pGraphicDev);
	m_pItem2_3->SetColliding(true);
	m_pLayer->CreateGameObject(L"Obj_TestItem9", m_pItem2_3);
	dynamic_pointer_cast<CItem>(m_pItem2_3)->SetDropItemInfo({ 60.f, -50.f, 14.f }, L"Item_Gold2", 1000);

	shared_ptr<CGameObject> m_pItem3_3 = make_shared<CItem>(m_pGraphicDev);
	m_pItem3_3->SetColliding(true);
	m_pLayer->CreateGameObject(L"Obj_TestItem10", m_pItem3_3);
	dynamic_pointer_cast<CItem>(m_pItem3_3)->SetDropItemInfo({ 64.f, -50.f, 14.f }, L"Player_Item_Antivenom", 1);

	// 4번방
	shared_ptr<CGameObject> m_pItem1_4 = make_shared<CItem>(m_pGraphicDev);
	m_pItem1_4->SetColliding(true);
	m_pLayer->CreateGameObject(L"Obj_TestItem11", m_pItem1_4);
	dynamic_pointer_cast<CItem>(m_pItem1_4)->SetDropItemInfo({ 56.f, -50.f, 14.f }, L"Item_Heirlooms", 1);

	shared_ptr<CGameObject> m_pItem2_4 = make_shared<CItem>(m_pGraphicDev);
	m_pItem2_4->SetColliding(true);
	m_pLayer->CreateGameObject(L"Obj_TestItem12", m_pItem2_4);
	dynamic_pointer_cast<CItem>(m_pItem2_4)->SetDropItemInfo({ 60.f, -50.f, 14.f }, L"Item_Gold2", 1000);

	shared_ptr<CGameObject> m_pItem3_4 = make_shared<CItem>(m_pGraphicDev);
	m_pItem3_4->SetColliding(true);
	m_pLayer->CreateGameObject(L"Obj_TestItem13", m_pItem3_4);
	dynamic_pointer_cast<CItem>(m_pItem3_4)->SetDropItemInfo({ 64.f, -50.f, 14.f }, L"Item_Gold2", 1000);
	
	// monsters
	shared_ptr<CGameObject> m_pBrigandCutthroat_1 = make_shared<CBrigandCutthroat>(m_pGraphicDev);
	shared_ptr<CGameObject> m_pBrigandCutthroat_2 = make_shared<CBrigandCutthroat>(m_pGraphicDev);
	shared_ptr<CGameObject> m_pBrigandBloodletter1 = make_shared<CBrigandBloodletter>(m_pGraphicDev);
	shared_ptr<CGameObject> m_pBrigandFusilier_1 = make_shared<CBrigandFusilier>(m_pGraphicDev);

	// heroes
	shared_ptr<CGameObject> m_pJester = make_shared<CJester>(m_pGraphicDev);
	shared_ptr<CGameObject> m_pHighwayman = make_shared<CHighwayman>(m_pGraphicDev);

	vector<shared_ptr<CGameObject>> Heroes_v;
	Heroes_v.push_back(m_pHighwayman);
	Heroes_v.push_back(m_pJester);
	dynamic_pointer_cast<CPlayer>(m_pPlayer)->SetHeroVec(&Heroes_v);

	// 방에 GameObject 넣기
	// Room1
	vector<shared_ptr<CGameObject>> Room1_v1;
	Room1_v1.push_back(m_pCurioT1);
	Room1_v1.push_back(m_pItem1);
	Room1_v1.push_back(m_pItem2);
	Room1_v1.push_back(m_pItem3);
	m_pRoom1->PushGameObjectVector(Room1_v1);

	// Room2
	vector<shared_ptr<CGameObject>> Room2_v1;
	Room2_v1.push_back(m_pItem1_2);
	Room2_v1.push_back(m_pItem2_2);
	Room2_v1.push_back(m_pItem3_2);
	Room2_v1.push_back(m_pItem4_2);
	Room2_v1.push_back(m_pTrap1);
	Room2_v1.push_back(m_pObstacle1);
	Room2_v1.push_back(m_pCurioS1);
	m_pRoom2->PushGameObjectVector(Room2_v1);
	
	// Room3
	// objects
	vector<shared_ptr<CGameObject>> Room3_v1;
	Room3_v1.push_back(m_pItem1_3);
	Room3_v1.push_back(m_pItem2_3);
	Room3_v1.push_back(m_pItem3_3);
	Room3_v1.push_back(m_pBrigandCutthroat_1);
	Room3_v1.push_back(m_pBrigandBloodletter1);
	Room3_v1.push_back(m_pBrigandCutthroat_2);
	Room3_v1.push_back(m_pBrigandFusilier_1);

	Room3_v1.push_back(m_pCurioC1);

	m_pRoom3->PushGameObjectVector(Room3_v1);

	// creatures
	vector<shared_ptr<CGameObject>> Room3_v4;
	Room3_v4.push_back(m_pBrigandCutthroat_1);
	Room3_v4.push_back(m_pBrigandBloodletter1);
	Room3_v4.push_back(m_pBrigandCutthroat_2);
	Room3_v4.push_back(m_pBrigandFusilier_1);

	Room3_v4.push_back(m_pHighwayman);
	Room3_v4.push_back(m_pJester);

	// heroes
	vector<shared_ptr<CGameObject>> Room3_v2;
	Room3_v2.push_back(m_pHighwayman);
	Room3_v2.push_back(m_pJester);
	m_pRoom3->PushHeroesVector(Room3_v2);

	// monsters
	vector<shared_ptr<CGameObject>> Room3_v3;
	Room3_v3.push_back(m_pBrigandBloodletter1);
	Room3_v3.push_back(m_pBrigandCutthroat_1);
	Room3_v3.push_back(m_pBrigandCutthroat_2);
	Room3_v3.push_back(m_pBrigandFusilier_1);
	m_pRoom3->PushMonstersVector(Room3_v3);

	// 배틀시스템 넣기
	shared_ptr<CBattleSystem> pRoom3_Battle = make_shared<CBattleSystem>();
	//pRoom3_Battle->Ready();
	pRoom3_Battle->PushCreaturesVector(Room3_v4);
	pRoom3_Battle->PushHeroesVector(Room3_v2);
	pRoom3_Battle->PushMonstersVector(Room3_v3);
	m_pRoom3->SetBattleSystem(pRoom3_Battle);
	m_pRoom3->SetBattleCameraPos(_vec3(WEALD_WALLSIZEX + WEALD_PATHSIZEX + 21.3f, 3.f, WEALD_WALLSIZEX * 14.f + 4.f ));
	
	//BattleUI Test
	shared_ptr<CBattleHeroUI> m_pHeroUI = make_shared<CBattleHeroUI>(m_pGraphicDev);
	m_pHeroUI->AwakeGameObject();
	m_pHeroUI->ReadyGameObject();
	CUIMgr::GetInstance()->AddUIObject( L"Battle_Hero_UI",dynamic_pointer_cast<CUIObj>(m_pHeroUI));
	pRoom3_Battle->GetHeroPanelUI(m_pHeroUI);

	// Room4
	vector<shared_ptr<CGameObject>> Room4_v1;
	Room4_v1.push_back(m_pCurioL1);
	Room4_v1.push_back(m_pItem1_4);
	Room4_v1.push_back(m_pItem2_4);
	Room4_v1.push_back(m_pItem3_4);
	Room4_v1.push_back(m_pObstacle2);
	m_pRoom4->PushGameObjectVector(Room4_v1);

	// 던전에 방 넣기
	vector<shared_ptr<CDungeonRoom>> Dungeon1_v;
	Dungeon1_v.push_back(m_pRoom1);
	Dungeon1_v.push_back(m_pRoom2);
	Dungeon1_v.push_back(m_pRoom3);
	Dungeon1_v.push_back(m_pRoom4);
	Dungeon1_v.push_back(m_pRoom5);
	m_pWealdDungeon->PushDungeonRoomVector(Dungeon1_v);

	// 현재 active 방
	m_pWealdDungeon->DisableAllRoom();
	m_pWealdDungeon->AbleRoom(1);
	dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SetCurrentRoom(1);

	// Layer에 GameObject 넣기
	m_pLayer->CreateGameObject(L"Obj_BrigandCutthroat", m_pBrigandCutthroat_1);
	m_pLayer->CreateGameObject(L"Obj_BrigandCutthroat", m_pBrigandCutthroat_2);
	m_pLayer->CreateGameObject(L"Obj_BrigandBloodletter", m_pBrigandBloodletter1);
	m_pLayer->CreateGameObject(L"Obj_BrigandFusilier", m_pBrigandFusilier_1);
	m_pLayer->CreateGameObject(L"Obj_Highwayman", m_pHighwayman);
	dynamic_pointer_cast<CHero>(m_pHighwayman)->SetObjKey(L"Obj_Highwayman");
	m_pLayer->CreateGameObject(L"Obj_Jester", m_pJester);
	dynamic_pointer_cast<CHero>(m_pJester)->SetObjKey(L"Obj_Jester");
	m_pLayer->CreateGameObject(L"OBJ_Trap", m_pTrap1);
	m_pLayer->CreateGameObject(L"OBJ_Obstacle", m_pObstacle1);
	m_pLayer->CreateGameObject(L"OBJ_Obstacle", m_pObstacle2);
	m_pLayer->CreateGameObject(L"OBJ_CurioTent", m_pCurioT1);
	m_pLayer->CreateGameObject(L"OBJ_CurioSpider", m_pCurioS1);
	m_pLayer->CreateGameObject(L"OBJ_CurioChest", m_pCurioC1);
	m_pLayer->CreateGameObject(L"OBJ_CurioLuggage", m_pCurioL1);

//PlayerObj
	shared_ptr<CGameObject> m_pPlayerHand = make_shared<CPlayerHand>(m_pGraphicDev);
	m_pLayer->CreateGameObject(L"Obj_PlayerHand", m_pPlayerHand);
	(dynamic_pointer_cast<CPlayer>(m_pPlayer))->SetPlayerHand(dynamic_pointer_cast<CPlayerHand>(m_pPlayerHand));
	
	dynamic_pointer_cast<CLayer>(m_pLayer)->AwakeLayer();

	return S_OK;
}

HRESULT CWeald_Dungeon::Ready_Layer_UI(tstring pLayerTag)
{
	shared_ptr<CLayer> m_pLayer = make_shared<CLayer>();
	m_mapLayer.insert({ pLayerTag, m_pLayer });

	shared_ptr<CGameObject> m_pInventory = make_shared<CInventory>(m_pGraphicDev);
	m_pLayer->CreateGameObject(L"Obj_UI", m_pInventory);

	CUIMgr::GetInstance()->AddUIObject(L"UI_Inventory", dynamic_pointer_cast<CUIObj>(m_pInventory));
	
	shared_ptr<CGameObject> m_pNarr = make_shared<CNarration>(m_pGraphicDev);
	CUIMgr::GetInstance()->AddUIObject(L"UI_Narration", dynamic_pointer_cast<CUIObj>(m_pNarr));
	m_pLayer->CreateGameObject(L"Obj_Narr", m_pNarr);

	shared_ptr<CGameObject> m_pTextBoard = make_shared<CTextBoard>(m_pGraphicDev);
	CUIMgr::GetInstance()->AddUIObject(L"UI_TextBoard", dynamic_pointer_cast<CUIObj>(m_pTextBoard));
	m_pLayer->CreateGameObject(L"Obj_TextBoard", m_pTextBoard);

	shared_ptr<CGameObject> pMouse = make_shared<CMouseCursor>(m_pGraphicDev);
	m_pLayer->CreateGameObject(L"Obj_Mouse", pMouse);
	CUIMgr::GetInstance()->AddUIObject(L"UI_Mouse", dynamic_pointer_cast<CUIObj>(pMouse));

	shared_ptr<CGameObject> pPlayerUI = make_shared<CPlayerFPSUI>(m_pGraphicDev);
	m_pLayer->CreateGameObject(L"Obj_Player_FPSUI", pPlayerUI);
	CUIMgr::GetInstance()->AddUIObject(L"UI_Player_FPSUI", dynamic_pointer_cast<CUIObj>(pPlayerUI));

	shared_ptr<CGameObject> pInteractionInfo = make_shared<CInteractionInfo>(m_pGraphicDev);
	m_pLayer->CreateGameObject(L"Obj_InteractionInfo", pInteractionInfo);
	CUIMgr::GetInstance()->AddUIObject(L"UI_InteractionInfo", dynamic_pointer_cast<CUIObj>(pInteractionInfo));

	// Description UI
	shared_ptr<CGameObject> pDescriptionUI = make_shared<CDescription>(m_pGraphicDev);
	m_pLayer->CreateGameObject(L"Obj_DescriptionUI", pDescriptionUI);

	CUIMgr::GetInstance()->AddUIObject(L"Obj_DescriptionUI", dynamic_pointer_cast<CUIObj>(pDescriptionUI));

	shared_ptr<CGameObject> pDungeonStatusUI = make_shared<CDungeonStatus>(m_pGraphicDev);
	m_pLayer->CreateGameObject(L"Obj_DungeonStatus", pDungeonStatusUI);
	CUIMgr::GetInstance()->AddUIObject(L"UI_DungeonStatus", dynamic_pointer_cast<CUIObj>(pDungeonStatusUI));

	shared_ptr<CGameObject> pScreenEffect = make_shared<CScreenEffect>(m_pGraphicDev);
	m_pLayer->CreateGameObject(L"Obj_ScreenEffect", pScreenEffect);
	CUIMgr::GetInstance()->AddUIObject(L"UI_ScreenEffect", dynamic_pointer_cast<CUIObj>(pScreenEffect));

	dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SetInventory(dynamic_pointer_cast<CInventory>(m_pInventory));
	dynamic_pointer_cast<CLayer>(m_pLayer)->AwakeLayer();

	return S_OK;
}
