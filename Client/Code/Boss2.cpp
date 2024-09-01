#include "pch.h"
#include "Boss2.h"
#include "Export_Utility.h"
#include "Export_System.h"

#include "Bullet1.h"
#include "Bullet2.h"
#include "Bullet3.h"
#include "Laser.h"
#include "Mob.h"
#include "Spike.h"
#include "Sunken.h"
#include "Player.h"
#include "Video.h"

#include "CameraMgr.h"

CBoss2::CBoss2(LPDIRECT3DDEVICE9 pGraphicDev)
	:CCreature(pGraphicDev)
{
}

CBoss2::CBoss2(LPDIRECT3DDEVICE9 pGraphicDev, STAT _tCommonStat, _int _iPosition,
	vector<shared_ptr<CSkill>>& _pVecSkill)
	: CCreature(pGraphicDev, _tCommonStat, _iPosition, _pVecSkill)
{
}

CBoss2::CBoss2(LPDIRECT3DDEVICE9 pGraphicDev, STAT _tCommonStat)
	: CCreature(pGraphicDev, _tCommonStat)
{
}

CBoss2::CBoss2(const CCreature& rhs)
	: CCreature(rhs)
{
}

CBoss2::~CBoss2()
{
}

void CBoss2::PickingObj()
{
	m_bPicked = true;
}

HRESULT CBoss2::ReadyGameObject()
{
	__super::ReadyGameObject();

	// 플레이어 받아오기 (거리 계산, 상태 변경)
	m_pPlayer = CGameMgr::GetInstance()->GetPlayer();

	// 컴퍼넌트 받아오기
	m_pPlayerTransformCom = dynamic_pointer_cast<CTransform>(m_pPlayer->GetComponent(L"Com_Transform", ID_DYNAMIC));

	m_pTransformCom->SetAngle(m_vAngle);

	{
		m_eAnimState = EBossState::P1_IDLE;
		m_vAngle = { 0.f, PI / 2, 0.f };

		m_pTransformCom->SetPosition(m_vPos.x, m_vPos.y, m_vPos.z);

		m_pTransformCom->SetAngle(m_vAngle);
		m_pTransformCom->Rotation(ROT_Y, PI / 2.f);

		m_pTextureCom->SetAnimKey(L"Boss_Phase1_Idle", 0.06f);
		m_vOriginSize = m_pTextureCom->GetTextureSize();
	}

	m_pColliderCom->SetPos(m_pTransformCom->GetPos());

	m_bColliding = true;
	m_eCollideID = ECollideID::BOSS;

	return S_OK;
}

_int CBoss2::UpdateGameObject(const _float& fTimeDelta)
{
	_int	iExit = CGameObject::UpdateGameObject(fTimeDelta);

	m_vPos = *m_pTransformCom->GetPos();

	//빌보드 시작
	_matrix matWorld;

	matWorld = *m_pTransformCom->GetWorld();
	SetBillBoard(matWorld);
	m_pTransformCom->SetWorld(&matWorld);

	Engine::AddRenderGroup(RENDER_ALPHA, shared_from_this());

	// 벽 아래로 못 뚫고 내려가게 하기
	if (m_eCurAnimState == EBossState::P1_PATTERN2DASH)
	{
		OnFloor(30.f);
	}


	// 영역 밖으로 벗어나면 가운데로 텔레포트
	if (m_eCurAnimState == EBossState::P1_PATTERN2DASH)
	{
		if (OutArea())
		{
			m_eCurAnimState = EBossState::P1_TELEPORT;
			m_bPattern2Dash = false;
			m_bIdle = true;
			m_fDashTime = 0.12f * 11;
			m_bPattern2 = false;
		}
	}

	// 피격시 0.5초 무적
	if (m_bHitByPlayer)
	{
		m_bSuperArmor = true;
		//m_fHittedIntervel -= fTimeDelta;
		/*if (m_fHittedIntervel < 0.f && !m_bChangeColor)
		{
			m_fHittedIntervel = 0.01f;
			m_iAlpha = 50;
			m_iRed = 0;
			m_iGreen = 0;
			m_iBlue = 0;
			m_bChangeColor = true;
		}
		else if (m_fHittedIntervel < 0.f && m_bChangeColor)
		{
			m_fHittedIntervel = 0.01f;
			m_iAlpha = 255;
			m_iRed = 255;
			m_iGreen = 255;
			m_iBlue = 255;
			m_bChangeColor = false;
		}*/

		m_fHittedTime -= fTimeDelta;
		if (m_fHittedTime < 0.f)
		{
			m_bSuperArmor = false;
			m_fHittedTime = 0.5f;
			m_bHitByPlayer = false;

			/*m_fHittedIntervel = 0.01f;
			m_iAlpha = 255;
			m_iRed = 255;
			m_iGreen = 255;
			m_iBlue = 255;
			m_bChangeColor = false;*/
		}
	}

	// FSM 조건
	FSM(fTimeDelta);

	// 애니메이션 변경
	ChangeAnim();
	// 시간에 따른 애니메이션 끝내기
	AnimDuration(fTimeDelta);

	// 벽에 충돌
	if (m_bWallCollision)
	{
		CCameraMgr::GetInstance()->AddEffectInfo(EEffectState::SHAKING, 0.04f, 0.7f);
		m_bPattern2Dash = false;
		m_bWallCollision = false;
		m_fDashTime = 0.12f * 11;
		m_bPattern2 = false;
		m_bGroggy = true;
		//m_eCurAnimState = EBossState::P1_TELEPORT;
		m_eCurAnimState = EBossState::P1_GROGGY;

		CSoundMgr::GetInstance()->CSoundMgr::GetInstance()->StopSound(CHANNELID::BOSS);
		CSoundMgr::GetInstance()->PlaySound(L"Boss_Groggy.wav", CHANNELID::BOSS, 3.f);
	}

	// 키 입력
	KeyInput();

	return iExit;
}

void CBoss2::LateUpdateGameObject()
{
	__super::LateUpdateGameObject();
}

void CBoss2::RenderGameObject()
{
	// Font
	_tchar		m_szString[128];
	TCHAR szBuff[32] = { };
	_stprintf_s(szBuff, TEXT("%d"),m_iHp);
	lstrcpy(m_szString, szBuff);
	_vec2 vFontPos;
	vFontPos = _vec2(400.f - 100.f, 100.f);
	Engine::Render_Font(L"Font_Default",
		m_szString,
		&vFontPos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

	/*if (m_bHitByPlayer)
	{
		m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->GetWorld());
		m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);

		m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		m_pGraphicDev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

		m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);

		m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(m_iAlpha, m_iRed, m_iGreen, m_iBlue));

		m_pTextureCom->SetAnimTexture();;
		m_pBufCom->RenderBuffer();
		m_pEffectBufCom->RenderBuffer();

		m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, 0xffffffff);

		m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, TRUE);
		m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	}
	else*/
	{
		m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->GetWorld());
		m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

		m_pTextureCom->SetAnimTexture();;
		m_pBufCom->RenderBuffer();
		m_pEffectBufCom->RenderBuffer();

		m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	}

	__super::RenderGameObject();
}

void CBoss2::AddComponent()
{
	shared_ptr<CComponent> pComponent;

	pComponent = m_pTransformCom = make_shared<CTransform>();
	m_pTransformCom->ReadyTransform();
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform",pComponent });

	pComponent = m_pBufCom = make_shared <CRcTex>(m_pGraphicDev);
	m_pBufCom->ReadyBuffer();
	m_mapComponent[ID_STATIC].insert({ L"Com_RCTex",pComponent });

	pComponent = m_pEffectBufCom = make_shared<CRcTex>(m_pGraphicDev);
	m_pEffectBufCom->ReadyBuffer();
	m_mapComponent[ID_STATIC].insert({ L"Com_EffectRCTex",pComponent });

	pComponent = m_pTextureCom = make_shared<CAnimator>(m_pGraphicDev);
	m_pTextureCom->SetAnimKey(m_strAnimKey, 0.1f);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animator", pComponent });

	pComponent = m_pColliderCom = make_shared<CCollider>(m_pGraphicDev);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider",pComponent });
	m_pColliderCom->SetScale({ 30.f, 30.f, 1.f });
	m_pColliderCom->SetPos(m_pTransformCom->GetPos());
}

