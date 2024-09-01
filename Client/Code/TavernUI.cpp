#include"pch.h"
#include "TavernUI.h"
#include "Hero.h"
#include "GameObject.h"
#include "Player.h"
#include "UIMgr.h"

#include"Export_Utility.h"
#include"Export_System.h"

CTavernUI::CTavernUI(LPDIRECT3DDEVICE9 _pGraphicDev)
	:Engine::CUIObj(_pGraphicDev)
{
}

CTavernUI::~CTavernUI()
{
}

HRESULT CTavernUI::ReadyGameObject()
{
	m_vSize = { 170.f , 100.f ,0.f };
	m_vAngle = { 0.f, 0.f, 0.f };
	m_vPos = { 0.f, -180.f, 0.5f };

	m_bVisible = false;
	m_bEnable = true;
	m_bActive = true;

	m_pTransCom->SetScale(m_vSize.x, m_vSize.y, m_vSize.z);
	m_pTransCom->SetPosition(m_vPos.x, m_vPos.y, m_vPos.z);
	m_pTransCom->SetAngle({ 0.f, 0.f, 0.f });

	CUIObj::ReadyGameObject();

	return S_OK;
}

_int CTavernUI::UpdateGameObject(const _float& fTimeDelta)
{
	if (!m_bVisible)
		return 0;

	if (GetAsyncKeyState('X'))
		m_bVisible = false;

	int iExit(0);
	CGameObject::UpdateGameObject(fTimeDelta);
	AddRenderGroup(RENDER_UI, shared_from_this());

	m_fDeltaTime = fTimeDelta;

	if (m_bIconClick) {
		IconClickEvent();
	}

	return iExit;
}

void CTavernUI::LateUpdateGameObject()
{
}

