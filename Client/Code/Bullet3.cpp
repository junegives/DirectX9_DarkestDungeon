#include "pch.h"
#include "Bullet3.h"
#include "Export_Utility.h"
#include "Export_System.h"

#include "Player.h"


CBullet3::CBullet3(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{
}

CBullet3::CBullet3(const CBullet3& rhs)
	: Engine::CGameObject(rhs)
{
}

CBullet3::~CBullet3()
{
}


HRESULT CBullet3::ReadyGameObject()
{
	__super::ReadyGameObject();

	// 플레이어 받아오기 (거리 계산, 상태 변경)
	m_pPlayer = CGameMgr::GetInstance()->GetPlayer();

	// 컴퍼넌트 받아오기
	m_pPlayerTransformCom = dynamic_pointer_cast<CTransform>(m_pPlayer->GetComponent(L"Com_Transform", ID_DYNAMIC));

	m_pTransformCom->SetAngle(m_vAngle);

	{
		m_eAnimState = EBullet3State::IDLE;
		m_vAngle = { 0.f, PI / 2, 0.f };

		m_pTransformCom->SetPosition(m_vPos.x, m_vPos.y, m_vPos.z);

		m_pTransformCom->SetAngle(m_vAngle);
		m_pTransformCom->Rotation(ROT_Y, PI / 2.f);

		m_pTextureCom->SetAnimKey(L"Boss_Projectile3", 0.06f);
		m_vOriginSize = m_pTextureCom->GetTextureSize();
	}

	SetEnable(false);

	return S_OK;
}

_int CBullet3::UpdateGameObject(const _float& fTimeDelta)
{
	_int	iExit = __super::UpdateGameObject(fTimeDelta);

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

void CBullet3::LateUpdateGameObject()
{
	__super::LateUpdateGameObject();

	//빌보드 시작
	_matrix matWorld;

	matWorld = *m_pTransformCom->GetWorld();
	SetBillBoard(matWorld);
	m_pTransformCom->SetWorld(&matWorld);
}

void CBullet3::RenderGameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->GetWorld());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTextureCom->SetAnimTexture();
	m_pBufCom->RenderBuffer();
	m_pEffectBufCom->RenderBuffer();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	__super::RenderGameObject();
}

void CBullet3::AddComponent()
{
	shared_ptr<CComponent> pComponent;

	pComponent = m_pTransformCom = make_shared<CTransform>();
	m_pTransformCom->ReadyTransform();
	m_pTransformCom->SetScale(m_vScale.x, m_vScale.y, m_vScale.x);
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

void CBullet3::FSM(const _float& fTimeDelta)
{
	if (m_eCurAnimState == EBullet3State::IDLE)
	{
		FireToPosition(fTimeDelta, m_fShootSpeed, m_vTargetPos);
		if (CalcDistance(m_vTargetPos) < 0.1f)
		{
			m_eCurAnimState = EBullet3State::SHOOT;
		}
	}

	// 업데이트 해제
	m_fAliveTime -= fTimeDelta;
	if (m_fAliveTime < 0.f)
	{
		m_eCurAnimState = EBullet3State::IDLE;
		m_fAliveTime = 6.f;
		SetEnable(false);
	}
}

void CBullet3::ChangeAnim()
{
	_float fXpos, fYpos;
	// Animation
	if (m_eCurAnimState != m_ePrevAnimState)
	{
		switch (m_eCurAnimState)
		{
		case EBullet3State::IDLE:
			m_pTextureCom->SetAnimKey(L"Boss_Projectile3", 0.06f);
			break;
		}
		m_ePrevAnimState = m_eCurAnimState;

	}

	_vec2 vcurPos = m_pTextureCom->GetTextureSize();

	if (m_eCurAnimState == EBullet3State::IDLE) {
		fXpos = (vcurPos.x / m_vOriginSize.x);
		fYpos = (vcurPos.y / m_vOriginSize.y);
	}
	else {
		fXpos = (vcurPos.x / m_vOriginSize.x);
		fYpos = (vcurPos.y / m_vOriginSize.y);
	}

	m_pTransformCom->SetScale(20.f * fXpos, 20.f * fYpos, 20.f * fXpos);

}

void CBullet3::AnimDuration(const _float& fTimeDelta)
{
	/*if (m_eCurAnimState == EBullet3State::IDLE)
	{
		m_fP1AttackTime -= fTimeDelta;
		if (m_fP1AttackTime < 0.f)
		{
			m_fP1AttackTime = 0.06f * 12;
			m_eCurAnimState = EBullet3State::P1_IDLE;
		}
	}*/
}

void CBullet3::KeyInput()
{
}

_float CBullet3::CalcDistance(_vec3 _Pos)
{
	_vec3 vPos;
	m_pTransformCom->GetInfo(INFO_POS, &vPos);

	_vec3 vDist = _Pos - vPos;

	return D3DXVec3Length(&vDist);
}

_vec3 CBullet3::CalcDirection(_vec3 _Pos)
{
	_vec3 vPos;
	m_pTransformCom->GetInfo(INFO_POS, &vPos);

	_vec3 vDist = _Pos - vPos;

	return vDist;
}

void CBullet3::FireToPosition(const _float& fTimeDelta, float _fSpeed, _vec3 _Pos)
{
	_vec3		vDir;
	vDir = CalcDirection(_Pos);
	D3DXVec3Normalize(&vDir, &vDir);
	m_pTransformCom->MoveForward(&vDir, fTimeDelta, _fSpeed);
}

_bool CBullet3::OutArea()
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
	if (m_pTransformCom->GetPos()->y > 300.f)
	{
		return true;
	}

	return false;
}