void CBoss2::OnCollide(shared_ptr<CGameObject> _pObj)
{

	if (ECollideID::WALL == _pObj->GetColType() && m_eCurAnimState == EBossState::P1_PATTERN2DASH)
	{
		if (m_bAbleWallCollsion)
		{
			return;
		}

		CSoundMgr::GetInstance()->CSoundMgr::GetInstance()->StopSound(CHANNELID::BOSS_EFFECT);	
		CSoundMgr::GetInstance()->PlaySound(L"Boss_WallHitted.wav", CHANNELID::BOSS_EFFECT, 1.f);
		
		m_bWallCollision = true;
		m_bAbleWallCollsion = true;
	}

	if ((ECollideID::PLAYER_PROJECTILE == _pObj->GetColType()) && !m_bSuperArmor)
	{
		DecreaseHp(5);
		m_bHitByPlayer = true;
		CSoundMgr::GetInstance()->CSoundMgr::GetInstance()->StopSound(CHANNELID::BOSS_HITTED);
		CSoundMgr::GetInstance()->PlaySound(L"Boss_Hitted.wav", CHANNELID::BOSS_HITTED, 1.f);
	}
}

void CBoss2::FSM(const _float& fTimeDelta)
{
	// Idle상태
	if (m_eCurAnimState == EBossState::P1_IDLE)
	{
		if (m_bIdle)
		{
			m_fIdleTime -= fTimeDelta;
			if (m_iHp <= 100)
			{
				m_fIdleTime = 2.f;
				m_bIdle = false;
				m_bChange = true;
				m_eCurAnimState = EBossState::P1_TELEPORT;
				m_iHp = 100;
			}
			if (m_fIdleTime < 0.f)
			{
				m_fIdleTime = 2.f;
				m_bIdle = false;
			}
		}
		else if (!m_bIdle)
		{
			if (m_iPattern1Count == 0 && !m_bPhase1PatternChoose)
			{
				m_iPattern1Count = m_iPhase1TotalPattern - 1;
				m_bPhase1PatternChoose = true;
				m_iPattern2Count--;
				m_iPattern4Count--;

				m_bPattern1 = true;
				m_eCurAnimState = EBossState::P1_TELEPORT;
			}
			else if (m_iPattern2Count == 0 && !m_bPhase1PatternChoose)
			{
				m_iPattern2Count = m_iPhase1TotalPattern - 1;
				m_bPhase1PatternChoose = true;
				m_iPattern1Count--;
				m_iPattern4Count--;

				m_bPattern2 = true;
				m_eCurAnimState = EBossState::P1_TELEPORT;
			}
			else if (m_iPattern4Count == 0 && !m_bPhase1PatternChoose)
			{
				m_iPattern4Count = m_iPhase1TotalPattern - 1;
				m_bPhase1PatternChoose = true;
				m_iPattern1Count--;
				m_iPattern2Count--;

				m_bPattern4 = true;
				m_eCurAnimState = EBossState::P1_TELEPORT;
			}
			m_bPhase1PatternChoose = false;
		}
	}

	// Teleport상태
	if (m_eCurAnimState == EBossState::P1_TELEPORT)
	{
		if(!m_bTeleportEffect)
		{
			// Effect
			shared_ptr<CEffect> pEffect = CEffectMgr::GetInstance()->GetEffect();

			pEffect->SetAnimEffect(L"Teleport_Effect", m_vPos, _vec3(33.f, 33.f, 33.f), 1.f, false);
			pEffect->SetActive(true);

			m_bTeleportEffect = true;
			m_bWhileTeleport = true;

			m_pTransformCom->SetPosition(300.f, -300.f, 300.f);
		}

		if (m_bWhileTeleport)
		{
			m_fTeleportWhileTime -= fTimeDelta;
			if (m_fTeleportWhileTime < 0.f)
			{
				m_fTeleportWhileTime = 1.2f;
				m_bDoTeleport = true;
				m_bWhileTeleport = false;
			}
		}

		if (m_bDoTeleport)
		{
			if (m_bPattern1)
			{
				m_eCurAnimState = EBossState::P1_PATTERN1;
				Teleport(MiddleTop);
				m_bPattern1Before = true;
			}
			else if (m_bPattern2)
			{
				m_eCurAnimState = EBossState::P1_PATTERN2DASHREADY;
				switch (rand() % 4)
				{
				case 0:
					Teleport(Left);
					break;
				case 1:
					Teleport(Top);
					break;
				case 2:
					Teleport(Bottom);
					break;
				case 3:
					Teleport(Right);
					break;
				}
				m_bPattern2DashReady = true;
			}
			else if (m_bPattern3)
			{
				m_eCurAnimState = EBossState::P1_PATTERN3LASERREADY;
				switch (rand() % 4)
				{
				case 0:
					Teleport(Left);
					break;
				case 1:
					Teleport(Top);
					break;
				case 2:
					Teleport(Bottom);
					break;
				case 3:
					Teleport(Right);
					break;
				}
				m_bPattern3LaserReady = true;
			}
			else if (m_bPattern4)
			{
				m_eCurAnimState = EBossState::P1_PATTERN4SHOTREADY;
				switch (rand() % 4)
				{
				case 0:
					Teleport(Left);
					break;
				case 1:
					Teleport(Top);
					break;
				case 2:
					Teleport(Bottom);
					break;
				case 3:
					Teleport(Right);
					break;
				}
				m_bPattern4ShotReady = true;
			}
			else if (m_bChange)
			{
				m_eCurAnimState = EBossState::P1_CHANGE;
				Teleport(_vec3(300.f, 30.f, 300.f));

				CSoundMgr::GetInstance()->CSoundMgr::GetInstance()->StopSound(CHANNELID::BGM);
				CSoundMgr::GetInstance()->PlayBGM(L"Boss_BGM2.wav", 1.f);

				CSoundMgr::GetInstance()->CSoundMgr::GetInstance()->StopSound(CHANNELID::BOSS);
				CSoundMgr::GetInstance()->PlaySound(L"Boss_Change.wav", CHANNELID::BOSS, 1.5f);
			}
			/*else if (m_bGroggy)
			{
				m_eCurAnimState = EBossState::P1_GROGGY;
				Teleport(_vec3(300.f, 30.f, 300.f));
			}*/
			else
			{
				m_eCurAnimState = EBossState::P1_IDLE;
				switch (rand() % 4)
				{
				case 0:
					Teleport(Left);
					break;
				case 1:
					Teleport(Top);
					break;
				case 2:
					Teleport(Bottom);
					break;
				case 3:
					Teleport(Right);
					break;
				}
				m_bIdle = true;
			}

			m_bDoTeleport = false;
			m_bTeleportEffect = false;
			m_bWhileTeleport = false;
		}
	}

	// Groggy 상태
	if (m_eCurAnimState == EBossState::P1_GROGGY)
	{
		CSoundMgr::GetInstance()->PlaySound(L"Boss_Groggy.wav", CHANNELID::BOSS, 3.f);

		m_fGroggyTime -= fTimeDelta;
		if (m_fGroggyTime < 0.f)
		{
			// 충돌 다시 되게 원래대로
			m_fGroggyTime = 5.f;
			m_bGroggy = false;
			m_bAbleWallCollsion = false;
			m_eCurAnimState = EBossState::P1_TELEPORT;
		}
	}

	// Pattern1 상태
	if (m_eCurAnimState == EBossState::P1_PATTERN1)
	{
		if (m_bPattern1Before)
		{
			m_fPattern1BeforeTime -= fTimeDelta;
			if (m_fPattern1BeforeTime < 0.f)
			{
				m_fPattern1BeforeTime = 1.f;
				m_bPattern1Before = false;
				m_bPattern1Start = true;
			}
		}
		// 구체 4개 쏘기
		if (m_bPattern1Start)
		{
			ShootBullet4();
			m_bPattern1Start = false;
		}

		if (!m_bPattern1Ready)
		{
			m_fPattern1ReadyTime -= fTimeDelta;
			if (m_fPattern1ReadyTime < 0.f)
			{
				m_fPattern1ReadyTime = 4.f;
				m_bPattern1Ready = true;
			}
		}

		// 발사 준비 완료
		if (m_bPattern1Ready)
		{
			m_eCurAnimState = EBossState::P1_PATTERN1FIRE;
			m_bPattern1Ready = false;

			CSoundMgr::GetInstance()->CSoundMgr::GetInstance()->StopSound(CHANNELID::BOSS);
			CSoundMgr::GetInstance()->PlaySound(L"Boss_Attack1.wav", CHANNELID::BOSS, 1.5f);
		}
	}

	// Pattern1 발사 상태
	if (m_eCurAnimState == EBossState::P1_PATTERN1FIRE)
	{
		m_fPattern1FireTime -= fTimeDelta;
		m_fPattern1FireIntervel -= fTimeDelta;
		if (m_fPattern1FireIntervel < 0.f)
		{
			m_fPattern1FireIntervel = 0.15f;
			ShootBullet3();
		}
		// 발사완료
		if (m_fPattern1FireTime < 0.f)
		{
			m_fPattern1FireTime = 3.f;
			m_eCurAnimState = EBossState::P1_TELEPORT;
			m_bPattern1 = false;
		}
	}

	// Pattern2 상태
	if (m_eCurAnimState == EBossState::P1_PATTERN2DASHREADY)
	{
		if (m_bPattern2DashReady)
		{
			m_fDashReadyTime -= fTimeDelta;
			if (m_fDashReadyTime < 0.f)
			{
				m_fDashReadyTime = 0.25f * 4;
				m_bPattern2DashReady = false;
				m_eCurAnimState = EBossState::P1_PATTERN2CHARGE;
				m_bPattern2DashCharge = true;

				CSoundMgr::GetInstance()->CSoundMgr::GetInstance()->StopSound(CHANNELID::BOSS);
				CSoundMgr::GetInstance()->PlaySound(L"Boss_Charge.wav", CHANNELID::BOSS, 1.f);
			}
		}
	}
	if (m_eCurAnimState == EBossState::P1_PATTERN2CHARGE)
	{
		if (m_bPattern2DashCharge)
		{
			//CSoundMgr::GetInstance()->PlaySound(L"Boss_Charge.wav", CHANNELID::BOSS, 0.5f);
			m_fDashChargeTime -= fTimeDelta;
			if (m_fDashChargeTime < 0.f)
			{
				CSoundMgr::GetInstance()->CSoundMgr::GetInstance()->StopSound(CHANNELID::BOSS);
				m_fDashChargeTime = 3.f;
				m_bPattern2DashCharge = false;
				m_eCurAnimState = EBossState::P1_PATTERN2DASH;
				m_bPattern2Dash = true;

				DashDir = CalcDirection();
				D3DXVec3Normalize(&DashDir, &DashDir);

				CSoundMgr::GetInstance()->CSoundMgr::GetInstance()->StopSound(CHANNELID::BOSS);
				CSoundMgr::GetInstance()->PlaySound(L"Boss_Dash.wav", CHANNELID::BOSS, 1.f);
			}
		}
	}
	if (m_eCurAnimState == EBossState::P1_PATTERN2DASH)
	{
		if (m_bPattern2Dash)
		{
			m_fDashTime -= fTimeDelta;

			DashTarget(fTimeDelta, m_fP1_DashSpeed, DashDir);
			//ChasePlayer(fTimeDelta, m_fP1_DashSpeed);
			if (m_fDashTime < 0.f)
			{
				m_fDashTime = 0.12f * 11;
				m_bPattern2Dash = false;
				m_eCurAnimState = EBossState::P1_TELEPORT;
				m_bPattern2 = false;
			}
		}
	}

	// Pattern4 상태
	if (m_eCurAnimState == EBossState::P1_PATTERN4SHOTREADY)
	{
		if (m_bPattern4ShotReady)
		{
			m_fPattern4ShotReadyTime -= fTimeDelta;
			if (m_fPattern4ShotReadyTime < 0.f)
			{
				m_fPattern4ShotReadyTime = 1.f;
				m_bPattern4ShotReady = false;
				m_eCurAnimState = EBossState::P1_PATTERN4SHOT;
				m_bPattern4Shot = true;
			}
		}
	}

	if (m_eCurAnimState == EBossState::P1_PATTERN4SHOT)
	{
		m_fPattern4ShotTime -= fTimeDelta;
		m_fPattern4FireIntervel -= fTimeDelta;
		if (m_fPattern4FireIntervel < 0.f)
		{
			m_fPattern4FireIntervel = 0.1f;
			ShootBullet2();
		}
		// 발사완료
		if (m_fPattern4ShotTime < 0.f)
		{
			m_fPattern4ShotTime = 3.f;
			m_eCurAnimState = EBossState::P1_TELEPORT;
			m_bPattern4 = false;
		}
	}

	// Pattern3 상태
	if (m_eCurAnimState == EBossState::P1_PATTERN3LASERREADY)
	{
		if (m_bPattern3LaserReady)
		{
			m_fPattern3LaserReadyTime -= fTimeDelta;
			if (m_fPattern3LaserReadyTime < 0.f)
			{
				m_fPattern3LaserReadyTime = 1.f;
				m_bPattern3LaserReady = false;
				m_eCurAnimState = EBossState::P1_PATTERN3LASER;
				m_bPattern3Laser = true;
			}
		}
	}

	if (m_eCurAnimState == EBossState::P1_PATTERN3LASER)
	{
		if (m_bPattern3Laser)
		{
			m_fPattern3LaserTime -= fTimeDelta;
			if (m_fPattern3LaserTime < 0.f)
			{
				m_fPattern3LaserTime = 0.12 * 25;
				m_bPattern3Laser = false;
				m_eCurAnimState = EBossState::P1_TELEPORT;
				m_bPattern3 = false;
			}
		}
	}

	// 페이즈2로
	if (m_eCurAnimState == EBossState::P1_CHANGE)
	{
		m_bPhase2 = true;
		if (m_bChange)
		{
			m_bSuperArmor = true;
			m_fP1ChangeTime -= fTimeDelta;
			if (m_fP1ChangeTime < 0.f)
			{
				m_bSuperArmor = false;
				m_fP1ChangeTime = 0.12f * 43;
				m_bChange = false;
				m_eCurAnimState = EBossState::P2_IDLE;
				m_bPhase2Idle = true;
			}
		}
	}

	// 사망
	if (m_eCurAnimState == EBossState::P2_DEATH)
	{
		m_fDeadTime -= fTimeDelta;
		if (m_fDeadTime < 0.f)
		{
			CGameMgr::GetInstance()->SetGameState(EGameState::VIDEO);

			CSoundMgr::GetInstance()->CSoundMgr::GetInstance()->StopAll();
			m_fDeadTime = 0.24f * 26;
			m_eCurAnimState = EBossState::ENUM_END;
			m_pTransformCom->SetPosition(300.f, -300.f, 300.f);
			SetEnable(false);

			m_pVideo = make_shared<CVideo>(m_pGraphicDev);
			m_pVideo->Ready_Video(CVideo::VIDEOID::VIDEO_ENDING);
		}
	}
	
	// 페이즈2 Idle
	if (m_eCurAnimState == EBossState::P2_IDLE)
	{
		// 사망
		if (m_iHp <= 0)
		{
			m_iHp = -100;
			m_bPhase2Idle = false;
			m_fP2IdleTime = 2.f;

			m_eCurAnimState = EBossState::P2_DEATH;
			m_bPhaseDeath = true;

			CSoundMgr::GetInstance()->CSoundMgr::GetInstance()->StopSound(CHANNELID::BOSS);
			CSoundMgr::GetInstance()->PlaySound(L"Boss_Death.wav", CHANNELID::BOSS, 5.f);

			// 웜들 없애기
			for (int i = 0; i < m_iMobTotalNum; i++)
			{
				if (dynamic_pointer_cast<CGameObject>(m_pVecMob[i])->GetIsEnable())
				{
					dynamic_pointer_cast<CGameObject>(m_pVecMob[i])->SetEnable(false);
				}
			}
			
			//CSoundMgr::GetInstance()->PlaySound(L"Boss_Hitted.wav", CHANNELID::BOSS_HITTED, 1.f);
		}
		
		m_fP2IdleTime -= fTimeDelta;
		if (m_fP2IdleTime < 0.f)
		{
			m_fP2IdleTime = 2.f;
			m_bPhase2Idle = false;

			if (m_iPhase2SpikeCount == 0 && !m_bPhase2PatternChoose)
			{
				m_iPhase2SpikeCount = m_iPhase2TotalPattern - 1;
				m_bPhase2PatternChoose = true;
				m_iPhase2SummonCount--;
				m_iPhase2SunkenReadyCount--;
				m_iPhase2SunkenAllCount--;

				m_eCurAnimState = EBossState::P2_SPIKE;
				m_bPhase2Spike = true;
			}
			else if (m_iPhase2SummonCount == 0 && !m_bPhase2PatternChoose)
			{
				m_iPhase2SummonCount = m_iPhase2TotalPattern - 1;
				m_bPhase2PatternChoose = true;
				m_iPhase2SpikeCount--;
				m_iPhase2SunkenReadyCount--;
				m_iPhase2SunkenAllCount--;

				m_eCurAnimState = EBossState::P2_SUMMON;
				m_bPhase2Summon = true;
			}
			else if (m_iPhase2SunkenReadyCount == 0 && !m_bPhase2PatternChoose)
			{
				m_iPhase2SunkenReadyCount = m_iPhase2TotalPattern - 1;
				m_bPhase2PatternChoose = true;
				m_iPhase2SpikeCount--;
				m_iPhase2SummonCount--;
				m_iPhase2SunkenAllCount--;

				m_eCurAnimState = EBossState::P2_SUNKEN;
				m_bPahse2SunkenReady = true;
			}
			else if (m_iPhase2SunkenAllCount == 0 && !m_bPhase2PatternChoose)
			{
				m_iPhase2SunkenAllCount = m_iPhase2TotalPattern - 1;
				m_bPhase2PatternChoose = true;
				m_iPhase2SpikeCount--;
				m_iPhase2SummonCount--;
				m_iPhase2SunkenReadyCount--;

				m_eCurAnimState = EBossState::P2_SUNKENALL;
				m_bPhase2SunkenAll = true;
			}
			m_bPhase2PatternChoose = false;


			//switch (rand() % 4)
			//{
			//case 0:
			//	m_eCurAnimState = EBossState::P2_SPIKE;
			//	m_bPhase2Spike = true;
			//	break;
			//case 1:
			//	m_eCurAnimState = EBossState::P2_SUMMON;
			//	m_bPhase2Summon = true;
			//	break;
			//case 2:
			//	m_eCurAnimState = EBossState::P2_SUNKEN;
			//	m_bPahse2SunkenReady = true;
			//	break;
			//case 3:
			//	m_eCurAnimState = EBossState::P2_SUNKENALL;
			//	m_bPhase2SunkenAll = true;
			//	break;
			//}
		}
	}

	// 페이즈2 Summon
	if (m_eCurAnimState == EBossState::P2_SUMMON)
	{
		m_fP2SummonTime -= fTimeDelta;
		if (m_fP2SummonTime < 0.f)
		{
			// 소환
			for (int i = 0; i < m_iMobTotalNum; i++)
			{
				if (!dynamic_pointer_cast<CGameObject>(m_pVecMob[i])->GetIsEnable())
				{
					dynamic_pointer_cast<CGameObject>(m_pVecMob[i])->SetEnable(true);
					m_iSummonNum--;
					if (m_iSummonNum < 0)
					{
						break;
					}
				}
			}
			m_iSummonNum = 3;
			m_fP2SummonTime = 1.f;
			m_bPhase2Summon = false;
			m_eCurAnimState = EBossState::P2_IDLE;
			m_bPhase2Idle = true;
		}
	}

	// 페이즈2 Spike
	if (m_eCurAnimState == EBossState::P2_SPIKE)
	{
		if (m_bPhase2Spike)
		{
			m_fP2SpikeTime -= fTimeDelta;
			if (m_fP2SpikeTime < 0.f)
			{
				m_fP2SpikeTime = 1.5f;
				CSoundMgr::GetInstance()->CSoundMgr::GetInstance()->StopSound(CHANNELID::SPIKE);
				CSoundMgr::GetInstance()->PlaySound(L"Boss_Death.wav", CHANNELID::SPIKE, 1.f);
				// 스파이크 소환
				for (int i = 0; i < 100; i++)
				{
					if (!dynamic_pointer_cast<CGameObject>(m_pVecSpike[i])->GetIsEnable())
					{
						dynamic_pointer_cast<CGameObject>(m_pVecSpike[i])->SetEnable(true);
						dynamic_pointer_cast<CTransform>(m_pVecSpike[i]->GetComponent(L"Com_Transform", ID_DYNAMIC))->
							SetPosition(m_vPos.x - 330.f + (rand() % 20 + 1) * 30.f, 20.f, m_vPos.z - 330.f + (rand() % 20 + 1) * 30.f);
						m_pVecSpike[i]->SetAnimState(ESpikeState::READY);
					}
				}
				m_bPhase2Spike = false;
				m_bPhase2Spike2 = true;
			}
		}
		if (m_bPhase2Spike2)
		{
			m_fP2SpikeTime2 -= fTimeDelta;
			if (m_fP2SpikeTime2 < 0.f)
			{
				m_fP2SpikeTime2 = 1.f;
				CSoundMgr::GetInstance()->CSoundMgr::GetInstance()->StopSound(CHANNELID::SPIKE);
				CSoundMgr::GetInstance()->PlaySound(L"Boss_Death.wav", CHANNELID::SPIKE, 1.f);
				// 스파이크 소환
				for (int i = 100; i < m_iSpikeTotalNum; i++)
				{
					if (!dynamic_pointer_cast<CGameObject>(m_pVecSpike[i])->GetIsEnable())
					{
						dynamic_pointer_cast<CGameObject>(m_pVecSpike[i])->SetEnable(true);
						dynamic_pointer_cast<CTransform>(m_pVecSpike[i]->GetComponent(L"Com_Transform", ID_DYNAMIC))->
							SetPosition(m_vPos.x - 385.f + (rand() % 20 + 1) * 35.f, 20.f, m_vPos.z - 385.f + (rand() % 20 + 1) * 35.f);
						m_pVecSpike[i]->SetAnimState(ESpikeState::READY);
					}
				}
				m_bPhase2Spike2 = false;
				m_eCurAnimState = EBossState::P2_IDLE;
				m_bPhase2Idle = true;
			}
		}
	}

	// 페이즈2 Sunken
	if (m_eCurAnimState == EBossState::P2_SUNKEN)
	{
		if (m_bPahse2SunkenReady)
		{
			m_fP2SunkenReadyTime -= fTimeDelta;
			if (m_fP2SunkenReadyTime < 0.f)
			{
				m_fP2SunkenReadyTime = 1.f;
				m_bPahse2SunkenReady = false;
				m_bPhase2Sunken = true;
			}
		}

		if (m_bPhase2Sunken)
		{
			m_fP2SunkenIntervel -= fTimeDelta;
			if (m_fP2SunkenIntervel < 0.f)
			{
				CSoundMgr::GetInstance()->CSoundMgr::GetInstance()->StopSound(CHANNELID::SUNKEN);
				CSoundMgr::GetInstance()->PlaySound(L"Boss_Sunken.wav", CHANNELID::SUNKEN, 1.f);
				// 플레이어로 향하는 방향 벡터
				_vec3 PlayerDir;
				PlayerDir = CalcDirection();
				D3DXVec3Normalize(&PlayerDir, &PlayerDir);

				m_fP2SunkenIntervel = 0.1f;
				// 성큰 소환
				for (int i = 0; i < m_iSunkenTotalNum; i++)
				{
					if (!dynamic_pointer_cast<CGameObject>(m_pVecSunken[i])->GetIsEnable())
					{
						dynamic_pointer_cast<CGameObject>(m_pVecSunken[i])->SetEnable(true);
						dynamic_pointer_cast<CTransform>(m_pVecSunken[i]->GetComponent(L"Com_Transform", ID_DYNAMIC))->
							SetPosition(m_vPos.x + PlayerDir.x * m_iSunkentIntervel * (m_iSunkenAttackNum + 1), 12.f, m_vPos.z + PlayerDir.z * m_iSunkentIntervel * (m_iSunkenAttackNum + 1));
						m_pVecSunken[i]->SetAnimState(ESunkenState::ATTACK);
						m_iSunkenAttackNum++;
						break;
					}
				}
			}

			m_fP2SunkenTime -= fTimeDelta;

			if (m_fP2SunkenTime < 0.f)
			{
				m_iSunkenAttackNum = 0;
				m_fP2SunkenIntervel = 0.1f;

				m_fP2SunkenTime = 3.f;

				m_bPhase2Sunken = false;
				m_eCurAnimState = EBossState::P2_IDLE;
				m_bPhase2Idle = true;
			}
		}
	}

	// 페이즈2 SunkenAll
	if (m_eCurAnimState == EBossState::P2_SUNKENALL)
	{
		_vec3 vPos;
		_vec3 vTargetPos;
		_vec3 vDir;
		m_pTransformCom->GetInfo(INFO_POS, &vPos);

		m_fP2SunkenAllIntervel -= fTimeDelta;
		if (m_fP2SunkenAllIntervel < 0.f)
		{
			CSoundMgr::GetInstance()->CSoundMgr::GetInstance()->StopSound(CHANNELID::SUNKEN);
			CSoundMgr::GetInstance()->PlaySound(L"Boss_Sunken.wav", CHANNELID::SUNKEN, 1.f);
			m_fP2SunkenAllIntervel = 0.1f;
			// 성큰 소환
			// 위
			vTargetPos = _vec3(300.f, 5.f, 360.f);
			vDir = vTargetPos - vPos;
			D3DXVec3Normalize(&vDir, &vDir);
			for (int i = 0; i < m_iSunkenTotalNum; i++)
			{
				if (!dynamic_pointer_cast<CGameObject>(m_pVecSunken[i])->GetIsEnable())
				{
					dynamic_pointer_cast<CGameObject>(m_pVecSunken[i])->SetEnable(true);
					dynamic_pointer_cast<CTransform>(m_pVecSunken[i]->GetComponent(L"Com_Transform", ID_DYNAMIC))->
						SetPosition(m_vPos.x + vDir.x * m_iSunkentIntervel * (m_iSunkenAttackAllNum + 1), 12.f, m_vPos.z + vDir.z * m_iSunkentIntervel * (m_iSunkenAttackAllNum + 1));
					m_pVecSunken[i]->SetAnimState(ESunkenState::ATTACK);
					break;
				}
			}
			//아래
			vTargetPos = _vec3(300.f, 5.f, 240.f);
			vDir = vTargetPos - vPos;
			D3DXVec3Normalize(&vDir, &vDir);
			for (int i = 0; i < m_iSunkenTotalNum; i++)
			{
				if (!dynamic_pointer_cast<CGameObject>(m_pVecSunken[i])->GetIsEnable())
				{
					dynamic_pointer_cast<CGameObject>(m_pVecSunken[i])->SetEnable(true);
					dynamic_pointer_cast<CTransform>(m_pVecSunken[i]->GetComponent(L"Com_Transform", ID_DYNAMIC))->
						SetPosition(m_vPos.x + vDir.x * m_iSunkentIntervel * (m_iSunkenAttackAllNum + 1), 12.f, m_vPos.z + vDir.z * m_iSunkentIntervel * (m_iSunkenAttackAllNum + 1));
					m_pVecSunken[i]->SetAnimState(ESunkenState::ATTACK);
					break;
				}
			}
			//우상
			vTargetPos = _vec3(360.f, 5.f, 360.f);
			vDir = vTargetPos - vPos;
			D3DXVec3Normalize(&vDir, &vDir);
			for (int i = 0; i < m_iSunkenTotalNum; i++)
			{
				if (!dynamic_pointer_cast<CGameObject>(m_pVecSunken[i])->GetIsEnable())
				{
					dynamic_pointer_cast<CGameObject>(m_pVecSunken[i])->SetEnable(true);
					dynamic_pointer_cast<CTransform>(m_pVecSunken[i]->GetComponent(L"Com_Transform", ID_DYNAMIC))->
						SetPosition(m_vPos.x + vDir.x * m_iSunkentIntervel * (m_iSunkenAttackAllNum + 1), 12.f, m_vPos.z + vDir.z * m_iSunkentIntervel * (m_iSunkenAttackAllNum + 1));
					m_pVecSunken[i]->SetAnimState(ESunkenState::ATTACK);
					break;
				}
			}
			//좌상
			vTargetPos = _vec3(240.f, 5.f, 360.f);
			vDir = vTargetPos - vPos;
			D3DXVec3Normalize(&vDir, &vDir);
			for (int i = 0; i < m_iSunkenTotalNum; i++)
			{
				if (!dynamic_pointer_cast<CGameObject>(m_pVecSunken[i])->GetIsEnable())
				{
					dynamic_pointer_cast<CGameObject>(m_pVecSunken[i])->SetEnable(true);
					dynamic_pointer_cast<CTransform>(m_pVecSunken[i]->GetComponent(L"Com_Transform", ID_DYNAMIC))->
						SetPosition(m_vPos.x + vDir.x * m_iSunkentIntervel * (m_iSunkenAttackAllNum + 1), 12.f, m_vPos.z + vDir.z * m_iSunkentIntervel * (m_iSunkenAttackAllNum + 1));
					m_pVecSunken[i]->SetAnimState(ESunkenState::ATTACK);
					break;
				}
			}
			//우하
			vTargetPos = _vec3(360.f, 5.f, 240.f);
			vDir = vTargetPos - vPos;
			D3DXVec3Normalize(&vDir, &vDir);
			for (int i = 0; i < m_iSunkenTotalNum; i++)
			{
				if (!dynamic_pointer_cast<CGameObject>(m_pVecSunken[i])->GetIsEnable())
				{
					dynamic_pointer_cast<CGameObject>(m_pVecSunken[i])->SetEnable(true);
					dynamic_pointer_cast<CTransform>(m_pVecSunken[i]->GetComponent(L"Com_Transform", ID_DYNAMIC))->
						SetPosition(m_vPos.x + vDir.x * m_iSunkentIntervel * (m_iSunkenAttackAllNum + 1), 12.f, m_vPos.z + vDir.z * m_iSunkentIntervel * (m_iSunkenAttackAllNum + 1));
					m_pVecSunken[i]->SetAnimState(ESunkenState::ATTACK);
					break;
				}
			}
			//좌하
			vTargetPos = _vec3(240.f, 5.f, 240.f);
			vDir = vTargetPos - vPos;
			D3DXVec3Normalize(&vDir, &vDir);
			for (int i = 0; i < m_iSunkenTotalNum; i++)
			{
				if (!dynamic_pointer_cast<CGameObject>(m_pVecSunken[i])->GetIsEnable())
				{
					dynamic_pointer_cast<CGameObject>(m_pVecSunken[i])->SetEnable(true);
					dynamic_pointer_cast<CTransform>(m_pVecSunken[i]->GetComponent(L"Com_Transform", ID_DYNAMIC))->
						SetPosition(m_vPos.x + vDir.x * m_iSunkentIntervel * (m_iSunkenAttackAllNum + 1), 12.f, m_vPos.z + vDir.z * m_iSunkentIntervel * (m_iSunkenAttackAllNum + 1));
					m_pVecSunken[i]->SetAnimState(ESunkenState::ATTACK);
					break;
				}
			}
			//좌
			vTargetPos = _vec3(240.f, 5.f, 300.f);
			vDir = vTargetPos - vPos;
			D3DXVec3Normalize(&vDir, &vDir);
			for (int i = 0; i < m_iSunkenTotalNum; i++)
			{
				if (!dynamic_pointer_cast<CGameObject>(m_pVecSunken[i])->GetIsEnable())
				{
					dynamic_pointer_cast<CGameObject>(m_pVecSunken[i])->SetEnable(true);
					dynamic_pointer_cast<CTransform>(m_pVecSunken[i]->GetComponent(L"Com_Transform", ID_DYNAMIC))->
						SetPosition(m_vPos.x + vDir.x * m_iSunkentIntervel * (m_iSunkenAttackAllNum + 1), 12.f, m_vPos.z + vDir.z * m_iSunkentIntervel * (m_iSunkenAttackAllNum + 1));
					m_pVecSunken[i]->SetAnimState(ESunkenState::ATTACK);
					break;
				}
			}
			//우
			vTargetPos = _vec3(360.f, 5.f, 300.f);
			vDir = vTargetPos - vPos;
			D3DXVec3Normalize(&vDir, &vDir);
			for (int i = 0; i < m_iSunkenTotalNum; i++)
			{
				if (!dynamic_pointer_cast<CGameObject>(m_pVecSunken[i])->GetIsEnable())
				{
					dynamic_pointer_cast<CGameObject>(m_pVecSunken[i])->SetEnable(true);
					dynamic_pointer_cast<CTransform>(m_pVecSunken[i]->GetComponent(L"Com_Transform", ID_DYNAMIC))->
						SetPosition(m_vPos.x + vDir.x * m_iSunkentIntervel * (m_iSunkenAttackAllNum + 1), 12.f, m_vPos.z + vDir.z * m_iSunkentIntervel * (m_iSunkenAttackAllNum + 1));
					m_pVecSunken[i]->SetAnimState(ESunkenState::ATTACK);
					m_iSunkenAttackAllNum++;
					break;
				}
			}
		}

		m_fP2SunkenAllTime -= fTimeDelta;

		if (m_fP2SunkenAllTime < 0.f)
		{
			m_iSunkenAttackAllNum = 0;
			m_fP2SunkenAllIntervel = 0.1f;

			m_fP2SunkenAllTime = 3.f;

			m_bPhase2SunkenAll = false;
			m_eCurAnimState = EBossState::P2_IDLE;
			m_bPhase2Idle = true;
		}
	}

	// 페이즈2 Bullet(그냥 일정시간마다 계속)
	if (m_bPhase2)
	{
		if (!m_bPhase2FireCool)
		{
			m_fP2FireCoolTime -= fTimeDelta;
		}
		if (m_fP2FireCoolTime < 0.f)
		{
			m_bPhase2FireCool = true;
			// 구체 4개 쏘기
			m_fP2FireCoolTime = 10.f;
			ShootBullet4();
			m_bPhase2FireReady = true;
		}
		if (m_bPhase2FireReady)
		{
			m_fP2FireReadyTime -= fTimeDelta;
			if (m_fP2FireReadyTime < 0.f)
			{
				m_fP2FireReadyTime = 3.f;
				m_bPhase2FireReady = false;
				m_bPhase2Fire = true;
			}
		}
		if (m_bPhase2Fire)
		{
			m_fP2FireShootTime -= fTimeDelta;
			m_fP2FireIntervel -= fTimeDelta;
			if (m_fP2FireIntervel < 0.f)
			{
				m_fP2FireIntervel = 0.15f;
				ShootBullet3();
			}
			// 발사완료
			if (m_fP2FireShootTime < 0.f)
			{
				m_fP2FireShootTime = 3.f;
				m_bPhase2Fire = false;
				m_bPhase2FireCool = false;
			}
		}
	}

	// 원형탄
	if (m_eCurAnimState == EBossState::P1_ATTACK)
	{
		if (!m_bBullet1Fire)
		{
			m_fBullet1Intervel -= fTimeDelta;
			if (m_fBullet1Intervel < 0.f)
			{
				ShootBullet1();
				m_fBullet1Intervel = 0.5f;
				m_bBullet1Fire = true;
			}
		}

		if (!m_bBullet2Fire)
		{
			ShootBullet2();
			m_bBullet2Fire = true;
		}
		if (m_bBullet2Fire)
		{
			m_fBullet2Intervel -= fTimeDelta;
			if (m_fBullet2Intervel < 0.f)
			{
				m_fBullet2Intervel = 0.06f;
				m_bBullet2Fire = false;
			}
		}
	}

	// 레이저 발사
	if (m_eCurAnimState == EBossState::P1_LASER2)
	{
		if (!m_bLaserFire)
		{
			ShootLaser();
			m_bLaserFire = true;
		}
		//if (m_bLaserFire)
		//{
		//	m_fLaserIntervel -= fTimeDelta;
		//	if (m_fLaserIntervel < 0.f)
		//	{
		//		m_fLaserIntervel = 0.5f;
		//		m_bLaserFire = false;
		//	}
		//}
	}

	// 돌진 상태
	if (m_eCurAnimState == EBossState::P1_DASH && m_fP1DashTime < 0.06f * 6)
	{
		ChasePlayer(fTimeDelta, m_fP1_DashSpeed);
	}
}

