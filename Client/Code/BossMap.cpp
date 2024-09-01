#include "pch.h"
#include "BossMap.h"
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

#include "Highwayman.h"
#include "Jester.h"
#include "Vestal.h"
#include "ShieldBreaker.h"

#include "Boss2.h"
#include "Bullet1.h"
#include "Bullet2.h"
#include "Bullet3.h"
#include "Mob_Bullet.h"
#include "Laser.h"
#include "Mob.h"
#include "Spike.h"
#include "Sunken.h"

#include "BrigandBloodletter.h"
#include "Alien.h"
#include "AlienMob.h"

#include "Export_System.h"
#include "Export_Utility.h"

#include"ResourceMgr.h"

#include"UIMgr.h"
#include"BattleHeroUI.h"
#include"SoundMgr.h"


#include"LightMgr.h"
#include"Narration.h"

#include"InteractionInfo.h"
#include "DungeonStatus.h"

CBossMap::CBossMap(LPDIRECT3DDEVICE9 pGraphicDev)
	:CScene(pGraphicDev)
{
	ZeroMemory(m_szString, sizeof(m_szString));	
}

CBossMap::~CBossMap()
{
}

HRESULT CBossMap::ReadyScene()
{
	CSoundMgr::GetInstance()->PlayBGM(L"Boss_BGM0.wav", 1.2f);

	// text
	lstrcpy(m_szString, L"Boss Map");
	// BossMap
	m_pBossMap = make_shared<CDungeonSystem>();
	// 방1
	m_pRoom1 = make_shared<CDungeonRoom>();
	m_pRoom2 = make_shared<CDungeonRoom>();
	m_pRoom3 = make_shared<CDungeonRoom>();

	Ready_Layer_Environment(L"Layer_3_Environment");
	Ready_Layer_SkyBox(L"Layer_1_SkyBox");
	Ready_Layer_GameObject(L"Layer_4_GameObj");
	Ready_Layer_UI(L"Layer_2_UI");
	CUIMgr::GetInstance()->SceneUIInitialize();
	Ready_Layer_Camera(L"Layer_5_Camera");

	for (auto& iter : m_mapLayer) { 
		iter.second->ReadyLayer();
	}
	//dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SetPlayerMode(EPlayerMode::BOSS_FIELD);

	SetLight();

 	return __super::ReadyScene();
}

_int CBossMap::UpdateScene(const _float& fTimeDelta)
{
	KeyInput();

	// 전투 트리거
	if (dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->GetCurrentRoom() == 2 && !m_pRoom2->GetBattleStart())
	{
		shared_ptr<CTransform> pTransform = dynamic_pointer_cast<CTransform>((CGameMgr::GetInstance()->GetPlayer())->GetComponent(L"Com_Transform", ID_DYNAMIC));

		if (pTransform->GetPos()->z > m_fBattlePositionZ)
		{
			_vec3* vPos = pTransform->GetPos();
			CCameraMgr::GetInstance()->SetState(ECameraMode::BATTLE);
			CCameraMgr::GetInstance()->MovingStraight(ECameraMode::ZOOMIN, { vPos->x , vPos->y + 5.f ,  m_fBattlePositionZ - 20.f });
			CUIMgr::GetInstance()->SelectUIVisibleOff(L"UI_Inventory");
			CUIMgr::GetInstance()->SelectUIVisibleOff(L"UI_DungeonStatus");

			//전투 시작 이펙트
			shared_ptr<CEffect> pEffect = CEffectMgr::GetInstance()->GetEffect();
			if (pEffect) {
				pEffect->SetAnimEffect(L"Effect_BattleStart", _vec3(vPos->x, vPos->y + 8.f, m_fBattlePositionZ - 5.f), _vec3(3.5f, 3.5f, 3.5f), 1.5f, false);
				pEffect->SetActive(true);
			}

			dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SetBattleTrigger(true);
			CSoundMgr::GetInstance()->CSoundMgr::GetInstance()->StopSound(CHANNELID::BGM);
			CSoundMgr::GetInstance()->PlayBGM(L"Boss_TurnBattleBGM.wav", 0.6f);
			pTransform->SetPosition(300.f, 0.f, m_fBattlePositionZ - 20.f);
			m_pRoom2->SetBattleStart(true);
			dynamic_pointer_cast<CInteractionInfo>(CUIMgr::GetInstance()->FindUI(L"UI_InteractionInfo"))->SetIsBattle(true);

			m_pRoom2->SetBattleCameraOriginPos({ vPos->x , vPos->y + 5.f ,  m_fBattlePositionZ - 20.f });
		}
	}

	// 전투시작되는 메커니즘
	if (dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->GetCurrentRoom() == 2)
	{
		if (dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->GetBattleTrigger())
		{
			ChangeLightInfo();

			m_pRoom2->SetBattleTrigger(true);
			// 전투준비
			m_pRoom2->BattleReady();
			dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SetInBattle(true);

			dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SetBattleTrigger(false);
		}
	}

	// 전투
	if (dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->GetCurrentRoom() == 2)
	{
		if (m_pRoom2->GetBattleTrigger())
		{
			// 전투가 끝나면
			if (m_pRoom2->BattleUpdate(fTimeDelta))
			{
				CCameraMgr::GetInstance()->SetFPSMode();
				m_pRoom2->SetBattleTrigger(false);
				dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SetInBattle(false);

				m_b3DBattleStart = true;
			}
		}
	}

	// 3D전투 시작될때
	if (m_b3DBattleStart && !m_bBattleReady)
	{

		CSoundMgr::GetInstance()->CSoundMgr::GetInstance()->StopSound(CHANNELID::BGM);
		CSoundMgr::GetInstance()->PlayBGM(L"Boss_BGM1.wav", 0.6f);
		m_pBossMap->DisableAllRoom();
		m_pBossMap->AbleRoom(1);
		dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SetCurrentRoom(1);

		dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SetSpeed(30.f);
		dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SetPlayerMode(EPlayerMode::BOSS_FIELD);
		m_bBattleReady = true;
	}


	int iExit;
	iExit = __super::UpdateScene(fTimeDelta);
	
	return iExit;
}

