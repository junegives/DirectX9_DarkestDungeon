#include "pch.h"
#include "Sunken.h"
#include "Export_Utility.h"
#include "Export_System.h"

#include "Laser.h"
#include "Player.h"

CSunken::CSunken(LPDIRECT3DDEVICE9 pGraphicDev)
	:CCreature(pGraphicDev)
{
}

CSunken::CSunken(LPDIRECT3DDEVICE9 pGraphicDev, STAT _tCommonStat, _int _iPosition,
	vector<shared_ptr<CSkill>>& _pVecSkill)
	: CCreature(pGraphicDev, _tCommonStat, _iPosition, _pVecSkill)
{
}

CSunken::CSunken(LPDIRECT3DDEVICE9 pGraphicDev, STAT _tCommonStat)
	: CCreature(pGraphicDev, _tCommonStat)
{
}

CSunken::CSunken(const CCreature& rhs)
	: CCreature(rhs)
{
}

CSunken::~CSunken()
{
}

void CSunken::PickingObj()
{
	m_bPicked = true;
}

HRESULT CSunken::ReadyGameObject()
{
	__super::ReadyGameObject();

	// 플레이어 받아오기 (거리 계산, 상태 변경)
	m_pPlayer = CGameMgr::GetInstance()->GetPlayer();

	// 컴퍼넌트 받아오기
	m_pPlayerTransformCom = dynamic_pointer_cast<CTransform>(m_pPlayer->GetComponent(L"Com_Transform", ID_DYNAMIC));

	m_pTransformCom->SetAngle(m_vAngle);

	{
		m_eAnimState = ESunkenState::IDLE;
		m_vAngle = { 0.f, PI / 2, 0.f };

		m_pTransformCom->SetPosition(m_vPos.x, m_vPos.y, m_vPos.z);

		m_pTransformCom->SetAngle(m_vAngle);
		m_pTransformCom->Rotation(ROT_Y, PI / 2.f);

		m_pTextureCom->SetAnimKey(L"Sunken", 0.12f);
		m_vOriginSize = m_pTextureCom->GetTextureSize();
	}
	m_pColliderCom->SetPos(m_pTransformCom->GetPos());

	m_bColliding = true;
	m_eCollideID = ECollideID::BOSS_SUNKEN;

	SetEnable(false);

	return S_OK;
}

_int CSunken::UpdateGameObject(const _float& fTimeDelta)
{
	_int	iExit = CGameObject::UpdateGameObject(fTimeDelta);

	// 한번 막기
	if (m_bEnable && !m_bMakeEnable) {
		m_bMakeEnable = true;
		m_bEnable = false;
	}

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

void CSunken::LateUpdateGameObject()
{
	__super::LateUpdateGameObject();
}

void CSunken::RenderGameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->GetWorld());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTextureCom->SetAnimTexture();;
	m_pBufCom->RenderBuffer();
	m_pEffectBufCom->RenderBuffer();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	__super::RenderGameObject();
}

void CSunken::AddComponent()
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
	m_pTextureCom->SetAnimKey(m_strAnimKey, 0.12f);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animator", pComponent });

	pComponent = m_pColliderCom = make_shared<CCollider>(m_pGraphicDev);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider",pComponent });
	m_pColliderCom->SetScale({ 4.f, 14.f, 4.f });
	m_pColliderCom->SetPos(m_pTransformCom->GetPos());
}

void CSunken::OnCollide(shared_ptr<CGameObject> _pObj)
{
}

void CSunken::FSM(const _float& fTimeDelta)
{
	// Out
	if (m_eCurAnimState == ESunkenState::ATTACK)
	{
		m_fAttackTime -= fTimeDelta;
		if (m_fAttackTime < 0.f)
		{
			m_fAttackTime = 0.12f * 13;
			m_bAttack = false;
			m_pTransformCom->SetPosition(300.f, -500.f, 300.f);
			m_eCurAnimState = ESunkenState::IDLE;
			SetEnable(false);
		}
	}
}

void CSunken::ChangeAnim()
{
	_float fXpos, fYpos;
	// Animation
	if (m_eCurAnimState != m_ePrevAnimState)
	{
		switch (m_eCurAnimState)
		{
		case ESunkenState::ATTACK:
			m_pTextureCom->SetAnimKey(L"Sunken", 0.12f);
			break;
		}
		m_ePrevAnimState = m_eCurAnimState;

	}

	_vec2 vcurPos = m_pTextureCom->GetTextureSize();

	if (m_eCurAnimState == ESunkenState::ATTACK) {
		fXpos = (vcurPos.x / m_vOriginSize.x);
		fYpos = (vcurPos.y / m_vOriginSize.y);
	}
	else {
		fXpos = (vcurPos.x / m_vOriginSize.x);
		fYpos = (vcurPos.y / m_vOriginSize.y);
	}

	m_pTransformCom->SetScale(8.f * 2.f * fXpos, 7.f * 2.f * fYpos, 8.f * 2.f * fXpos);
	m_pColliderCom->SetScale({ 4.f * 2.f * fXpos, 14.f * 2.f * fYpos, 4.f * 2.f * fXpos });

}

void CSunken::AnimDuration(const _float& fTimeDelta)
{

}

void CSunken::KeyInput()
{

}