void CBoss2::ChangeAnim()
{
	_float fXpos, fYpos;
	// Animation
	if (m_eCurAnimState != m_ePrevAnimState)
	{
		switch (m_eCurAnimState)
		{
		case EBossState::P1_IDLE:
			m_pTextureCom->SetAnimKey(L"Boss_Phase1_Idle", 0.06f);
			break;
		case EBossState::P1_TELEPORT:
			m_pTextureCom->SetAnimKey(L"Boss_Phase1_Idle", 0.06f);
			break;
		case EBossState::P1_PATTERN1:
			m_pTextureCom->SetAnimKey(L"Boss_Phase1_Idle", 0.06f);
			break;
		case EBossState::P1_PATTERN1FIRE:
			m_pTextureCom->SetAnimKey(L"Boss_Phase1_Attack", 0.06f);
			break;
		case EBossState::P1_PATTERN2DASHREADY:
			m_pTextureCom->SetAnimKey(L"Boss_Phase1_Dash_Ready", 0.25f);
			break;
		case EBossState::P1_PATTERN2CHARGE:
			m_pTextureCom->SetAnimKey(L"Boss_Phase1_Dash_Charge", 0.06f);
			break;
		case EBossState::P1_PATTERN2DASH:
			m_pTextureCom->SetAnimKey(L"Boss_Phase1_Dash", 0.06f);
			break;
		case EBossState::P1_PATTERN3LASERREADY:
			m_pTextureCom->SetAnimKey(L"Boss_Phase1_Idle", 0.06f);
			break;
		case EBossState::P1_PATTERN3LASER:
			m_pTextureCom->SetAnimKey(L"Boss_Phase1_Laser1", 0.12f);
			break;
		case EBossState::P1_PATTERN4SHOTREADY:
			m_pTextureCom->SetAnimKey(L"Boss_Phase1_Idle", 0.12f);
			break;
		case EBossState::P1_PATTERN4SHOT:
			m_pTextureCom->SetAnimKey(L"Boss_Phase1_Attack", 0.06f);
			break;
		case EBossState::P1_ATTACK:
			m_pTextureCom->SetAnimKey(L"Boss_Phase1_Attack", 0.06f);
			break;
		case EBossState::P1_LASER1:
			m_pTextureCom->SetAnimKey(L"Boss_Phase1_Laser1", 0.06f);
			break;
		case EBossState::P1_LASER2:
			m_pTextureCom->SetAnimKey(L"Boss_Phase1_Laser2", 0.06f);
			break;
		case EBossState::P1_DASH:
			m_pTextureCom->SetAnimKey(L"Boss_Phase1_Dash", 0.12f);
			break;
		case EBossState::P1_CHANGE:
			m_pTextureCom->SetAnimKey(L"Boss_Phase1_Change", 0.12f);
			break;
		case EBossState::P1_GROGGY:
			m_pTextureCom->SetAnimKey(L"Boss_Phase1_Groggy", 0.12f);
			break;
		case EBossState::P2_IDLE:
			m_pTextureCom->SetAnimKey(L"Boss_Phase2_Idle", 0.06f);
			break;
		case EBossState::P2_DEATH:
			m_pTextureCom->SetAnimKey(L"Boss_Phase2_Death", 0.24f);
			break;
		}
		m_ePrevAnimState = m_eCurAnimState;

	}

	_vec2 vcurPos = m_pTextureCom->GetTextureSize();

	if (m_eCurAnimState == EBossState::P1_IDLE) {
		fXpos = (vcurPos.x / m_vOriginSize.x);
		fYpos = (vcurPos.y / m_vOriginSize.y);
	}
	else {
		fXpos = (vcurPos.x / m_vOriginSize.x);
		fYpos = (vcurPos.y / m_vOriginSize.y);
	}

	m_pTransformCom->SetScale(40.f * 0.9f * fXpos, 35.f * 0.9f * fYpos, 40.f * 0.9f * fXpos);
	m_pColliderCom->SetScale({ 80.f * 0.9f * fXpos, 70.f * 0.9f * fYpos, 80.f * 0.9f * fXpos });
	m_pColliderCom->SetRadius({ 40.f * 0.9f * fXpos});

}