void CBossMap::LateUpdateScene()
{
	__super::LateUpdateScene();

}

void CBossMap::RenderScene()
{
}

void CBossMap::SetLight()
{
	CLightMgr::GetInstance()->DungeonLightAllOff();

	D3DLIGHT9 _pLightInfo1;

	_pLightInfo1.Diffuse = { 0.2f , 0.2f , 0.2f , 1.f };
	_pLightInfo1.Specular = { 0.2f , 0.2f , 0.2f , 1.f };
	_pLightInfo1.Ambient = { 0.2f , 0.2f , 0.2f , 1.f };
	_pLightInfo1.Direction = { 1.f, -1.f, 1.f };

	m_pDirLight = CLightMgr::GetInstance()->InitDirectionLight(m_pGraphicDev, _pLightInfo1);


	D3DLIGHT9 _pLightInfo2;

	_pLightInfo2.Diffuse = { 0.5f , 0.5f , 0.5f , 1.f };
	_pLightInfo2.Specular = { 1.f , 1.f , 1.f , 1.f };
	_pLightInfo2.Ambient = { 1.f , 1.f , 1.f , 1.f };
	_pLightInfo2.Position = { 300.f, 0.f, m_fBattlePositionZ };
	_pLightInfo2.Range = 20.f;

	_pLightInfo2.Attenuation0 = 0.f;
	_pLightInfo2.Attenuation1 = 0.f;
	_pLightInfo2.Attenuation2 = 0.f;

	m_pPointLight = CLightMgr::GetInstance()->InitPointLight(m_pGraphicDev, _pLightInfo2);

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->PlayerLightOff();



}

void CBossMap::ChangeLightInfo()
{

	m_pPointLight->LightOff();
	m_pDirLight->LightOff();


	D3DLIGHT9 _pLightInfo1;

	_pLightInfo1.Diffuse = { 0.8f , 0.8f , 0.8f , 1.f };
	_pLightInfo1.Specular = { 0.8f , 0.8f , 0.8f , 1.f };
	_pLightInfo1.Ambient = { 0.8f , 0.8f , 0.8f , 1.f };
	_pLightInfo1.Direction = { 1.f, -1.f, 1.f };

	CLightMgr::GetInstance()->InitDirectionLight(m_pGraphicDev, _pLightInfo1);

}

