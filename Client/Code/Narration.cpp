#include"pch.h"
#include "Narration.h"
#include"Export_Utility.h"
#include"Export_System.h"
#include"SoundMgr.h"

CNarration::CNarration(LPDIRECT3DDEVICE9 _pGraphicDev)
	:CUIObj(_pGraphicDev)
{
}

CNarration::~CNarration()
{
}

HRESULT CNarration::ReadyGameObject()
{
	if (m_bReady) return S_OK;


	m_vSize = { WINCX * 0.5f, 150.f, 0.f };
	m_vPos = { 0.f , 280.f, 0.f };
	m_vAngle = { 0.f ,0.f, 0.f };

	m_bVisible = false;
	m_bEnable = true;
	m_bActive = true;

	m_pTransCom->SetScale(m_vSize.x, m_vSize.y, m_vSize.z);
	m_pTransCom->SetPosition(m_vPos.x, m_vPos.y, m_vPos.z);
	m_pTransCom->SetAngle({ 0.f, 0.f, 0.f });

	CUIObj::ReadyGameObject();

	SetNarrationMap();

	return S_OK;
}

_int CNarration::UpdateGameObject(const _float& fTimeDelta)
{

	if (!m_bVisible)
		return 0;


	if (fTimeDelta > 5.f)
		return 0;

	m_fActTime += fTimeDelta;

	if (m_fActTime > m_fTotalTime) {

		m_bVisible = false;
		m_fActTime = 0.f;
		m_strCurrentKey = L"";
	}

	CGameObject::UpdateGameObject(fTimeDelta);
	AddRenderGroup(RENDER_UI, shared_from_this());

	return 0;
}

void CNarration::LateUpdateGameObject()
{
}

void CNarration::RenderGameObject()
{
	if (!m_bVisible) {
		return;
	}


	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransCom->GetWorld());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);

	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_pTextureCom->SetTexture(0);
	m_pRcTexCom->RenderBuffer();

	_vec2 vPos = {0.f , 20.f };
	_vec2 vPos2 = { WINCX , 100.f };
	
	Engine::RenderFontCenter(L"Font_Point", m_NarrationMap[m_strCurrentKey].c_str(), &vPos, &vPos2, D3DXCOLOR(0.878f, 0.807f, 0.6f, 1.f));

	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

}

void CNarration::ActiveUI(tstring _strKey)
{
	m_strCurrentKey = _strKey;
	m_bVisible = true;
	
	tstring SoundKey = m_strCurrentKey + L".wav";
	CSoundMgr::GetInstance()->StopSound(CHANNELID::NARRATION);
	CSoundMgr::GetInstance()->PlaySound(SoundKey.c_str(), CHANNELID::NARRATION, 0.5f);

	m_fActTime = 0.f;

	if (_strKey == L"Narr_Battle_Critical0" || _strKey == L"Narr_Battle_Critical1" ||
		_strKey == L"Narr_Battle_Critical2" || _strKey == L"Narr_Battle_Critical3") {
		m_fTotalTime = 3.f;
	}
	else {
		m_fTotalTime = 8.f;
	}

}

void CNarration::AddComponent()
{
	if (m_bReady) return;

	shared_ptr<CComponent> pComponent;

	CUIObj::AddComponent();

	pComponent = m_pTextureCom = make_shared<CTexture>(m_pGraphicDev);
	m_pTextureCom->SetTextureKey(L"UI_Narration_BackGround", TEX_NORMAL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture",pComponent });

}

void CNarration::SetNarrationMap()
{

	m_NarrationMap.insert({L"Narr_tut_firstdungeon", L"이 길은 산적들이 점령하고 있으니, 샛길을 통해 가자꾸나.\n 영지는 바로 저 앞에 있다."});
	m_NarrationMap.insert({L"Narr_tut_firstBattle", L"기습이다! 이 버러지들에게 똑똑히 전하라.\n이 땅의 적법한 상속자가 돌아왔으니, 너희 족속들은 이제 발붙일 곳조차 없을 것이라고!"});
	m_NarrationMap.insert({L"Narr_Darkest_Return1", L"넌 지금껏 공허의 끝자락만을 살짝 엿보았을 뿐이지,\n이제 너도, 나처럼 저들의 진실된 모습을 볼 수 있을 게다..."});
	m_NarrationMap.insert({L"Narr_Battle_Critical0", L"강력한 일격이로다!"});
	m_NarrationMap.insert({L"Narr_Battle_Critical1", L"대단하구나!"});
	m_NarrationMap.insert({L"Narr_Battle_Critical2", L"결정타로구나!"});
	m_NarrationMap.insert({L"Narr_Battle_Critical3", L"강력하구나!"});
	m_NarrationMap.insert({L"Narr_Tut_Curio", L"꼼꼼히 살펴보고 지나가거라.\n이 버려진 길에는 찾을만한 물건이 아주 많다."});
	m_NarrationMap.insert({L"Narr_EnterVillage", L"집에 돌아온 걸 환영한다. 이 황폐한 마을, 타락한 영지가 모두 네 것이다.\n 너도 이곳에 묶여버린 셈이지."});
	m_NarrationMap.insert({L"Narr_SecondDungeon", L"이 말종들을 잔혹하게 몰살해버려라.\n네가 돌아왔음을 모두에게 알려라!"});
	m_NarrationMap.insert({L"Narr_FirstLight", L"평안을 안겨주는 빛이구나!"});
	m_NarrationMap.insert({L"Narr_Trap_Fail", L"고대의 함정이 기다리고 있다가, 피를 갈구하며 튀어 오르는구나."});
	m_NarrationMap.insert({L"Narr_Visit_Guild", L"실수는 용납되지 않는다. 우린 지상 최대의 악과 맞서야 하느니라.\n병사들 역시 최고로 훈련되어야 할 게야."});

}
