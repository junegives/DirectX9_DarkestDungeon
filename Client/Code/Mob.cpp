#include "pch.h"
#include "Mob.h"
#include "Export_Utility.h"
#include "Export_System.h"

#include "Bullet1.h"
#include "Bullet2.h"
#include "Mob_Bullet.h"
#include "Laser.h"
#include "Player.h"

CMob::CMob(LPDIRECT3DDEVICE9 pGraphicDev)
	:CCreature(pGraphicDev)
{
}

CMob::CMob(LPDIRECT3DDEVICE9 pGraphicDev, STAT _tCommonStat, _int _iPosition,
	vector<shared_ptr<CSkill>>& _pVecSkill)
	: CCreature(pGraphicDev, _tCommonStat, _iPosition, _pVecSkill)
{
}

CMob::CMob(LPDIRECT3DDEVICE9 pGraphicDev, STAT _tCommonStat)
	: CCreature(pGraphicDev, _tCommonStat)
{
}

CMob::CMob(const CCreature& rhs)
	: CCreature(rhs)
{
}

CMob::~CMob()
{
}

void CMob::PickingObj()
{
	m_bPicked = true;
}

HRESULT CMob::ReadyGameObject()
{
	__super::ReadyGameObject();

	// 플레이어 받아오기 (거리 계산, 상태 변경)
	m_pPlayer = CGameMgr::GetInstance()->GetPlayer();

	// 컴퍼넌트 받아오기
	m_pPlayerTransformCom = dynamic_pointer_cast<CTransform>(m_pPlayer->GetComponent(L"Com_Transform", ID_DYNAMIC));

	m_pTransformCom->SetAngle(m_vAngle);

	{
		m_eAnimState = EMobState::GO_OUT;
		m_vAngle = { 0.f, PI / 2, 0.f };

		m_pTransformCom->SetPosition(m_vPos.x, m_vPos.y, m_vPos.z);

		m_pTransformCom->SetAngle(m_vAngle);
		m_pTransformCom->Rotation(ROT_Y, PI / 2.f);

		m_pTextureCom->SetAnimKey(L"Mob_Out", 0.12f);
		m_vOriginSize = m_pTextureCom->GetTextureSize();
	}

	m_pColliderCom->SetPos(m_pTransformCom->GetPos());

	m_bColliding = true;
	m_eCollideID = ECollideID::MOB;

	SetEnable(false);

	return S_OK;
}

_int CMob::UpdateGameObject(const _float& fTimeDelta)
{
	// 한번 막기
	if (m_bEnable && !m_bMakeEnable) {
		m_bMakeEnable = true;
		m_bEnable = false;
	}

	_int	iExit = CGameObject::UpdateGameObject(fTimeDelta);

	m_vPos = *m_pTransformCom->GetPos();

	//빌보드 시작
	_matrix matWorld;

	matWorld = *m_pTransformCom->GetWorld();
	SetBillBoard(matWorld);
	m_pTransformCom->SetWorld(&matWorld);

	Engine::AddRenderGroup(RENDER_ALPHA, shared_from_this());

	// FSM 조건
	FSM(fTimeDelta);

	// 애니메이션 변경
	ChangeAnim();
	// 시간에 따른 애니메이션 끝내기
	AnimDuration(fTimeDelta);

	// 키 입력
	//KeyInput();

	return iExit;
}

void CMob::LateUpdateGameObject()
{
	__super::LateUpdateGameObject();
}

void CMob::RenderGameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->GetWorld());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTextureCom->SetAnimTexture();;
	m_pBufCom->RenderBuffer();
	m_pEffectBufCom->RenderBuffer();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	__super::RenderGameObject();
}

void CMob::AddComponent()
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

void CMob::OnCollide(shared_ptr<CGameObject> _pObj)
{
	if (ECollideID::PLAYER_PROJECTILE == _pObj->GetColType())
	{
		DecreaseHp(5);
		CSoundMgr::GetInstance()->CSoundMgr::GetInstance()->StopSound(CHANNELID::MOB);
		CSoundMgr::GetInstance()->PlaySound(L"Mob_Hitted.wav", CHANNELID::MOB, 1.5f);
	}
}