void CBossMap::SetWall(shared_ptr<CWall> _pWall, shared_ptr<CLayer> _pLayer,
	_float _fX, _float _fZ, _float _fSize, tstring _tKeyName)
{
	for (int i = 0; i < 5; i++)
	{
		switch (i)
		{
		case 0:
			_pWall = make_shared<CWall>(m_pGraphicDev, L"Com_Boss_ObjectWallTexture", 1, true);
			_pWall->SetPos(_vec3(_fX, _fSize, _fZ + _fSize));
			_pWall->SetAngle(_vec3(0.f, PI * 0.5f, 0.f));
			_pWall->SetScale(_vec3(_fSize, _fSize, 1.f));
			dynamic_pointer_cast<CWall>(_pWall)->SetKey2Name(_tKeyName);
			_pLayer->CreateGameObject(L"OBJ_Wall", _pWall);
			m_pVecWallList.push_back(_pWall);
			break;
		case 1:
			_pWall = make_shared<CWall>(m_pGraphicDev, L"Com_Boss_ObjectWallTexture", 1, true);
			_pWall->SetPos(_vec3(_fX, _fSize, _fZ - _fSize));
			_pWall->SetAngle(_vec3(0.f, PI * 0.5f, 0.f));
			_pWall->SetScale(_vec3(_fSize, _fSize, 1.f));
			dynamic_pointer_cast<CWall>(_pWall)->SetKey2Name(_tKeyName);
			_pLayer->CreateGameObject(L"OBJ_Wall", _pWall);
			m_pVecWallList.push_back(_pWall);
			break;
		case 2:
			_pWall = make_shared<CWall>(m_pGraphicDev, L"Com_Boss_ObjectWallTexture", 1, true);
			_pWall->SetPos(_vec3(_fX - _fSize, _fSize, _fZ));
			_pWall->SetAngle(_vec3(0.f, 0.f, 0.f));
			_pWall->SetScale(_vec3(_fSize, _fSize, 1.f));
			dynamic_pointer_cast<CWall>(_pWall)->SetKey2Name(_tKeyName);
			_pLayer->CreateGameObject(L"OBJ_Wall", _pWall);
			m_pVecWallList.push_back(_pWall);
			break;
		case 3:
			_pWall = make_shared<CWall>(m_pGraphicDev, L"Com_Boss_ObjectWallTexture", 1, true);
			_pWall->SetPos(_vec3(_fX + _fSize, _fSize, _fZ));
			_pWall->SetAngle(_vec3(0.f, 0.f, 0.f));
			_pWall->SetScale(_vec3(_fSize, _fSize, 1.f));
			dynamic_pointer_cast<CWall>(_pWall)->SetKey2Name(_tKeyName);
			_pLayer->CreateGameObject(L"OBJ_Wall", _pWall);
			m_pVecWallList.push_back(_pWall);
			break; 

		case 4:
			_pWall = make_shared<CWall>(m_pGraphicDev, L"Com_Boss_ObjectWallTopTexture", 1, true);
			_pWall->SetPos(_vec3(_fX, _fSize * 2.f, _fZ));
			_pWall->SetAngle(_vec3(PI * 0.5f, 0.f, 0.f));
			_pWall->SetScale(_vec3(_fSize, _fSize, 1.f));
			dynamic_pointer_cast<CWall>(_pWall)->SetKey2Name(_tKeyName);
			_pLayer->CreateGameObject(L"OBJ_Wall", _pWall);
			m_pVecWallList.push_back(_pWall);
			break;
		}
	}
}

void CBossMap::KeyInput()
{
	if (GetAsyncKeyState('7') & 0x8000) {
		m_b3DBattleStart = true;
	}

	if (GetAsyncKeyState('8') & 0x8000) {
		shared_ptr<CTransform> pTransform = dynamic_pointer_cast<CTransform>((CGameMgr::GetInstance()->GetPlayer())->GetComponent(L"Com_Transform", ID_DYNAMIC));
		pTransform->SetPosition(300.f, 0.f, m_fBattlePositionZ - 20.f);
		dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SetCurrentRoom(2);
		m_pBossMap->DisableAllRoom();
		m_pBossMap->AbleRoom(2);
	}

	// 전투 강제종료
	if (GetAsyncKeyState('9') & 0x8000) {
		m_pRoom2->GetBattleSystem()->EndBattle();
		CSoundMgr::GetInstance()->StopAll();
		CSoundMgr::GetInstance()->PlayBGM(L"Boss_BGM1.wav", 0.6f);
		CUIMgr::GetInstance()->SelectUIVisibleOn(L"UI_Inventory");
		CUIMgr::GetInstance()->SelectUIVisibleOff(L"Battle_Hero_UI");
		CUIMgr::GetInstance()->SelectUIVisibleOn(L"UI_DungeonStatus");
		dynamic_pointer_cast<CInteractionInfo>(CUIMgr::GetInstance()->FindUI(L"UI_InteractionInfo"))->SetIsBattle(false);

		m_pRoom2->SetBattleTrigger(false);
		dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SetInBattle(false);

		m_b3DBattleStart = true;
	}
}

