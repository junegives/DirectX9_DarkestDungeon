#include"pch.h"
#include"Export_System.h"
#include "PuzzleHint.h"
#include"Export_Utility.h"

#include"UIMgr.h"


CPuzzleHint::CPuzzleHint(LPDIRECT3DDEVICE9 _pGraphicDev)
	:CUIObj(_pGraphicDev)
{
}

CPuzzleHint::~CPuzzleHint()
{
}

HRESULT CPuzzleHint::ReadyGameObject()
{

	m_vSize = { 250.f, 122.f, 0.f };
	m_vPos = { 0.f , 0.f, 0.f };
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

_int CPuzzleHint::UpdateGameObject(const _float& fTimeDelta)
{
	_int iExit = CGameObject::UpdateGameObject(fTimeDelta);
	AddRenderGroup(RENDER_UI, shared_from_this());

	return iExit;
}

void CPuzzleHint::LateUpdateGameObject()
{
	if (m_bVisible)
		CUIMgr::GetInstance()->SelectUIVisibleOff(L"UI_InteractionInfo");
		
}

void CPuzzleHint::RenderGameObject()
{

	if (!m_bVisible) {
		return;
	}


	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransCom->GetWorld());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);

	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0xc0);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pTextureCom->SetTexture(0);
	m_pRcTexCom->RenderBuffer();

	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

}

void CPuzzleHint::AddComponent()
{
	shared_ptr<CComponent> pComponent;

	CUIObj::AddComponent();

	pComponent = m_pTextureCom = make_shared<CTexture>(m_pGraphicDev);
	m_pTextureCom->SetTextureKey(L"UI_Puzzle_Hint", TEX_NORMAL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture",pComponent });

}