void CMob::FSM(const _float& fTimeDelta)
{
	// Out
	if (m_eCurAnimState == EMobState::GO_OUT)
	{
		m_fOutTime -= fTimeDelta;
		if (m_fOutTime < 0.f)
		{
			m_fOutTime = 0.12f * 7;
			m_eCurAnimState = EMobState::IDLE;
			m_bIdle = true;
			m_bOut = false;
		}
	}

	// Idle
	if (m_eCurAnimState == EMobState::IDLE)
	{
		m_fIdleTime -= fTimeDelta;

		// 사망
		if (m_iHp <= 0)
		{
			m_fIdleTime = 1.f;
			m_bIdle = false;
			m_eCurAnimState = EMobState::DEATH;
			m_bDeath = true;

			CSoundMgr::GetInstance()->CSoundMgr::GetInstance()->StopSound(CHANNELID::MOB);
			CSoundMgr::GetInstance()->PlaySound(L"Mob_Death.wav", CHANNELID::MOB, 1.5f);
		}
		if (m_fIdleTime < 0.f)
		{
			m_fIdleTime = 1.f;
			// 탄환 발사
			if (m_iAttackCount > 0)
			{
				m_eCurAnimState = EMobState::ATTACK;
				m_bAttack = true;
				m_bIdle = false;

				CSoundMgr::GetInstance()->CSoundMgr::GetInstance()->StopSound(CHANNELID::MOB);
				CSoundMgr::GetInstance()->PlaySound(L"Mob_Attack.wav", CHANNELID::MOB, 1.5f);
			}
			// 들어가기
			else
			{
				m_eCurAnimState = EMobState::GO_IN;
				m_bIn = true;
				m_bIdle = false;
			}
		}
	}

	// Attack
	if (m_eCurAnimState == EMobState::ATTACK)
	{
		m_fAttackTime -= fTimeDelta;
		if (m_fAttackTime < 0.12f * 3 && m_bAttack)
		{
			ShootBullet1();
			m_iAttackCount -= 1;
			m_bAttack = false;
		}

		if (m_fAttackTime < 0.f)
		{
			m_fAttackTime = 0.12f * 11;
			m_eCurAnimState = EMobState::IDLE;
			m_bIdle = true;
		}
	}

	// In
	if (m_eCurAnimState == EMobState::GO_IN)
	{
		m_fInTime -= fTimeDelta;
		if (m_fInTime < 0.f)
		{
			m_fInTime = 0.12f * 9;
			m_eCurAnimState = EMobState::GO_OUT;
			m_bOut = true;
			m_bIn = false;

			m_iAttackCount = 3;
			// 비활성화
			SetEnable(false);
		}
	}
	
	// 사망
	if (m_eCurAnimState == EMobState::DEATH)
	{
		m_fDeathTime -= fTimeDelta;

		// 사망
		if (m_fDeathTime < 0)
		{
			m_fDeathTime = 0.12f * 11;
			m_bDeath = false;
			m_eCurAnimState = EMobState::GO_OUT;
			m_iHp = 30;

			// 비활성화
			SetEnable(false);
		}
	}
}