HRESULT CBossMap::Ready_Layer_Environment(tstring pLayerTag)
{
	shared_ptr<CLayer> m_pLayer = make_shared<CLayer>();
	m_mapLayer.insert({ pLayerTag, m_pLayer });

	shared_ptr<CGameObject> m_pTerrain = make_shared<CTerrain>(m_pGraphicDev, L"Com_Boss_FloorTexture", ETerrainType::DUNGEON2);
	m_pLayer->CreateGameObject(L"OBJ_Terrain", m_pTerrain);

	shared_ptr<CWall> _pWall;


	////벽
	//for (int i = 0; i < 20; i++)
	//{
	//	_pWall = make_shared<CWall>(m_pGraphicDev, L"Com_Boss_WallTexture", 6, true);
	//	_pWall->SetPos(_vec3(0.f, BOSS_WALLSIZEUPY, BOSS_WALLSIZEX + BOSS_WALLSIZEX * 2.f * i));
	//	_pWall->SetAngle(_vec3(0.f, 0.f, 0.f));
	//	_pWall->SetScale(_vec3(BOSS_WALLSIZEX, BOSS_WALLSIZEX, 1.f));
	//	m_pLayer->CreateGameObject(L"OBJ_Wall", _pWall);
	//}
	//for (int i = 0; i < 20; i++)
	//{
	//	_pWall = make_shared<CWall>(m_pGraphicDev, L"Com_Boss_WallTexture", 6, true);
	//	_pWall->SetPos(_vec3(640.f, BOSS_WALLSIZEUPY, BOSS_WALLSIZEX + BOSS_WALLSIZEX * 2.f * i));
	//	_pWall->SetAngle(_vec3(0.f, 0.f, 0.f));
	//	_pWall->SetScale(_vec3(BOSS_WALLSIZEX, BOSS_WALLSIZEX, 1.f));
	//	m_pLayer->CreateGameObject(L"OBJ_Wall", _pWall);
	//}
	//for (int i = 0; i < 20; i++)
	//{
	//	_pWall = make_shared<CWall>(m_pGraphicDev, L"Com_Boss_WallTexture", 6, true);
	//	_pWall->SetPos(_vec3(BOSS_WALLSIZEX + BOSS_WALLSIZEX * 2.f * i, BOSS_WALLSIZEUPY, 0.f));
	//	_pWall->SetAngle(_vec3(0.f, PI * 0.5f, 0.f));
	//	_pWall->SetScale(_vec3(BOSS_WALLSIZEX, BOSS_WALLSIZEX, 1.f));
	//	m_pLayer->CreateGameObject(L"OBJ_Wall", _pWall);
	//}
	//for (int i = 0; i < 20; i++)
	//{
	//	_pWall = make_shared<CWall>(m_pGraphicDev, L"Com_Boss_WallTexture", 6, true);
	//	_pWall->SetPos(_vec3(BOSS_WALLSIZEX + BOSS_WALLSIZEX * 2.f * i, BOSS_WALLSIZEUPY, 640.f));
	//	_pWall->SetAngle(_vec3(0.f, PI * 0.5f, 0.f));
	//	_pWall->SetScale(_vec3(BOSS_WALLSIZEX, BOSS_WALLSIZEX, 1.f));
	//	m_pLayer->CreateGameObject(L"OBJ_Wall", _pWall);
	//}

	// 장애물 배치
	SetWall(_pWall, m_pLayer, 200.f, 200.f, 20.f, L"Wall1");
	//SetWall(_pWall, m_pLayer, 300.f, 300.f, 20.f, L"Wall2");
	SetWall(_pWall, m_pLayer, 400.f, 400.f, 20.f, L"Wall3");
	SetWall(_pWall, m_pLayer, 200.f, 400.f, 20.f, L"Wall4");
	SetWall(_pWall, m_pLayer, 400.f, 200.f, 20.f, L"Wall5");


	//가장 최하위 순서에 돌려줄 것
	dynamic_pointer_cast<CLayer>(m_pLayer)->AwakeLayer();


	return S_OK;
}

