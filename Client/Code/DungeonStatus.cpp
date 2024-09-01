#include "pch.h"
#include "DungeonStatus.h"
#include "GameMgr.h"
#include "Player.h"
#include "Hero.h"
#include "Export_Utility.h"
#include "Export_System.h"

#include "EffectMgr.h"

#include "SoundMgr.h"
#include "CameraMgr.h"

CDungeonStatus::CDungeonStatus(LPDIRECT3DDEVICE9 _pGraphicDev)
	:CUIObj(_pGraphicDev)
{
}

CDungeonStatus::~CDungeonStatus()
{
}

HRESULT CDungeonStatus::ReadyGameObject()
{

	m_bVisible = false;
	m_bEnable = true;
	m_bActive = true;


	return S_OK;
}

_int CDungeonStatus::UpdateGameObject(const _float& fTimeDelta)
{

	if (m_vHeroVec.empty()) {

		if (CGameMgr::GetInstance()->GetPlayer()) {
			vector<shared_ptr<CGameObject>> vecTemp = *dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->GetHeroVec();

			for (auto& iter : vecTemp) {
				m_vHeroVec.push_back(dynamic_pointer_cast<CHero>(iter)->GetHeroType());
			}
		}

		m_iCurrentHeroNum = (size_t)m_vHeroVec.size();
		m_bVisible = true;
	}

	if (!m_bVisible)
		return 0;

	_int iExit = CGameObject::UpdateGameObject(fTimeDelta);
	AddRenderGroup(RENDER_UI, shared_from_this());

	return iExit;
}

void CDungeonStatus::LateUpdateGameObject()
{
}

void CDungeonStatus::RenderGameObject()
{

	if (!m_bVisible) {
		return;
	}

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);

	for (int i = 0; i < m_iCurrentHeroNum; ++i) {
		m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom[i]->GetWorld());
		m_pTextureCom->SetTexture(static_cast<_uint>(m_vHeroVec[i]));
		m_pRCTexCom2[i]->RenderBuffer();

	}

	m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

}

void CDungeonStatus::GetEvent(_bool _bHPEvent, EHeroType _eHero)
{
	if (!m_bVisible)
		return;


	_int iCount = 0;

	for (auto& iter : m_vHeroVec) {
		if (_eHero == iter) {
			break;
		}
		++iCount;
	}
	
	_vec3 vPos = *m_pTransformCom[iCount]->GetPos();
	
	if (_bHPEvent) {
		////체력차감 이벤트
		shared_ptr<CEffect> pEffect = CEffectMgr::GetInstance()->GetEffect();
		pEffect->SetAnimEffect(L"Effect_Blood", _vec3(vPos.x , vPos.y -5.f , 0.f), _vec3(45.f, 45.f, 1.f), 2.f, true);
		pEffect->SetActive(true);

	}
	else {
		//스트레스 이벤트
		shared_ptr<CEffect> pEffect = CEffectMgr::GetInstance()->GetEffect();
		pEffect->SetAnimEffect(L"UI_Head_StressUp", _vec3(vPos.x, vPos.y +10.f, 0.f), _vec3(60.f, 50.f, 1.f), 2.f, true);
		pEffect->SetActive(true);

	}

	CSoundMgr::GetInstance()->StopSound(CHANNELID::HIGHWAYMAN);
	CSoundMgr::GetInstance()->PlaySound(L"Hero_Stress_Up.wav", CHANNELID::HIGHWAYMAN, 1.f);

}

void CDungeonStatus::GetBuff(EHeroType _eHero)
{

	if (!m_bVisible)
		return;


	_int iCount = 0;

	for (auto& iter : m_vHeroVec) {
		if (_eHero == iter) {
			break;
		}
		++iCount;
	}

	_vec3 vPos = *m_pTransformCom[iCount]->GetPos();

	shared_ptr<CEffect> pEffect = CEffectMgr::GetInstance()->GetEffect();
	pEffect->SetAnimEffect(L"Stat_Attribute_Buff", _vec3(vPos.x +50.f, vPos.y+30.f, 0.f), _vec3(15.f,20.f, 1.f), 2.f, true);
	pEffect->SetActive(true);

	

	CSoundMgr::GetInstance()->StopSound(CHANNELID::HIGHWAYMAN);
	CSoundMgr::GetInstance()->PlaySound(L"Gen_Status_Buff.wav", CHANNELID::HIGHWAYMAN, 1.f);

}

void CDungeonStatus::AddComponent()
{

	shared_ptr<CComponent> pComponent;


	for (int i = 0; i < 4; ++i) {

		//Transform 생성
		pComponent = m_pTransformCom[i] = make_shared<CTransform>();
		m_pTransformCom[i]->ReadyTransform();

		tstring strCurNum = to_wstring(i);
		tstring strComName = L"Com_Transform" + strCurNum;
		tstring strComName2 = L"Com_RcTex" + strCurNum;

		m_mapComponent[ID_DYNAMIC].insert({ strComName ,pComponent });
		m_pTransformCom[i]->SetScale(42.f, 42.f, 1.f);
		m_pTransformCom[i]->SetPosition(-600.f, 100.f - 85.f * i, 0.f);

		//RcTex생성
		pComponent = m_pRCTexCom2[i] = make_shared<CRcTex>(m_pGraphicDev);
		m_pRCTexCom2[i]->ReadyBuffer();
		m_mapComponent[ID_STATIC].insert({ strComName2,pComponent });

	}


	pComponent = m_pTextureCom = make_shared<CTexture>(m_pGraphicDev);
	m_pTextureCom->SetTextureKey(L"UI_HeroPortrait", TEXTUREID::TEX_NORMAL);

}