void CBoss2::AnimDuration(const _float& fTimeDelta)
{
	if (m_eCurAnimState == EBossState::P1_ATTACK)
	{
		m_fP1AttackTime -= fTimeDelta;
		if (m_fP1AttackTime < 0.f)
		{
			m_fP1AttackTime = 0.06f * 12;
			m_eCurAnimState = EBossState::P1_IDLE;
			m_bBullet1Fire = false;
			m_bBullet2Fire = false;
		}
	}
	if (m_eCurAnimState == EBossState::P1_LASER1)
	{
		m_fP1Laser1Time -= fTimeDelta;
		if (m_fP1Laser1Time < 0.f)
		{
			m_fP1Laser1Time = 0.06f * 7;
			m_eCurAnimState = EBossState::P1_LASER2;
		}
	}
	if (m_eCurAnimState == EBossState::P1_LASER2)
	{
		m_fP1Laser2Time -= fTimeDelta;
		if (m_fP1Laser2Time < 0.f)
		{
			m_fP1Laser2Time = 0.06f * 4;
			m_eCurAnimState = EBossState::P1_IDLE;
			m_bLaserFire = false;
		}
	}
	if (m_eCurAnimState == EBossState::P1_DASH)
	{
		m_fP1DashTime -= fTimeDelta;
		if (m_fP1DashTime < 0.f)
		{
			m_fP1DashTime = 0.06f * 11;
			m_eCurAnimState = EBossState::P1_IDLE;
		}
	}
}