void CTavernUI::RenderGameObject()
{

	if (!m_bVisible)
		return;

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransCom->GetWorld());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);

	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	m_pTextureCom->SetTextureKey(m_strBackKey, TEX_NORMAL);
	m_pTextureCom->SetTexture(0);
	m_pRcTexCom->RenderBuffer();

	TCHAR buf[64];

	//Hp
	_vec2 vPos = { m_vPos.x + WINCX * 0.5f + 20.f, (m_vPos.y * -1.f) + WINCY * 0.5f - 41.f };

	if (m_iHP < 10)
		_stprintf_s(buf, TEXT("    %d"), m_iHP);

	else if (m_iHP < 100)
		_stprintf_s(buf, TEXT("  %d"), m_iHP);

	Engine::Render_Font(L"Font_Point", buf, &vPos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

	vPos = { m_vPos.x + WINCX * 0.5f + 60.f, (m_vPos.y * -1.f) + WINCY * 0.5f - 41.f };
	_stprintf_s(buf, TEXT(" /"));
	Engine::Render_Font(L"Font_Point", buf, &vPos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

	vPos = { m_vPos.x + WINCX * 0.5f + 80.f, (m_vPos.y * -1.f) + WINCY * 0.5f - 41.f };
	_stprintf_s(buf, TEXT("%d"), m_iMaxHP);
	Engine::Render_Font(L"Font_Point", buf, &vPos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));


	//Stress

	vPos = { m_vPos.x + WINCX * 0.5f + 20.f, (m_vPos.y * -1.f) + WINCY * 0.5f - 7.f };

	//자릿수 띄워주기
	if (m_iStress < 10)
		_stprintf_s(buf, TEXT("    %d"), m_iStress);

	else if (m_iStress < 100)
		_stprintf_s(buf, TEXT("  %d"), m_iStress);

	Engine::Render_Font(L"Font_Point", buf, &vPos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

	vPos = { m_vPos.x + WINCX * 0.5f + 60.f, (m_vPos.y * -1.f) + WINCY * 0.5f - 7.f };
	_stprintf_s(buf, TEXT(" /"));
	Engine::Render_Font(L"Font_Point", buf, &vPos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

	vPos = { m_vPos.x + WINCX * 0.5f + 80.f, (m_vPos.y * -1.f) + WINCY * 0.5f - 7.f };
	_stprintf_s(buf, TEXT("%d"), 100);
	Engine::Render_Font(L"Font_Point", buf, &vPos, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

	// Gold
	vPos = { m_vPos.x + WINCX * 0.5f - 39.f, (m_vPos.y * -1.f) + WINCY * 0.5f + 59.f };
	_stprintf_s(buf, TEXT("%d"), 500);
	Engine::Render_Font(L"Font_Point", buf, &vPos, D3DXCOLOR(0.79f, 0.73f, 0.537f, 1.f));

	//if (m_iCursorPos != 99) {
	//    //Render Alpha

	//    m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	//    m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0xc0);
	//    m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	//    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pCheckTransform->GetWorld());
	//    m_pCheckTexture->SetTexture(0);
	//    m_pCheckRcTex->RenderBuffer();

	//    m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	//}

	m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

}

void CTavernUI::SetHero(shared_ptr<CHero> _pHero)
{
	m_pHero = _pHero;

	switch (m_pHero.lock()->GetHeroType())
	{
	case EHeroType::SHILEDBREAKER:
		m_strBackKey = L"UI_HeroTavern_ShieldBreaker";
		break;

	case EHeroType::HIGHWAYMAN:
		m_strBackKey = L"UI_HeroTavern_Highwayman";
		break;

	case EHeroType::JESTER:
		m_strBackKey = L"UI_HeroTavern_Jester";
		break;

	case EHeroType::VESTAL:
		m_strBackKey = L"UI_HeroTavern_Vestal";
		break;

	case EHeroType::ENUM_END:
		m_strBackKey = L"UI_HeroTavern_Base";
		break;

	default:
		m_strBackKey = L"UI_HeroTavern_Base";
		break;
	}

	m_iMaxHP = m_pHero.lock()->GetCommonStat().iMaxHp;
	m_iHP = m_pHero.lock()->GetCommonStat().iHp;
	m_iStress = m_pHero.lock()->GetStress();
	m_iGold = 500;

	// m_iCursorPos = 99;
}

void CTavernUI::PickingUI(LONG _fX, LONG _fY, _bool _bLB)
{
	//Y축검사
	if (_fY > 590 || _fY < 564.f)
		return;

	if (661.f < _fX && 731.f >= _fX) {
		// 구매 버튼 트리거
		//m_iCursorPos = 0;
		//m_pCheckTransform->SetPosition(m_vPos.x - 45.f, m_vPos.y, m_vPos.z);

		// 풀피거나 스트레스가 0이면 리턴
		/*if (m_pHero.lock()->GetHp() == m_pHero.lock()->GetMaxHp() || m_pHero.lock()->GetStress() <= 0)
			return;*/

		// 돈 부족하면 리턴
		if (dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->GetGold() < 500)
		{
			CSoundMgr::GetInstance()->StopSound(CHANNELID::UI);
			CSoundMgr::GetInstance()->PlaySound(L"ui_town_buy_fail.wav", CHANNELID::UI, 1.f);

			return;
		}

		dynamic_pointer_cast<CPlayer>(CGameMgr::GetInstance()->GetPlayer())->SetGold(500, false);

		CSoundMgr::GetInstance()->StopSound(CHANNELID::EFFECT);
		CSoundMgr::GetInstance()->PlaySound(L"town_tavern_bar.wav", CHANNELID::EFFECT, 1.f);

		CSoundMgr::GetInstance()->StopSound(CHANNELID::UI);
		CSoundMgr::GetInstance()->PlaySound(L"ui_select.wav", CHANNELID::UI, 1.f);

		// 체력이랑 스트레스 회복하기
		m_pHero.lock()->IncreaseHP(20);
		m_pHero.lock()->DecreaseStress(50);
		if (m_pHero.lock()->GetHp() > m_pHero.lock()->GetMaxHp()) m_pHero.lock()->SetHp(m_pHero.lock()->GetMaxHp());
		if (m_pHero.lock()->GetStress() < 0) m_pHero.lock()->SetStress(0);

		m_iHP = m_pHero.lock()->GetHp();
		m_iStress = m_pHero.lock()->GetStress();
	}
}

void CTavernUI::HoverUI(LONG _fX, LONG _fY)
{
	//Y축검사
	if (_fY > 590 || _fY < 564.f)
		return;

	if (661.f < _fX && 731.f >= _fX) {

		CUIMgr::GetInstance()->SetDescription(L"휴식하기", { (_float)_fX - WINCX / 2.f, -(_float)_fY + WINCY / 2.f, 0.f });
	}
}

void CTavernUI::AddComponent()
{
	shared_ptr<CComponent> pComponent;

	CUIObj::AddComponent();

	pComponent = m_pCheckTexture = make_shared<CTexture>(m_pGraphicDev);
	m_pCheckTexture->SetTextureKey(L"UI_Check", TEX_NORMAL);
	m_mapComponent[ID_STATIC].insert({ L"Com_CheckTexture", pComponent });

	pComponent = m_pTextureCom = make_shared<CTexture>(m_pGraphicDev);
	m_pTextureCom->SetTextureKey(L"UI_HeroTavern_Base", TEX_NORMAL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });
}

void CTavernUI::SetIconPos()
{
}

void CTavernUI::IconClickEvent()
{
}