void CMob::ChangeAnim()
{
	_float fXpos, fYpos;
	// Animation
	if (m_eCurAnimState != m_ePrevAnimState)
	{
		switch (m_eCurAnimState)
		{
		case EMobState::IDLE:
			m_pTextureCom->SetAnimKey(L"Mob_Idle", 0.06f);
			break;
		case EMobState::ATTACK:
			m_pTextureCom->SetAnimKey(L"Mob_Shoot", 0.12f);
			break;
		case EMobState::GO_IN:
			m_pTextureCom->SetAnimKey(L"Mob_In", 0.12f);
			break;
		case EMobState::GO_OUT:
			m_pTextureCom->SetAnimKey(L"Mob_Out", 0.12f);
			break;
		case EMobState::DEATH:
			m_pTextureCom->SetAnimKey(L"Mob_Death", 0.12f);
			break;
		}
		m_ePrevAnimState = m_eCurAnimState;

	}

	_vec2 vcurPos = m_pTextureCom->GetTextureSize();

	if (m_eCurAnimState == EMobState::IDLE) {
		fXpos = (vcurPos.x / m_vOriginSize.x);
		fYpos = (vcurPos.y / m_vOriginSize.y);
	}
	else {
		fXpos = (vcurPos.x / m_vOriginSize.x);
		fYpos = (vcurPos.y / m_vOriginSize.y);
	}
	
	m_pTransformCom->SetScale(8.f * 0.8f * fXpos, 8.f * 0.8f * fYpos, 8.f * 0.8f * fXpos);
	m_pColliderCom->SetScale({ 16.f * 0.8f * fXpos, 16.f * 0.8f * fYpos, 16.f * 0.8f * fXpos });
	m_pColliderCom->SetRadius(8.f * 0.8f * fXpos);
}

void CMob::AnimDuration(const _float& fTimeDelta)
{
}

void CMob::KeyInput()
{
	if (GetAsyncKeyState('1') & 0x8000) {
		m_eCurAnimState = EMobState::IDLE;
	}
	if (GetAsyncKeyState('2') & 0x8000) {
		m_eCurAnimState = EMobState::ATTACK;
	}
	if (GetAsyncKeyState('3') & 0x8000) {
		m_eCurAnimState = EMobState::GO_IN;
	}
	if (GetAsyncKeyState('4') & 0x8000) {
		m_eCurAnimState = EMobState::GO_OUT;
	}
	if (GetAsyncKeyState('5') & 0x8000) {
		m_eCurAnimState = EMobState::DEATH;
	}
}

_float CMob::CalcDistance()
{
	_vec3 vPos;
	m_pTransformCom->GetInfo(INFO_POS, &vPos);

	_vec3 vPosPlayer;
	m_pPlayerTransformCom->GetInfo(INFO_POS, &vPosPlayer);

	_vec3 vDist = vPosPlayer - vPos;

	return D3DXVec3Length(&vDist);
}

_vec3 CMob::CalcDirection()
{
	_vec3 vPos;
	m_pTransformCom->GetInfo(INFO_POS, &vPos);

	_vec3 vPosPlayer;
	m_pPlayerTransformCom->GetInfo(INFO_POS, &vPosPlayer);

	_vec3 vDist = vPosPlayer - vPos;

	return vDist;
}

void CMob::ChasePlayer(const _float& fTimeDelta, float _fSpeed)
{
	_vec3		vDir;
	vDir = CalcDirection();
	D3DXVec3Normalize(&vDir, &vDir);
	vDir.y = 0.f;
	m_pTransformCom->MoveForward(&vDir, fTimeDelta, _fSpeed);
}

void CMob::ShootBullet1()
{
	for (int i = 0; i < m_iBullet1TotalNum; i++)
	{
		if (!dynamic_pointer_cast<CGameObject>(m_pVecBullet1[i])->GetIsEnable())
		{
			dynamic_pointer_cast<CGameObject>(m_pVecBullet1[i])->SetEnable(true);
			m_pVecBullet1[i]->SetOnCollision(false);
			shared_ptr<CTransform> pTransform = dynamic_pointer_cast<CTransform>(
				m_pVecBullet1[i]->GetComponent(L"Com_Transform", ID_DYNAMIC));
			pTransform->SetPosition(m_vPos.x, m_vPos.y, m_vPos.z);
			_vec3 vPosPlayer;
			m_pPlayerTransformCom->GetInfo(INFO_POS, &vPosPlayer);
			vPosPlayer = _vec3(vPosPlayer.x, vPosPlayer.y , vPosPlayer.z);
			m_pVecBullet1[i]->SetTarget(vPosPlayer);
			break;
		}
	}
}