void CBoss2::KeyInput()
{
	//if (GetAsyncKeyState('2') & 0x8000) {
	//	m_eCurAnimState = EBossState::P1_ATTACK;
	//}
	/*if (GetAsyncKeyState('3') & 0x8000) {
		m_eCurAnimState = EBossState::P1_LASER1;
	}*/
	if (GetAsyncKeyState('4') & 0x8000) {
		m_iHp = 100;
	}

	/*if (GetAsyncKeyState('5') & 0x8000) {
		for (int i = 0; i < m_iMobTotalNum; i++)
		{
			if (!dynamic_pointer_cast<CGameObject>(m_pVecMob[i])->GetIsEnable())
			{
				dynamic_pointer_cast<CGameObject>(m_pVecMob[i])->SetEnable(true);
				break;
			}
		}
	}*/
}

void CBoss2::OnFloor(_float _fHeight)
{
	if (m_pTransformCom->GetPos()->y < _fHeight)
	{
		m_pTransformCom->SetPosition(m_vPos.x, _fHeight, m_vPos.z);
	}
}

void CBoss2::Teleport(_vec3 vPos)
{
	CSoundMgr::GetInstance()->CSoundMgr::GetInstance()->StopSound(CHANNELID::BOSS_EFFECT);
	CSoundMgr::GetInstance()->PlaySound(L"Boss_Teleport.wav", CHANNELID::BOSS_EFFECT, 1.f);
	

	PrevTeleportPos = CurrentTeleportPos;
	CurrentTeleportPos = vPos;
	if (PrevTeleportPos != CurrentTeleportPos)
	{
	}

	m_pTransformCom->SetPosition(vPos.x, vPos.y, vPos.z);
	m_vPos = vPos;

	if (PrevTeleportPos != CurrentTeleportPos)
	{
		shared_ptr<CEffect> pEffect = CEffectMgr::GetInstance()->GetEffect();

		pEffect->SetAnimEffect(L"Teleport_Effect", m_vPos, _vec3(33.f, 33.f, 33.f), 1.f, false);
		pEffect->SetActive(true);
	}
}