HRESULT CBossMap::Ready_Layer_SkyBox(tstring pLayerTag)
{
	shared_ptr<CLayer> m_pLayer = make_shared<CLayer>();
	m_mapLayer.insert({ pLayerTag, m_pLayer });


	shared_ptr<CGameObject> m_pSkyBox = make_shared<CSkyBox>(m_pGraphicDev, L"Com_BossMap_SkyBoxTexture");
	m_pLayer->CreateGameObject(L"OBJ_BossMap_SkyBox", m_pSkyBox);

	dynamic_pointer_cast<CLayer>(m_pLayer)->AwakeLayer();

	return S_OK;
}

HRESULT CBossMap::Ready_Layer_Camera(tstring pLayerTag)
{
	shared_ptr<CLayer> m_pLayer = make_shared<CLayer>();
	m_mapLayer.insert({ pLayerTag, m_pLayer });

	shared_ptr<CGameObject> m_pCamera = make_shared<CStaticCamera>(m_pGraphicDev);
	m_pLayer->CreateGameObject(L"OBJ_Camera", m_pCamera);

	CCameraMgr::GetInstance()->SetMainCamera(dynamic_pointer_cast<CStaticCamera>(m_pCamera));
	CCameraMgr::GetInstance()->SetFPSMode();
	
	dynamic_pointer_cast<CLayer>(m_pLayer)->AwakeLayer();

	return S_OK;
}

