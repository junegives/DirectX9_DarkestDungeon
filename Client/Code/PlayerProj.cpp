#include "pch.h"
#include "PlayerProj.h"
#include "Export_System.h"
#include "Export_Utility.h"

#include "EffectMgr.h"
#include "Effect.h"

#include "ParticleMgr.h"
#include "FireworkParticle.h"


CPlayerProj::CPlayerProj(LPDIRECT3DDEVICE9 _pGraphicDev, tstring _strKeyName, _vec3 _vInitPos, _matrix _matPlrWorld)
	:CGameObject(_pGraphicDev), m_strAnimKeyName(_strKeyName)
{
	memcpy(&m_vStartPos, &_vInitPos, sizeof(_vInitPos));
	memcpy(&m_matWorld, &_matPlrWorld, sizeof(_matPlrWorld));
	ZeroMemory(&m_tProjInfo, sizeof(m_tProjInfo));
}

CPlayerProj::~CPlayerProj()
{
}

HRESULT CPlayerProj::ReadyGameObject()
{
	//√ ±‚ ¿ßƒ° setting
	m_pTransmCom->SetWorld(&m_matWorld);
	memcpy(&m_vLook, &m_matWorld.m[2][0], sizeof(_vec3));
	m_pTransmCom->SetPosition(m_vStartPos.x, m_vStartPos.y + 1.f, m_vStartPos.z);

	m_tProjInfo.iSpeed = 200;
	m_strEffectAnimKey = L"SpellHand_Proj_Effect";
	m_pColliderCom->SetPos(m_pTransmCom->GetPos());


	m_bColliding = true;
	m_eCollideID = ECollideID::PLAYER_PROJECTILE;

	return S_OK;
}

_int CPlayerProj::UpdateGameObject(const _float& fTimeDelta)
{
	if (!m_bTestBool)
		return 0;

	_int iExit(0);
	iExit = __super::UpdateGameObject(fTimeDelta);


	//MoveForward
	_vec3 vLook, vPos;
	m_pTransmCom->GetInfo(INFO::INFO_LOOK, &vLook);
	m_pTransmCom->GetInfo(INFO::INFO_POS, &vPos);

	vPos += fTimeDelta * m_tProjInfo.iSpeed * m_vLook;

	m_fTotalRange += fTimeDelta * m_tProjInfo.iSpeed;

	m_pTransmCom->SetPosition(vPos.x, vPos.y, vPos.z);

	_matrix matWorld;

	//∫Ù∫∏µÂ
	matWorld = *m_pTransmCom->GetWorld();
	SetBillBoard(matWorld);
	m_pTransmCom->SetWorld(&matWorld);

	AddRenderGroup(RENDERID::RENDER_ALPHA, shared_from_this());

	return iExit;
}

void CPlayerProj::LateUpdateGameObject()
{
	if (m_fTotalRange > 800.f) {
		m_bEnable = false;
	}
}

void CPlayerProj::RenderGameObject()
{
	if (!m_bTestBool)
		return;

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransmCom->GetWorld());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);


	m_pTextureCom->SetTexture();
	m_pRcTexCom->RenderBuffer();


	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);


}

void CPlayerProj::AddComponent()
{
	shared_ptr<CComponent> pComponent;

	pComponent = m_pRcTexCom = make_shared<CRcTex>(m_pGraphicDev);
	m_pRcTexCom->ReadyBuffer();
	m_mapComponent[ID_STATIC].insert({ L"Com_RcTex",pComponent });

	pComponent = m_pTextureCom = make_shared<CTexture>(m_pGraphicDev);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture",pComponent });
	m_pTextureCom->SetTextureKey(m_strAnimKeyName, TEXTUREID::TEX_NORMAL);

	pComponent = m_pTransmCom = make_shared<CTransform>();
	m_pTransmCom->ReadyTransform();
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform",pComponent });
	m_pTransmCom->SetAngle({ 0.f, 0.f, 0.f });
	m_pTransmCom->SetScale(0.5f, 0.5f, 1.f);

	pComponent = m_pColliderCom = make_shared<CCollider>(m_pGraphicDev);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider",pComponent });
	m_pColliderCom->SetScale({ 1.f, 1.f, 1.f });
	m_pColliderCom->SetRadius(1.f);

}