_bool CBoss2::OutArea()
{
	if (m_pTransformCom->GetPos()->x > 640.f)
	{
		return true;
	}
	if (m_pTransformCom->GetPos()->x < 0.f)
	{
		return true;
	}
	if (m_pTransformCom->GetPos()->z > 640.f)
	{
		return true;
	}
	if (m_pTransformCom->GetPos()->z < 0.f)
	{
		return true;
	}
	if (m_pTransformCom->GetPos()->y < 0.f)
	{
		return true;
	}
	if (m_pTransformCom->GetPos()->y > 640.f)
	{
		return true;
	}

	return false;
}

_float CBoss2::CalcDistance()
{
	_vec3 vPos;
	m_pTransformCom->GetInfo(INFO_POS, &vPos);

	_vec3 vPosPlayer;
	m_pPlayerTransformCom->GetInfo(INFO_POS, &vPosPlayer);

	_vec3 vDist = vPosPlayer - vPos;

	return D3DXVec3Length(&vDist);
}

_vec3 CBoss2::CalcDirection()
{
	_vec3 vPos;
	m_pTransformCom->GetInfo(INFO_POS, &vPos);

	_vec3 vPosPlayer;
	m_pPlayerTransformCom->GetInfo(INFO_POS, &vPosPlayer);

	_vec3 vDist = vPosPlayer - vPos;

	return vDist;
}

