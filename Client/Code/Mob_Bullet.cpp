#include "pch.h"
#include "Mob_Bullet.h"
#include "Export_Utility.h"
#include "Export_System.h"

#include "Player.h"
#include "SoundMgr.h"


CMobBullet::CMobBullet(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{
}

CMobBullet::CMobBullet(const CMobBullet& rhs)
	: Engine::CGameObject(rhs)
{
}

CMobBullet::~CMobBullet()
{
}


HRESULT CMobBullet::ReadyGameObject()
{
	__super::ReadyGameObject();

	// 플레이어 받아오기 (거리 계산, 상태 변경)
	m_pPlayer = CGameMgr::GetInstance()->GetPlayer();

	// 컴퍼넌트 받아오기
	m_pPlayerTransformCom = dynamic_pointer_cast<CTransform>(m_pPlayer->GetComponent(L"Com_Transform", ID_DYNAMIC));

	m_pTransformCom->SetAngle(m_vAngle);

	{
		m_eAnimState = EMobBulletState::IDLE;
		m_vAngle = { 0.f, PI / 2, 0.f };

		m_pTransformCom->SetPosition(m_vPos.x, m_vPos.y, m_vPos.z);

		m_pTransformCom->SetAngle(m_vAngle);
		m_pTransformCom->Rotation(ROT_Y, PI / 2.f);

		m_pTextureCom->SetAnimKey(L"Mob_Missile", 0.06f);
		m_vOriginSize = m_pTextureCom->GetTextureSize();
	}

	m_pColliderCom->SetPos(m_pTransformCom->GetPos());

	m_bColliding = true;
	m_eCollideID = ECollideID::BOSS_PROJECTILE;

	SetEnable(false);

	return S_OK;
}

_int CMobBullet::UpdateGameObject(const _float& fTimeDelta)
{
	_int	iExit = __super::UpdateGameObject(fTimeDelta);

	// 한번 막기
	if (m_bEnable && !m_bMakeEnable) {
		m_bMakeEnable = true;
		m_bEnable = false;
	}

	m_vPos = *m_pTransformCom->GetPos();

	Engine::AddRenderGroup(RENDER_ALPHA, shared_from_this());

	// FSM 조건
	FSM(fTimeDelta);

	// 애니메이션 변경
	ChangeAnim();
	// 시간에 따른 애니메이션 끝내기
	AnimDuration(fTimeDelta);

	// 키 입력
	KeyInput();

	

	return iExit;
}

void CMobBullet::LateUpdateGameObject()
{
	__super::LateUpdateGameObject();

	//빌보드 시작
	_matrix matWorld;

	matWorld = *m_pTransformCom->GetWorld();
	SetBillBoard(matWorld);
	m_pTransformCom->SetWorld(&matWorld);
}

void CMobBullet::RenderGameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->GetWorld());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTextureCom->SetAnimTexture();;
	m_pBufCom->RenderBuffer();
	m_pEffectBufCom->RenderBuffer();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	__super::RenderGameObject();
}

void CMobBullet::AddComponent()
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

void CMobBullet::OnCollide(shared_ptr<CGameObject> _pObj)
{
	if (m_bCollsion)
	{
		return;
	}

	if (ECollideID::WALL == _pObj->GetColType() || ECollideID::ENVIRONMENT == _pObj->GetColType())
	{
		//Effect Setting
		/*shared_ptr<Engine::CEffect> pEffect = CEffectMgr::GetInstance()->GetEffect();

		pEffect->SetProjEffect(m_strEffectAnimKey, *m_pTransformCom->GetPos(), 0.5f);
		pEffect->SetActive(true);*/

		m_pTransformCom->SetPosition(400.f, -200.f, 300.f);
		m_bCollsion = true;
		SetEnable(false);

		CSoundMgr::GetInstance()->CSoundMgr::GetInstance()->StopSound(CHANNELID::BOSS_PROJ2);
		CSoundMgr::GetInstance()->PlaySound(L"Boss_ProjectileImpact.wav", CHANNELID::BOSS_PROJ2, 1.f);
	}

	if (ECollideID::PLAYER == _pObj->GetColType())
	{
		m_pTransformCom->SetPosition(400.f, -200.f, 300.f);
		m_bCollsion = true;
		SetEnable(false);

		CSoundMgr::GetInstance()->CSoundMgr::GetInstance()->StopSound(CHANNELID::BOSS_PROJ2);
		CSoundMgr::GetInstance()->PlaySound(L"Boss_ProjectileImpact.wav", CHANNELID::BOSS_PROJ2, 1.f);
	}
}

void CMobBullet::FSM(const _float& fTimeDelta)
{
	if (m_eCurAnimState == EMobBulletState::IDLE)
	{
		FireToPosition(fTimeDelta, m_fShootSpeed, m_vTargetPos);
	}

	// 업데이트 해제
	if (CalcDistance(m_vTargetPos) < 2.f)
	{
		m_pTransformCom->SetPosition(400.f, -200.f, 300.f);
		SetEnable(false);
	}
}

void CMobBullet::ChangeAnim()
{
	_float fXpos, fYpos;
	// Animation
	if (m_eCurAnimState != m_ePrevAnimState)
	{
		switch (m_eCurAnimState)
		{
		case EMobBulletState::IDLE:
			m_pTextureCom->SetAnimKey(L"Mob_Missile", 0.06f);
			break;
		}
		m_ePrevAnimState = m_eCurAnimState;

	}

	_vec2 vcurPos = m_pTextureCom->GetTextureSize();

	if (m_eCurAnimState == EMobBulletState::IDLE) {
		fXpos = (vcurPos.x / m_vOriginSize.x);
		fYpos = (vcurPos.y / m_vOriginSize.y);
	}
	else {
		fXpos = (vcurPos.x / m_vOriginSize.x);
		fYpos = (vcurPos.y / m_vOriginSize.y);
	}

	m_pTransformCom->SetScale(1.5f * fXpos, 1.5f * fYpos, 1.5f * fXpos);
	m_pColliderCom->SetScale({ 3.f * fXpos, 3.f * fYpos, 3.f * fXpos });
	m_pColliderCom->SetRadius(3.f * fXpos);
}

void CMobBullet::AnimDuration(const _float& fTimeDelta)
{
	/*if (m_eCurAnimState == EMobBulletState::IDLE)
	{
		m_fP1AttackTime -= fTimeDelta;
		if (m_fP1AttackTime < 0.f)
		{
			m_fP1AttackTime = 0.06f * 12;
			m_eCurAnimState = EMobBulletState::P1_IDLE;
		}
	}*/
}

void CMobBullet::KeyInput()
{
}

_float CMobBullet::CalcDistance(_vec3 _Pos)
{
	_vec3 vPos;
	m_pTransformCom->GetInfo(INFO_POS, &vPos);

	_vec3 vDist = _Pos - vPos;

	return D3DXVec3Length(&vDist);
}

_vec3 CMobBullet::CalcDirection(_vec3 _Pos)
{
	_vec3 vPos;
	m_pTransformCom->GetInfo(INFO_POS, &vPos);

	_vec3 vDist = _Pos - vPos;

	return vDist;
}

void CMobBullet::FireToPosition(const _float& fTimeDelta, float _fSpeed, _vec3 _Pos)
{
	_vec3		vDir;
	vDir = CalcDirection(_Pos);
	D3DXVec3Normalize(&vDir, &vDir);
	m_pTransformCom->MoveForward(&vDir, fTimeDelta, _fSpeed);
}
