#include "pch.h"
#include "Laser.h"
#include "Export_Utility.h"
#include "Export_System.h"

#include "Player.h"


CLaser::CLaser(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
{
}

CLaser::CLaser(const CLaser& rhs)
	: Engine::CGameObject(rhs)
{
}

CLaser::~CLaser()
{
}


HRESULT CLaser::ReadyGameObject()
{
	__super::ReadyGameObject();

	// 플레이어 받아오기 (거리 계산, 상태 변경)
	m_pPlayer = CGameMgr::GetInstance()->GetPlayer();

	// 컴퍼넌트 받아오기
	m_pPlayerTransformCom = dynamic_pointer_cast<CTransform>(m_pPlayer->GetComponent(L"Com_Transform", ID_DYNAMIC));

	m_pTransformCom->SetAngle(m_vAngle);
	
	{
		m_eAnimState = ELaserState::IDLE;
		m_vAngle = { 0.f, PI / 2, 0.f };

		m_pTransformCom->SetPosition(m_vPos.x, m_vPos.y, m_vPos.z);

		m_pTransformCom->SetAngle(m_vAngle);
		m_pTransformCom->Rotation(ROT_Y, PI / 2.f);

		m_pTextureCom->SetAnimKey(L"Boss_Laser", 0.06f);
		m_vOriginSize = m_pTextureCom->GetTextureSize();
	}

	SetEnable(false);

	return S_OK;
}

_int CLaser::UpdateGameObject(const _float& fTimeDelta)
{
	_int	iExit = __super::UpdateGameObject(fTimeDelta);

	m_vPos = *m_pTransformCom->GetPos();

	Engine::AddRenderGroup(RENDER_ALPHA, shared_from_this());

	_vec3 vDir = m_vTargetPos - m_vStartPos;
	_vec3 vPos = m_vStartPos + vDir * 0.5f;

	m_pTransformCom->SetPosition(vPos.x, vPos.y, vPos.z);

	m_pTransformCom->SetScale(3.f, D3DXVec3Length(&vDir), 0.f);

	_vec3 vBossTransform;

	_vec3 Dir1;
	_vec3 Dir2;
	float Dot;

	Dir1 = _vec3(0.f, -1.f, 0.f);
	Dir2 = _vec3(vDir.x, 0.f, vDir.z);
	D3DXVec3Normalize(&Dir1, &Dir1);
	D3DXVec3Normalize(&Dir2, &Dir2);
	
	Dot = D3DXVec3Dot(&Dir1, &Dir2);
	float a = acosf(Dot);
	
	Dir1 = _vec3(0.f, -1.f, 0.f);
	Dir2 = _vec3(vDir.x, 0.f, vDir.z);
	D3DXVec3Normalize(&Dir1, &Dir1);
	D3DXVec3Normalize(&Dir2, &Dir2);

	Dot = D3DXVec3Dot(&Dir1, &Dir2);
	float b = acosf(Dot);

	m_pTransformCom->SetAngle(_vec3(a, b, 0.f));

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

void CLaser::LateUpdateGameObject()
{
	__super::LateUpdateGameObject();
}

void CLaser::RenderGameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->GetWorld());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTextureCom->SetAnimTexture();;
	m_pBufCom->RenderBuffer();
	m_pEffectBufCom->RenderBuffer();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	__super::RenderGameObject();
}

void CLaser::AddComponent()
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

void CLaser::FSM(const _float& fTimeDelta)
{
	if (m_eCurAnimState == ELaserState::IDLE)
	{
		//FireToPosition(fTimeDelta, m_fShootSpeed, m_vTargetPos);
	}

	// 업데이트 해제
	if (m_bEnable)
	{
		m_fAttackTime -= fTimeDelta;
		if (m_fAttackTime < 0.f)
		{
			m_fAttackTime = 2.f;
			m_bEnable = false;
		}
	}
}

void CLaser::ChangeAnim()
{
	_float fXpos, fYpos;
	// Animation
	if (m_eCurAnimState != m_ePrevAnimState)
	{
		switch (m_eCurAnimState)
		{
		case ELaserState::IDLE:
			m_pTextureCom->SetAnimKey(L"Boss_Laser", 0.06f);
			break;
		}
		m_ePrevAnimState = m_eCurAnimState;

	}

	_vec2 vcurPos = m_pTextureCom->GetTextureSize();

	if (m_eCurAnimState == ELaserState::IDLE) {
		fXpos = (vcurPos.x / m_vOriginSize.x);
		fYpos = (vcurPos.y / m_vOriginSize.y);
	}
	else {
		fXpos = (vcurPos.x / m_vOriginSize.x);
		fYpos = (vcurPos.y / m_vOriginSize.y);
	}

	m_pTransformCom->SetScale(2.f * fXpos, 30.f * fYpos, 1.f);

}

void CLaser::AnimDuration(const _float& fTimeDelta)
{
	/*if (m_eCurAnimState == ELaserState::IDLE)
	{
		m_fP1AttackTime -= fTimeDelta;
		if (m_fP1AttackTime < 0.f)
		{
			m_fP1AttackTime = 0.06f * 12;
			m_eCurAnimState = ELaserState::P1_IDLE;
		}
	}*/
}

void CLaser::KeyInput()
{
}

_float CLaser::CalcDistance(_vec3 _Pos)
{
	_vec3 vPos;
	m_pTransformCom->GetInfo(INFO_POS, &vPos);

	_vec3 vDist = _Pos - vPos;

	return D3DXVec3Length(&vDist);
}

_vec3 CLaser::CalcDirection(_vec3 _Pos)
{
	_vec3 vPos;
	m_pTransformCom->GetInfo(INFO_POS, &vPos);

	_vec3 vDist = _Pos - vPos;

	return vDist;
}

void CLaser::FireToPosition(const _float& fTimeDelta, float _fSpeed, _vec3 _Pos)
{
	_vec3		vDir;
	vDir = CalcDirection(_Pos);
	D3DXVec3Normalize(&vDir, &vDir);
	m_pTransformCom->MoveForward(&vDir, fTimeDelta, _fSpeed);
}