void CBoss2::ChasePlayer(const _float& fTimeDelta, float _fSpeed)
{
	_vec3		vDir;
	vDir = CalcDirection();
	D3DXVec3Normalize(&vDir, &vDir);
	//vDir.y = 0.f;
	m_pTransformCom->MoveForward(&vDir, fTimeDelta, _fSpeed);
}

void CBoss2::DashTarget(const _float& fTimeDelta, float _fSpeed, _vec3& _vDir)
{
	D3DXVec3Normalize(&_vDir, &_vDir);
	
	m_pTransformCom->MoveForward(&_vDir, fTimeDelta, _fSpeed);
}

void CBoss2::ShootBullet1()
{
	for (int i = 0; i < m_iBullet1TotalNum; i++)
	{
		if (!dynamic_pointer_cast<CGameObject>(m_pVecBullet1[i])->GetIsEnable())
		{
			dynamic_pointer_cast<CGameObject>(m_pVecBullet1[i])->SetEnable(true);
			shared_ptr<CTransform> pTransform = dynamic_pointer_cast<CTransform>(
				m_pVecBullet1[i]->GetComponent(L"Com_Transform", ID_DYNAMIC));
			pTransform->SetPosition(m_vPos.x, m_vPos.y, m_vPos.z);
			break;
		}
	}
}

void CBoss2::ShootBullet2()
{
	for (int i = 0; i < m_iBullet2TotalNum; i++)
	{
		if (!dynamic_pointer_cast<CGameObject>(m_pVecBullet1[i])->GetIsEnable())
		{
			CSoundMgr::GetInstance()->CSoundMgr::GetInstance()->StopSound(CHANNELID::BOSS_PROJ1);
			CSoundMgr::GetInstance()->PlaySound(L"BossProjectile1.wav", CHANNELID::BOSS_PROJ1, 0.5f);
			dynamic_pointer_cast<CGameObject>(m_pVecBullet1[i])->SetEnable(true);
			m_pVecBullet1[i]->SetOnCollision(false);
			shared_ptr<CTransform> pTransform = dynamic_pointer_cast<CTransform>(
				m_pVecBullet1[i]->GetComponent(L"Com_Transform", ID_DYNAMIC));
			pTransform->SetPosition(m_vPos.x, m_vPos.y, m_vPos.z);
			_vec3 vPosPlayer;
			m_pPlayerTransformCom->GetInfo(INFO_POS, &vPosPlayer);
			vPosPlayer = _vec3(vPosPlayer.x - 10.f + (rand() % 10) * 2.f, vPosPlayer.y - 10.f + (rand() % 10) * 2.f, vPosPlayer.z);
			m_pVecBullet1[i]->SetTarget(vPosPlayer);
			_vec3 vDir = vPosPlayer - m_vPos;
			m_pVecBullet1[i]->SetTargetDir(vDir);
			break;
		}
	}
}