void CPlayerProj::OnCollide(shared_ptr<CGameObject> _pObj)
{
	if (!m_bTestBool)
		return;

	if (ECollideID::WALL == _pObj->GetColType() || ECollideID::ENVIRONMENT == _pObj->GetColType())
	{
		// Effect Setting
		// ≈∫»Ø ¿Ã∆Â∆Æ
		shared_ptr<Engine::CEffect> pEffect = CEffectMgr::GetInstance()->GetEffect();

		pEffect->SetProjEffect(m_strEffectAnimKey, *m_pTransmCom->GetPos(), 0.5f);
		pEffect->SetActive(true);

		shared_ptr<CParticleSystem> pParticle = CParticleMgr::GetInstance()->GetExplosionParticle();

		if (pParticle)
		{
			pParticle->SettingOrigin(m_pTransmCom->GetPos(), 100);
			pParticle->SetActive(true);
		}

		m_pTransmCom->SetPosition(200.f, -200.f, 200.f);

		m_bTestBool = false;
		m_bEnable = false;

	}
	else if (ECollideID::BOSS == _pObj->GetColType())
	{
		// Effect Setting
		// ≈∫»Ø ¿Ã∆Â∆Æ
		shared_ptr<Engine::CEffect> pEffect = CEffectMgr::GetInstance()->GetEffect();

		pEffect->SetProjEffect(m_strEffectAnimKey, *m_pTransmCom->GetPos(), 0.5f);
		pEffect->SetActive(true);

		shared_ptr<Engine::CEffect> pEffect2 = CEffectMgr::GetInstance()->GetEffect();
		switch (rand() % 2)
		{
		case 0:
			// Effect
			pEffect2->SetAnimEffect(L"Green_Blood", *m_pTransmCom->GetPos(), _vec3(3.f, 3.f, 3.f), 1.f, false);
			pEffect2->SetActive(true);
			break;
		case 1:
			// Effect
			pEffect2->SetAnimEffect(L"Yellow_Blood", *m_pTransmCom->GetPos(), _vec3(3.f, 3.f, 3.f), 1.f, false);
			pEffect2->SetActive(true);
			break;
		}

		shared_ptr<CParticleSystem> pParticle = CParticleMgr::GetInstance()->GetExplosionParticle();

		if (pParticle)
		{
			pParticle->SettingOrigin(m_pTransmCom->GetPos(), 100);
			pParticle->SetActive(true);
		}

		m_pTransmCom->SetPosition(200.f, -200.f, 200.f);

		m_bTestBool = false;
		m_bEnable = false;
	}
	else if (ECollideID::MOB == _pObj->GetColType())
	{
		// Effect Setting
		// ≈∫»Ø ¿Ã∆Â∆Æ
		shared_ptr<Engine::CEffect> pEffect = CEffectMgr::GetInstance()->GetEffect();

		pEffect->SetProjEffect(m_strEffectAnimKey, *m_pTransmCom->GetPos(), 0.5f);
		pEffect->SetActive(true);

		// Effect
		shared_ptr<Engine::CEffect> pEffect2 = CEffectMgr::GetInstance()->GetEffect();
		switch (rand() % 2)
		{
		case 0:
			// Effect
			pEffect2->SetAnimEffect(L"Green_Blood", *m_pTransmCom->GetPos(), _vec3(3.f, 3.f, 3.f), 1.f, false);
			pEffect2->SetActive(true);
			break;
		case 1:
			// Effect
			pEffect2->SetAnimEffect(L"Yellow_Blood", *m_pTransmCom->GetPos(), _vec3(3.f, 3.f, 3.f), 1.f, false);
			pEffect2->SetActive(true);
			break;
		}

		m_pTransmCom->SetPosition(200.f, -200.f, 200.f);

		m_bTestBool = false;
		//m_bEnable = false;

		// Particle Setting
		/*dynamic_pointer_cast<CFireworkParticle>(m_pParticle)->Setting(m_pTransmCom->GetPos(), 6000);
		m_pParticle->SetActive(true);*/
	}
	m_bEnable = false;
}