HRESULT CBossMap::Ready_Layer_GameObject(tstring pLayerTag)
{
	shared_ptr<CLayer> m_pLayer = make_shared<CLayer>();
	m_mapLayer.insert({ pLayerTag, m_pLayer });

	// Player
	shared_ptr<CGameObject> m_pPlayer;

	m_pPlayer = dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer());
	m_pPlayer->SetPos({ 300.f, 0.f, 50.f });
	m_pLayer->CreateGameObject(L"Obj_Player", m_pPlayer);

	dynamic_pointer_cast<CPlayer>(m_pPlayer)->SetInDungeon(true);
	dynamic_pointer_cast<CPlayer>(m_pPlayer)->SetInBossRoom(true);
	dynamic_pointer_cast<CTransform>(m_pPlayer->GetComponent(L"Com_Transform", ID_DYNAMIC))->SetPosition(300.f, 0.f, 150.f);
	dynamic_pointer_cast<CTransform>(m_pPlayer->GetComponent(L"Com_Transform", ID_DYNAMIC))->SetAngle({ 0.f, 0.f, 0.f });

	for (auto& iter : *dynamic_pointer_cast<CPlayer>(m_pPlayer)->GetHeroVec())
	{
		m_pLayer->CreateGameObject(dynamic_pointer_cast<CHero>(iter)->GetObjKey(), iter);
	}
	dynamic_pointer_cast<CPlayer>(m_pPlayer)->SetInBattle(false);

	for (auto& iter : *dynamic_pointer_cast<CPlayer>(m_pPlayer)->GetHeroVec())
	{
		dynamic_pointer_cast<CHero>(iter)->SetInDungeon(true);
	}

	dynamic_pointer_cast<CPlayer>(m_pPlayer)->SetInDungeon(true);


	// Boss
	shared_ptr<CGameObject> m_pBoss = make_shared<CBoss2>(m_pGraphicDev);
	m_pBoss->SetPos({ 300.f, 30.f, 300.f });
	m_pLayer->CreateGameObject(L"Obj_Boss", m_pBoss);

	// Mob
	// 우
	shared_ptr<CGameObject> m_pMob1 = make_shared<CMob>(m_pGraphicDev);
	m_pMob1->SetPos({ 360.f, 5.f, 300.f });
	m_pLayer->CreateGameObject(L"Obj_Mob", m_pMob1);
	//좌
	shared_ptr<CGameObject> m_pMob2 = make_shared<CMob>(m_pGraphicDev);
	m_pMob2->SetPos({ 240.f, 5.f, 300.f });
	m_pLayer->CreateGameObject(L"Obj_Mob", m_pMob2);
	//우상
	shared_ptr<CGameObject> m_pMob3 = make_shared<CMob>(m_pGraphicDev);
	m_pMob3->SetPos({ 330.f, 5.f, 360.f });
	m_pLayer->CreateGameObject(L"Obj_Mob", m_pMob3);
	//좌상
	shared_ptr<CGameObject> m_pMob4 = make_shared<CMob>(m_pGraphicDev);
	m_pMob4->SetPos({ 270.f, 5.f, 360.f });
	m_pLayer->CreateGameObject(L"Obj_Mob", m_pMob4);

	shared_ptr<CGameObject> m_pMob5 = make_shared<CMob>(m_pGraphicDev);
	m_pMob5->SetPos({ 330.f, 5.f, 240.f });
	m_pLayer->CreateGameObject(L"Obj_Mob", m_pMob5);

	shared_ptr<CGameObject> m_pMob6 = make_shared<CMob>(m_pGraphicDev);
	m_pMob6->SetPos({ 270.f, 5.f, 240.f });
	m_pLayer->CreateGameObject(L"Obj_Mob", m_pMob6);

	// monsters
	shared_ptr<CGameObject> m_pAlienMob1 = make_shared<CAlienMob>(m_pGraphicDev);
	shared_ptr<CGameObject> m_pAlienMob2 = make_shared<CAlienMob>(m_pGraphicDev);
	shared_ptr<CGameObject> m_pAlien = make_shared<CAlien>(m_pGraphicDev);
	shared_ptr<CGameObject> m_pAlienMob3 = make_shared<CAlienMob>(m_pGraphicDev);

	vector<shared_ptr<CGameObject>> Heroes_v;
	for (auto& iter : *dynamic_pointer_cast<CPlayer>(m_pPlayer)->GetHeroVec())
	{
		Heroes_v.push_back(iter);
	}

	// BossBullet
	vector<shared_ptr<CBullet1>> pVecProjectile1;
	vector<shared_ptr<CBullet2>> pVecProjectile2;
	vector<shared_ptr<CBullet3>> pVecProjectile3;
	vector<shared_ptr<CLaser>> pVecLaser;
	vector<shared_ptr<CSpike>> pVecSpike;
	vector<shared_ptr<CSunken>> pVecSunken;
	shared_ptr<CBullet1> pBullet1;
	shared_ptr<CBullet2> pBullet2;
	shared_ptr<CBullet3> pBullet3;
	shared_ptr<CMobBullet> pMobBullet;
	shared_ptr<CLaser> pLaser;
	shared_ptr<CSpike> pSpike;
	shared_ptr<CSunken> pSunken;

	// 스파이크
	for (int i = 0; i < 200; i++)
	{
		pSpike = nullptr;
		pSpike = make_shared<CSpike>(m_pGraphicDev);
		pSpike->SetPos({ 300.f, -500.f, 300.f });
		pVecSpike.push_back(pSpike);
		m_pLayer->CreateGameObject(L"Obj_Spike", pVecSpike[i]);
	}
	// 성큰
	for (int i = 0; i < 400; i++)
	{
		pSunken = nullptr;
		pSunken = make_shared<CSunken>(m_pGraphicDev);
		pSunken->SetPos({ 200.f, -500.f, 300.f });
		pVecSunken.push_back(pSunken);
		m_pLayer->CreateGameObject(L"Obj_Sunken", pVecSunken[i]);
	}

	for (int i = 0; i < 50; i++)
	{
		pBullet1 = nullptr;
		pBullet1 = make_shared<CBullet1>(m_pGraphicDev);
		pBullet1->SetPos({ -100.f, -100.f, -100.f });
		pVecProjectile1.push_back(pBullet1);
		m_pLayer->CreateGameObject(L"Obj_Boss_P1_Bullet1", pVecProjectile1[i]);
	}
	for (int i = 0; i < 800; i++)
	{
		pBullet2 = nullptr;
		pBullet2 = make_shared<CBullet2>(m_pGraphicDev);
		pBullet2->SetPos({ -100.f, -200.f, -100.f });
		pVecProjectile2.push_back(pBullet2);
		m_pLayer->CreateGameObject(L"Obj_Boss_P1_Bullet2", pVecProjectile2[i]);
	}
	for (int i = 0; i < 5; i++)
	{
		pBullet3 = nullptr;
		pBullet3 = make_shared<CBullet3>(m_pGraphicDev);
		pBullet3->SetPos({ -100.f, -100.f, -100.f });
		pVecProjectile3.push_back(pBullet3);
		m_pLayer->CreateGameObject(L"Obj_Boss_P1_Bullet1", pVecProjectile3[i]);
	}
	for (int i = 0; i < 3; i++)
	{
		pLaser = nullptr;
		pLaser = make_shared<CLaser>(m_pGraphicDev);
		pLaser->SetPos({ -100.f, -200.f, -100.f });
		pVecLaser.push_back(pLaser);
		m_pLayer->CreateGameObject(L"Obj_Boss_P1_Bullet2", pVecLaser[i]);
	}
	dynamic_pointer_cast<CBoss2>(m_pBoss)->SetBullet1(pVecProjectile1);
	dynamic_pointer_cast<CBoss2>(m_pBoss)->SetBullet2(pVecProjectile2);
	dynamic_pointer_cast<CBoss2>(m_pBoss)->SetBullet3(pVecProjectile3);
	dynamic_pointer_cast<CBoss2>(m_pBoss)->SetLaser(pVecLaser);
	dynamic_pointer_cast<CBoss2>(m_pBoss)->SetSpike(pVecSpike);
	dynamic_pointer_cast<CBoss2>(m_pBoss)->SetSunken(pVecSunken);

	// MobBullet
	vector<shared_ptr<CMobBullet>> pVecProjectile_Mob;
	shared_ptr<CMobBullet> pBullet_Mob;
	for (int i = 0; i < 100; i++)
	{
		pMobBullet = nullptr;
		pMobBullet = make_shared<CMobBullet>(m_pGraphicDev);
		pMobBullet->SetPos({ -100.f, -100.f, -100.f });
		pVecProjectile_Mob.push_back(pMobBullet);
		m_pLayer->CreateGameObject(L"Obj_Mob_Bullet1", pVecProjectile_Mob[i]);
	}
	dynamic_pointer_cast<CMob>(m_pMob1)->SetBullet1(pVecProjectile_Mob);
	dynamic_pointer_cast<CMob>(m_pMob2)->SetBullet1(pVecProjectile_Mob);
	dynamic_pointer_cast<CMob>(m_pMob3)->SetBullet1(pVecProjectile_Mob);
	dynamic_pointer_cast<CMob>(m_pMob4)->SetBullet1(pVecProjectile_Mob);
	dynamic_pointer_cast<CMob>(m_pMob5)->SetBullet1(pVecProjectile_Mob);
	dynamic_pointer_cast<CMob>(m_pMob6)->SetBullet1(pVecProjectile_Mob);

	// Mob 넣기
	vector<shared_ptr<CMob>> pVecMob;
	pVecMob.push_back(dynamic_pointer_cast<CMob>(m_pMob4));
	pVecMob.push_back(dynamic_pointer_cast<CMob>(m_pMob2));
	pVecMob.push_back(dynamic_pointer_cast<CMob>(m_pMob3));
	pVecMob.push_back(dynamic_pointer_cast<CMob>(m_pMob6));
	pVecMob.push_back(dynamic_pointer_cast<CMob>(m_pMob1));
	pVecMob.push_back(dynamic_pointer_cast<CMob>(m_pMob5));
	dynamic_pointer_cast<CBoss2>(m_pBoss)->SetMob(pVecMob);

	// 방에 GameObject 넣기
	// Room1
	vector<shared_ptr<CGameObject>> Room1_v1;
	Room1_v1.push_back(m_pBoss);
	Room1_v1.push_back(m_pMob3);
	Room1_v1.push_back(m_pMob1);
	Room1_v1.push_back(m_pMob5);
	Room1_v1.push_back(m_pMob2);
	Room1_v1.push_back(m_pMob6);
	Room1_v1.push_back(m_pMob4);
	
	for (int i = 0; i < 50; i++)
	{
		Room1_v1.push_back(pVecProjectile1[i]);
	}
	for (int i = 0; i < 800; i++)
	{
		Room1_v1.push_back(pVecProjectile2[i]);
	}
	for (int i = 0; i < 5; i++)
	{
		Room1_v1.push_back(pVecProjectile3[i]);
	}
	for (int i = 0; i < 100; i++)
	{
		Room1_v1.push_back(pVecProjectile_Mob[i]);
	}
	for (int i = 0; i < 200; i++)
	{
		Room1_v1.push_back(pVecSpike[i]);
	}
	for (int i = 0; i < 400; i++)
	{
		Room1_v1.push_back(pVecSunken[i]);
	}
	m_pRoom1->PushGameObjectVector(Room1_v1);

	// Room2
	vector<shared_ptr<CGameObject>> Room2_v1;
	Room2_v1.push_back(m_pAlienMob1);
	Room2_v1.push_back(m_pAlienMob2);
	Room2_v1.push_back(m_pAlien);
	Room2_v1.push_back(m_pAlienMob3);

	m_pRoom2->PushGameObjectVector(Room2_v1);

	// creatures
	vector<shared_ptr<CGameObject>> Room2_v4;
	Room2_v4.push_back(m_pAlienMob1);
	Room2_v4.push_back(m_pAlienMob2);
	Room2_v4.push_back(m_pAlien);
	Room2_v4.push_back(m_pAlienMob3);

	for (auto& iter : *dynamic_pointer_cast<CPlayer>(m_pPlayer)->GetHeroVec())
	{
		Room2_v4.push_back(iter);
	}

	// heroes
	vector<shared_ptr<CGameObject>> Room2_v2;
	for (auto& iter : *dynamic_pointer_cast<CPlayer>(m_pPlayer)->GetHeroVec())
	{
		Room2_v2.push_back(iter);
	}
	m_pRoom2->PushHeroesVector(Room2_v2);

	// monsters
	vector<shared_ptr<CGameObject>> Room2_v3;
	Room2_v3.push_back(m_pAlienMob1);
	Room2_v3.push_back(m_pAlienMob2);
	Room2_v3.push_back(m_pAlien);
	Room2_v3.push_back(m_pAlienMob3);
	m_pRoom2->PushMonstersVector(Room2_v3);

	// 배틀시스템 넣기
	shared_ptr<CBattleSystem> pRoom2_Battle = make_shared<CBattleSystem>();
	pRoom2_Battle->SetBattleType(1);
	//pRoom3_Battle->Ready();
	pRoom2_Battle->PushCreaturesVector(Room2_v4);
	pRoom2_Battle->PushHeroesVector(Room2_v2);
	pRoom2_Battle->PushMonstersVector(Room2_v3);
	m_pRoom2->SetBattleSystem(pRoom2_Battle);
	m_pRoom2->SetBattleCameraPos(_vec3(300.f, 3.f, m_fBattlePositionZ));
	pRoom2_Battle->SetGraphicDev(m_pGraphicDev);

	//BattleUI Test
	shared_ptr<CBattleHeroUI> m_pHeroUI = make_shared<CBattleHeroUI>(m_pGraphicDev);
	m_pHeroUI->AwakeGameObject();
	m_pHeroUI->ReadyGameObject();
	CUIMgr::GetInstance()->AddUIObject(L"Battle_Hero_UI", dynamic_pointer_cast<CUIObj>(m_pHeroUI));
	pRoom2_Battle->GetHeroPanelUI(m_pHeroUI);

	// 던전에 방 넣기
	vector<shared_ptr<CDungeonRoom>> Dungeon1_v;
	Dungeon1_v.push_back(m_pRoom1);
	Dungeon1_v.push_back(m_pRoom2);
	Dungeon1_v.push_back(m_pRoom3);
	m_pBossMap->PushDungeonRoomVector(Dungeon1_v);

	// 현재 active 방
	m_pBossMap->DisableAllRoom();
	m_pBossMap->AbleRoom(2);
	dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SetCurrentRoom(2);

	// Layer에 GameObject 넣기
	m_pLayer->CreateGameObject(L"Obj_BriB", m_pAlienMob1);
	m_pLayer->CreateGameObject(L"Obj_BriB", m_pAlienMob2);
	m_pLayer->CreateGameObject(L"Obj_BriB", m_pAlien);
	m_pLayer->CreateGameObject(L"Obj_Alien", m_pAlienMob3);
	

	//m_pLayer->CreateGameObject(L"Obj_Player", m_pPlayer);
	m_pLayer->CreateGameObject(L"Obj_PlayerHand", dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->GetPlayerHand());

	dynamic_pointer_cast<CLayer>(m_pLayer)->AwakeLayer();

	return S_OK;
}

HRESULT CBossMap::Ready_Layer_UI(tstring pLayerTag)
{
	shared_ptr<CLayer> m_pLayer = make_shared<CLayer>();
	m_mapLayer.insert({ pLayerTag, m_pLayer });

	shared_ptr<CGameObject> pDungeonStatusUI = make_shared<CDungeonStatus>(m_pGraphicDev);
	m_pLayer->CreateGameObject(L"Obj_DungeonStatus", pDungeonStatusUI);
	CUIMgr::GetInstance()->AddUIObject(L"UI_DungeonStatus", dynamic_pointer_cast<CUIObj>(pDungeonStatusUI));

	dynamic_pointer_cast<CLayer>(m_pLayer)->AwakeLayer();

	return S_OK;
}