void CBoss2::ShootBullet3()
{
	for (int i = 0; i < 50; i++)
	{
		if (!dynamic_pointer_cast<CGameObject>(m_pVecBullet2[i])->GetIsEnable())
		{
			CSoundMgr::GetInstance()->CSoundMgr::GetInstance()->StopSound(CHANNELID::BOSS_PROJ2);
			CSoundMgr::GetInstance()->PlaySound(L"BossProjectile2.wav", CHANNELID::BOSS_PROJ2, 0.3f);
			dynamic_pointer_cast<CGameObject>(m_pVecBullet2[i])->SetEnable(true);
			m_pVecBullet2[i]->SetOnCollision(false);
			shared_ptr<CTransform> pTransform = dynamic_pointer_cast<CTransform>(
				m_pVecBullet2[i]->GetComponent(L"Com_Transform", ID_DYNAMIC));
			pTransform->SetPosition(m_vPos.x, m_vPos.y, m_vPos.z);
			_vec3 vTarget;
			if (m_bPhase2)
			{
				vTarget = m_vPos + _vec3((-10.f + (rand() % 4 + 1) * 4.f) * 1280.f, -1.f * 1280.f, (-10.f + (rand() % 4 + 1) * 4.f) * 1280.f);
			}
			else
			{
				vTarget = m_vPos + _vec3((-6.f + (rand() % 2 + 1) * 4.f) * 1280.f, -5.f * 1280.f, (-6.f + (rand() % 2 + 1) * 4.f) * 1280.f);
			}
			m_pVecBullet2[i]->SetTarget(vTarget);
			break;
		}
	}
	for (int i = 50; i < 100; i++)
	{
		if (!dynamic_pointer_cast<CGameObject>(m_pVecBullet2[i])->GetIsEnable())
		{
			dynamic_pointer_cast<CGameObject>(m_pVecBullet2[i])->SetEnable(true);
			m_pVecBullet2[i]->SetOnCollision(false);
			shared_ptr<CTransform> pTransform = dynamic_pointer_cast<CTransform>(
				m_pVecBullet2[i]->GetComponent(L"Com_Transform", ID_DYNAMIC));
			pTransform->SetPosition(Right.x, Right.y, Right.z);
			_vec3 vTarget;
			vTarget = Right + _vec3((-6.f + (rand() % 2 + 1) * 4.f) * 1280.f, -5.f * 1280.f, (-6.f + (rand() % 2 + 1) * 4.f) * 1280.f);
			m_pVecBullet2[i]->SetTarget(vTarget);
			break;
		}
	}
	for (int i = 100; i < 150; i++)
	{
		if (!dynamic_pointer_cast<CGameObject>(m_pVecBullet2[i])->GetIsEnable())
		{
			dynamic_pointer_cast<CGameObject>(m_pVecBullet2[i])->SetEnable(true);
			m_pVecBullet2[i]->SetOnCollision(false);
			shared_ptr<CTransform> pTransform = dynamic_pointer_cast<CTransform>(
				m_pVecBullet2[i]->GetComponent(L"Com_Transform", ID_DYNAMIC));
			pTransform->SetPosition(Left.x, Left.y, Left.z);
			_vec3 vTarget;
			vTarget = Left + _vec3((-6.f + (rand() % 2 + 1) * 4.f) * 1280.f, -5.f * 1280.f, (-6.f + (rand() % 2 + 1) * 4.f) * 1280.f);
			m_pVecBullet2[i]->SetTarget(vTarget);
			break;
		}
	}
	for (int i = 150; i < 200; i++)
	{
		if (!dynamic_pointer_cast<CGameObject>(m_pVecBullet2[i])->GetIsEnable())
		{
			dynamic_pointer_cast<CGameObject>(m_pVecBullet2[i])->SetEnable(true);
			m_pVecBullet2[i]->SetOnCollision(false);
			shared_ptr<CTransform> pTransform = dynamic_pointer_cast<CTransform>(
				m_pVecBullet2[i]->GetComponent(L"Com_Transform", ID_DYNAMIC));
			pTransform->SetPosition(Top.x, Top.y, Top.z);
			_vec3 vTarget;
			vTarget = Top + _vec3((-6.f + (rand() % 2 + 1) * 4.f) * 1280.f, -5.f * 1280.f, (-6.f + (rand() % 2 + 1) * 4.f) * 1280.f);
			m_pVecBullet2[i]->SetTarget(vTarget);
			break;
		}
	}
	for (int i = 200; i < 250; i++)
	{
		if (!dynamic_pointer_cast<CGameObject>(m_pVecBullet2[i])->GetIsEnable())
		{
			dynamic_pointer_cast<CGameObject>(m_pVecBullet2[i])->SetEnable(true);
			m_pVecBullet2[i]->SetOnCollision(false);
			shared_ptr<CTransform> pTransform = dynamic_pointer_cast<CTransform>(
				m_pVecBullet2[i]->GetComponent(L"Com_Transform", ID_DYNAMIC));
			pTransform->SetPosition(Bottom.x, Bottom.y, Bottom.z);
			_vec3 vTarget;
			vTarget = Bottom + _vec3((-6.f + (rand() % 2 + 1) * 4.f) * 1280.f, -5.f * 1280.f, (-6.f + (rand() % 2 + 1) * 4.f) * 1280.f);
			m_pVecBullet2[i]->SetTarget(vTarget);
			break;
		}
	}
}

void CBoss2::ShootBullet4()
{
	for (int i = 0; i < 4; i++)
	{
		if (!dynamic_pointer_cast<CGameObject>(m_pVecBullet3[i])->GetIsEnable())
		{
			CSoundMgr::GetInstance()->CSoundMgr::GetInstance()->StopSound(CHANNELID::BOSS_PROJ3);
			CSoundMgr::GetInstance()->PlaySound(L"BossProjectile3.wav", CHANNELID::BOSS_PROJ3, 0.4f);
			dynamic_pointer_cast<CGameObject>(m_pVecBullet3[i])->SetEnable(true);
			shared_ptr<CTransform> pTransform = dynamic_pointer_cast<CTransform>(
				m_pVecBullet3[i]->GetComponent(L"Com_Transform", ID_DYNAMIC));
			pTransform->SetPosition(m_vPos.x, m_vPos.y, m_vPos.z);
			_vec3 vTarget;
			switch (i)
			{
			case 0:
				vTarget = Right;
				break;
			case 1:
				vTarget = Left;
				break;
			case 2:
				vTarget = Top;
				break;
			case 3:
				vTarget = Bottom;
				break;
			}
			m_pVecBullet3[i]->SetTarget(vTarget);
		}
	}
}

void CBoss2::ShootBullet5()
{
	for (int i = 0; i < 5; i++)
	{
		if (!dynamic_pointer_cast<CGameObject>(m_pVecBullet3[i])->GetIsEnable())
		{
			CSoundMgr::GetInstance()->CSoundMgr::GetInstance()->StopSound(CHANNELID::BOSS_PROJ3);
			CSoundMgr::GetInstance()->PlaySound(L"BossProjectile3.wav", CHANNELID::BOSS_PROJ3, 0.4f);
			dynamic_pointer_cast<CGameObject>(m_pVecBullet3[i])->SetEnable(true);
			shared_ptr<CTransform> pTransform = dynamic_pointer_cast<CTransform>(
				m_pVecBullet3[i]->GetComponent(L"Com_Transform", ID_DYNAMIC));
			pTransform->SetPosition(m_vPos.x, m_vPos.y, m_vPos.z);
			_vec3 vTarget;
			switch (i)
			{
			case 0:
				vTarget = Right;
				break;
			case 1:
				vTarget = Left;
				break;
			case 2:
				vTarget = Top;
				break;
			case 3:
				vTarget = Bottom;
				break;
			case 4:
				vTarget = MiddleTop;
				break;
			}
			m_pVecBullet3[i]->SetTarget(vTarget);
		}
	}
}

void CBoss2::ShootLaser()
{
	for (int i = 0; i < m_iLaserTotalNum; i++)
	{
		if (!dynamic_pointer_cast<CGameObject>(m_pVecLaser[i])->GetIsEnable())
		{
			dynamic_pointer_cast<CGameObject>(m_pVecLaser[i])->SetEnable(true);
			shared_ptr<CTransform> pTransform = dynamic_pointer_cast<CTransform>(
				m_pVecLaser[i]->GetComponent(L"Com_Transform", ID_DYNAMIC));
			_vec3 vPosition = m_vPos;
			_vec3 vDirX;
			_vec3 vDirY;
			m_pTransformCom->GetInfo(INFO_RIGHT, &vDirX);
			m_pTransformCom->GetInfo(INFO_UP, &vDirY);
			D3DXVec3Normalize(&vDirX, &vDirX);
			D3DXVec3Normalize(&vDirY, &vDirY);
			switch (i)
			{
				// 윗 눈
			case 0:
				vPosition += vDirY * 14.f;
				break;
				// 아래 왼 눈
			case 1:
				vPosition -= vDirX * 16.f;
				break;
				// 아래 오른 눈
			case 2:
				vPosition += vDirX * 16.f;
				break;
			}
			m_pVecLaser[i]->SetStart(vPosition);

			_vec3 vPosPlayer;
			m_pPlayerTransformCom->GetInfo(INFO_POS, &vPosPlayer);
			vPosPlayer = _vec3(vPosPlayer.x, vPosPlayer.y, vPosPlayer.z);
			m_pVecLaser[i]->SetTarget(vPosPlayer);
		}
	}
}
