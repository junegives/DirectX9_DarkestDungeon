#include"pch.h"
#include "InteractionInfo.h"

#include"Export_Utility.h"
#include"Export_System.h"

CInteractionInfo::CInteractionInfo(LPDIRECT3DDEVICE9 _pGraphicDev)
	:CUIObj(_pGraphicDev)
{
}

CInteractionInfo::~CInteractionInfo()
{
}

HRESULT CInteractionInfo::ReadyGameObject()
{
	if (m_bReady) return S_OK;


	m_vSize = { 173.f, 35.f, 0.f };
	m_vPos = { 300.f , -100.f, 0.f };
	m_vAngle = { 0.f ,0.f, 0.f };

	m_bVisible = false;
	m_bEnable = true;
	m_bActive = true;



	m_pTransCom->SetScale(m_vSize.x, m_vSize.y, m_vSize.z);
	m_pTransCom->SetPosition(m_vPos.x, m_vPos.y, m_vPos.z);
	m_pTransCom->SetAngle({ 0.f, 0.f, 0.f });

	CUIObj::ReadyGameObject();

	return S_OK;
}

_int CInteractionInfo::UpdateGameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::UpdateGameObject(fTimeDelta);
	AddRenderGroup(RENDER_UI, shared_from_this());

	return iExit;
}

void CInteractionInfo::LateUpdateGameObject()
{
	if (!m_bCheckObj)
		m_bVisible = false;

	m_bCheckObj = false;
}

void CInteractionInfo::RenderGameObject()
{
	if (!m_bVisible || m_bIsBattle) {
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

	_vec2 vPos = { 365.f , 285.f };
	_vec2 vPos2 = { 900.f , 355.f };

	Engine::RenderFontCenter(L"Font_Point_Title", m_strMainKey.c_str(), &vPos, &vPos2, YELLOW);

	_vec2 vPos3 = { 300.f , 285.f };
	_vec2 vPos4 = { WINCX , 355.f };

	Engine::RenderFontCenter(L"Font_Default_Light", m_strInfo.c_str(), &vPos3, &vPos4, WHITE);

	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

}

void CInteractionInfo::AddComponent()
{
	if (m_bReady) return;

	shared_ptr<CComponent> pComponent;

	CUIObj::AddComponent();

	pComponent = m_pTextureCom = make_shared<CTexture>(m_pGraphicDev);
	m_pTextureCom->SetTextureKey(L"UI_Interaction", TEX_NORMAL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture",pComponent });

}
